/* Functions and class of Mine
 *
 */

#include <iostream>
#include <exception>
#include <string>
#include <queue>
#include <vector>
#include <cassert>
#include "Mine.h"
#include "P2random.h"

using namespace std;



//====================Tile=================
//get_val()
int Tile::get_val() const {
	return val;
}
int Tile::get_val() {
	return val;
}

//get_row()
unsigned int Tile::get_row() const {
	return row;
}
unsigned int Tile::get_row() {
	return row;
}

//get_colomn()
unsigned int Tile::get_colomn() const {
	return colomn;
}
unsigned int Tile::get_colomn() {
	return colomn;
}



//=================Map==================
void Map::read_header() {
	string junk;
	cin >> junk;

	//read size
	cin >> map_size;
	cin >> ws;

	map.resize(map_size);
	for (unsigned int i = 0; i < map_size; ++i)
		map[i].resize(map_size);

	cin >> junk;

	//read starting point
	cin >> start_row;
	cin >> start_colomn;
	cin >> ws;
	
	//error handling - starting position invalid
	if (start_row > map_size - 1 || start_colomn > map_size - 1)
		throw runtime_error("starting position exceeding grid size\n");
}

void Map::read_M() {
	//read map data
	for (unsigned int i = 0; i < map_size; ++i) {
		for (unsigned int j = 0; j < map_size; ++j) {
			int val;
			cin >> val;
			map[i][j].first = val;
			map[i][j].second = false;
		}
		cin >> ws;
	}
}

void Map::read_R() {
	string junk;
	//read descriptive data
	unsigned int seed;
	cin >> junk;
	cin >> seed;
	cin >> ws;

	unsigned int max_rubble;
	cin >> junk;
	cin >> max_rubble;
	cin >> ws;

	unsigned int tnt_chance;
	cin >> junk;
	cin >> tnt_chance;
	cin >> ws;

	//generate random map
	stringstream ss;
	P2random::PR_init(ss, map_size, seed, max_rubble, tnt_chance);

	//push into the map
	for (unsigned int i = 0; i < map_size; ++i) {
		for (unsigned int j = 0; j < map_size; ++j) {
			int val;
			ss >> val;
			map[i][j].first = val;
			map[i][j].second = false;
		}
		ss >> ws;
	}
}

//OPTIMIZATION: CAN USE A SIMPLER WAY TO READ, REFER TO THE SPEC
void Map::read() {
	//read file type and redirect to read_M() and read_R()
	char type;
	cin >> type >> ws;
	if (type == 'M') {
		map_type = true;
		read_header();
		read_M();
	}
	else if (type == 'R') {
		map_type = false;
		read_header();
		read_R();
	}
	else
		throw runtime_error("Cannot recognize file type\n");
}

unsigned int Map::get_map_size() const
{
	return map_size;
}

unsigned int Map::get_start_row() const {
	return start_row;
}

unsigned int Map::get_start_colomn() const {
	return start_colomn;
}

unsigned int Map::get_start_val()
{
	return map[start_row][start_colomn].first;
}

int Map::get_val(unsigned int row, unsigned int colomn) const
{
	return map[row][colomn].first;
}

int Map::get_val(unsigned int row, unsigned int colomn) {
	return map[row][colomn].first;
}

bool Map::get_explored(unsigned int row, unsigned int colomn) const
{
	return map[row][colomn].second;
}

bool Map::get_explored(unsigned int row, unsigned int colomn) {
	return map[row][colomn].second;
}

//[] operator overload
vector<pair<int, bool>> &Map::operator[](unsigned int row) {
	return map[row];
}
const vector<pair<int, bool>> &Map::operator[](unsigned int row) const {
	return map[row];
}

