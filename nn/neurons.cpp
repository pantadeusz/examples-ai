#include <vector>
#include <memory>
#include <functional>

using namespace std;

class neuron_i
{
public:
    virtual double get_sigma(int t) = 0;
};

class const_neuron : public neuron_i
{
public:
    double value;
    double get_sigma(int t) { return value; }
    const_neuron(double v = 1.0) { value = v; }
};

class neuron_c : public neuron_i
{
public:
    /// w[0] oznacza bias
    vector<double> w;                   ///< wagi
    vector<double> h;                   ///< przebieg czasowy pola lokalnego
    vector<shared_ptr<neuron_i>> sigma; ///< neurony wejściowe

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
            s += w.at(i) * sigma.at(i)->get_sigma(t - 1);
        }
        h.push_back(s);
    }
    int get_t() { return h.size(); }

    neuron_c(double bias, double value_0)
    {
        w.push_back(bias);
        h.push_back(value_0);
        sigma.push_back(make_shared<const_neuron>(1.0));
        activation_f = [](double v){return (v>0)?1.0:0.0;};
    }
};

int main()
{
    neuron_c neuron(0.0, 0.0);
    return neuron.get_sigma(0);
}