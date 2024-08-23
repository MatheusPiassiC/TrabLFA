#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>

using namespace std;


class Gramatica {
    /*

    S −> aS | bS | C | D
    C −> c | .
    D −> abc
    D −> .

    */
private: 
    map<string, vector<string>> regras;

public:

    string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
    // Hash map para armazenar a gramática

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
            esquerda.erase(remove_if(esquerda.begin(), esquerda.end(), ::isspace), esquerda.end());

            // Dividir as produções à direita pelo delimitador '|'
            while (getline(ss, direita, '|')) {
                // Adicionar a produção à lista de regras
                direita = trim(direita);
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

    bool isInPrev(const string &w, const set<string> &prev) {
        for (char c : w) {
            string s(1, c); 
            if (prev.find(s) == prev.end()) {
                return false;
            }
        }
        return true;
    }


    void acharAnulaveis() {
        
        cout << "Achar anuvelais" << endl;
        set<string> anulaveis;

        for (const auto &producoes : regras) {
            for (const string &rule : producoes.second) {
                if (rule == ".") {
                    anulaveis.insert(producoes.first);
                }
            }
        }

        bool changed;
        do {
            set<string> prev = anulaveis;
            changed = false;

            for (const auto &producoes : regras) {
                string A = producoes.first;
                for (const string &rule : producoes.second) {
                    if (isInPrev(rule, prev)) {
                        if (anulaveis.find(A) == anulaveis.end()) {
                            anulaveis.insert(A);
                            changed = true;
                        }
                    }
                }
            }

        } while (changed);


        for( auto& regra : anulaveis){
            cout << regra << " ";
        }

        for (auto& regra : anulaveis){
            
        }

        return;
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
    gramatica.acharAnulaveis();

    //gramatica.removeRecursao();
    


    return 0;

}