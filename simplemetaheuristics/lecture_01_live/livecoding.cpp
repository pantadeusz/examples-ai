/**
 * 
 * We will need following header files
 * 
 * */
#include <iostream>
#include <random>
#include <functional>

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

auto ackleyFunction = [](const vector< double > &p) -> double {
		double x,y;
		tie(x,y) = pair<double, double>(p[0], p[1]);
		return (-20*exp(-0.2*sqrt(0.5*(x*x+y*y)))-
			exp(0.5*(cos(2.0*M_PI*x)+cos(2*M_PI*y)))
			 + M_E + 20);
};


/**
 * 
 * And the main function
 * 
 * */

int main(int argc, char **argv) {
	return 0;
}

