#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

using namespace std;
random_device r_device;
mt19937 rand_gen(r_device());

using matrix_t = vector<vector<double>>;
using vector_t = vector<double>;


// mnozenie macierzy przez wektor
vector_t operator*(const matrix_t& m, const vector_t& v)
{
    vector_t result(m.size());
    for (int i = 0; i < m.size(); i++) { // przechodzimy po wierszach
        result[i] = 0;
        auto m_row = m.at(i);
        for (int j = 0; j < v.size(); j++) { // przechodzimy po kolumnach i elementach wektora
            result[i] += m_row.at(j) * v.at(j);
        }
    }
    return result;
}
// suma wektorow
vector_t operator+(const vector_t& a, const vector_t& b)
{
    vector_t ret;
    for (int i = 0; i < a.size(); i++) {
        ret[i] = a.at(i) + b.at(i);
    }
    return ret;
}

ostream& operator<<(ostream& o, const matrix_t& m)
{
    for (auto row : m) {
        o << "| ";
        for (auto e : row) {
            o << e << " ";
        }
        o << "|\n";
    }
    o << endl;
    return o;
}
ostream& operator<<(ostream& o, const vector_t& row)
{
    o << "[ ";
    for (auto e : row) {
        o << e << " ";
    }
    o << "]\n";
    return o;
}

function<double(double)> bipolar_f = [](double x) {
    if (x < 0) return -1.0;
    if (x > 0) return 1.0;
    return 0.0;
};

function<double(double)> unipolar_f = [](double x) {
    if (x > 0) return 1.0;
    return 0.0;
};

function<double(double)> fermi_f = [](double x) {
    const double g = 0.9999;
    return 1.0/(1+exp(-g*x));
    
};


// aktywacja dla kazdego elementu wektora
vector_t activate(const vector_t& a_, function<double(double)> f = bipolar_f)
{
    auto a = a_; // przepisujemy
    std::transform(a.begin(), a.end(), a.begin(), f);
    return a;
}

vector<vector_t> feed_forward(
    const vector<matrix_t>& m, // macierz wag
    const vector<vector_t>& a, // wejscia
    function<double(double)> f = unipolar_f)
{
    vector<vector_t> result = a;
    for (int i = 1; i < a.size(); i++) {
        auto a_bias = result.at(i - 1);
        a_bias.push_back(1.0); // bias mnozymy przez 1
        result[i] = activate(m.at(i) * a_bias, f);
    }
    return result;
}



int main(int argc, char** argv)
{
    if (argc < 3) {
        cerr << "Podaj 2 argumenty do funkcji xor" << endl;
        return -1;
    }
    // wagi dla warstw
    vector<matrix_t> m = {
        // warstwa wejsciowa nie ma wag. W praktyce mozna pominac
        {{}, {}},
        // wagi dla pierwszej warstwy ukrytej, ostatnia kolumna to bias
        {
            {1, 1, -0.5},
            {-1, -1, 1.5}},
        // wagi dla warstwy wyjsciowej, ostatnia waga to bias
        {{1, 1, -1.5}}};
     vector<vector_t> a = {
         {stof(argv[1]), stof(argv[2])}, // wejscia - nie dopisuje 1 na ostatnim neuronie - jest to bias
         {0, 0},                         // warstwa ukryta
         {0}                             // wyjscie
     };
    for (auto m_ : m)
        cout << m_ << endl;
    for (auto a_ : a)
        cout << "aktywacja: " << a_;
    cout << "------------------\n";
    a = feed_forward(m, a, unipolar_f);
    for (auto m_ : m)
        cout << m_;
    for (auto a_ : a)
        cout << a_;
    cout << "wynik " << a[0][0] << " xor " << a[0][1] << " = " << a[2][0] << endl;

    return 0;
}