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
    map<string, set<string>> closures;

    Gramatica(const string& nomeArquivo) {
        lerGramatica(nomeArquivo);
        removerRecursividadeInicial();
        eliminarRegrasLambda(); // Assumimos que a eliminação de regras lambda já foi realizada.
        // buildClosures();
        // removeChainRules();
        // removerSimbolosInuteis();
        // removerVariaveisInalcancaveis();
    }

    string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    void removerRecursividadeInicial(){
        bool recursao = false;
        for (auto& producao : regras["S"]) {
            for (char letra: producao){
                if (letra == 'S'){
                    cout << letra << endl;
                    recursao = true;
                }
            }
        }
        if(recursao == true){
            regras["S'"].push_back("S");
        }
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
        }

    void eliminarRegrasLambda() {
        set<string> anulaveis;
        acharAnulaveis(anulaveis);

        if (anulaveis.find("S") != anulaveis.end()) {
            if(regras.find("S'") != regras.end()){
                regras["S'"].push_back(".");
                auto& regrasS = regras["S"];
                regrasS.erase(remove(regrasS.begin(), regrasS.end(), "."), regrasS.end());
            }
        }

            // Remover regras λ
            for (auto& producoes : regras) {
                if (producoes.first != "S'") {
                    auto& regrasDireitas = producoes.second;
                    regrasDireitas.erase(remove(regrasDireitas.begin(), regrasDireitas.end(), "."), regrasDireitas.end());
                }
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

        void buildClosures() {
            for (const auto& rule : regras) {
                string A = rule.first;
                closures[A].insert(A);
                for (const string& B : rule.second) {
                    if (regras.find(B) != regras.end()) {
                        closures[A].insert(B);
                    }
                }
            }

            bool changed;
            do {
                changed = false;
                for (auto& closure : closures) {
                    string A = closure.first;
                    set<string> newSet = closure.second;
                    for (const string& B : closure.second) {
                        newSet.insert(closures[B].begin(), closures[B].end());
                    }
                    if (newSet.size() > closures[A].size()) {
                        closures[A] = newSet;
                        changed = true;
                    }
                }
            } while (changed);
        }

        void removeChainRules() {
            for (const auto& closure : closures) {
                string A = closure.first;
                set<string> newRules;
                for (const string& B : closure.second) {
                    if (A != B) {
                        for (const string& rule : regras[B]) {
                            if (regras.find(rule) == regras.end()) {
                                newRules.insert(rule);
                            }
                        }
                    }
                }
                for (const string& newRule : newRules) {
                    if (find(regras[A].begin(), regras[A].end(), newRule) == regras[A].end()) {
                        regras[A].push_back(newRule);
                    }
                }
            }

            for (auto& regra : regras) {
                set<string> toRemove;
                for (const string& B : regra.second) {
                    if (regras.find(B) != regras.end()) {
                        toRemove.insert(B);
                    }
                }
                for (const string& r : toRemove) {
                    auto& regrasDireitas = regra.second;
                    regrasDireitas.erase(remove(regrasDireitas.begin(), regrasDireitas.end(), r), regrasDireitas.end());
                }
            }
        }
        
        void removerSimbolosInuteis() {
        set<string> terminais;

        // Passo 1: Identificar variáveis que geram terminais diretamente
        for (const auto& producoes : regras) {
            string A = producoes.first;
            for (const string& regra : producoes.second) {
                if (all_of(regra.begin(), regra.end(), [](char c) { return islower(c); })) {
                    terminais.insert(A);
                    break;
                }
            }
        }

        // Passo 2: Propagar os terminais
        bool mudou;
        do {
            mudou = false;
            set<string> novosTerminais = terminais;
            for (const auto& producoes : regras) {
                string A = producoes.first;
                for (const string& regra : producoes.second) {
                    if (all_of(regra.begin(), regra.end(), [&terminais](char c) { return islower(c) || terminais.count(string(1, c)); })) {
                        if (novosTerminais.insert(A).second) {
                            mudou = true;
                        }
                    }
                }
            }
            terminais = novosTerminais;
        } while (mudou);

        // Passo 3: Remover regras com variáveis inúteis
        for (auto it = regras.begin(); it != regras.end(); ) {
            if (!terminais.count(it->first)) {
                it = regras.erase(it);
            } else {
                auto& regrasDireitas = it->second;
                regrasDireitas.erase(
                    remove_if(regrasDireitas.begin(), regrasDireitas.end(), [&terminais](const string& regra) {
                        return any_of(regra.begin(), regra.end(), [&terminais](char c) {
                            return isupper(c) && !terminais.count(string(1, c));
                        });
                    }),
                    regrasDireitas.end()
                );
                ++it;
            }
        }
    }

        void removerVariaveisInalcancaveis() {
        set<string> reach = {"S'"}; // Inicia com a variável inicial
        set<string> prev;
        bool mudou;

        do {
            set<string> newReach = reach;
            prev = reach;

            for (const string& A : prev) {
                if (regras.find(A) != regras.end()) {
                    for (const string& regra : regras[A]) {
                        for (char c : regra) {
                            string s(1, c);
                            if (isupper(c)) {
                                newReach.insert(s);
                            }
                        }
                    }
                }
            }

            mudou = (newReach != reach);
            reach = newReach;

        } while (mudou);

        // Remover regras que contêm variáveis inalcançáveis
        for (auto it = regras.begin(); it != regras.end(); ) {
            if (reach.find(it->first) == reach.end()) {
                it = regras.erase(it);
            } else {
                ++it;
            }
        }
    }
};

    
int main(int argc, const char** argv) {
    Gramatica gramatica("gramatica.txt");

    gramatica.mostrarRegras();

    return 0;
}
