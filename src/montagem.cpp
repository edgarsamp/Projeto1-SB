#include <bits/stdc++.h>
#include "../include/montagem.hpp"

using namespace std;

struct Simbolo {
    string nome;
    int endereco;
    bool foi_definido;
    vector<unsigned int> pendencias;
    bool eh_externo;
};

vector <Simbolo> tab_simbolos;
vector <tuple<string, int>> tab_definicoes;
vector <tuple<string, int>> tab_uso;
string tab_real = "";

void print_tabela_simbolos(){
    cout << "----------- Tabela de simbolos -----------" << endl;
    for (const auto& simbolo : tab_simbolos) {
        cout << "Nome: " << simbolo.nome << ", endereco: " << simbolo.endereco 
             << ", Foi definida: " << (simbolo.foi_definido ? "Sim" : "Nao") 
             << ", Pendencias: ( ";
        for (int valor : simbolo.pendencias) {
            cout << valor << " ";
        }
        cout << "), eh externa: " << (simbolo.eh_externo ? "Sim" : "Nao")  << endl;
    }
    cout << "----------- Tabela de simbolos -----------" << endl;
}

void print_tabela_definicoes(){
    cout << "----------- Tabela de definicoes -----------" << endl;
    for (const auto& simbolo : tab_definicoes) 
        cout << "Nome: " << get<0>(simbolo) << ", endereco: " << get<1>(simbolo) << endl;
    cout << "----------- Tabela de definicoes -----------" << endl;
}

void print_tabela_uso(){
    cout << "----------- Tabela de uso -----------" << endl;
    for (const auto& simbolo : tab_uso) 
        cout << "Nome: " << get<0>(simbolo) << ", endereco: " << get<1>(simbolo) << endl;
    cout << "----------- Tabela de uso -----------" << endl;
}

