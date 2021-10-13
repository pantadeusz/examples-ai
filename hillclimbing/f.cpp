
#include <functional>
#include <iostream>
#include <memory>
#include <tuple>
using namespace std;
void wykonujmy(std::function<void()> funkcyja, int n)
{
    for (int i = 0; i < n; i++)
        funkcyja();
}

auto fabryka = [](int x_){
    using namespace std;
    auto  x = make_shared<int>(x_);
    return [=]() {*x = *x+1;cout << *x << endl; };
};

auto para = [](){
    return make_pair(10,std::string("xyz"));
};

pair<int,string> para2(){
    return {10,"xyzXXX"};
};


int main(int argc, char** argv)
{
    using namespace std;
    int x = 123;
    auto f = fabryka(10);
    cout << "wyniki:" << endl;
    wykonujmy(f, 3);
    auto [a,b] = para2();
    cout << a << " : " << b << endl;
    return 0;
}
