#include <bits/stdc++.h>
#include "../include/preProc.hpp"
#include "../include/montagem.hpp"

using namespace std;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cout << "Numero incorreto de argumentos. Use: ./main -o/-p arquivo" << endl;
        return 1;
    }
    vector<string> programa;

    string opcao = argv[1];
    string arquivo = argv[2];

    programa = le_arquivo(arquivo);

    if (opcao == "-o") {
        cout << "Montagem do arquivo: " << arquivo << endl;

        Programa programa_montado = monta(programa);
        string caminho_saida;

        if(programa_montado.tem_begin){ // se tem eh um modulo
            caminho_saida = "./saidas/" + programa_montado.nome + ".obj";
            escrever_arquivo_modulos(programa_montado);

        }else{
            caminho_saida = "./saidas/myfile.obj";
            escrever_arquivo_simples(caminho_saida, programa_montado.programa, " ");
        }

        if (!programa_montado.tem_erro){
            cout << "Montagem concluido. Arquivo de saida: " << caminho_saida << endl;
        }else{
            cout << "O Programa encontrou algum erro durante a montagem. Arquivo de saida: " << caminho_saida << endl;
        }
    

    } else if (opcao == "-p") {
        cout << "Pre-processamento do arquivo: " << arquivo << endl;

        programa = pre_processamento(programa);

        string caminho_saida = "./saidas/myfile.pre";
        escrever_arquivo_simples(caminho_saida, programa, "\n");
        cout << "Pre-processamento concluido. Arquivo de saida: " << caminho_saida << endl;

    } else {
        cout << "Opcao invalida. Use: ./main -o/-p arquivo" << std::endl;
        return 1;
    }

    return 0;
}
