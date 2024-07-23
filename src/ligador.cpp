#include <bits/stdc++.h>
#include "../include/ligacao.hpp"
using namespace std;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cout << "Numero incorreto de argumentos. Use: ./ligador arquivo1 arquivo2" << endl;
        return 1;
    }

    
    string path1 = argv[1]; 
    string path2 = argv[2]; 

    try {
        vector<int> programa;
        Modulo mod1 = le_arquivo_modulo(path1);
        Modulo mod2 = le_arquivo_modulo(path2);

        programa = liga(mod1, mod2);

        string caminho_saida = "./saidas/prog1.e";
        cout << "Ligacao concluida. Arquivo de saida: " << caminho_saida << endl;
        escrever_arquivo_montado(caminho_saida, programa);


    } catch (const exception& e) {
        cerr << "Erro: Nao foi possivel ler algum arquivo" << e.what() << endl;
    }

    return 0;
}
