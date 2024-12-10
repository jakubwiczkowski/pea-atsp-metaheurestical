

#include <chrono>
#include "simulated_annealing.h"

solution simulated_annealing::solve(graph &graph, int time_limit) {
    solution best_solution;

    auto start_time = std::chrono::high_resolution_clock::now();

    int k = 0;
    while(true) {




        k++;

        auto current_runtime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start_time)
                .count();

        if (current_runtime >= time_limit) break;
    }


    return best_solution;
}
