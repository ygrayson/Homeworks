/* Header file for Tile.cpp
 *
 */
#include <vector>
#include <queue>
#include <stack>
#include "Options.h"


class Tile {
private:
	int val;
	unsigned int row;
	unsigned int colomn; //location [row, colomn]

public:
	//Default Constructor
	Tile() : val(0), row(0), colomn(0) {}

	//Initial Constructor
	Tile(int val_in, unsigned int row_in, unsigned int colomn_in) : val(val_in), row(row_in), colomn(colomn_in) {}

	//Copy Constructor
	Tile(const Tile &tile_in) : val(tile_in.val), row(tile_in.row), colomn(tile_in.colomn) {}

	//Destructor
//	~Tile() {}

	int get_val() const;
	int get_val();

	unsigned int get_row() const;
	unsigned int get_row();

	unsigned int get_colomn() const;
	unsigned int get_colomn();
	
};


class Tile_easy {
public:
	bool operator()(Tile const & lhs, Tile const & rhs) {
		if (lhs.get_val() != rhs.get_val())
			return lhs.get_val() > rhs.get_val(); //TODO, WHY?
		else if (lhs.get_colomn() != rhs.get_colomn())
			return lhs.get_colomn() > rhs.get_colomn();
		else //lhs.get_val() == rhs.get_val() && lhs.get_colomn() == rhs.get_colomn()
			return lhs.get_row() > rhs.get_row();
	}
};

class Tile_hard {
public:
	bool operator()(Tile const & lhs, Tile const & rhs) {
		if (lhs.get_val() != rhs.get_val())
			return lhs.get_val() < rhs.get_val(); //QUESTION: WHY? WHY THE SIGN IS THIS WAY?
		else if (lhs.get_colomn() != rhs.get_colomn())
			return lhs.get_colomn() < rhs.get_colomn();
		else //lhs.get_val() == rhs.get_val() && lhs.get_colomn() == rhs.get_colomn()
			return lhs.get_row() < rhs.get_row();
	}
};

struct Median_tiles {
	std::priority_queue<int, std::vector<int>, std::less<int>> smaller_half;
	std::priority_queue<int, std::vector<int>, std::greater<int>> bigger_half;
};


//for stats output mode
struct Stats_tiles{
	std::queue<Tile> first_tiles;
	std::stack<Tile> last_tiles;
	std::priority_queue<Tile, std::vector<Tile>, Tile_easy> easiest_tiles;
	std::priority_queue<Tile, std::vector<Tile>, Tile_hard> hardest_tiles;
};


class Map {
private:
	bool map_type; //true = M, false = R
	unsigned int map_size;

	//starting location
	unsigned int start_row;
	unsigned int start_colomn;

	//map representation
	std::vector<std::vector<std::pair<int, bool> > > map; //< value, explored = false >

	//read helper function
	void read_header();
	void read_M();
	void read_R();

public:
	//Default Ctor
	Map() : map_type(true), map_size(0), start_row(0), start_colomn(0), map() {} 

	//Copy Ctor
	Map(Map &map_in) : map_type(map_in.map_type), map_size(map_in.map_size), start_row(map_in.start_row), start_colomn(map_in.start_colomn), map(map_in.map) {}

	//read data from input file
	void read();

	//
	unsigned int get_map_size() const;

	//
	unsigned int get_start_row() const;

	//
	unsigned int get_start_colomn() const;

	//return the starting position value
	unsigned int get_start_val();

	//return the val at specific location in the map
	int get_val(unsigned int row, unsigned int colomn) const;
	int get_val(unsigned int row, unsigned int colomn);

	//return whether the location has been 
	bool get_explored(unsigned int row, unsigned int colomn) const;
	bool get_explored(unsigned int row, unsigned int colomn);

	//overload [] operator for easier access
	std::vector<std::pair<int, bool>> &operator[](unsigned int row);
	const std::vector<std::pair<int, bool>> &operator[](unsigned int row) const;

	//explore a tile
	void explore(const Tile &tile, const Options &option, 
				 Median_tiles &tile_val,
				 Stats_tiles &stats_tiles,
				 unsigned int &num_tiles, unsigned int &num_rubbles);

	//explode a tile (and print)
	void explode(const Tile &tile, const Options &option, 
				 Median_tiles &tile_val,
				 Stats_tiles &stats_tiles,
				 unsigned int &num_tiles, unsigned int &num_rubbles);
};

//add unexplored tiles that are adjacent to "current" in "the_map" into the "pq"
void add_unexplored_adjacent(const Tile &current, const Map &the_map, std::priority_queue<Tile, std::vector<Tile>, Tile_easy> &pq);

void add_unexplored_nonzero_adjacent(const Tile & current, const Map & the_map, std::priority_queue<Tile, std::vector<Tile>, Tile_easy> &pq);