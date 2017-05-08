// g++ -std=c++11 hillclimb.cpp -o hillclimb

// MIT License, Tadeusz Puźniakowski

#include <tuple>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <functional>
#include <vector>


using namespace std;

/* Ackley test function for optimization */
double ackley(const vector<double > &solution);
/* Simple test functio */
double parabola(const vector<double> &solution);



/* random number generator in range (a,b) */
double r(double a, double b);

/* check if vector v is within range [minV, maxV] */
bool inRange(const vector < double > &v, const vector < double > &minV, const vector < double > &maxV);

/* The randomized hill climbing algorithm */
vector < double > hillClimbing(const vector< double > solution0, 
							const vector < double > &minV, 
							const vector < double > & maxV, 
							function < double (const vector<double>&) > goalF );

int main ()  {
    srand(time(NULL));
    vector< double > minV = {-5,-5}, maxV = {5,5}; // dziedzina funkcji
    vector< double > solution = {r(-5, 5), r(-5, 5)}; // punkt startu
    int TTL = 20; // liczba iteracji bez poprawy po ktorej algorytm konczy
  
	solution = hillClimbing(solution, minV, maxV, ackley);
  
    cout << "Rozwiązanie: " << solution[1] << " , " << solution[1] << endl;

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


/* random number generator in range (a,b) */
double r(double a, double b) {
    return ((rand()%100000)/100000.0)*(b-a)+a;
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

/* The randomized hill climbing algorithm */
vector < double > hillClimbing(const vector< double > solution0, const vector < double > &minV, const vector < double > & maxV, function < double (const vector<double>&) > goalF ) {
    vector< double > solution = solution0;    
    int TTL = 20; // no solution improvement after 20 iterations results in algorithm stop
    while (TTL) {
        auto solutionCandidate = solution;
        for (auto &e: solutionCandidate) {
			e = e+r(-0.01, 0.01);  // new solution based on the previous solution
		}
        if(!inRange(solutionCandidate, minV, maxV)) {
            cout << "out of range: " << solution[0] << " , " << solution[1] << endl;
        } else {
                auto y = goalF(solution);
                auto yp = goalF(solutionCandidate);
                if (y <= yp) {
                    solution = solutionCandidate;
                    TTL = 20;
                } else {
                    TTL--;
                }
                cout << "current solution:" ;
                for (auto e : solution) cout << " " << e;
                cout << ": " << y << endl;
        }
    }
	return solution;
}
