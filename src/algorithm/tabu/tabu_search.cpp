

#include <map>
#include <chrono>
#include <utility>
#include "tabu_search.h"

solution tabu_search::solve(graph &graph, int time_limit) {
//     dla małych problemów
//    static int term_time = 15;
//    static int critical_iterations = 18;

    // dla średnich problemów
//    static int term_time = 30;
//    static int critical_iterations = 35;

    solution best_solution;

    std::map<std::vector<vertex_t>, int> terms;

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<vertex_t> current_best = random_permutation(graph);
    std::vector<vertex_t> optimal(current_best);
    best_solution.weight = graph.get_path_weight(optimal);

    int iterations_without_improvement = 0;

    while (true) {
        std::vector<vertex_t> best_neighbour = this->neighbour_function(current_best,
                                                                        graph,
                                                                        meval,
                                                                        terms,
                                                                        best_solution.weight);

//        if (graph.get_path_weight(best_neighbour) > graph.get_path_weight(current_best)) {
//            std::cout << "worse neighbour" << std::endl;
//        }

//        if (best_neighbour == current_best) {
//            std::cout << "same path" << std::endl;
//        }

        int neighbour_weight = graph.get_path_weight(best_neighbour);
        int current_weight = graph.get_path_weight(current_best);

        current_best = best_neighbour;

//        std::cout << neighbour_weight << std::endl;

        terms.emplace(current_best, term_time);

        if (neighbour_weight < best_solution.weight) {
            optimal = current_best;
            best_solution.weight = neighbour_weight;
            auto found_after = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - start_time)
                    .count();
            best_solution.found_after = found_after;
//            std::cout << "   LEPSZE ROZWIAZANIE: " << best_solution.weight << std::endl;

            iterations_without_improvement = 0;
        } else if (neighbour_weight >= current_weight) {
            iterations_without_improvement++;
//            std::cout << "" << std::endl;
//            std::cout << neighbour_weight << " >= " << current_weight << std::endl;
//            std::cout << "BEZ POPRAWY: " << iterations_without_improvement << std::endl;
//            std::cout << "" << std::endl;
        }
//        else if (diff > critical_weight_diff) {
//            iterations_without_improvement++;
//        }

        std::vector<std::vector<vertex_t>> cycles_to_erase;

        for (auto& [cycle, term]: terms) {
            terms[cycle]--;

            if (term - 1 <= 0) cycles_to_erase.emplace_back(cycle);
        }

        for (auto& cycle: cycles_to_erase) {
            terms.erase(cycle);
        }

        if (iterations_without_improvement >= critical_iterations) {
            current_best = random_permutation(graph);
            int weight = graph.get_path_weight(current_best);

            if (weight < best_solution.weight) {
                best_solution.weight = weight;
                optimal = current_best;
                auto found_after = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now() - start_time)
                        .count();
                best_solution.found_after = found_after;
            }

            iterations_without_improvement = 0;
//            std::cout << "RESET" << std::endl;
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
