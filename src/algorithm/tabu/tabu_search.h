

#ifndef PEA_ATSP_OPT_TABU_SEARCH_H
#define PEA_ATSP_OPT_TABU_SEARCH_H


#include "../algorithm.h"

class tabu_search: public algorithm {
public:
    solution solve(graph &graph, int time_limit) override;
};


#endif //PEA_ATSP_OPT_TABU_SEARCH_H
