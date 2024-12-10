#ifndef PEA_ATSP_SOLUTION_H
#define PEA_ATSP_SOLUTION_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <limits>

#include "../type/types.h"

/**
 * Struktura reprezentująca rozwiązanie
 * problemu Komiwojażera
 */
struct solution {
    /**
     * Cykl Hamiltona
     */
    std::vector<vertex_t> vertices;
    /**
     * Koszt cyklu
     */
    int weight = std::numeric_limits<int>::max();
};

/**
 * Wyświetla podane przybliżone rozwiązanie problemu Komiwojażera
 *
 * @param solution rozwiązanie
 */
static void print_solution(solution& solution) {
    std::cout << "[>] Przybliżony optymalny cykl Hamiltona: " << solution.weight << std::endl;
    std::cout << "[>] Scieżka: ";
    for (auto vertex : solution.vertices) {
        std::cout << vertex << " ";
    }
    std::cout << std::endl;
}

#endif //PEA_ATSP_SOLUTION_H
