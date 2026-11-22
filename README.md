# Grafos em C++

Uma implementação de estruturas e algoritmos para análise de **Grafos não direcionados** desenvolvida em **C++**.

O sistema realiza a leitura de grafos a partir de arquivos de texto utilizando lista de adjacências, permitindo avaliar propriedades estruturais (como completude, regularidade, bipartição e planaridade), detectar ciclos, decompor componentes conexas, reconstruir circuitos eulerianos e executar buscas em profundidade (DFS) e largura (BFS).

---

## 📌 Funcionalidades

* **Representação e Exportação:** Leitura de grafos via arquivo de texto por lista de adjacências e exportação para visualização no formato Graphviz (`DOT`).
* **Propriedades Estruturais:** Cálculo do grau dos vértices e verificação automática de grafo completo ($K_n$), regularidade ($k$-regular), conexidade e condição necessária de planaridade ($E \le 3V - 6$).
* **Bipartição (2-Coloração):** Identificação de grafos bipartidos via DFS com separação explícita dos vértices nos subconjuntos independentes $A$ e $B$.
* **Detecção de Ciclos:** Verificação de ciclicidade com reconstrução do caminho fechado da primeira aresta de retorno (*back-edge*) encontrada.
* **Caminhos e Circuitos Eulerianos:** Verificação de existência pelo Teorema de Euler e reconstrução do circuito euleriano em tempo linear $O(V + E)$ através do **Algoritmo de Hierholzer**.
* **Componentes Conexas e Buscas:** Decomposição do grafo em componentes conectadas, caminhos exploratórios via **DFS** (*Depth-First Search*) e menores caminhos em número de arestas via **BFS** (*Breadth-First Search*).

---

## 🛠️ Tecnologias e Estrutura

* **Linguagem:** C++
* **Estrutura do Repositório:**

  ```text
  Graph/
  ├── Graph.h           # Definição das estruturas Edge e Graph (lista de adjacências e E/S)
  ├── mainGraph.cpp     # Implementação da classe Process com os algoritmos e rotina principal
  ├── README.md         # Documentação do projeto
  └── *.txt / *.text    # Instâncias de grafos para validação e testes
  ```

### 📂 Arquivos de Teste Inclusos

* **`bipartite.txt`**: Grafo de 7 vértices que, apesar do nome, contém um ciclo ímpar (`0 - 1 - 2`), servindo como caso de teste negativo para validar a falha na 2-coloração (`Bipartido: Não`).
* **`complete.text`**: Grafo completo $K_4$ (4 vértices e 6 arestas), onde todos os vértices são adjacentes entre si, validando o teste de completude e regularidade.
* **`mapaViario.txt`**: Grafo conexo de 12 vértices e 20 arestas simulando uma malha viária dividida em três regiões interligadas, ideal para testar rotas via BFS/DFS e graus dos nós.
* **`mediumG.txt`**: Grafo de médio porte (250 vértices e 1.273 arestas) da biblioteca de Sedgewick, utilizado para avaliar desempenho e escalabilidade nas buscas e detecção de ciclos.
* **`notcicle.txt`**: Grafo caminho $P_4$ (4 vértices e 3 arestas: `0 - 1 - 2 - 3`), utilizado para validar grafos acíclicos (árvores) e verificar a ausência de ciclos.
* **`tinyCG.txt`**: Grafo pequeno conexo (*Tiny Connected Graph*, 6 vértices e 8 arestas), ideal para inspeção manual rápida dos caminhos gerados por DFS e BFS.
* **`tinyDAG7.txt`**: Grafo pequeno de 7 vértices (originalmente estruturado como um DAG e interpretado aqui como não direcionado) para testes de topologia e ciclos.
* **`tinyG.txt`**: Grafo pequeno clássico (13 vértices e 13 arestas) dividido em 3 componentes desconexas, usado para validar a contagem e o agrupamento de componentes conectadas.

---

## 🚀 Como Executar

### Pré-requisitos

* Compilador G++ / Clang / MSVC instalado.

### Compilação e Execução

1. **Clone o repositório:**

   ```bash
   git clone [https://github.com/DiogoCarv/Graph.git](https://github.com/DiogoCarv/Graph.git)
   cd Graph
   ```

2. **Compile o código-fonte:**

   ```bash
   g++ mainGraph.cpp -o grafo
   ```

3. **Execute o programa passando um arquivo de grafo como argumento:**

   * **Linux/macOS:**

     ```bash
     ./grafo mediumG.txt
     ```

   * **Windows:**

     ```bash
     grafo.exe mediumG.txt
     ```

---

## 👤 Autor

Desenvolvido por [Diogo Carvalho](https://github.com/DiogoCarv).
