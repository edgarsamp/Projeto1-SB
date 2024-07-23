#define CATCH_CONFIG_MAIN
#include <bits/stdc++.h>
#include "./catch.hpp"
#include "../include/preProc.hpp"
using namespace std;

TEST_CASE("Teste de pre_processamento", "[preProc]") {

    SECTION("Teste de remove_espacos") {
        REQUIRE(remove_espacos("   a   b   c   ") == "a b c");
        REQUIRE(remove_espacos("   x y z   ") == "x y z");
        REQUIRE(remove_espacos("   ") == "");
    }

    SECTION("Linha com ';' no inicio") {
        REQUIRE(remove_comentarios(";a b c d e f") == "");
    }


    SECTION("Linha com ';' no meio") {
        REQUIRE(remove_comentarios("a b c; d e f") == "a b c");
    }

    SECTION("Linha com ';' e espaços extras") {
        REQUIRE(remove_comentarios("x y z ; 1 2 3") == "x y z ");
    }

    SECTION("Linha sem ';'") {
        REQUIRE(remove_comentarios("no semicolon here") == "no semicolon here");
    }

    SECTION("Linha com ';' no final") {
        REQUIRE(remove_comentarios("semicolon at the end;") == "semicolon at the end");
    }
    
    SECTION("Linha com mais de uma ';'") {
        REQUIRE(remove_comentarios(";a b c; d e f") == "");
    }
    
    SECTION("Linha com mais de uma ';'") {
        REQUIRE(remove_comentarios("a; b c d e f;") == "a");
    }
    
    SECTION("Linha com mais de uma ';'") {
        REQUIRE(remove_comentarios("a ;b c; d e f") == "a ");
    }

    SECTION("Teste de realiza_pre_processamento") {
        vector<string> arquivo = {"  a   b \t  c   ", "  x y z   ", "  \ta\t  "};
        vector<string> esperado = {"A B C", "X Y Z", "A"};
        REQUIRE(pre_processamento(arquivo) == esperado);
    }

    SECTION("Teste de realiza_pre_processamento") {
        vector<string> arquivo = {"  a   b ;  c   ", ";  x y z   ", "  a++2; "};
        vector<string> esperado = {"A B", "A++2"};
        REQUIRE(pre_processamento(arquivo) == esperado);
    }

    SECTION("Teste de Rotulo com quebra de linha") {
        vector<string> arquivo = {"Abobrinha:   \t;  c   \t\t  a", ";  x y z   ", "  \ta++2; "};
        vector<string> esperado = {"ABOBRINHA: A++2"};
        REQUIRE(pre_processamento(arquivo) == esperado);
    }

    SECTION("Teste de IF nos comentarios") {
        vector<string> arquivo = {"Abobrinha:   ADD 1\t;  IF   \t\t  1", ";  x y z   ", "  \t DOIS: CONST 2; 2 "};
        vector<string> esperado = {"ABOBRINHA: ADD 1", "DOIS: CONST 2"};
        REQUIRE(pre_processamento(arquivo) == esperado);
    }

    SECTION("Teste de IF") {
        vector<string> arquivo = {"Abobrinha:   ADD 1\t", "  IF   \t\t  1", ";  x y z   ", "  \t DOIS: CONST 2; 2 "};
        vector<string> esperado = {"ABOBRINHA: ADD 1"};
        REQUIRE(pre_processamento(arquivo) == esperado);
    }
}
