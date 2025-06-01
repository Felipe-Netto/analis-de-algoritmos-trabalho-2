#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

using namespace std;

class Grafo {
private:
    int numVertices;
    int numArestas;
    int labelCapacity;
    string* labels;
    int* degree;
    int** adj;
    int** weight;
    bool directed;
    bool weighted;

    void initStructures(int initialCapacity) {
        if (initialCapacity <= 0) {
            throw invalid_argument("Capacidade inicial deve ser positiva");
        }
        try {
            labelCapacity = initialCapacity;
            labels = new string[labelCapacity];
            degree = new int[labelCapacity];
            for (int i = 0; i < labelCapacity; ++i) {
                degree[i] = 0;
            }
            numVertices = 0;
            adj = nullptr;
            weight = nullptr;
        } catch (const exception& e) {
            throw runtime_error("Falha na alocação de memória durante inicialização");
        }
    }

    void ensureLabelCapacity() {
        if (numVertices < labelCapacity) return;
        try {
            int newCap = labelCapacity * 2;
            string* newLabels = new string[newCap];
            int* newDegree = new int[newCap];
            for (int i = 0; i < numVertices; ++i) {
                newLabels[i] = labels[i];
                newDegree[i] = degree[i];
            }
            for (int i = numVertices; i < newCap; ++i) {
                newDegree[i] = 0;
            }
            delete[] labels;
            delete[] degree;
            labels = newLabels;
            degree = newDegree;
            labelCapacity = newCap;
        } catch (const exception& e) {
            throw runtime_error("Falha na alocação de memória ao expandir capacidade");
        }
    }
    
    int findOrAddLabel(const string& r) {
        if (r.empty()) {
            throw invalid_argument("Rótulo não pode ser vazio");
        }
        for (int i = 0; i < numVertices; ++i) {
            if (labels[i] == r) return i;
        }
        ensureLabelCapacity();
        labels[numVertices] = r;
        degree[numVertices] = 0;
        return numVertices++;
    }

    int findLabel(const string& r) const {
        if (r.empty()) {
            throw invalid_argument("Rótulo não pode ser vazio");
        }
        for (int i = 0; i < numVertices; ++i) {
            if (labels[i] == r) return i;
        }
        return -1;
    }

    void freeAdj() {
        if (adj) {
            try {
                for (int i = 0; i < numVertices; ++i) {
                    delete[] adj[i];
                    if (weighted && weight) {
                        delete[] weight[i];
                    }
                }
                delete[] adj;
                adj = nullptr;
            } catch (...) {
                throw runtime_error("Erro ao liberar memória das matrizes de adjacência");
            }
        }
        if (weighted && weight) {
            delete[] weight;
            weight = nullptr;
        }
    }

    void primeiraPassagem(const string& nomeArquivo) {
        if (nomeArquivo.empty()) {
            throw invalid_argument("Nome do arquivo não pode ser vazio");
        }

        ifstream in(nomeArquivo);
        if (!in.is_open()) {
            throw runtime_error("Não foi possível abrir o arquivo: " + nomeArquivo);
        }

        string linha;
        int lineNum = 0;
        while (getline(in, linha)) {
            lineNum++;
            if (linha.empty()) continue;
            
            size_t p1 = linha.find(';');
            if (p1 == string::npos) {
                in.close();
                throw runtime_error("Formato inválido na linha " + to_string(lineNum) + ": falta primeiro separador");
            }
            
            size_t p2 = linha.find(';', p1 + 1);
            if (p2 == string::npos) {
                in.close();
                throw runtime_error("Formato inválido na linha " + to_string(lineNum) + ": falta segundo separador");
            }

            try {
                string ru = linha.substr(0, p1);
                string rv = linha.substr(p1 + 1, p2 - (p1 + 1));

                int iu = findOrAddLabel(ru);
                int iv = findOrAddLabel(rv);
        
                if (directed) {
                    degree[iu]++;
                } else {
                    degree[iu]++;
                    degree[iv]++;
                }
                numArestas++;
            } catch (const exception& e) {
                in.close();
                throw runtime_error("Erro na linha " + to_string(lineNum) + ": " + e.what());
            }
        }
        in.close();
    }

