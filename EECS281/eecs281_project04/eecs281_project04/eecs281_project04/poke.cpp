/*
 * EECS 281 - Project 4
 * Pokemon trainer traveling problem
 */

#include <iomanip>
#include <iostream>
#include "Pokemon.h"


using namespace std;

int main(int argc, char** argv) {
	cout << setprecision(2);//Always show 2 decimal places
	cout << fixed;//Disable scientific notation for large numbers

	Mode mode;
	try {
		mode = get_options(argc, argv);
	}
	catch (runtime_error &err) {
		cerr << err.what();
		exit(1);
	}
	

	Map map;
	map.read();
	//visit pokemons according to modes
	if (mode == Mode::MST) {
		try {
			map.construct_MST();
		}
		catch (runtime_error &error) {
			cerr << error.what();
			exit(1);
		}
		
		//print out MST
		map.print_MST();
	}
	else if (mode == Mode::FASTTSP) {
		map.fast_TSP();

		//print out route for TSP
		map.print_TSP();
	}
	else {//mode == Mode::OPTTSP
		map.optimal_TSP();
	}


	return 0;
}