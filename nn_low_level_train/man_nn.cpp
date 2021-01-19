/**
 * przykładowa implementacja warstwowej sieci neuronowej. Sieć nie obsługuje rekurencji. Jest klasycznym rodzajem sieci neuronowej.
 * */

#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>

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

/*

siec neuronowa ma taką konstrukcję, że mamy tablicę macierzy wag, 
gdzie każda macierz odpowiada jenden warstwie, 
oraz mamy jedną pustą macierz dla warstwy 0, ponieważ jest to warstwa wejściowa. Ona nie ma żadnych wejść

Każda warstwa reprezentowana przez macierz ma tyle wierszy ile jest neuronów w tej warstwie, natomiast tyle kolumn, ile jest wejść + 1, ponieważ uwzględniamy wagę biasu.
*/
vector_t activate(
    const vector_t &a, function<double(double)> f)
{
    vector_t result = a;
    for (auto &e : result)
    {
        e = f(e);
    }
    return result;
}

// obliczenie wartości dla kolejnych warstw sieci neuronowej.
// Sieć jest reprezentowana przez
// m - macierze wag poszczególnych warstw,
// oraz a - wektory wartości wzbudzenia poszczególnych neuronów
// uwaga - a[*] nie zawiera biasu!!
vector<vector_t> feed_forward(
    const vector<matrix_t> &m, const vector<vector_t> &a,
    function<double(double)> f)
{
    vector<vector_t> result = a;
    // przechodzimy po warstwach
    for (int i = 1; i < a.size(); i++)
    {
        // wejscia
        auto a_bias = result.at(i - 1);
        // przygotowany input z biasem
        //a_bias.insert(a_bias.begin(), 1.0);
        a_bias.push_back(1.0);
        // teraz dla kazdego neuronu
        for (int neuron_i = 0; neuron_i < m.at(i).size(); neuron_i++)
        {
            double sum = 0.0;
            for (int n_input_i = 0; n_input_i < m.at(i).at(neuron_i).size(); n_input_i++)
            {
                sum += m.at(i).at(neuron_i).at(n_input_i) * a_bias.at(n_input_i);
            }
            result.at(i).at(neuron_i) = f(sum);
        }
        //        a_bias.push_back(1.0); // bias jest zawsze 1. Liczba wag macierzy wag jest większa o 1
        //        result[i] = activate(m.at(i) * a_bias, f);
    }
    return result;
}

