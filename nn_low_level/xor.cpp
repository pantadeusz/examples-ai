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
// vector_t wynik =  m*v;

vector_t operator+(const vector_t &a,const vector_t &b) {
    vector_t ret;
    for (int i = 0; i < a.size(); i++) {
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

vector_t activate(const vector_t &a, function<double(double)> f = [](double x){
    if (x < 0) return -1.0;
    if (x > 0) return 1.0;
    return 0.0;
}) {
    vector_t result = a;
    for (auto &e: result) {
        e = f(e);
    }
    return result;
}

vector<vector_t> feed_forward (const vector<matrix_t> &m, const vector<vector_t> &a, 
function<double(double)> f  = [](double x){
    if (x > 0) return 1.0;
    return 0.0;
} ) {
    vector<vector_t> result = a;
    for (int i = 1; i < a.size(); i++) {
        auto a_bias = result.at(i-1);
        a_bias.push_back(1.0);
        result[i] = activate(m.at(i)*a_bias, f);
    }
    return result;
}


int main()
{
    // wagi dla warstw
    vector<matrix_t> m = {{},
        // wagi dla pierwszej warstwy ukrytej, ostatnia to bias
        {
        {1, 1,-0.5},
        {-1, -1, 1.5}
        },
        // wagi dla warstwy wyjsciowej, ostatnia to bias
        {
        {1, 1,-1.5}
        }
    };
    vector<vector_t> a = {
        {0.0, 1.0}, // wejscia
        {0, 0 }, // warstwa ukryta
        {0} // wyjscie
    };
    for (auto m_:m)
    cout << m_;
    for (auto a_:a)
    cout << a_;
    cout << "------------------\n";
    a = feed_forward(m,a);//, [](auto x){return x;});
    for (auto m_:m)
    cout << m_;
    for (auto a_:a)
    cout << a_;

    return 0;
}