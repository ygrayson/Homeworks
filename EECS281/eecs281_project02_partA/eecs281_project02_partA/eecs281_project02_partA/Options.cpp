#include "Options.h"
#include <getopt.h>
#include <stdlib.h>
#include <iostream>


//get command line options
void Options::get_options(int argc, char** argv) {
	int option_index = 0, option = 0;

	// Don't display getopt error messages about options
	opterr = false;

	// all possible command line options
	static struct option longOpts[] = {       //QUESTION:WHY WE NEED "struct" keyword HERE, option has already been declared
		{ "help", no_argument, nullptr, 'h' },
		{ "stats", required_argument, nullptr, 's' },
		{ "median", no_argument, nullptr, 'm' },
		{ "verbose", no_argument, nullptr, 'v' },
		{ nullptr, 0, nullptr, '\0' }
	};

	while ((option = getopt_long(argc, argv, "hs:mv", longOpts, &option_index)) != -1) {
		switch (option) {
		case 's':
			stats = true;
			num_to_print = std::atoi(optarg);
			break;

		case 'm':
			median = true;
			break;

		case 'v':
			verbose = true;
			break;

		case 'h':
			std::cout << "This is the helper documents for ./MineEscape program\n"
				<< "--stats / -s: print extra summerization upon termination\n"
				<< "--median / -m : print median difficulty\n"
				<< "--verbose / -v : print every rubble value cleared\n"
				<< "--help / -h : print help message" << std::endl;
			exit(0);

		default:
			//invalid command flags handling
			std::cerr << "Cannot recognize the flag indicated. Refer to --help\n";
			exit(1);
			break;
		}
	}
}

bool Options::print_stats() const {
	return stats;
}

bool Options::print_median() const {
	return median;
}


bool Options::print_verbose() const {
	return verbose;
}

unsigned int Options::get_num_to_print() const {
	return num_to_print;
}
