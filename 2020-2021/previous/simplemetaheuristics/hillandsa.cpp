/*
 * Copyright 2017 Tadeusz Puźniakowski
 *
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * */

// g++ -std=c++11 hillandsa.cpp -o hillclimb

// code created while lecture on SA and HC

#include <tuple>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <functional>
#include <vector>
#include <list>


#include <random>
#include <chrono>


using namespace std;

/* The hill climbing algorithm */
vector < double > hillClimbing(const vector< double > solution0, 
                            function < bool (const vector<double>&) > inDomain, 
                            function < double (const vector<double>&) > goalF,
                            function < bool (const vector<double>&) > termCondition,
                            bool maximize = false,
                            unsigned generatorSeed = chrono::high_resolution_clock::now().time_since_epoch().count()
                            );


vector < double > simulatedAnnealing(
        const vector< double > solution0,                   // initial solution 
        function < bool (const vector<double>&) > inDomain, // solution domain check
        function < double (const vector<double>&) > goalF,  // goal function (for minimization)
        function < bool (const vector<double>&) > termCondition, // termination condition
        function < double ( int ) > T,                       // temperature
        bool maximize = false,                                       // maximize or minimize
        unsigned generatorSeed = chrono::high_resolution_clock::now().time_since_epoch().count()                            // random number generator seed
        );

////////////////////////////////////////////////////////////////////////
//
//   The main function testing the execution of optimization
//
////////////////////////////////////////////////////////////////////////


int main (int argc, char **argv)  {
    
    // random start point
    default_random_engine generator(chrono::high_resolution_clock::now().time_since_epoch().count());
    uniform_real_distribution<double> rdist(-5, 5);
    vector< double > solution = {rdist(generator), rdist(generator)}; // start point
    
    // check if value is in function domain
    vector< double > minV = {-5,-5}, maxV = {5,5};
    auto functionDomain = [&](const vector < double > & v){
        for (int i = 0; i < v.size(); i++) {
            if((v[i] <= minV[i]) || (v[i] >= maxV[i])) {return false;}
        }
        return true;
    };
    
    //  function -- Ackley - minimization
    auto f = [](const vector<double > &solution) -> double {
            double x,y;
            tie(x,y) = pair<double, double>(solution[0], solution[1]);
            return (-20*exp(-0.2*sqrt(0.5*(x*x+y*y)))-
                exp(0.5*(cos(2.0*M_PI*x)+cos(2*M_PI*y)))
                 + M_E + 20);
    };
   
    int iterations = 1000;
    for (int i = 1; i < argc; i++) {
		if (string(argv[i]) == "iterations") {
			iterations = atoi(argv[i+1]);
			i++;
			cout << "iterations = " << iterations << endl;
		}
	}
    int i = 0;
    solution = hillClimbing(solution, functionDomain, f, 
        [&](const vector < double > &){i++;if (i > iterations) return true; return false;}
    );
    cout << "Solution HC ( " << f(solution) << " ): " << solution[0] << " , " << solution[1] << endl;
    
 /*   i = 0;
    solution = {rdist(generator), rdist(generator)}; // start point
    solution = simulatedAnnealing(solution, functionDomain, f, 
        [&](const vector < double > &){i++;if (i > iterations) return true; return false;},
        [](int i) { return pow(0.99999,(double)i); }
    );
    cout << "Solution SA ( " << f(solution) << " ): " << solution[0] << " , " << solution[1] << endl;
  */

    return 0;
}





/* The randomized hill climbing algorithm */
vector < double > simulatedAnnealing(
        const vector< double > solution0,                   // initial solution 
        function < bool (const vector<double>&) > inDomain, // solution domain check
        function < double (const vector<double>&) > goalF,  // goal function (for minimization)
        function < bool (const vector<double>&) > termCondition, // termination condition
        function < double ( int ) > T,                       // temperature
        bool maximize,                                       // maximize or minimize
        unsigned generatorSeed                               // random number generator seed
        ) {
    std::default_random_engine generator(generatorSeed);
    std::normal_distribution<double> rdist(0.0,0.01); // distribution for neighbour finding
    std::uniform_real_distribution<double> u(0, 1);

    list < vector< double > > solution = { solution0 };
    int i = 0;
    while (!termCondition(solution.back())) {
        auto solutionCandidate = solution.back();
        for (auto &e: solutionCandidate) {
            e = e+rdist(generator);  // new solution based on the previous solution
        }
        if(inDomain(solutionCandidate)) {
            auto y = goalF(solution.back()); // current solution quality (previous)
            auto yp = goalF(solutionCandidate); // new solution quality
            
            if (((y > yp) && !maximize) || (((y < yp) && maximize))) { // better solution - accept allways
                solution.push_back(solutionCandidate);
                if ((i % 100) == 0 ) cout << "N " << solution.back()[0] << " " << solution.back()[1] << " "  << y << endl;
            } else {
				// T
				if (u(generator) < exp(-fabs(y-yp)/T(i)) ) {
					solution.push_back(solutionCandidate);
					if ((i % 100) == 0 )cout << "N " << solution.back()[0] << " " << solution.back()[1] << " " << y << endl;
				}
			}
        }
        i++;
    }
    auto best = solution.front();
    for (auto s : solution) {
		auto y = goalF(best); // current solution quality (previous)
		auto yp = goalF(s); // new solution quality
		if (((y > yp) && !maximize) || (((y < yp) && maximize))) { // better solution - accept allways
                best = s;
        } 
	}
    return best;
}













/* The randomized hill climbing algorithm */
vector < double > hillClimbing(
        const vector< double > solution0,                   // initial solution 
        function < bool (const vector<double>&) > inDomain, // solution domain check
        function < double (const vector<double>&) > goalF,  // goal function (for minimization)
        function < bool (const vector<double>&) > termCondition, // termination condition
        bool maximize,                                       // maximize or minimize
        unsigned generatorSeed                               // random number generator seed
        ) {
    std::default_random_engine generator(generatorSeed);
    std::normal_distribution<double> rdist(0.0,0.01); // distribution for neighbour finding
//  std::uniform_real_distribution<double> rdist(-0.01, 0.01);

    vector< double > solution = solution0;
    int i = 0;
    while (!termCondition(solution)) {
        auto solutionCandidate = solution;
        for (auto &e: solutionCandidate) {
            e = e+rdist(generator);  // new solution based on the previous solution
        }
        if(inDomain(solutionCandidate)) {
            auto y = goalF(solution); // current solution quality (previous)
            auto yp = goalF(solutionCandidate); // new solution quality
            
            if (((y > yp) && !maximize) || (((y < yp) && maximize))) { // better solution - accept allways
                solution = solutionCandidate;
                //if ((i % 100) == 0 ) 
                cout << "P " << solution[0] << " " << solution[1] << " "  << y << endl;
            }
        }
        i++;
    }
    return solution;
}
