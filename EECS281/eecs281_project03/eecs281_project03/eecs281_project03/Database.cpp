#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>

#include "Database.h"
using namespace std;

//get_command()
bool get_command(int argc, char ** argv) {
	bool quiet = false;

	//if no flag, argc == 1, will not enter loop
	for (int i = 1; i < argc; ++i) {
		//help
		if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") {
			cout << "Help on how to use this program\n"
				<< "-h/--help: print help message\n"
				<< "-q/--quiet: quiet mode, only numerical summary printed" << endl;
			exit(0);
		}
		//quiet mode
		else if (std::string(argv[i]) == "-q" || std::string(argv[i]) == "--quiet") {
			quiet = true;
		}
		//unrecognized flag
		else {
			//cerr << "Unrecognized flag. Refer to -h/--help for help." << endl;
			//exit(1);
		}
	}

	return quiet;
}



//=============================================Table=======================================
//=========================================================================================
//Table::read_rows()
size_t Table::read_rows(int num) {

	size_t start = table_data.size();
	table_data.reserve(table_data.size() + num);
	//read #num of rows
	for (int i = 0; i < num; ++i) {
		table_data.push_back(Rows());
		//read each entry of the row
		for (size_t j = 0; j < colomn_names.size(); ++j) {
			//read
			string input;
			cin >> input;

			//push into the last row according to data type
			switch (colomn_types[j]) {
			case EntryType::Bool:
				if (input == "true")
					table_data.back().push_back(TableEntry{ true });
				else
					table_data.back().push_back(TableEntry{ false });
				break;
			case EntryType::Int:
				table_data.back().push_back(TableEntry{ stoi(input) });
				break;
			case EntryType::Double:
				table_data.back().push_back(TableEntry{ stod(input) });
				break;
			case EntryType::String:
				table_data.back().push_back(TableEntry{ input });
				break;
			}
		}

		//update the index accordingly
		if (hash_col != string::npos) {
			auto iter = hash_index.find(table_data.back()[hash_col]);
			
			//value not in hash_index, create new vector
			if (iter == hash_index.end())
				hash_index[table_data.back()[hash_col]] = { table_data.size() - 1 };
			//value in the hash_index, push_back index
			else
				iter->second.push_back(table_data.size() - 1);
		}
		else if (bst_col != string::npos) {
			auto iter = bst_index.find(table_data.back()[bst_col]);

			//value not in hash_index, create new vector
			if (iter == bst_index.end())
				bst_index[table_data.back()[bst_col]] = { table_data.size() - 1 };
			//value in the hash_index, push_back index
			else
				iter->second.push_back(table_data.size() - 1);
		}
	}//for()

	//kill the last newline
	cin >> ws;

	//check index validity
	if (bst_col != string::npos) {
		size_t check_bst = 0;
		for (auto iter = bst_index.begin(); iter != bst_index.end(); ++iter) {
			check_bst += iter->second.size();
		}
		assert(check_bst == table_data.size());
	}
	else if (hash_col != string::npos) {
		size_t check_hash = 0;
		for (auto iter = hash_index.begin(); iter != hash_index.end(); ++iter) {
			check_hash += iter->second.size();
		}
		assert(check_hash == table_data.size());
	}

	return start;
}

//Table::delete_row()
int Table::delete_row(size_t col_idx, char operant, string value) {
	int count = 0;

	switch (colomn_types[col_idx]) {
	case EntryType::Bool:
		if (value == "true")
			count = delete_row_helper(col_idx, operant, TableEntry(true));
		else
			count = delete_row_helper(col_idx, operant, TableEntry(false));
		break;

	case EntryType::Int:
		count = delete_row_helper(col_idx, operant, TableEntry(stoi(value)));
		break;
	case EntryType::Double:
		count = delete_row_helper(col_idx, operant, TableEntry(stod(value)));
		break;
	case EntryType::String:
		count = delete_row_helper(col_idx, operant, TableEntry(value));
		break;
	default:
		//can't exist
		break;
	}

	//check index validity
	if (bst_col != string::npos) {
		size_t check_bst = 0;
		for (auto iter = bst_index.begin(); iter != bst_index.end(); ++iter) {
			check_bst += iter->second.size();
		}
		assert(check_bst == table_data.size());
	}
	else if (hash_col != string::npos) {
		size_t check_hash = 0;
		for (auto iter = hash_index.begin(); iter != hash_index.end(); ++iter) {
			check_hash += iter->second.size();
		}
		assert(check_hash == table_data.size());
	}

	return count;
}


