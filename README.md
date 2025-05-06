# Metaheuristic Algorithms for the Asymmetric Travelling Salesman Problem (ATSP)

This repository contains a project implementation and performance analysis of three metaheuristic approaches—**Simulated Annealing**, **Tabu Search** and **Genetic Algorithm**—for solving the Asymmetric Travelling Salesman Problem (ATSP). The project was developed as part of the "Designing Efficient Algorithms" course and explores the application of metaheuristics to NP-hard optimization problems.

## Overview

The ATSP is a variant of the classical Travelling Salesman Problem (TSP) where the cost of travelling from city A to city B may differ from the cost of travelling from B to A. Given its NP-hard nature, finding exact solutions for large instances is computationally prohibitive. This project uses metaheuristic algorithms to find near-optimal solutions within a reasonable time.

## Problem Description

* **ATSP Definition:** Find the shortest possible cycle that visits each city exactly once and returns to the starting point, considering asymmetric travel costs between cities.
* **Challenges:** Due to its NP-hardness, the problem requires approaches that balance solution quality with computation time.

## Implemented Algorithms

### Simulated Annealing (SA)

* **Concept:** Inspired by the physical annealing process, SA seeks a global minimum by probabilistically accepting worse solutions to escape local optima.
* **Key Features:**

    * **Initial Temperature:** Computed based on the average absolute difference between the initial solution and a randomly selected neighbor. The formula is scaled to enhance acceptance of worse solutions at the start.
    * **Cooling Schedule:** Uses a geometric cooling schedule:
      T(k+1) = a · T(k) with 0 < a < 1.
      The coefficient *a* is tuned to ensure the temperature asymptotically approaches zero near the predefined time limit.
    * **Neighborhood Definition:** The neighborhood is defined by the "swap" operation, where two elements in the solution are exchanged.
    * **Acceptance Criterion:** A new solution is accepted based on the probability
      exp((x\_current - x\_candidate) / T),
      allowing for controlled exploration even when the candidate solution is worse.

### Tabu Search (TS)

* **Concept:** TS is a local search method that uses a memory structure (the Tabu list) to avoid cycling and to diversify the search.
* **Key Features:**

    * **Neighborhood Exploration:** Evaluates the entire neighborhood of the current solution using three types of moves:

        * **Swap:** Exchange two elements.
        * **Insertion:** Remove an element and insert it at another position.
        * **Inversion:** Reverse the order of a subsequence.
    * **Tabu List:** Maintains a list of recently visited solutions (or moves) with a set tenure (cadence) to prevent revisiting them, unless a candidate solution meets the aspiration criteria (i.e., it improves the current best-known solution).
    * **Diversification Mechanism:** If no improvement is found for several iterations, the algorithm resets the current solution randomly to explore new regions of the solution space.

### Genetic Algorithm (GA)

* **Concept:** GA is inspired by natural evolutionary processes. A population of candidate tours (chromosomes) evolves over time via selection, crossover, and mutation to improve solution quality.
* **Key Features:**

    * **Representation:** Chromosomes encode Hamiltonian cycles as sequences of city indices, without the closing edge.
    * **Selection:** Tournament selection with pool size proportional to the population: each tournament draws \$3/10,N\$ individuals and selects the fittest to form the mating pool (with replacement).
    * **Crossover:** Two methods implemented with probability \$p\_c = 0.8\$:

        * **Partially Mapped Crossover (PMX)**
        * **Order Crossover (OX)**
    * **Mutation:** Two operators with probability \$p\_m = 0.01\$:

        * **Inversion:** Reverse a randomly chosen subsequence.
        * **Insertion:** Remove a gene and reinsert it at a random position.
    * **Succession:** Partial generational replacement—offspring replace random individuals in the population.

## Experiment Setup

