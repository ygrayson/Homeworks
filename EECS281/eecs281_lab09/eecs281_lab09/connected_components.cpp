/* 
    EECS 281 Lab 9: Connected Components 

    For this lab, you will write a program that calculates the 
    number of connected components in an undirected graph. 

    Your program will receive an input file in the following format:
    1) The first line of input contains the number of vertices V and number of edges E in the graph.
    2) The next E lines list out the connections between vertices in the graph in the format of "u v",
       which indicates the existence of an edge between u and v.

    For example, the following input file would be read as follows:
    4 2  (the graph has 4 vertices (numbered from 0-3), and two edges exist in the graph)
    0 1  (the first of these edges connects vertex 0 with vertex 1)
    0 2  (the second of these edges connects vertex 0 with vertex 2)

    In this case, vertices {0, 1, 2} form a connected component, and vertex {3} forms a connected component
    (since 3 is not connected to anything). Thus, your program should print out 2.

    There are many different approaches that you can use to solve this problem. However, there are some
    algorithms that are more efficient than others. We won't take off any points if your solution isn't
    as efficient as it should be, but it's recommended that you think about the algorithms we have learned
    so far in the course. Which algorithms work best for problems like this?
*/

//Algorithm 1: vector of different connected components


#include <stdio.h>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <algorithm>

using namespace std;

class Graph{
    unsigned int V;//# of vertex
    unsigned int E;//# of edges
    // DONE: add any additional member variables, as needed
	vector<unordered_set<int>> connected;

public:
    // Graph constructor that initializes the graph and its member variables
    Graph(unsigned int V, unsigned int E){
        this->V = V;
        this->E = E;
		this->connected.reserve(V);
        // TODO: initialize member variables that you added above

    }

	//read an edge from input file
	void read_edge();

	//find a vertex
	//return string::npos if not found
	size_t find_vertex(int vertex);

	//add a vertex
	void add_vertex(size_t pos, int vertex);

	//merge connected components idx2 into connected components idx1
	void merge_components(size_t idx1, size_t idx2);

	//print number of connected components
	void print_connected();

};

int main(){
    std::ios_base::sync_with_stdio(false);

	//read # of vertex and # of edges
    unsigned int vertex_count, edge_count = 0;
    cin >> vertex_count;
    cin >> edge_count;


	//  Your program should print out the number of connected components,
	//     i.e. cout << solution << endl;
    // DONE: implement your program here     
	Graph graph(vertex_count, edge_count);
	for (unsigned int i = 0; i < edge_count; ++i) {
		graph.read_edge();
	}

	graph.print_connected();

	return 0;
} 

//Graph::read_edge()
void Graph::read_edge() {
	//read from input file
	int vertex1, vertex2 = 0;
	cin >> vertex1;
	cin >> vertex2;

	size_t idx1 = find_vertex(vertex1);
	size_t idx2 = find_vertex(vertex2);

	//new connected components
	if (idx1 == string::npos && idx2 == string::npos) {
		connected.push_back(unordered_set<int>({ vertex1, vertex2 }));
	}
	//add vertex1 into the vertex2 component
	else if (idx1 == string::npos && idx2 != string::npos) {
		add_vertex(idx2, vertex1);
	}
	//add vertex2 into the vertex1 component
	else if (idx1 != string::npos && idx2 == string::npos) {
		add_vertex(idx1, vertex2);
	}
	else { //idx1, idx2 both found
		if (idx1 < idx2)
			merge_components(idx1, idx2);
		else if (idx1 > idx2)
			merge_components(idx2, idx1);
		//do nothing if idx1 == idx2
	}
}

//Graph::find_vertex()
size_t Graph::find_vertex(int vertex) {
	//find index of the connected component
	for (size_t i = 0; i < connected.size(); ++i) {
		auto find_iter = connected[i].find(vertex);
		if (find_iter != connected[i].end())
			return i;
	}

	//not found
	return string::npos;
}

//Graph::add_vertex()
void Graph::add_vertex(size_t pos, int vertex) {
	connected[pos].insert(vertex);
}

//Graph::merge_components()
void Graph::merge_components(size_t idx1, size_t idx2) {
	//merge connected[idx2] into connected[idx1]
	connected[idx1].insert(connected[idx2].begin(), connected[idx2].end());

	//delete hash table at idx2
	connected.erase(connected.begin() + idx2);
}

//Graph::print_connected()
void Graph::print_connected() {
	//total # of vertices connected in components
	size_t total_connected = 0;
	for (auto iter = connected.begin(); iter != connected.end(); ++iter) {
		total_connected += iter->size();
	}

	//lonely individual vertices
	size_t unconnected = V - total_connected;
	
	cout << unconnected + connected.size() << endl;
}
