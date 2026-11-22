#include "Graph.h"
#include <bits/stdc++.h>
#include <limits>

using namespace std;

class Process
{
private:
    Graph G;
    vector<int> cycle;               // Armazena a sequência de vértices do primeiro ciclo detetado
    vector<int> bipartiteColors;     // Vetor de coloração (-1: não visitado, 0: conjunto A, 1: conjunto B)
    bool isBip;                      // Flag booleana que guarda o resultado do teste de bipartição

    // DFS para deteção de ciclos em grafos não direcionados.
    // 'parent' indica o vértice imediatamente anterior para evitar interpretar
    // a aresta bidirecional de regresso (v -> parent) como um ciclo.
    bool dfsCycle(int v, int parent, vector<bool> &marked, vector<int> &edgeTo){
        marked[v] = true;

        for (const Edge &e : G.getAdj(v)){
            // Se já foi encontrado um ciclo noutro ramo da recursão, encerra imediatamente
            if (!cycle.empty()){
                return true;
            }

            int w = e.other(v); // Obtém o vértice vizinho conectado pela aresta

            if (!marked[w]){
                edgeTo[w] = v; // Regista o predecessor para permitir a reconstrução do caminho
                if (dfsCycle(w, v, marked, edgeTo)){
                    return true;
                }
            } else if (w != parent){ 
                // Se 'w' já foi marcado e NÃO é o pai direto na árvore DFS,
                // significa que encontrámos uma aresta de retorno ("back-edge"), logo existe ciclo.
                cycle.clear();

                // Reconstrói o caminho desde 'v' subindo a árvore de predecessores até 'w'
                for (int x = v; x != w; x = edgeTo[x]){
                    cycle.push_back(x);
                }

                cycle.push_back(w);
                cycle.push_back(v); // Fecha o percurso no vértice de partida

                return true;
            }
        }
        return false;
    }

    // DFS para testar a 2-colorabilidade (bipartição).
    // Tenta colorir o grafo com as cores 0 e 1 de forma alternada.
    bool dfsBipartite(int v, int c, vector<int> &color){
        color[v] = c;

        for (const Edge &e : G.getAdj(v)){
            int w = e.other(v);

            if (color[w] == -1){
                // Se o vizinho ainda não foi colorido, atribui a cor oposta (1 - c)
                if (!dfsBipartite(w, 1 - c, color)){
                    return false;
                }
            } else if (color[w] == color[v]){
                // Se o vizinho já possui a mesma cor do vértice atual, o grafo não é bipartido
                return false;
            }
        }
        return true;
    }

    // Executa a verificação de bipartição para todas as componentes conexas
    void checkBipartite(){
        int V = G.getV();
        bipartiteColors.assign(V, -1);
        isBip = true;

        for (int v = 0; v < V; v++){
            if (bipartiteColors[v] == -1){
                if (!dfsBipartite(v, 0, bipartiteColors)){
                    isBip = false;
                    return;
                }
            }
        }
    }

public:
    // Construtor: inicializa a cópia do grafo e pré-computa a bipartição
    Process(const Graph &G) : G(G) {
        checkBipartite();
    }

    // Um grafo simples é completo (Kn) se possuir o número máximo possível de arestas: V*(V-1)/2
    bool isComplete(){
        int n = G.getV();
        int e = G.getE();
        if (n <= 1) return true;
        return e == (n * (n - 1)) / 2;
    }

    // Em grafos não direcionados, o grau de um vértice é o tamanho da sua lista de adjacências
    int degree(int v){
        return G.getAdj(v).size();
    }

    // Um grafo é k-regular se todos os seus vértices possuírem exatamente o mesmo grau
    bool isRegular(){
        int V = G.getV();
        if (V <= 1) return true;

        int targetDegree = degree(0);
        for (int v = 1; v < V; v++){
            if (degree(v) != targetDegree){
                return false;
            }
        }
        return true;
    }

