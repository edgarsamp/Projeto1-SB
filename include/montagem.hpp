#ifndef MONTAGEM_HPP
#define MONTAGEM_HPP

#include <bits/stdc++.h>
using namespace std;

// TODO arrumar tudo aqui

struct Programa {
    bool tem_erro;
    bool tem_begin;
    string nome;
    vector<string> programa;
    vector <tuple<string, int>> tab_definicoes;
    vector <tuple<string, int>> tab_uso;
    string tab_real;
};

Programa monta(vector<string> programa);
void escrever_arquivo_modulos(const Programa programa);

#endif 