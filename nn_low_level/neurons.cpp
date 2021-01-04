#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <random>

using namespace std;
random_device r_device;
mt19937 rand_gen(r_device());

class neuron_i
{
public:
    /// w[0] oznacza bias
    vector<double> w;                    ///< wagi
    vector<double> h;                    ///< przebieg czasowy pola lokalnego (wartość od t)
    vector<shared_ptr<neuron_i>> inputs; ///< neurony wejściowe

    virtual double get_sigma(int t) = 0;
    virtual void update() = 0;

    int get_t() { return h.size(); } ///< pobiera aktualny t (czyli ostatni policzony)
};

class const_neuron : public neuron_i
{
public:
    double get_sigma(int t) { return h.at(0); } // const
    void update() { h.push_back(h.back()); }
    const_neuron(double v = 1.0) { h.push_back(v); }
};

class neuron_c : public neuron_i
{
public:
    function<double(double)> activation_f;

    double get_sigma(int t) { return activation_f(h.at(t)); }

    void update()
    {
        int t = get_t();
        if (t <= 0)
            throw "Brakuje stanu początkowego";
        double s = 0;
        for (int i = 0; i < w.size(); i++)
        {
            s += w.at(i) * inputs.at(i)->get_sigma(t - 1);
        }
        h.push_back(s);
    }

    neuron_c(double bias, double value_0)
    {
        h.push_back(value_0); ///< aktualna wartość pola lokalnego
        w.push_back(bias);
        inputs.push_back(make_shared<const_neuron>(1.0)); ///< bias
        double k = 1.0, L = 1.0, x0 = 0;
        activation_f = [k, L, x0](double v) {
            return 1.0 / (1.0 + exp(-k * (v - x0)));
        }; //[](double v) { return (v > 0) ? 1.0 : 0.0; }; ///< funkcja aktywacji
    }
};

vector<vector<shared_ptr<neuron_i>>> generate_network_one_dir(vector<int> layers_sizes)
{
    uniform_real_distribution<double> dist(-1.0, 1.0);
    vector<shared_ptr<neuron_i>> input_layer;
    vector<vector<shared_ptr<neuron_i>>> ret_network;
    for (int i = 0; i < layers_sizes.at(0); i++)
        input_layer.push_back(make_shared<const_neuron>(0.0)); // warstwa wejsciowa
    ret_network.push_back(input_layer);
    /// tworzymy warstwy
    for (int l = 1; l < layers_sizes.size(); l++)
    {
        vector<shared_ptr<neuron_i>> hidden_layer;
        // dla kazdej warstwy tworzymy odpowiednia liczbe neuronow
        for (int i = 0; i < layers_sizes.at(l); i++)
        {
            hidden_layer.push_back(make_shared<neuron_c>(1.0, 0.0)); // warstwa ukryta
            // podpinamy wejscia tych neuronow do neuronow z warstwy poprzedniej
            for (int c = 0; c < layers_sizes.at(l - 1); c++)
            {
                hidden_layer.back()->inputs.push_back(ret_network.at(l - 1).at(c));
                hidden_layer.back()->w.push_back(dist(rand_gen));
            }
        }
        ret_network.push_back(hidden_layer);
    }
    return ret_network;
}

auto calculate_nn = [](auto nn) {
    for (auto &e : nn)
    {
        e->update();
    }
};

vector<shared_ptr<neuron_i>> to_one_dim(vector<vector<shared_ptr<neuron_i>>> e)
{
    vector<shared_ptr<neuron_i>> flatter;
    for (auto &i : e)
    {
        flatter.insert(flatter.end(), i.begin(), i.end());
    }
    return flatter;
};

int main()
{
    auto nn = generate_network_one_dir({5, 4, 1});
    auto od = to_one_dim(nn);
    nn.at(0).at(0)->h[0] = 0.7;
    for (int t = 0; t < 3; t++)
    {
        for (auto layer : nn)
        {
            for (auto n : layer)
            {
                cout << n->get_sigma(t) << " ";
            }
            cout << endl;
        }
        calculate_nn(od);
    }
    //    neuron_c neuron(0.0, 0.0);

    //    return neuron.get_sigma(0);
    return 0;
}