//Table::delete_row_helper()
int Table::delete_row_helper(std::size_t col_idx, char operant, TableEntry value) {
	int count = 0;
	
	//hash index exist on col_idx
	if (col_idx == hash_col && operant == '=') {
		auto find_iter = hash_index.find(value);
		//can't find value
		if (find_iter == hash_index.end())
			count = 0;
		//value found
		else {
			for (auto row_iter = find_iter->second.rbegin(); row_iter != find_iter->second.rend(); ++row_iter) {
				table_data.erase(table_data.begin() + *row_iter);
			}
			count = (int)find_iter->second.size();

			//regenerate hash indexs
			generate_hash(hash_col);
		}
	}
	//bst index exist on col_idx
	else if (col_idx == bst_col) {
		if (operant == '=') {
			auto find_iter = bst_index.find(value);
			//can't find value
			if (find_iter == bst_index.end()) {
				count = 0;
			}
			//value found
			else {
				for (auto row_iter = find_iter->second.rbegin(); row_iter != find_iter->second.rend(); ++row_iter) {
					table_data.erase(table_data.begin() + *row_iter);
				}
				count = (int)find_iter->second.size();
			}
		}
		else if (operant == '<') {
			vector<size_t> to_delete;//index of rows to be deleted

			auto find_iter = bst_index.find(value);
			//value found in the table
			if (find_iter != bst_index.end()) {
				//loop through all values strictly smaller than "value"
				for (auto bst_iter = bst_index.begin(); bst_iter != find_iter; ++bst_iter) {
					for (auto row_iter = bst_iter->second.begin(); row_iter != bst_iter->second.end(); ++row_iter) {
						to_delete.push_back(*row_iter);
					}
				}
			}
			else { //value not found in the table
				//loop through all values smaller than upper bound
				for (auto bst_iter = bst_index.begin(); bst_iter != bst_index.upper_bound(value); ++bst_iter) {
					for (auto row_iter = bst_iter->second.begin(); row_iter != bst_iter->second.end(); ++row_iter) {
						to_delete.push_back(*row_iter);
					}
				}
			}
			
			count = (int)to_delete.size();
			sort(to_delete.begin(), to_delete.end());

			//delete from back to start
			for (auto delete_iter = to_delete.rbegin(); delete_iter != to_delete.rend(); ++delete_iter) {
				table_data.erase(table_data.begin() + *delete_iter);
			}

		}
		else { //operant == '>'
			vector<size_t> to_delete;//index of rows to be deleted

			for (auto bst_iter = bst_index.upper_bound(value); bst_iter != bst_index.end(); ++bst_iter) {
				for (auto row_iter = bst_iter->second.begin(); row_iter != bst_iter->second.end(); ++row_iter) {
					to_delete.push_back(*row_iter);
				}
			}
			count = (int)to_delete.size();
			sort(to_delete.begin(), to_delete.end());

			//delete from back to start
			for (auto delete_iter = to_delete.rbegin(); delete_iter != to_delete.rend(); ++delete_iter) {
				table_data.erase(table_data.begin() + *delete_iter);
			}
		}

		//regenerate bst index after deletion
		generate_bst(bst_col);
	}
	else { //no helpful index exist

		//loop through all rows
		vector<Rows>::iterator it = table_data.begin();
		while (it != table_data.end()) {
			//if satisfy condition, delete it
			if (compare((*it)[col_idx], operant, value)) {
				//delete table entry
				it = table_data.erase(it);
				++count;
				continue;
			}
			++it;
		}

		//update hash or bst index
		if (hash_col != string::npos) {
			hash_index.clear();
			generate_hash(hash_col);
		}
		if (bst_col != string::npos) {
			bst_index.clear();
			generate_bst(bst_col);
		}
	}
	

	return count;
}


//Table::print_content_all()
int Table::print_content_all(std::vector<std::size_t>& to_print, bool quiet) {
	if (!quiet) {
		//loop through every entry, print with all indicated colomn index
		for (vector<Rows>::iterator row_iter = table_data.begin(); row_iter != table_data.end(); ++row_iter) {
			for (vector<size_t>::iterator iter = to_print.begin(); iter != to_print.end(); ++iter) {
				cout << (*row_iter)[*iter] << " ";
			}
			cout << endl;
		}
	}
	return (int) table_data.size();
}


