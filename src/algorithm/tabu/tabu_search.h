

#ifndef PEA_ATSP_OPT_TABU_SEARCH_H
#define PEA_ATSP_OPT_TABU_SEARCH_H


#include <functional>
#include "../algorithm.h"

/**
 * Klasa implementująca algorytm Tabu Search
 */
class tabu_search : public algorithm {
private:
    int term_time;
    int critical_iterations;
public:
    /**
     * Funkcja ewaluacyjna pomiędzy dwoma cyklami
     *
     * @param x_a obecny cykl
     * @param x_k cykl w sąsiedztwie
     * @param graph graf
     * @return różnica pomiędzy kosztem x_a a x_k
     */
    static int meval(std::vector<vertex_t> &x_a, std::vector<vertex_t> &x_k, graph &graph);

    /**
     * Typ listy tabo
     */
    typedef std::map<std::pair<vertex_t, vertex_t>, int> tabu_map_t;

    /**
     * Typ funkcji ewaluującej
     */
    typedef std::function<int(std::vector<vertex_t> & x_a, std::vector<vertex_t> & x_k, graph & graph)> eval_func;
    /**
     * Typ funkcji generującej najlepszego sąsiada
     */
    typedef std::function<std::pair<std::pair<vertex_t, vertex_t>, std::vector<vertex_t>>(std::vector<vertex_t> &,
                                                graph&,
                                                eval_func,
                                                tabu_map_t &,
                                                int aspire_to)> neighbour_t;

    neighbour_t neighbour_function;

    explicit tabu_search(neighbour_t neighbour_function,
                         int term_time,
                         int critical_iterations);

    solution solve(graph &graph, int time_limit) override;
};

/**
 * Generacja sąsiada metodą swap
 */
static tabu_search::neighbour_t swap = [](std::vector<vertex_t>& current,
                                   graph& graph,
                                   tabu_search::eval_func meval,
                                   tabu_search::tabu_map_t& tabu_map,
                                   int aspire_to) -> std::pair<std::pair<vertex_t, vertex_t>, std::vector<vertex_t>> {
    std::vector<vertex_t> without_cycle(current);
    without_cycle.pop_back();

    std::vector<vertex_t> best_neighbour;
    std::pair<vertex_t, vertex_t> best_swapped;
    int best_cost = std::numeric_limits<int>::min();

    for (long i = 0; i < without_cycle.size(); ++i) {
        for (long j = 0; j < without_cycle.size(); ++j) {
            if (i == j) continue;

            std::vector<vertex_t> iteration(without_cycle);
            std::iter_swap(iteration.begin() + i, iteration.begin() + j);
            iteration.emplace_back(iteration[0]);
            vertex_t i_vertex = iteration[i];
            vertex_t j_vertex = iteration[j];

            if (graph.get_path_weight(iteration) < aspire_to) {
                best_neighbour = iteration;
                best_swapped = {std::min(i_vertex, j_vertex), std::max(i_vertex, j_vertex)};
                best_cost = meval(current, iteration, graph);
            } else {
                if (tabu_map.find({std::min(i_vertex, j_vertex), std::max(i_vertex, j_vertex)}) != tabu_map.end()) continue;

                int eval = meval(current, iteration, graph);
                if (eval < best_cost) continue;

                best_neighbour = iteration;
                best_swapped = {std::min(i_vertex, j_vertex), std::max(i_vertex, j_vertex)};
                best_cost = eval;
            }
        }
    }

    return {best_swapped, best_neighbour};
};

/**
 * Generacja sąsiada metodą inversion
 */
static tabu_search::neighbour_t inversion2 = [](std::vector<vertex_t>& current,
                                         graph& graph,
                                         tabu_search::eval_func meval,
                                         tabu_search::tabu_map_t& tabu_map,
                                         int aspire_to) -> std::pair<std::pair<vertex_t, vertex_t>, std::vector<vertex_t>> {
    static const size_t insertion_size = 4;

    std::vector<vertex_t> without_cycle(current);
    without_cycle.pop_back();

    std::vector<vertex_t> best_neighbour;
    int best_cost = std::numeric_limits<int>::min();

    for (int i = 0; i < without_cycle.size(); ++i) {
        std::vector<vertex_t> iteration(without_cycle);
        if (i <= iteration.size() - 4) {
            std::iter_swap(iteration.begin() + i, iteration.begin() + i + 3);
            std::iter_swap(iteration.begin() + i + 1, iteration.begin() + i + 2);
        } else if (i == iteration.size() - 3) {
            std::iter_swap(iteration.begin() + i, iteration.begin() + i + 3 - iteration.size());
            std::iter_swap(iteration.begin() + i + 1, iteration.begin() + i + 2);
        } else if (i == iteration.size() - 2) {
            std::iter_swap(iteration.begin() + i, iteration.begin() + i + 3 - iteration.size());
            std::iter_swap(iteration.begin() + i + 1, iteration.begin() + i + 2 - iteration.size());
        } else if (i == iteration.size() - 1) {
            std::iter_swap(iteration.begin() + i, iteration.begin() + i + 3 - iteration.size());
            std::iter_swap(iteration.begin() + i + 1 - iteration.size(), iteration.begin() + i + 2 - iteration.size());
        }
        iteration.emplace_back(iteration[0]);

        if (graph.get_path_weight(iteration) < aspire_to) {
            best_neighbour = iteration;
            best_cost = meval(current, iteration, graph);
        } else {
//            if (tabu_map.find({std::min()}) != tabu_map.end()) continue;

            int eval = meval(current, iteration, graph);
            if (eval < best_cost) continue;

            best_neighbour = iteration;
            best_cost = eval;
        }
    }

    return {{1, 2}, best_neighbour};
};

/**
 * Generacja sąsiada metodą insertion
 */
static tabu_search::neighbour_t insertion = [](std::vector<vertex_t>& current,
                                               graph& graph,
                                               tabu_search::eval_func meval,
                                               tabu_search::tabu_map_t& tabu_map,
                                               int aspire_to) -> std::pair<std::pair<vertex_t, vertex_t>, std::vector<vertex_t>> {
    static const size_t insertion_size = 4;

    std::vector<vertex_t> without_cycle(current);
    without_cycle.pop_back();

    std::vector<vertex_t> best_neighbour;
    int best_cost = std::numeric_limits<int>::min();

    for (int i = 0; i < without_cycle.size() - insertion_size; ++i) {
        std::vector<vertex_t> iteration(without_cycle);
        std::rotate(iteration.begin() + i, iteration.begin() + i + insertion_size, iteration.begin() + i + insertion_size + 1);
        iteration.emplace_back(iteration[0]);

        if (graph.get_path_weight(iteration) < aspire_to) {
            best_neighbour = iteration;
            best_cost = meval(current, iteration, graph);
        } else {
            int eval = meval(current, iteration, graph);

//            if (tabu_map.find(iteration) != tabu_map.end()) continue;
            if (eval < best_cost) continue;

            best_neighbour = iteration;
            best_cost = eval;
        }
    }

    return {{1, 2}, best_neighbour};
};


#endif //PEA_ATSP_OPT_TABU_SEARCH_H
