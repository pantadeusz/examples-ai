#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <random>
#include <vector>

using namespace std;

random_device rd;           // Will be used to obtain a seed for the random number engine
mt19937 mt_generator(rd()); // Standard mersenne_twister_engine seeded with rd()

/*
x = losowy_punkt_dziedziny_f.

powtarzaj n razy
   x` = x+rand()
   y = f(x`)
   jeżeli y > f(x)
       x = x`

*/

ostream& operator<<(ostream& o, vector<double> v)
{
    for (auto e : v) {
        o << std::fixed << std::setprecision(5) << "\t" << e;
    }
    return o;
}

vector<double> hill_climbing(function<double(vector<double>)> f, function<bool(vector<double>)> f_domain, vector<double> p0, int iterations)
{
    auto p = p0;
    uniform_int_distribution<> distrib(0, p.size() - 1);
    uniform_real_distribution<> distrib_r(-0.1, 0.1);

    if (!f_domain(p)) throw std::invalid_argument("The p0 point must be in domain");
    for (int i = 0; i < iterations; i++) {
        auto p2 = p;

        p2[distrib(mt_generator)] += distrib_r(mt_generator);
        // remember about domain
        double y2 = f(p2);
        if (y2 < f(p)) {
            p = p2;
        }
    }
    return p;
}
vector<double> operator+(vector<double> a, vector<double> b)
{
    for (int i = 0; i < a.size(); i++) {
        a[i] += b[i];
    }
    return a;
}

vector<double> tabu_search(function<double(vector<double>)> f, function<bool(vector<double>)> f_domain, vector<double> p0, int iterations)
{
    // option: tabu size: // const int tabu_size = 5000;
    const double mod_range = 0.01;

    auto offset_f = [=](int i, double dir) {
        vector<double> v(p0.size());
        v[i] = dir;
        return v;
    };
    vector<vector<double>> directions;
    for (int i = 0; i < p0.size(); i++) {
        directions.push_back(offset_f(i, mod_range));
        directions.push_back(offset_f(i, -mod_range));
    }

    vector<vector<double>> tabu_list = {p0};
    // check if the element is in tabu
    auto in_tabu = [&tabu_list](vector<double> p) {
        for (auto& e : tabu_list) {
            int checks = 0;
            for (int i = 0; i < e.size(); i++)
                if (abs(e[i] - p[i]) < 0.00000001) checks++;
            if (checks == e.size()) return true; // in tabu
        }
        return false;
    };

    for (int i = 0; i < iterations; i++) {
        vector<double> best_neighbour;

        auto tabu_i = tabu_list.end(); //.size() - 1;
        do {
            tabu_i--;
            for (auto direction : directions) {
                if (((best_neighbour.size() == 0) ||
                        (f(*tabu_i + direction) < f(best_neighbour))) &&
                    (!in_tabu(*tabu_i + direction))) {
                    best_neighbour = *tabu_i + direction;
                }
            }
            if (best_neighbour.size() != 0) break;

        } while (tabu_i != tabu_list.begin());
        tabu_list.push_back(best_neighbour);
        /* option - limit Tabu size: */ //  if (tabu_list.size() > tabu_size) tabu_list.pop_front();
        cout << (tabu_list.back()) << " " << f(tabu_list.back()) << endl;
    }

    return *std::min_element(tabu_list.begin(), tabu_list.end(), [f](auto a, auto b) {
        return f(a) < f(b);
    });
}


vector<double> simulated_annealing(
    function<double(vector<double>)> f,
    function<bool(vector<double>)> f_domain,
    vector<double> p0,
    int iterations,
    function<vector<double>(vector<double>)> N,
    function<double(int)> T)
{
    auto s_current = p0;
    auto s_global_best = p0;

    //    uniform_int_distribution<> distrib(0, p.size() - 1);
    uniform_real_distribution<> u_k(0.0, 1.0);

    if (!f_domain(s_current)) throw std::invalid_argument("The p0 point must be in domain");

    for (int k = 0; k < iterations; k++) {
        auto s_next = N(s_current);
        if (f(s_next) < f(s_current)) {
            s_current = s_next;
        } else {
            double u = u_k(mt_generator);
            if (u < exp(-abs(f(s_next) - f(s_current)) / T(k))) {
                s_current = s_next;
            } else {
                // nothing...
            }
        }
        if (f(s_current) < f(s_global_best)) {
            s_global_best = s_current;
        }
        //        cout << k << " " << f(s_current) << endl;
        cout << s_current << " " << f(s_current) << endl;
    }
    return s_global_best;
}


int main()
{
    auto function1 = [](vector<double> v) {
        return 100 - (v[0] * v[0] + v[1] * v[1]);
    };

    auto ackley = [](vector<double> v) {
        double x = v.at(0), y = v.at(1);
        return -20 * exp(-0.2 * sqrt(0.5 * (x * x + y * y))) - exp(0.5 * (cos(2 * M_PI * x) + cos(2 * M_PI * y))) + M_E + 20.0;
    };

    auto ackley_domain = [](vector<double> v) {
        return (abs(v[0]) <= 5) && (abs(v[1]) <= 5);
    };

    uniform_real_distribution<> distrib_r(-5, 5);
    vector<double> ackley_p0 = {
        distrib_r(mt_generator),
        distrib_r(mt_generator),
    };


    //    auto result = hill_climbing(ackley, ackley_domain, ackley_p0, 10);
    //    cout << result << " -> " << ackley(result) << endl;

//    auto result = tabu_search(ackley, ackley_domain, ackley_p0, 10000);
    auto result = simulated_annealing(
        ackley, ackley_domain, ackley_p0, 10000,
        [](auto p) {
            normal_distribution<double> n(0.0, 0.3);
            for (auto& e : p) {
                e = e + n(mt_generator);
            }
            return p;
        },
        [](int k) { return 1000.0 / k; });
    cout << result << " -> " << ackley(result) << endl;

    return 0;
}