//Table::print_content_where()
int Table::print_content_where(size_t col_idx, char operant, string & value, 
							   vector<size_t>& to_print, bool quiet) {

	int count = 0; //# of rows printed
	
	switch (colomn_types[col_idx]) {
	case(EntryType::Bool):
		if (value == "true")
			count = print_content_where_helper(col_idx, operant, TableEntry{ true }, to_print, quiet);
		else //value == "false"
			count = print_content_where_helper(col_idx, operant, TableEntry{ false }, to_print, quiet);
		break;
	case(EntryType::Int):
		count = print_content_where_helper(col_idx, operant, TableEntry{ stoi(value) }, to_print, quiet);
		break;
	case(EntryType::Double):
		count = print_content_where_helper(col_idx, operant, TableEntry{ stod(value) }, to_print, quiet);
		break;
	case(EntryType::String):
		count = print_content_where_helper(col_idx, operant, TableEntry{ value }, to_print, quiet);
		break;
	}
	
	return count;
}


//Table::print_content_where_helper()
int Table::print_content_where_helper(size_t col_idx, char operant, TableEntry value, 
	vector<size_t> &to_print, bool quiet) {

	int count = 0;

	//hash index available
	if (col_idx == hash_col && operant == '=') {
		auto find_iter = hash_index.find(value);
		//no such value in the table
		if (find_iter == hash_index.end()) {
			count = 0;
		}
		//value found, find_iter->second = vector of index satisfy equality
		else {
			count = (int) find_iter->second.size();
			if (!quiet) {
				//iterate through the vector of row index
				for (vector<size_t>::iterator row_it = find_iter->second.begin();
					row_it != find_iter->second.end(); ++row_it) {
					//iterate through the to_print vector
					for (auto col_it = to_print.begin(); col_it != to_print.end(); ++col_it) {
						cout << table_data[*row_it][*col_it] << " ";
					}
					cout << endl;
				}
			}
		}
	}
	//bst index available
	else if (col_idx == bst_col) {
		if (operant == '=') {
			auto find_iter = bst_index.find(value);
			//value not found
			if (find_iter == bst_index.end())
				count = 0;
			else {
				count = (int) find_iter->second.size();
				if (!quiet) {
					//iterate through the vector of row index
					for (vector<size_t>::iterator row_it = find_iter->second.begin();
						row_it != find_iter->second.end(); ++row_it) {
						//iterate through the to_print vector
						for (auto col_it = to_print.begin(); col_it != to_print.end(); ++col_it) {
							cout << table_data[*row_it][*col_it] << " ";
						}
						cout << endl;
					}
				}
			}
		}
		else if (operant == '<') {
			auto find_iter = bst_index.find(value);
			//'value' exist, print everything strictly smaller
			if (find_iter != bst_index.end()) {
				//loop through all smaller values
				for (auto it = bst_index.begin(); it != find_iter; ++it) {
					for (auto rowidx_iter = it->second.begin(); rowidx_iter != it->second.end(); ++rowidx_iter) {
						if (!quiet) {
							for (auto col_iter = to_print.begin(); col_iter != to_print.end(); ++col_iter) {
								cout << table_data[*rowidx_iter][*col_iter] << " ";
							}
							cout << endl;
						}
						++count;
					}
				}
			}
			//'value' doesn't exist, print everything smaller than its upper bound
			else {
				//loop through all values smaller than upper bound
				for (auto it = bst_index.begin(); it != bst_index.upper_bound(value); ++it) {
					for (auto rowidx_iter = it->second.begin(); rowidx_iter != it->second.end(); ++rowidx_iter) {
						if (!quiet) {
							for (auto col_iter = to_print.begin(); col_iter != to_print.end(); ++col_iter) {
								cout << table_data[*rowidx_iter][*col_iter] << " ";
							}
							cout << endl;
						}
						++count;
					}
				}
			}
			
		}
		else { // operant == '>'
			auto upper_iter = bst_index.upper_bound(value);

			//loop through all larger values
			for (auto it = upper_iter ; it != bst_index.end(); ++it) {
				for (auto rowidx_iter = it->second.begin(); rowidx_iter != it->second.end(); ++rowidx_iter) {
					if (!quiet) {
						for (auto col_iter = to_print.begin(); col_iter != to_print.end(); ++col_iter) {
							cout << table_data[*rowidx_iter][*col_iter] << " ";
						}
						cout << endl;
					}
					++count;
				}
			}
		}
	}
	//no index available
	else {
		//loop through every entry, print according to condition
		for (vector<Rows>::iterator row_iter = table_data.begin(); row_iter != table_data.end(); ++row_iter) {

			if (compare((*row_iter)[col_idx], operant, value)) {
				if (!quiet) {
					for (vector<size_t>::iterator iter = to_print.begin(); iter != to_print.end(); ++iter) {
						cout << (*row_iter)[*iter] << " ";
					}
					cout << endl;
				}
				++count;
			}
		}
	}
	
	return count;
}

