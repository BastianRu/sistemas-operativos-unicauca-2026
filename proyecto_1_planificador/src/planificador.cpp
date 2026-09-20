/**
 * @file
 * @brief Simulacion de la planificacion de procesos con colas de prioridad.
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
 */

#include "planificador.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <limits>

namespace {

/**
 * @brief Pasa una cadena a minusculas.
 * @param s Cadena a convertir
 * @return La cadena con todas sus letras en minuscula
 */
std::string en_minusculas(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

/**
 * @brief Inserta un proceso en una cola ordenada de forma ascendente por tiempo
 * restante. En caso de empate queda detras de los que ya estaban, de modo que el
 * desempate lo decide el orden de llegada.
 *
 * @param cola Cola de listos, ordenada por tiempo restante
 * @param p Proceso a insertar
 */
[[maybe_unused]] void insertar_por_restante(std::deque<Proceso *> &cola, Proceso *p) {
    /**
     * \todo Insertar el proceso en la cola de modo que quede ordenada de menor
     * a mayor tiempo restante. Cuando dos procesos empatan, el que ya estaba
     * conserva su lugar, de modo que el desempate lo decide el orden de llegada.
     *
     * Mientras no este implementado, el proceso entra al final y SJF y SRT se
     * comportan como FIFO.
     */
    cola.push_back(p);
}

/**
 * @brief Suma tiempo de espera a todo proceso listo distinto del que usa la CPU.
 * @param procesos Todos los procesos de la simulacion
 * @param actual Proceso que tiene la CPU, que no acumula espera
 * @param ahora Instante en que comienza el intervalo
 * @param tiempo Duracion del intervalo
 */
void sumar_espera(std::vector<Proceso> &procesos, const Proceso *actual, int ahora, int tiempo) {
    for (Proceso &p : procesos) {
        if (&p != actual && p.estado == Estado::listo) {
            p.espera += tiempo;
            p.quantums.push_back({Tipo_quantum::espera, ahora, ahora + tiempo});
        }
    }
}

/**
 * @brief Registra el uso de CPU de un proceso y descuenta su tiempo restante.
 * @param p Proceso que recibe la CPU
 * @param ahora Instante en que comienza el intervalo
 * @param tiempo Unidades de tiempo concedidas
 */
void sumar_cpu(Proceso *p, int ahora, int tiempo) {
    p->cpu += tiempo;
    p->restante -= tiempo;
    p->quantums.push_back({Tipo_quantum::cpu, ahora, ahora + tiempo});
}

}  // namespace

bool estrategia_desde_texto(const std::string &texto, Estrategia &destino) {
    const std::string t = en_minusculas(texto);
    if (t == "fifo") { destino = Estrategia::FIFO; return true; }
    if (t == "sjf")  { destino = Estrategia::SJF;  return true; }
    if (t == "rr")   { destino = Estrategia::RR;   return true; }
    if (t == "srt")  { destino = Estrategia::SRT;  return true; }
    return false;
}

std::string texto_de_estrategia(Estrategia estrategia) {
    switch (estrategia) {
        case Estrategia::FIFO: return "FIFO";
        case Estrategia::SJF:  return "SJF";
        case Estrategia::RR:   return "RR";
        case Estrategia::SRT:  return "SRT";
    }
    return "?";
}

void preparar(std::vector<Proceso> &procesos, std::vector<Cola_prioridad> &colas) {
    for (Cola_prioridad &c : colas) {
        c.llegada.clear();
        c.listos.clear();
        c.terminados.clear();
    }
    for (Proceso &p : procesos) {
        p.reiniciar();
        colas[p.prioridad].llegada.push_back(&p);
    }
    for (Cola_prioridad &c : colas) {
        std::stable_sort(c.llegada.begin(), c.llegada.end(),
                         [](const Proceso *a, const Proceso *b) { return a->llegada < b->llegada; });
    }
}

int proxima_llegada(const std::vector<Cola_prioridad> &colas) {
    int menor = std::numeric_limits<int>::max();
    for (const Cola_prioridad &c : colas) {
        if (!c.llegada.empty()) {
            menor = std::min(menor, c.llegada.front()->llegada);
        }
    }
    return (menor == std::numeric_limits<int>::max()) ? -1 : menor;
}

int procesar_llegadas(int ahora, std::vector<Cola_prioridad> &colas) {
    int total = 0;
    for (Cola_prioridad &c : colas) {
        while (!c.llegada.empty() && c.llegada.front()->llegada <= ahora) {
            Proceso *p = c.llegada.front();
            c.llegada.pop_front();

            p->estado = Estado::listo;
            p->espera = ahora - p->llegada;
            if (p->espera > 0) {
                p->quantums.push_back({Tipo_quantum::espera, p->llegada, ahora});
            }
            /**
             * \todo En las colas SJF y SRT el proceso que llega se inserta
             * segun su rafaga, no al final. Usar insertar_por_restante().
             */
            c.listos.push_back(p);
            total++;
        }
    }
    return total;
}

Resultado planificar(std::vector<Proceso> &procesos, std::vector<Cola_prioridad> &colas, bool traza) {
    Resultado resultado;
    const int ncolas = static_cast<int>(colas.size());

    preparar(procesos, colas);

    int por_terminar = static_cast<int>(procesos.size());
    if (por_terminar == 0 || ncolas == 0) {
        return resultado;
    }

    int ahora = proxima_llegada(colas);
    if (ahora < 0) {
        return resultado;
    }
    procesar_llegadas(ahora, colas);

    // La simulacion arranca en la cola de mayor prioridad que tenga listos
    int pos = 0;
    while (pos < ncolas && colas[pos].listos.empty()) {
        pos++;
    }
    if (pos == ncolas) {
        return resultado;
    }
    int quantum = colas[pos].quantum;

    while (por_terminar > 0) {
        Cola_prioridad &cola = colas[pos];
        Proceso *actual = cola.listos.front();
        cola.listos.pop_front();
        actual->estado = Estado::ejecucion;

        // De entrada se le concede el quantum, o lo que le falte si es menos
        int quantum_asignado = std::min(quantum, actual->restante);
        bool cambiar_de_cola = true;

        /**
         * \todo Implementar la expropiacion de SRT.
         *
         * Si la cola se planifica con SRT y durante este quantum llega un
         * proceso cuya rafaga es menor que lo que le resta al que se ejecuta,
         * el proceso actual solo recibe CPU hasta ese instante. En ese caso no
         * se pasa a la siguiente cola, y al quantum se le descuenta el tiempo
         * ya consumido.
         *
         * Mientras no este implementado, SRT no expropia y se comporta como RR.
         */

        if (traza) {
            std::cerr << "[" << ahora << "] cola " << (pos + 1) << " -> " << actual->nombre
                      << " durante " << quantum_asignado << "\n";
        }

        sumar_cpu(actual, ahora, quantum_asignado);
        sumar_espera(procesos, actual, ahora, quantum_asignado);
        if (quantum_asignado > 0) {
            resultado.secuencia.push_back({actual->nombre, quantum_asignado});
        }

        // Las llegadas del intervalo se atienden antes de reencolar al actual
        procesar_llegadas(ahora + quantum_asignado, colas);

        if (actual->restante == 0) {
            actual->finalizacion = ahora + quantum_asignado;
            actual->estado = Estado::terminado;
            cola.terminados.push_back(actual);
            cambiar_de_cola = true;
            por_terminar--;
        } else {
            actual->estado = Estado::listo;
            switch (cola.estrategia) {
                case Estrategia::FIFO:
                    // Sin expropiacion: recupera la CPU antes que los demas.
                    // Este es el caso resuelto que sirve de ejemplo.
                    cola.listos.push_front(actual);
                    break;
                case Estrategia::RR:
                    /**
                     * \todo El proceso que agota su quantum vuelve al final de
                     * la cola de listos.
                     */
                    cola.listos.push_front(actual);
                    break;
                case Estrategia::SJF:
                    /**
                     * \todo SJF tampoco expropia, de modo que el proceso
                     * conserva la CPU hasta terminar.
                     */
                    cola.listos.push_front(actual);
                    break;
                case Estrategia::SRT:
                    /**
                     * \todo En SRT el proceso vuelve a la cola ordenado por su
                     * tiempo restante.
                     */
                    cola.listos.push_front(actual);
                    break;
            }
        }

        ahora += quantum_asignado;
        if (por_terminar == 0) {
            break;
        }

        if (cambiar_de_cola) {
            int intentos = 0;
            do {
                pos = (pos + 1) % ncolas;
                intentos++;
            } while (intentos < ncolas && colas[pos].listos.empty());
        }

        // Sin nadie listo, el tiempo salta hasta la proxima llegada
        if (colas[pos].listos.empty()) {
            const int siguiente = proxima_llegada(colas);
            if (siguiente < 0) {
                break;
            }
            ahora = std::max(ahora, siguiente);
            procesar_llegadas(ahora, colas);

            int intentos = 0;
            while (intentos < ncolas && colas[pos].listos.empty()) {
                pos = (pos + 1) % ncolas;
                intentos++;
            }
        }

        if (colas[pos].listos.empty()) {
            std::cerr << "No hay ninguna cola con procesos listos; la simulacion se detiene.\n";
            break;
        }

        if (cambiar_de_cola) {
            quantum = colas[pos].quantum;
        }
    }

    for (const Proceso &p : procesos) {
        resultado.espera_total += p.espera;
        resultado.tiempo_total = std::max(resultado.tiempo_total, p.finalizacion);
    }
    resultado.espera_promedio =
        static_cast<double>(resultado.espera_total) / static_cast<double>(procesos.size());
    return resultado;
}

void imprimir_resultados(const std::vector<Proceso> &procesos, const Resultado &resultado) {
    std::printf("\nResultados de la simulacion\n");
    std::printf("Tiempo total de la simulacion: %d unidades de tiempo\n", resultado.tiempo_total);
    std::printf("Tiempo promedio de espera: %.3f unidades de tiempo\n\n", resultado.espera_promedio);

    std::printf("%5s%20s%8s%8s%10s%10s%10s\n", "#", "Proceso", "Prior.", "Lleg.", "Rafaga",
                "Espera", "Fin");
    int i = 1;
    for (const Proceso &p : procesos) {
        std::printf("%5d%20s%8d%8d%10d%10d%10d\n", i++, p.nombre.c_str(), p.prioridad + 1, p.llegada,
                    p.ejecucion, p.espera, p.finalizacion);
    }

    std::printf("\nTiempo total de espera: %d   Tiempo promedio de espera: %.3f\n",
                resultado.espera_total, resultado.espera_promedio);

    std::printf("\nSecuencia de ejecucion\n");
    for (const Tramo_secuencia &t : resultado.secuencia) {
        std::printf("%s (%d) ", t.nombre.c_str(), t.tiempo);
    }
    std::printf("\n");
}
