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

// g++ -std=c++11 simannealing.cpp -o simannealing

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

/* The simulated annealing algorithm */
vector < double > simulatedAnnealing(const vector< double > solution0, 
                            function < bool (const vector<double>&) > inDomain, 
                            function < double (const vector<double>&) > goalF,
                            function < bool (const vector<double>&) > termCondition,
                            function < double ( int ) > T = [](const double k){return pow(0.99999,(double)k);},
                            bool maximize = false,
                            unsigned generatorSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count()
                            );

////////////////////////////////////////////////////////////////////////
//
//   The main function testing the execution of optimization
//
////////////////////////////////////////////////////////////////////////


int main ()  {
    
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
    int i = 0;
    solution = simulatedAnnealing(solution, functionDomain, f, 
        [&](const vector < double > &){i++;if (i > 100000) return true; return false;},
        [](const double k){return pow(0.99999,(double)k);}
    );
  
    cout << "Solution ( " << f(solution) << " ): " << solution[0] << " , " << solution[1] << endl;

    return 0;
}


/* The simulated annealing */
vector < double > simulatedAnnealing(
        const vector< double > solution0,                   // initial solution 
        function < bool (const vector<double>&) > inDomain, // solution domain check
        function < double (const vector<double>&) > goalF,  // goal function (for minimization)
        function < bool (const vector<double>&) > termCondition, // termination condition
        function < double ( int ) > T,                      // temperature function
        bool maximize,                                       // maximize or minimize
        unsigned generatorSeed                             // random number generator seed
        ) {
    std::default_random_engine generator(generatorSeed);
    std::normal_distribution<double> rdist(0.0,0.01); // distribution for neighbour finding
//  std::uniform_real_distribution<double> rdist(-0.01, 0.01);

    list < vector< double > > solution = { solution0 }; // classic approach - store every result in container
        
    std::uniform_real_distribution<double> zerooneprob(0, 1);
    int i = 0;
    while (!termCondition(solution.back())) {
        auto solutionCandidate = solution.back();
        for (auto &e: solutionCandidate) {
            e = e+rdist(generator);  // new solution based on the previous solution
        }
        if(inDomain(solutionCandidate)) {
            auto y = goalF(solution.back()); // current solution quality (previous)
            auto yp = goalF(solutionCandidate); // new solution quality
            
            if (((y > yp) && !maximize) || (((y < yp) && maximize))) { // better solution
                solution.push_back(solutionCandidate);
            } else {
                auto e = exp(-(fabs(yp-y)/T(i)));
                if ( zerooneprob(generator) < e ) { // not that good, but temperature allows us to accept
                    solution.push_back(solutionCandidate);
                }
            }
        }
        i++;
    }
    auto best = solution.back();

    for (auto e : solution) {
        auto y = goalF(best); // current solution quality (previous)
        auto yp = goalF(e); // new solution quality
        if (((y > yp) && !maximize) || (((y < yp) && maximize))) best = e;  
    }
    return best;
}
