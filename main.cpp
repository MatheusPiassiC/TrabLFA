#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <string>

using namespace std;


class Gramatica {
    /*

    S −> aS | bS | C | D
    C −> c | .
    D −> abc
    D −> .

    */

public:
    // Hash map para armazenar a gramática
    map<string, vector<string>> regras;

    Gramatica(const string& nomeArquivo) {
        lerGramatica(nomeArquivo);
    }

    // Função para ler o arquivo e armazenar a gramática em um hash map
    void lerGramatica(const string& nomeArquivo) {
        ifstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            cerr << "Erro ao abrir o arquivo!" << endl;
            return;
        }

        string linha;
        while (getline(arquivo, linha)) {
            // Separar a parte esquerda e direita da produção
            stringstream ss(linha);
            string esquerda, direita;

            getline(ss, esquerda, '-'); // Obtém a parte esquerda (não-terminal)
            ss.ignore(2); // Ignora "->"

            // Dividir as produções à direita pelo delimitador '|'
            while (getline(ss, direita, '|')) {
                // Adicionar a produção à lista de regras
                regras[esquerda].push_back(direita);
            }
        }

        arquivo.close();
    }

    // Função para exibir as regras
    void mostrarRegras() const {
        for (auto& par : regras) {
            cout << par.first << " -> ";
            for (size_t i = 0; i < par.second.size(); ++i) {
                cout << par.second[i];
                if (i != par.second.size() - 1) {
                    cout << " | ";
                }
            }
            cout << endl;
        }
    }

    void removeRecursao(){
        cout << "Remove Rec" << endl;
        for (auto producao : regras["S"]) {
            cout << producao << endl;
        }
    }
};

int main(int argc, const char** argv) {


    /*

    S −> aS | bS | C | D
    C −> c | .
    D −> abc
    D −> .

    */
    Gramatica gramatica("gramatica.txt");
    //gramatica.mostrarRegras();

    gramatica.mostrarRegras();

    gramatica.removeRecursao();
    


    return 0;

}