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

// 段付けの数をカウントする変数
int indent = 0;

// 段付けをする関数
void indent_print(int ind) {
    for (int i = 0; i < ind; i++) {
        printf("    ");
    }
}

// ifを出力するたびに、そのインデントを記録する変数
int if_indent = 0;

//　空文のフラッグ
int empty_stmt_flag = 0;

// 繰り返し文のフラッグ
int while_flag = 0;

// ###文法###
// プログラム (program) ::= "program" "名前" ";" ブロック "."
// ブロック (block) ::= { 変数宣言部 | 副プログラム宣言 } 複合文
// 変数宣言部 (variable declaration) ::="var" 変数名の並び ":" 型 ";" { 変数名の並び ":" 型 ";" }
// 変数名の並び (variable names) ::= 変数名 { "," 変数名 }
// 変数名 (variable name) ::= "名前"
// 型 (type) ::= 標準型 | 配列型
// 標準型 (standard type) ::= "integer" | "boolean" | "char"
// 配列型 (array type) ::= "array" "[" "符号なし整数" "]" "of" 標準型
// 副プログラム宣言 (subprogram declaration) ::="procedure" 手続き名 [ 仮引数部 ] ";" [ 変数宣言部 ] 複合文 ";"
// 手続き名 (procedure name) ::= "名前"
// 仮引数部 (formal parameters) ::="(" 変数名の並び ":" 型 { ";" 変数名の並び ":" 型 } ")"
// 複合文 (compound statement) ::= "begin" 文 { ";" 文 } "end"
// 文 (statement) ::= 代入文 | 分岐文 | 繰り返し文 | 脱出文 | 手続き呼び出し文 | 戻り文 | 入力文 | 出力文 | 複合文 | 空文
// 分岐文 (condition statement) ::= "if" 式 "then" 文 [ "else" 文 ]
// # どの"if"に対応するか曖昧な"else"は候補の内で最も近い"if"に対応するとする．
// 繰り返し文 (iteration statement) ::= "while" 式 "do" 文
// 脱出文 (exit statement) ::= "break"
// # この脱出文は少なくとも一つの繰り返し文に含まれていなくてはならない
// 手続き呼び出し文 (call statement) ::= "call" 手続き名 [ "(" 式の並び ")" ]
// 式の並び (expressions) ::= 式 { "," 式 }
// 戻り文 (return statement) ::= "return"
// 代入文 (assignment statement) ::= 左辺部 ":=" 式
// 左辺部 (left part) ::= 変数
// 変数 (variable) ::= 変数名 [ "[" 式 "]" ]
// 式 (expression) ::= 単純式 { 関係演算子 単純式 }
// # 関係演算子は左に結合的である．
// 単純式 (simple expression) ::= [ "+" | "-" ] 項 { 加法演算子 項 }
// # 加法演算子は左に結合的である．
// 項 (term) ::= 因子 { 乗法演算子 因子 }
// # 乗法演算子は左に結合的である．
// 因子 (factor) ::= 変数 | 定数 | "(" 式 ")" | "not" 因子 | 標準型 "(" 式 ")"
// 定数 (constant) ::= "符号なし整数" | "false" | "true" | "文字列"
// 乗法演算子 (multiplicative operator) ::= "*" | "div" | "and"
// 加法演算子 (additive operator) ::= "+" | "-" | "or"
// 関係演算子 (relational operator) ::= "=" | "<>" | "<" | "<=" | ">" | ">="
// 入力文 (input statement) ::= ("read" | "readln") [ "(" 変数 { "," 変数 } ")" ]
// 出力文 (output statement) ::=("write" | "writeln") [ "(" 出力指定 { "," 出力指定 } ")" ]
// 出力指定 (output format) ::= 式 [ ":" "符号なし整数" ] | "文字列"
// # この"文字列"の長さ (文字数)は 1以外である．
// # 長さが 1の場合は式から生成される定数の一つである"文字列"とする．
// 空文 (empty statement) ::= ε


