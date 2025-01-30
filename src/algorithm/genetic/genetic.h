#ifndef GENETIC_H
#define GENETIC_H
#include <functional>
#include <algorithm>
#include <random>

#include "../algorithm.h"

/**
 * Klasa implementująca działanie
 * algorytmu genetycznego
 */
class genetic : public algorithm {
    static inline std::mt19937 random_engine = std::mt19937(std::random_device()());
    static inline std::uniform_real_distribution<> chance = std::uniform_real_distribution(0.0, 1.0);

    /**
     * Rozmiar populacji
     */
    unsigned int population_size;
    /**
     * Współczynnik mutacji
     */
    double mutation_factor;
    /**
     * Współczynnik krzyżowania
     */
    double crossover_factor;

public:
    /**
     * Typ reprezentujący chromosom
     */
    typedef std::vector<vertex_t> chromosome_t;
    /**
     * Typ reprezentujący funkcję mutacji.
     * Mutacja jest przeprowadzana na referencji do
     * chromosomu.
     */
    typedef std::function<void(chromosome_t&)> mutation_fn;
    /**
     * Typ reprezentujący funkcję krzyżowania. Możliwe jest
     * otrzymanie dwóch różnych potomków z funkcji poprzez
     * zamienienie ich miejscami w argumentach.
     */
    typedef std::function<chromosome_t(const chromosome_t&, const chromosome_t&)> crossover_fn;
    /**
     * Typ reprezentujący populacje (zbiór chromosomów).
     */
    typedef std::vector<chromosome_t> population_t;

    /**
     * Używana funkcja krzyżowania
     */
    crossover_fn crossover_function;
    /**
     * Używana funkcja mutacji
     */
    mutation_fn mutation_function;

    genetic(const crossover_fn& crossover_function, const mutation_fn& mutation_function,
            unsigned int population_size, double mutation_factor, double crossover_factor);

    solution solve(graph& graph, int time_limit) override;

    /**
     * Funkcja przystosowania chromosomu. Zwraca ona
     * koszt cyklu Hamiltona stworzony przez chromosom.
     * Ze względu na to, że mniejszy koszt jest lepszy,
     * znaki w selekcji zostały odwrócone.
     *
     * @param graph graf, na podstawie którego wyliczany jest koszt
     * @param chromosome chromosom, z którego ma być zbudowany cykl Hamiltona
     * @return koszt cyklu Hamiltona
     */
    static long fitness(graph& graph, chromosome_t& chromosome);

    /**
     * Funkcja inicjalizująca populację chromosomów.
     * Tworzy ona losowe permutacje tworzące ścieżki Hamiltona.
     *
     * @param graph graf, na którego podstawie mają być generowane chromosomy
     * @return wygenerowana populacja
     */
    [[nodiscard]] population_t initialize_population(graph& graph) const;
    [[nodiscard]] population_t initialize_population_nn(graph& graph) const;

    /**
     * Funkcja oceny populacji. Wyniki bazują na funkcji fitness.
     *
     * @param graph graf, na podstawie którego ma być robiona ocena
     * @param current_population aktualna populacja
     * @return ocena dla każdego osobnika w populacji
     */
    static std::vector<long> evaluate_population(graph& graph, population_t& current_population);

    /**
     * Funkcja selekcji. Stosowaną metodą selekcji jest selekcja
     * turniejowa.
     *
     * @param fitness aktualna ocena populacji
     * @param current_population aktualna populacja
     * @return wyniki turnieju
     */
    population_t selection(std::vector<long>& fitness, population_t& current_population) const;

    /**
     * Funkcja krzyżowania. Tworzy potomków z losowych rodziców wybranych
     * podczas selekcji. Tworzona subpopulacja nie wymienia całkowicie
     * populacji oryginalnej (sukcesja częściowa).
     *
     * @param selected_population wygrani turnieju
     * @param crossover_function funkcja krzyżowania
     * @param mutation_function funkcja mutacji
     * @return potomkowie
     */
    population_t crossover_and_mutate(population_t& selected_population,
                                      const crossover_fn& crossover_function,
                                      const mutation_fn& mutation_function) const;

    /**
     * Odwraca losowo wybrany podciąg w chromosomie.
     */
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

    /**
     * Metoda krzyżowania OX
     */
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

    /**
     * Metoda krzyżowania PMX
     */
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
