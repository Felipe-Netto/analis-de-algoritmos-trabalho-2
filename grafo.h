// Felipe Giovanne Alves Netto - 2147115
// Luana Ribeiro Solly - 2093108
// Gabriel Oda - 2178645

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

using namespace std;

class Grafo {
private:
    int numVertices;
    int numArestas;
    int capacidadeRotulos;
    string* rotulos;
    int* grau;
    int** adj;
    int** peso;
    bool direcionado;
    bool ponderado;

    void inicializarEstruturas(int capacidadeInicial) {
        if (capacidadeInicial <= 0) {
            throw invalid_argument("Capacidade inicial deve ser positiva");
        }
        try {
            capacidadeRotulos = capacidadeInicial;
            rotulos = new string[capacidadeRotulos];
            grau = new int[capacidadeRotulos];
            for (int i = 0; i < capacidadeRotulos; ++i) {
                grau[i] = 0;
            }
            numVertices = 0;
            adj = nullptr;
            peso = nullptr;
        } catch (const exception& e) {
            throw runtime_error("Falha na alocacao de memoria durante inicializacao");
        }
    }

    void garantirCapacidadeRotulos() {
        if (numVertices < capacidadeRotulos) return;
        try {
            int novaCapacidade = capacidadeRotulos * 2;
            string* novosRotulos = new string[novaCapacidade];
            int* novosGraus = new int[novaCapacidade];
            for (int i = 0; i < numVertices; ++i) {
                novosRotulos[i] = rotulos[i];
                novosGraus[i] = grau[i];
            }
            for (int i = numVertices; i < novaCapacidade; ++i) {
                novosGraus[i] = 0;
            }
            delete[] rotulos;
            delete[] grau;
            rotulos = novosRotulos;
            grau = novosGraus;
            capacidadeRotulos = novaCapacidade;
        } catch (const exception& e) {
            throw runtime_error("Falha na alocacao de memoria ao expandir capacidade");
        }
    }
    
    int encontrarOuAdicionarRotulo(const string& r) {
        if (r.empty()) {
            throw invalid_argument("Rotulo nao pode ser vazio");
        }
        for (int i = 0; i < numVertices; ++i) {
            if (rotulos[i] == r) return i;
        }
        garantirCapacidadeRotulos();
        rotulos[numVertices] = r;
        grau[numVertices] = 0;
        return numVertices++;
    }

    int encontrarRotulo(const string& r) const {
        if (r.empty()) {
            throw invalid_argument("Rotulo nao pode ser vazio");
        }
        for (int i = 0; i < numVertices; ++i) {
            if (rotulos[i] == r) return i;
        }
        return -1;
    }

    void liberarAdj() {
        if (adj) {
            try {
                for (int i = 0; i < numVertices; ++i) {
                    delete[] adj[i];
                    if (ponderado && peso) {
                        delete[] peso[i];
                    }
                }
                delete[] adj;
                adj = nullptr;
            } catch (...) {
                throw runtime_error("Erro ao liberar memoria das matrizes de adjacencia");
            }
        }
        if (ponderado && peso) {
            delete[] peso;
            peso = nullptr;
        }
    }

    void primeiraPassagem(const string& nomeArquivo) {
        if (nomeArquivo.empty()) {
            throw invalid_argument("Nome do arquivo nao pode ser vazio");
        }

        ifstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            throw runtime_error("Nao foi possivel abrir o arquivo: " + nomeArquivo);
        }

