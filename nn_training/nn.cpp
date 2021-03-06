/**
 * przykładowa implementacja warstwowej sieci neuronowej. Sieć nie obsługuje
 * rekurencji. Jest klasycznym rodzajem sieci neuronowej.
 * */

/**
 * Przykład uczenia dla kursu Bitcoina
 * 
 * ./a.out -o btcusd_trained.csv -dot btcusd_graphs -i 100000 btcusd_network_structure.csv btcusd_training.csv > progress.txt
 * 
 * */

#include "dot_print.hpp"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <vector>

using namespace std;
random_device r_device;
mt19937 rand_gen(r_device());

using vector_t = vector<double>;
using matrix_t = vector<vector_t>;

vector_t operator*(const matrix_t& m, const vector_t& v)
{
    vector_t result(m.size());
    for (int i = 0; i < m.size(); i++) { // przechodzimy po wierszach
        double s = 0;
        for (int j = 0; j < v.size();
             j++) { // przechodzimy po kolumnach i elementach wektora
            s += m.at(i).at(j) * v.at(j);
        }
        result[i] = s;
    }
    return result;
}
vector_t operator+(const vector_t& a, const vector_t& b)
{
    vector_t ret;
    for (int i = 0; i < a.size(); i++) {
        ret[i] = a.at(i) + b.at(i);
    }
    return ret;
}

ostream& operator<<(ostream& o, const vector_t& row)
{
    string row_str;
    for (auto e : row) {
        row_str += ((row_str.size() > 0) ? "," : "") + to_string(e);
    }
    o << row_str << "\n";
    return o;
}

ostream& operator<<(ostream& o, const matrix_t& m)
{
    for (auto row : m) {
        o << row;
    }
    return o;
}

ostream& operator<<(ostream& output,
    const vector<matrix_t>& nnetwork_matrices)
{
    vector<vector<double>> towrite;
    for (int i = 1; i < nnetwork_matrices.size(); i++) {
        towrite.push_back({(double)(nnetwork_matrices[i].size())});
        for (auto r : nnetwork_matrices[i])
            towrite.push_back(r);
    }
    output << towrite;
    return output;
}

