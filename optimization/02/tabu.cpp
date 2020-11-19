#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <list>

using namespace std;
random_device r;
default_random_engine e1(r());

// minimalizacja
auto hill_climbing_alg = [](auto get_random_sol, auto get_all_neighbours, auto goal, int max_iterations, auto callback) {
    auto current_solution = get_random_sol();
    for (int iteration = 0; iteration < max_iterations; iteration++)
    {
        auto next_solutions = get_all_neighbours(current_solution);
        next_solutions.push_back(current_solution);
        current_solution = *(max_element(next_solutions.begin(), next_solutions.end(), [&goal](auto a, auto b) {
            return goal(a) > goal(b);
        }));
        callback(iteration, current_solution);
    }
    return current_solution;
};

//minimalizacja
auto hill_climbing_r_alg = [](auto get_random_sol, auto get_all_neighbours, auto goal, int max_iterations, auto callback) {
    auto current_solution = get_random_sol();
    for (int iteration = 0; iteration < max_iterations; iteration++)
    {
        auto next_solutions = get_all_neighbours(current_solution);
        uniform_int_distribution<int> uniform_dist(0, next_solutions.size() - 1);
        auto next_sol = next_solutions.at(uniform_dist(e1));
        if (goal(current_solution) > goal(next_sol))
        {
            current_solution = next_sol;
        }
        callback(iteration, current_solution);
    }
    return current_solution;
};

//minimalizacja
auto simulated_annealing = [](auto get_random_sol, auto N, auto goal, auto T, int max_iterations, auto callback) {
    using namespace std;
    auto current_solution = get_random_sol();
    auto global_best = current_solution;
    uniform_real_distribution<double> uk(0.0, 1.0);
    for (int iteration = 0; iteration < max_iterations; iteration++)
    {
        auto next_sol = N(current_solution);
        if (goal(current_solution) > goal(next_sol))
        {
            current_solution = next_sol;
        }
        else
        {
            if (uk(e1) < exp(-abs(goal(next_sol) - goal(current_solution)) / T(iteration)))
            {
                current_solution = next_sol;
            }
        }
        if (goal(current_solution) < goal(global_best))
            global_best = current_solution;
        callback(iteration, current_solution);
    }
    return global_best;
};

// minimalizacja
auto tabu_alg = [](auto get_random_sol, auto get_all_neighbours, auto goal, int max_iterations, int tabu_size, auto callback) {
    auto current_solution = get_random_sol();
    auto global_best = current_solution; // musimy znalezc najlepsze w ogole
    using namespace std;
    list<decltype(current_solution)> tabu_list; // lista tabu - czyli punkty do ktorych juz nie wracamy
    tabu_list.push_back(current_solution);
    for (int iteration = 0; iteration < max_iterations; iteration++)
    {
        list<decltype(current_solution)> next_solutions;
        for (auto &n : get_all_neighbours(current_solution))
            if (std::find(tabu_list.begin(), tabu_list.end(),n)==tabu_list.end()) // czy punkt jest w tabu?
                next_solutions.push_back(n); // jesli nie, to dopisujemy punkt
        if (next_solutions.size() == 0) {cerr << "Tabu blocked" << endl; break;}
        current_solution = *(max_element(next_solutions.begin(), next_solutions.end(), [&goal](auto a, auto b) {
            return goal(a) > goal(b);
        })); // wybieramy najlepszy i dopisujemy do tabu
        tabu_list.push_back(current_solution);
        if (tabu_list.size() > tabu_size) // ograniczamy rozmiar tabu
            tabu_list.pop_front(); // czyli kasujemy najstarszy
        if (goal(current_solution) < goal(global_best)) // jesli znalezlismy cos lepszego niz do tej pory
            global_best = current_solution; // to aktualizujemy najlepszy globalnie znaleziony
        callback(iteration, current_solution);
    }
    return global_best;
};

int main(int argc, char **argv)
{

    uniform_real_distribution<double> uniform_dist(-5, 5);
    int max_iterations = (argc > 2) ? stoi(argv[2]) : 10000;

    auto sphere_f = [](vector<double> x) {
        double sum = 0;
        for (auto e : x)
        {
            sum += e * e;
        }
        return sum;
    };

    auto himmelblau = [](vector<double> d) {
        double x = d.at(0);
        double y = d.at(1);
        return -20 * exp(-0.2 * sqrt(0.5 * (x * x + y * y))) - exp(0.5 * cos(2 * M_PI * x) + cos(2 * M_PI * y)) + M_E + 20;
    };

    auto ackley = [](vector<double> d) {
        double x = d.at(0);
        double y = d.at(1);
        return pow(x * x + y - 11, 2.0) + pow(x + y * y - 7, 2);
    };

    auto init = [&uniform_dist]() {
        vector<double> x(2);
        x[0] = uniform_dist(e1);
        x[1] = uniform_dist(e1);
        return x;
    };

    /// f(x)    x należy do R^n
    auto neighbours = [](vector<double> x, double dx = 0.01) {
        vector<vector<double>> ret;
        for (int i = 0; i < x.size(); i++)
        {
            auto nx = x;
            nx[i] += dx;
            if (nx[i] < 5.0)
                ret.push_back(nx);
            nx[i] -= 2.0 * dx;
            if (nx[i] > -5.0)
                ret.push_back(nx);
        }
        return ret;
    };

    auto goal = himmelblau;
    auto debug_print = [=](int i, vector<double> v) {
        int stepprint = max_iterations / 100;
        //if ((i % stepprint) == 0)
            //            cout << i << " " << goal(v) << endl;
            cout << v.at(0) << " " << v.at(1) << " " << goal(v) << endl;
    };
    vector<double> solution;
    if ((argc > 1) && (string(argv[1]) == "hc"))
    {
        cerr << "hc" << endl;
        solution = hill_climbing_alg(
            init,
            neighbours,
            goal,
            max_iterations,
            debug_print);
    }
    if ((argc > 1) && (string(argv[1]) == "rhc"))
    {
        cerr << "rhc" << endl;
        solution = hill_climbing_r_alg(
            init,
            neighbours,
            goal,
            max_iterations,
            debug_print);
    }
    if ((argc > 1) && (string(argv[1]) == "sa"))
    {
        cerr << "sa" << endl;
        solution = simulated_annealing(
            init,
            [](vector<double> v) {
                std::normal_distribution<double> n;
                for (auto &e : v)
                    e = e + n(e1) * 0.002;
                return v;
            },
            goal,
            [](auto k) { return 200000.0 / k; },
            max_iterations,
            debug_print);
    }
    if ((argc > 1) && (string(argv[1]) == "tabu"))
    {
        cerr << "tabu" << endl;
        solution = tabu_alg(
            init,
            neighbours,
            goal,
            max_iterations,
            1000, // tabu size
            debug_print);
    }
    cerr << "#result: [ ";
    for (auto e : solution)
        cerr << e << " ";
    cerr << "] -> " << goal(solution) << endl;
}
