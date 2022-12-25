/*
 * Pokemon.cpp
 * 
 */

#include <iostream>
#include <getopt.h>
#include <exception>
#include <cmath>
#include "Pokemon.h"

using namespace std;

//get command line options
Mode get_options(int argc, char** argv) {
	int option_index = 0, option = 0;
	Mode mode = Mode::NULL_MODE;

	// Don't display getopt error messages about options
	opterr = false;

	// all possible command line options
	static struct option longOpts[] = {       //QUESTION:WHY WE NEED "struct" keyword HERE, option has already been declared
		{ "help", no_argument, nullptr, 'h' },
		{ "mode", required_argument, nullptr, 'm' },
		{ nullptr, 0, nullptr, '\0' }
	};

	while ((option = getopt_long(argc, argv, "hm:", longOpts, &option_index)) != -1) {
		switch (option) {
		case 'm':
			if (std::string(optarg) == "MST") {
				mode = Mode::MST;
			}
			else if (std::string(optarg) == "FASTTSP") {
				mode = Mode::FASTTSP;
			}
			else if (std::string(optarg) == "OPTTSP") {
				mode = Mode::OPTTSP;
			}
			else {
				cerr << "mode not recognized. Refer to --help\n" << endl;
				exit(1);
			}
			break;
		case 'h':
			cout << "This is the helper documents for ./poke program\n"
				<< "--mode / -m [MST|FASTTSP|OPTTSP]: specify the mode MST, FASTTSP, or OPTTSP\n"
				<< "--help / -h : print help message" << endl;
			exit(0);

		default:
			//invalid command flags handling
			throw runtime_error("Cannot recognize the flag indicated. Refer to --help\n");
		}
	}//while

	//if -m/--mode flag was not set
	if (mode == Mode::NULL_MODE)
		throw runtime_error("Mode not specified\n");

	return mode;
}



//====================Map====================
//Map::read()
void Map::read() {
	size_t num_pokemon = 0;

	cin >> num_pokemon;
	all_pokemon.reserve(num_pokemon);

	for (size_t i = 0; i < num_pokemon; ++i) {
		int x, y = 0;//coordinates
		cin >> x;
		cin >> y;

		all_pokemon.push_back(vertex(x, y));
	}
}

//Map::construct_MST()
void Map::construct_MST() {

	all_pokemon[0].distance = 0;
	
	//each iteration add one vertex
	for (size_t i = 0; i < all_pokemon.size(); ++i) {
		double min_distance = numeric_limits<double>::infinity();
		size_t new_innie = string::npos;

		//find shortest outtie
		for (size_t j = 0; j < all_pokemon.size(); ++j) {
			if (!all_pokemon[j].connected) {
				if (all_pokemon[j].distance < min_distance) {
					new_innie = j;
					min_distance = all_pokemon[j].distance;
				}
			}
		}//for()

		//error handling - no outtie reachable
		if (new_innie == string::npos)
			throw runtime_error("Cannot construct MST\n");

		//add shortest outtie into the graph
		all_pokemon[new_innie].connected = true;

		//update distance of all vertex connected to new innie
		for (size_t k = 0; k < all_pokemon.size(); ++k) {
			if (!all_pokemon[k].connected) {
				if (edge_exist(all_pokemon[new_innie].coordinate, all_pokemon[k].coordinate)) {
					double new_distance = calc_distance(all_pokemon[new_innie].coordinate, all_pokemon[k].coordinate);
					if (new_distance < all_pokemon[k].distance) {
						all_pokemon[k].distance = new_distance;
						all_pokemon[k].predecessor = new_innie;
					}
				}
			}
		}
	}//for()
}

//Map::fast_TSP()
void Map::fast_TSP() {
	arbitary_insertion();
}