/*
Tworzy sieć neuronową, warstwową, reprezentowaną w następujący sposób:

każda warstwa sieci jest opisana poprzez macierz wag, wyglądająca tak:
[ a a a ] <- dla neuronu 0 danej warstwy
[ a a a ] <- dla neuronu 1 danej warstwy
[ a a a ] <- dla neuronu 2 danej warstwy
[ a a a ] <- dla neuronu 3 danej warstwy
każdy wiersz to jeden wektor. Wiersz taki odpowia wagom dla kolejnych wejść neuronu. W powyższym przykładzie są 4 neurony, natomiast każdy z nich ma 2 wejścia z poprzedniej warstwy oraz wagę biasu.

*/
pair<vector<matrix_t>, vector<vector_t>> generate_nn_layes(vector<int> sizes, function<double(int l, int i, int j)> g)
{
    vector<matrix_t> m = {{}}; // wektor macierzy wag dla kolejnych warstw
    vector<vector_t> a = {};   // wektor wzbudzenia neuronów na kolejnych warstwach

    // wielkości kolejnych warstw - liczby neuronów. 0 - warstwa wejściowa
    for (int l = 0; l < sizes.size(); l++)
    {
        a.push_back(vector_t(sizes[l])); // tworzymy wartosci wzbudzenia neuronow dla warstw
        if (l > 0)                       // l - layer - warstwa ukryta lub wyjściowa ma indeks > 0
        {
            // dodajemy 1 poniewaz musimy uwzglednic wage biasu
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

// pobiera warości wejść dla sieci neuronowej, zwraca wartości wyjść sieci neuronowej
// UWAGA: inputs nie zawiera biasu. Bias jest zawsze ustawiony na 1.0 dla wszystkich warstw.
vector<vector_t> calculate_result_from_network(const vector_t &inputs,
                                               const pair<vector<matrix_t>, const vector<vector_t>> &neural_network,
                                               function<double(double)> activation_function)
{
    vector<vector_t> n_activations = neural_network.second;
    for (int i = 0; i < inputs.size(); i++)
    {
        n_activations.at(0).at(i) = inputs.at(i);
    }
    //    for (auto &act_layer : n_activations)
    //        act_layer.at(0) = 1.0; // zawsze ustawiamy bias na 1.
    auto a_out = feed_forward(neural_network.first, n_activations, activation_function);
    //for (auto l : n_activations)
    //{
    //    for (auto e : l)
    //    {
    //        cout << e << " ";
    //    }
    //    cout << endl;
    //}
    return move(a_out); // return whole network
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
    const pair<vector<matrix_t>, const vector<vector_t>> &neural_network,
    function<double(double)> activation_function)
{
    double cost = 0;
    for (auto [inputs, outputs] : evaluation_set)
    {
        // get the result from network
        auto outputs_calculated = calculate_result_from_network(inputs, neural_network, activation_function);
        // see what is the result
        for (int i = 0; i < outputs.size(); i++)
        {
            double d = abs(outputs_calculated.back().at(i) - outputs.at(i));
            cost += d;
        }
    }
    return cost;
}

/**
 * Szybki sposób na prostą konwersję pliku csv na zbiór ciągów znaków w tabeli
 * */
vector<vector<string>> csv_to_vectors(istream &input)
{
    vector<vector<string>> ret_table;
    for (string line; getline(input, line);)
    {
        stringstream l(line);
        ret_table.push_back({});
        for (string cell; getline(l, cell, ',');)
            ret_table.back().push_back(cell);
    }
    return ret_table;
}

/*
przekształcenie dwuwymiarowej tabeli w tabelę z nagłówkiem
*/
vector<vector<double>> csv_strings_to_doubles(const vector<vector<string>> &input)
{
    vector<vector<double>> ret_tab;
    for (int row = 0; row < input.size(); row++)
    {
        ret_tab.push_back({});
        for (int col = 0; col < input[row].size(); col++)
        {
            try
            {
                ret_tab.back().push_back(stod(input[row][col]));
            }
            catch (const exception &e)
            {
                ret_tab.back().push_back(0.0);
                cerr << "error converting cell: " << to_string(row) << ":" << to_string(col) << ":" << e.what() << endl;
            }
        }
    }
    return ret_tab;
}

/*
przekształcenie dwuwymiarowej tabeli w tabelę z nagłówkiem
*/
pair<vector<string>, vector<vector<double>>> csv_strings_to_table_with_headers(const vector<vector<string>> &input)
{
    return {input[0], csv_strings_to_doubles({input.begin() + 1, input.end()})};
}

void save_csv(ostream &output, const vector<vector<double>> &csv_to_save)
{
    for (auto &row : csv_to_save)
    {
        for (int i = 0; i < row.size(); i++)
            output << ((i == 0) ? "" : ",") << row[i];
        output << "\n";
    }
}

void save_csv(ostream &output, const pair<vector<string>, vector<vector<double>>> &csv_to_save)
{
    for (int i = 0; i < csv_to_save.first.size(); i++)
        output << ((i == 0) ? "" : ",") << csv_to_save.first[i];
    output << "\n";
    save_csv(output, csv_to_save.second);
}

pair<vector<matrix_t>, vector<vector_t>> load_nn_from_csv(istream &input)
{
    auto input_data = csv_strings_to_doubles(csv_to_vectors(input));
    vector<matrix_t> ret_matrix(1); // first is empty
    vector<vector_t> ret_neurons(1);
    // only hidden and output layers
    for (int row = 0; row < input_data.size();)
    {
        int neurons = input_data.at(row).at(0); // najpierw ile ma byc
        row++;
        matrix_t layer;
        for (; layer.size() < neurons; row++)
        {
            layer.push_back(input_data[row]);
        }
        ret_neurons.back().resize(layer.back().size() - 1);
        ret_matrix.push_back(layer);
        ret_neurons.push_back({});
    }
    ret_neurons.back().resize(ret_matrix.back().size());
    return {ret_matrix, ret_neurons};
}

void save_nn_to_csv(ostream &output, vector<matrix_t> &nnetwork_matrices)
{
    vector<vector<double>> towrite;
    for (int i = 1; i < nnetwork_matrices.size(); i++)
    {
        towrite.push_back({(double)(nnetwork_matrices[i].size())});
        for (auto r : nnetwork_matrices[i])
            towrite.push_back(r);
    }
    save_csv(output, towrite);
}

void save_nn_to_dot(ostream &output, const vector<matrix_t> &nnetwork_matrices, const vector<vector_t> &activations)
{
    vector<vector<double>> towrite;
    /*
digraph G {
 a -> b [ label="a to b" ];
 b -> c [ label="another label"];
}
*/
    output << "digraph G {" << endl;
    for (int i = 0; i < nnetwork_matrices.size(); i++)
    {
        if (i == 0)
        {
            for (int j = 0; j < activations.at(0).size(); j++)
                output << "n_" << i << "_" << j << " [ label=\""
                       << "n_" << i << "_" << j << "(s=" << activations.at(i).at(j) << ")\" ];" << endl;
        }
        else
            for (int j = 0; j < nnetwork_matrices[i].size(); j++)
            {
                output << "n_" << i << "_" << j << " [ label=\""
                       << "n_" << i << "_" << j << "(s=" << activations.at(i).at(j) << ", b=" << nnetwork_matrices[i][j].back() << ")\" ];" << endl;
                for (int w_i = 0; w_i < nnetwork_matrices[i][j].size() - 1; w_i++)
                {
                    output << "n_" << (i - 1) << "_" << (w_i) << " -> n_" << i << "_" << j << " [ label=\"" << nnetwork_matrices[i][j][w_i] << "\" ];" << endl;
                }
            }
    }
    output << "}" << endl;
}

function<double(double)> unipolar = [](double x) {
    if (x > 0)
        return 1.0;
    return 0.0;
};

function<double(double)> fermi = [](double x) {
    return 1.0 / (1.0 + exp(-x));
};

vector<matrix_t> nn_randomize(const vector<matrix_t> &nn, double range_min = -10, double range_max = 10)
{
    auto m = nn;
    uniform_real_distribution<double> d(range_min, range_max);
    for (auto &current_m : m)
    {
        for (auto &current_row : current_m)
        {
            for (double &current_w : current_row)
            {
                current_w = d(rand_gen);
            }
        }
    }
    return move(m);
}

vector<matrix_t> nn_random_change(const vector<matrix_t> &nn, double dr = 0.01)
{
    auto m = nn;
    normal_distribution<double> d(0.0, dr);
    for (auto &current_m : m)
    {
        for (auto &current_row : current_m)
        {
            for (double &current_w : current_row)
            {
                current_w += d(rand_gen);
            }
        }
    }
    return move(m);
}

vector<matrix_t> training_nn(vector<matrix_t> m, vector<vector_t> a, function<double(double)> activation_function, string training_set, int iterations)
{

    // zaladujmy plik csv do uczenia naszej sieci
    std::ifstream input(training_set);
    auto input_table = csv_strings_to_doubles(csv_to_vectors(input));

    int result_columns = m.back().size(); // automatycznie dobieramy liczbe kolumn z wynikami

    vector<pair<vector_t, vector_t>> evaluation_set;
    // dane uczace
    for (auto training_set_row : input_table)
    {
        evaluation_set.push_back({{training_set_row.begin(), training_set_row.end() - result_columns}, {training_set_row.end() - result_columns, training_set_row.end()}});
    }

    cout << "all_cost: " << cost_function(evaluation_set, {m, a}, activation_function) << endl;

    //    auto best_so_far = m; // copy current best
    double m_cost = cost_function(evaluation_set, {m, a}, activation_function);

#pragma omp parallel for
    //private(m,a,evaluation_set,activation_function)
    for (int iteration = 0; iteration < iterations; iteration++)
    {
        auto mn = nn_randomize(m, -10, 10);
        double mn_current_cost = cost_function(evaluation_set, {mn, a}, activation_function);
        {
#pragma omp critical
            if (mn_current_cost < m_cost)
            {
                cout << "prev_cost " << m_cost << " -> ";
                m_cost = mn_current_cost;
                m = mn;
                //save_nn_to_csv(cout, best_so_far);
                cout << "current_cost : " << m_cost << endl;
            }
        }
    }

    return m;
}

void save_nn_for_input_graphviz_dot(const string &fname_prefix, const vector_t &inputs, vector_t outputs_true, const vector<matrix_t> &m,
vector <vector_t> a_calculated
) {
        cout << "[ ";
        for (auto i : inputs)
            cout << i << " ";
        cout << "] ---> [ ";
        for (auto r : a_calculated.back())
            cout << r << " ";
        cout << "] vs [ ";
        for (auto t : outputs_true)
            cout << t << " ";
        cout << "]" << endl;
        string fname = fname_prefix;
        for (int tsr : a_calculated[0])
            fname = fname + "_" + to_string(tsr);
        fname = fname + ".dot";
        ofstream out_dot(fname);
        save_nn_to_dot(out_dot, m, a_calculated);
        out_dot.close();
}

void verify_nn(vector<matrix_t> m, vector<vector_t> a, function<double(double)> activation_function, string training_set)
{

    // zaladujmy plik csv do uczenia naszej sieci
    std::ifstream input(training_set);
    auto input_table = csv_strings_to_doubles(csv_to_vectors(input));

    int result_columns = m.back().size(); // automatycznie dobieramy liczbe kolumn z wynikami

    for (auto training_set_row : input_table)
    {
        vector_t inputs = {training_set_row.begin(), training_set_row.end() - result_columns};
        vector_t outputs_true = {training_set_row.end() - result_columns, training_set_row.end()};
        auto a_calculated = calculate_result_from_network(inputs, {m, a}, activation_function);
        save_nn_for_input_graphviz_dot("g__", inputs, outputs_true, m, a_calculated);
    }
}
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "Usage:" << endl;
        cerr << argv[0] << " "
             << "[network_topology] [training_set]" << endl;
        cerr << "Example:" << endl;
        cerr << argv[0] << " "
             << "nn.csv xor_set.csv" << endl;
        return -1;
    }
    string input_neural_network_name = argv[1];
    string training_set_filename = argv[2];
    function<double(double)> activation_function;
    activation_function = fermi;
    //activation_function = unipolar;
    // m - macierze kolejnych wag dla kolejnych warstw
    auto [m, a] = generate_nn_layes({2, 2, 1}, [](int l, int i, int j) {
        uniform_real_distribution<double> d(-1.0, 1.0);
        return d(rand_gen);
    }); //  load_nn_from_csv(cin);

    std::ifstream nn_file(input_neural_network_name);
    m = load_nn_from_csv(nn_file).first;

    if (argc > 3)
    {
        vector<matrix_t> best_so_far = training_nn(m, a, activation_function, training_set_filename, 10000000);
        if (string(argv[3]) == "-")
        {
            save_nn_to_csv(cout, best_so_far);
        }
        else
        {
            ofstream f(argv[3]);
            save_nn_to_csv(f, best_so_far);
        }
        m = best_so_far;
    }
    save_nn_to_csv(cout, m);
    verify_nn(m, a, activation_function, training_set_filename);

    return 0;
}