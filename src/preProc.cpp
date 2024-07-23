#include <bits/stdc++.h>
#include "../include/preProc.hpp"
using namespace std;

string para_maiusculo(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

string remove_espacos(string str){
    regex r("\\s+");

    str = regex_replace(str, r, " ");

    if (!str.empty() && str.back() == ' ') str.pop_back();
    if (!str.empty() && str.front() == ' ') str.erase(str.begin());

    return str;
}

string remove_comentarios(const string& str) {
    size_t pos = str.find(';');
    if (pos != string::npos) {
        return str.substr(0, pos);
    }
    return str;
}

tuple<string, string, int> filtra_dir(const string str, int linha_index, string dir){
    size_t pos = str.find(dir);

    string first_part = str.substr(0, pos);
    string second_part = str.substr(pos + 6); // 6 é o comprimento de ": EQU "
    return make_tuple(first_part, second_part, linha_index);
}

string substituir_ocorrencias(string linha, const tuple<string, string, int> eq) {
    const string& procurar = get<0>(eq);
    const string& substituir = get<1>(eq);

    // Cria a regex para a string a ser procurada, usando delimitadores de palavra (\b)
    regex re("\\b" + procurar + "\\b");

    // Substitui todas as ocorrências exatas
    return regex_replace(linha, re, substituir);
}

vector<string> pre_processamento(vector<string> arquivo) {
    vector<string> arquivoTemp;
    vector<string> arquivoPronto;

    // Remove comentarios, espacos em branco desnecessarios e linhas em branco
    for (unsigned int i = 0; i < arquivo.size(); i++){
        string linha = para_maiusculo(arquivo[i]);

        linha = remove_comentarios(linha);
        linha = remove_espacos(linha);
        if(linha.empty())
            continue;

        arquivoTemp.push_back(linha);
    }

    // Ve se tem Rotulo seguido de quebra de linha
    bool rotuloQuebrado = false;
    for (unsigned int i = 0; i < arquivoTemp.size(); i++){
        string linha = arquivoTemp[i];  

        if(rotuloQuebrado){
            linha = arquivoTemp[i-1] + ' ' + linha;
            rotuloQuebrado = false;
        }

        if(linha.back() == ':' && i+1 < arquivoTemp.size()){ //TODO pode ser que seja ':\n'
            rotuloQuebrado = true;
            continue;
        }
        arquivoPronto.push_back(linha);
    }

    // Armazena o nome, conteudo e linha do equ
    vector <tuple<string, string, int>> dirEQU;

    // Identifica os EQu e salva em dirEQU
    for (unsigned int i = 0; i < arquivoPronto.size(); i++){
        string linha = arquivoPronto[i];
        // Se tem EQU
        if(linha.find(": EQU ") != string::npos)
            dirEQU.push_back(filtra_dir(linha, i, ": EQU "));
    }

    // Trata os EQU
    for (unsigned int i = 0; i < arquivoPronto.size(); i++){
        for (const auto& dir : dirEQU){
            string novaLinha = substituir_ocorrencias(arquivoPronto[i], dir);
            arquivoPronto[i] = novaLinha;
        }
    }

    // Remove as definições dos EQU
    for (unsigned int i = dirEQU.size(); i-- > 0;)
        arquivoPronto.erase(arquivoPronto.begin() + get<2>(dirEQU[i]));

    // Trata os IFs
    vector <tuple<string, string, int>> dirIF;

    for (unsigned int i = arquivoPronto.size(); i-- > 0;){
        string linha = arquivoPronto[i];
        // Se tem IF 1
        if(linha == "IF 1"){
            arquivoPronto.erase(arquivoPronto.begin() + i);
        }else if (linha == "IF 0"){
            arquivoPronto.erase(arquivoPronto.begin() + i+1);
            arquivoPronto.erase(arquivoPronto.begin() + i);
        }
        
    }

    return arquivoPronto;
}

vector<string> le_arquivo(const string& caminho_arquivo) {
    ifstream arquivo(caminho_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << caminho_arquivo << endl;
        return {};
    }

    vector<string> conteudo;
    string linha;
    while (getline(arquivo, linha)) {
        conteudo.push_back(linha);
    }

    arquivo.close();
    return conteudo;
}

void escrever_arquivo_simples(const string& caminho_arquivo, const vector<string>& conteudo, string sep) {
    ofstream arquivo(caminho_arquivo);
    if (!arquivo.is_open()) {
        arquivo.open(caminho_arquivo);
        cerr << "Erro ao abrir o arquivo para escrita: " << caminho_arquivo << endl;
        return;
    }
    for (const auto& linha : conteudo) {
        arquivo << linha << sep;
    }

    arquivo.close();
}