//Map::nearest_insertion()
void Map::arbitary_insertion() {

	//Step 1: initialize sub-tour with 1 nodes (index 0)
	all_pokemon[0].connected = true;
	all_pokemon[0].predecessor = 0;
	all_pokemon[0].distance = 0;


	//Step 2: n-1 loops to insert all the rest vertex with lowest cost
	for (size_t i = 1; i < all_pokemon.size(); ++i) {

		//Step 2.1: find 2 vertex to insert between
		size_t ver1, ver2 = string::npos;
		double min_increase = numeric_limits<double>::infinity();
		for (size_t k = 0; k < all_pokemon.size(); ++k) {
			if (all_pokemon[k].connected) {
				double increase = calc_distance(all_pokemon[i].coordinate, all_pokemon[k].coordinate) +
					calc_distance(all_pokemon[i].coordinate, all_pokemon[all_pokemon[k].predecessor].coordinate) - 
					all_pokemon[k].distance;
				if (increase < min_increase) {
					min_increase = increase;
					ver1 = k;
					ver2 = all_pokemon[k].predecessor;
				}
			}
		}

		//Step 2.2: insert
		all_pokemon[ver1].predecessor = i;
		all_pokemon[ver1].distance = calc_distance(all_pokemon[ver1].coordinate, all_pokemon[i].coordinate);
		all_pokemon[i].connected = true;
		all_pokemon[i].predecessor = ver2;
		all_pokemon[i].distance = calc_distance(all_pokemon[i].coordinate, all_pokemon[ver2].coordinate);
	}
}

//Map::optimal_TSP()
void Map::optimal_TSP() {
	//use heuristic as the current best
	//OPTIMIZATION
	fast_TSP();
	current_distance = total_distance();

	current_best.reserve(all_pokemon.size());
	size_t follow = 0;
	current_best.push_back(follow);
	for (size_t i = 0; i < all_pokemon.size() - 1; ++i) {
		follow = all_pokemon[follow].predecessor;
		current_best.push_back(follow);
	}


	//initialize path to be {0, 1, 2, ..., n-1}
	path.reserve(all_pokemon.size());
	for (size_t i = 0; i < all_pokemon.size(); ++i) {
		path.push_back(i);
	}

	//fix first one to be 0, generate all the permutations (branch and bound)
	genPerms(1);

	//print optimal tour
	cout << current_distance << endl;
	for (size_t i : current_best) {
		cout << i << " ";
	}
	cout << endl;

}

//Map::print_MST()
void Map::print_MST() {
	double distance = total_distance();

	cout << distance << endl;

	//print all edges
	for (size_t i = 1; i < all_pokemon.size(); ++i) {
		if (i < all_pokemon[i].predecessor)
			cout << i << " " << all_pokemon[i].predecessor << endl;
		else
			cout << all_pokemon[i].predecessor << " " << i << endl;
	}
}

//Map::print_TSP()
void Map::print_TSP() {
	//print out total distance of TSP
	double distance = total_distance();

	cout << distance << endl;

	//print out the connection
	size_t current = 0;
	for (size_t i = 0; i < all_pokemon.size(); ++i) {
		cout << current << " ";
		current = all_pokemon[current].predecessor;
	}
}

//Map::calc_distance()
double Map::calc_distance(pair<int, int> const &coordin1, pair<int, int> const &coordin2) {

	double x = (double) coordin1.first - (double) coordin2.first;
	double y = (double) coordin1.second - (double) coordin2.second;

	return sqrt(x * x + y * y);
}

//Map::total_distance()
double Map::total_distance() {
	double total_distance = 0;
	
	for (size_t i = 0; i < all_pokemon.size(); ++i) {
		total_distance += all_pokemon[i].distance;
	}

	return total_distance;
}

//Map::edge_exist()
bool Map::edge_exist(pair<int, int> const &coordin1, pair<int, int> const &coordin2) {
	//coordin1 on the land
	if (coordin1.first > 0 || coordin1.second > 0) {
		//coordin2 on the land or coastline
		if (coordin2.first >= 0 || coordin2.second >= 0)
			return true;
		else
			return false;
	}
	//coordin1 in the sea
	else if (coordin1.first < 0 && coordin1.second < 0) {
		//coordin2 on the land
		if (coordin2.first > 0 || coordin2.second > 0)
			return false;
		else
			return true;
	}
	//coordin1 on the coastline
	else {
		return true;
	}
}