// プリティプリント
// プログラムのプリティプリントとは，見やすく段付けして印刷されたプログラムリスト
// である．プログラムの見やすさには主観的な要素が多く，どのようなリストがもっとも見
// やすいかは一概に言えないが，本課題においては，次の条件を満たすものとする．
// • 段付の 1 段は空白 4 文字とする．
// • 行頭を除いて複数の空白は連続しない．行頭を除いてタブは現れない．行末には空
// 白やタブは現れない．すなわち，特に指定されていない限り，行中の字句と字句の
// 間は 1 つの空白だけがある．
// • 前項の指示に関わらず，";", "."の直前には空白を入れない．
// • 字句";"の次は必ず改行される．ただし，仮引数中の";"については改行しない．
// • 最初の字句"program"は段付けされない．つまり，1 カラム目 (行頭) から表示さ
// れる．
// • 変数宣言部"var"は行頭から 1 段段付けされる．また，変数の並びは改行し，"var"
// からさらに 1 段段付けする．
// • 副プログラム宣言 (キーワード)"procedure"で始まる行は行頭から 1 段段付けさ
// れる．
// • 副プログラム宣言内の一番外側の"begin", "end"は行頭から 1 段段付けされる．
// • 対応する"begin", "end"が段付けされるときは同じ量だけ段付けされる．直前の
// 字句に引き続いて”end”を印刷すると，対応する"begin"より段付け量が多くなる
// ときは改行して同じにする．
// • 対応する"begin", "end"の間の文は，その"begin", "end"より少なくとも 1 段多
// く段付けされる．
// • 分岐文の"else"の前では必ず改行し，その段付けの量は対応する"if"と同じで
// ある．
// • 分岐文，繰り返し文中の文が複合文でなく，"then", "else", "do"の次で改行する
// ときは，その改行後の文は"if", "while"よりも 1 段多く段付けされる．
// • 分岐文，繰り返し文中の文が複合文のときは，その先頭の"begin"の前で改行し，
// 段付けする．
// • 以上に指定のない点については，見やすさに基づいて字句を配置すること．
// • 注釈は削除する．その結果，構文解析結果が変わるときには空白を一つ入れる．
// 直感的には，プリティプリントとは，注釈及び無駄な空白やタブがなく (字句と字句の
// 間には一つ空白があってよい)，副プログラム宣言部や複合文，ブロック内部はそのすぐ
// 外よりも一段段付けがされているようなプログラムリストである．


// すべての関数をあらかじめ宣言しておく
int parse_program();
int parse_block();
int parse_var_decl();
int parse_var_names();
int parse_var_name();
int parse_type();
int parse_standard_type();
int parse_array_type();
int parse_subpro_decl();
int parse_procedure_name();
int parse_formal_params();
int parse_comp_stmt();
int parse_stmt();
int parse_cond_stmt();
int parse_iter_stmt();
int parse_exit_stmt();
int parse_call_stmt();
int parse_expressions();
int parse_return_stmt();
int parse_assign_stmt();
int parse_left_part();
int parse_variable();
int parse_expression();
int parse_simple_expression();
int parse_term();
int parse_factor();
int parse_constant();
int parse_multiplicative_op();
int parse_additive_op();
int parse_relational_op();
int parse_input_stmt();
int parse_output_stmt();
int parse_output_format();
int parse_empty_stmt();

// OK
int parse_program() {
    if (token != TPROGRAM) return (error("Keyword 'program' is not found"));
    token = scan(); printf("program");
    if (token != TNAME) return (error("Program name is not found"));
    token = scan(); printf(" %s", string_attr);
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan(); printf(";\n");
    if (parse_block() == ERROR) return (ERROR);
    if (token != TDOT) return(error("Period is not found at the end of program"));
    token = scan(); printf(".\n");
    return(NORMAL);
}

// OK
int parse_block() {
    while (token == TVAR || token == TPROCEDURE) {
        if (token == TVAR) {
            if (parse_var_decl() == ERROR) return (ERROR);
        } else if (token == TPROCEDURE) {
            if (parse_subpro_decl() == ERROR) return (ERROR);
        }
    }
    if (parse_comp_stmt() == ERROR) return (ERROR);
    return (NORMAL);
}

// OK
int parse_var_decl() {
    if (token != TVAR) return (error("Keyword 'var' is not found"));
    token = scan();
    indent_print(1);
    printf("var\n");
    indent_print(2);
    if (parse_var_names() == ERROR) return (ERROR);
    if (token != TCOLON) return (error("Colon is not found"));
    token = scan(); printf(" : ");
    if (parse_type() == ERROR) return (ERROR);
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan(); printf(";\n");
    while (token == TNAME) {
        indent_print(2);
        if (parse_var_names() == ERROR) return (ERROR);
        if (token != TCOLON) return (error("Colon is not found"));
        token = scan(); printf(" : ");
        if (parse_type() == ERROR) return (ERROR);
        if (token != TSEMI) return (error("Semicolon is not found"));
        token = scan(); printf(";\n");
    }
    indent = 0;
    return (NORMAL);
}

