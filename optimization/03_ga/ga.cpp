#include <iostream>
#include <functional>
#include <random>

///////////////////////  ALGORYTM GENETYCZNY

std::mt19937 rand_gen(10101212121);


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
        for (auto &e : population) {
            std::cout << " " << e.fit;
        }
        std::cout << std::endl;
        iterations_max--;
        if (iterations_max > 0) return false;
        else return true;
    };
    return term_condition;
};

auto selection = [] (auto population) {
    return population;
};
auto crossover = [] (auto population) {
    return population;
};
auto mutation = [] (auto population) {
    return population;
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
        std::uniform_int_distribution<char> uni(0,1);
        for (auto &e : chromosome)
            e = uni(rand_gen);
        fit = -1;
    }
};

double fitness(specimen_t genotype)
{
    double s = 0;
    for (auto e : genotype.chromosome) s+= e;
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
    for (int i = 0; i < 10; i++)
    {
        ret.push_back(specimen_t(32));
        ret[i].randomize();
    }
    return ret;
};

int main()
{
    int iterator = 10;
    auto ret = genetic_algorithm( calculate_pop_fitness,
                              generate_init_pop,
                            get_term_condition_iterations(std::vector<specimen_t>(), iterator),
                             selection,
                             crossover,
                             mutation);
    

}