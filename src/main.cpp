#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

int main(int argc, char *argv[]){
    // verify if the log was provided
    if (argc < 2){
        std::cerr << "Uso: ./graphroute <arquivo_de_log>"; // expected usage format
        return 1;
    }

    // open the log file passed as an argument    
    std::ifstream inputFile(argv[1]);
    // abort if the file can't be read
    if (!inputFile.is_open()) {
        std::cerr << "Falha ao encontrar o arquivo digitado." << std::endl;
        return 1;
    }

        
    // tracks unique edges to avoid duplicates
    std::unordered_set<std::string> seenEdges;
    // tracks unique IP addresses (vertices)
    std::unordered_set<std::string> vertices;

    //adjacency list graph
    std::unordered_map<
        std::string,
        std::unordered_set<std::string>
    > grafo;

    std::string line;

    // skip header line
    std::getline(inputFile, line);

    // split each line into fields and print the hopFrom -> hopTo pair
    while(std::getline(inputFile, line)){
        std::stringstream ss(line);
        std::vector<std::string> fields;
        std::string field;

        while(std::getline(ss, field, ','))
            fields.push_back(field);
        
        // skip malformed lines missing fields
        if (fields.size() < 6)
            continue;

        std::string hopFrom = fields[4];
        std::string hopTo = fields[5];

        // "*" means a network timeout, no edge here
        if (hopTo == "*")
            continue;
        
        // missing endpoint, invalid edge
        if (hopFrom.empty() || hopTo.empty())
            continue;
                
        // unique key for the edge (e.g. "1.1.1.1->2.2.2.2")
        std::string edgeKey = hopFrom + "->" + hopTo;

        // skip if this edge has already been seen
        if (seenEdges.count(edgeKey))
            continue;

        // register edge and both endpoints
        seenEdges.insert(edgeKey);
        vertices.insert(hopFrom);
        vertices.insert(hopTo);

        //add edge to adjency list
        grafo[hopFrom].insert(hopTo);
    }

    // print summary after loading
    std::cout << "\nGrafo de roteamento inicializado!" << std::endl;
    std::cout << "Vertices unicos (IPs): " << vertices.size()
    << " | Arestas: " << seenEdges.size() << std::endl;

    //graph test
    std::cout << "\nLista de adjacencia:\n" << std::endl;

    for(const auto& [origem, destinos] : grafo){
        std::cout << origem << "-> ";
        for(const auto& destino : destinos) {
            std::cout << destino << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}