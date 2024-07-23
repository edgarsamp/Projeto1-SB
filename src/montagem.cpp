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

vector<string> separa_tokens(const string& str) {
    vector<string> tokens;
    istringstream iss(str);
    string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

bool define_rotulo(const string token){
    return token.back() == ':';
}

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

// Função que retorna o índice de um rótulo no vetor de tuplas, ou -1 se não existir
int get_rotulo_id(const string nome_rotulo){
    for (unsigned int i = 0; i < tab_simbolos.size(); i++)
        if (tab_simbolos[i].nome == nome_rotulo)
            return i;
        
    return -1;
}

// Função que retorna o índice de um rótulo no vetor de tuplas, ou -1 se não existir
int get_public_id(const string nome_rotulo){
    for (unsigned int i = 0; i < tab_definicoes.size(); i++)
        if (get<0>(tab_definicoes[i]) == nome_rotulo)
            return i;
        
    return -1;
}

// Função que retorna a tupla correspondente ao nome do rótulo ou uma tupla com valores padrão se não for encontrado
tuple<Simbolo> get_rotulo_por_nome(const string nome){
    for (unsigned int i = 0; i < tab_simbolos.size(); i++)
        if (tab_simbolos[i].nome == nome)
            return tab_simbolos[i];
    
     // Retorna uma tupla com valores padrão se o rótulo não for encontrado
    return Simbolo{"", -1, false, vector<unsigned int>{}, false};
}

bool eh_intrucao(const string token){
    vector<string>instrucoes = {"ADD", "SUB", "MUL", "DIV", "JMP", "JPMN", "JMPP", "JMPZ", "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP", "CONST", "SPACE", "BEGIN", "EXTERN", "PUBLIC", "END"};

    for (auto & inst: instrucoes)
        if(token == inst)
            return true;

    return false;
}

bool eh_numero(const string str) {
    // Regex para verificar números inteiros positivos e negativos
    regex int_regex("^-?\\d+$");
    // Regex para verificar números hexadecimais no formato 0x ou 0X
    regex hex_regex("^0[xX][0-9a-fA-F]+$");

    // Verifica se a string corresponde a qualquer um dos padrões
    if (regex_match(str, int_regex) || regex_match(str, hex_regex)) {
        return true;
    }
    return false;
}

bool eh_copy_argumento(const string token) {
    // Encontra a posição da vírgula
    size_t pos = token.find(',');
    if (pos == string::npos) {
        return false; // Se não houver vírgula, o formato é inválido
    }

    return true;
}

bool eh_simbolo(string token){
    if(!eh_intrucao(token) && !eh_numero(token) && !eh_copy_argumento(token))
            return true;
    return false;
}

bool usa_simbolo(vector<string> linha){
    for (unsigned int i = 0; i < linha.size(); i++){
        string token = linha[i];
        // Se não é instrução, nem numero e nem argumento do copy tem que ser um simbolo
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
    bool tem_begin = false;
    bool tem_erro = false;
    string nome_modulo = "";

    for (unsigned int i = 0; i < programa.size(); i++){
        string linha = programa[i];
        vector<string> linha_quebrada = separa_tokens(linha);
        vector<tuple<string, int>> pendencias_temp = {};
        vector<Simbolo> simblos_temp = {};

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
                            cout << "ERRO - Rotudo " << nome_rotulo << " redefinido na linha: " << i + 1 << endl; 
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
                                    cout << "deu ruim na lista de pendencias aqui ein: " << tab_simbolos[id].nome << " - " << pend << endl;
                                    tem_erro = true;
                                }
                                tab_simbolos[id].pendencias.pop_back();
                            }
                            int id_public = get_public_id(nome_rotulo);
                            if(id_public != -1){
                                get<1>(tab_definicoes[id_public]) = count;
                            }
                        }
                    }else{ // Caso seja um novo rotulo adiciona na tabela de simbolos
                        tab_simbolos.push_back(Simbolo{nome_rotulo, count, true, vector<unsigned int>{}, false});
                        int id_public = get_public_id(nome_rotulo);
                        if(id_public != -1){
                            get<1>(tab_definicoes[id_public]) = count;
                        }
                    }


                }else{
                    cout << "ERRO - Nome do rotulo '" << nome_rotulo << "' eh invalido" << endl;
                    tem_erro = true;
                }
            }else{// se declara mais de um rotulo por linha
                cout << "ERRO - Declarando mais de um rotulo na Linha: " << i + 1 << endl; //TODO perguntar pro lucca se é isso mesmo o ERRO (Rotulo dobrado na mesma linha)
                tem_erro = true;
                break;
            }

            // remove o rotulo da linha
            linha_quebrada.erase(linha_quebrada.begin());
        }
        
        // Le a linha
        // Se a linha usa algum simbolo

        // arruma os argumentos do copy
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
            if(linha_quebrada.size() != 3){ // erro do tipo COPY A1,
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
                continue;
            }
        }

        if(usa_simbolo(linha_quebrada)){
            bool eh_public = false;
            for (unsigned int i_linha = 0; i_linha < linha_quebrada.size(); i_linha++) {
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
                            if(eh_public && get_public_id(token) == -1){
                                tab_definicoes.push_back(make_tuple(token, -1));
                                eh_public = false;;
                                simbolo_temp = Simbolo{token, -1, false, vector<unsigned int>{}, false};
                            }else{
                                linha_quebrada[i_linha] = "-1";
                                simbolo_temp = Simbolo{token, -1, false, vector<unsigned int>{(count+i_linha)}, false};
                            }
                            simblos_temp.push_back(simbolo_temp);

                            
                        }else{ // Se achou
                            if(tab_simbolos[id].eh_externo){// se eh externo
                                tab_uso.push_back(make_tuple(token, count+i_linha));
                                linha_quebrada[i_linha] = "0";
                            }else if(tab_simbolos[id].foi_definido){ // se esta definido 
                                linha_quebrada[i_linha] = to_string(tab_simbolos[id].endereco);

                            }else {
                            // }else if(get_public_id(token) == -1){ // se e nao é public
                                pendencias_temp.push_back(make_tuple(token, count+i_linha));
                                // tab_simbolos[id].pendencias.push_back(count+i_linha); // adiciona na lista de pendencias
                                linha_quebrada[i_linha] = "-1";
                            }
                        }
                    }else{
                        cout << "ERRO - Nome do rotulo '" << token << "' eh invalido" << endl;
                        tem_erro = true;
                    }
                }
            }
        }


        // cout << "end." << i << " " ;
        // for (unsigned int a = 0; a < linha_quebrada.size(); a++){
        //     cout << linha_quebrada[a] << " ";
        // }
        // cout << "- ";        

        // Montagem da linha
        string inst = linha_quebrada[0];

        int opCode = -1;
        int arg1 = -10000;
        int arg2 = -10000;
        
        if(inst =="ADD"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 1;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="SUB"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 2;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="MUL"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 3;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
            
        }else if(inst =="DIV"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 4;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="JMP"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 5;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="JPMN"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 6;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="JMPP"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 7;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="JMPZ"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 8;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="COPY"){
            if(linha_quebrada.size() != 3){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else if(!eh_numero(linha_quebrada[1]) || !eh_numero(linha_quebrada[2])){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
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
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 10;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="STORE"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 11;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="INPUT"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 12;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="OUTPUT"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 13;
                arg1 = converte_numero(linha_quebrada[1]);
                count += 2;
                tab_real += "01";
            }
        }else if(inst =="STOP"){
            if(linha_quebrada.size() != 1){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                opCode = 14;
                count += 1;
                tab_real += "0";
            }
        }else if(inst =="SPACE"){
            if(linha_quebrada.size() != 1){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                arg1 = 0;
                count += 1;
                tab_real += "0";
            }
        }else if(inst =="CONST"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }else{
                arg1 = converte_numero(linha_quebrada[1]);
                count += 1;
                tab_real += "0";
            }
        }else if(inst =="BEGIN"){
            if(linha_quebrada.size() != 1){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }
        }else if(inst =="EXTERN"){
            if(linha_quebrada.size() != 1){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }
        }else if(inst =="PUBLIC"){
            if(linha_quebrada.size() != 2){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }
        }else if(inst =="END"){
            if(linha_quebrada.size() != 1){
                cout << "ERRO - Numero de operandos errado para uma instrucao. Linha: " << i + 1  << " - " << linha << endl;
                tem_erro = true;
            }
            break;
        }else {
                cout << "ERRO - Instrucao ou diretiva invalida na linha "<< i+1 << ": " << linha << endl;
                tem_erro = true;
        }

        // Se é uma operação adiciona o upcode
        if(opCode != -1){
            programa_temp.push_back(opCode);
            // cout << " - Opcode: " << opCode;

            // se for uma linha valida adiciona os simbolos usados nela
            for(const auto simb : simblos_temp){
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
            // cout << " - arg1: " << arg1;
        }

        if(arg2 != -10000){
            programa_temp.push_back(arg2);
            // cout << " - arg2: " << arg2;
        }
        // cout << endl;
        // print_tabela_simbolos();
        // print_tabela_definicoes();
    }

    for (const auto& simbolo : tab_simbolos) {
        if(!simbolo.foi_definido){
            cout << "ERRO - Rotudo '" << simbolo.nome << "' ausente" << endl; 
        }
    }


    // if(tem_begin){
    //     cout << nome_modulo << endl;
    //     cout << "REAL\n" << tab_real << endl;
    //     print_tabela_definicoes();
    //     print_tabela_uso();
    // }
    

    vector<string> programa_final = {};

    for (const auto& a : programa_temp){
        programa_final.push_back(to_string(a));
    }

    // print_tabela_simbolos();


    return Programa{tem_erro, tem_begin, nome_modulo, programa_final, tab_definicoes, tab_uso, tab_real};
}

/*
tuple<
bool, // tem_begin
string,     // nome_modulo
vector<string>, // programa_montado
vector <tuple<string, int>>, // tabela de definicoes
vector <tuple<string, int>>, // tabela de uso
string // tabela real
>
*/
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