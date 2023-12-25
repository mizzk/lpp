#include "token-list.h"

int token;

/* keyword list */
struct KEY key[KEYWORDSIZE] = {
    {"and", TAND},         {"array", TARRAY},     {"begin", TBEGIN},
    {"boolean", TBOOLEAN}, {"break", TBREAK},     {"call", TCALL},
    {"char", TCHAR},       {"div", TDIV},         {"do", TDO},
    {"else", TELSE},       {"end", TEND},         {"false", TFALSE},
    {"if", TIF},           {"integer", TINTEGER}, {"not", TNOT},
    {"of", TOF},           {"or", TOR},           {"procedure", TPROCEDURE},
    {"program", TPROGRAM}, {"read", TREAD},       {"readln", TREADLN},
    {"return", TRETURN},   {"then", TTHEN},       {"true", TTRUE},
    {"var", TVAR},         {"while", TWHILE},     {"write", TWRITE},
    {"writeln", TWRITELN}};

/* Token counter */
int numtoken[NUMOFTOKEN + 1];

/* string of each token */
char *tokenstr[NUMOFTOKEN + 1] = {
    "",        "NAME",    "program", "var",     "array",     "of",     "begin",
    "end",     "if",      "then",    "else",    "procedure", "return", "call",
    "while",   "do",      "not",     "or",      "div",       "and",    "char",
    "integer", "boolean", "readln",  "writeln", "true",      "false",  "NUMBER",
    "STRING",  "+",       "-",       "*",       "=",         "<>",     "<",
    "<=",      ">",       ">=",      "(",       ")",         "[",      "]",
    ":=",      ".",       ",",       ":",       ";",         "read",   "write",
    "break"};

int error(char *mes) {
    fprintf(stderr, "Line: %4d ERROR: %s.\n", get_linenum(), mes);
    return ERROR;
}
// ###文法###
// プログラム (program) ::= "program" "名前" ";" ブロック "."
// ブロック (block) ::= { 変数宣言部 | 副プログラム宣言 } 複合文 変数宣言部 (variable declaration) ::=
// "var" 変数名の並び ":" 型 ";" { 変数名の並び ":" 型 ";" }
// 変数名の並び (variable names) ::= 変数名 { "," 変数名 } 変数名 (variable name) ::= "名前"
// 型 (type) ::= 標準型 | 配列型
// 標準型 (standard type) ::= "integer" | "boolean" | "char"
// 配列型 (array type) ::= "array" "[" "符号なし整数" "]" "of" 標準型 
// 副プログラム宣言 (subprogram declaration) ::= "procedure" 手続き名 [ 仮引数部 ] ";" [ 変数宣言部 ] 複合文 ";"
// 手続き名 (procedure name) ::= "名前" 仮引数部 (formal parameters) ::=
// "(" 変数名の並び ":" 型 { ";" 変数名の並び ":" 型 } ")" 複合文 (compound statement) ::= "begin" 文 { ";" 文 } "end"
// 文 (statement) ::= 代入文 | 分岐文 | 繰り返し文 | 脱出文 | 手続き呼び出し文 | 戻り文 | 入力文 | 出力文 | 複合文 | 空文
// 分岐文 (condition statement) ::= "if" 式 "then" 文 [ "else" 文 ]
// # どの"if"に対応するか曖昧な"else"は候補の内で最も近い"if"に対応するとする.
// 繰り返し文 (iteration statement) ::= "while" 式 "do" 文
// 脱出文 (exit statement) ::= "break"
// # この脱出文は少なくとも一つの繰り返し文に含まれていなくてはならない
// 手続き呼び出し文 (call statement) ::= "call" 手続き名 [ "(" 式の並び ")" ] 式の並び (expressions) ::= 式 { "," 式 }
// 戻り文 (return statement) ::= "return"
// 代入文 (assignment statement) ::= 左辺部 ":=" 式 左辺部 (left part) ::= 変数
// 変数 (variable) ::= 変数名 [ "[" 式 "]" ]
// 式 (expression) ::= 単純式 { 関係演算子 単純式 }
// # 関係演算子は左に結合的である.
// 単純式 (simple expression) ::= [ "+" | "-" ] 項 { 加法演算子 項 }
// # 加法演算子は左に結合的である.
// 項 (term) ::= 因子 { 乗法演算子 因子 }
// # 乗法演算子は左に結合的である.
// 因子 (factor) ::= 変数 | 定数 | "(" 式 ")" | "not" 因子 | 標準型 "(" 式 ")" 定数 (constant) ::= "符号なし整数" | "false" | "true" | "文字列"
// 乗法演算子 (multiplicative operator) ::= "*" | "div" | "and" 加法演算子 (additive operator) ::= "+" | "-" | "or"
// 関係演算子 (relational operator) ::= "=" | "<>" | "<" | "<=" | ">" | ">=" 入力文 (input statement) ::= ("read" | "readln") [ "(" 変数 { "," 変数 } ")" ] 出力文 (output statement) ::=
// ("write" | "writeln") [ "(" 出力指定 { "," 出力指定 } ")" ] 出力指定 (output format) ::= 式 [ ":" "符号なし整数" ] | "文字列"
// # この"文字列"の長さ (文字数)は 1以外である.
// # 長さが 1の場合は式から生成される定数の一つである"文字列"とする.
// 空文 (empty statement) ::= ε

// すべての関数をあらかじめ宣言しておく
int parse_program();
int parse_block();
int parse_var_decl();
int parse_var_names();
int parse_type();
int parse_standard_type();
int parse_array_type();
int parse_subpro_decl();
int parse_procedure_name();
int parse_formal_params();
int parse_comp_stmt();


int parse_program() {
    if (token != TPROGRAM) return (error("Keyword ’program’ is not found"));
    token = scan();
    if (token != TNAME) return (error("Program name is not found"));
    token = scan();
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan();
    if (parse_block() == ERROR) return (ERROR);
    if (token != TDOT) return(error("Period is not found at the end of program"));
    token = scan();
    return(NORMAL);
}

int parse_block() {
    while(token == TVAR || token == TPROCEDURE) {
        if(parse_var_decl() == ERROR) return(ERROR);
        if(parse_subpro_decl() == ERROR) return(ERROR);
    }
    if(parse_comp_stmt() == ERROR) return(ERROR);
    return(NORMAL);
}


int parse_var_decl() {
    if (token != TVAR) return (error("Keyword ’var’ is not found"));
    token = scan();
    if (token != TNAME) return (error("Variable name is not found"));
    token = scan();
    if (token != TCOLON) return (error("Colon is not found"));
    token = scan();
    if (token != TCHAR && token != TINTEGER && token != TBOOLEAN) return (error("Type is not found"));
    token = scan();
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan();
    while (token == TNAME) {
        token = scan();
        if (token != TCOLON) return (error("Colon is not found"));
        token = scan();
        if (token != TCHAR && token != TINTEGER && token != TBOOLEAN) return (error("Type is not found"));
        token = scan();
        if (token != TSEMI) return (error("Semicolon is not found"));
        token = scan();
    }
    return(NORMAL);
}

int parse_var_names() {
    if (token != TNAME) return (error("Variable name is not found"));
    token = scan();
    while (token == TCOMMA) {
        token = scan();
        if (token != TNAME) return (error("Variable name is not found"));
        token = scan();
    }
    return(NORMAL);
}



int main(int nc, char *np[]) {
    int tk, i;

    if (nc < 2) {
        error("File name is not given.");
        return 0;
    }

    if (init_scan(np[1]) < 0) {
        error("File can not open.");
        return 0;
    }

    token = scan();
    parse_program();
    end_scan();

    return 0;
}