

#include <chrono>
#include <random>
#include <utility>
#include "simulated_annealing.h"
#include "../greedy/nearest_neighbour.h"

simulated_annealing::simulated_annealing(std::function<double(double)> temp_function,
                                         std::function<std::vector<vertex_t>(
                                                 std::vector<vertex_t>&)> neighbour_function) {
    this->temp_function = std::move(temp_function);
    this->neighbour_function = std::move(neighbour_function);
}

solution simulated_annealing::solve(graph &graph, int time_limit) {
    static nearest_neighbour initial_sol_gen;

    static int init_temp_iterations = 10000;
    static double initial_increase_acceptance = 0.999;

    static std::random_device r;
    static std::default_random_engine e1(r());
    static std::uniform_real_distribution<double> random_double(0, 1);

    solution best_solution;

    std::vector<vertex_t> current_path = initial_sol_gen.solve(graph, -1).vertices;
    std::vector<vertex_t> optimal(current_path);

    best_solution.vertices = current_path;
    best_solution.weight = graph.get_path_weight(current_path);
    best_solution.found_after = 0;
    best_solution.relative_error_values.emplace_back(0, best_solution.weight);

    int L_k = ((graph.get_vertices() * (graph.get_vertices() - 1)) / 2);

    auto start_time = std::chrono::high_resolution_clock::now();

    double sum = 0;
    for (int i = 0; i < init_temp_iterations; ++i) {
        std::vector<vertex_t> neighbour = this->neighbour_function(current_path);
        sum += std::abs(graph.get_path_weight(neighbour) - best_solution.weight);
    }
    sum /= init_temp_iterations;

    double temperature = -(sum / std::log(initial_increase_acceptance)) * 2;

    int k = 0;
    while (true) {
        for (int L = 0; L < L_k; ++L) {
            int current_cost = graph.get_path_weight(current_path);

            std::vector<vertex_t> neighbour = this->neighbour_function(current_path);
            int neighbour_cost = graph.get_path_weight(neighbour);

            if (neighbour_cost < current_cost ||
                std::exp((double) - (neighbour_cost - current_cost) / temperature) >= random_double(e1)) {
                current_path = neighbour;

                if (neighbour_cost < best_solution.weight) {
                    optimal = neighbour;
                    best_solution.weight = neighbour_cost;

                    auto found_after = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::high_resolution_clock::now() - start_time)
                            .count();
                    best_solution.found_after = found_after;
                    best_solution.relative_error_values.emplace_back(found_after, neighbour_cost);
                }
            }

            auto current_runtime = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::high_resolution_clock::now() - start_time)
                    .count();

            if (current_runtime >= time_limit) break;
        }

        k++;
        temperature = this->temp_function(temperature);

        auto current_runtime = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - start_time)
                .count();

        if (current_runtime >= time_limit) break;
    }

    std::cout << "T_k = " << temperature << std::endl;
    std::cout << "exp(-1/T_k) = " << std::exp(-1.0/temperature) << std::endl;

    best_solution.vertices = optimal;

    return best_solution;
}
