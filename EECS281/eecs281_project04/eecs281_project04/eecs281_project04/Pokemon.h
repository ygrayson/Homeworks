/*
 * Pokemon.h
 * header file
 */

#include <vector>
#include <limits>
#include <string>


//enum struct of all possible modes
enum struct Mode { MST, FASTTSP, OPTTSP, NULL_MODE };

//possible status of a vertex
enum struct Status {TSP, MST, UNCONNECTED};

//get command line options
Mode get_options(int argc, char** argv);

//==============Map=================
class Map {
public:
	//Default Ctor
	Map() {}

	//read map input from cin
	void read();

	//construct MST
	//using Prim's Algorithm, always using index 0 vertex as starting point
	void construct_MST();

	//Part B: construct TSP by heuristics
	void fast_TSP();

	//Part C: construct optimal TSP
	//Branch and Bound Algorithm
	void optimal_TSP();

	//given a constructed MST, print it out
	void print_MST();

	//given a solved TSP problem, print out solution
	void print_TSP();

private:

	//a single vertex representing a pokemon in the map
	struct vertex {
		std::pair<int, int> coordinate;
		bool connected;
		double distance;
		std::size_t predecessor;

		vertex(int x, int y) : coordinate({ x,y }), connected(false),
			distance(std::numeric_limits<double>::infinity()), predecessor(std::string::npos) {}
	};

	//all the pokemon as vertex
	std::vector<vertex> all_pokemon;

	//current best path for optimal TSP
	std::vector<size_t> current_best;
	double current_distance;

	//path of current permutation
	std::vector<size_t> path;

	//---------Helper Functions-------

	//Part B (Alogithm 2): construct TSP by heuristics: Arbitary Insertion
	//every step find nearest one and insert between 2 vertex
	void arbitary_insertion();

	//calculate Eucleadian distance between (x1, y1) and (x2, y2)
	double calc_distance(std::pair<int, int> const &coordin1, std::pair<int, int> const &coordin2);

	//calculate total distance of whole graph
	double total_distance();

	//for part A, determine if an edge exist between 2 vertex
	//idx1, idx2 are index for 2 vertex in all_pokemon
	bool edge_exist(std::pair<int, int> const &coordin1, std::pair<int, int> const &coordin2);

	//for the path, given current fix_length, generate all the permutation for the non-fixeed part
	void genPerms(std::size_t fix_length);

	//for the path, given gix_length connected, decide if future promising
	bool promising(std::size_t fix_length);

	//construct MST for the rest of the graph
	//return distance of the potential completed MST
	double lower_bound(std::size_t fix_length);

	//connect the unconnected using MST, and all connect back to the end
	//return the distance
	double MST_distance(std::vector<std::size_t> &connection, std::size_t end);
};

