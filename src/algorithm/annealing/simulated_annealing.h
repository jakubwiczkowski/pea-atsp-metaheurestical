

#ifndef PEA_ATSP_OPT_SIMULATED_ANNEALING_H
#define PEA_ATSP_OPT_SIMULATED_ANNEALING_H


#include <functional>
#include "../algorithm.h"

/**
 * Klasa implementująca algorytm Symulowanego Wyżarzania
 */
class simulated_annealing: public algorithm {
    /**
     * Funckja temperatury
     */
    std::function<double(double)> temp_function;
    /**
     * Funkcja generująca losowego sąsiada
     */
    std::function<std::vector<vertex_t>(std::vector<vertex_t>&)> neighbour_function;

public:
    explicit simulated_annealing(std::function<double(double)> temp_function,
                                 std::function<std::vector<vertex_t>(std::vector<vertex_t>&)> neighbour_function);

    solution solve(graph &graph, int time_limit) override;
};

/**
 * Generacja losowego sąsiada metodą swap
 */
static auto swap_a = [](const std::vector<vertex_t>& current) {
    static std::random_device r;
    static std::default_random_engine e1(r());
    std::vector<vertex_t> neighbour(current);
    neighbour.pop_back();

    std::uniform_int_distribution<long> random_vertex(0, neighbour.size() - 1);

    long first_index = random_vertex(e1);
    long second_index = random_vertex(e1);

    vertex_t first = neighbour[first_index];
    vertex_t second = neighbour[second_index];

    // muszą być dwa różne
    while (first == second) {
        second_index = random_vertex(e1);
        second = neighbour[second_index];
    }

    std::iter_swap(neighbour.begin() + first_index, neighbour.begin() + second_index);
    neighbour.emplace_back(neighbour[0]);

    return neighbour;
};


#endif //PEA_ATSP_OPT_SIMULATED_ANNEALING_H
