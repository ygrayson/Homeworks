/* EECS281 Project 2 - Mine All Mine
 * Grayson Yin
 *
 */

 /* Keywords to search in file:
  * TODO: to do
  * BUG: there's bug
  * UNSURE: need further check
  * QUESTION: need to ask someone
  * OPTIMIZATION: can be optimized
  * TESTING: test codes to be deleted before completion
  */

//TODO: debugging - normal-large-M-in.txt not passed, but all the rest has been passed
		//MUST BE SOMETHING ABOUT TNT

//TODO: STACK CANNOT BE USED, BUT DEQUE CAN
#include <iostream>
#include <queue>
#include <stack>
#include <iomanip>
#include "Mine.h"
//#include "Options.h"

using namespace std;


int main(int argc, char** argv) {
	//for later output
	cout << fixed << setprecision(2);

	//get options from command line
	Options option;
	option.get_options(argc, argv);

	//representation of 2D grid
	Map mine_map;
	try {
		mine_map.read();
	}
	catch (runtime_error &err) {
		cerr << err.what();
		exit(1);
	}
	
	//PQ for all reachable tiles and exploding tiles
	priority_queue<Tile, vector<Tile>, Tile_easy> reachable;
	priority_queue<Tile, vector<Tile>, Tile_easy> tnt;

	//for Default output mode
	unsigned int num_tiles = 0;
	unsigned int num_rubbles = 0;

	//for median output mode
	Median_tiles rubble_val;

	//for stats output mode
	Stats_tiles stats_tiles;


	//initialize starting position and explore it
	Tile current_tile(mine_map.get_start_val(), mine_map.get_start_row(), mine_map.get_start_colomn());
	//explore current_tile
	mine_map.explore(current_tile, option, rubble_val, stats_tiles, num_tiles, num_rubbles);
	//tile or tnt
	if (current_tile.get_val() != -1) {//current_tile is a tile
		add_unexplored_adjacent(current_tile, mine_map, reachable);
	}
	else { //current_tile is a tnt

		//exploding process - add 4 adjacent to be exploded
		add_unexplored_adjacent(current_tile, mine_map, tnt);

		//start exploding tiles/tnt around
		while (!tnt.empty()) {
			Tile to_explode(tnt.top());
			tnt.pop();

			//ignore invalidated entries
			if (to_explode.get_val() != mine_map[to_explode.get_row()][to_explode.get_colomn()].first)
				continue;

			//explode this one(to_explode) and become visible (add a 0 value)
			mine_map.explode(to_explode, option, rubble_val, stats_tiles, num_tiles, num_rubbles);
			reachable.push(Tile(0, to_explode.get_row(), to_explode.get_colomn()));

			//exploding a tnt
			 if (to_explode.get_val() == -1) {
				//aurrounding nonzero add to be exploded
				add_unexplored_adjacent(to_explode, mine_map, tnt);
			}
		} //while(!tnt.empty())
	}


	//main loop
	//loop until escape
	while (current_tile.get_row() != 0 &&
		current_tile.get_colomn() != 0 &&
		current_tile.get_row() != mine_map.get_map_size() - 1 &&
		current_tile.get_colomn() != mine_map.get_map_size() - 1) { //not on the edge

		//current tile move to new "easiest" tile if unexplored
		current_tile = reachable.top();
		reachable.pop();

		//ignore invalidated entries
		if (current_tile.get_val() != mine_map[current_tile.get_row()][current_tile.get_colomn()].first)
			continue;

		//explore current_tile
		mine_map.explore(current_tile, option, rubble_val, stats_tiles, num_tiles, num_rubbles);


		//tile or tnt
		if (current_tile.get_val() != -1) {//current_tile is a tile
			add_unexplored_adjacent(current_tile, mine_map, reachable);
		}
		else { //current_tile is a tnt

			//exploding process - add 4 adjacent to be exploded
			add_unexplored_adjacent(current_tile, mine_map, tnt);

			//start exploding tiles/tnt around
			while (!tnt.empty()) {
				Tile to_explode(tnt.top());
				tnt.pop();

				//ignore invalidated entries
				if (to_explode.get_val() != mine_map[to_explode.get_row()][to_explode.get_colomn()].first)
					continue;

				//explode this one(to_explode) and become visible (add a 0 value)
				mine_map.explode(to_explode, option, rubble_val, stats_tiles, num_tiles, num_rubbles);
				reachable.push(Tile(0, to_explode.get_row(), to_explode.get_colomn()));

				//exploding a tnt
				if (to_explode.get_val() == -1) {
					//surrounding nonzero add to be exploded
					add_unexplored_adjacent(to_explode, mine_map, tnt);
				}
			} //while(!tnt.empty())

		}//else (tnt)

	} //while(!escaped)

	//Default output
	cout << "Cleared "
		 << num_tiles
		 << " tiles containing "
		 << num_rubbles
		 << " rubble and escaped.\n";

	if (option.print_stats()) {
		//
		cout << "First tiles cleared:\n";
		unsigned int i = 0;
		while (i < option.get_num_to_print() && !stats_tiles.first_tiles.empty()) {
			if (stats_tiles.first_tiles.front().get_val() == -1) {
				cout << "TNT"
					<< " at ["
					<< stats_tiles.first_tiles.front().get_row()
					<< ","
					<< stats_tiles.first_tiles.front().get_colomn()
					<< "]\n";
			}
			else {
				cout << stats_tiles.first_tiles.front().get_val()
					<< " at ["
					<< stats_tiles.first_tiles.front().get_row()
					<< ","
					<< stats_tiles.first_tiles.front().get_colomn()
					<< "]\n";
			}
			i++;
			stats_tiles.first_tiles.pop();
		}

		//
		cout << "Last tiles cleared:\n";
		i = 0;
		while (i < option.get_num_to_print() && !stats_tiles.last_tiles.empty()) {
			if (stats_tiles.last_tiles.top().get_val() == -1) {
				cout << "TNT"
					<< " at ["
					<< stats_tiles.last_tiles.top().get_row()
					<< ","
					<< stats_tiles.last_tiles.top().get_colomn()
					<< "]\n";
			}
			else {
				cout << stats_tiles.last_tiles.top().get_val()
					<< " at ["
					<< stats_tiles.last_tiles.top().get_row()
					<< ","
					<< stats_tiles.last_tiles.top().get_colomn()
					<< "]\n";
			}
			i++;
			stats_tiles.last_tiles.pop();
		}

		//
		cout << "Easiest tiles cleared:\n";
		i = 0;
		while (i < option.get_num_to_print() && !stats_tiles.easiest_tiles.empty()) {
			if (stats_tiles.easiest_tiles.top().get_val() == -1) {
				cout << "TNT"
					<< " at ["
					<< stats_tiles.easiest_tiles.top().get_row()
					<< ","
					<< stats_tiles.easiest_tiles.top().get_colomn()
					<< "]\n";
			}
			else {
				cout << stats_tiles.easiest_tiles.top().get_val()
					<< " at ["
					<< stats_tiles.easiest_tiles.top().get_row()
					<< ","
					<< stats_tiles.easiest_tiles.top().get_colomn()
					<< "]\n";
			}
			i++;
			stats_tiles.easiest_tiles.pop();
		}

		//
		cout << "Hardest tiles cleared:\n";
		i = 0;
		while (i < option.get_num_to_print() && !stats_tiles.hardest_tiles.empty()) {
			if (stats_tiles.hardest_tiles.top().get_val() == -1) {
				cout << "TNT"
					<< " at ["
					<< stats_tiles.hardest_tiles.top().get_row()
					<< ","
					<< stats_tiles.hardest_tiles.top().get_colomn()
					<< "]\n";
			}
			else {
				cout << stats_tiles.hardest_tiles.top().get_val()
					<< " at ["
					<< stats_tiles.hardest_tiles.top().get_row()
					<< ","
					<< stats_tiles.hardest_tiles.top().get_colomn()
					<< "]\n";
			}
			i++;
			stats_tiles.hardest_tiles.pop();
		}
	}

	return 0;
}