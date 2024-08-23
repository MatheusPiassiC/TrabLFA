#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <set>
#include <sstream>
#include <algorithm> // Para trim

using namespace std;

class Gramatica {
public:
    map<string, vector<string>> regras;

    Gramatica(const string& nomeArquivo) {
        lerGramatica(nomeArquivo);
    }

    void lerGramatica(const string& nomeArquivo) {
        ifstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            cerr << "Erro ao abrir o arquivo!" << endl;
            return;
        }

        string linha;
        while (getline(arquivo, linha)) {
            stringstream ss(linha);
            string esquerda, direita;

            getline(ss, esquerda, '-');
            esquerda = trim(esquerda);

            ss.ignore(2); // Ignora "->"

            while (getline(ss, direita, '|')) {
                direita = trim(direita);

                // Substituir '.' por "λ"
                if (direita == ".") {
                    direita = "λ";
                }

                regras[esquerda].push_back(direita);
            }
        }

        arquivo.close();
    }

    void mostrarRegras() const {
        for (const auto& par : regras) {
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

private:
    string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
};

bool isInPrev(const string &w, const set<string> &prev) {
    for (char c : w) {
        string s(1, c); 
        if (prev.find(s) == prev.end()) {
            return false;
        }
    }
    return true;
}

set<string> findNullable(const map<string, vector<string>> &productions) {
    set<string> nullSet;

    for (const auto &prod : productions) {
        for (const string &rule : prod.second) {
            if (rule == ".") {
                nullSet.insert(prod.first);
            }
        }
    }

    bool changed;
    do {
        set<string> prev = nullSet;
        changed = false;

        for (const auto &prod : productions) {
            string A = prod.first;
            for (const string &rule : prod.second) {
                if (isInPrev(rule, prev)) {
                    if (nullSet.find(A) == nullSet.end()) {
                        nullSet.insert(A);
                        changed = true;
                    }
                }
            }
        }

    } while (changed);

    return nullSet;
}

int main() {
    Gramatica gramatica("gramatica.txt");
    gramatica.mostrarRegras(); // Opcional para exibir as regras lidas

    set<string> nullableSet = findNullable(gramatica.regras);

    cout << "Variáveis anuláveis: ";
    for (const string &c : nullableSet) {
        cout << c << " ";
    }
    cout << endl;

    return 0;
}