//Table::generate_hash()
void Table::generate_hash(size_t col_idx) {
	//OPTIMIZATION - same index, no need to generate

	//update hash_index
	hash_index.clear();
	bst_index.clear();
	for (size_t row_i = 0; row_i < table_data.size(); ++row_i) {
		auto iter = hash_index.find(table_data[row_i][col_idx]);
		//if the key not in, create new vector with row_i
		if (iter == hash_index.end())
			hash_index[table_data[row_i][col_idx]] = { row_i };
		//if the key already in, push_back row_i
		else
			iter->second.push_back(row_i);
	}
	
	hash_col = col_idx;
	bst_col = string::npos;

	//check index validity
	if (bst_col != string::npos) {
		size_t check_bst = 0;
		for (auto iter = bst_index.begin(); iter != bst_index.end(); ++iter) {
			check_bst += iter->second.size();
		}
		assert(check_bst == table_data.size());
	}
	else if (hash_col != string::npos) {
		size_t check_hash = 0;
		for (auto iter = hash_index.begin(); iter != hash_index.end(); ++iter) {
			check_hash += iter->second.size();
		}
		assert(check_hash == table_data.size());
	}

}

//Table::generate_bst()
void Table::generate_bst(size_t col_idx) {
	//OPTIMIZATION - same index, no need to generate

	bst_index.clear();
	hash_index.clear();
	for (size_t row_i = 0; row_i < table_data.size(); ++row_i) {
		auto iter = bst_index.find(table_data[row_i][col_idx]);
		//if the key not in, create new vector with row_i
		if (iter == bst_index.end())
			bst_index[table_data[row_i][col_idx]] = { row_i };
		//if the key already in, push_back row_i
		else
			iter->second.push_back(row_i);
	}

	bst_col = col_idx;
	hash_col = string::npos;

	//check index validity
	if (bst_col != string::npos) {
		size_t check_bst = 0;
		for (auto iter = bst_index.begin(); iter != bst_index.end(); ++iter) {
			check_bst += iter->second.size();
		}
		assert(check_bst == table_data.size());
	}
	else if (hash_col != string::npos) {
		size_t check_hash = 0;
		for (auto iter = hash_index.begin(); iter != hash_index.end(); ++iter) {
			check_hash += iter->second.size();
		}
		assert(check_hash == table_data.size());
	}
}

//Table::compare()
bool Table::compare(TableEntry val_1, char operant, TableEntry val_2) {
	switch (operant) {
	case '<':
		return val_1 < val_2;
	case '=':
		return val_1 == val_2;
	case '>':
		return val_1 > val_2;
	default:
		//shouldn't happen
		return true;
	}
}


//============================================Database===========================================
//===============================================================================================
void Database::create() {
	string waste;

	//read table name
	string tablename;
	cin >> tablename;

	//error handling - <tablename> already exist
	auto iter = all_tables.find(tablename);
	if (iter != all_tables.end()) {
		cout << "Error: Cannot create already existing table " << tablename << endl;
		getline(cin, waste);
		return;
	}

	//read size of columns
	size_t length = 0;
	cin >> length;

	//read header types
	vector<EntryType> header_type(length);
	string col_type;
	for (size_t i = 0; i < length; ++i) {
		cin >> col_type;
		if (col_type == "bool")
			header_type[i] = EntryType::Bool;
		else if (col_type == "int")
			header_type[i] = EntryType::Int;
		else if (col_type == "double")
			header_type[i] = EntryType::Double;
		else if (col_type == "string")
			header_type[i] = EntryType::String;
		else {
			//unrecognized type input
		}
	}

	//read header names
	vector<string> header_names;
	header_names.reserve(length);
	string col_name;
	for (size_t i = 0; i < length; ++i) {
		cin >> col_name;
		header_names.push_back(col_name);
	}
	cin >> ws;

	//construct a table using all components
	all_tables.emplace(tablename, Table(header_type, header_names));

	cout << "New table " << tablename << " with column(s) ";
	for (size_t i = 0; i < length; ++i)
		cout << header_names[i] << " ";
	cout << "created\n";

}//create()