void Map::explore(const Tile &tile, const Options &option, 
				  Median_tiles &rubble_val,
				  Stats_tiles &stats_tiles,
				  unsigned int &num_tiles, unsigned int &num_rubbles)
{
	if (tile.get_val() == -1) { //tnt
		//print verbose mode
		if (option.print_verbose()) {
			cout << "TNT explosion at ["
				 << tile.get_row()
				 << ","
				 << tile.get_colomn()
				 << "]!\n";
		}

		//print median mode
		//do nothing

		//print stats mode
		if (option.print_stats()) {
			stats_tiles.first_tiles.push(tile);
			stats_tiles.last_tiles.push(tile);
			stats_tiles.easiest_tiles.push(tile);
			stats_tiles.hardest_tiles.push(tile);
		}
	}
	else if (tile.get_val() > 0) { //tile
		num_tiles++;
		num_rubbles += tile.get_val();

		//print verbose mode
		if (option.print_verbose()) {
			cout << "Cleared: "
				<< tile.get_val()
				<< " at ["
				<< tile.get_row()
				<< ","
				<< tile.get_colomn()
				<< "]\n";
		}

		//print median mode
		if (option.print_median()) {
			if (rubble_val.smaller_half.empty())
				rubble_val.smaller_half.push(tile.get_val());
			else { //non-empty
				//push to each side accordingly
				if (tile.get_val() >= rubble_val.smaller_half.top())
					rubble_val.bigger_half.push(tile.get_val());
				else //tile.get_val() < rubble_val.smaller_half.top()
					rubble_val.smaller_half.push(tile.get_val());

				//rebalance rubble_val.smaller_half and rubble_val.bigger_half
				if (rubble_val.bigger_half.size() == rubble_val.smaller_half.size() + 2) {
					rubble_val.smaller_half.push(rubble_val.bigger_half.top());
					rubble_val.bigger_half.pop();
				}
				else if (rubble_val.bigger_half.size() == rubble_val.smaller_half.size() - 2) {
					rubble_val.bigger_half.push(rubble_val.smaller_half.top());
					rubble_val.smaller_half.pop();
				}
			}

			//calculate the median
			double median = 0;
			if (rubble_val.bigger_half.size() == rubble_val.smaller_half.size())
				median = ((double) rubble_val.bigger_half.top() + (double) rubble_val.smaller_half.top()) / 2;
			else if (rubble_val.bigger_half.size() == rubble_val.smaller_half.size() + 1)
				median = rubble_val.bigger_half.top();
			else if (rubble_val.bigger_half.size() == rubble_val.smaller_half.size() - 1)
				median = rubble_val.smaller_half.top();

			cout << "Median difficulty of clearing rubble is: " << median << "\n";
		}

		//print stats mode
		if (option.print_stats()) {
			stats_tiles.first_tiles.push(tile);
			stats_tiles.last_tiles.push(tile);
			stats_tiles.easiest_tiles.push(tile);
			stats_tiles.hardest_tiles.push(tile);
		}
		
	}
	else {} // print nothing for 0 val

	
	
	//actual exploration: change representation in the 2D grid
	map[tile.get_row()][tile.get_colomn()].first = 0;
	map[tile.get_row()][tile.get_colomn()].second = true;

}

//explode
void Map::explode(const Tile & tile, const Options &option, 
				  Median_tiles &rubble_val,
				  Stats_tiles &stats_tiles,
				  unsigned int &num_tiles, unsigned int &num_rubbles)
{
	if (tile.get_val() == -1) { //tnt
		//print verbose mode
		if (option.print_verbose()) {
			cout << "TNT explosion at ["
				<< tile.get_row()
				<< ","
				<< tile.get_colomn()
				<< "]!\n";
		}

		//print median mode
		//do nothing

		//print stats mode
		if (option.print_stats()) {
			stats_tiles.first_tiles.push(tile);
			stats_tiles.last_tiles.push(tile);
			stats_tiles.easiest_tiles.push(tile);
			stats_tiles.hardest_tiles.push(tile);
		}
	}
	else if (tile.get_val() > 0) { //tile
		num_tiles++;
		num_rubbles += tile.get_val();

		//print verbose mode
		if (option.print_verbose()) {
			cout << "Cleared by TNT: "
				<< tile.get_val()
				<< " at ["
				<< tile.get_row()
				<< ","
				<< tile.get_colomn()
				<< "]\n";
		}

		//print median mode
		if (option.print_median()) {
			if (rubble_val.smaller_half.empty())
				rubble_val.smaller_half.push(tile.get_val());
			else { //non-empty
				//push to each side accordingly
				if (tile.get_val() >= rubble_val.smaller_half.top())
					rubble_val.bigger_half.push(tile.get_val());
				else //tile.get_val() < rubble_val.smaller_half.top()
					rubble_val.smaller_half.push(tile.get_val());

				//rebalance rubble_val.smaller_half and rubble_val.bigger_half
				if (rubble_val.bigger_half.size() == rubble_val.smaller_half.size() + 2) {
					rubble_val.smaller_half.push(rubble_val.bigger_half.top());
					rubble_val.bigger_half.pop();
				}
				else if (rubble_val.bigger_half.size() == rubble_val.smaller_half.size() - 2) {
					rubble_val.bigger_half.push(rubble_val.smaller_half.top());
					rubble_val.smaller_half.pop();
				}
			}

			//calculate the median
			double median = 0;
			if (rubble_val.bigger_half.size() == rubble_val.smaller_half.size())
				median = ((double) rubble_val.bigger_half.top() + (double) rubble_val.smaller_half.top()) / 2;
			else if (rubble_val.bigger_half.size() == rubble_val.smaller_half.size() + 1)
				median = rubble_val.bigger_half.top();
			else if (rubble_val.bigger_half.size() == rubble_val.smaller_half.size() - 1)
				median = rubble_val.smaller_half.top();

			cout << "Median difficulty of clearing rubble is: " << median << "\n";
		}

		//print stats mode
		if (option.print_stats()) {
			stats_tiles.first_tiles.push(tile);
			stats_tiles.last_tiles.push(tile);
			stats_tiles.easiest_tiles.push(tile);
			stats_tiles.hardest_tiles.push(tile);
		}
	}
	else {} // print nothing for 0 val

	//actual explosion: change representation in the 2D grid
	map[tile.get_row()][tile.get_colomn()].first = 0;
}

