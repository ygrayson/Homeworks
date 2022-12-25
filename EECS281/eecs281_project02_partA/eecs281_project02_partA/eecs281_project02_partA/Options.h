/* Options header file
 *
 */

class Options {
private:
	bool stats = false;
	bool median = false;
	bool verbose = false;
	unsigned int num_to_print;

public:
	Options() : stats(false), median(false), verbose(false), num_to_print(0) {}

	void get_options(int argc, char** argv);

	bool print_stats() const;

	bool print_median() const;

	bool print_verbose() const;

	unsigned int get_num_to_print() const;
};
