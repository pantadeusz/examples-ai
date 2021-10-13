#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <functional>




using namespace std;

void wywolujemy(function<void()> f) {
    f();
}

int main()
{
    using namespace std;

    int x = 10;
    auto mojafunkcja = [&](){cout << x << endl;};    
    x = 30;
    wywolujemy(mojafunkcja);
   return 0;
}