//Database::insert()
void Database::insert() {
	string waste;
	cin >> waste;

	//read tablename
	string tablename;
	cin >> tablename;
	unordered_map<string, Table>::iterator it = all_tables.find(tablename); //find table_name

	//read number of row insertion
	int num_rows;
	cin >> num_rows;
	cin >> waste;
	size_t start_row = 0; //starting point of insertion

	//error handling - <tablename> not found
	if (it == all_tables.end()) {
		cout << "Error: " << tablename << " does not name a table in the database" << endl;
		getline(cin, waste);
		for (int i = 0; i < num_rows; ++i) {
			getline(cin, waste);
		}
		return;
	}

	start_row = (it->second).read_rows(num_rows);
	cout << "Added " << num_rows << " rows to " << tablename
		<< " from position " << start_row << " to " << start_row + num_rows - 1
		<< endl;

}//insert()

//Database::delete_from()
void Database::delete_from() {
	string waste;
	cin >> waste;

	//read tablename
	string tablename;
	cin >> tablename;

	//find the corresponding table
	unordered_map<string, Table>::iterator table_it = all_tables.find(tablename);
	int num_deletion = 0;

	//error handling - table not found
	if (table_it == all_tables.end()) {
		cout << "Error: " << tablename << " does not name a table in the database" << endl;
		getline(cin, waste);
		return; 
	}

	cin >> waste;
	//read column name and criterion
	string col_name;
	cin >> col_name;

	//error handling, can't find colomn
	auto col_iter = table_it->second.colomn_names.find(col_name);
	if (col_iter == table_it->second.colomn_names.end()) {
		cout << "Error: " << col_name << " does not name a column in " << tablename << endl;
		getline(cin, waste);
		return;
	}

	//colomn index
	size_t col_idx = col_iter->second;
	//deletion criterion
	char operant;
	string value;
	cin >> operant;
	cin >> value;

	//call the table to delete row
	num_deletion = table_it->second.delete_row(col_idx, operant, value);
	cout << "Deleted " << num_deletion << " rows from " << tablename << endl;
	cin >> ws;
}

//Database::remove()
void Database::remove() {
	string tablename;
	cin >> tablename;

	//find the corresponding table
	unordered_map<string, Table>::iterator it = all_tables.find(tablename);

	//error checking - cannot find table
	if (it == all_tables.end()) {
		cout << "Error: " << tablename << " does not name a table in the database" << endl;
		cin >> ws;
		return;
	}

	all_tables.erase(it);
	cout << "Table " << tablename << " deleted" << endl;
	cin >> ws;
}

//Database::print()
void Database::print(bool quiet) {
	string waste;
	cin >> waste;

	//read and find tablename
	string tablename;
	cin >> tablename;
	unordered_map<string, Table>::iterator table_it = all_tables.find(tablename);
	//error handling, table not found
	if (table_it == all_tables.end()) {
		cout << "Error: " << tablename << " does not name a table in the database" << endl;
		getline(cin, waste);
		return;
	}

	//read size N
	int N = 0;
	cin >> N;

	//index of columns to be printed
	vector<size_t> to_print;
	to_print.reserve(N);
	vector<string> header;
	header.reserve(N);

	//error handling - print_colname1...n not found
	for (int i = 0; i < N; ++i) {
		string col_name;
		cin >> col_name;
		auto col_iter = table_it->second.colomn_names.find(col_name);
		if (col_iter == table_it->second.colomn_names.end()) {
			cout << "Error: " << col_name << " does not name a column in " << tablename << endl;
			getline(cin, waste);
			return;
		}
		else { //col_name found
			to_print.push_back(col_iter->second);
			header.push_back(col_name);
		}
		
	}

	string mode;//WHERE or ALL
	cin >> mode;
	int M = 0;//# of rows printed

	if (mode[0] == 'W') {//WHERE mode
		string col_name;
		cin >> col_name;

		//error handling - colname not found
		auto col_iter = table_it->second.colomn_names.find(col_name);
		if (col_iter == table_it->second.colomn_names.end()) {
			cout << "Error: " << col_name << " does not name a column in " << tablename << endl;
			getline(cin, waste);
			return;
		}

		//---------START PRITING--------
		//print column headers
		if (!quiet) {
			for (int i = 0; i < N; ++i)
				cout << header[i] << " ";
			cout << endl;
		}

		//print content
		size_t col_idx = col_iter->second;
		char operant;
		string value;
		cin >> operant;
		cin >> value;

		M = table_it->second.print_content_where(col_idx, operant, value, to_print, quiet);
	}
	else {//ALL mode
		//---------START PRITING--------
		//print column headers
		if (!quiet) {
			for (int i = 0; i < N; ++i)
				cout << header[i] << " ";
			cout << endl;
		}

		//print content
		M = table_it->second.print_content_all(to_print, quiet);
	}
	
	//print summary
	cout << "Printed " << M << " matching rows from " << tablename << endl;
	cin >> ws;
}

