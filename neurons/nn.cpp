/**
 * Example from lecture: NAI 2019
 * Tadeusz Puźniakowski
 * */
#include <iostream>
#include <vector>
#include <memory>
#include <functional>

// do not use this in production code unless you know what you are doing:
using namespace std;

// todo:
// artificial neuron
// network
// (hopefully) simple learning

class neuron_t {
    public:
    vector<double> w;
    vector<shared_ptr<neuron_t>> x;
    function<double()> activation;
    neuron_t(){
        w = {1.0};
        x = {nullptr};
        activation =[](){return 1.0;};
    }
};


int main() {
    auto bias_neuron = make_shared<neuron_t>();
    auto input0 = make_shared<neuron_t>();
    input0->activation = [](){return -0.5;};
    auto input1 = make_shared<neuron_t>();
    input1->activation = [](){return -0.1;};

    auto neuron0 = make_shared<neuron_t>();
    neuron0->w= {0.5, 0.9, 0.1};
    neuron0->x= {bias_neuron, input0,input1};
    neuron0->activation = [=](){
        double s = 0;
        for (int i = 0; i < neuron0->w.size(); i++) 
            s += neuron0->w[i]*neuron0->x[i]->activation();
        return s;
        //if (s > 0.0) return 1.0;
        //else return 0.0;
    };
   // auto neuron1 = make_shared<neuron_t>();
    cout << neuron0->activation() << endl;
    return 0;
}