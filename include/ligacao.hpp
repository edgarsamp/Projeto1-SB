#ifndef LIGACAO_HPP
#define LIGACAO_HPP

#include <bits/stdc++.h>
using namespace std;

// Estrutura para armazenar as informações de cada seção
struct Modulo {
    vector<tuple<string, int>> uso;
    vector<tuple<string, int>> def;
    string real_bits;
    vector<int> values;
};

void escrever_arquivo_montado(const string caminho_arquivo, const vector<int> programa);
Modulo le_arquivo_modulo(const string path);
vector<int> liga(Modulo mod1, Modulo mod2);
void imprime_arquivo(Modulo data);

#endif 