    // Teorema de Euler: Um grafo conexo admite um circuito Euleriano sse todos os vértices têm grau par.
    // Se for desconexo, todas as arestas têm obrigatoriamente de estar na mesma componente conexa.
    bool isEulerian(){
        if (!isConnected()){
            int nonZeroDegreeComp = -1;
            ConnectedComponents cc(G);
            for (int v = 0; v < G.getV(); v++){
                if (degree(v) > 0){
                    if (nonZeroDegreeComp == -1){
                        nonZeroDegreeComp = cc.getId(v);
                    } else if (cc.getId(v) != nonZeroDegreeComp){
                        return false; // Existem arestas em componentes conexas separadas
                    }
                }
            }
        }

        // Todos os vértices devem possuir grau par
        for (int v = 0; v < G.getV(); v++){
            if (degree(v) % 2 != 0){
                return false;
            }
        }
        return true;
    }

    // Teorema de Euler para caminhos abertos:
    // Um grafo conexo admite um caminho Euleriano sse tiver exatamente 2 vértices com grau ímpar.
    bool isEulerianPath(){
        int oddCount = 0;
        int nonZeroDegreeComp = -1;
        ConnectedComponents cc(G);

        for (int v = 0; v < G.getV(); v++){
            if (degree(v) > 0){
                if (nonZeroDegreeComp == -1){
                    nonZeroDegreeComp = cc.getId(v);
                } else if (cc.getId(v) != nonZeroDegreeComp){
                    return false;
                }
            }
            if (degree(v) % 2 != 0){
                oddCount++;
            }
        }
        return oddCount == 2;
    }

    // O grafo é conexo se a partir de um vértice qualquer (ex: 0) todos os outros forem atingíveis
    bool isConnected(){
        int V = G.getV();
        if (V <= 1) return true;

        depthFirstPaths dfs(G, 0);
        for (int v = 0; v < V; v++){
            if (!dfs.hasPathTo(v)){
                return false;
            }
        }
        return true;
    }

    // Retorna se o grafo é bipartido
    bool isBipartite(){
        return isBip;
    }

    // Separa os vértices nos dois subconjuntos independentes A e B (cores 0 e 1)
    pair<vector<int>, vector<int>> getBipartiteSets(){
        if (!isBip) return {{}, {}};
        vector<int> setA, setB;
        for (int v = 0; v < G.getV(); v++){
            if (bipartiteColors[v] == 0) setA.push_back(v);
            else if (bipartiteColors[v] == 1) setB.push_back(v);
        }
        return {setA, setB};
    }

    // Condição necessária da fórmula de Euler para planaridade em grafos simples:
    // Qualquer grafo planar com V >= 3 satisfaz E <= 3V - 6.
    bool isPossiblePlanar(){
        int V = G.getV();
        int E = G.getE();

        if (V <= 2) return true;
        return E <= (3 * V - 6);
    }

    // Dispara a busca em profundidade para verificar a presença de ciclos
    bool isCyclic(){
        int V = G.getV();
        vector<bool> marked(V, false);
        vector<int> edgeTo(V, -1);
        cycle.clear();

        for (int v = 0; v < V; v++){
            if (!marked[v]){
                if (dfsCycle(v, -1, marked, edgeTo)){
                    return true;
                }
            }
        }
        return false;
    }

    // Retorna o vetor contendo o ciclo recuperado
    vector<int> getCycle() const{
        return cycle;
    }

