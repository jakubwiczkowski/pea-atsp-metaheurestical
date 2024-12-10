

#ifndef PEA_ATSP_OPT_SIMULATED_ANNEALING_H
#define PEA_ATSP_OPT_SIMULATED_ANNEALING_H


#include "../algorithm.h"

class simulated_annealing: public algorithm {
public:
    solution solve(graph &graph, int time_limit) override;
};


#endif //PEA_ATSP_OPT_SIMULATED_ANNEALING_H
