#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Database.h"
#include "TableEntry.h"

using namespace std;
using std::boolalpha;

//TODO: remove_if() for optimization
//TODO: cin instead of getline()
int main(int argc, char** argv) {
	//prelinminary
	ios_base::sync_with_stdio(false);
	cin >> boolalpha;
	cout << boolalpha;


	//get command options from command line
	bool quiet = get_command(argc, argv);

	//database
	Database data;
	string command;
	
	//loop to get 1 command each time from user
	do {
		//get command
		cout << "% ";
		cin >> command;

		//if comment, skip it
		if (command[0] == '#') {
			string waste;
			getline(cin, waste);
			continue;
		}
		else {
			//8 different commands
			switch (command[0]) {
			//QUIT
			case 'Q':
				cout << "Thanks for being silly!" << endl;
				return 0;
			//CREATE
			case 'C':
				data.create();
				break;
			//INSERT INTO
			case 'I':
				data.insert();
				break;
			//DELETE FROM
			case 'D':
				data.delete_from();
				break;
			//REMOVE
			case 'R':
				data.remove();
				break;
			//PRINT FROM
			case 'P':
				data.print(quiet);
				break;
			//JOIN
			case 'J':
				data.join(quiet);
				break;
			//GENERATE INDEX
			case 'G':
				data.generate_idx();
				break;
			default:
				//unrecognized command
				cout << "Error: unrecognized command" << endl;
				break;
			}
		}
	} while (true); //loop until QUIT command

}