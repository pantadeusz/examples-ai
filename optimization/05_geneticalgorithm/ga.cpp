#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <tuple>
#include <vector>

using genotype_t = std::vector<char>;
using population_t = std::vector<genotype_t>;
using population_fitness_t = std::vector<double>;


std::random_device random_dev;
std::default_random_engine random_engine(random_dev());
std::uniform_real_distribution<double> double_random_dist(0.0, 1.0);
std::uniform_int_distribution<char> rand_01(0, 1);


std::vector<int> selection_roulette(population_fitness_t fitnesses)
{
    std::vector<int> ret;
    double sum_fit = std::accumulate(fitnesses.begin(), fitnesses.end(), 0.0);
    std::uniform_real_distribution<double> random_distr(0.0, sum_fit);
    for (int j = 0; j < fitnesses.size(); j++) {
        double rand_selected = random_distr(random_engine);
        double s = fitnesses[0];
        int selected_i = fitnesses.size() - 1;
        for (int i = 0; i < fitnesses.size() - 1; i++) {
            if (s > rand_selected) {
                selected_i = i;
                break;
            }
            s = s + fitnesses[i + 1];
        }

        ret.push_back(selected_i);
    }
    return ret;
}

std::pair<genotype_t, genotype_t> empty_crossover_f(genotype_t a, genotype_t b)
{
    return {a, b};
}
genotype_t empty_mutation_f(genotype_t a, double)
{
    return a;
}


population_t genetic_algorithm(
    std::function<double(genotype_t)> fitnes_f,
    const population_t init_population,
    double crossover_probability,
    double mutation_probability,
    std::function<std::pair<genotype_t, genotype_t>(genotype_t, genotype_t)> crossover_f,
    std::function<genotype_t(genotype_t, double)> mutation_f,
    std::function<std::vector<int>(population_fitness_t)> select_f,
    std::function<bool(population_t, population_fitness_t, int)> termination_cond = [](auto, auto, auto i) {
        return i < 100;
    })
{
    int iteration = 0;
    population_t population = init_population;
    population_fitness_t population_fit;
    for (auto& gene : population)
        population_fit.push_back(fitnes_f(gene));
    while (termination_cond(population, population_fit, iteration)) {
        std::vector<int> parent_pop_i = select_f(population_fit);

        population_t offspring_pop(parent_pop_i.size());
        for (int i = 1; i < parent_pop_i.size(); i += 2) {
            int parent_idx_0 = parent_pop_i[i - 1];
            int parent_idx_1 = parent_pop_i[i];
            if (double_random_dist(random_engine) < crossover_probability) {
                auto [a, b] = crossover_f(population[parent_idx_0], population[parent_idx_1]);
                offspring_pop[i - 1] = a;
                offspring_pop[i] = b;
            } else {
                offspring_pop[i - 1] = population[parent_idx_0];
                offspring_pop[i] = population[parent_idx_1];
            }
        }

        for (int i = 0; i < parent_pop_i.size(); i++) {
            offspring_pop[i] = mutation_f(offspring_pop[i], mutation_probability);
        }

        population = offspring_pop;

        for (int i = 0; i < population.size(); i++)
            population_fit[i] = fitnes_f(population[i]);
        iteration++;
    }

    return population;
}


/////// problem implementation

double one_max(genotype_t gene)
{
    return std::accumulate(gene.begin(), gene.end(), 0.0);
}

int main()
{
    population_t init_pop(10);
    for (auto& p : init_pop) {
        for (int i = 0; i < 20; i++)
            p.push_back(rand_01(random_engine));
    }

    auto debug_term_cond = [](auto pop, auto fit, auto i) {
        std::cout << i << ":";
        for (auto e : fit) {
            std::cout << " " << e;
        }
        std::cout << std::endl;
        return i < 100;
    };

    auto result = genetic_algorithm(
        one_max,
        init_pop,
        0.8, 0.001, empty_crossover_f, empty_mutation_f, selection_roulette,debug_term_cond);
    return 0;
}