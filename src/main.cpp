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

    std::string archiveName = argv[1];

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
    > graph;

    std::string line;

    // skip header line
    std::getline(inputFile, line);

    // split each line into fields 
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

        // add edge to adjacency list
        graph[hopFrom].insert(hopTo);

        graph[hopTo];
    }

    // print summary after loading
    std::cout << "\nGrafo de roteamento inicializado!" << std::endl;
    std::cout << "Vertices unicos (IPs): " << vertices.size()
    << " | Arestas: " << seenEdges.size() << std::endl;

    int option;

    do{
        std::cout << "\n==========================================================" << std::endl;
        std::cout << "\n===================== GRAPH ROUTE ========================" << std::endl;
        std::cout << "\n==========================================================" << std::endl;
        std::cout << "1 - Exibir grafo completo" << std::endl;
        std::cout << "2 - Encontrar menor caminho" << std::endl;
        std::cout << "3 - Calcular diametro" << std::endl;
        std::cout << "4 - Identificar roteadores criticos" << std::endl;
        std::cout << "0 - Sair" << std::endl;
        std::cout << "==========================================================" << std::endl;

        std::cout << "\nOpcao: ";
        std::cin >> option;

        switch(option){

            case 1: {
                std::string dotFile = archiveName + ".dot";
                std::ofstream file(dotFile);

                file << "digraph G {" << std::endl;

                for(const auto& pair : graph){
                    for(const auto& destination : pair.second){
                        file << "    \"" << pair.first << "\" -> \"" << destination << "\";" << std::endl;
                    }
                }

                file << "}" << std::endl;
                file.close();

                std::cout << "\nArquivo " << dotFile << " gerado!" << std::endl;

                int outputFormat;
                std::cout << "\nSelecione o formato de saida:" << std::endl;
                std::cout << "1 - Tela" << std::endl;
                std::cout << "2 - Imagem (PNG)" << std::endl;
                std::cout << "3 - Documento (PDF)" << std::endl;
                std::cout << "\nOpcao: ";
                std::cin >> outputFormat;

                switch(outputFormat){
                    case 1 : {
                        std::string command = "dot -Tx11 " + dotFile;
                        system(command.c_str());
                        break;
                    }
                    case 2 : {
                        std::string pngFile = archiveName + ".png";
                        system(("dot -Tpng " + dotFile + " -o " + pngFile).c_str());
                        std::cout << "\nArquivo " << pngFile << " gerado!" << std::endl;
                        break;
                    }
                    case 3 : {
                        std::string pdfFile = archiveName + ".pdf";
                        system(("dot -Tpdf " + dotFile + " -o " + pdfFile).c_str());
                        std::cout << "\nArquivo " << pdfFile << " gerado!" << std::endl;
                        break;  
                    }
                }
                break;
            }

            case 2: {
                std::string source;
                std::string destination;

                std::cout << "\nIP de origem: ";
                std::cin >> source;

                std::cout << "IP de destino: ";
                std::cin >> destination;

                if(!graph.count(source) || !graph.count(destination)){
                    std::cout << "\nIP nao encontrado no grafo."
                              << std::endl;
                    break;
                }

                std::queue<std::string> bfsQueue;
                std::unordered_set<std::string> visited;
                std::unordered_map<std::string, std::string> parent;

                bfsQueue.push(source);
                visited.insert(source);

                bool found = false;

                while(!bfsQueue.empty()){
                    std::string current = bfsQueue.front();
                    bfsQueue.pop();

                    if(current == destination){
                        found = true;
                        break;
                    }

                    for(const auto& neighbor : graph[current]){
                        if(!visited.count(neighbor)){
                            visited.insert(neighbor);
                            parent[neighbor] = current;

                            bfsQueue.push(neighbor);
                        }
                    }
                }

                if(!found){
                    std::cout << "\nNao existe caminho entre os vertices."
                              << std::endl;
                    break;
                }

                std::vector<std::string> path;

                std::string current = destination;

                while(current != source){
                    path.push_back(current);
                    current = parent[current];
                }

                path.push_back(source);

                std::reverse(path.begin(), path.end());

                std::cout << "\nMenor caminho encontrado:\n"
                          << std::endl;

                for(size_t i = 0; i < path.size(); i++){
                    std::cout << path[i];
                    if(i < path.size() - 1)
                        std::cout << " -> ";
                }

                std::cout << "\n\nSaltos: "
                          << path.size() - 1
                          << std::endl;

                std::unordered_set<std::string> pathEdges;
                for(size_t i = 0; i < path.size() - 1; i++){
                    std::string edgeKey = path[i] + "->" + path[i + 1];
                    pathEdges.insert(edgeKey);
                }

                std::string dotFile = archiveName + ".dot";
                std::ofstream file(dotFile);
                file << "digraph G {" << std::endl;

                for(const auto& pair : graph){
                    for(const auto& dest : pair.second){
                        std::string edgeKey = pair.first + "->" + dest;
                        if(pathEdges.count(edgeKey)){
                            file << "    \"" << pair.first << "\" -> \"" << dest << "\" [color=red];" << std::endl;
                        } else {
                            file << "    \"" << pair.first << "\" -> \"" << dest << "\";" << std::endl;
                        }
                    }
                }

                for(size_t i = 0; i < path.size() - 1; i++){
                    file << "    \"" << path[i] << "\" [style=filled, fillcolor=green];" << std::endl;
                }

                file << "    \"" << path.back() << "\" [style=filled, fillcolor=red];" << std::endl;

                file << "}" << std::endl;
                file.close();

                int outputFormat;
                std::cout << "\nSelecione o formato de saida:" << std::endl;
                std::cout << "1 - Tela" << std::endl;
                std::cout << "2 - Imagem (PNG)" << std::endl;
                std::cout << "3 - Documento (PDF)" << std::endl;
                std::cout << "\nOpcao: ";
                std::cin >> outputFormat;

                switch(outputFormat){
                    case 1 : {
                        std::string command = "dot -Tx11 " + dotFile;
                        system(command.c_str());
                        break;
                    }
                    case 2 : {
                        std::string pngFile = archiveName + ".png";
                        system(("dot -Tpng " + dotFile + " -o " + pngFile).c_str());
                        std::cout << "\nArquivo " << pngFile << " gerado!" << std::endl;
                        break;
                    }
                    case 3 : {
                        std::string pdfFile = archiveName + ".pdf";
                        system(("dot -Tpdf " + dotFile + " -o " + pdfFile).c_str());
                        std::cout << "\nArquivo " << pdfFile << " gerado!" << std::endl;
                        break;  
                    }
                }

                break;
            }
         
            case 3: {
                int diameter = 0;

                for(const auto& source : vertices){
                    std::queue<std::string> bfsQueue;
                    std::unordered_map<std::string, int> dist;

                    bfsQueue.push(source);
                    dist[source] = 0;

                    while(!bfsQueue.empty()){
                        std::string current = bfsQueue.front();
                        bfsQueue.pop();

                        for(const auto& neighbor : graph[current]){
                            if(!dist.count(neighbor)){
                                dist[neighbor] = dist[current] + 1;
                                bfsQueue.push(neighbor);

                                if(dist[neighbor] > diameter)
                                    diameter = dist[neighbor];
                            }
                        }
                    }
                }

                std::cout << "\nDiametro do grafo: "
                          << diameter << std::endl;
                break;
            }

            case 4: {
                std::unordered_map<std::string, int> inDegree;

                for(const auto& pair : graph){
                    for(const auto& destination : pair.second){
                        inDegree[destination]++;
                    }
                }
                std::vector<std::pair<std::string, int>> ranking(inDegree.begin(), inDegree.end());
                std::sort(ranking.begin(), ranking.end(),
                    [](const auto& a, const auto& b){
                        return a.second > b.second;
                    }
                );
                std::cout << "\nRoteadores criticos:\n"
                          << std::endl;
                for(size_t i = 0; i < std::min(ranking.size(), size_t(5)); i++){
                    std::cout << ranking[i].first << " (grau de entrada: " << ranking[i].second << ")" << std::endl;
                }
                break;
            }
            case 0: {
                std::cout << "\nEncerrando o programa..."
                          << std::endl;
                break;
            }
            default: {
                std::cout << "\nOpcao invalida."
                          << std::endl;
            }
        }

    } while (option != 0);
    
    return 0;
}