//Database::generate_idx()
void Database::generate_idx() {
	string waste;
	cin >> waste;

	string tablename;
	cin >> tablename;
	//find the corresponding table
	unordered_map<string, Table>::iterator it = all_tables.find(tablename);

	//error handling - tablename
	if (it == all_tables.end()) {
		cout << "Error: " << tablename << " does not name a table in the database" << endl;
		getline(cin, waste);
		return;
	}

	//read index type
	bool hash_idx;
	string idx_type;
	cin >> idx_type;
	if (idx_type == "hash")
		hash_idx = true;
	else
		hash_idx = false;

	cin >> waste;
	cin >> waste;

	//read column name
	string col_name;
	cin >> col_name;
	auto col_iter = it->second.colomn_names.find(col_name);

	//error handling - colname
	if (col_iter == it->second.colomn_names.end()) {
		cout << "Error: " << col_name << " does not name a column in " << tablename << endl;
		getline(cin, waste);
		return;
	}
	size_t col_idx = col_iter->second;

	//generate index
	if (hash_idx)//hash
		it->second.generate_hash(col_idx);
	else //bst
		it->second.generate_bst(col_idx);


	cout << "Created " << idx_type << " index for table " << tablename
		<< " on column " << col_name << endl;
	cin >> ws;
}

//Database::join()
void Database::join(bool quiet) {
	string waste;
	int num_rows = 0;

	//read table names
	string tablename1;
	string tablename2;
	cin >> tablename1;
	cin >> waste;
	cin >> tablename2;

	//find table 1 and table 2
	auto table1 = all_tables.find(tablename1); //table1 is iterator
	auto table2 = all_tables.find(tablename2); //table2 is iterator
	//error handling
	if (table1 == all_tables.end()) {
		cout << "Error: " << tablename1 << " does not name a table in the database" << endl;
		getline(cin, waste);
		return;
	}
	if (table2 == all_tables.end()) {
		cout << "Error: " << tablename2 << " does not name a table in the database" << endl;
		getline(cin, waste);
		return;
	}

	//read column names
	string colname1;
	string colname2;
	cin >> waste;
	cin >> colname1;
	cin >> waste;
	cin >> colname2;

	//find colomn 1 and colomn 2
	auto colomn1_iter = table1->second.colomn_names.find(colname1);
	auto colomn2_iter = table2->second.colomn_names.find(colname2);
	//error handling
	if (colomn1_iter == table1->second.colomn_names.end()) {
		cout << "Error: " << colname1 << " does not name a column in " << tablename1 << endl;
		getline(cin, waste);
		return;
	}
	if (colomn2_iter == table2->second.colomn_names.end()) {
		cout << "Error: " << colname2 << " does not name a column in " << tablename2 << endl;
		getline(cin, waste);
		return;
	}
	size_t colomn1_idx = colomn1_iter->second;
	size_t colomn2_idx = colomn2_iter->second;

	//read N
	int N = 0;
	cin >> waste;
	cin >> waste;
	cin >> N;

	//find print_colname1 to print_colnameN and push into vector of size_t
	vector<pair<size_t, int>> to_print;
	to_print.reserve(N);
	vector<string> header;
	header.reserve(N);

	for (int i = 0; i < N; ++i) {
		//read print_colname and which table it's from
		string print_colname;
		int one_or_two;
		cin >> print_colname;
		cin >> one_or_two;

		//print_colname in table 1
		if (one_or_two == 1) {
			auto iter = table1->second.colomn_names.find(print_colname);
			if (iter == table1->second.colomn_names.end()) {
				cout << "Error: " << print_colname << " does not name a column in " << tablename1 << endl;
				getline(cin, waste);
				return;
			}
			to_print.push_back({ iter->second, 1 });
			header.push_back(print_colname);
		}
		else {//print_colname in table 2
			auto iter = table2->second.colomn_names.find(print_colname);
			if (iter == table2->second.colomn_names.end()) {
				cout << "Error: " << print_colname << " does not name a column in " << tablename2 << endl;
				getline(cin, waste);
				return;
			}
			to_print.push_back({ iter->second, 2 });
			header.push_back(print_colname);
		}
	}

	//print colomn header
	if (!quiet) {
		for (size_t i = 0; i < header.size(); ++i) {
			cout << header[i] << " ";
		}
		cout << endl;
	}

	//print the cotent
	num_rows = join_helper(table1, table2, colomn1_idx, colomn2_idx, to_print, quiet);

	//print the summary
	cout << "Printed " << num_rows << " rows from joining " 
		<< tablename1 << " to " << tablename2 << endl;
}

