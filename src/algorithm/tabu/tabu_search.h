

#ifndef PEA_ATSP_OPT_TABU_SEARCH_H
#define PEA_ATSP_OPT_TABU_SEARCH_H


#include <functional>
#include "../algorithm.h"

class tabu_search : public algorithm {
private:
    int term_time;
    int critical_iterations;
public:
    static int meval(std::vector<vertex_t> &x_a, std::vector<vertex_t> &x_k, graph &graph);

    typedef std::function<int(std::vector<vertex_t> & x_a, std::vector<vertex_t> & x_k, graph & graph)> eval_func;
    typedef std::function<std::vector<vertex_t>(std::vector<vertex_t> &,
                                                graph&,
                                                eval_func,
                                                std::map<std::vector<vertex_t>, int> &,
                                                int aspire_to)> neighbour_t;

    neighbour_t neighbour_function;

    explicit tabu_search(neighbour_t neighbour_function,
                         int term_time,
                         int critical_iterations);

    solution solve(graph &graph, int time_limit) override;
};


#endif //PEA_ATSP_OPT_TABU_SEARCH_H