    // Algoritmo de Hierholzer para encontrar o circuito de Euler em tempo linear O(V + E)
    vector<int> getEulerianCycle(){
        if (!isEulerian() || G.getE() == 0){
            return {};
        }

        int V = G.getV();
        
        // Estrutura para manter cada aresta com um identificador único,
        // garantindo que uma aresta bidirecional seja percorrida apenas uma vez.
        struct EdgeRef {
            int to;
            int edgeId;
        };

        vector<vector<EdgeRef>> adj(V);
        int edgeCount = 0;
        for (int u = 0; u < V; u++){
            for (const Edge &e : G.getAdj(u)){
                int w = e.other(u);
                // Adiciona a aresta uma única vez quando u < w para atribuir o mesmo ID aos dois sentidos
                if (u < w){
                    adj[u].push_back({w, edgeCount});
                    adj[w].push_back({u, edgeCount});
                    edgeCount++;
                }
            }
        }

        vector<bool> edgeUsed(edgeCount, false);
        vector<int> currentEdgeIndex(V, 0); // Evita reprocessar arestas já descartadas na lista

        // Localiza o primeiro vértice com arestas para servir de ponto de partida
        int startVertex = 0;
        for (int v = 0; v < V; v++){
            if (degree(v) > 0){
                startVertex = v;
                break;
            }
        }

        stack<int> currPath;
        vector<int> circuit;
        currPath.push(startVertex);

        // Exploração em profundidade: avança até ficar sem arestas disponíveis e depois retrocede
        while (!currPath.empty()){
            int curr = currPath.top();

            // Avança o apontador de arestas ignorando aquelas que já foram consumidas
            while (currentEdgeIndex[curr] < (int)adj[curr].size() && edgeUsed[adj[curr][currentEdgeIndex[curr]].edgeId]){
                currentEdgeIndex[curr]++;
            }

            if (currentEdgeIndex[curr] < (int)adj[curr].size()){
                // Encontrou uma aresta válida: marca como usada e empilha o próximo vértice
                EdgeRef nextEdge = adj[curr][currentEdgeIndex[curr]];
                edgeUsed[nextEdge.edgeId] = true;
                currentEdgeIndex[curr]++;
                currPath.push(nextEdge.to);
            } else {
                // Não há mais arestas saindo de 'curr': adiciona ao circuito final e desempilha
                circuit.push_back(curr);
                currPath.pop();
            }
        }

        // O percurso foi montado na ordem inversa do desempilhamento; invertemos para a ordem correta
        reverse(circuit.begin(), circuit.end());
        return circuit;
    }

    // Classe auxiliar de caminhos baseada em Busca em Profundidade (DFS)
    class depthFirstPaths{
    private:
        vector<bool> marked; // marked[v] = existe caminho da origem 's' até 'v'?
        vector<int> edgeTo;  // edgeTo[v] = vértice predecessor de 'v' no caminho
        const Graph &G;
        int source;

        void dfs(int v){
            marked[v] = true;
            for (const Edge &e : G.getAdj(v)){
                int w = e.other(v);
                if (!marked[w]){
                    edgeTo[w] = v;
                    dfs(w);
                }
            }
        }

    public:
        depthFirstPaths(const Graph &G, int s) : G(G), source(s){
            marked.resize(G.getV(), false);
            edgeTo.resize(G.getV(), -1);
            dfs(s);
        }

        bool hasPathTo(int v) const { return marked[v]; }

        // Reconstrói o caminho de 's' até 'v' usando uma pilha
        stack<int> pathTo(int v) const{
            stack<int> path;
            if (!hasPathTo(v)) return path;

            for (int x = v; x != source; x = edgeTo[x]){
                path.push(x);
            }
            path.push(source);
            return path;
        }
    };

    // Classe auxiliar de caminhos baseada em Busca em Largura (BFS)
    class breadthFirstPaths{
    private:
        vector<bool> marked; // marked[v] = existe um caminho de 's' para 'v'?
        vector<int> edgeTo;  // edgeTo[v] = último vértice no caminho de 's' para 'v'
        vector<int> distTo;  // distTo[v] = distância (em arestas) de 's' para 'v'
        const Graph &G;
        int source;

