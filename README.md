# Trabalho Prático 1 - Software Básico

**Aluno:** Edgar Sampaio de Barros  
**Matrícula:** 16/0005213  
**Professor:** Bruno Macchiavello

## Descrição
Este trabalho consiste na implementação em C++ de um montador e ligador para o assembly inventado discutido em sala de aula.

## Funcionalidades
- [x] Aceita maiúsculas e minúsculas (não sensível ao caso).
- [x] A diretiva CONST aceita valores positivos, negativos e hexadecimais no formato `0x` (ex: `0x12`).
- [x] O comando COPY separa os argumentos por “,” sem espaço.
- [x] Desconsidera todos os espaços, tabulações ou enters desnecessários.
- [x] Permite rótulos seguidos de dois pontos e ENTER; o rótulo é considerado como da linha seguinte.
- [x] Aceita comentários em qualquer parte do código iniciados por `;` (os comentários são removidos no pré-processamento de EQU e IF).
- [x] Assume que os SPACES e CONST sempre estão no final.

## Erros Detectados
- [x] Rótulo ausente.
- [x] Rótulo duplicado na mesma linha.
- [x] Rótulo redefinido.
- [x] Número de operandos incorreto para uma instrução.
- [x] Erros léxicos de rótulos (rótulos não podem começar com número e o único caractere especial permitido é “_”).
- [x] Instrução ou diretiva inválida.

## Compilação e Execução
Para compilar o projeto, utilize o comando `make`.

### Pré-processamento
Faça o pré-processamento com:
```bash
.\montador.exe -p arquivo.asm
```

### Montagem
Depois, faça a montagem utilizando o arquivo gerado pelo pré-processamento:
```bash
.\montador.exe -p arquivo.asm
```

### Ligação
Caso seja necessário ligar os arquivos, use:

```bash
.\ligador.exe arquivo1.obj arquivo2.obj
```

### Simulação
Para simular o programa, use:

```bash
.\simulador arquivo.e
```
Ou, caso o arquivo não precise de ligação:

```bash
.\simulador arquivo.obj
```
#### Versão do GCC e SO
Este projeto foi desenvolvido utilizando a versão do GCC: `gcc version 6.3.0 (MinGW.org GCC-6.3.0-1)` e testado em dois computadores com Windows 10.

