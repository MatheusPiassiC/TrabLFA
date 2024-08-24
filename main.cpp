#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include <cctype> 

using namespace std;

class Gramatica {
public:
    map<string, vector<string>> regras;
    map<string, set<string>> closures;

    Gramatica(const string& nomeArquivo) {
        lerGramatica(nomeArquivo);
        removerRecursividadeInicial();
        eliminarRegrasLambda(); // Assumimos que a eliminação de regras lambda já foi realizada.
        buildClosures();\
        removeChainRules();
        removerSimbolosInuteis();
        removerVariaveisInalcancaveis();
        transformarParaFNC();
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

    void transformarParaFNC() {
        map<string, string> novosTerminais;

        map<string, map<string,string>> novasRegras;
        
        // Parte 1: Substituir símbolos de regras cujo tamanho seja maior que 1 por não terminais
        for (auto& producoes : regras) {
            for (string& regra : producoes.second) {
                if (regra.size() > 1) {
                    string novaRegra;
                    for (char& simbolo : regra) { // CAC  // c / C
                        string s(1, simbolo);
                        if (islower(simbolo)) {
                            if (novosTerminais.find(s) == novosTerminais.end()) {
                                string novoSimbolo = string(1, toupper(simbolo)) + "'";
                                novosTerminais[s] = novoSimbolo;
                            }
                            // INSIRA O COMANDO AQUI
                            novaRegra += novosTerminais[s]; // Substitui o terminal pelo novo não-terminal
                        }
                        else {
                            novaRegra += simbolo;
                        }
                    }
                    novasRegras[producoes.first][regra] = novaRegra;
                    // for (const auto& outerPair : novasRegras) {
                    //     cout << "Categoria: " << outerPair.first << endl;
                    //         for (const auto& innerPair : outerPair.second) {
                    //             cout << "  Regra: " << innerPair.first << " -> Valor: " << innerPair.second << endl;
                    //         }
                    // }


                for (auto& chaves : novasRegras) {
                    string categoria = chaves.first;

                    // Iterar sobre o vetor de regras anteriores da mesma categoria
                    for (auto& regraPrev : regras[categoria]) {
                        // Verificar se regraPrev existe no mapa interno de novasRegras
                        if (chaves.second.find(regraPrev) != chaves.second.end()) {
                            // Substituir o valor da regra anterior pelo valor correspondente em novasRegras
                            regraPrev = chaves.second.at(regraPrev);
                        }
                    }
                }
                    //cout << novaRegra << endl;
                }
            }
        }
        map<string, int> novoSimboloContador;
        //Parte 2: Dividir todas as regras de tamanho maior que 2 de forma sequencial
    for (auto& producoes : regras) {
        vector<string> novasProducoes;
        for (string regra : producoes.second) {
            while (regra.size() > 2) {
                string novoSimbolo = "T" + to_string(++novoSimboloContador[producoes.first]);
                novasProducoes.push_back(regra.substr(0, 2) + novoSimbolo);
                regra = novoSimbolo + regra.substr(2);
            }
            novasProducoes.push_back(regra);
        }
        producoes.second = novasProducoes;
    }
        

        // Inserir as novas regras para os terminais substituídos
        for (const auto& par : novosTerminais) {
            regras[par.second].push_back(par.first);
        }
    }
};

    
int main(int argc, const char** argv) {
    Gramatica gramatica("gramatica.txt");

    gramatica.mostrarRegras();

    return 0;
}
