cmake_minimum_required(VERSION 3.30)
project(pea_atsp_opt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3")

add_executable(pea_atsp_opt
        src/main.cpp
        src/type/types.h
        src/graph/graph.cpp
        src/graph/graph.h
        src/menu/menu.cpp
        src/menu/menu.h
        src/graph/loader/loader.h
        src/graph/loader/regular_loader.cpp
        src/graph/loader/regular_loader.h
        src/graph/loader/xml_loader.cpp
        src/graph/loader/xml_loader.h
        src/algorithm/algorithm.h
        src/algorithm/solution.h
        src/algorithm/greedy/nearest_neighbour.cpp
        src/algorithm/greedy/nearest_neighbour.h
        src/algorithm/annealing/simulated_annealing.cpp
        src/algorithm/annealing/simulated_annealing.h)
