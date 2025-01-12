

#include <map>
#include <chrono>
#include <random>
#include "tabu_search.h"
#include "../greedy/nearest_neighbour.h"

solution tabu_search::solve(graph &graph, int time_limit) {
    static nearest_neighbour initial_sol_gen;

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> random_int(term_time * 0.5, term_time * 1.5);

    solution best_solution;

    tabu_map_t terms;

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<vertex_t> current_best = initial_sol_gen.solve(graph, -1).vertices;
    std::vector<vertex_t> optimal(current_best);
    best_solution.weight = graph.get_path_weight(optimal);
    best_solution.found_after = 0;
    best_solution.relative_error_values.emplace_back(0, best_solution.weight);

    int iterations_without_improvement = 0;

    while (true) {
        std::pair<std::pair<vertex_t, vertex_t>, std::vector<vertex_t>> best_neighbour = this->neighbour_function(current_best,
                                                                        graph,
                                                                        meval,
                                                                        terms,
                                                                        best_solution.weight);

        int neighbour_weight = graph.get_path_weight(best_neighbour.second);
        int current_weight = graph.get_path_weight(current_best);

        current_best = best_neighbour.second;

        terms.emplace(best_neighbour.first, random_int(e1));

        if (neighbour_weight < best_solution.weight) {
            optimal = current_best;
            best_solution.weight = neighbour_weight;
            auto found_after = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - start_time)
                    .count();
            best_solution.found_after = found_after;
            best_solution.relative_error_values.emplace_back(found_after, neighbour_weight);

            iterations_without_improvement = 0;
        } else if (neighbour_weight >= current_weight) {
            iterations_without_improvement++;
        }
        else if (neighbour_weight < current_weight) {
            iterations_without_improvement = 0;
        }

        std::vector<std::pair<vertex_t, vertex_t>> cycles_to_erase;

        for (auto& [cycle, term]: terms) {
            terms[cycle]--;

            if (term - 1 <= 0) cycles_to_erase.emplace_back(cycle);
        }

        for (auto& cycle: cycles_to_erase) {
            terms.erase(cycle);
        }

        if (iterations_without_improvement >= critical_iterations) {
//            current_best = random_permutation(graph);
            current_best = initial_sol_gen.solve(graph, -1).vertices;
            int weight = graph.get_path_weight(current_best);

            if (weight < best_solution.weight) {
                best_solution.weight = weight;
                optimal = current_best;
                auto found_after = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now() - start_time)
                        .count();
                best_solution.found_after = found_after;
                best_solution.relative_error_values.emplace_back(found_after, weight);
            }

            iterations_without_improvement = 0;
        }

        auto current_runtime = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - start_time)
                .count();

        if (current_runtime >= time_limit) break;
    }

    best_solution.weight = graph.get_path_weight(optimal);
    best_solution.vertices = optimal;

    return best_solution;
}

int tabu_search::meval(std::vector<vertex_t> &x_a, std::vector<vertex_t> &x_k, graph &graph) {
    return graph.get_path_weight(x_a) - graph.get_path_weight(x_k);
}

tabu_search::tabu_search(
        neighbour_t neighbour_function,
        int term_time,
        int critical_iterations) {
    this->neighbour_function = neighbour_function;
    this->term_time = term_time;
    this->critical_iterations = critical_iterations;
}