        void bfs(int s){
            queue<int> q;
            marked[s] = true;
            distTo[s] = 0;
            q.push(s);

            while (!q.empty()){
                int v = q.front();
                q.pop();

                for (const Edge &e : G.getAdj(v)){
                    int w = e.other(v);
                    if (!marked[w]){
                        edgeTo[w] = v;
                        distTo[w] = distTo[v] + 1;
                        marked[w] = true;
                        q.push(w);
                    }
                }
            }
        }

    public:
        breadthFirstPaths(const Graph &G, int s) : G(G), source(s){
            marked.resize(G.getV(), false);
            edgeTo.resize(G.getV(), -1);
            distTo.resize(G.getV(), numeric_limits<int>::max());
            bfs(s);
        }

        bool hasPathTo(int v) const { return marked[v]; }
        int distanceTo(int v) const { return distTo[v]; }

        // Reconstrói o menor caminho de 's' até 'v' usando uma pilha
        stack<int> pathTo(int v) const{
            stack<int> path;
            if (!hasPathTo(v)) return path;

            for (int x = v; x != source; x = edgeTo[x]){
                path.push(x);
            }
            path.push(source);
            return path;
        }
    };

    // Classe para decomposição do grafo nas suas Componentes Conexas (CC)
    class ConnectedComponents{
    private:
        vector<bool> marked;
        vector<int> id;  // id[v] = identificador numérico da componente à qual 'v' pertence
        int count;       // Total de componentes conexas encontradas
        const Graph &G;

        // Preenche recursivamente todos os vértices alcançáveis com o mesmo ID
        void dfs(int v, int componentId){
            marked[v] = true;
            id[v] = componentId;

            for (const Edge &e : G.getAdj(v)){
                int w = e.other(v);
                if (!marked[w]){
                    dfs(w, componentId);
                }
            }
        }

    public:
        ConnectedComponents(const Graph &G) : G(G), count(0){
            marked.resize(G.getV(), false);
            id.resize(G.getV(), -1);

            // Inicia uma nova DFS para cada vértice ainda não visitado
            for (int v = 0; v < G.getV(); v++){
                if (!marked[v]){
                    dfs(v, count);
                    count++;
                }
            }
        }

        int getCount() const { return count; }
        int getId(int v) const { return id[v]; }
        bool connected(int v, int w) const { return id[v] == id[w]; }
    };
};