        string linha;
        int numLinha = 0;
        while (getline(arquivo, linha)) {
            numLinha++;
            if (linha.empty()) continue;
            
            size_t p1 = linha.find(';');
            if (p1 == string::npos) {
                arquivo.close();
                throw runtime_error("Formato invalido na linha " + to_string(numLinha) + ": falta primeiro separador");
            }
            
            size_t p2 = linha.find(';', p1 + 1);
            if (p2 == string::npos) {
                arquivo.close();
                throw runtime_error("Formato invalido na linha " + to_string(numLinha) + ": falta segundo separador");
            }

            try {
                string rotuloU = linha.substr(0, p1);
                string rotuloV = linha.substr(p1 + 1, p2 - (p1 + 1));

                int indiceU = encontrarOuAdicionarRotulo(rotuloU);
                int indiceV = encontrarOuAdicionarRotulo(rotuloV);
        
                if (direcionado) {
                    grau[indiceU]++;
                } else {
                    grau[indiceU]++;
                    grau[indiceV]++;
                }
                numArestas++;
            } catch (const exception& e) {
                arquivo.close();
                throw runtime_error("Erro na linha " + to_string(numLinha) + ": " + e.what());
            }
        }
        arquivo.close();
    }

    void alocarAdjEZerarGrau() {
        try {
            adj = new int*[numVertices];
            if (ponderado) {
                peso = new int*[numVertices];
            }
            for (int i = 0; i < numVertices; ++i) {
                if (grau[i] < 0) {
                    throw runtime_error("Grau negativo detectado para vertice " + rotulos[i]);
                }
                adj[i] = new int[grau[i]];
                if (ponderado) {
                    peso[i] = new int[grau[i]];
                }
                grau[i] = 0;
            }
        } catch (const exception& e) {
            throw runtime_error("Falha na alocacao de memoria para listas de adjacencia");
        }
    }

    void segundaPassagem(const string& nomeArquivo) {
        if (nomeArquivo.empty()) {
            throw invalid_argument("Nome do arquivo nao pode ser vazio");
        }

        ifstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            throw runtime_error("Nao foi possivel abrir o arquivo: " + nomeArquivo);
        }

        string linha;
        int numLinha = 0;
        while (getline(arquivo, linha)) {
            numLinha++;
            if (linha.empty()) continue;
            
            size_t p1 = linha.find(';');
            size_t p2 = linha.find(';', p1 + 1);
            if (p1 == string::npos || p2 == string::npos) {
                arquivo.close();
                throw runtime_error("Formato invalido na linha " + to_string(numLinha));
            }

            try {
                string rotuloU = linha.substr(0, p1);
                string rotuloV = linha.substr(p1 + 1, p2 - (p1 + 1));
                int indiceU = encontrarRotulo(rotuloU);
                int indiceV = encontrarRotulo(rotuloV);

                if (indiceU < 0 || indiceV < 0) {
                    throw runtime_error("Vertice nao encontrado: " + (indiceU < 0 ? rotuloU : rotuloV));
                }

                int valorPeso = 1;
                if (ponderado) {
                    size_t p3 = linha.find(';', p2 + 1);
                    if (p3 == string::npos) {
                        throw runtime_error("Falta peso na aresta para grafo ponderado");
                    }
                    string strPeso = linha.substr(p2 + 1, p3 - (p2 + 1));
                    try {
                        valorPeso = stoi(strPeso);
                        if (valorPeso < 0) {
                            throw runtime_error("Peso negativo nao permitido: " + strPeso);
                        }
                    } catch (const invalid_argument&) {
                        throw runtime_error("Peso invalido: " + strPeso);
                    } catch (const out_of_range&) {
                        throw runtime_error("Peso fora do intervalo permitido: " + strPeso);
                    }
                }

                int idxU = grau[indiceU]++;
                if (idxU >= numVertices) {
                    throw runtime_error("Indice de adjacencia excede limite para vertice " + rotuloU);
                }
                adj[indiceU][idxU] = indiceV;
                if (ponderado) {
                    peso[indiceU][idxU] = valorPeso;
                }

                if (!direcionado) {
                    int idxV = grau[indiceV]++;
                    if (idxV >= numVertices) {
                        throw runtime_error("Indice de adjacencia excede limite para vertice " + rotuloV);
                    }
                    adj[indiceV][idxV] = indiceU;
                    if (ponderado) {
                        peso[indiceV][idxV] = valorPeso;
                    }
                }
            } catch (const exception& e) {
                arquivo.close();
                throw runtime_error("Erro na linha " + to_string(numLinha) + ": " + e.what());
            }
        }
        arquivo.close();
    }

    void buscaProfundidadeRecursiva(int u, bool visitado[], int** adjOrdenado, int* grauOrdenado) const {
        if (u < 0 || u >= numVertices) {
            throw out_of_range("Indice de vertice invalido na busca em profundidade");
        }
        visitado[u] = true;
        cout << rotulos[u] << " ";
        int d = grauOrdenado[u];
        for (int i = 0; i < d; ++i) {
            int v = adjOrdenado[u][i];
            if (v < 0 || v >= numVertices) {
                throw out_of_range("Indice de adjacencia invalido na busca em profundidade");
            }
            if (!visitado[v]) {
                buscaProfundidadeRecursiva(v, visitado, adjOrdenado, grauOrdenado);
            }
        }
    }