    void alocaAdjESetDegreeZero() {
        try {
            adj = new int*[numVertices];
            if (weighted) {
                weight = new int*[numVertices];
            }
            for (int i = 0; i < numVertices; ++i) {
                if (degree[i] < 0) {
                    throw runtime_error("Grau negativo detectado para vertice " + labels[i]);
                }
                adj[i] = new int[degree[i]];
                if (weighted) {
                    weight[i] = new int[degree[i]];
                }
                degree[i] = 0;
            }
        } catch (const exception& e) {
            throw runtime_error("Falha na alocação de memória para listas de adjacência");
        }
    }

    void segundaPassagem(const string& nomeArquivo) {
        if (nomeArquivo.empty()) {
            throw invalid_argument("Nome do arquivo não pode ser vazio");
        }

        ifstream in(nomeArquivo);
        if (!in.is_open()) {
            throw runtime_error("Não foi possível abrir o arquivo: " + nomeArquivo);
        }

        string linha;
        int lineNum = 0;
        while (getline(in, linha)) {
            lineNum++;
            if (linha.empty()) continue;
            
            size_t p1 = linha.find(';');
            size_t p2 = linha.find(';', p1 + 1);
            if (p1 == string::npos || p2 == string::npos) {
                in.close();
                throw runtime_error("Formato inválido na linha " + to_string(lineNum));
            }

            try {
                string ru = linha.substr(0, p1);
                string rv = linha.substr(p1 + 1, p2 - (p1 + 1));
                int iu = findLabel(ru);
                int iv = findLabel(rv);

                if (iu < 0 || iv < 0) {
                    throw runtime_error("Vertice nao encontrado: " + (iu < 0 ? ru : rv));
                }

                int w = 1;
                if (weighted) {
                    size_t p3 = linha.find(';', p2 + 1);
                    if (p3 == string::npos) {
                        throw runtime_error("Falta peso na aresta para grafo ponderado");
                    }
                    string rs = linha.substr(p2 + 1, p3 - (p2 + 1));
                    try {
                        w = stoi(rs);
                        if (w < 0) {
                            throw runtime_error("Peso negativo não permitido: " + rs);
                        }
                    } catch (const invalid_argument&) {
                        throw runtime_error("Peso inválido: " + rs);
                    } catch (const out_of_range&) {
                        throw runtime_error("Peso fora do intervalo permitido: " + rs);
                    }
                }

                int idxU = degree[iu]++;
                if (idxU >= numVertices) {
                    throw runtime_error("Indice de adjacencia excede limite para vertice " + ru);
                }
                adj[iu][idxU] = iv;
                if (weighted) {
                    weight[iu][idxU] = w;
                }

                if (!directed) {
                    int idxV = degree[iv]++;
                    if (idxV >= numVertices) {
                        throw runtime_error("Indice de adjacencia excede limite para vertice " + rv);
                    }
                    adj[iv][idxV] = iu;
                    if (weighted) {
                        weight[iv][idxV] = w;
                    }
                }
            } catch (const exception& e) {
                in.close();
                throw runtime_error("Erro na linha " + to_string(lineNum) + ": " + e.what());
            }
        }
        in.close();
    }

    void dfsRec(int u, bool visited[], int** sortedAdj, int* sortedDeg) const {
        if (u < 0 || u >= numVertices) {
            throw out_of_range("Indice de vertice invalido na DFS");
        }
        visited[u] = true;
        cout << labels[u] << " ";
        int d = sortedDeg[u];
        for (int i = 0; i < d; ++i) {
            int v = sortedAdj[u][i];
            if (v < 0 || v >= numVertices) {
                throw out_of_range("Indice de adjacencia invalido na DFS");
            }
            if (!visited[v]) {
                dfsRec(v, visited, sortedAdj, sortedDeg);
            }
        }
    }

public:
    Grafo() {
        try {
            numVertices = 0;
            numArestas = 0;
            labelCapacity = 0;
            labels = nullptr;
            degree = nullptr;
            adj = nullptr;
            weight = nullptr;
            directed = false;
            weighted = false;
            initStructures(8);
        } catch (const exception& e) {
            throw runtime_error("Falha na construção do grafo: " + string(e.what()));
        }
    }

    ~Grafo() {
        try {
            delete[] labels;
            delete[] degree;
            freeAdj();
        } catch (...) {
            cerr << "Erro ao destruir o grafo" << endl;
        }
    }

    Grafo(const Grafo&) = delete;
    Grafo& operator=(const Grafo&) = delete;

