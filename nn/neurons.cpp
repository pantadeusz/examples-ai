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
        }; ///< funkcja aktywacji
    }
};


int main()
{
    neuron_c neuron(0.0, 0.0);


    return neuron.get_sigma(0);
}