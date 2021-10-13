#include <functional>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

random_device rd;  //Will be used to obtain a seed for the random number engine
mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

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
        o << e << ",";
    }
    return o;
}

vector<double> hill_climbing(function<double(vector<double>)> f, vector<double> p0, int iterations)
{
    auto p = p0;
    uniform_int_distribution<> distrib(0, p.size() - 1);
    uniform_real_distribution<> distrib_r(-0.1, 0.1);

    for (int i = 0; i < iterations; i++) {
        auto p2 = p;

        p[distrib(gen)] += distrib_r(gen);
        double y2 = f(p2);
        if (y2 > f(p)) {
            p = p2;
        }
        cout << p << " ->" << f(p) << endl;
    }
    return p;
}
int main()
{
    hill_climbing([](vector<double> v) {
        return 100 - v[0] * v[0];
    },
        {10.2, 5.5}, 5000);
    return 0;
}
