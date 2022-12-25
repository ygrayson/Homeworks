//OPTIMIZATION: find() and insert() have some redundant loops, where looping twice to inset element

#include "hashtable.h"
#include <iostream>
using namespace std;

struct test_key{
	size_t x = 0;
	//Default Ctor
	test_key() {}
	explicit test_key(size_t nx): x(nx){}

	//== overload
	bool operator==(test_key other) const {
		return x == other.x;
	}

	//!= overload
	bool operator!=(test_key other) const {
		return x != other.x;
	}
};

struct test_hash{
	//hash(k) = k.x
	size_t operator()(test_key k) const{
		return k.x;
	}
};

struct test_val{
	int v;
	test_val(): v(0) {}
	explicit test_val(int x): v(x) {}
};

int main(){
	//First Testing Table
	HashTable<test_key, test_val, test_hash> table;
	table.insert(test_key(5), test_val(1));
	table[test_key(50)] = test_val(2);
	assert(table.size() == 2);
	assert(table[test_key(50)].v == 2);

	table.erase(test_key(50));
	assert(table.size() == 1);
	assert(table[test_key(5)].v == 1);

	table[test_key(5)] = test_val(10);
	assert(table[test_key(5)].v == 10);



	//Second Testing Table
	HashTable<int, int> num_table;
	assert(num_table.insert(1, 99));
	num_table[2] = 101;
	assert(!num_table.insert(2, 100));
	assert(num_table.size() == 2);
	assert(num_table[1] == 99);
	assert(num_table[2] == 101);



	//Third Testing Table
	HashTable<string, int> name_age;
	name_age["RRR"] = 40;
	name_age["GGG"] = 21;
	name_age["NNN"] = 21;
	assert(name_age["NNN"] == name_age["GGG"]);
	assert(name_age["NNN"] == 21);

	name_age["NNN"] = 22;
	assert(name_age["NNN"] == 22);

	name_age.insert("NNN2", 20);
	name_age.insert("NNN3", 20);
	name_age.erase("NNN2");
	assert(name_age["NNN3"] == 20);


	//Forth Testing Table
	HashTable<int, int> complimentary;
	for (int i = 0; i < 100; ++i) {
		complimentary.insert(i, 100 - i);
	}

	for (int j = 0; j < 100; ++j) {
		assert(complimentary[j] = 100 - j);
	}

	cout << "Tests Passed!" << endl;
	return 0;
}