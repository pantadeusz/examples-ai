// g++ -std=c++11 simannealing.cpp -o simannealing

// MIT License, Tadeusz Puźniakowski

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

/* Ackley test function for optimization */
double ackley(const vector<double > &solution);
/* Simple test functio */
double parabola(const vector<double> &solution);

/* check if vector v is within range [minV, maxV] */
bool inRange(const vector < double > &v, const vector < double > &minV, const vector < double > &maxV);

/* The randomized hill climbing algorithm */
vector < double > simulatedAnnealing(const vector< double > solution0, 
							const vector < double > &minV, 
							const vector < double > & maxV, 
							function < double (const vector<double>&) > goalF,
							function < double ( int ) > T = [](const double k){return pow(0.99,(double)k);},
							unsigned generatorSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count());

int main ()  {
	default_random_engine generator(chrono::high_resolution_clock::now().time_since_epoch().count());
	uniform_real_distribution<double> rdist(-5, 5);

    vector< double > minV = {-5,-5}, maxV = {5,5}; // dziedzina funkcji
    vector< double > solution = {rdist(generator), rdist(generator)}; // punkt startu
    
    // maximizing function
    auto f = ackley;
	solution = simulatedAnnealing(solution, minV, maxV, f,[](const double k){return pow(0.99,(double)k);} );
	//solution = simulatedAnnealing(solution, minV, maxV, parabola);
  
    cout << "Solution ( " << f(solution) << " ): " << solution[1] << " , " << solution[1] << endl;

    return 0;
}







/* Ackley test function for optimization */
double ackley(const vector<double > &solution) {
    double x,y;
    tie(x,y) = pair<double, double>(solution[0], solution[1]);
    return -(-20*exp(-0.2*sqrt(0.5*(x*x+y*y)))-
        exp(0.5*(cos(2.0*M_PI*x)+cos(2*M_PI*y)))
         + M_E + 20);
}
/* Simple test functio */
double parabola(const vector<double> &solution) {
    return -(solution[0]*solution[0]+solution[1]*solution[1]);
}

/* check if vector v is within range [minV, maxV] */
bool inRange(const vector < double > &v, const vector < double > &minV, const vector < double > &maxV) {
	for (int i = 0; i < v.size(); i++) {
		if((v[i] <= minV[i]) || (v[i] >= maxV[i])) {
			return false;
		}
	} 
	return true;
}

/* The simulated annealing for maximizing functions */
vector < double > simulatedAnnealing(const vector< double > solution0, 
		const vector < double > &minV, 
		const vector < double > & maxV, 
		function < double (const vector<double>&) > goalF,
		function < double ( int ) > T,
		unsigned generatorSeed ) {
	std::default_random_engine generator(generatorSeed);
//	std::normal_distribution<double> rdist(0.0,0.005); // distribution for neighbour finding
	std::uniform_real_distribution<double> rdist(-0.01, 0.01);

	std::uniform_real_distribution<double> zerooneprob(0, 1);

    list < vector< double > > solution = { solution0 }; // classic approach - store every result in container
        
    int TTL = 100000; // no solution improvement after 20 iterations results in algorithm stop

    for (int i = 0; i < TTL; i++) {
        auto solutionCandidate = solution.back();
        for (auto &e: solutionCandidate) {
			e = e+rdist(generator);  // new solution based on the previous solution
		}
        if(inRange(solutionCandidate, minV, maxV)) {
			auto y = goalF(solution.back()); // current solution quality (previous)
			auto yp = goalF(solutionCandidate); // new solution quality
			if (y <= yp) { // better solution - accept allways
				solution.push_back(solutionCandidate);
				cout << "current solution:" ;
				for (auto e : solution.back()) cout << " " << e;
				cout << ": " << y << endl;
			} else {
				auto e = exp(-(fabs(yp-y)/T(i)));
				if ( zerooneprob(generator) < e ) {
					solution.push_back(solution.back());
					// cout << "current --------:" ;
					// for (auto e : solution.back()) cout << " " << e;
					// cout << ": " << y << endl;
				}
			}
        }
    }
    auto best = solution.back();
    for (auto e : solution) {
		auto y = goalF(best); // current solution quality (previous)
		auto yp = goalF(e); // new solution quality
		if (y < yp) best = e; 
	}
	return best; //solution.back();
}
