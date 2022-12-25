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

//Algorithm 2 - explore each time and do a breadth-first search to cover all connected
//				explore untill all vertex has been explored

#include <stdio.h>
#include <vector>
#include <iostream>
#include <queue>
using namespace std;

class Graph{
    unsigned int V;
    unsigned int E;
    // TODO: add any additional member variables, as needed
	vector<bool> explored;
	vector<vector<unsigned int>> adjlist;
	unsigned int num_connected = 0;

public:
    // Graph constructor that initializes the graph and its member variables
	Graph(unsigned int V, unsigned int E) : V(V), E(E), explored(V, false), 
		adjlist(V), num_connected(0) {}
        // DONE: initialize member variables that you added above


    //read edges
	void read_edge();

	//find connected components
	void find_connection();

	//print # of connected components
	void print_connected();

};

int main(){
    //  Your program should print out the number of connected components,
    //     i.e. cout << solution << endl;
    std::ios_base::sync_with_stdio(false);

	//read # of vertex and edge
    unsigned int vertex_count, edge_count = 0;
    cin >> vertex_count;
    cin >> edge_count;
    Graph graph(vertex_count, edge_count);    

	graph.read_edge();
	graph.find_connection();
	graph.print_connected();

	return 0;

} 

//Graph::read_edge()
void Graph::read_edge() {
	for (unsigned int i = 0; i < E; ++i) {
		unsigned int vertex1, vertex2 = 0;
		cin >> vertex1;
		cin >> vertex2;

		adjlist[vertex1].push_back(vertex2);
		adjlist[vertex2].push_back(vertex1);
	}
}

//Graph::find_connection()
void Graph::find_connection() {
	for (unsigned int i = 0; i < explored.size(); ++i) {
		if (explored[i] == false) {
			++num_connected;

			//do a breadth-first search to find all connected vertex from i
			queue<unsigned int> connected({ i });
			while (!connected.empty()) {
				unsigned int current = connected.front();
				connected.pop();
				explored[current] = true;

				//add all vertex adjacent to 'current'
				for (auto iter = adjlist[current].begin(); iter != adjlist[current].end(); ++iter) {
					if(!explored[*iter])
						connected.push(*iter);
				}
			}
		}
	}
}

//Graph::print_connected()
void Graph::print_connected() {
	cout << num_connected << endl;
}