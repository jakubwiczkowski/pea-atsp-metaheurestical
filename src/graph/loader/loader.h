

#ifndef PEA_ATSP_OPT_LOADER_H
#define PEA_ATSP_OPT_LOADER_H


#include <string>
#include "../graph.h"

template<typename T>
class loader {
public:
    virtual std::pair<T, graph*> load(std::ifstream& file) = 0;
};


#endif //PEA_ATSP_OPT_LOADER_H