int main(int argc, char *argv[]){
    if (argc < 2){
        cerr << "Uso: " << argv[0] << " <arquivo_do_grafo>" << endl;
        return 1;
    }

    ifstream in(argv[1]);
    if (!in){
        cerr << "Erro ao abrir o ficheiro: " << argv[1] << endl;
        return 1;
    }

    // Leitura e instanciação do grafo não direcionado
    Graph G(in);
    G.show();
    G.showDot();
    
    // Objeto processador que executa as análises estruturais
    Process graph(G);

    // 1. Mostrar grau dos vértices
    cout << endl << "------- Graus dos Vértices -------" << endl;
    for (int v = 0; v < G.getV(); v++){
        cout << "Vértice " << v << " | Grau: " << graph.degree(v) << endl;
    }

    // 2. Avaliação das propriedades do grafo
    cout << endl <<"------- Classificações do Grafo -------" << endl;
    cout << "Completo: " << (graph.isComplete() ? "Sim" : "Não") << endl;
    cout << "Regular: " << (graph.isRegular() ? "Sim" : "Não") << endl;
    cout << "Conexo: " << (graph.isConnected() ? "Sim" : "Não") << endl;
    cout << "Euleriano: " << (graph.isEulerian() ? "Sim" : "Não") << endl;
    cout << "Possui Caminho Euleriano: " << (graph.isEulerianPath() ? "Sim" : "Não") << endl;
    cout << "Bipartido: " << (graph.isBipartite() ? "Sim" : "Não") << endl;
    cout << "Possível Planar (E <= 3V - 6): " << (graph.isPossiblePlanar() ? "Sim" : "Não") << endl;
    cout << "Contém Ciclos: " << (graph.isCyclic() ? "Sim" : "Não") << endl;

    // 3. Exibição das componentes conexas com os conjuntos de vértices agrupados
    cout << endl << "------- Componentes Conectadas -------" << endl;
    Process::ConnectedComponents cc(G);
    cout << "Total de componentes: " << cc.getCount() << endl;
    vector<vector<int>> compList(cc.getCount());
    for (int v = 0; v < G.getV(); v++){
        compList[cc.getId(v)].push_back(v);
    }
    for (int i = 0; i < cc.getCount(); i++){
        cout << "Componente " << i << ": { ";
        for (int v : compList[i]){
            cout << v << " ";
        }
        cout << "}" << endl;
    }

    // 4. Exibição do circuito de Euler se existir
    cout << endl << "------- Ciclo de Euler -------" << endl;
    if (graph.isEulerian()){
        vector<int> eulerCycle = graph.getEulerianCycle();
        if (!eulerCycle.empty()){
            cout << "Ciclo encontrado: ";
            for (size_t i = 0; i < eulerCycle.size(); i++){
                cout << eulerCycle[i] << (i + 1 < eulerCycle.size() ? " -> " : "");
            }
            cout << endl;
        } else {
            cout << "Grafo sem arestas." << endl;
        }
    } else {
        cout << "O grafo não é Euleriano." << endl;
    }

    // 5. Exibição dos subconjuntos da partição se for bipartido
    cout << endl << "------- Conjuntos Bipartidos -------" << endl;
    if (graph.isBipartite()){
        auto [setA, setB] = graph.getBipartiteSets();
        cout << "Conjunto A: { ";
        for (int v : setA) cout << v << " ";
        cout << "}" << endl;

        cout << "Conjunto B: { ";
        for (int v : setB) cout << v << " ";
        cout << "}" << endl;
    } else {
        cout << "O grafo não é bipartido." << endl;
    }

    // 6. Exibição da sequência do ciclo detetado
    cout << endl << "------ Deteção de Ciclo -------" << endl;
    if (graph.isCyclic()){
        vector<int> c = graph.getCycle();
        cout << "Ciclo encontrado: ";
        for (size_t i = 0; i < c.size(); i++){
            cout << c[i] << (i + 1 < c.size() ? " - " : "");
        }
        cout << endl;
    } else {
        cout << "Nenhum ciclo encontrado (grafo acíclico)." << endl;
    }

    // 7. Exibição dos caminhos via DFS e menores caminhos via BFS a partir da origem 0
    if (G.getV() > 0){
        int origem = 0;

        cout << endl << "------- Caminhos via DFS (Origem: " << origem << ") -------" << endl;
        Process::depthFirstPaths dfs(G, origem);
        for (int v = 0; v < G.getV(); v++){
            if (dfs.hasPathTo(v)){
                stack<int> path = dfs.pathTo(v);
                cout << "Para " << v << ": ";
                while (!path.empty()){
                    cout << path.top();
                    path.pop();
                    if (!path.empty()) cout << " -> ";
                }
                cout << endl;
            } else {
                cout << "Para " << v << ": Inalcançável" << endl;
            }
        }

        cout << endl << "------- Menores Caminhos via BFS (Origem: " << origem << ") -------" << endl;
        Process::breadthFirstPaths bfs(G, origem);
        for (int v = 0; v < G.getV(); v++){
            if (bfs.hasPathTo(v)){
                stack<int> path = bfs.pathTo(v);
                cout << "Para " << v << " (distância: " << bfs.distanceTo(v) << "): ";
                while (!path.empty()){
                    cout << path.top();
                    path.pop();
                    if (!path.empty()) cout << " -> ";
                }
                cout << endl;
            } else {
                cout << "Para " << v << ": Inalcançável" << endl;
            }
        }
    }

    return 0;
}