// obliczenie wartości dla kolejnych warstw sieci neuronowej.
// Sieć jest reprezentowana przez
// m - macierze wag poszczególnych warstw,
// oraz a - wektory wartości wzbudzenia poszczególnych neuronów
// uwaga - a[*] nie zawiera biasu!!
vector<vector_t> feed_forward(const vector<matrix_t>& m,
    const vector<vector_t>& a, // bez 1 na pozycji bias
    function<double(double)> f)
{
    vector<vector_t> result = a;

    // przechodzimy po warstwach
    for (int i = 1; i < a.size(); i++) {
        // wejscia
        auto a_bias = result.at(i - 1);
        // przygotowany input z biasem
        // a_bias.insert(a_bias.begin(), 1.0);
        a_bias.push_back(1.0);
        // teraz dla kazdego neuronu
        for (int neuron_i = 0; neuron_i < m.at(i).size(); neuron_i++) {
            double sum = 0.0;
            for (int n_input_i = 0; n_input_i < m.at(i).at(neuron_i).size();
                 n_input_i++) {
                sum += m.at(i).at(neuron_i).at(n_input_i) * a_bias.at(n_input_i);
            }
            result.at(i).at(neuron_i) = f(sum);
        }
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
każdy wiersz to jeden wektor. Wiersz taki odpowia wagom dla kolejnych wejść
neuronu. W powyższym przykładzie są 4 neurony, natomiast każdy z nich ma 2
wejścia z poprzedniej warstwy oraz wagę biasu.

*/
pair<vector<matrix_t>, vector<vector_t>>
generate_nn_layes(vector<int> sizes, function<double(int l, int i, int j)> g)
{
    vector<matrix_t> m = {{}}; // wektor macierzy wag dla kolejnych warstw
    vector<vector_t> a = {};   // wektor wzbudzenia neuronów na kolejnych warstwach

    // wielkości kolejnych warstw - liczby neuronów. 0 - warstwa wejściowa
    for (int l = 0; l < sizes.size(); l++) {
        a.push_back(
            vector_t(sizes[l])); // tworzymy wartosci wzbudzenia neuronow dla warstw
        if (l > 0)               // l - layer - warstwa ukryta lub wyjściowa ma indeks > 0
        {
            // dodajemy 1 poniewaz musimy uwzglednic wage biasu
            matrix_t m_l = matrix_t(sizes[l], vector_t(sizes[l - 1] + 1));
            for (int i = 0; i < m_l.size(); i++) {
                for (int j = 0; j < m_l[i].size(); j++) {
                    m_l[i][j] = g(l, i, j);
                }
            }
            m.push_back(m_l);
        }
    }
    return {m, a};
}

// pobiera warości wejść dla sieci neuronowej, zwraca wartości wyjść sieci
// neuronowej UWAGA: inputs nie zawiera biasu. Bias jest zawsze ustawiony na 1.0
// dla wszystkich warstw.
vector<vector_t> calculate_result_from_network(
    const vector_t& inputs,
    const pair<vector<matrix_t>, const vector<vector_t>>& neural_network,
    function<double(double)> activation_function)
{
    vector<vector_t> n_activations = neural_network.second;
    for (int i = 0; i < inputs.size(); i++) {
        n_activations.at(0).at(i) = inputs.at(i);
    }
    auto a_out =
        feed_forward(neural_network.first, n_activations, activation_function);
    return move(a_out); // return whole network
}

/// dla danych historycznych stwierdzic czy kupujemy czy sprzedajemy
/// wejscia: 9 wejsc // srednia dzienna
/// wyjscie 0 - sprzedaj, 1 - kupuj

// funkcja oblicza jak bardzo siec neuronowa sie myli. Należy do tego podejść
// statystycznie, więc wektor evaluation_set powinien byc jak największy, ale
// oczywiście w granicach możliwości obliczeniowych im wyższa wartość, tym
// gorzej działa nasza sieć neuronowa. Należy pamiętać o ustaleniu funkcji
// aktywacji
//                        znane_wejscia     poprawne_wyjscia
double cost_function(
    const vector<pair<vector_t, vector_t>>& evaluation_set,
    const pair<vector<matrix_t>, const vector<vector_t>>& neural_network,
    function<double(double)> activation_function)
{
    double cost = 0;
    for (auto [inputs, outputs] : evaluation_set) {
        // get the result from network
        auto outputs_calculated = calculate_result_from_network(
            inputs, neural_network, activation_function);
        // see what is the result
        for (int i = 0; i < outputs.size(); i++) {
            double d = abs(outputs_calculated.back().at(i) - outputs.at(i));
            cost += d;
        }
    }
    return cost;
}

/**
 * Szybki sposób na prostą konwersję pliku csv na zbiór ciągów znaków w tabeli
 * */
vector<vector<string>> csv_to_vectors(istream& input)
{
    vector<vector<string>> ret_table;
    for (string line; getline(input, line);) {
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
vector<vector<double>>
csv_strings_to_doubles(const vector<vector<string>>& input)
{
    vector<vector<double>> ret_tab;
    for (int row = 0; row < input.size(); row++) {
        ret_tab.push_back({});
        for (int col = 0; col < input[row].size(); col++) {
            try {
                ret_tab.back().push_back(stod(input[row][col]));
            } catch (const exception& e) {
                ret_tab.back().push_back(0.0);
                cerr << "error converting cell: " << to_string(row) << ":"
                     << to_string(col) << ":" << e.what() << endl;
            }
        }
    }
    return ret_tab;
}

/*
przekształcenie dwuwymiarowej tabeli w tabelę z nagłówkiem
*/
pair<vector<string>, vector<vector<double>>>
csv_strings_to_table_with_headers(const vector<vector<string>>& input)
{
    return {input[0], csv_strings_to_doubles({input.begin() + 1, input.end()})};
}

void save_csv(ostream& output,
    const pair<vector<string>, vector<vector<double>>>& csv_to_save)
{
    for (int i = 0; i < csv_to_save.first.size(); i++)
        output << ((i == 0) ? "" : ",") << csv_to_save.first[i];
    output << "\n";
    output << csv_to_save.second;
}

pair<vector<matrix_t>, vector<vector_t>> load_nn_from_csv(istream& input)
{
    auto input_data = csv_strings_to_doubles(csv_to_vectors(input));
    vector<matrix_t> ret_matrix(1); // first is empty
    vector<vector_t> ret_neurons(1);
    // only hidden and output layers
    for (int row = 0; row < input_data.size();) {
        int neurons = input_data.at(row).at(0); // najpierw ile ma byc
        row++;
        matrix_t layer;
        for (; layer.size() < neurons; row++) {
            layer.push_back(input_data[row]);
        }
        ret_neurons.back().resize(layer.back().size() - 1);
        ret_matrix.push_back(layer);
        ret_neurons.push_back({});
    }
    ret_neurons.back().resize(ret_matrix.back().size());
    return {ret_matrix, ret_neurons};
}

void save_nn_to_dot(ostream& output, const vector<matrix_t>& nnetwork_matrices, const vector<vector_t>& activations)
{
    vector<vector<double>> towrite;
    /*
digraph G {
a -> b [ label="a to b" ];
b -> c [ label="another label"];
}
*/
    output << "digraph G {" << endl;
    for (int i = 0; i < nnetwork_matrices.size(); i++) {
        if (i == 0) {
            for (int j = 0; j < activations.at(0).size(); j++)
                output << "n_" << i << "_" << j << " [ label=\""
                       << "n_" << i << "_" << j << "(s=" << activations.at(i).at(j)
                       << ")\" ];" << endl;
        } else
            for (int j = 0; j < nnetwork_matrices[i].size(); j++) {
                output << "n_" << i << "_" << j << " [ label=\""
                       << "n_" << i << "_" << j << "(s=" << activations.at(i).at(j)
                       << ", b=" << nnetwork_matrices[i][j].back() << ")\" ];" << endl;
                for (int w_i = 0; w_i < nnetwork_matrices[i][j].size() - 1; w_i++) {
                    output << "n_" << (i - 1) << "_" << (w_i) << " -> n_" << i << "_" << j
                           << " [ label=\"" << nnetwork_matrices[i][j][w_i] << "\" ];"
                           << endl;
                }
            }
    }
    output << "}" << endl;
}
/**
 * activation functions
 * */
function<double(double)> unipolar = [](double x) { return (x > 0) ? 1.0 : 0.0; };
function<double(double)> fermi = [](double x) { return 1.0 / (1.0 + exp(-x)); };

vector<matrix_t> nn_randomize(const vector<matrix_t>& nn,
    double range_min = -10,
    double range_max = 10)
{
    auto m = nn;
    uniform_real_distribution<double> d(range_min, range_max);
    for (auto& current_m : m) {
        for (auto& current_row : current_m) {
            for (double& current_w : current_row) {
                current_w = d(rand_gen);
            }
        }
    }
    return move(m);
}

vector<matrix_t> nn_random_change(const vector<matrix_t>& nn,
    double dr = 0.01)
{
    auto m = nn;
    normal_distribution<double> d(0.0, dr);
    for (auto& current_m : m) {
        for (auto& current_row : current_m) {
            for (double& current_w : current_row) {
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

    int result_columns =
        m.back().size(); // automatycznie dobieramy liczbe kolumn z wynikami

    vector<pair<vector_t, vector_t>> evaluation_set;
    // dane uczace
    for (auto training_set_row : input_table) {
        evaluation_set.push_back(
            {{training_set_row.begin(), training_set_row.end() - result_columns},
                {training_set_row.end() - result_columns, training_set_row.end()}});
    }

    //    auto best_so_far = m; // copy current best
    double m_cost = cost_function(evaluation_set, {m, a}, activation_function);
    cout << "all_cost: " << m_cost << endl;


    #pragma omp parallel for
    // private(m,a,evaluation_set,activation_function)
    for (int iteration = 0; iteration < iterations; iteration++) {
        auto mn = nn_randomize(m, -10, 10);
        double mn_current_cost =
            cost_function(evaluation_set, {mn, a}, activation_function);
        {
            #pragma omp critical
            if (mn_current_cost < m_cost) {
                cout << "prev_cost " << m_cost << " -> ";
                m_cost = mn_current_cost;
                m = mn;
                // save_nn_to_csv(cout, best_so_far);
                cout << "current_cost : " << m_cost << endl;
            }
        }
    }

    return m;
}


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
            if (uk(rand_gen) < exp(-abs(goal(next_sol) - goal(current_solution)) / T(iteration)))
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


vector<matrix_t> training_nn_sa(vector<matrix_t> m, vector<vector_t> a, function<double(double)> activation_function, string training_set, int iterations)
{
    // zaladujmy plik csv do uczenia naszej sieci
    std::ifstream input(training_set);
    auto input_table = csv_strings_to_doubles(csv_to_vectors(input));

    int result_columns =
        m.back().size(); // automatycznie dobieramy liczbe kolumn z wynikami

    vector<pair<vector_t, vector_t>> evaluation_set;
    // dane uczace
    for (auto training_set_row : input_table) {
        evaluation_set.push_back(
            {{training_set_row.begin(), training_set_row.end() - result_columns},
                {training_set_row.end() - result_columns, training_set_row.end()}});
    }
    m = nn_randomize(m, -10, 10);

    //    auto best_so_far = m; // copy current best
    double m_cost = cost_function(evaluation_set, {m, a}, activation_function);
    cout << "all_cost: " << m_cost << endl;

simulated_annealing(
  [&](){return nn_randomize(m, -10, 10);}, 
[&](auto mat){return nn_random_change(mat,0.1);}, 
[&](auto mat){return cost_function(evaluation_set, {mat, a}, activation_function);}, 
[&](auto i) {return 200000.0/i;}, iterations,
[&](auto i, auto sol){
  cout << i << " -> " <<  cost_function(evaluation_set, {sol, a}, activation_function) << endl;
});
    return m;
}




void save_nn_for_input_graphviz_dot(const string& fname_prefix,
    const vector_t& inputs,
    vector_t outputs_true,
    const vector<matrix_t>& m,
    vector<vector_t> a_calculated)
{
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
    for (double tsr : a_calculated[0])
        fname = fname + "_" + to_string(tsr);
    fname = fname + ".dot";
    ofstream out_dot(fname);
    save_nn_to_dot(out_dot, m, a_calculated);
    out_dot.close();
}

void verify_nn(vector<matrix_t> m, vector<vector_t> a, function<double(double)> activation_function, string training_set, string save_graphviz = "")
{
    // zaladujmy plik csv do uczenia naszej sieci
    std::ifstream input(training_set);
    auto input_table = csv_strings_to_doubles(csv_to_vectors(input));

    int result_columns =
        m.back().size(); // automatycznie dobieramy liczbe kolumn z wynikami
    double diff_sum = 0.0;
    for (auto training_set_row : input_table) {
        vector_t inputs = {training_set_row.begin(),
            training_set_row.end() - result_columns};
        vector_t outputs_true = {training_set_row.end() - result_columns,
            training_set_row.end()};
        auto a_calculated =
            calculate_result_from_network(inputs, {m, a}, activation_function);
        for (int i = 0; i < a_calculated.back().size(); i++) {
            diff_sum += abs(a_calculated.back().at(i) - outputs_true.at(i));
        }
        if (save_graphviz.size()) {
            save_nn_for_input_graphviz_dot(save_graphviz, inputs, outputs_true, m,
                a_calculated);
        }
    }
    cout << "verification: " << diff_sum << endl;
}

int help_txt(int argc, char** argv)
{
    if (argc < 3) {
        cerr << "Usage - training:" << endl;
        cerr << argv[0] << " "
             << "[-o output] [-dot dot_filename_prefix'] [-i iterations] "
                "network_topology training_or_test_set"
             << endl;
        cerr << "Example:" << endl;
        cerr << argv[0] << " "
             << "-o xxx.csv -dot xxx -i 1000000 nn.csv xor_set.csv" << endl;
        cerr << "Usage - use trained network:" << endl;
        cerr << argv[0] << " "
             << "-x network inputs"
             << endl;
        cerr << "Example:" << endl;
        cerr << argv[0] << " "
             << "-x xxx.csv xor_input.csv" << endl;

        return -1;
    }
    return 0;
}


int nn_training(list<string> args, function<double(double)> activation_function)
{
    string training_set_filename = args.back();
    args.pop_back();
    string input_neural_network_name = args.back();
    args.pop_back();

    // m - macierze kolejnych wag dla kolejnych warstw
    /*auto [m, a] =  generate_nn_layes({2, 2, 1}, [](int l, int i, int j) {
        uniform_real_distribution<double> d(-1.0, 1.0);
        return d(rand_gen);
    }); //  load_nn_from_csv(cin);
*/
    std::ifstream nn_file(input_neural_network_name);
    auto [m, a] = load_nn_from_csv(nn_file); //.first;

    auto otput_nn_fname = std::find(args.begin(), args.end(), string("-o"));
    if (otput_nn_fname != args.end()) {
        auto iterations_s = std::find(args.begin(), args.end(), string("-i"));
        int iterations = 10000000;
        if (iterations_s != args.end()) {
            iterations_s++;
            iterations = stoi(*iterations_s);
        }
        vector<matrix_t> best_so_far = training_nn(
//        vector<matrix_t> best_so_far = training_nn_sa(
              m, a, activation_function, training_set_filename, iterations);

        otput_nn_fname++;
        if (*otput_nn_fname == "-") {
            cout << best_so_far;
        } else {
            ofstream f(*otput_nn_fname);
            f << best_so_far;
        }
        m = best_so_far;
    } else {
        cout << m;
    }

    auto graph_viz_fname = std::find(args.begin(), args.end(), string("-dot"));
    if (graph_viz_fname != args.end()) {
        graph_viz_fname++;
        verify_nn(m, a, activation_function, training_set_filename,
            *graph_viz_fname);
    } else {
        verify_nn(m, a, activation_function, training_set_filename);
    }
    return 0;
}
int nn_execution(string input_nn_fn, string input_data_fn, function<double(double)> activation_function)
{
    std::ifstream nn_file(input_nn_fn);
    std::ifstream data_file(input_data_fn);
    pair<vector<matrix_t>, vector<vector_t>> full_nn;
    full_nn = load_nn_from_csv(nn_file);
    auto& m = full_nn.first;
    auto& a = full_nn.second;
    auto inputs_from_file = csv_strings_to_doubles(csv_to_vectors(data_file));

    for (auto inputs : inputs_from_file) {
        for (int i = 0; i < inputs.size(); i++) {
            cout << inputs[i] << " ";
        }
        cout << "->";
        // get the result from network
        auto outputs_calculated = calculate_result_from_network(
            inputs, full_nn, activation_function);
        // see what is the result
        for (int i = 0; i < outputs_calculated.back().size(); i++) {
            cout << " " << outputs_calculated.back().at(i);
        }
        cout << endl;
    }


    return 0;
}

int main(int argc, char** argv)
{
    if (help_txt(argc, argv) != 0) return -1;

    function<double(double)> activation_function;
    //activation_function = fermi;
    activation_function = unipolar;


    list<string> args(argv + 1, argv + argc);
    if (args.front() == "-x") {
        args.pop_front();
        return nn_execution(args.front(), args.back(), activation_function);
    } else {
        return nn_training(args, activation_function);
    }
}