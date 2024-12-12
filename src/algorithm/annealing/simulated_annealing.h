

#ifndef PEA_ATSP_OPT_SIMULATED_ANNEALING_H
#define PEA_ATSP_OPT_SIMULATED_ANNEALING_H


#include <functional>
#include "../algorithm.h"

class simulated_annealing: public algorithm {
    std::function<double(double)> temp_function;
    std::function<std::vector<vertex_t>(std::vector<vertex_t>)> neighbour_function;

public:
    explicit simulated_annealing(std::function<double(double)> temp_function,
                                 std::function<std::vector<vertex_t>(std::vector<vertex_t>)> neighbour_function);

    solution solve(graph &graph, int time_limit) override;
};


#endif //PEA_ATSP_OPT_SIMULATED_ANNEALING_H
