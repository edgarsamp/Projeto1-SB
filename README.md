# Trabalho Prático 1 -Software-Basico

Aluno: Edgar Sampaio de Barros
Matricula: 16/0005213
Professor: Bruno Macchiavello

make

faça o pre-processamento com: 
.\montador.exe -p arquivo .asm

depois faça a montagem utilizando o arquivo gerado pelo pre-processamento:
.\montador.exe -o arquivo.pre

caso seja necessario ligar os arquivos use:
.\ligador.exe arquivo1.obj arquivo2.obj

para simular o programa use:
.\simulador arquivo.e

ou, caso o arquivo nao precise de ligacação

.\simulador arquivo.obj

Features:
[x] Aceitar Maiúsculas e Minúsculas (não ser sensível ao CASO)
[x] A diretiva CONST deve aceitar positivos, negativos e hexa no formato 0x (ex: 0x12). .
[x] O comando COPY deve separar os argumentos por “,” SEM espaço
[x] Desconsiderar todos os espaços, tabulações ou enter desnecessários.
[x] Pode dar rótulo seguido de dois pontos e ENTER. O rótulo é considerado como da linha seguinte
[x] Aceitar comentário em qualquer parte do código iniciado por ; (o comentário deve ser removido no pré-processamento de EQU e IF)
[x] Assumir que os SPACES E CONST sempre estao no final


Erros detectados:
[x] Rotulo ausente
[x] Rotulo dobrado na mesma linha
[x] Rotudo redefinido
[x] Numero de operandos errada p uma instrucao
[x] Erros lexicos de rotulos (rotulos nao podem começar com número e unico caracter especial q podem ter eh “_”. )
[x] Instrução ou diretiva inválida 