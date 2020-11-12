/**
 * 
 * We will need following header files
 * 
 * */
#include <iostream>
#include <random>
#include <functional>
#include <list>

/**
 * 
 * We will use following random number generators:
 * 
 * */
//    std::default_random_engine generator(generatorSeed);
//(a)    std::normal_distribution<double> rdist(0.0,0.01); // distribution for neighbour finding
//(b)  std::uniform_real_distribution<double> rdist(-0.01, 0.01);
//rdist(generator)

/**
 * 
 * We will try to solve optimization problem for Ackley function
 * 
 * */

// auto ackleyFunction = [](const vector< double > &p) -> double {
// 		double x,y;
// 		tie(x,y) = pair<double, double>(p[0], p[1]);
// 		return (-20*exp(-0.2*sqrt(0.5*(x*x+y*y)))-
// 			exp(0.5*(cos(2.0*M_PI*x)+cos(2*M_PI*y)))
// 			 + M_E + 20);
// };



using namespace std;




template < class Solution >
Solution generalProblemSolver(Solution s0,
		function < double (Solution) > fitness,
		function < bool (Solution) > termCondition,
		function < Solution (Solution) > getNeighbour
	) {
	int i = 0;
	std::list < Solution > s;
	s.push_back(s0);
	// warunek zakończenia
	while (!termCondition(s.back())) {
		auto newSolution = getNeighbour(s.back());
		if (fitness(newSolution) >= fitness(s.back()) ) {
			s.push_back(newSolution);
		} else {
			s.push_back(s.back());
		}
	}
	return s.back();
}



/**
 * 
 * And the main function
 * 
 * */

int main(int argc, char **argv) {
	// funkcja optymalizowana
	auto fit = [](double x)->double{
		return 100+-100*x*x;
	};
	double x0 = ((double)rand()/RAND_MAX)*4-2;
	int timeToGo = 100000;
	auto termCond = [&](double s)->bool{
		timeToGo--;
		return timeToGo <= 0;
	};
	auto getNeighbour = [](double s) {
		return s + (double)(rand()%1000)/100000.0 - 0.005;
	};

	cout << "x0 = " << x0 << endl;
	double foundX = generalProblemSolver<double>( x0,fit,termCond,getNeighbour);
	cout << "solution = " << foundX << endl;
	return 0;
}

