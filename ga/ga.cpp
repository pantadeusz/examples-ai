#include <vector>
#include <functional>
#include <iostream>

using namespace std;

typedef vector<char> chromosome_t;

chromosome_t genetic_algorithm(vector<chromosome_t> initial_population,
                               function<double(chromosome_t)> fitness,
                               function<vector<chromosome_t>(chromosome_t, chromosome_t)> crossover,
                               function<chromosome_t(chromosome_t)> mutation,
                               function<vector<chromosome_t>(vector<chromosome_t>, function<double(chromosome_t)>)> selection,
                               function<bool(vector<chromosome_t>)> termination_condition)
{
    vector<chromosome_t> population = initial_population;
    while (!termination_condition(population))
    {
        auto parents = selection(population, fitness);
        vector<chromosome_t> offspring;
        for (unsigned i = 0; offspring.size() < population.size(); i += 2)
        {
            auto a = population[i];     // wylosowanie osobnika z rodzicow
            auto b = population[i + 1]; // wylosowanie osobnika z rodzicow
            auto ne = crossover(a, b);
            offspring.insert(offspring.end(), ne.begin(), ne.end());
        }
        for (auto &e : offspring)
        {
            e = mutation(e);
        }
        population = offspring;
    }
}

int main()
{
    ///////// DEBUGOWANIE ///

    auto print_chromosome = [](chromosome_t &c, auto fitness) {
        for (auto e : c)
        {
            cout << (int)e;
        }
        cout << " " << fitness(c) << endl;
    };

    ////////////////  elementy algorytmu
    double crossover_prob = 0.12;
    double mutation_prob = 0.001;
    int pop_size = 20;
    int iteration_counter = 2000;

    // krzyzowanie jednopunktowe
    auto crossover = [&crossover_prob](chromosome_t a, chromosome_t b) -> vector<chromosome_t> {
        vector<chromosome_t> crossedover = {a, b};
        if (crossover_prob < ((double)rand()) / (double)RAND_MAX)
        {
            int cross_point = rand() % a.size();
            for (unsigned i = 0; i < cross_point; i++)
            {
                crossedover[0][i] = b[i];
                crossedover[1][i] = a[i];
            }
        }
        return crossedover;
    };

    // mutacja jednordna
    auto mutation = [&mutation_prob](chromosome_t a) {
        for (auto &i : a)
        {
            if (mutation_prob < ((double)rand()) / (double)RAND_MAX)
            {
                i = 1 - i;
            }
        }
        return a;
    };

    auto selection = [](vector<chromosome_t> pop,
                        function<double(chromosome_t)> fitness) {
        vector<chromosome_t> ret_pop;
        for (int i = 0; i < pop.size(); i++)
        {
            auto a = pop[rand() % pop.size()];
            auto b = pop[rand() % pop.size()];
            ret_pop.push_back((fitness(a) > fitness(b)) ? a : b);
        }
        return ret_pop;
    };
    /////////////// ZAGADNIENIE DO ROZWIĄZANIA - OneMax  /////////////////////
    auto fitness = [](chromosome_t c) -> double {
        double ret = 0;
        for (auto i : c)
        {
            //ret = ret * 10 + i;
            ret = ret + i;
        }
        return ret;
    };

    auto term_condition = [&](vector<chromosome_t> pop) {
        auto best_fit = pop[0];
        double avg = 0;
        for (auto c : pop)
        {
            avg += fitness(c);
            cout << fitness(c) << " ";
            if (fitness(c) > fitness(best_fit))
                best_fit = c;
        }
        avg = avg / (double)pop.size();
        cout << " (" << avg << ") ";

        print_chromosome(best_fit, fitness);

        iteration_counter--;
        return iteration_counter <= 0;
    };

    vector<chromosome_t> initial_population;
    for (int i = 0; i < pop_size; i++)
    {
        chromosome_t c(32); // nowy chromosom
        for (auto &e : c)
        {
            e = rand() % 2;
        }
        initial_population.push_back(c);
    }

    srand(time(0));
    /////////  TERAZ ALGORYTM
    auto result = genetic_algorithm(initial_population,
                                    fitness,
                                    crossover,
                                    mutation,
                                    selection,
                                    term_condition);
}