public:
    Grafo() {
        try {
            numVertices = 0;
            numArestas = 0;
            capacidadeRotulos = 0;
            rotulos = nullptr;
            grau = nullptr;
            adj = nullptr;
            peso = nullptr;
            direcionado = false;
            ponderado = false;
            inicializarEstruturas(8);
        } catch (const exception& e) {
            throw runtime_error("Falha na construcao do grafo: " + string(e.what()));
        }
    }

    ~Grafo() {
        try {
            delete[] rotulos;
            delete[] grau;
            liberarAdj();
        } catch (...) {
            cerr << "Erro ao destruir o grafo" << endl;
        }
    }

    Grafo(const Grafo&) = delete;
    Grafo& operator=(const Grafo&) = delete;

    void carregarG1(const string& nomeArquivo) {
        try {
            direcionado = false;
            ponderado = false;
            numVertices = 0;
            numArestas = 0;
            delete[] rotulos;
            delete[] grau;
            inicializarEstruturas(8);

            primeiraPassagem(nomeArquivo);
            alocarAdjEZerarGrau();
            segundaPassagem(nomeArquivo);
        } catch (const exception& e) {
            throw runtime_error("Erro ao carregar G1: " + string(e.what()));
        }
    }

    void carregarG2(const string& nomeArquivo) {
        try {
            direcionado = true;
            ponderado = false;
            numVertices = 0;
            numArestas = 0;
            delete[] rotulos;
            delete[] grau;
            inicializarEstruturas(8);

            primeiraPassagem(nomeArquivo);
            alocarAdjEZerarGrau();
            segundaPassagem(nomeArquivo);
        } catch (const exception& e) {
            throw runtime_error("Erro ao carregar G2: " + string(e.what()));
        }
    }

    void carregarG3(const string& nomeArquivo) {
        try {
            direcionado = false;
            ponderado = true;
            numVertices = 0;
            numArestas = 0;
            delete[] rotulos;
            delete[] grau;
            inicializarEstruturas(8);

            primeiraPassagem(nomeArquivo);
            alocarAdjEZerarGrau();
            segundaPassagem(nomeArquivo);
        } catch (const exception& e) {
            throw runtime_error("Erro ao carregar G3: " + string(e.what()));
        }
    }

    void imprimirAdj() const {
        try {
            for (int i = 0; i < numVertices; ++i) {
                cout << rotulos[i] << ":";
                int cont = direcionado || !ponderado ? grau[i] : grau[i];
                for (int j = 0; j < cont; ++j) {
                    if (adj[i][j] < 0 || adj[i][j] >= numVertices) {
                        throw out_of_range("Indice de adjacencia invalido");
                    }
                    cout << " " << rotulos[adj[i][j]];
                    if (ponderado) {
                        cout << "(" << peso[i][j] << ")";
                    }
                }
                cout << "\n";
            }
        } catch (const exception& e) {
            throw runtime_error("Erro ao imprimir grafo: " + string(e.what()));
        }
    }

    void buscaEmLargura(const string& rotuloInicial) const {
        try {
            int inicio = encontrarRotulo(rotuloInicial);
            if (inicio < 0) {
                throw invalid_argument("Vertice inicial \"" + rotuloInicial + "\" nao existe");
            }

            int** adjOrdenado = new int*[numVertices];
            int* grauOrdenado = new int[numVertices];
            for (int u = 0; u < numVertices; ++u) {
                int d = grau[u];
                grauOrdenado[u] = d;
                int* temp = new int[d];
                for (int i = 0; i < d; ++i) {
                    temp[i] = adj[u][i];
                }
                for (int i = 0; i < d - 1; ++i) {
                    int idxMin = i;
                    for (int j = i + 1; j < d; ++j) {
                        if (rotulos[temp[j]] < rotulos[temp[idxMin]]) {
                            idxMin = j;
                        }
                    }
                    int aux = temp[i];
                    temp[i] = temp[idxMin];
                    temp[idxMin] = aux;
                }
                adjOrdenado[u] = temp;
            }

            bool* visitado = new bool[numVertices];
            for (int i = 0; i < numVertices; ++i) {
                visitado[i] = false;
            }

            int* fila = new int[numVertices];
            int inicio_fila = 0, fim_fila = 0;

            visitado[inicio] = true;
            fila[fim_fila++] = inicio;

            cout << endl << "Busca em largura: " << endl;
            cout << "Visitando vertices: ";

            while (inicio_fila < fim_fila) {
                int u = fila[inicio_fila++];
                if (u < 0 || u >= numVertices) {
                    throw out_of_range("Indice de vertice invalido na busca em largura");
                }
                cout << rotulos[u] << " ";

                int d = grauOrdenado[u];
                for (int i = 0; i < d; ++i) {
                    int v = adjOrdenado[u][i];
                    if (v < 0 || v >= numVertices) {
                        throw out_of_range("Indice de adjacencia invalido na busca em largura");
                    }
                    if (!visitado[v]) {
                        visitado[v] = true;
                        fila[fim_fila++] = v;
                    }
                }
            }
            cout << "\n";

            for (int u = 0; u < numVertices; ++u) {
                delete[] adjOrdenado[u];
            }
            delete[] adjOrdenado;
            delete[] grauOrdenado;
            delete[] visitado;
            delete[] fila;
        } catch (const exception& e) {
            throw runtime_error("Erro na busca em largura: " + string(e.what()));
        }
    }

    void buscaEmProfundidade(const string& rotuloInicial) const {
        try {
            int inicio = encontrarRotulo(rotuloInicial);
            if (inicio < 0) {
                throw invalid_argument("Vertice inicial \"" + rotuloInicial + "\" nao existe");
            }

            cout << endl << "Busca em profundidade: " << endl;
            cout << "Visitando vertices: ";

            int** adjOrdenado = new int*[numVertices];
            int* grauOrdenado = new int[numVertices];
            for (int u = 0; u < numVertices; ++u) {
                int d = grau[u];
                grauOrdenado[u] = d;
                int* temp = new int[d];
                for (int i = 0; i < d; ++i) {
                    temp[i] = adj[u][i];
                }
                for (int i = 0; i < d - 1; ++i) {
                    int idxMin = i;
                    for (int j = i + 1; j < d; ++j) {
                        if (rotulos[temp[j]] < rotulos[temp[idxMin]]) {
                            idxMin = j;
                        }
                    }
                    int aux = temp[i];
                    temp[i] = temp[idxMin];
                    temp[idxMin] = aux;
                }
                adjOrdenado[u] = temp;
            }

            bool* visitado = new bool[numVertices];
            for (int i = 0; i < numVertices; ++i) {
                visitado[i] = false;
            }

            try {
                buscaProfundidadeRecursiva(inicio, visitado, adjOrdenado, grauOrdenado);
                cout << "\n";
            } catch (const exception& e) {
                for (int u = 0; u < numVertices; ++u) {
                    delete[] adjOrdenado[u];
                }
                delete[] adjOrdenado;
                delete[] grauOrdenado;
                delete[] visitado;
                throw;
            }

            for (int u = 0; u < numVertices; ++u) {
                delete[] adjOrdenado[u];
            }
            delete[] adjOrdenado;
            delete[] grauOrdenado;
            delete[] visitado;
        } catch (const exception& e) {
            throw runtime_error("Erro na busca em profundidade: " + string(e.what()));
        }
    }

    void menorCaminho(const string& inicio, const string& destino) const {
        try {
            int s = encontrarRotulo(inicio);
            int t = encontrarRotulo(destino);
            if (s < 0 || t < 0) {
                throw invalid_argument("Vertice inicial ou final nao existe");
            }

            if (!ponderado) {
                throw invalid_argument("Grafo precisa ser ponderado para calcular menor caminho");
            }

            const int INFINITO = 1000000000;
            int* distancia = new int[numVertices];
            int* anterior = new int[numVertices];
            bool* visitado = new bool[numVertices];

            for (int i = 0; i < numVertices; ++i) {
                distancia[i] = INFINITO;
                anterior[i] = -1;
                visitado[i] = false;
            }
            distancia[s] = 0;

            try {
                for (int iter = 0; iter < numVertices; ++iter) {
                    int u = -1;
                    int menorDist = INFINITO;
                    for (int i = 0; i < numVertices; ++i) {
                        if (!visitado[i] && distancia[i] < menorDist) {
                            menorDist = distancia[i];
                            u = i;
                        }
                    }
                    if (u == -1) break;
                    visitado[u] = true;
                    if (u == t) break;

                    for (int i = 0; i < grau[u]; ++i) {
                        int v = adj[u][i];
                        if (v < 0 || v >= numVertices) {
                            throw out_of_range("Indice de adjacencia invalido");
                        }
                        int w = peso[u][i];
                        if (w < 0) {
                            throw invalid_argument("Peso negativo detectado");
                        }
                        if (!visitado[v] && distancia[u] + w < distancia[v]) {
                            distancia[v] = distancia[u] + w;
                            anterior[v] = u;
                        }
                    }
                }

                if (distancia[t] == INFINITO) {
                    cout << "Nao existe caminho de " << inicio << " a " << destino << ".\n";
                    delete[] distancia;
                    delete[] anterior;
                    delete[] visitado;
                    return;
                }

                cout << endl << "Caminho minimo: " << endl;
                cout << "Custo: " << distancia[t] << "\n";

                int* caminho = new int[numVertices];
                int tamCaminho = 0;
                for (int v = t; v != -1; v = anterior[v]) {
                    if (tamCaminho >= numVertices) {
                        throw runtime_error("Ciclo detectado no caminho");
                    }
                    caminho[tamCaminho++] = v;
                }

                cout << "Arestas desse caminho:\n";
                for (int i = tamCaminho - 1; i > 0; --i) {
                    int u = caminho[i];
                    int v = caminho[i - 1];
                    cout << "(" << rotulos[u] << "," << rotulos[v] << ")";
                    if (i > 1) cout << " -> ";
                }
                cout << "\n";

                delete[] caminho;
                delete[] distancia;
                delete[] anterior;
                delete[] visitado;
            } catch (...) {
                delete[] distancia;
                delete[] anterior;
                delete[] visitado;
                throw;
            }
        } catch (const exception& e) {
            throw runtime_error("Erro ao calcular menor caminho: " + string(e.what()));
        }
    }
};
