

#include <chrono>
#include <random>
#include <utility>
#include "simulated_annealing.h"

simulated_annealing::simulated_annealing(std::function<double(double)> temp_function,
                                         std::function<std::vector<vertex_t>(
                                                 std::vector<vertex_t>)> neighbour_function) {
    this->temp_function = std::move(temp_function);
    this->neighbour_function = std::move(neighbour_function);
}

solution simulated_annealing::solve(graph &graph, int time_limit) {
    static int init_temp_iterations = 50;
    static double initial_increase_acceptance = 0.8;

    solution best_solution;

    static std::random_device r;
    static std::default_random_engine e1(r());
    static std::uniform_int_distribution<int> random_starting_vertex(0, graph.get_vertices());
    static std::uniform_real_distribution<double> random_double(0, 1);

    vertex_t starting_vertex = random_starting_vertex(e1);

    std::vector<vertex_t> current_path;
    current_path.emplace_back(starting_vertex);

    for (vertex_t u = 0; u < graph.get_vertices(); ++u) {
        if (u == starting_vertex) continue;
        current_path.emplace_back(u);
    }

    current_path.emplace_back(starting_vertex);

    std::shuffle(current_path.begin() + 1, current_path.end() - 1, e1);

    best_solution.vertices = current_path;
    best_solution.weight = graph.get_path_weight(current_path);

    int L_k = (graph.get_vertices() * (graph.get_vertices() - 1)) / 2;

    auto start_time = std::chrono::high_resolution_clock::now();

//    double sum = 0;
//    for (int i = 0; i < init_temp_iterations; ++i) {
//        std::vector<vertex_t> neighbour = this->neighbour_function(current_path);
//        sum += std::abs(graph.get_path_weight(neighbour) - best_solution.weight);
//    }
//    sum /= init_temp_iterations;
//
//    double temperature = -(sum / std::log(initial_increase_acceptance));

    double sum = 0;
    for (int i = 0; i < init_temp_iterations; ++i) {
        std::vector<vertex_t> neighbour = this->neighbour_function(current_path);
        sum += std::abs(graph.get_path_weight(neighbour) - best_solution.weight);
    }
    sum /= init_temp_iterations;

    double temperature = -(sum / std::log(1.0 / (double) best_solution.weight));

//    double temperature = best_solution.weight * 2;
    std::cout << "T_0 = " << temperature << std::endl;
    int k = 0;
    while (true) {
        for (int L = 0; L < L_k; ++L) {
            int current_cost = graph.get_path_weight(current_path);

            std::vector<vertex_t> neighbour = this->neighbour_function(current_path);
            int neighbour_cost = graph.get_path_weight(neighbour);

            if (neighbour_cost < current_cost ||
                std::exp((double) (current_cost - neighbour_cost) / temperature) > random_double(e1)) {
                current_path = neighbour;
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

        if (current_runtime >= time_limit || temperature == 0) break;
    }

    std::cout << "T_k = " << temperature << std::endl;
    std::cout << "exp(-1/T_k) = " << std::exp(-1.0/temperature) << std::endl;

    best_solution.weight = graph.get_path_weight(current_path);
    best_solution.vertices = current_path;

    return best_solution;
}