* **Hardware:** AMD Ryzen 5 4600H (3 GHz), 32 GB RAM, Linux OS.
* **Test Instances:** Three benchmark instances from University of Heidelberg:

    * **ftv55.atsp:** 56 vertices
    * **ftv170.atsp:** 171 vertices
    * **rbg358.atsp:** 358 vertices
* **Execution Parameters:**

    * **Time Limits:** 2 min (56), 4 min (171), 6 min (358).
    * **Repetitions:** 10 runs per configuration.
    * **Performance Metrics:** Best cost, average cost, relative error (%), and execution time in ms.

## Results and Analysis

### SA and TS

- **Simulated Annealing:**  
  The performance of SA depended heavily on the choice of the cooling coefficient _a_. Experiments with different values of _a_ demonstrated that the best results were obtained when the temperature schedule was tuned to nearly exhaust the time limit, allowing thorough exploration early on.

- **Tabu Search:**  
  TS showed promising results when the length of the tabu tenure and the iteration limit without improvements were carefully balanced. Its diversification strategy helped avoid stagnation, although fine-tuning these parameters required extensive experimentation.

- **Comparison:**  
  For the smallest (56 vertices) and largest (358 vertices) instances, both algorithms produced competitive results. However, the TS algorithm’s performance was less sensitive to the elapsed time compared to SA due to its built-in diversification mechanism.

### Genetic Algorithm

We focus here on the ftv170.atsp instance (171 vertices, time budget 240 s) for the Genetic Algorithm.

#### Variable Population Size

Experiments with population sizes of 200, 300, and 400 showed:

* **Best performance** at **200 individuals**, achieving lower average relative error and best-of-run cost compared to larger populations. Larger populations increased exploration but resulted in higher computational overhead without consistent quality gains.

#### Variable Mutation Rate

Mutation rates \$p\_m \in {0.01, 0.05, 0.1}\$ were tested:

* **\$p\_m = 0.05\$** achieved the best trade-off between exploration and exploitation, yielding the lowest average relative error.
* **Low mutation** (\$0.01\$) risked premature convergence; **high mutation** (0.1) introduced too much randomness, degrading solution quality.

#### Variable Crossover Rate

Crossover probabilities \$p\_c \in {0.5, 0.7, 0.9}\$ were evaluated:

* **\$p\_c = 0.7\$–\$0.8\$** provided the best balance, facilitating effective recombination without disrupting high-quality schemata.

#### Comparison with Simulated Annealing

Under the same time budget (240 s):

* **GA best cost:** 5109 (relative error ≈ 80%) with population 200, OX crossover, inversion mutation, \$p\_c = 0.8\$, \$p\_m = 0.05\$.
* **SA best cost:** 3270 (geometric cooling, \$a = 0.9985\$), outperforming GA by \~64% in relative error.

The inferior GA result may stem from random initialization versus SA’s greedy-start, as well as suboptimal selection and succession strategies.

#### Conclusions

* **Parameter Sensitivity:** GA performance is highly dependent on population size, mutation and crossover rates, and operator choices.
* **Population:** Smaller (200) populations converged faster and delivered better solutions under fixed time.
* **Mutation:** Insertion mutation outperformed inversion, which often made overly disruptive changes.
* **Crossover:** PMX and OX yield similar quality; best results observed with OX.
* **Overall:** GA underperformed compared to SA in these experiments. Future work should investigate hybrid initialisation (e.g., seeding with greedy tours), adaptive parameter control, and enhanced selection/succession schemes.

## How to Run the Project

### Prerequisites

* CMake >=3.30
* C++ >=20

### Building the Project

```bash
git clone https://github.com/jakubwiczkowski/pea-atsp-metaheuristic.git
cd pea-atsp-metaheuristic
mkdir build && cd build
cmake ..
```

### Running the Project

```bash
./pea_atsp_opt --instance ftv170.atsp --method GA --pop-size 200 --pc 0.8 --pm 0.05 --time 240
```

Adjust parameters as needed in the command-line options.

## License

This project is licensed under the [MIT License](LICENSE).
