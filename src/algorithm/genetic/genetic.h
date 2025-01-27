#ifndef GENETIC_H
#define GENETIC_H
#include <functional>
#include <algorithm>
#include <random>

#include "../algorithm.h"


class genetic : public algorithm {
    static inline std::mt19937 random_engine = std::mt19937(std::random_device()());
    static inline std::uniform_real_distribution<> chance = std::uniform_real_distribution(0.0, 1.0);

    unsigned int population_size;
    double mutation_factor;
    double crossover_factor;

public:
    typedef std::vector<vertex_t> chromosome_t;
    typedef std::function<void(chromosome_t&)> mutation_fn;
    typedef std::function<chromosome_t(const chromosome_t&, const chromosome_t&)> crossover_fn;
    typedef std::vector<chromosome_t> population_t;

    crossover_fn crossover_function;
    mutation_fn mutation_function;

    genetic(const crossover_fn& crossover_function, const mutation_fn& mutation_function,
            unsigned int population_size, double mutation_factor, double crossover_factor);

    solution solve(graph& graph, int time_limit) override;

    static long fitness(graph& graph, chromosome_t& chromosome);

    [[nodiscard]] population_t initialize_population(graph& graph) const;
    [[nodiscard]] population_t initialize_population_nn(graph& graph) const;
    static std::vector<long> evaluate_population(graph& graph, population_t& current_population);
    population_t selection(std::vector<long>& fitness, population_t& current_population) const;
    population_t crossover_and_mutate(population_t& selected_population,
                                      const crossover_fn& crossover_function, const mutation_fn& mutation_function) const;

    const static inline mutation_fn inversion = [](chromosome_t& chromosome) {
        std::uniform_int_distribution<> random(0, chromosome.size() - 1);

        long position_a = 0, position_b = 0;

        do {
            position_a = random(random_engine);
            position_b = random(random_engine);
        }
        while (position_a == position_b);

        if (position_a > position_b) std::swap(position_a, position_b);

        std::reverse(chromosome.begin() + position_a, chromosome.begin() + position_b + 1);
    };

    /**
     * Metoda mutacji polegająca na losowym wybraniu genu z chromosomu,
     * a następnie przestawieniu go w losowe miejsce w chromosomie.
     */
    const static inline mutation_fn insertion = [](chromosome_t& chromosome) {
        std::uniform_int_distribution<long> random(0, chromosome.size() - 1);

        const long old_position = random(random_engine);
        vertex_t gene = chromosome[old_position];
        const long new_position = random(random_engine);

        chromosome.erase(chromosome.begin() + old_position);
        chromosome.insert(chromosome.begin() + new_position, gene);
    };

    const static inline mutation_fn swap = [](chromosome_t& chromosome) {
        std::uniform_int_distribution<long> random(0, chromosome.size() - 1);

        long position_a = 0, position_b = 0;

        do {
            position_a = random(random_engine);
            position_b = random(random_engine);
        }
        while (position_a == position_b);

        std::iter_swap(chromosome.begin() + position_a, chromosome.begin() + position_b);
    };

    const static inline crossover_fn ox = [](const chromosome_t& parent_a, const chromosome_t& parent_b) -> chromosome_t {
        std::uniform_int_distribution<long> random(0, parent_a.size() - 1);
        chromosome_t child(parent_a.size(), std::numeric_limits<vertex_t>::max());

        long position_a = 0, position_b = 0;

        do {
            position_a = random(random_engine);
            position_b = random(random_engine);
        }
        while (position_a == position_b);

        if (position_a > position_b) std::swap(position_a, position_b);

        for (int i = position_a; i <= position_b; i++) {
            child[i] = parent_a[i];
        }

        for (int i = 0; i < parent_b.size(); i++) {
            if (std::ranges::find(child, parent_b[i]) != child.end()) continue;

            child[std::ranges::find(child, std::numeric_limits<vertex_t>::max()) - child.begin()] = parent_b[i];
        }

        return child;
    };

    const static inline crossover_fn pmx = [](const chromosome_t& parent_a, const chromosome_t& parent_b) -> chromosome_t {
        std::uniform_int_distribution<long> random(0, parent_a.size() - 1);
        chromosome_t child(parent_a.size(), std::numeric_limits<vertex_t>::max());

        long position_a = 0, position_b = 0;

        do {
            position_a = random(random_engine);
            position_b = random(random_engine);
        }
        while (position_a == position_b);

        if (position_a > position_b) std::swap(position_a, position_b);

        for (int i = position_a; i <= position_b; i++) {
            child[i] = parent_a[i];
        }

        for (int i = position_a; i <= position_b; i++) {
            vertex_t to_copy = parent_b[i];
            vertex_t copied = parent_a[i];

            if (std::ranges::find(child, to_copy) != child.end()) continue;

            long index = std::ranges::find(parent_b, copied) - parent_b.begin();

            if (child[index] == std::numeric_limits<vertex_t>::max()) {
                child[index] = to_copy;
            }
            else {
                vertex_t k = child[index];
                long index_k = std::ranges::find(parent_b, k) - parent_b.begin();
                child[index_k] = to_copy;
            }
        }

        for (int i = 0; i < parent_b.size(); i++) {
            if (std::ranges::find(child, parent_b[i]) != child.end()) continue;

            child[std::ranges::find(child, std::numeric_limits<vertex_t>::max()) - child.begin()] = parent_b[i];
        }

        return child;
    };
};

#endif //GENETIC_H
