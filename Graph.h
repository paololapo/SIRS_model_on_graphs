#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <sstream>
#include <fstream>
#include <random>

class Graph {
    public:       
        // Constructors      
        Graph() {}
        Graph(std::string type, int numNodes);
    
        // Graph by adjacency map
        using NeighborSet = std::unordered_set<int>;
        std::unordered_map<int, Graph::NeighborSet> adjacencyMap;

        // Insert points and edges
        void insertPoint(const int& p);
        void insertEdge(const int& p1, const int& p2);

        // Get details about the graph
        bool hasPoint(const int& p) const;
        bool hasEdge(const int& p1, const int& p2) const;

        int countVertices() const;
        int countEdges() const;

        void saveToCSV(std::string fileName);

        // Remove points and edges
        void removePoint(const int& p1);
        void removeEdge(const int& p1, const int& p2);

        // Check equality and inequality
        bool operator==(const Graph& other) const {
            return (adjacencyMap == other.adjacencyMap);
        }
        bool operator!=(const Graph& other) const {
            return !(*this == other);
        }
    
    private:
        void generateBarabasiAlbert(int numNodes);

};

// Code to handle std::unordered_set of std::pair<int, int> 
using IntPair = std::pair<int, int>;
namespace std {
  template <>
  struct hash<IntPair> {
    std::size_t operator() (const IntPair& p) const {
        std::string uniqueIntPairString = std::to_string(p.first) + "##" + std::to_string(p.second);
        std::hash<std::string> stringHasher;
        return stringHasher(uniqueIntPairString);
    }
  };

}