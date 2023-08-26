#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <numeric>
#include <cmath>

#include "Graph.h"

// Open a .csv file and store the data as doubles
std::vector<std::vector<double>> openCSV(std::string fileName);

// Save the time evolution into a .csv file
void saveToCSV(const std::vector<std::vector<int>>& data, const std::string& filename);

// Single step of the disease spreading
std::vector<int> singleStep(const std::vector<int>& prevStep, const Graph& G, const int& numNodes, const std::vector<std::vector<double>>& mutations, const std::vector<std::vector<double>>& disease, double reinfectionCoeff);

// Chose events based on an array of partial probabilities
int chooseEvent(const std::vector<double>& partialProbabilities);

// Reinfection function
double timeDependentProbability(int timeStep, double coeff);

// First step of the disease spreading
std::vector<int> firstStep(int numNodes, int numInfect);


int main() {
    // Create graph
    Graph G("Barabasi", 200); 
    G.saveToCSV("./files/graph.csv");
    int numNodes = G.countVertices();

    // Open .csv files
    std::vector<std::vector<double>> mutations;
    mutations = openCSV("./files/mutations.csv");    //mutation[0][1] = prob(0->1)
    
    std::vector<std::vector<double>> disease;
    disease = openCSV("./files/disease.csv");        //disease[0][0] transmission 0, disease[0][1] recovery 0 

    //Time evolution
    int timeSteps = 1000;
    double reinfectionCoeff = 0.01;

    std::vector<std::vector<int>> evolution;
    std::vector<int> initStep = firstStep(numNodes, 10);
    evolution.push_back(initStep);

    for (int t=0; t<timeSteps; t++){
        std::vector<int> curStep = evolution.back();
        evolution.push_back(singleStep(curStep, G, numNodes, mutations, disease, reinfectionCoeff));
    }

    saveToCSV(evolution, "./files/evolution.csv");
    
    
    return 0;
}


std::vector<std::vector<double>> openCSV(std::string fileName){
    std::vector<std::vector<double>> data;
    
    // Open file
    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("openCSV(): failed to open the file");
    }
    // Read file
    std::string line;
    while (getline(file, line)) {
        std::vector<double> row;
        std::stringstream ss(line);
        std::string cell;

        while (getline(ss, cell, ';')) {
            try {
                double value = std::stod(cell);
                row.push_back(value);
            } catch (const std::exception& e) {
                throw std::runtime_error("openCSV(): error reading cell: " + cell);
            }
        }
        data.push_back(row);
    }
    return data;
}


std::vector<int> singleStep(const std::vector<int>& prevStep, const Graph& G, const int& numNodes, const std::vector<std::vector<double>>& mutations, const std::vector<std::vector<double>>& disease, double reinfectionCoeff){
    std::vector<int> nextStep = prevStep;

    std::random_device rd;
    std::mt19937 generator(rd()); 
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    
    // Infection
    for (int node=0; node<numNodes; node++){   
        if (prevStep[node] > 0) {
            // Contacts
            for (int neighbor : G.adjacencyMap.at(node)){
                double randomNum = distribution(generator);
                if (disease[prevStep[node]-1][0] > randomNum && prevStep[neighbor] == 0){
                    // Actual infection
                    nextStep[neighbor] = prevStep[node];
                }
            }
        }
    }


    // Mutation
    for (int node=0; node<numNodes; node++){
        if (prevStep[node] > 0){
            // Get the mutation probabilities for the node variant
            std::vector<double> mutationProbabilities = mutations[prevStep[node]-1];
            int mutation = chooseEvent(mutationProbabilities);
            if (mutation > 0){
                // Actual mutation
                nextStep[node] = mutation+1;
            } 
        }
    }

    // Recovery
    for (int node=0; node<numNodes; node++){
        // If infect and not mutated
        if (prevStep[node] > 0 && nextStep[node] == prevStep[node]){
            double randomNum = distribution(generator);
            if (disease[prevStep[node]-1][1] > randomNum){
                // Actual recovery
                nextStep[node] = -1;
            }        
        }
    }

    // Reinfection
    for (int node=0; node<numNodes; node++){
        if (prevStep[node] < 0){
            double randomNum = distribution(generator);
            double prob = timeDependentProbability(prevStep[node], reinfectionCoeff);
            if (prob > randomNum){
                nextStep[node] = 0;
            } else{
                nextStep[node]--;
            }
        }
    }

    return nextStep;
}


int chooseEvent(const std::vector<double>& partialProbabilities) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    // Calculate cumulative probabilities
    std::vector<double> cumulativeProbabilities;
    std::partial_sum(partialProbabilities.begin(), partialProbabilities.end(), std::back_inserter(cumulativeProbabilities));

    // Generate a random number between 0 and 1
    double randomValue = distribution(generator);

    // Find the index of the chosen event
    for (int chosenEvent = 0; chosenEvent < static_cast<int>(cumulativeProbabilities.size()); chosenEvent++) {
        if (randomValue <= cumulativeProbabilities[chosenEvent]) {
            return chosenEvent;  // Return the chosen event
        }
    }

    return -1;  // No event occurred
}


double timeDependentProbability(int timeStep, double coeff){
    // Expected negative time step
    if (timeStep >= 0){
        throw std::runtime_error("Something wrong with recovery");
    }
    double time = static_cast<double>(timeStep);
    return 1 - exp(time*coeff);
}


std::vector<int> firstStep(int numNodes, int numInfect){
    std::vector<int> initStep;
    for (int i=0; i<numNodes; i++){
        initStep.push_back(0);
    }

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, numNodes);

    for (int j=0; j<numInfect; j++){
        int randNode = distribution(generator);
        // First variant
        initStep[randNode] = 1; 
    }

    return initStep;
}


void saveToCSV(const std::vector<std::vector<int>>& data, const std::string& filename) {
    std::ofstream outFile(filename);
    
    if (!outFile) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            outFile << row[i];
            if (i != row.size() - 1) {
                outFile << ",";  // Add comma between values
            }
        }
        outFile << "\n";  // Newline after each row
    }

    outFile.close();
}