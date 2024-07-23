#ifndef PREPROC_HPP
#define PREPROC_HPP

#include <bits/stdc++.h>
using namespace std;

string replace_all(const string& str, string a, string b);
string para_maiusculo(const string& str);
string remove_espacos(string str);
string remove_comentarios(const string& str);
tuple<string, string> filtra_equ(const string str);
vector<string> pre_processamento(vector<string> arquivo);
vector<string> le_arquivo(const string& caminho_arquivo);
void escrever_arquivo_simples(const string& caminho_arquivo, const vector<string>& conteudo, string sep);

#endif // PREPROC_HPP