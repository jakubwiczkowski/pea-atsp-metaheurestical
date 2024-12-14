#ifndef PEA_ATSP_ALGORITHM_H
#define PEA_ATSP_ALGORITHM_H


#include "../graph/graph.h"
#include "solution.h"

/**
 * Klasa reprezentująca algorytm służący
 * do rozwiązania Asymetrycznego Problemu Komiwojażera
 */
class algorithm {
public:
    /**
     * Funkcja rozwiązująca problem Komiwojażera
     * na podanym grafie
     *
     * @param graph graf, w którym ma być znaleziony najkrótszy cykl Hamiltona
     * @param time_limit limit czasu wykonania algorytmu (w sekundach)
     * @return rozwiązanie problemu Komiwojażera
     */
    virtual solution solve(graph& graph, int time_limit) = 0;

    std::vector<vertex_t> random_permutation(graph& graph);
};


#endif //PEA_ATSP_ALGORITHM_H
