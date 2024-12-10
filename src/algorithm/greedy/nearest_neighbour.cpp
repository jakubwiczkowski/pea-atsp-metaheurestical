
#include <algorithm>
#include <limits>
#include "nearest_neighbour.h"

solution nearest_neighbour::solve(graph &graph, int time_limit) {
    solution best_solution;
    best_solution.weight = std::numeric_limits<int>::max();

    auto adjacency_list = graph.generate_adjacency_list();

    for (vertex_t starting_vertex = 0; starting_vertex < graph.get_vertices(); ++starting_vertex) {
        std::vector<vertex_t> visited;
        visited.emplace_back(starting_vertex);

        vertex_t current = starting_vertex;

        while (visited.size() < graph.get_vertices()) {
            for (vertex_t potential_next: adjacency_list[current]) {
                if (std::find(visited.begin(), visited.end(), potential_next) != visited.end()) continue;

                current = potential_next;
                visited.emplace_back(current);

                break;
            }
        }

        visited.emplace_back(starting_vertex);

        int weight = graph.get_path_weight(visited);

        if (weight < best_solution.weight) {
            best_solution.weight = weight;
            best_solution.vertices = visited;
        }
    }

    return best_solution;
}
