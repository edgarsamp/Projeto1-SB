#include <bits/stdc++.h>
#include "../include/ligacao.hpp"

using namespace std;

vector<tuple<string, int>> tab_global_def = {};

void print_tabela_definicoes(vector<tuple<string, int>> tab_definicoes){
    cout << "----------- Tabela global de definicoes -----------" << endl;
    for (const auto& simbolo : tab_definicoes) 
        cout << "Nome: " << get<0>(simbolo) << ", endereco: " << get<1>(simbolo) << endl;
    cout << "----------- Tabela global de definicoes -----------" << endl;
}

void imprime_arquivo(Modulo data){
    cout << "-USO-" << data.uso.size() << endl;
        for (const auto& p : data.uso) {
            cout << get<0>(p) << " " << get<1>(p) << endl;
        }

        cout << "-DEF-" << endl;
        for (const auto& p : data.def) {
            cout << get<0>(p) << " " << get<1>(p) << endl;
        }

        cout << "REAL: " << endl;
        cout << data.real_bits << endl;

        cout << "Values:" << endl;
        for (const auto& val : data.values) {
            cout << val << " ";
        }
        cout << endl;
}

// Função que le o arquivo e extrai as informações
Modulo le_arquivo_modulo(const string path) {

    cout << path << endl;
    ifstream file(path);
    if (!file) {
        throw runtime_error("\nNao foi possivel abrir o arquivo: " + path);
    }

    Modulo data;
    string line;
    string current_section;

    while (getline(file, line)) {
        if (line.empty()){
            continue;
        }else if (line == "-USO-") {
            current_section = "USO";
        } else if (line == "-DEF-") {
            current_section = "DEF";
        } else if (line == "-REAL-") {
            current_section = "REAL";
        } else {
            if (current_section == "USO" || current_section == "DEF") {
                stringstream ss(line);
                string str;
                int num;
                ss >> str >> num;
                if (current_section == "USO") {
                    data.uso.emplace_back(str, num);
                } else {
                    data.def.emplace_back(str, num);
                }
            } else if (current_section == "REAL") {
                if (data.real_bits.empty()) {
                    data.real_bits = line;
                } else {
                    stringstream ss(line);
                    int num;
                    while (ss >> num) {
                        data.values.push_back(num);
                    }
                }
            }
        }
    }

    file.close();
    return data;
}

void escrever_arquivo_montado(const string caminho_arquivo, const vector<int> programa) {
    ofstream arquivo(caminho_arquivo);
    if (!arquivo.is_open()) {
        arquivo.open(caminho_arquivo);
        cerr << "Erro ao abrir o arquivo para escrita: " << caminho_arquivo << endl;
        return;
    }
    for (const auto& inst : programa) {
        arquivo << inst << " ";
    }
    arquivo.close();
}

int get_valor(string simbolo){
    for(const auto def : tab_global_def){
        if(get<0>(def) == simbolo)
            return get<1>(def);
    }
    return -1;
}


vector<int> liga(Modulo mod1, Modulo mod2){
    vector<int> programa = {};
    int fator_correcao_mod1 = 0;
    int fator_correcao_mod2 = mod1.values.size();

    // Alinha os codigos objetos
    for(const auto inst : mod1.values){
        programa.push_back(inst);
    }

    for(const auto inst : mod2.values){
        programa.push_back(inst);
    }

    // Constri a tabela global de definicoes
    for(const auto def : mod1.def){
        tab_global_def.push_back(make_tuple(get<0>(def), get<1>(def) + fator_correcao_mod1));
    }
    for(const auto def : mod2.def){
        tab_global_def.push_back(make_tuple(get<0>(def), get<1>(def) + fator_correcao_mod2));
    }


    //Corrigindo os enderecos pendentes nas tabelas de uso do primeiro modulo
    for(const auto uso : mod1.uso){ // para cada valor
        //Pega o endereco na tabela de uso
        int valor = get_valor(get<0>(uso));

        if(valor != -1){ // Se encontrou

            // Resolve a referencia 
            programa[get<1>(uso)+fator_correcao_mod1] += valor;
            mod1.real_bits[get<1>(uso)] = '0'; // Seta como 0 para nao alterar mais o valor
        }

    }

    //Corrigindo os enderecos pendentes nas tabelas de uso do segundo modulo
    for(const auto uso : mod2.uso){// para cada valor
        //Pega o endereco na tabela de uso
        int valor = get_valor(get<0>(uso));

        if(valor != -1){ // Se encontr
            programa[get<1>(uso)+fator_correcao_mod2] += valor; // Resolve a referencia
            mod2.real_bits[get<1>(uso)] = '0';// Seta como 0 para nao alterar mais o valor
        }
    }


    // Aplica o fator de correcao no enderecos relativos
    for (unsigned int i = 0; i < mod1.real_bits.size(); i++){
        char vlr = mod1.real_bits[i];
        if(vlr == '1'){
            programa[i+fator_correcao_mod1] += fator_correcao_mod1;
        }
    }
    for (unsigned int i = 0; i < mod2.real_bits.size(); i++){
        char vlr = mod2.real_bits[i];
        if(vlr == '1'){
            programa[i+fator_correcao_mod2] += fator_correcao_mod2;
        }
    }

    return programa;
}