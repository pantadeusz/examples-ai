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
    int row_i = 0;
    o << "{ ";
    for (auto row : m) {
        o << (((row_i++) == 0) ? "" : ",");
        o << "{ ";
        int i = 0;
        for (auto e : row) {
            o << (((i++) == 0) ? " " : ", ") << e;
        }
        o << "}";
    }
    o << " }"; // << endl;
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
    return 1.0 / (1 + exp(-g * x));
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

vector<vector_t> generate_activations(vector<matrix_t> nn)
{
    vector<vector_t> activations;
    for (auto layer : nn) {
        activations.push_back(vector_t(layer.size()));
    }
    return activations;
}

vector<vector_t> generate_activations_with_input(vector<matrix_t> nn, vector_t input)
{
    vector<vector_t> activations = generate_activations(nn);
    if (activations.at(0).size() != input.size()) throw invalid_argument("wrong size of input " + to_string(nn.at(0).size()) + " " + to_string(input.size()));
    activations.at(0) = input;
    return activations;
}

double loss_function(vector<vector_t> training_set, vector<matrix_t> nn, function<double(double)> f = unipolar_f)
{
    double loss = 0.0;
    vector<vector_t> activations = generate_activations(nn);

    for (auto training_row : training_set) {
        vector_t inputs = {training_row.begin(), training_row.end() - 1};
        double expected_output = training_row.back();
        activations[0] = inputs;
        auto result_activations = feed_forward(nn, activations, f);
        loss += abs(result_activations.back().at(0) - expected_output);
    }
    return loss;
}


random_device rd;
mt19937 randgen(rd());

vector<matrix_t> generate_nn(vector<int> layer_sizes)
{
    vector<matrix_t> m(layer_sizes.size());
    for (int layer_i = 0; layer_i < layer_sizes.size(); layer_i++) {
        if (layer_i == 0) {
            m[layer_i].resize(layer_sizes[layer_i]);
        } else {
            m[layer_i] = matrix_t(layer_sizes[layer_i], vector_t(layer_sizes[layer_i - 1] + 1));
        }
    }
    return m;
}

vector<matrix_t> set_nn_random_weights(vector<matrix_t> m)
{
    for (auto& layer : m) {
        for (auto& neruon : layer) {
            for (auto& w : neruon) {
                uniform_real_distribution<double> distr(-2.0, 2.0);
                w = distr(randgen);
            }
        }
    }
    return m;
}

vector<matrix_t> generate_random_nn(vector<int> layer_sizes)
{
    vector<matrix_t> m = generate_nn(layer_sizes);
    return set_nn_random_weights(m);
}
vector<matrix_t> teach_nn(vector<vector_t> training_set, vector<matrix_t> m, function<double(double)> activation_f)
{
    auto best_so_far = m;
    for (int i = 0; i < 10000000; i++) {
        m = set_nn_random_weights(m);
        auto prev_cost = loss_function(training_set, best_so_far, activation_f);
        auto current_cost = loss_function(training_set, m, activation_f);
        if (current_cost < prev_cost) {
            cout << i << " " << prev_cost << " " << current_cost << endl;
            best_so_far = m;
            if (current_cost == 0) {
                cout << "FOUND AT ITERATION " << i << endl;
                break;
            }
        }
        // cout << "cost = " << result << endl;
    }
    return best_so_far;
}


vector<vector_t> training_set = {
    {0, 0, 0},
    {1, 0, 0},
    {1, 1, 1},
    {0, 1, 0}};
// vector<vector_t> training_set = {
//     {0, 0, 0},
//     {1, 0, 1},
//     {1, 1, 0},
//     {0, 1, 1}};

int main(int argc, char** argv)
{
    if (argc < 3) {
        cerr << "Podaj 2 argumenty do funkcji xor" << endl;
        return -1;
    }

    vector<matrix_t> m = generate_random_nn({2, 2, 1});

    auto activation_f = unipolar_f;

    m = teach_nn(training_set, m, activation_f);
//m ={
//{ { },{ } },
//{ {  -1.66308, -1.20361, 1.6106},{  -1.52649, -1.77215, 1.70358} },
//{ {  -1.81891, -1.62669, -0.384691} }};
    auto a = generate_activations_with_input(m, {stof(argv[1]), stof(argv[2])});

    int layer_i = 0;
    cout << "{\n";
    for (auto m_ : m) {
        cout << ((layer_i++) ? ",\n" : "") << m_;
    }
    cout << "}" << endl;
    for (auto a_ : a)
        cout << "aktywacja: " << a_;
    cout << "------------------\n";
    a = feed_forward(m, a, activation_f);
    for (auto m_ : m)
        cout << m_;
    for (auto a_ : a)
        cout << a_;
    cout << "wynik " << a[0][0] << " xor " << a[0][1] << " = " << a[2][0] << endl;

    return 0;
}