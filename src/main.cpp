#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

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
                
        std::cout << hopFrom << " -> " << hopTo << std::endl;   
    }
    return 0;
}