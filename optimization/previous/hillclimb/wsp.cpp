#include <iostream>
#include <vector>
#include <random>
using namespace std;

using solution_t = vector<int>;
auto goal = [](solution_t x) {
    double ret;
    for (auto e:x) ret += e;
    return ret;
};

int main() {
    double p = 0.10; // prawdopodobienstwo modyfikacji bitu
    random_device r;
    uniform_int_distribution<int> uniform_dist(0, 1);
    uniform_real_distribution<double> uniform_dist_double(0.0, 1.0);
    solution_t x(10); // tymczasowe rozwiązanie
    for (auto &xi:x) {
        xi = uniform_dist(r);
    }
    for (auto &xi:x) cout << xi << " "; cout << endl; 

    int failed_to_fix = 0;
    while (failed_to_fix < 10) {
        // modyfikacja
        auto xn = x;
        for (auto &xi:xn) {
            if (uniform_dist_double(r) < p) {
                xi = 1 - xi;
            }
        }
        // wybranie lepszego
        if (goal(xn) > goal(x)) {
            x = xn;
            failed_to_fix = 0;
        } else {
            failed_to_fix ++;
        }
    }
    for (auto &xi:x) cout << xi << " "; cout << endl; 
    return 0;
}