// OK
int parse_var_names() {
    if (parse_var_name() == ERROR) return (ERROR);
    while (token == TCOMMA) {
        token = scan(); printf(" , ");
        if (parse_var_name() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

// OK
int parse_var_name() {
    if (token != TNAME) return (error("Variable name is not found"));
    token = scan(); printf("%s", string_attr);
    return (NORMAL);
}

// OK
int parse_type() {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        if (parse_standard_type() == ERROR) return (ERROR);
    } else if (token == TARRAY) {
        if (parse_array_type() == ERROR) return (ERROR);
    } else {
        return (error("Type is not found"));
    }
    return (NORMAL);
}

// OK
int parse_standard_type() {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        printf("%s", tokenstr[token]);
        token = scan();
        return (NORMAL);
    } else {
        return (error("Standard type is not found"));
    }
}

// OK
int parse_array_type() {
    if (token != TARRAY) return (error("Keyword 'array' is not found"));
    token = scan(); printf("array");
    if (token != TLSQPAREN) return (error("Left square parenthesis is not found"));
    token = scan(); printf(" [ ");
    if (token != TNUMBER) return (error("Number is not found"));
    token = scan(); printf("%d", num_attr);
    if (token != TRSQPAREN) return (error("Right square parenthesis is not found"));
    token = scan(); printf(" ] ");
    if (token != TOF) return (error("Keyword 'of' is not found"));
    token = scan(); printf("of ");
    if (parse_standard_type() == ERROR) return (ERROR);
    return (NORMAL);
}

int parse_subpro_decl() {
    indent_print(1);
    if (token != TPROCEDURE) return (error("Keyword 'procedure' is not found"));
    token = scan(); printf("procedure ");
    if (parse_procedure_name() == ERROR) return (ERROR);
    if (token == TLPAREN) {
        if (parse_formal_params() == ERROR) return (ERROR);
    }
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan(); printf(";\n");
    if (token == TVAR) {
        if (parse_var_decl() == ERROR) return (ERROR);
    } //ここまでOK
    indent = 1;
    if (parse_comp_stmt() == ERROR) return (ERROR);
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan(); printf(";\n");
    indent = 0;
    return (NORMAL);
}

// OK
int parse_procedure_name() {
    if (token != TNAME) return (error("Procedure name is not found"));
    token = scan(); printf("%s", string_attr);
    return (NORMAL);
}

// OK
int parse_formal_params() {
    if (token != TLPAREN) return (error("Left parenthesis is not found"));
    token = scan(); printf(" ( ");
    if (parse_var_names() == ERROR) return (ERROR);
    if (token != TCOLON) return (error("Colon is not found"));
    token = scan(); printf(" : ");
    if (parse_type() == ERROR) return (ERROR);
    while (token == TSEMI) {
        token = scan(); printf("; ");
        if (parse_var_names() == ERROR) return (ERROR);
        if (token != TCOLON) return (error("Colon is not found"));
        token = scan(); printf(" : ");
        if (parse_type() == ERROR) return (ERROR);
    }
    if (token != TRPAREN) return (error("Right parenthesis is not found"));
    token = scan(); printf(" )");
    return (NORMAL);
}

int parse_comp_stmt() {
    if (token != TBEGIN) return (error("Keyword 'begin' is not found"));
    token = scan();
    int original_indent = indent;
    indent_print(indent);
    printf("begin\n");
    indent++;
    if (parse_stmt() == ERROR) return (ERROR);
    while (token == TSEMI) {
        if(empty_stmt_flag == 1) {
            indent_print(indent);
            empty_stmt_flag = 0;
        }
        token = scan(); printf(";\n");
        indent = original_indent + 1;
        if (parse_stmt() == ERROR) return (ERROR);
    }
    if (token != TEND) return (error("Keyword 'end' is not found"));
    if (empty_stmt_flag == 1) {
        empty_stmt_flag = 0;
    } else {
        printf("\n");
    }
    indent_print(original_indent);
    token = scan(); printf("end");
    return (NORMAL);
}

// OK
int parse_stmt() {
    // indent_print(indent);
    if (token == TNAME) {
        if (parse_assign_stmt() == ERROR) return (ERROR);
    } else if (token == TIF) {
        if (parse_cond_stmt() == ERROR) return (ERROR);
    } else if (token == TWHILE) {
        if (parse_iter_stmt() == ERROR) return (ERROR);
    } else if (token == TBREAK) {
        if (parse_exit_stmt() == ERROR) return (ERROR);
    } else if (token == TCALL) {
        if (parse_call_stmt() == ERROR) return (ERROR);
    } else if (token == TRETURN) {
        if (parse_return_stmt() == ERROR) return (ERROR);
    } else if (token == TREAD || token == TREADLN) {
        if (parse_input_stmt() == ERROR) return (ERROR);
    } else if (token == TWRITE || token == TWRITELN) {
        if (parse_output_stmt() == ERROR) return (ERROR);
    } else if (token == TBEGIN) {
        if (parse_comp_stmt() == ERROR) return (ERROR);
    } else if (token == TSEMI || token == TEND) {
        if (parse_empty_stmt() == ERROR) return (ERROR);
    } else {
        return (error("Statement is not found"));
    }
    return (NORMAL);
}

// OK
int parse_cond_stmt() {
    indent_print(indent);
    if (token != TIF) return (error("Keyword 'if' is not found"));
    token = scan(); printf("if ");
    if_indent = indent;
    if (parse_expression() == ERROR) return (ERROR);
    if (token != TTHEN) return (error("Keyword 'then' is not found"));
    token = scan(); printf(" then\n");
    indent++;
    if (parse_stmt() == ERROR) return (ERROR);
    if (token == TELSE) {
    printf("\n");
        token = scan();
        // indent--; indent_print(original_indent);
        indent_print(if_indent);
        printf("else\n");
        indent = if_indent+1;
        if (parse_stmt() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

// OK
int parse_iter_stmt() {
    while_flag = 1;
    indent_print(indent);
    if (token != TWHILE) return (error("Keyword 'while' is not found"));
    token = scan(); printf("while ");
    if (parse_expression() == ERROR) return (ERROR);
    if (token != TDO) return (error("Keyword 'do' is not found"));
    token = scan(); printf(" do\n"); indent++;
    if (parse_stmt() == ERROR) return (ERROR);
    indent--;
    indent--;
    // while_flag = 0;
    return (NORMAL);
}

// OK
int parse_exit_stmt() {
    indent_print(indent);
    if (token != TBREAK) return (error("Keyword 'break' is not found"));
    token = scan(); printf("break");
    // indent--;
    if(while_flag == 0) {
        return (error("Break statement is not in while statement"));
    }
    return (NORMAL);
}

// OK
int parse_call_stmt() {
    indent_print(indent);
    if (token != TCALL) return (error("Keyword 'call' is not found"));
    token = scan(); printf("call ");
    if (parse_procedure_name() == ERROR) return (ERROR);
    if (token == TLPAREN) {
        token = scan(); printf(" ( ");
        if (parse_expressions() == ERROR) return (ERROR);
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan(); printf(" ) ");
    }
    indent--;
    return (NORMAL);
}

// OK
int parse_expressions() {
    if (parse_expression() == ERROR) return (ERROR);
    while (token == TCOMMA) {
        token = scan(); printf(" , ");
        if (parse_expression() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

// OK
int parse_return_stmt() {
    indent_print(indent);
    if (token != TRETURN) return (error("Keyword 'return' is not found"));
    token = scan(); printf("return");
    indent--;
    return (NORMAL);
}

// OK
int parse_assign_stmt() {
    indent_print(indent);   
    if (parse_left_part() == ERROR) return (ERROR);
    if (token != TASSIGN) return (error("Assignment operator is not found"));
    token = scan(); printf(" := ");
    if (parse_expression() == ERROR) return (ERROR);
    indent--;
    return (NORMAL);
}

// OK
int parse_left_part() {
    if (parse_variable() == ERROR) return (ERROR);
    return (NORMAL);
}

int parse_variable() {
    if (parse_var_name() == ERROR) return (ERROR);
    if (token == TLSQPAREN) {
        token = scan(); printf(" [ ");
        if (parse_expression() == ERROR) return (ERROR);
        if (token != TRSQPAREN) return (error("Right square parenthesis is not found"));
        token = scan(); printf(" ]");
    }
    return (NORMAL);
}

// OK
int parse_expression() {
    if (parse_simple_expression() == ERROR) return (ERROR);
    while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
        printf(" ");
        if (parse_relational_op() == ERROR) return (ERROR);
        printf(" ");
        if (parse_simple_expression() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

// OK
int parse_simple_expression() {
    if (token == TPLUS || token == TMINUS) {
        printf("%s ", tokenstr[token]);
        token = scan();
    }
    if (parse_term() == ERROR) return (ERROR);
    while (token == TPLUS || token == TMINUS || token == TOR) {
        printf(" ");
        if (parse_additive_op() == ERROR) return (ERROR);
        printf(" ");
        if (parse_term() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

int parse_term() {
    if (parse_factor() == ERROR) return (ERROR);
    while (token == TSTAR || token == TDIV || token == TAND) {
        printf(" ");
        if (parse_multiplicative_op() == ERROR) return (ERROR);
        printf(" ");
        if (parse_factor() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

int parse_factor() {
    if (token == TNAME) {
        if (parse_variable() == ERROR) return (ERROR);
    } else if (token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING) {
        if (parse_constant() == ERROR) return (ERROR);
    } else if (token == TLPAREN) {
        token = scan(); printf("( ");
        if (parse_expression() == ERROR) return (ERROR);
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan(); printf(" )");
    } else if (token == TNOT) {
        token = scan(); printf("not ");
        if (parse_factor() == ERROR) return (ERROR);
    } else if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        printf("%s", tokenstr[token]);
        token = scan();
        if (token != TLPAREN) return (error("Left parenthesis is not found"));
        token = scan(); printf(" ( ");
        if (parse_expression() == ERROR) return (ERROR);
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan(); printf(" )");
    } else {
        return (error("Factor is not found"));
    }
    return (NORMAL);
}

int parse_constant() {
    if (token == TNUMBER) {
        token = scan(); printf("%d", num_attr);
    } else if (token == TSTRING) {
        token = scan(); printf("\'%s\'", string_attr);
    } else if (token == TFALSE) {
        token = scan(); printf("false");
    } else if (token == TTRUE) {
        token = scan(); printf("true");
    } else {
        return (error("Constant is not found"));
    }
    return (NORMAL);
}

int parse_multiplicative_op() {
    if (token == TSTAR || token == TDIV || token == TAND) {
        printf("%s", tokenstr[token]);
        token = scan();  
    } else {
        return (error("Multiplicative operator is not found"));
    }
    return (NORMAL);
}

int parse_additive_op() {
    if (token == TPLUS || token == TMINUS || token == TOR) {
        printf("%s", tokenstr[token]);
        token = scan(); 
    } else {
        return (error("Additive operator is not found"));
    }
    return (NORMAL);
}

int parse_relational_op() {
    if (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
        printf("%s", tokenstr[token]);
        token = scan(); 
    } else {
        return (error("Relational operator is not found"));
    }
    return (NORMAL);
}

int parse_input_stmt() {
    indent_print(indent);
    if (token != TREAD && token != TREADLN) return (error("Keyword 'read' or 'readln' is not found"));
    printf("%s", tokenstr[token]);
    token = scan(); 
    if (token == TLPAREN) {
        token = scan(); printf(" ( ");
        if (parse_variable() == ERROR) return (ERROR);
        while (token == TCOMMA) {
            token = scan(); printf(" , ");
            if (parse_variable() == ERROR) return (ERROR);
        }
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan(); printf(" )");
    }
    indent--;
    return (NORMAL);
}

int parse_output_stmt() {
    indent_print(indent);
    if (token != TWRITE && token != TWRITELN) return (error("Keyword 'write' or 'writeln' is not found"));
    printf("%s", tokenstr[token]);
    token = scan(); 
    if (token == TLPAREN) {
        token = scan(); printf(" ( ");
        if (parse_output_format() == ERROR) return (ERROR);
        while (token == TCOMMA) {
            token = scan(); printf(" , ");
            if (parse_output_format() == ERROR) return (ERROR);
        }
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan(); printf(" )");
    }
    indent--;
    return (NORMAL);
}

int parse_output_format() {
    if (token == TPLUS || token == TMINUS || token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        if (parse_expression() == ERROR) return (ERROR);
        if (token == TCOLON) {
            token = scan(); printf(" : ");
            if (token != TNUMBER) return (error("Number is not found"));
            token = scan(); printf("%d", num_attr);
        }
    } else if (token = TSTRING) {
        token = scan(); printf("\'%s\'", string_attr);
    } else {
        return (error("Output format is not found"));
    }
    return (NORMAL);
}

int parse_empty_stmt() {
    empty_stmt_flag = 1;
    return (NORMAL);
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