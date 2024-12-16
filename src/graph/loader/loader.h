

#ifndef PEA_ATSP_OPT_LOADER_H
#define PEA_ATSP_OPT_LOADER_H


#include <string>
#include "../graph.h"

/**
 * Klasa reprezentująca mechanizmy
 * wczytywania grafów z pliku
 *
 * @tparam T informacje nagłówkowe pliku
 */
template<typename T>
class loader {
public:
    /**
     * Wczytuje graf z podanego pliku
     *
     * @param file strumień wejściowy pliku
     * @return dane nagłówkowe i graf
     */
    virtual std::pair<T, graph*> load(std::ifstream& file) = 0;
};


#endif //PEA_ATSP_OPT_LOADER_H