//Database::join_helper()
int Database::join_helper(std::unordered_map<std::string, Table>::iterator table1, //iterator pointing to first table
	std::unordered_map<std::string, Table>::iterator table2,//iterator pointing to second table
	std::size_t colidx1, std::size_t colidx2, //compared colomn index
	std::vector<pair<std::size_t, int>> &print_colidx,//printed colomns index <col_index, 1 or 2>
	bool quiet) {

	int count = 0;
	
	for (size_t row_i = 0; row_i < table1->second.table_data.size(); ++row_i) {

		//hash index exist on colidx2
		if (colidx2 == table2->second.hash_col) {
			auto hash_iter = table2->second.hash_index.find(table1->second.table_data[row_i][colidx1]);
			//value from table1 found in table2
			if (hash_iter != table2->second.hash_index.end()) {
				for (auto row_iter = hash_iter->second.begin(); row_iter != hash_iter->second.end(); ++row_iter) {
					if (!quiet) {
						//print out columns
						for (auto print_iter = print_colidx.begin(); print_iter != print_colidx.end(); ++print_iter) {
							if (print_iter->second == 1) {
								cout << table1->second.table_data[row_i][print_iter->first] << " ";
							}
							else {//iter->second == 2
								cout << table2->second.table_data[*row_iter][print_iter->first] << " ";
							}
						}
						cout << endl;
					}
				}
				count += (int)hash_iter->second.size();
			}

		}
		//bst index exist on colidx2
		else if (colidx2 == table2->second.bst_col) {
			auto bst_iter = table2->second.bst_index.find(table1->second.table_data[row_i][colidx1]);
			if (bst_iter != table2->second.bst_index.end()) {
				for (auto row_iter = bst_iter->second.begin(); row_iter != bst_iter->second.end(); ++row_iter) {
					if (!quiet) {
						//print out columns
						for (auto print_iter = print_colidx.begin(); print_iter != print_colidx.end(); ++print_iter) {
							if (print_iter->second == 1) {
								cout << table1->second.table_data[row_i][print_iter->first] << " ";
							}
							else {//iter->second == 2
								cout << table2->second.table_data[*row_iter][print_iter->first] << " ";
							}
						}
						cout << endl;
					}
				}
				count += (int)bst_iter->second.size();
			}
		}
		//no helpful index exist
		else {
			//loop through second table
			for (size_t row_j = 0; row_j < table2->second.table_data.size(); ++row_j) {
				//if the checking entry are the same
				if (table1->second.table_data[row_i][colidx1] == table2->second.table_data[row_j][colidx2]) {
					if (!quiet) {
						//print out columns
						for (auto iter = print_colidx.begin(); iter != print_colidx.end(); ++iter) {
							if (iter->second == 1) {
								cout << table1->second.table_data[row_i][iter->first] << " ";
							}
							else {//iter->second == 2
								cout << table2->second.table_data[row_j][iter->first] << " ";
							}
						}
						cout << endl;
					}
					++count;
				}

			}//inner for()
		}
		
	}//outer for()

	return count;
}