    void carregarG1(const string& nomeArquivo) {
        try {
            directed = false;
            weighted = false;
            numVertices = 0;
            numArestas = 0;
            delete[] labels;
            delete[] degree;
            initStructures(8);

            primeiraPassagem(nomeArquivo);
            alocaAdjESetDegreeZero();
            segundaPassagem(nomeArquivo);
        } catch (const exception& e) {
            throw runtime_error("Erro ao carregar G1: " + string(e.what()));
        }
    }

    void carregarG2(const string& nomeArquivo) {
        try {
            directed = true;
            weighted = false;
            numVertices = 0;
            numArestas = 0;
            delete[] labels;
            delete[] degree;
            initStructures(8);

            primeiraPassagem(nomeArquivo);
            alocaAdjESetDegreeZero();
            segundaPassagem(nomeArquivo);
        } catch (const exception& e) {
            throw runtime_error("Erro ao carregar G2: " + string(e.what()));
        }
    }

    void carregarG3(const string& nomeArquivo) {
        try {
            directed = false;
            weighted = true;
            numVertices = 0;
            numArestas = 0;
            delete[] labels;
            delete[] degree;
            initStructures(8);

            primeiraPassagem(nomeArquivo);
            alocaAdjESetDegreeZero();
            segundaPassagem(nomeArquivo);
        } catch (const exception& e) {
            throw runtime_error("Erro ao carregar G3: " + string(e.what()));
        }
    }

    void imprimirAdj() const {
        try {
            for (int i = 0; i < numVertices; ++i) {
                cout << labels[i] << ":";
                int cnt = directed || !weighted ? degree[i] : degree[i];
                for (int j = 0; j < cnt; ++j) {
                    if (adj[i][j] < 0 || adj[i][j] >= numVertices) {
                        throw out_of_range("Índice de adjacência inválido");
                    }
                    cout << " " << labels[adj[i][j]];
                    if (weighted) {
                        cout << "(" << weight[i][j] << ")";
                    }
                }
                cout << "\n";
            }
        } catch (const exception& e) {
            throw runtime_error("Erro ao imprimir grafo: " + string(e.what()));
        }
    }

    void buscaEmLargura(const string& startLabel) const {
        try {
            int start = findLabel(startLabel);
            if (start < 0) {
                throw invalid_argument("Vertice inicial \"" + startLabel + "\" nao existe");
            }

            int** sortedAdj = new int*[numVertices];
            int* sortedDeg = new int[numVertices];
            for (int u = 0; u < numVertices; ++u) {
                int d = degree[u];
                sortedDeg[u] = d;
                int* temp = new int[d];
                for (int i = 0; i < d; ++i) {
                    temp[i] = adj[u][i];
                }
                for (int i = 0; i < d - 1; ++i) {
                    int minIdx = i;
                    for (int j = i + 1; j < d; ++j) {
                        if (labels[temp[j]] < labels[temp[minIdx]]) {
                            minIdx = j;
                        }
                    }
                    int aux = temp[i];
                    temp[i] = temp[minIdx];
                    temp[minIdx] = aux;
                }
                sortedAdj[u] = temp;
            }

            bool* visited = new bool[numVertices];
            for (int i = 0; i < numVertices; ++i) {
                visited[i] = false;
            }

            int* q = new int[numVertices];
            int head = 0, tail = 0;

            visited[start] = true;
            q[tail++] = start;

            cout << endl << "Busca em largura: " << endl;
            cout << "Visitando vertices: ";

            while (head < tail) {
                int u = q[head++];
                if (u < 0 || u >= numVertices) {
                    throw out_of_range("Índice de vertice inválido na BFS");
                }
                cout << labels[u] << " ";

                int d = sortedDeg[u];
                for (int i = 0; i < d; ++i) {
                    int v = sortedAdj[u][i];
                    if (v < 0 || v >= numVertices) {
                        throw out_of_range("Índice de adjacência inválido na BFS");
                    }
                    if (!visited[v]) {
                        visited[v] = true;
                        q[tail++] = v;
                    }
                }
            }
            cout << "\n";

            for (int u = 0; u < numVertices; ++u) {
                delete[] sortedAdj[u];
            }
            delete[] sortedAdj;
            delete[] sortedDeg;
            delete[] visited;
            delete[] q;
        } catch (const exception& e) {
            throw runtime_error("Erro na busca em largura: " + string(e.what()));
        }
    }

