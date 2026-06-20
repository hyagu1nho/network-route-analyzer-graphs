# Network Route Analyzer — Grafos

Analisador de topologia de rede construído a partir de logs de traceroute. O programa lê um arquivo `.log` contendo registros de saltos entre roteadores, constrói um **grafo direcionado não ponderado** em memória e disponibiliza um menu interativo para consultas e visualizações.

Projeto desenvolvido para a disciplina de **Estruturas de Dados II** /  UPF.

---

## Integrantes

| Nome | GitHub |
|------|--------|
| Hyago Tonelli | [@hyagu1nho](https://github.com/hyagu1nho) |
| Henrique Prediger | [@HPrediger](https://github.com/HPrediger) |


---

## Professor

> Leonardo Costella

---

## Dependências

- **Compilador C++17** — `g++` (GCC 9+)
- **Graphviz** — necessário para geração de visualizações (PNG, PDF, tela)

### Instalando o Graphviz (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install graphviz
```

---

## Compilação

```bash
g++ -std=c++17 -O2 -o graphroute main.cpp
```

---

## Execução

```bash
./graphroute <arquivo_de_log>
```

**Exemplo:**

```bash
./graphroute input_1.log
```

---

## Formato do arquivo de entrada

O programa aceita arquivos `.log` com campos separados por vírgula. A primeira linha é o cabeçalho e é ignorada. Cada linha subsequente representa um salto de rede:

```
prb_id,probe_src,dst_addr,hop,hop_from,hop_to,rtt
```

| Campo       | Descrição                                          |
|-------------|----------------------------------------------------|
| `prb_id`    | Identificador da sonda/medição                     |
| `probe_src` | IP de origem da sonda                              |
| `dst_addr`  | IP de destino final                                |
| `hop`       | Número sequencial do salto                         |
| `hop_from`  | IP do roteador que enviou o pacote **(vértice origem)** |
| `hop_to`    | IP do roteador que recebeu o pacote **(vértice destino)** |
| `rtt`       | Round-Trip Time acumulado (ms)                     |

Linhas com `hop_to = *` (timeout) ou com campos `hop_from`/`hop_to` em branco são descartadas automaticamente.

---

## Funcionalidades

```
==========================================================
===================== GRAPH ROUTE ========================
==========================================================
1 - Exibir grafo completo
2 - Encontrar menor caminho
3 - Calcular diametro
4 - Identificar roteadores criticos
0 - Sair
==========================================================
```

### 1. Exibir Grafo Completo
Gera um arquivo `.dot` com a topologia completa e oferece exportação para:
- **Tela** — abre janela interativa via Graphviz
- **PNG** — salva `<arquivo>.png` na pasta atual
- **PDF** — salva `<arquivo>.pdf` na pasta atual

### 2. Encontrar Menor Caminho
Solicita dois IPs (origem e destino) e calcula o caminho com o **menor número de saltos** usando BFS. Exibe a sequência de IPs e o total de saltos. Também gera visualização com o caminho destacado.

### 3. Calcular Diâmetro do Grafo
Calcula e exibe o **diâmetro** do grafo — a maior distância mínima entre qualquer par de vértices alcançáveis.

### 4. Identificar Roteadores Críticos
Exibe o **Top 5 IPs com maior grau de entrada (in-degree)**, identificando os principais hubs de infraestrutura presentes no log.

---

## Justificativa das escolhas técnicas

### Estrutura de dados — Lista de Adjacência com `unordered_map` + `unordered_set`

A lista de adjacência foi implementada como `unordered_map<string, unordered_set<string>>`. Essa combinação oferece:
- **O(1) amortizado** para inserção e busca de vértices e arestas
- **Eliminação automática de arestas duplicadas** pelo `unordered_set`, sem necessidade de verificação manual
- Eficiência de memória proporcional ao número real de arestas (esparsidade típica de grafos de rede)

### Menor caminho — BFS (Busca em Largura)

Como o grafo é **não ponderado**, o BFS garante que o primeiro caminho encontrado até o destino é necessariamente o de menor número de saltos. Complexidade: **O(V + E)**.

### Diâmetro — BFS a partir de cada vértice

O diâmetro é calculado executando BFS a partir de todos os vértices e rastreando a maior distância mínima encontrada. Complexidade: **O(V × (V + E))**, adequada para o volume dos logs fornecidos.

### Roteadores críticos — contagem de in-degree

Para cada aresta `u → v` inserida no grafo, o grau de entrada de `v` é incrementado em um `unordered_map<string, int>`. Ao final, os cinco maiores valores são extraídos. Complexidade: **O(V log V)** para ordenação parcial.

### Visualização — Graphviz (formato DOT)

O formato `.dot` foi escolhido por ser o padrão da ferramenta Graphviz, amplamente disponível em sistemas Linux e compatível com os formatos de saída exigidos (tela, PNG, PDF). A geração do arquivo é feita pelo próprio programa e a renderização é delegada ao binário `dot` via `system()`.

