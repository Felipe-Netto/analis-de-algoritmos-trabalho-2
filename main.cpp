#include <iostream>
#include <string>
#include <sstream>
#include "grafo.h"

using namespace std;

void mostrarMenu() {
    cout << "\n=== Menu de Operacoes ===\n";
    cout << "1. Imprimir lista de adjacencias do G1 (nao-direcionado)\n";
    cout << "2. Imprimir lista de adjacencias do G2 (direcionado)\n";
    cout << "3. Imprimir lista de adjacencias do G3 (ponderado)\n";
    cout << "4. Busca em largura (G1)\n";
    cout << "5. Busca em profundidade (G2)\n";
    cout << "6. Caminho minimo (G3)\n";
    cout << "0. Sair\n";
    cout << "Escolha uma opcao: ";
}

int main() {
    Grafo g1, g2, g3;
    string entrada, verticeInicio, verticeDestino;
    int opcao;

    cout << "=== Sistema de Analise de Grafos ===\n";
    cout << "Carregando grafos...\n";

    try {
        g1.carregarG1("grafos/g1.txt");
        g2.carregarG2("grafos/g2.txt");
        g3.carregarG3("grafos/g3.txt");
        cout << "Grafos carregados com sucesso!\n";
    } catch (const exception& e) {
        cout << "Erro ao carregar grafos: " << e.what() << endl;
        return 1;
    }

    do {
        mostrarMenu();
        cin >> opcao;
        cin.ignore(); // Limpa o buffer do teclado

        try {
            switch (opcao) {
                case 1:
                    cout << "\nGrafo G1 (nao-direcionado):\n";
                    g1.imprimirAdj();
                    break;

                case 2:
                    cout << "\nGrafo G2 (direcionado):\n";
                    g2.imprimirAdj();
                    break;

                case 3:
                    cout << "\nGrafo G3 (ponderado):\n";
                    g3.imprimirAdj();
                    break;

                case 4:
                    cout << "Digite o vertice inicial para busca em largura no G1: ";
                    getline(cin, verticeInicio);
                    g1.buscaEmLargura(verticeInicio);
                    break;

                case 5:
                    cout << "Digite o vertice inicial para busca em profundidade no G2: ";
                    getline(cin, verticeInicio);
                    g2.buscaEmProfundidade(verticeInicio);
                    break;

                case 6: {
                    cout << "Digite os vertices inicial e final separados por espaco: ";
                    getline(cin, entrada);
                    istringstream iss(entrada);
                    if (iss >> verticeInicio >> verticeDestino) {
                        g3.menorCaminho(verticeInicio, verticeDestino);
                    } else {
                        cout << "Erro: Formato invalido. Use: vertice_inicial vertice_final\n";
                    }
                    break;
                }

                case 0:
                    cout << "Encerrando o programa...\n";
                    break;

                default:
                    cout << "Opcao invalida!\n";
            }
        } catch (const exception& e) {
            cout << "Erro: " << e.what() << endl;
        }

    } while (opcao != 0);

    return 0;
}
