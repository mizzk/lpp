#include "token-list.h"


// 記号表のための構造体の定義

// 型
struct TYPE {
    int ttype;        /* TPINT TPCHAR TPBOOL TPARRAYINT TPARRAYCHAR TPARRAYBOOL
           TPPROC */
    int arraysize;    /* 配列型の場合の配列サイズ */
    struct TYPE *etp; /* 配列型の場合の要素の型 */
    struct TYPE *paratp; /* 手続き型の場合の，仮引数の型リスト */
};

// 行番号
struct LINE {
    int reflinenum;
    struct LINE *nextlinep;
};

// 記号表
struct ID {
    char *name;
    char *procname; /* 手続き宣言内で定義されている時の手続き名．それ以外はNULL
                     */
    struct TYPE *itp;   /* 型 */
    int ispara;         /* 1:仮引数, 0:通常の変数 */
    int deflinenum;     /* 定義行 */
    struct LINE *irefp; /* 参照行のリスト */
    struct ID *nextp;
} *globalidroot,
    *localidroot; /* Pointers to root of global & local symboltables */


// 読み込んだトークンを格納する変数
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

// 行番号とともにエラーメッセージを出力する関数
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

// 　空文のフラッグ
int empty_stmt_flag = 0;

// 繰り返し文のフラッグ
int while_flag = 0;

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

// プログラムの構文解析関数
int parse_program() {
    if (token != TPROGRAM) return (error("Keyword 'program' is not found"));
    token = scan();  // printf("program");
    if (token != TNAME) return (error("Program name is not found"));
    token = scan();  // printf(" %s", string_attr);
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan();  // printf(";\n");
    if (parse_block() == ERROR) return (ERROR);
    if (token != TDOT)
        return (error("Period is not found at the end of program"));
    token = scan();  // printf(".\n");
    return (NORMAL);
}

// ブロックの構文解析関数
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

// 変数宣言部の構文解析関数
int parse_var_decl() {
    if (token != TVAR) return (error("Keyword 'var' is not found"));
    token = scan();
    indent_print(1);
    printf("var\n");
    indent_print(2);
    if (parse_var_names() == ERROR) return (ERROR);
    if (token != TCOLON) return (error("Colon is not found"));
    token = scan();
    printf(" : ");
    if (parse_type() == ERROR) return (ERROR);
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan();
    printf(";\n");
    while (token == TNAME) {
        indent_print(2);
        if (parse_var_names() == ERROR) return (ERROR);
        if (token != TCOLON) return (error("Colon is not found"));
        token = scan();
        printf(" : ");
        if (parse_type() == ERROR) return (ERROR);
        if (token != TSEMI) return (error("Semicolon is not found"));
        token = scan();
        printf(";\n");
    }
    indent = 0;
    return (NORMAL);
}

