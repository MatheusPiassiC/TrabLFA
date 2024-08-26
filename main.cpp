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
    map<string, set<string>> fechos;

    //realiza a leitura de arquivo
    Gramatica(const string& nomeArquivo) {
        lerGramatica(nomeArquivo);
    }


    // Funcao que remove espaços em branco no inicio e ao final de uma string e vai ser chamada na funcao de leitura da gramatica
    string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    // Função para ler a gramática a partir de um arquivo
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

    //Funcao que cria um novo S' que aponta pra S 
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

    /*
    Metodo que vai ser chamado na funcao acharAnulaveis para verificar se um produção inteira é composta apenas por simbolos anulaveis 
    Os simbolos anulaveis sao registrado em anterior
    */
    bool todosSimbolosAnulaveis(const string &producao, const set<string> &simbolosAnulaveis) {
        for (char simbolo : producao) {
            string simboloStr(1, simbolo);
            if (simbolosAnulaveis.find(simboloStr) == simbolosAnulaveis.end()) {
                return false;
            }
        }
        return true;
    }

    void acharAnulaveis(set<string>& anulaveis) {
        for (const auto &producoes : regras) {
            for (const string &regra : producoes.second) {
                if (regra == ".") {
                    anulaveis.insert(producoes.first);
                }
            }
        }

        bool mudou;
        do {
            set<string> prev = anulaveis;
            mudou = false;

            for (const auto &producoes : regras) {
                string A = producoes.first;
                for (const string &regra : producoes.second) {
                    if (todosSimbolosAnulaveis(regra, prev)) {
                        if (anulaveis.find(A) == anulaveis.end()) {
                            anulaveis.insert(A);
                            mudou = true;
                        }
                    }
                }
            }

        } while (mudou);
    }

    void eliminarRegrasLambda() {
        set<string> anulaveis;
        acharAnulaveis(anulaveis);

        //Caso S gere lambda, esse trecho remove essa derivacao da variavel e a inclui em S' 
        if (anulaveis.find("S") != anulaveis.end()) {
            regras["S'"].push_back(".");
            auto& regrasS = regras["S"];
            regrasS.erase(remove(regrasS.begin(), regrasS.end(), "."), regrasS.end());
        }

        // Remover regras λ com excessao na variavel S'
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


    //Metodo feito para criar o fecho transitvo das regras de cadeia que vao ser utilizados na funcao removerRegrasCadeia()
    void criarFechos() {
        for (const auto& rule : regras) {
            string A = rule.first;
            fechos[A].insert(A);
            for (const string& B : rule.second) {
                if (regras.find(B) != regras.end()) {
                    fechos[A].insert(B);
                }
            }
        }

        bool mudou;
        do {
            mudou = false;
            for (auto& fecho : fechos) {
                string A = fecho.first;
                set<string> novoSet = fecho.second;
                for (const string& B : fecho.second) {
                    novoSet.insert(fechos[B].begin(), fechos[B].end());
                }
                if (novoSet.size() > fechos[A].size()) {
                    fechos[A] = novoSet;
                    mudou = true;
                }
            }
        } while (mudou);
    }

    void removerRegrasCadeia() {
        for (const auto& fecho : fechos) {
            string A = fecho.first;
            set<string> novasRegras;
            for (const string& B : fecho.second) {
                if (A != B) {
                    for (const string& rule : regras[B]) {
                        if (regras.find(rule) == regras.end()) {
                            novasRegras.insert(rule);
                        }
                    }
                }
            }
            for (const string& novaRegra : novasRegras) {
                if (find(regras[A].begin(), regras[A].end(), novaRegra) == regras[A].end()) {
                    regras[A].push_back(novaRegra);
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

        // PRimeira etapa: Identificar variáveis que geram terminais diretamente
        for (const auto& producoes : regras) {
            string A = producoes.first;
            for (const string& regra : producoes.second) {
                if (all_of(regra.begin(), regra.end(), [](char c) { return islower(c); })) {
                    terminais.insert(A);
                    break;
                }
            }
        }

        // Segunda etapa: Propagar os terminais
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

        // Terceira etapa: Remover regras com variáveis inúteis
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
        set<string> reach;
            if(regras.find("S'") != regras.end()){
                reach = {"S'"}; // Inicia com a variável inicial
            }
            else{
                reach = {"S"};
            }
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

    //Primeira parte do processo de converssao a FNC
    void substituirTerminais() {
        map<string, string> terminaisParaNaoTerminais; 

        // Primeira etapa: Substituir terminais em produções com tamanho maior que 1
        for (auto& producao : regras) {
            vector<string> novasRegras;

            for (const string& regra : producao.second) {
                if (regra.size() > 1) {
                    string novaRegra;

                    for (char c : regra) {
                        if (islower(c)) { 
                            string terminal(1, c);
                            if (terminaisParaNaoTerminais.find(terminal) == terminaisParaNaoTerminais.end()) {
                                string novoNaoTerminal = string(1, toupper(c)) + "'";
                                terminaisParaNaoTerminais[terminal] = novoNaoTerminal;
                                regras[novoNaoTerminal].push_back(terminal);
                            }
                            novaRegra += terminaisParaNaoTerminais[terminal];
                        } else {
                            novaRegra += c;
                        }
                    }

                    novasRegras.push_back(novaRegra); // Adicionar a nova regra à lista de regras
                } else {
                    novasRegras.push_back(regra); // Manter regras de tamanho 1 inalteradas
                }
            }

            // Substituir as regras antigas pelas novas
            producao.second = novasRegras;
        }
    }

    //Segunda parte do processo de converssao a FNC
    void dividirRegrasLongas() {
        int contadorT = 1; // Contador para gerar os novos não-terminais T1, T2, ...
        map<string, string> regraParaVariavel; 

        for (auto& producao : regras) {
            vector<string> novasRegras;

            for (const string& regra : producao.second) {
                vector<string> simbolos;

                // Extrair símbolos corretamente, tratando casos como B' e Tn
                for (size_t i = 0; i < regra.size(); ++i) {
                    if (isupper(regra[i]) && i + 1 < regra.size() && regra[i + 1] == '\'') {
                        simbolos.push_back(regra.substr(i, 2));
                        i++; // Pular o próximo caractere (o apóstrofo)
                    } else if (regra[i] == 'T' && i + 1 < regra.size() && isdigit(regra[i + 1])) {
                        size_t j = i + 1;
                        while (j < regra.size() && isdigit(regra[j])) {
                            j++;
                        }
                        simbolos.push_back(regra.substr(i, j - i));
                        i = j - 1; // Pular os caracteres do número
                    } else {
                        simbolos.push_back(string(1, regra[i]));
                    }
                }

                if (simbolos.size() > 2) {
                    string ultimaVariavel = simbolos.back();

                    for (int i = simbolos.size() - 2; i > 0; --i) {
                        string subRegra = simbolos[i] + ultimaVariavel;

                        // Verifica se já existe uma variável que gera essa sub-regra
                        if (regraParaVariavel.find(subRegra) == regraParaVariavel.end()) {
                            string novoNaoTerminal = "T" + to_string(contadorT++);

                            // Mapear a sub-regra para o novo não-terminal
                            regraParaVariavel[subRegra] = novoNaoTerminal;
                            regras[novoNaoTerminal].push_back(subRegra);
                        }

                        // Reutilizar a variável existente
                        ultimaVariavel = regraParaVariavel[subRegra];
                    }

                    novasRegras.push_back(simbolos[0] + ultimaVariavel);
                } else {
                    novasRegras.push_back(regra);
                }
            }

            // Atualizar as regras da produção com as novas regras divididas
            producao.second = novasRegras;
        }
    }

    //Para fins de organização, chamamos as duas partes do processo de FNC dentro deste metodo
    void converterFNC(){
        substituirTerminais();
        dividirRegrasLongas();
    }

        
};

    
int main(int argc, char* argv[]) {

    const char* nomeArquivo = argv[1];

    Gramatica gramatica(nomeArquivo);

        gramatica.removerRecursividadeInicial();
        gramatica.eliminarRegrasLambda();
        gramatica.criarFechos();
        gramatica.removerRegrasCadeia();
        gramatica.removerSimbolosInuteis();
        gramatica.removerVariaveisInalcancaveis();


    cout << "Gramatica preparada para conversao em FNC:" << endl;
    gramatica.mostrarRegras();


    cout << "Gramatica em FNC:" << endl;
    gramatica.converterFNC();
    gramatica.mostrarRegras();

    return 0;
}
