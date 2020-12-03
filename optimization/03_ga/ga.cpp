#include <iostream>
#include <functional>
#include <random>
#include <algorithm>
#include <utility>
#include <string_view>
///////////////////////  ALGORYTM GENETYCZNY

std::random_device rdev;
std::mt19937 rand_gen(192987432);

auto genetic_algorithm = [](
                             auto calculate_pop_fitness,
                             auto generate_init_pop,
                             auto term_condition,
                             auto selection,
                             auto crossover,
                             auto mutation) {
    using namespace std;
    auto population = generate_init_pop();
    population = calculate_pop_fitness(population);
    while (!term_condition(population))
    {
        auto parents = selection(population);
        auto offspring = crossover(parents);
        offspring = mutation(offspring);
        population = calculate_pop_fitness(offspring);
    }
    return population;
};

auto get_term_condition_iterations = [](auto pop, int &iterations_max) {
    auto term_condition = [&iterations_max](decltype(pop) population) {
        std::cout << "[" << iterations_max << "] ";
        for (auto &e : population)
        {
            std::cout << " " << e.fit;
        }
        std::cout << std::endl;
        iterations_max--;
        if (iterations_max > 0)
            return false;
        else
            return true;
    };
    return term_condition;
};

auto selection = [](auto population) {
    return population;
};
auto crossover = [](auto population) {
    return population;
};
auto mutation = [](auto population) {
    return population;
};

auto get_selection_roulette = [](auto pop, unsigned int elite_size = 0) {
    auto selection_roulette = [elite_size](auto population) {
        using namespace std;
        decltype(population) selected_specimens;
        double sum_fitness = accumulate(population.begin(), population.end(), 0.0, [](auto a, auto b) { return a + b.fit; });

        std::sort(population.begin(), population.end(), [](auto a, auto b) { return a.fit > b.fit; });
        cout << "sum_fit = " << sum_fitness << " " << population[0].fit << endl;
        for (int e = 0; e < elite_size; e++)
            selected_specimens.push_back(population.at(e));
        uniform_real_distribution<double> dist(0.0, sum_fitness);
        for (int c = elite_size; c < population.size(); c++)
        {
            double r = dist(rand_gen);
            double s = 0.0;
            for (unsigned int i = 0; i < population.size(); i++)
            {
                s += population[i].fit;
                if (r < s)
                {
                    selected_specimens.push_back(population.at(i));
                    break;
                }
            }
        }
        std::shuffle(selected_specimens.begin(), selected_specimens.end(),rand_gen);
        return selected_specimens;
    };
    return selection_roulette;
};

auto get_crossover_one_point = [](auto pop, double p_crossover = 0.9) {
    auto crossover_one_point = [p_crossover](decltype(pop) population) {
        decltype(pop) ret_pop;
        std::uniform_real_distribution<double> r_pcross(0.0, 1.0);
        for (int i = 0; i < (population.size() - 1); i += 2)
        {
            auto a = population.at(i);
            auto b = population.at(i + 1);

            if (r_pcross(rand_gen) < p_crossover)
            {
                std::uniform_int_distribution<int> dist(0, a.chromosome.size() - 1);
                auto crossover_p = dist(rand_gen);
                for (int g = crossover_p; g < a.chromosome.size(); g++)
                {
                    std::swap(a.chromosome[g], b.chromosome[g]);
                }
            }
            ret_pop.push_back(a);
            ret_pop.push_back(b);
        }
        return ret_pop;
    };
    return crossover_one_point;
};

auto get_mutation_uniform = [](auto pop, double p_mutation = 0.1) {
    auto mutation_uniform = [p_mutation](decltype(pop) population) {
        decltype(pop) ret_pop;
        std::uniform_real_distribution<double> r_mut(0.0, 1.0);
        for (int i = 0; i < population.size(); i++)
        {
            auto a = population.at(i);
            for (int l = 0; l < a.chromosome.size(); l++)
            {
                if (r_mut(rand_gen) < p_mutation)
                {
                    a.chromosome[l] = 1 - a.chromosome[l];
                }
            }
            ret_pop.push_back(a);
        }
        return ret_pop;
    };
    return mutation_uniform;
};
///////////////////////// ZADANIE DO ROZWIĄZANIA

struct specimen_t
{
    std::vector<char> chromosome;
    double fit;
    specimen_t(int n = 0)
    {
        chromosome.resize(n);
        fit = -1;
    }
    void randomize()
    {
        std::uniform_int_distribution<char> uni(0, 1);
        for (auto &e : chromosome)
            e = uni(rand_gen);
        fit = -1;
    }
};

double fitness(specimen_t genotype)
{
    double s = 0;
    for (auto e : genotype.chromosome)
        s += e;
    return s;
}

std::vector<specimen_t> calculate_pop_fitness(const std::vector<specimen_t> pop)
{
    std::vector<specimen_t> ret;
    ret.reserve(pop.size());
    for (auto e : pop)
    {
        e.fit = fitness(e);
        ret.push_back(e);
    }
    return ret;
}

std::vector<specimen_t> generate_init_pop()
{
    std::vector<specimen_t> ret;
    for (int i = 0; i < 32; i++)
    {
        ret.push_back(specimen_t(32));
        ret[i].randomize();
    }
    rand_gen.seed(rdev());
    return ret;
};

int main()
{
    int iterator = 1000;
    auto ret = genetic_algorithm(calculate_pop_fitness,
                                 generate_init_pop,
                                 get_term_condition_iterations(std::vector<specimen_t>(), iterator),
                                 get_selection_roulette(std::vector<specimen_t>(), 2),
                                 get_crossover_one_point(std::vector<specimen_t>(), 0.6),
                                 get_mutation_uniform(std::vector<specimen_t>(), 1.0 / 32.0));
}
