/**
 * przykładowa implementacja warstwowej sieci neuronowej. Sieć nie obsługuje rekurencji. Jest klasycznym rodzajem sieci neuronowej.
 * */

#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <random>

using namespace std;
random_device r_device;
mt19937 rand_gen(r_device());

using matrix_t = vector<vector<double>>;
using vector_t = vector<double>;

vector_t operator*(const matrix_t &m, const vector_t &v)
{
    vector_t result(m.size());
    for (int i = 0; i < m.size(); i++)
    { // przechodzimy po wierszach
        double s = 0;
        for (int j = 0; j < v.size(); j++)
        { // przechodzimy po kolumnach i elementach wektora
            s += m.at(i).at(j) * v.at(j);
        }
        result[i] = s;
    }
    return result;
}
vector_t operator+(const vector_t &a, const vector_t &b)
{
    vector_t ret;
    for (int i = 0; i < a.size(); i++)
    {
        ret[i] = a.at(i) + b.at(i);
    }
    return ret;
}
ostream &operator<<(ostream &o, const matrix_t &m)
{
    for (auto row : m)
    {
        o << "| ";
        for (auto e : row)
        {
            o << e << " ";
        }
        o << "|\n";
    }
    return o;
}
ostream &operator<<(ostream &o, const vector_t &row)
{
    o << "[ ";
    for (auto e : row)
    {
        o << e << " ";
    }
    o << "]\n";
    return o;
}

vector_t activate(
    const vector_t &a, function<double(double)> f = [](double x) {
        if (x < 0)
            return -1.0;
        if (x > 0)
            return 1.0;
        return 0.0;
    })
{
    vector_t result = a;
    for (auto &e : result)
    {
        e = f(e);
    }
    return result;
}

// obliczenie wartości dla kolejnych warstw sieci neuronowej. Sieć jest reprezentowana przez m - macierze wag poszczególnych warstw, oraz a - wektory wartości wzbudzenia poszczególnych neuronów
vector<vector_t> feed_forward(
    const vector<matrix_t> &m, const vector<vector_t> &a,
    function<double(double)> f = [](double x) {
        if (x > 0)
            return 1.0;
        return 0.0;
    })
{
    vector<vector_t> result = a;
    for (int i = 1; i < a.size(); i++)
    {
        auto a_bias = result.at(i - 1);
        a_bias.push_back(1.0);
        result[i] = activate(m.at(i) * a_bias, f);
    }
    return result;
}

pair<vector<matrix_t>, vector<vector_t>> generate_nn_layes(vector<int> sizes, function<double(int l, int i, int j)> g)
{
    vector<matrix_t> m = {{}};
    vector<vector_t> a = {};

    for (int l = 0; l < sizes.size(); l++)
    {
        a.push_back(vector_t(sizes[l])); // tworzymy wartosci wzbudzenia neuronow dla warstw
        if (l > 0)
        {
            matrix_t m_l = matrix_t(sizes[l], vector_t(sizes[l - 1] + 1));
            for (int i = 0; i < m_l.size(); i++)
            {
                for (int j = 0; j < m_l[i].size(); j++)
                {
                    m_l[i][j] = g(l, i, j);
                }
            }
            m.push_back(m_l);
        }
    }
    return {m, a};
}

/// dla danych historycznych stwierdzic czy kupujemy czy sprzedajemy
/// wejscia: 9 wejsc // srednia dzienna
/// wyjscie 0 - sprzedaj, 1 - kupuj

// funkcja oblicza jak bardzo siec neuronowa sie myli. Należy do tego podejść statystycznie, więc
// wektor evaluation_set powinien byc jak największy, ale oczywiście w granicach możliwości obliczeniowych
// im wyższa wartość, tym gorzej działa nasza sieć neuronowa. Należy pamiętać o ustaleniu funkcji aktywacji
//                        znane_wejscia     poprawne_wyjscia
double cost_function(
    const vector<pair<vector_t, vector_t>> &evaluation_set,
    pair<vector<matrix_t>, const vector<vector_t>> neural_network,
    function<double(double)> activation_function = [](double x) {
        if (x > 0)
            return 1.0;
        return 0.0;
    })
{
    double cost = 0;
    for (auto [inputs, outputs] : evaluation_set)
    {
        auto [m, a] = neural_network;
        auto a_out = feed_forward(m, a, activation_function); //, [](auto x){return x;});
        for (int i = 0; i < a_out.back().size(); i++)
        {
            cost += abs(a_out.back().at(i) - outputs.at(i));
        }
    }
    return cost;
}

int main()
{
    auto [m, a] = generate_nn_layes({9, 5, 1}, [](int l, int i, int j) {
        uniform_real_distribution<double> d(-1.0, 1.0);
        return d(rand_gen);
    });
    for (auto m_ : m)
        cout << m_;
    for (auto a_ : a)
        cout << a_;
    cout << "------------------\n";
    a = feed_forward(m, a); //, [](auto x){return x;});
    for (auto m_ : m)
        cout << m_;
    for (auto a_ : a)
        cout << a_;
    cout << "cost: " << cost_function({{{1, 2, 1, 0, 1, 0, 1, 0, 9}, {1}}}, {m, a}) << endl;

    return 0;
}