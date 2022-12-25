#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include "TableEntry.h"

using Rows = std::vector<TableEntry>;

bool get_command(int argc, char** argv);


class Table {
public:
	//Default Ctor
	Table() {}

	//Ctor - OPTIMIZATION
	Table(std::vector<EntryType> colomn_types_in,
		std::vector<std::string> colomns_names_in) :
		colomn_types{ colomn_types_in }, hash_col{ std::string::npos }, hash_index{}, 
		bst_col{ std::string::npos }, bst_index{}, table_data{} {

		//construct colomn_name to be <name, index>
		std::size_t idx = 0;
		for (std::vector<std::string>::iterator it = colomns_names_in.begin(); it != colomns_names_in.end(); ++it) {
			colomn_names.emplace(*it, idx);
			++idx;
		}
	}

	//read input from cin, given # of rows, return the starting # of rows
	size_t read_rows(int num);

	//delete a row with criterion satisfying {>, <, =} value
	int delete_row(std::size_t col_idx, char operant, std::string value);

	//print content (colnames + row content) with colnames, modes(WHERE or ALL)
	//return # of rows printed
	int print_content_all(std::vector<std::size_t> &to_print, bool quiet);

	//print content in where mode
	int print_content_where(std::size_t col_idx, char operant, std::string &value, 
		std::vector<std::size_t> &to_print, bool quiet);

	//generate hash index
	void generate_hash(std::size_t col_idx);

	//generate binary search tree index
	void generate_bst(std::size_t col_idx);

private:
	friend class Database;

	//-----------HELPER FUNCTIONS--------------
	//compare table entries
	bool compare(TableEntry val_1, char operant, TableEntry val_2);

	int delete_row_helper(std::size_t col_idx, char operant, TableEntry value);

	int print_content_where_helper(std::size_t col_idx, char operant, TableEntry value,
		std::vector<std::size_t> &to_print, bool quiet);

	//------------DATA--------------
	//vector of EntryType for the types of each colomn
	std::vector<EntryType> colomn_types;
	
	//hash table from colomn_names to colomn_index <colomn_names, colomn_index>
	std::unordered_map<std::string, std::size_t> colomn_names;

	//Hash Index
	//hash table with key=TableEntry (a specific colomn) and value=row_index <TableEntry, row_index>
	std::size_t hash_col = std::string::npos;
	std::unordered_map<TableEntry, std::vector<std::size_t>> hash_index;

	//BST Index
	//binary search tree with key=TableEntry and value=row_index
	std::size_t bst_col = std::string::npos;
	std::map<TableEntry, std::vector<std::size_t>> bst_index;
	
	//big table data
	std::vector<Rows> table_data;

};


class Database {
public:
	//Default Ctor
	Database() {}

	//Destructor
	~Database() {}

	//create a table
	void create();

	//insert into a table
	void insert();

	//delete from a table
	void delete_from();

	//remove a table
	void remove();

	//print from a table
	void print(bool quiet);

	//generate index
	void generate_idx();

	//join 2 tables and print
	void join(bool quiet);

private:
	//<table_name, Table>
	std::unordered_map<std::string, Table> all_tables;

	//----------HELPER FUNCTIONS--------------
	//return the #of rows printed
	int join_helper(std::unordered_map<std::string, Table>::iterator table1, //iterator pointing to first table
		std::unordered_map<std::string, Table>::iterator table2,//iterator pointing to second table
		std::size_t colidx1, std::size_t colidx2, //compared colomns
		std::vector<std::pair<std::size_t, int>> &print_colidx,//printed colomns <index, table 1/2>
		bool quiet); 

};
