#include "genetic.h"

#include <chrono>
#include <random>
#include <unordered_set>

#include "../greedy/nearest_neighbour.h"

genetic::genetic(const crossover_fn& crossover_function, const mutation_fn& mutation_function,
                 unsigned int population_size, double mutation_factor, double crossover_factor) {
    this->crossover_function = crossover_function;
    this->mutation_function = mutation_function;

    this->population_size = population_size;
    this->mutation_factor = mutation_factor;
    this->crossover_factor = crossover_factor;
}

solution genetic::solve(graph& graph, int time_limit) {
    std::uniform_int_distribution<long> random(0, this->population_size - 1);
    solution best_solution;
    best_solution.weight = std::numeric_limits<int>::max();

    const auto start_time = std::chrono::high_resolution_clock::now();

    population_t population = initialize_population_nn(graph);
    std::vector<double> evaluation = evaluate_population(graph, population);

    while (true) {
        population_t selected = selection(evaluation, population);
        population_t children = crossover_and_mutate(selected, this->crossover_function, this->mutation_function);

        // sukcesja częściowa i losowa
        for (chromosome_t& child : children) {
            population[random(random_engine)] = child;
        }

        evaluation = evaluate_population(graph, population);

        size_t best_index = 0;
        for (size_t i = 0; i < evaluation.size(); i++) {
            if (evaluation[i] < evaluation[best_index]) continue;

            best_index = i;
        }

        int cost = graph.get_non_closed_path_weight(population[best_index]);

        if (best_solution.weight > cost) {
            auto found_after = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::high_resolution_clock::now() - start_time)
                            .count();

            best_solution.weight = cost;
            best_solution.found_after = found_after;
            best_solution.relative_error_values.emplace_back(found_after, cost);

            // std::cout << cost << std::endl;

            std::vector solution(population[best_index]);
            solution.emplace_back(solution.front());

            best_solution.vertices = solution;
        }

        const auto current_runtime = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - start_time)
            .count();

        if (current_runtime >= time_limit) break;
    }

    return best_solution;
}

double genetic::fitness(graph& graph, std::vector<vertex_t>& chromosome) {
    return 1.0 / graph.get_non_closed_path_weight(chromosome);
}

genetic::population_t genetic::initialize_population(graph& graph) const {
    population_t population(this->population_size);

    for (int i = 0; i < this->population_size; i++) {
        std::vector chromosome(graph.get_vertices_list());
        std::ranges::shuffle(chromosome, random_engine);

        population[i] = chromosome;
    }

    return population;
}

genetic::population_t genetic::initialize_population_nn(graph& graph) const {
    static nearest_neighbour nn;
    chromosome_t nn_sol = nn.solve(graph, -1).vertices;
    nn_sol.pop_back();

    population_t population(this->population_size);

    for (int i = 0; i < this->population_size; i++) {
        population[i] = std::vector(nn_sol);
    }

    return population;
}

std::vector<double> genetic::evaluate_population(graph& graph, population_t& current_population) {
    std::vector<double> evaluation;

    for (auto& element : current_population) {
        evaluation.push_back(fitness(graph, element));
    }

    return evaluation;
}

genetic::population_t genetic::selection(std::vector<double>& fitness, population_t& current_population) {
    static std::uniform_int_distribution<short> decision(0, 1);
    std::uniform_int_distribution<long> random(0, current_population.size() - 1);

    const int tournament_size = this->population_size * 0.3;

    population_t selected;

    while (selected.size() < this->population_size) {
        // jako indeks w current_population
        std::unordered_set<size_t> potential_contestants;

        while (potential_contestants.size() < tournament_size) {
            potential_contestants.emplace(random(random_engine));
        }

        std::vector contestants(potential_contestants.begin(), potential_contestants.end());

        size_t winner = contestants[0];

        for (int i = 1; i < contestants.size(); i++) {
            if (fitness[i] < fitness[winner]) continue;
            if (fitness[i] == fitness[winner]) {
                winner = decision(random_engine) ? winner : i;
                continue;
            }

            winner = i;
        }

        selected.push_back(current_population[winner]);
    }

    return selected;
}

genetic::population_t genetic::crossover_and_mutate(population_t& selected_population,
                                                    const crossover_fn& crossover_function,
                                                    const mutation_fn& mutation_function) const {
    population_t new_population;

    while (selected_population.size() >= 1) {
        std::uniform_int_distribution<long> random(0, selected_population.size() - 1);

        long parent_index_a = 0, parent_index_b = 0;

        do {
            parent_index_a = random(random_engine);
            parent_index_b = random(random_engine);
        } while (parent_index_a == parent_index_b);

        if (chance(random_engine) < this->crossover_factor) {
            chromosome_t child_a = crossover_function(selected_population[parent_index_a], selected_population[parent_index_b]);
            chromosome_t child_b = crossover_function(selected_population[parent_index_b], selected_population[parent_index_a]);

            if (chance(random_engine) < this->mutation_factor) mutation_function(child_a);
            if (chance(random_engine) < this->mutation_factor) mutation_function(child_b);

            new_population.emplace_back(child_a);
            new_population.emplace_back(child_b);
        }

        chromosome_t parent_a = selected_population[parent_index_a];
        chromosome_t parent_b = selected_population[parent_index_b];

        selected_population.erase(std::ranges::find(selected_population, parent_a));
        selected_population.erase(std::ranges::find(selected_population, parent_b));
    }

    return new_population;
}