//add unexplored tiles that are adjacent to "current" in "the_map" into the "pq"
void add_unexplored_adjacent(const Tile & current, const Map & the_map, priority_queue<Tile, vector<Tile>, Tile_easy> &pq) {
	//not on the left edge && left tile unexplored
	if (current.get_colomn() != 0 && !the_map[current.get_row()][current.get_colomn() - 1].second) {
		Tile temp(the_map.get_val(current.get_row(), current.get_colomn() - 1), current.get_row(), current.get_colomn() - 1);
		pq.push(temp);
	}
		
	//not on the right edge && right tile unexplored
	if (current.get_colomn() != the_map.get_map_size() - 1 && !the_map[current.get_row()][current.get_colomn() + 1].second) {
		Tile temp(the_map.get_val(current.get_row(), current.get_colomn() + 1), current.get_row(), current.get_colomn() + 1);
		pq.push(temp); 
	}
		
	//not on the top edge && up tile unexplored
	if (current.get_row() != 0 && !the_map[current.get_row() - 1][current.get_colomn()].second) {
		Tile temp(the_map.get_val(current.get_row() - 1, current.get_colomn()), current.get_row() - 1, current.get_colomn());
		pq.push(temp);
	}
		
	//not on the lower edge && lower tile unexplored
	if (current.get_row() != the_map.get_map_size() - 1 && !the_map[current.get_row() + 1][current.get_colomn()].second) {
		Tile temp(the_map.get_val(current.get_row() + 1, current.get_colomn()), current.get_row() + 1, current.get_colomn());
		pq.push(temp);
	}
		
}


//add unexplored nonzero tiles that are adjacent to "current" in "the_map" into the "pq"
void add_unexplored_nonzero_adjacent(const Tile & current, const Map & the_map, priority_queue<Tile, vector<Tile>, Tile_easy> &pq) {
	//not on the left edge && left tile nonzero unexplored
	if (current.get_colomn() != 0 && !the_map[current.get_row()][current.get_colomn() - 1].second && the_map[current.get_row()][current.get_colomn() - 1].first != 0) {
		Tile temp(the_map.get_val(current.get_row(), current.get_colomn() - 1), current.get_row(), current.get_colomn() - 1);
		pq.push(temp);
	}

	//not on the right edge && right tile unexplored
	if (current.get_colomn() != the_map.get_map_size() - 1 && !the_map[current.get_row()][current.get_colomn() + 1].second && the_map[current.get_row()][current.get_colomn() + 1].first != 0) {
		Tile temp(the_map.get_val(current.get_row(), current.get_colomn() + 1), current.get_row(), current.get_colomn() + 1);
		pq.push(temp);
	}

	//not on the top edge && up tile unexplored
	if (current.get_row() != 0 && !the_map[current.get_row() - 1][current.get_colomn()].second && the_map[current.get_row() - 1][current.get_colomn()].first != 0) {
		Tile temp(the_map.get_val(current.get_row() - 1, current.get_colomn()), current.get_row() - 1, current.get_colomn());
		pq.push(temp);
	}

	//not on the lower edge && lower tile unexplored
	if (current.get_row() != the_map.get_map_size() - 1 && !the_map[current.get_row() + 1][current.get_colomn()].second && the_map[current.get_row() + 1][current.get_colomn()].first != 0) {
		Tile temp(the_map.get_val(current.get_row() + 1, current.get_colomn()), current.get_row() + 1, current.get_colomn());
		pq.push(temp);
	}

}