    void buscaEmProfundidade(const string& startLabel) const {
        try {
            int start = findLabel(startLabel);
            if (start < 0) {
                throw invalid_argument("Vertice inicial \"" + startLabel + "\" nao existe");
            }

            cout << endl << "Busca em profundidade: " << endl;
            cout << "Visitando vertices: ";

            int** sortedAdj = new int*[numVertices];
            int* sortedDeg = new int[numVertices];
            for (int u = 0; u < numVertices; ++u) {
                int d = degree[u];
                sortedDeg[u] = d;
                int* temp = new int[d];
                for (int i = 0; i < d; ++i) {
                    temp[i] = adj[u][i];
                }
                for (int i = 0; i < d - 1; ++i) {
                    int minIdx = i;
                    for (int j = i + 1; j < d; ++j) {
                        if (labels[temp[j]] < labels[temp[minIdx]]) {
                            minIdx = j;
                        }
                    }
                    int aux = temp[i];
                    temp[i] = temp[minIdx];
                    temp[minIdx] = aux;
                }
                sortedAdj[u] = temp;
            }

            bool* visited = new bool[numVertices];
            for (int i = 0; i < numVertices; ++i) {
                visited[i] = false;
            }

            try {
                dfsRec(start, visited, sortedAdj, sortedDeg);
                cout << "\n";
            } catch (const exception& e) {
                for (int u = 0; u < numVertices; ++u) {
                    delete[] sortedAdj[u];
                }
                delete[] sortedAdj;
                delete[] sortedDeg;
                delete[] visited;
                throw;
            }

            for (int u = 0; u < numVertices; ++u) {
                delete[] sortedAdj[u];
            }
            delete[] sortedAdj;
            delete[] sortedDeg;
            delete[] visited;
        } catch (const exception& e) {
            throw runtime_error("Erro na busca em profundidade: " + string(e.what()));
        }
    }

    void menorCaminho(const string& inicio, const string& destino) const {
        try {
            int s = findLabel(inicio);
            int t = findLabel(destino);
            if (s < 0 || t < 0) {
                throw invalid_argument("Vertice inicial ou final nao existe");
            }

            if (!weighted) {
                throw invalid_argument("Grafo precisa ser ponderado para calcular menor caminho");
            }

            const int INF = 1000000000;
            int* dist = new int[numVertices];
            int* prev = new int[numVertices];
            bool* visited = new bool[numVertices];

            for (int i = 0; i < numVertices; ++i) {
                dist[i] = INF;
                prev[i] = -1;
                visited[i] = false;
            }
            dist[s] = 0;

            try {
                for (int iter = 0; iter < numVertices; ++iter) {
                    int u = -1;
                    int minDist = INF;
                    for (int i = 0; i < numVertices; ++i) {
                        if (!visited[i] && dist[i] < minDist) {
                            minDist = dist[i];
                            u = i;
                        }
                    }
                    if (u == -1) break;
                    visited[u] = true;
                    if (u == t) break;

                    for (int i = 0; i < degree[u]; ++i) {
                        int v = adj[u][i];
                        if (v < 0 || v >= numVertices) {
                            throw out_of_range("Indice de adjacencia invalido");
                        }
                        int w = weight[u][i];
                        if (w < 0) {
                            throw invalid_argument("Peso negativo detectado");
                        }
                        if (!visited[v] && dist[u] + w < dist[v]) {
                            dist[v] = dist[u] + w;
                            prev[v] = u;
                        }
                    }
                }

                if (dist[t] == INF) {
                    cout << "Nao existe caminho de " << inicio << " a " << destino << ".\n";
                    delete[] dist;
                    delete[] prev;
                    delete[] visited;
                    return;
                }

                cout << endl << "Caminho minimo: " << endl;
                cout << "Custo: " << dist[t] << "\n";

                int* path = new int[numVertices];
                int pathSize = 0;
                for (int v = t; v != -1; v = prev[v]) {
                    if (pathSize >= numVertices) {
                        throw runtime_error("Ciclo detectado no caminho");
                    }
                    path[pathSize++] = v;
                }

                cout << "Arestas desse caminho:\n";
                for (int i = pathSize - 1; i > 0; --i) {
                    int u = path[i];
                    int v = path[i - 1];
                    cout << "(" << labels[u] << "," << labels[v] << ")";
                    if (i > 1) cout << " -> ";
                }
                cout << "\n";

                delete[] path;
                delete[] dist;
                delete[] prev;
                delete[] visited;
            } catch (...) {
                delete[] dist;
                delete[] prev;
                delete[] visited;
                throw;
            }
        } catch (const exception& e) {
            throw runtime_error("Erro ao calcular menor caminho: " + string(e.what()));
        }
    }
};
