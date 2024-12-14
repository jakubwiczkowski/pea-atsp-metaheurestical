#include <random>
#include <algorithm>
#include "algorithm.h"

std::vector<vertex_t> algorithm::random_permutation(graph &graph) {
    static std::random_device r;
    static std::default_random_engine e1(r());
    static std::uniform_int_distribution<int> random_starting_vertex(0, graph.get_vertices() - 1);

    vertex_t starting_vertex = random_starting_vertex(e1);

    std::vector<vertex_t> current_path;
    current_path.emplace_back(starting_vertex);

    for (vertex_t u = 0; u < graph.get_vertices(); ++u) {
        if (u == starting_vertex) continue;
        current_path.emplace_back(u);
    }

    current_path.emplace_back(starting_vertex);

    std::shuffle(current_path.begin() + 1, current_path.end() - 1, e1);

    return current_path;
}