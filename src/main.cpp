#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <algorithm>

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

        grafo[hopTo];
    }

    // print summary after loading
    std::cout << "\nGrafo de roteamento inicializado!" << std::endl;
    std::cout << "Vertices unicos (IPs): " << vertices.size()
    << " | Arestas: " << seenEdges.size() << std::endl;

    int opcao;

    do{
        std::cout << "\n===== GRAPH ROUTE ======" << std::endl;
        std::cout << "1 - Exibir estatisticas do grafo" << std::endl;
        std::cout << "2 - Encontrar menor caminho" << std::endl;
        std::cout << "3 - Calcular diametro" << std::endl;
        std::cout << "4 - Identificar roteadores criticos" << std::endl;
        std::cout << "0 - Sair" << std::endl;

        std::cout << "\nOpcao: ";
        std::cin >> opcao;

        switch(opcao){

            case 1:
                std::cout << "\nEstatisticas do grafo" << std::endl;
                std::cout << "Vertices: "
                          << vertices.size()
                          << std::endl;

                std::cout << "Arestas: "
                          << seenEdges.size()
                          << std::endl;
                break;

            case 2: {
                std::string origem;
                std::string destino;

                std::cout << "\nIP de origem:";
                std::cin >> origem;

                std::cout << "IP de destino: ";
                std::cin >> destino;

                if(!grafo.count(origem) || !grafo.count(destino)){
                    std::cout << "\nIP nao encontrado no grafo."
                              << std::endl;
                    break;
                }

                std::queue<std::string> fila;
                std::unordered_set<std::string> visitados;
                std::unordered_map<std::string, std::string> pai;

                fila.push(origem);
                visitados.insert(origem);

                bool find = false;

                while(!fila.empty()){
                    std::string atual = fila.front();
                    fila.pop();

                    if(atual == destino){
                        find = true;
                        break;
                    }

                    for(const auto& vizinho : grafo[atual]){
                        if(!visitados.count(vizinho)){
                            visitados.insert(vizinho);
                            pai[vizinho] = atual;

                            fila.push(vizinho);
                        }
                    }
                }

                if(!find){
                    std::cout << "\nNao existe caminho entre os vertices."
                              << std::endl;
                    break;
                }

                std::vector<std::string> caminho;

                std::string atual = destino;

                while(atual != origem){
                    caminho.push_back(atual);
                    atual = pai[atual];
                }

                caminho.push_back(origem);

                std::reverse(caminho.begin(), caminho.end());

                std::cout << "\nMenor caminho encontrado:\n"
                          << std::endl;

                for(size_t i = 0; i < caminho.size(); i++){
                    std::cout << caminho[i];
                    if(i < caminho.size() - 1)
                        std::cout << " -> ";
                }

                std::cout << "\n\nSaltos: "
                          << caminho.size() - 1
                          << std::endl;
                break;
            }
         
            case 3: {
                int diametro = 0;

                for(const auto& origem : vertices){
                    std::queue<std::string> fila;
                    std::unordered_map<std::string, int>dist;

                    fila.push(origem);
                    dist[origem] = 0;

                    while(!fila.empty()){
                        std::string atual = fila.front();
                        fila.pop();

                        for(const auto& vizinho : grafo[atual]){
                            if(!dist.count(vizinho)){
                                dist[vizinho] = dist[atual] + 1;
                                fila.push(vizinho);

                                if(dist[vizinho] > diametro)
                                    diametro = dist[vizinho];
                            }
                        }

                    }
                }

                std::cout << "\nDiametro do grafo:"
                          << diametro << std::endl;
                break;
            }
          

            case 4:
                std::cout << "\nteste"
                          << std::endl;
                break;

            case 0:
                std::cout << "\nEncerrando o programa..."
                          << std::endl;
                break;

            default:
                std::cout << "\nOpcao invalida."
                          << std::endl;
        }

    } while (opcao != 0);
    
    return 0;
}