//Map::genPerms()
void Map::genPerms(size_t fix_length) {
	if (fix_length == path.size()) {
		//connect 0 to the last node
		all_pokemon[0].connected = true;
		all_pokemon[0].distance = calc_distance(all_pokemon[0].coordinate, all_pokemon[path.back()].coordinate);

		double distance = total_distance();
		
		if (distance < current_distance) {
			current_distance = distance;
			current_best = path;
		}
		return;
	}

	if (!promising(fix_length))
		return;

	//full permutation
	for (size_t i = fix_length; i < path.size(); ++i) {
		swap(path[fix_length], path[i]);
		all_pokemon[path[fix_length]].connected = true;
		all_pokemon[path[fix_length]].distance = calc_distance(all_pokemon[path[fix_length]].coordinate, all_pokemon[path[fix_length - 1]].coordinate);
		all_pokemon[path[fix_length]].predecessor = path[fix_length - 1];

		genPerms(fix_length + 1);

		all_pokemon[path[fix_length]].connected = false;
		all_pokemon[path[fix_length]].distance = numeric_limits<double>::infinity();
		all_pokemon[path[fix_length]].predecessor = string::npos;
		swap(path[fix_length], path[i]);
	} // for
}

//Map::promising()
bool Map::promising(std::size_t fix_length)
{
	if (path.size() - fix_length < 4)
		return true;//OPTIMIZATION
	
	//lower bound for this potential solution
	double low_bound = lower_bound(fix_length);
	return low_bound < current_distance;
}

//Map::lower_bound()
double Map::lower_bound(size_t fix_length) {
	double bound = 0;
	vector<size_t> unconnected(path.begin() + fix_length, path.end());

	//add existing distance of connected
	for (size_t i = 1; i < fix_length; ++i) { //if n vertex fixed, n-1 edges added into distance
		bound += all_pokemon[path[i]].distance;
	}

	//add MST distance of unconnected
	bound += MST_distance(unconnected, path[fix_length - 1]);

	return bound;
}

//Map::MST_distance()
double Map::MST_distance(vector<size_t> &unconnected, size_t end) {
	vector<bool> mst_connection(unconnected.size(), false);
	vector<double> distance(unconnected.size(), numeric_limits<double>::infinity());

	distance[0] = 0;

	//each iteration add one vertex
	for (size_t i = 0; i < unconnected.size(); ++i) {
		double min_distance = numeric_limits<double>::infinity();
		size_t new_innie = string::npos;

		//find shortest outtie
		for (size_t j = 0; j < unconnected.size(); ++j) {
			if (!mst_connection[j]) {
				if (distance[j] < min_distance) {
					new_innie = j;
					min_distance = distance[j];
				}
			}
		}//for()

		//add shortest outtie into the graph
		mst_connection[new_innie] = true;

		//update distance of all vertex connected to new innie
		for (size_t k = 0; k < unconnected.size(); ++k) {
			if (!mst_connection[k]) {
				double new_distance = calc_distance(all_pokemon[unconnected[new_innie]].coordinate, all_pokemon[unconnected[k]].coordinate);
				if (new_distance < distance[k])
					distance[k] = new_distance;
			}
				
		}
	}//for()

	//MST_distance = MST_distance + connect back to end and 0
	double MST_distance = 0;
	for (double i : distance) {
		MST_distance += i;
	}

	//find connection from TSP to MST
	double min_to_zero = numeric_limits<double>::infinity();
	double min_to_end = numeric_limits<double>::infinity();
	for (size_t i = 0; i < unconnected.size(); ++i) {
		double distance_to_zero = calc_distance(all_pokemon[unconnected[i]].coordinate, all_pokemon[0].coordinate);
		double distance_to_end = calc_distance(all_pokemon[unconnected[i]].coordinate, all_pokemon[end].coordinate);
		min_to_zero = distance_to_zero < min_to_zero ? distance_to_zero : min_to_zero;
		min_to_end = distance_to_end < min_to_end ? distance_to_end : min_to_end;
	}
	
	MST_distance = MST_distance + min_to_zero + min_to_end;

	return MST_distance;
}
