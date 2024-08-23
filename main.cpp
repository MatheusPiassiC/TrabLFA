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
public:
    map<string, vector<string>> regras;

    Gramatica(const string& nomeArquivo) {
        lerGramatica(nomeArquivo);
    }

    string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
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
            ss.ignore(2);
            esquerda.erase(remove_if(esquerda.begin(), esquerda.end(), ::isspace), esquerda.end());

            while (getline(ss, direita, '|')) {
                direita = trim(direita);
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

    bool isInPrev(const string &w, const set<string> &prev) {
        for (char c : w) {
            string s(1, c);
            if (prev.find(s) == prev.end()) {
                return false;
            }
        }
        return true;
    }

    void acharAnulaveis(set<string>& anulaveis) {
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

        cout << "Variáveis anuláveis: ";
        for (const auto& regra : anulaveis) {
            cout << regra << " ";
        }
        cout << endl;
    }

    void eliminarRegrasLambda() {
        set<string> anulaveis;
        acharAnulaveis(anulaveis);

        // Remover regras λ
        for (auto& producoes : regras) {
            auto& regrasDireitas = producoes.second;
            regrasDireitas.erase(remove(regrasDireitas.begin(), regrasDireitas.end(), "."), regrasDireitas.end());
        }

        // Adicionar novas regras omitindo variáveis anuláveis
        map<string, vector<string>> novasRegras;
        for (const auto& producoes : regras) {
            string A = producoes.first;
            for (const string& regra : producoes.second) {
                vector<string> combinacoes = {""};

                for (char c : regra) {
                    string s(1, c);
                    if (anulaveis.find(s) != anulaveis.end()) {
                        vector<string> novasCombinacoes;
                        for (const string& comb : combinacoes) {
                            novasCombinacoes.push_back(comb + s);
                            novasCombinacoes.push_back(comb);
                        }
                        combinacoes = novasCombinacoes;
                    } else {
                        for (string& comb : combinacoes) {
                            comb += s;
                        }
                    }
                }

                for (const string& novaRegra : combinacoes) {
                    if (!novaRegra.empty()) {
                        novasRegras[A].push_back(novaRegra);
                    }
                }
            }
        }

        // Atualizar a gramática com as novas regras
        for (auto& producoes : novasRegras) {
            for (const string& novaRegra : producoes.second) {
                if (find(regras[producoes.first].begin(), regras[producoes.first].end(), novaRegra) == regras[producoes.first].end()) {
                    regras[producoes.first].push_back(novaRegra);
                }
            }
        }
    }
};

int main(int argc, const char** argv) {
    Gramatica gramatica("gramatica.txt");

    gramatica.eliminarRegrasLambda();
    gramatica.mostrarRegras();

    return 0;
}

    


    return 0;

}
