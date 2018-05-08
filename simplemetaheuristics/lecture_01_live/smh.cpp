/*

Copyright (c) 2018 Tadeusz Puźniakowski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */


#include <vector>
#include <functional>
#include <list>
#include <iostream>
#include <cmath>
#include <random>

using namespace std;

std::mt19937 r_gen; // generator liczb pseudolosowych


void print_solution( vector < double > s) {
	for (auto e: s) cout << e << " ";
	cout << endl;
}
/**
 * 
 * Algorytm wspinaczkowy w uniwersalnej wersji
 * 
 * @param f funkcja optymalizowana
 * @param init_solution rozwiązanie początkowe - punkt startu algorymtu
 * @param next_solution funkcja generująca następne rozwiązanie z otoczenia rozwiązania
 * @param max_i liczba iteracji algorytmu do wykonania
 * 
 * */
vector < double > hillclimb(function < double ( vector < double > ) > f, 
							vector < double > init_solution,
							function < vector < double > (vector < double >, double) > next_solution,
							int max_i
							) {
	auto current_solution = init_solution;
								
	for (int i = 0; i < max_i; i++) {
		auto ns = next_solution(current_solution,0.01);
		if (f(ns) > f (current_solution)) {
			current_solution = ns;
			cout <<"i: " << i << ":" << f(ns) << " : "; print_solution(ns);
		}
	}
	return current_solution;
}


/// funkcja sferyczna
auto sphere_f = [](vector < double > x) {
	double ret = 0;
	for (auto e : x) ret = ret + e*e;
	return -ret;
};
/// następne rozwiazanie generowane losowo
auto sphere_f_next_sol = [](vector < double > x, double r) {
	vector < double > ret = x;
	for (auto &e: ret) {
		e = e + r*(((r_gen())/(double)r_gen.max()-0.5)*2);
	}
	return ret;
};

/// funkcja Ackley-a, ma ona wiele optimów lokalnych
auto ackley_f = [](vector < double > v) {
	if (v.size()!=2) throw "x i y!!";
	auto &x = v[0], &y = v[1];
	return -(-20.0*exp(-0.2*sqrt(0.5*(x*x + y*y)))
		- exp(0.5*(cos(2*M_PI*x) + cos(2*M_PI*y)))
		+ M_E + 20);
};


int main (int argc, char **argv) {
    r_gen.seed(time(NULL));
   	vector < double > init_solution = {
		5.0*(((r_gen())/(double)r_gen.max()-0.5)*2),
		5.0*(((r_gen())/(double)r_gen.max()-0.5)*2)
		};

	auto result = hillclimb(ackley_f, init_solution,sphere_f_next_sol,1000);

	cout << "solution: " << result[0] << "," << result[0] << "   f(s) = " << sphere_f(result) << endl;
	return 0;
}