// 変数名の並びの構文解析関数
int parse_var_names() {
    if (parse_var_name() == ERROR) return (ERROR);
    while (token == TCOMMA) {
        token = scan();
        printf(" , ");
        if (parse_var_name() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

// 変数名の構文解析関数
int parse_var_name() {
    if (token != TNAME) return (error("Variable name is not found"));
    token = scan();
    printf("%s", string_attr);
    return (NORMAL);
}

// 型の構文解析関数
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

// 標準型の構文解析関数
int parse_standard_type() {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        printf("%s", tokenstr[token]);
        token = scan();
        return (NORMAL);
    } else {
        return (error("Standard type is not found"));
    }
}

// 配列型の構文解析関数
int parse_array_type() {
    if (token != TARRAY) return (error("Keyword 'array' is not found"));
    token = scan();
    printf("array");
    if (token != TLSQPAREN)
        return (error("Left square parenthesis is not found"));
    token = scan();
    printf(" [ ");
    if (token != TNUMBER) return (error("Number is not found"));
    token = scan();
    printf("%d", num_attr);
    if (token != TRSQPAREN)
        return (error("Right square parenthesis is not found"));
    token = scan();
    printf(" ] ");
    if (token != TOF) return (error("Keyword 'of' is not found"));
    token = scan();
    printf("of ");
    if (parse_standard_type() == ERROR) return (ERROR);
    return (NORMAL);
}

// 部分プログラム宣言部の構文解析関数
int parse_subpro_decl() {
    indent_print(1);
    if (token != TPROCEDURE) return (error("Keyword 'procedure' is not found"));
    token = scan();
    printf("procedure ");
    if (parse_procedure_name() == ERROR) return (ERROR);
    if (token == TLPAREN) {
        if (parse_formal_params() == ERROR) return (ERROR);
    }
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan();
    printf(";\n");
    if (token == TVAR) {
        if (parse_var_decl() == ERROR) return (ERROR);
    }
    indent = 1;
    if (parse_comp_stmt() == ERROR) return (ERROR);
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan();
    printf(";\n");
    indent = 0;
    return (NORMAL);
}

// 手続き名の構文解析関数
int parse_procedure_name() {
    if (token != TNAME) return (error("Procedure name is not found"));
    token = scan();
    printf("%s", string_attr);
    return (NORMAL);
}

// 仮引数部の構文解析関数
int parse_formal_params() {
    if (token != TLPAREN) return (error("Left parenthesis is not found"));
    token = scan();
    printf(" ( ");
    if (parse_var_names() == ERROR) return (ERROR);
    if (token != TCOLON) return (error("Colon is not found"));
    token = scan();
    printf(" : ");
    if (parse_type() == ERROR) return (ERROR);
    while (token == TSEMI) {
        token = scan();
        printf("; ");
        if (parse_var_names() == ERROR) return (ERROR);
        if (token != TCOLON) return (error("Colon is not found"));
        token = scan();
        printf(" : ");
        if (parse_type() == ERROR) return (ERROR);
    }
    if (token != TRPAREN) return (error("Right parenthesis is not found"));
    token = scan();
    printf(" )");
    return (NORMAL);
}

// 複合文の構文解析関数
int parse_comp_stmt() {
    if (token != TBEGIN) return (error("Keyword 'begin' is not found"));
    token = scan();
    int original_indent = indent;
    indent_print(indent);
    printf("begin\n");
    indent++;
    if (parse_stmt() == ERROR) return (ERROR);
    while (token == TSEMI) {
        if (empty_stmt_flag == 1) {
            indent_print(indent);
            empty_stmt_flag = 0;
        }
        token = scan();
        printf(";\n");
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
    token = scan();
    printf("end");
    return (NORMAL);
}

// 文の構文解析関数
int parse_stmt() {
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

// 条件文の構文解析関数
int parse_cond_stmt() {
    indent_print(indent);
    if (token != TIF) return (error("Keyword 'if' is not found"));
    token = scan();
    printf("if ");
    if_indent = indent;
    if (parse_expression() == ERROR) return (ERROR);
    if (token != TTHEN) return (error("Keyword 'then' is not found"));
    token = scan();
    printf(" then\n");
    indent++;
    if (parse_stmt() == ERROR) return (ERROR);
    if (token == TELSE) {
        printf("\n");
        token = scan();
        indent_print(if_indent);
        printf("else\n");
        indent = if_indent + 1;
        if (parse_stmt() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

// 繰り返し文の構文解析関数
int parse_iter_stmt() {
    while_flag = 1;
    indent_print(indent);
    if (token != TWHILE) return (error("Keyword 'while' is not found"));
    token = scan();
    printf("while ");
    if (parse_expression() == ERROR) return (ERROR);
    if (token != TDO) return (error("Keyword 'do' is not found"));
    token = scan();
    printf(" do\n");
    indent++;
    if (parse_stmt() == ERROR) return (ERROR);
    indent--;
    indent--;
    // while_flag = 0;
    return (NORMAL);
}

// 脱出文の構文解析関数
int parse_exit_stmt() {
    indent_print(indent);
    if (token != TBREAK) return (error("Keyword 'break' is not found"));
    token = scan();
    printf("break");
    // indent--;
    if (while_flag == 0) {
        return (error("Break statement is not in while statement"));
    }
    return (NORMAL);
}

// 手続き呼び出し文の構文解析関数
int parse_call_stmt() {
    indent_print(indent);
    if (token != TCALL) return (error("Keyword 'call' is not found"));
    token = scan();
    printf("call ");
    if (parse_procedure_name() == ERROR) return (ERROR);
    if (token == TLPAREN) {
        token = scan();
        printf(" ( ");
        if (parse_expressions() == ERROR) return (ERROR);
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan();
        printf(" )");
    }
    indent--;
    return (NORMAL);
}

// 式の並びの構文解析関数
int parse_expressions() {
    if (parse_expression() == ERROR) return (ERROR);
    while (token == TCOMMA) {
        token = scan();
        printf(" , ");
        if (parse_expression() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

// 戻り文の構文解析関数
int parse_return_stmt() {
    indent_print(indent);
    if (token != TRETURN) return (error("Keyword 'return' is not found"));
    token = scan();
    printf("return");
    indent--;
    return (NORMAL);
}

// 代入文の構文解析関数
int parse_assign_stmt() {
    indent_print(indent);
    if (parse_left_part() == ERROR) return (ERROR);
    if (token != TASSIGN) return (error("Assignment operator is not found"));
    token = scan();
    printf(" := ");
    if (parse_expression() == ERROR) return (ERROR);
    indent--;
    return (NORMAL);
}

// 左辺部の構文解析関数
int parse_left_part() {
    if (parse_variable() == ERROR) return (ERROR);
    return (NORMAL);
}

// 変数の構文解析関数
int parse_variable() {
    if (parse_var_name() == ERROR) return (ERROR);
    if (token == TLSQPAREN) {
        token = scan();
        printf(" [ ");
        if (parse_expression() == ERROR) return (ERROR);
        if (token != TRSQPAREN)
            return (error("Right square parenthesis is not found"));
        token = scan();
        printf(" ]");
    }
    return (NORMAL);
}

// 式の構文解析関数
int parse_expression() {
    if (parse_simple_expression() == ERROR) return (ERROR);
    while (token == TEQUAL || token == TNOTEQ || token == TLE ||
           token == TLEEQ || token == TGR || token == TGREQ) {
        printf(" ");
        if (parse_relational_op() == ERROR) return (ERROR);
        printf(" ");
        if (parse_simple_expression() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

// 単純式の構文解析関数
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

// 項の構文解析関数
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

// 因子の構文解析関数
int parse_factor() {
    if (token == TNAME) {
        if (parse_variable() == ERROR) return (ERROR);
    } else if (token == TNUMBER || token == TFALSE || token == TTRUE ||
               token == TSTRING) {
        if (parse_constant() == ERROR) return (ERROR);
    } else if (token == TLPAREN) {
        token = scan();
        printf("( ");
        if (parse_expression() == ERROR) return (ERROR);
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan();
        printf(" )");
    } else if (token == TNOT) {
        token = scan();
        printf("not ");
        if (parse_factor() == ERROR) return (ERROR);
    } else if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        printf("%s", tokenstr[token]);
        token = scan();
        if (token != TLPAREN) return (error("Left parenthesis is not found"));
        token = scan();
        printf(" ( ");
        if (parse_expression() == ERROR) return (ERROR);
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan();
        printf(" )");
    } else {
        return (error("Factor is not found"));
    }
    return (NORMAL);
}

// 定数の構文解析関数
int parse_constant() {
    if (token == TNUMBER) {
        token = scan();
        printf("%d", num_attr);
    } else if (token == TSTRING) {
        token = scan();
        printf("\'%s\'", string_attr);
    } else if (token == TFALSE) {
        token = scan();
        printf("false");
    } else if (token == TTRUE) {
        token = scan();
        printf("true");
    } else {
        return (error("Constant is not found"));
    }
    return (NORMAL);
}

// 乗算演算子の構文解析関数
int parse_multiplicative_op() {
    if (token == TSTAR || token == TDIV || token == TAND) {
        printf("%s", tokenstr[token]);
        token = scan();
    } else {
        return (error("Multiplicative operator is not found"));
    }
    return (NORMAL);
}

// 加算演算子の構文解析関数
int parse_additive_op() {
    if (token == TPLUS || token == TMINUS || token == TOR) {
        printf("%s", tokenstr[token]);
        token = scan();
    } else {
        return (error("Additive operator is not found"));
    }
    return (NORMAL);
}

// 関係演算子の構文解析関数
int parse_relational_op() {
    if (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ ||
        token == TGR || token == TGREQ) {
        printf("%s", tokenstr[token]);
        token = scan();
    } else {
        return (error("Relational operator is not found"));
    }
    return (NORMAL);
}

// 入力文の構文解析関数
int parse_input_stmt() {
    indent_print(indent);
    if (token != TREAD && token != TREADLN)
        return (error("Keyword 'read' or 'readln' is not found"));
    printf("%s", tokenstr[token]);
    token = scan();
    if (token == TLPAREN) {
        token = scan();
        printf(" ( ");
        if (parse_variable() == ERROR) return (ERROR);
        while (token == TCOMMA) {
            token = scan();
            printf(" , ");
            if (parse_variable() == ERROR) return (ERROR);
        }
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan();
        printf(" )");
    }
    indent--;
    return (NORMAL);
}

// 出力文の構文解析関数
int parse_output_stmt() {
    indent_print(indent);
    if (token != TWRITE && token != TWRITELN)
        return (error("Keyword 'write' or 'writeln' is not found"));
    printf("%s", tokenstr[token]);
    token = scan();
    if (token == TLPAREN) {
        token = scan();
        printf(" ( ");
        if (parse_output_format() == ERROR) return (ERROR);
        while (token == TCOMMA) {
            token = scan();
            printf(" , ");
            if (parse_output_format() == ERROR) return (ERROR);
        }
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan();
        printf(" )");
    }
    indent--;
    return (NORMAL);
}

// 出力書式の構文解析関数
int parse_output_format() {
    if (token == TPLUS || token == TMINUS || token == TNAME ||
        token == TNUMBER || token == TFALSE || token == TTRUE ||
        token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        if (parse_expression() == ERROR) return (ERROR);
        if (token == TCOLON) {
            token = scan();
            printf(" : ");
            if (token != TNUMBER) return (error("Number is not found"));
            token = scan();
            printf("%d", num_attr);
        }
    } else if (token == TSTRING) {
        token = scan();
        printf("\'%s\'", string_attr);
    } else {
        return (error("Output format is not found"));
    }
    return (NORMAL);
}

// 空文の構文解析関数
int parse_empty_stmt() {
    empty_stmt_flag = 1;
    return (NORMAL);
}

// 実際の処理
int main(int nc, char *np[]) {
    int tk, i;

    // 引数のチェック
    if (nc < 2) {
        error("File name is not given.");
        return 0;
    }

    // ファイルのオープン
    if (init_scan(np[1]) < 0) {
        error("File can not open.");
        return 0;
    }

    // まず1つめのトークンを読んでおく
    token = scan();

    // 構文解析
    parse_program();

    // ファイルのクローズ
    end_scan();

    return 0;
}