// Recebe a linha do arquivo pre-processado e retorna um vetor com os tokens separados
vector<string> separa_tokens(const string& str) {
    vector<string> tokens;
    istringstream iss(str);
    string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

// Retorna se o token eh uma definicao de rotulo
bool define_rotulo(const string token){
    return token.back() == ':';
}

// Verifica se o nome do rotulo eh um nome valido
bool check_nome_rotulo(const string rotulo) {
    // Verifica se a string está vazia
    if (rotulo.empty()) 
        return false;

    // Verifica se o primeiro caractere é um número
    if (isdigit(rotulo[0])) 
        return false;

    // Verifica se todos os caracteres são alfanuméricos ou '_'
    for (char c : rotulo) 
        if (!isalnum(c) && c != '_') 
            return false;

    return true;
}

// Função que retorna o índice de um simbolo na tabela de simbolos, ou -1 se não existir
int get_rotulo_id(const string nome_rotulo){
    for (unsigned int i = 0; i < tab_simbolos.size(); i++)
        if (tab_simbolos[i].nome == nome_rotulo)
            return i;
        
    return -1;
}

// Função que retorna o índice de simbolo na tabela de definicoes, ou -1 se não existir
int get_public_id(const string nome_rotulo){
    for (unsigned int i = 0; i < tab_definicoes.size(); i++)
        if (get<0>(tab_definicoes[i]) == nome_rotulo)
            return i;
        
    return -1;
}

// Função que retorna a um simbolo correspondente ao nome recebido ou um simbolo com valores padrão se não for encontrado
tuple<Simbolo> get_rotulo_por_nome(const string nome){
    for (unsigned int i = 0; i < tab_simbolos.size(); i++)
        if (tab_simbolos[i].nome == nome)
            return tab_simbolos[i];
    
     // Retorna uma tupla com valores padrão se o rótulo não for encontrado
    return Simbolo{"", -1, false, vector<unsigned int>{}, false};
}

// Verifica se o token eh uma instrucao ou diretiva
bool eh_intrucao(const string token){
    vector<string>instrucoes = {"ADD", "SUB", "MUL", "DIV", "JMP", "JPMN", "JMPP", "JMPZ", "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP", "CONST", "SPACE", "BEGIN", "EXTERN", "PUBLIC", "END"};

    for (auto & inst: instrucoes)
        if(token == inst)
            return true;

    return false;
}

// Verifica se o token eh um numero 
bool eh_numero(const string token) {
    // Regex para verificar numeros inteiros positivos e negativos
    regex int_regex("^-?\\d+$");
    // Regex para verificar numeros hexadecimais no formato 0x ou 0X
    regex hex_regex("^0[xX][0-9a-fA-F]+$");

    // Verifica se o token corresponde a qualquer um dos padroes
    if (regex_match(token, int_regex) || regex_match(token, hex_regex)) {
        return true;
    }
    return false;
}

// Verifica se o token esta no formato de argumento da instrucao copy (A,B)
bool eh_copy_argumento(const string token) {
    // Encontra a posição da vírgula
    size_t pos = token.find(',');
    if (pos == string::npos) {
        return false; // Se não houver vírgula, o formato é inválido
    }
    return true;
}

// Verifica se o token eh algum simbolo
bool eh_simbolo(string token){
    if(!eh_intrucao(token) && !eh_numero(token) && !eh_copy_argumento(token))
            return true;
    return false;
}

// Verifica se a linha usa algum simbolo
bool usa_simbolo(vector<string> linha){
    for (unsigned int i = 0; i < linha.size(); i++){
        string token = linha[i];
        if(eh_simbolo(token))
            return true;
    }
    return false;
}

int converte_numero(string num){
    // Verifica se a string representa um número hexadecimal
    if (num.substr(0, 2) == "0x" || num.substr(0, 2) == "0X") {
        return stoi(num, nullptr, 16); // Converte de hexadecimal para decimal
    }

    // Caso contrário, converte de string para inteiro decimal
    return stoi(num); // Converte de decimal
}

// Ajusta o argumento do copy para ser dois simbolos
vector<string> separa_copy_argumento(const string token) {
    // Encontra a posição da vírgula
    size_t pos = token.find(',');

    // Separa os dois rótulos
    string arg1 = token.substr(0, pos);
    string arg2 = token.substr(pos + 1);

    return vector<string>{arg1, arg2};
}

Programa monta(vector<string> programa){
    vector <int> programa_temp;
    int count = 0;
    bool tem_begin = false; //Verifica se eh um modulo
    bool tem_erro = false; // Tag que so vai permitir montar a linha caso nao tenha error, se encontrar error o usuario eh notificado e a linha eh ignorada
    string nome_modulo = "";

    for (unsigned int i = 0; i < programa.size(); i++){
        string linha = programa[i];
        vector<string> linha_quebrada = separa_tokens(linha);
        vector<tuple<string, int>> pendencias_temp = {};
        vector<Simbolo> simbolos_temp = {};

        // Checa se é definicao de rotulo
        if(define_rotulo(linha_quebrada[0])){
            bool unico_rotulo_linha = true;
            
            //Remove o : do nome
            linha_quebrada[0].pop_back(); 

            // Checa se tem mais de uma declaraçao de rotulo na mesma linha
            for (auto & token: linha_quebrada){
                if(define_rotulo(token)){
                    unico_rotulo_linha = false;
                    break;
                }
            }

            if(unico_rotulo_linha){
                string nome_rotulo =  linha_quebrada[0];

                // se é um nome valido
                if(check_nome_rotulo(nome_rotulo)){
                    if(linha_quebrada.size() > 1){
                        if(linha_quebrada[1] == "BEGIN"){
                            tem_begin = true;
                            nome_modulo = nome_rotulo;
                        }else if (linha_quebrada[1] == "EXTERN"){// Caso seja externo adiciona na tabela de simbolos com a tag extern true
                            tab_simbolos.push_back(Simbolo{nome_rotulo, count, false, vector<unsigned int>{}, true});
                        }
                        
                    } 

                    int id = get_rotulo_id(nome_rotulo);
                    if(id != -1){ // Se o rotulo existe
                        if(tab_simbolos[id].foi_definido){// Se ja estava definido
                            cout << "ERRO SEMANTICO - Rotudo " << nome_rotulo << " redefinido na linha: " << i + 1 << endl; 
                            tem_erro = true;
                        }else{ // se nao estava definido
                            tab_simbolos[id].endereco = count;   // Ajusta o endereço
                            tab_simbolos[id].foi_definido = true;    // Seta como definido

                            // Resolve as pendencias
                            for (unsigned int i = tab_simbolos[id].pendencias.size(); i-- > 0;){
                                unsigned int pend = tab_simbolos[id].pendencias[i];
                                if(pend < programa_temp.size()){
                                    programa_temp[pend] = tab_simbolos[id].endereco;
                                }else{
                                    cout << "deu ruim na lista de pendencias: " << tab_simbolos[id].nome << " - " << pend << endl;
                                    tem_erro = true;
                                }
                                tab_simbolos[id].pendencias.pop_back();
                            }
                            int id_public = get_public_id(nome_rotulo);
                            if(id_public != -1){ // se for um rotulo publico, atualiza tambem a tabela de definicoes
                                get<1>(tab_definicoes[id_public]) = count;
                            }
                        }
                    }else{ // Caso seja um novo rotulo, adiciona na tabela de simbolos
                        tab_simbolos.push_back(Simbolo{nome_rotulo, count, true, vector<unsigned int>{}, false});
                        int id_public = get_public_id(nome_rotulo);
                        if(id_public != -1){ // se for um rotulo publico, adiciona tambem a tabela de definicoes
                            get<1>(tab_definicoes[id_public]) = count;
                        }
                    }

                }else{ // se o nome nao eh valido
                    cout << "ERRO LEXICO - Nome do rotulo '" << nome_rotulo << "' eh invalido" << endl;
                    tem_erro = true;
                }
            }else{// se declara mais de um rotulo por linha
                cout << "ERRO SEMANTICO - Declarando mais de um rotulo na Linha: " << i + 1 << endl; //TODO perguntar pro lucca se é isso mesmo o ERRO (Rotulo dobrado na mesma linha)
                tem_erro = true;
                break;
            }

            // remove o rotulo da linha
            linha_quebrada.erase(linha_quebrada.begin());
        }
        
        // Le a linha
        // Se a linha usa algum simbolo

        // caso seja copy, arruma os argumentos
        if(linha_quebrada[0] == "COPY" && linha_quebrada.size() == 2){
            vector<string> linha_temp = {"COPY"};
            string token = linha_quebrada[1];

            if(eh_copy_argumento(token)){
                vector<string> args_temp = separa_copy_argumento(token);
                for (const auto arg_temp : args_temp){
                    if(!arg_temp.empty())
                        linha_temp.push_back(arg_temp);
                }
            }else{
                linha_temp.push_back(token);
            }
            linha_quebrada = linha_temp;
        }

        if(usa_simbolo(linha_quebrada)){
            bool eh_public = false;
            for (unsigned int i_linha = 0; i_linha < linha_quebrada.size(); i_linha++) { // para cada token da linha
                string token = linha_quebrada[i_linha];
                if(token == "PUBLIC"){
                    eh_public = true;
                    continue;
                }
                if(eh_simbolo(token)){ // se o token é um simbolo
                    if(check_nome_rotulo(token)){ // se é um nome valido
                        int id = get_rotulo_id(token); // procura na tabela de simbolos

                        if(id == -1){ // Se não achou, adiciona como nao definido
                            Simbolo simbolo_temp;
                            if(eh_public && get_public_id(token) == -1){ // se a linha esta adicionando um rotulo publico novo
                                tab_definicoes.push_back(make_tuple(token, -1));
                                eh_public = false;;
                                simbolo_temp = Simbolo{token, -1, false, vector<unsigned int>{}, false};
                            }else{ // se eh um rotulo nao publico
                                linha_quebrada[i_linha] = "-1"; // coloca -1 no lugar para para demarcar a ausencia de endereco
                                simbolo_temp = Simbolo{token, -1, false, vector<unsigned int>{(count+i_linha)}, false};
                            }
                            // Adiciona na tabela de temporaria de simbolos, no final caso a linha nao tenha erro ele sera adicionado na tabela de simbolos
                            simbolos_temp.push_back(simbolo_temp);

                            
                        }else{ // Se achou
                            if(tab_simbolos[id].eh_externo){// se eh externo
                                tab_uso.push_back(make_tuple(token, count+i_linha)); // adiciona na tabela de uso
                                linha_quebrada[i_linha] = "0"; // como nao temos vetores, coloca 0 como endereco
                            }else if(tab_simbolos[id].foi_definido){ // se esta definido, coloca o endereco
                                linha_quebrada[i_linha] = to_string(tab_simbolos[id].endereco);

                            }else { // se achou mas nao esta definido, adiciona como pendencia
                                pendencias_temp.push_back(make_tuple(token, count+i_linha));
                                linha_quebrada[i_linha] = "-1";
                            }
                        }
                    }else{ // Se o nome nao eh valido
                        cout << "ERRO LEXICO - Nome do rotulo '" << token << "' eh invalido" << endl;
                        tem_erro = true;
                    }
                }
            }
        }

        // Montagem da linha
        string inst = linha_quebrada[0];

        int opCode = -1;
        int arg1 = -10000;
        int arg2 = -10000;
        
        if(inst =="ADD"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 1;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="SUB"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 2;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="MUL"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 3;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="DIV"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 4;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="JMP"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 5;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="JPMN"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 6;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="JMPP"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 7;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="JMPZ"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 8;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="COPY"){
            if(linha_quebrada.size() != 3){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else if(!eh_numero(linha_quebrada[1]) || !eh_numero(linha_quebrada[2])){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else {
                opCode = 9;
                arg1 = converte_numero(linha_quebrada[1]);
                arg2 = converte_numero(linha_quebrada[2]);
                count += 3;
                tab_real += "011";
            }
        }else if(inst =="LOAD"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 10;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="STORE"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 11;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="INPUT"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 12;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="OUTPUT"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 13;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="STOP"){
            if(linha_quebrada.size() != 1){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 14;
                count += 1;
                tab_real += "0";
            }
        }else if(inst =="SPACE"){
            if(linha_quebrada.size() != 1){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                arg1 = 0;
                count += 1;
                tab_real += "0";
            }
        }else if(inst =="CONST"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                arg1 = converte_numero(linha_quebrada[1]);
                count += 1;
                tab_real += "0";
            }
        }else if(inst =="BEGIN"){
            if(linha_quebrada.size() != 1){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }
        }else if(inst =="EXTERN"){
            if(linha_quebrada.size() != 1){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }
        }else if(inst =="PUBLIC"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }
        }else if(inst =="END"){
            if(linha_quebrada.size() != 1){
                cout << "ERRO SINTATICO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }
            break;
        }else {
                cout << "ERRO SEMANTICO - Instrucao ou diretiva invalida na linha "<< i+1 << ": " << linha << endl;
                tem_erro = true;
        }

        // Se é uma linha valida adiciona o upcode
        if(opCode != -1){
            programa_temp.push_back(opCode);

            // se for uma linha valida adiciona os simbolos usados nela
            for(const auto simb : simbolos_temp){
                tab_simbolos.push_back(simb);
            }

            // se for uma linha valida adiciona as pendencias
            for (const auto pend : pendencias_temp){
                int id = get_rotulo_id(get<0>(pend));
                if(id != -1)
                    tab_simbolos[id].pendencias.push_back(get<1>(pend));
            }
        }

        // Adiciona os argumentos da operacao
        if(arg1 != -10000){
            programa_temp.push_back(arg1);
        }

        if(arg2 != -10000){
            programa_temp.push_back(arg2);
        }
    }

    // Caso no final exista algum símbolo que foi usado, mas não foi definido
    for (const auto& simbolo : tab_simbolos) {
        if(!simbolo.foi_definido){
            cout << "ERRO SEMANTICO - Rotudo '" << simbolo.nome << "' ausente" << endl; 
        }
    }

    vector<string> programa_final = {};

    for (const auto& a : programa_temp){
        programa_final.push_back(to_string(a));
    }

    return Programa{tem_erro, tem_begin, nome_modulo, programa_final, tab_definicoes, tab_uso, tab_real};
}

void escrever_arquivo_modulos(const Programa programa) {
    string caminho_arquivo = "./saidas/" + programa.nome + ".obj";
    ofstream arquivo(caminho_arquivo);
    if (!arquivo.is_open()) {
        arquivo.open(caminho_arquivo);
        cerr << "Erro ao abrir o arquivo para escrita: " << caminho_arquivo << endl;
        return;
    }

    arquivo << "-USO-" << endl;
    for (const auto& linha : programa.tab_uso) {
        arquivo << get<0>(linha) << " " << get<1>(linha) << endl;
    }

    arquivo << "\n-DEF-" << endl;
    for (const auto& linha : programa.tab_definicoes) {
        arquivo << get<0>(linha) << " " << get<1>(linha) << endl;
    }

    arquivo << "\n-REAL-" << endl;
    arquivo << programa.tab_real << endl << endl;

    for (const auto& linha : programa.programa) {
        arquivo << linha << " ";
    }

    arquivo.close();
}