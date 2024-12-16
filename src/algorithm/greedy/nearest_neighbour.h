

#ifndef PEA_ATSP_OPT_NEAREST_NEIGHBOUR_H
#define PEA_ATSP_OPT_NEAREST_NEIGHBOUR_H


#include "../algorithm.h"

/**
 * Klasa implementująca algorytm zachłanny Nearest Neighbour
 */
class nearest_neighbour: public algorithm {
public:
    solution solve(graph &graph, int time_limit) override;
};


#endif //PEA_ATSP_OPT_NEAREST_NEIGHBOUR_H
