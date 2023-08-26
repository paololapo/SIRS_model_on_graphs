#include "Graph.h"

Graph::Graph(std::string type, int numNodes){
    if (type == "Barabasi"){
        generateBarabasiAlbert(numNodes);
    }
}

// Add a new node to the graph
void Graph::insertPoint(const int& p){
    if (adjacencyMap.count(p)){
        std::cout << "Node: " << p << " already in the graph" << std::endl;
    } else {
    adjacencyMap[p];
    }
}

// Add a new edge between two nodes
void Graph::insertEdge(const int& p1, const int& p2){
    adjacencyMap[p1].insert(p2);
    adjacencyMap[p2].insert(p1);
}

// Check if a node exists
bool Graph::hasPoint(const int& p) const{
    return adjacencyMap.count(p);
}

// Check if an edge exists
bool Graph::hasEdge(const int& p1, const int& p2) const{
    // Count directions of the edge
    int dir = 0;
    if (adjacencyMap.count(p1) && adjacencyMap.at(p1).count(p2)){
        dir++;
    }
    if (adjacencyMap.count(p2) && adjacencyMap.at(p2).count(p1)) {
      dir++;
    }

    // Results
    if (dir == 0){
        return false;
    } else if (dir == 2) {
        return true;
    } else{
        throw std::runtime_error("hasEdge(): unexpected result");
    }
}

// Count the nodes in the graph
int Graph::countVertices() const{
    return adjacencyMap.size();
}

// Count all the edges (and isolated points) in the graph
int Graph::countEdges() const{
    // Short name for edge int tupla
    using IntPair = std::pair<int, int>;
    std::unordered_set<IntPair> edgeSet;

    // Search for edges
    int numEdges = 0;

    // Fill the edgeSet
    for (const auto& node : adjacencyMap){
        const auto& p1 = node.first;
        const auto& p1_neighbors = node.second;

        // Isolated point
        if (p1_neighbors.empty()) {
        numEdges++;
        }
        // Edge
        else{
        for (const auto& p2 : p1_neighbors){
            IntPair edge;
            // Edge (p1->p2) == (p2->p1)
            if (p1 < p2){
            edge = std::make_pair(p1,p2);
            }
            else{
            edge = std::make_pair(p2,p1);
            }
            edgeSet.insert(edge);
        }
        }
    }
    // Count edges in the edgeSet
    for ([[maybe_unused]] auto edge : edgeSet){
        numEdges++;
    }

    return numEdges;
}

// Remove a node from the graph
void Graph::removePoint(const int& p1){
    // Point does not exist: return
    if (!hasPoint(p1)) return;

    // Remove edges in the NeighborSet
    const Graph::NeighborSet originalNeighbors = adjacencyMap.at(p1);
    for (auto p2 : originalNeighbors){
        removeEdge(p1, p2);
    }

    // Finally, erase point from the adjacencyMap
    adjacencyMap.erase(p1);
}

// Remove an edge from the graph
void Graph::removeEdge(const int& p1, const int& p2){
    if (hasEdge(p1,p2)) {
      adjacencyMap[p1].erase(p2);
      adjacencyMap[p2].erase(p1);
    }
}

void Graph::saveToCSV(std::string fileName){
    std::ofstream outFile(fileName);
    
    if (!outFile) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }

    for (int node=0; node < countVertices(); node++){
        int numNeighbor = adjacencyMap.at(node).size();
        int i = 0;
        for (int neighbor : adjacencyMap.at(node)){
            outFile << neighbor;
            if (i != numNeighbor-1){
                outFile << ",";
            }
            i++;
        }
        outFile << "\n";
    }
    outFile.close();
}

void Graph::generateBarabasiAlbert(int numNodes){
    // Hyperparametes of the graph
    int m = 2; // BA parameter
    int n0 = 4; // Initial number of nodes
    
    // Add initial nodes
    for (int i = 0; i < n0; ++i) {
        insertPoint(i);
    }

    // Random module
    std::random_device rd;  // Seed
    std::mt19937 gen(rd()); // Mersenne Twister random number generator 
    
    // Complete the graph
    for (int newNode = n0; newNode < numNodes; newNode++) {
        // Compute the probabilities based on the current graph
        std::vector<double> probabilities;
        for (const auto &entry : adjacencyMap) {
            probabilities.push_back(static_cast<double>(entry.second.size()));
        }
        std::discrete_distribution<int> distribution(probabilities.begin(), probabilities.end());

        // Insert node newNode
        insertPoint(newNode);
        
        // Add m edges based on that probabilities
        for (int j = 0; j < m; j++){
            int graphNode = distribution(gen);
            // Insert new edge
            // Note that the edge can already exist
            insertEdge(newNode, graphNode);
        }
    }
}