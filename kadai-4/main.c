#include "token-list.h"
#define ENABLE_PRETTY_PRINT 0
#define ENABLE_PRETTY_PRINT_IN_CASL 1


// 入力ファイル
// sample11pp.mpl
// program sample11pp;
// procedure kazuyomikomi(n : integer);
// begin
// writeln(’input the number of data’);
// readln(n)
// end;
// var sum : integer;
// procedure wakakidasi;
// begin
// writeln(’Sum of data = ’, sum)
// end;
// var data : integer;
// procedure goukei(n, s : integer);
// var data : integer;
// begin
// s := 0;
// while n > 0 do begin
// readln(data);
// s := s + data;
// n := n - 1
// end
// end;
// var n : integer;
// begin
// call kazuyomikomi(n);
// call goukei(n * 2, sum);
// call wakakidasi
// end.

// 出力ファイル
// 1 %%sample11pp START L0001
// 2 ; program sample11pp;
// 3 $$n%kazuyomikomi
// 4 DC 0
// 5 ; procedure kazuyomikomi ( n : integer );
// 6 $kazuyomikomi
// 7 POP GR2
// 8 POP GR1
// 9 ST GR1,$$n%kazuyomikomi
// 10 PUSH 0,GR2
// 11 ; begin
// 12 LAD GR1,=’input the number of data’
// 13 LAD GR2,0
// 14 CALL WRITESTR
// 15 CALL WRITELINE
// 16 ; writeln ( ’input the number of data’ );
// 17 LD GR1,$$n%kazuyomikomi
// 18 CALL READINT
// 19 CALL READLINE
// 20 ; readln ( n )
// 21 RET
// 22 ; end;
// 23 ; var
// 24 $sum DC 0
// 25 ; sum : integer;
// 26 ; procedure wakakidasi;
// 27 $wakakidasi
// 28 ; begin
// 29 LAD GR1,=’Sum of data = ’
// 30 LAD GR2,0
// 31 CALL WRITESTR
// 32 LD GR1,$sum
// 33 LAD GR2,0
// 34 CALL WRITEINT
// 35 CALL WRITELINE
// 36 ; writeln ( ’Sum of data = ’ , sum )
// 37 RET
// 38 ; end;
// 39 ; var
// 40 $data DC 0
// 41 ; data : integer;
// 42 $$s%goukei
// 43 DC 0
// 44 $$n%goukei
// 45 DC 0
// 46 ; procedure goukei ( n , s : integer );
// 47 ; var
// 48 $data%goukei
// 49 DC 0
// 50 ; data : integer;
// 51 $goukei
// 52 POP GR2
// 53 POP GR1
// 54 ST GR1,$$s%goukei
// 55 POP GR1
// 56 ST GR1,$$n%goukei
// 57 PUSH 0,GR2
// 58 ; begin
// 59 LD GR1,$$s%goukei
// 60 PUSH 0,GR1
// 61 LAD GR1,0
// 62 POP GR2
// 63 ST GR1,0,GR2
// 64 ;s := 0;
// 65 L0002
// 66 LD GR1,$$n%goukei
// 67 LD GR1,0,GR1
// 68 PUSH 0,GR1
// 69 LAD GR1,0
// 70 POP GR2
// 71 CPA GR2,GR1
// 72 JPL L0004
// 73 LAD GR1,0
// 74 JUMP L0005
// 75 L0004
// 76 LAD GR1,1
// 77 L0005
// 78 CPA GR1,GR0
// 79 JZE L0003
// 80 ; while n > 0 do
// 81 ; begin
// 82 LAD GR1,$data%goukei
// 83 CALL READINT
// 84 CALL READLINE
// 85 ; readln( data );
// 86 LD GR1,$$s%goukei
// 87 PUSH 0,GR1
// 88 LD GR1,$$s%goukei
// 89 LD GR1,0,GR1
// 90 PUSH 0,GR1
// 91 LD GR1,$data%goukei
// 92 POP GR2
// 93 ADDA GR1,GR2
// 94 JOV EOVF
// 95 POP GR2
// 96 ST GR1,0,GR2
// 97 ; s := s + data;
// 98 LD GR1,$$n%goukei
// 99 PUSH 0,GR1
// 100 LD GR1,$$n%goukei
// 101 LD GR1,0,GR1
// 102 PUSH 0,GR1
// 103 LAD GR1,1
// 104 POP GR2
// 105 SUBA GR2,GR1
// 106 JOV EOVF
// 107 LD GR1,GR2
// 108 POP GR2
// 109 ST GR1,0,GR2
// 110 ; n := n - 1
// 111 ; end
// 112 JUMP L0002
// 113 L0003
// 114 RET
// 115 ; end;
// 116 ; var
// 117 $n DC 0
// 118 ; n : integer;
// 119 L0001
// 120 LAD GR0,0
// 121 ; begin
// 122 LAD GR1,$n
// 123 PUSH 0,GR1
// 124 CALL $kazuyomikomi
// 125 ; call kazuyomikomi ( n );
// 126 LAD GR1,$n
// 127 LD GR1,0,GR1
// 128 PUSH 0,GR1
// 129 LAD GR1,2
// 130 POP GR2
// 131 MULA GR1,GR2
// 132 JOV EOVF
// 133 LAD GR2,=0
// 134 ST GR1,0,GR2
// 135 PUSH 0,GR2
// 136 LAD GR1,$sum
// 137 PUSH 0,GR1
// 138 CALL $goukei
// 139 ; call goukei ( n * 2 , sum );
// 140 CALL $wakakidasi
// 141 ; call wakakidasi
// 142 CALL FLUSH
// 143 RET
// 144 ; end.







// 以下は課題4で追加した変数や関数の定義

// breakで戻るべきラベル番号を記憶する変数
int break_label = -1;

// 最後に読んだ式でoprが1になったかどうか
int is_expression_opr = 0;

// 現在oprが続いているかどうか
int is_opr = 0;

// 定数のTRUEが出現したかどうか
int is_constant_true = 0;

// 定数のFALSEが出現したかどうか
int is_constant_false = 0;

// 左辺部かどうか
int is_left_part = 0;

// 現在の代入文の左辺部がパラメータでない変数であるかどうか
int is_vari_left_part = 0;

// 現在の左辺部の変数名
char left_variable[MAXSTRSIZE] = "";

// 現在の左辺部の変数名が配列型であるかどうか
int is_left_array = 0;

// 現在の配列の名前
char current_array_name[MAXSTRSIZE] = "";

// 現在の関係演算子
int current_relational_opr = 0;

// CASLに表示するコメントを記憶する文字列
char casl_comment[MAXSTRSIZE] = "; ";


FILE *caslfp = NULL; /* File pointer */

// 新しいラベル番号を返す
int get_label_num(void) {
    static int labelcounter = 1;
    return labelcounter++;
}

// ラベル番号を指定してラベルを生成する
void gen_label(int labelnum) {
    fprintf(caslfp, "L%04d\n", labelnum);
}

// コードを生成する(opc: オペコード, opr: オペランド)
void gen_code(char *opc, char *opr) {
    fprintf(caslfp, "\t%s\t%s\n", opc, opr);
}

// コードを生成する(opc: オペコード, label: ラベル番号)
void gen_code_label(char *opc, int labelnum) {
    fprintf(caslfp, "\t%s\tL%04d\n", opc, labelnum);
}

// コードを生成する(opc: オペコード, opr: オペランド, label: ラベル番号)
void gen_code_opr_label(char *opc, char *opr, int labelnum) {
    fprintf(caslfp, "\t%s\t%s,L%04d\n", opc, opr, labelnum);
}



// 以下は課題3で追加した変数や関数の定義

// 型の文字列
char *typestr[NUMOFTYPE + 1] = {"",
                                "integer",
                                "char",
                                "boolean",
                                "array of integer",
                                "array of char",
                                "array of boolean",
                                "procedure"};

// 変数宣言部のフラッグ(0: 変数宣言部でない, 1: 変数宣言部である)
int var_decl_flag = 0;

// 仮引数部のフラッグ(0: 仮引数部でない, 1: 仮引数部である)
int formal_params_flag = 0;

// 副プログラム宣言でその名前を格納する変数
char subpro_name[MAXSTRSIZE] = "";

// 副プログラム宣言部のフラッグ(0: 副プログラム宣言部でない, 1:副プログラム宣言部である)
int subpro_decl_flag = 0;

// 手続き呼び出し文のフラッグ(0: 手続き呼び出し文でない, 1:手続き呼び出し文である)
int call_stmt_flag = 0;

// 配列型のフラッグ(0: 配列型でない, 1: 配列型である)
int array_flag = 0;


// 変数表のための構造体の定義

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

// 変数表
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

// 大域変数表の初期化
void init_globalidtab() { /* Initialise the table */
    globalidroot = NULL;
}

// 副プログラム宣言内の変数表の初期化
void init_localidtab() { /* Initialise the table */
    localidroot = NULL;
}

// 大域変数表の検索
struct ID *search_globalidtab(char *np) {
    struct ID *p;

    for (p = globalidroot; p != NULL; p = p->nextp) {
        if (strcmp(np, p->name) == 0) return (p);
    }
    return (NULL);
}

// 副プログラム宣言内の変数表の検索
struct ID *search_localidtab(char *np) {
    struct ID *p;

    for (p = localidroot; p != NULL; p = p->nextp) {
        if (strcmp(np, p->name) == 0) return (p);
    }
    return (NULL);
}

// 大域変数表への変数の登録
int set_globalidtab(char *np, struct TYPE *tp, int ispara, int deflinenum) {
    struct ID *p;
    char *cp;

    if ((p = search_globalidtab(np)) != NULL) {
        // すでに登録されている
        char error_message[MAXSTRSIZE];
        sprintf(error_message, "Variable %s is already defined in line %d", np,
                p->deflinenum);
        return (error(error_message));
    } else {
        // 未登録
        if ((p = (struct ID *)malloc(sizeof(struct ID))) == NULL) {
            return (error("can not malloc in set_globalidtab"));
        }
        if ((cp = (char *)malloc(strlen(np) + 1)) == NULL) {
            return (error("can not malloc-2 in set_globalidtab"));
        }
        strcpy(cp, np);
        p->name = cp;
        p->itp = tp;
        p->ispara = ispara;
        p->deflinenum = deflinenum;
        p->irefp = NULL;

        // リストの末尾に追加する．もしリストが空ならば先頭に追加する
        struct ID *q;
        for (q = globalidroot; q != NULL; q = q->nextp) {
            if (q->nextp == NULL) {
                q->nextp = p;
                break;
            }
        }
        if (q == NULL) {
            globalidroot = p;
        }
    }
    return (NORMAL);
}

// 副プログラム宣言内の変数表への変数の登録
int set_localidtab(char *np, char *pnp, struct TYPE *tp, int ispara, int deflinenum) {
    struct ID *p;
    char *cp;

    if ((p = search_localidtab(np)) != NULL) {
        // すでに登録されている場合
        char error_message[MAXSTRSIZE];
        sprintf(error_message, "Variable %s is already defined2 in line %d", np,
                p->deflinenum);
        return (error(error_message));
    } else {
        // 未登録の場合
        if ((p = (struct ID *)malloc(sizeof(struct ID))) == NULL) {
            return (error("can not malloc in set_localidtab"));
        }
        if ((cp = (char *)malloc(strlen(np) + 1)) == NULL) {
            return (error("can not malloc-2 in set_localidtab"));
        }
        strcpy(cp, np);
        p->name = cp;
        p->procname = pnp;
        p->itp = tp;
        p->ispara = ispara;
        p->deflinenum = deflinenum;
        p->irefp = NULL;

        // リストの末尾に追加する．もしリストが空ならば先頭に追加する
        struct ID *q;
        for (q = localidroot; q != NULL; q = q->nextp) {
            if (q->nextp == NULL) {
                q->nextp = p;
                break;
            }
        }
        if (q == NULL) {
            localidroot = p;
        }
    }
    return (NORMAL);
}

// 大域変数表の出力
void print_globalidtab() {
    struct ID *p;
    for (p = globalidroot; p != NULL; p = p->nextp) {
        // 参照行をカンマ区切りでtmprefstringに格納
        char tmprefstring[MAXSTRSIZE] = "";
        struct LINE *r;
        for (r = p->irefp; r != NULL; r = r->nextlinep) {
            if (r->nextlinep == NULL) {
                char tmpstring[MAXSTRSIZE];
                sprintf(tmpstring, "%d", r->reflinenum);
                strcat(tmprefstring, tmpstring);
            } else {
                char tmpstring[MAXSTRSIZE];
                sprintf(tmpstring, "%d,", r->reflinenum);
                strcat(tmprefstring, tmpstring);
            }
        }

        if (p->itp->ttype == TPPROC) {
            char paratpstr[MAXSTRSIZE] = "";
            if (p->itp->paratp != NULL) {
                strcat(paratpstr, "(");
                struct TYPE *q;
                for (q = p->itp->paratp; q != NULL; q = q->paratp) {
                    if (q->paratp == NULL) {
                        strcat(paratpstr, typestr[q->ttype]);
                    } else {
                        char tmpstring[MAXSTRSIZE];
                        sprintf(tmpstring, "%s,", typestr[q->ttype]);
                        strcat(paratpstr, tmpstring);
                    }
                }
                strcat(paratpstr, ")");
            }
            char tmpstring[MAXSTRSIZE] = "procedure";
            strcat(tmpstring, paratpstr);
            // printf("%-15s|%-30s|%-15d|%-10s\n", p->name, tmpstring,
            //        p->deflinenum, tmprefstring);
        } else if (p->itp->ttype == TPARRAYINT ||
                   p->itp->ttype == TPARRAYCHAR ||
                   p->itp->ttype == TPARRAYBOOL) {
            char tmpstring[MAXSTRSIZE];
            sprintf(tmpstring, "array[%d] of %s", p->itp->arraysize,
                    typestr[p->itp->etp->ttype]);
            // printf("%-15s|%-30s|%-15d|%-10s\n", p->name, tmpstring,
            //        p->deflinenum, tmprefstring);
        } else {
            // printf("%-15s|%-30s|%-15d|%-10s\n", p->name, typestr[p->itp->ttype],
            //        p->deflinenum, tmprefstring);
        }
    }
}

// 副プログラム宣言内の変数表の出力
void print_localidtab() {
    struct ID *p;

    for (p = localidroot; p != NULL; p = p->nextp) {
        // 参照行をカンマ区切りでtmprefstringに格納
        char tmprefstring[MAXSTRSIZE] = "";
        struct LINE *r;
        for (r = p->irefp; r != NULL; r = r->nextlinep) {
            if (r->nextlinep == NULL) {
                char tmpstring[MAXSTRSIZE];
                sprintf(tmpstring, "%d", r->reflinenum);
                strcat(tmprefstring, tmpstring);
            } else {
                char tmpstring[MAXSTRSIZE];
                sprintf(tmpstring, "%d,", r->reflinenum);
                strcat(tmprefstring, tmpstring);
            }
        }
        if (p->itp == NULL) {
            // printf("%-15s|NULL                          |%-15d|%-10s\n",
            //        p->name, p->deflinenum, tmprefstring);
        } else {
            // printf("%s:%-13s|%-30s|%-15d|%-10s\n", p->name, p->procname, typestr[p->itp->ttype],p->deflinenum, tmprefstring);
        }
    }
}

// 副プログラム宣言内の変数表の解放
void free_localidtab() {
    // printf("free_localidtab\n");
    struct ID *p, *q;
    for (p = localidroot; p != NULL; p = q) {
        q = p->nextp;
        free(p);
    }
}

// 大域変数表の解放
void free_globalidtab() {
    // printf("free_globalidtab\n");
    struct ID *p, *q;
    for (p = globalidroot; p != NULL; p = q) {
        q = p->nextp;
        free(p);
    }
}


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
        if (ENABLE_PRETTY_PRINT) printf("    ");
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
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf("program"); strcat(casl_comment, "program");
    if (token != TNAME) return (error("Program name is not found"));
    token = scan();

    // プログラム名のラベルを生成してSTART
    int labelnum = get_label_num();
    fprintf(caslfp, "%%%%%s\tSTART\tL%04d\n", string_attr, labelnum);

    if (ENABLE_PRETTY_PRINT) printf(" %s", string_attr); strcat(casl_comment, " "); strcat(casl_comment, string_attr);
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(";\n"); strcat(casl_comment, ";");

    // caslfpにcasl_commentを出力してcasl_commentを初期化
    if (ENABLE_PRETTY_PRINT_IN_CASL) fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");

    if (parse_block() == ERROR) return (ERROR);
    if (token != TDOT)
        return (error("Period is not found at the end of program"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(".\n");

    outlib(caslfp);
    gen_code("END", "");

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
    gen_label(1);
    gen_code("LAD", "GR0,0");
    if (parse_comp_stmt() == ERROR) return (ERROR);
    gen_code("CALL", "FLUSH");
    gen_code("RET", "");
    return (NORMAL);
}

// 変数宣言部の構文解析関数
int parse_var_decl() {
    var_decl_flag = 1;
    // printf("DEBUG: var_decl");

    if (token != TVAR) return (error("Keyword 'var' is not found"));
    token = scan();
    indent_print(1);
    if (ENABLE_PRETTY_PRINT) printf("var\n");
    strcat(casl_comment, "var");
    fprintf(caslfp, "%s\n", casl_comment);
    // printf("DEBUG: 661\n");
    strcpy(casl_comment, "; ");
    // printf("DEBUG: 663\n");

    // indent_print(2);
    // printf("DEBUG: parse_var_names\n");
    if (parse_var_names() == ERROR) return (ERROR);
    // printf("DEBUG: parse_var_names end\n");
    if (token != TCOLON) return (error("Colon is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(" : "); strcat(casl_comment, " : ");
    if (parse_type() == ERROR) return (ERROR);
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(";\n"); strcat(casl_comment, ";");
    while (token == TNAME) {
        indent_print(2);
        if (parse_var_names() == ERROR) return (ERROR);
        if (token != TCOLON) return (error("Colon is not found"));
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" : "); strcat(casl_comment, " : ");
        if (parse_type() == ERROR) return (ERROR);
        if (token != TSEMI) return (error("Semicolon is not found"));
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(";\n"); strcat(casl_comment, ";");
    }
    indent = 0;

    if (ENABLE_PRETTY_PRINT_IN_CASL == 1 && strcmp(subpro_name, "") == 0) {
        fprintf(caslfp, "%s\n", casl_comment);
        strcpy(casl_comment, "; ");
    }

    var_decl_flag = 0;
    return (NORMAL);
}

// 変数名の並びの構文解析関数
int parse_var_names() {
    if (parse_var_name() == ERROR) return (ERROR);
    while (token == TCOMMA) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" , "); strcat(casl_comment, " , ");
        if (parse_var_name() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

// 変数名の構文解析関数
int parse_var_name() {
    // printf("DEBUG: parse_var_name\n");
    if (token != TNAME) return (error("Variable name is not found"));
    if (var_decl_flag == 1) {  // 変数宣言部である場合
        // printf("DEBUG: parse_var_name var_decl_flag == 1\n");
        if (strcmp(subpro_name, "") == 0) {
            // printf("DEBUG: parse_var_name var_decl_flag == 1 if\n");
            if (set_globalidtab(string_attr, NULL, 0, get_linenum()) == ERROR)
                return (ERROR);
            // printf("DEBUG: parse_var_name var_decl_flag == 1 if end\n");
            
            // いま追加した変数の宣言をcaslfpに書き込む
            fprintf(caslfp, "$%s\tDC\t0\n", string_attr);
        } else {
            // printf("DEBUG: parse_var_name var_decl_flag == 1 else\n");
            // char tmpstring[MAXSTRSIZE];
            // sprintf(tmpstring, "%s:%s", string_attr, subpro_name);
            if (set_localidtab(string_attr, subpro_name, NULL, 0, get_linenum()) == ERROR)
                return (ERROR);
        }
        // printf("DEBUG: parse_var_name end\n");
    } else if (formal_params_flag == 1) {  // 仮引数部である場合
        // char tmpstring[MAXSTRSIZE];
        // sprintf(tmpstring, "%s:%s", string_attr, subpro_name);
        if (set_localidtab(string_attr, subpro_name, NULL, 1, get_linenum()) == ERROR)
            return (ERROR);
    } else {
        // それ以外の場所で参照されている場合

        struct ID *p;
        char tmpstring[MAXSTRSIZE];

        // 大域部分で参照されている場合は，大域変数表だけを検索
        if (strcmp(subpro_name, "") == 0) {
            if ((p = search_globalidtab(string_attr)) == NULL) {
                return (error("Variable is not defined1"));
            }

            // 副プログラム宣言内で参照されている場合は，副プログラム宣言内の変数表を検索し，無ければ大域変数表を検索
        } else {
            // sprintf(tmpstring, "%s:%s", string_attr, subpro_name);
            if ((p = search_localidtab(string_attr)) == NULL) {
                if ((p = search_globalidtab(string_attr)) == NULL) {
                    return (error("Variable is not defined2"));
                } else {
                    char tmpstring2[MAXSTRSIZE];
                    sprintf(tmpstring2, "GR1,$%s", p->name); // <<<<<<<<
                    gen_code("LD", tmpstring2);
                }
            } else {
                char tmpstring2[MAXSTRSIZE];
                // 仮引数で参照されている場合
                if (p->ispara == 1) {
                    sprintf(tmpstring2, "GR1,$$%s%%%s", p->name, p->procname);
                    gen_code("LD", tmpstring2);
                } 
                // 仮引数以外で参照されている場合
                else {
                    sprintf(tmpstring2, "GR1,$%s%%%s", p->name, p->procname);
                    gen_code("LAD", tmpstring2); //FIXME:　LDに変更したほうがいいかも
                }
            
            }
        }
        // 参照行番号を記録するための構造体を確保
        struct LINE *q;
        if ((q = (struct LINE *)malloc(sizeof(struct LINE))) == NULL) {
            return (error("can not malloc in parse_var_name"));
        }
        q->reflinenum = get_linenum();
        q->nextlinep = NULL;

        struct LINE *r;
        for (r = p->irefp; r != NULL; r = r->nextlinep) {
            if (r->nextlinep == NULL) {
                r->nextlinep = q;
                break;
            }
        }
        if (r == NULL) {
            p->irefp = q;
        }
    }
    if (ENABLE_PRETTY_PRINT) printf("%s", string_attr); 
    // if (formal_params_flag == 1 || var_decl_flag == 1) {
        strcat(casl_comment, string_attr);
    // }
    token = scan();
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

        if (array_flag == 1) {
            // ここでは何もしない
        } else if (var_decl_flag == 1) {
            struct TYPE *tp;
            if ((tp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
                return (error("can not malloc in parse_standard_type"));
            }
            switch (token) {
                case TINTEGER:
                    tp->ttype = TPINT;
                    break;
                case TBOOLEAN:
                    tp->ttype = TPBOOL;
                    break;
                case TCHAR:
                    tp->ttype = TPCHAR;
                    break;
            }
            tp->arraysize = 0;
            tp->etp = NULL;
            tp->paratp = NULL;

            // 変数表を検索して，itpがNULLの要素に型を登録する
            if (strcmp(subpro_name, "") ==
                0) {  // 副プログラムじゃなければ大域変数表を検索
                struct ID *p;
                for (p = globalidroot; p != NULL; p = p->nextp) {
                    if (p->itp == NULL) {
                        p->itp = tp;
                    }
                }
            } else {  // 副プログラム宣言内の変数表を検索
                struct ID *p;
                for (p = localidroot; p != NULL; p = p->nextp) {
                    if (p->itp == NULL) {
                        p->itp = tp;
                    }
                }
            }
        } else if (formal_params_flag == 1) {
            struct TYPE *tp;
            if ((tp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
                return (error("can not malloc in parse_standard_type"));
            }
            switch (token) {
                case TINTEGER:
                    tp->ttype = TPINT;
                    break;
                case TBOOLEAN:
                    tp->ttype = TPBOOL;
                    break;
                case TCHAR:
                    tp->ttype = TPCHAR;
                    break;
            }
            tp->arraysize = 0;
            tp->etp = NULL;
            tp->paratp = NULL;

            // ローカル変数表を検索して，itpがNULLの要素に型を登録する
            struct ID *p;

            // 副プログラム宣言内の仮引数の個数を数える変数
            int formal_params_count = 0;

            for (p = localidroot; p != NULL; p = p->nextp) {
                if (p->itp == NULL) {
                    p->itp = tp;
                    formal_params_count++;
                }
            }

            // globalidtabで，itpがTPPROCで，nameがsubpro_nameのものを検索
            p = search_globalidtab(subpro_name);

            if (formal_params_count == 1) {
                p->itp->paratp = tp;
            } else if (formal_params_count > 1) {
                for (int i = 0; i < formal_params_count; i++) {
                    struct TYPE *q;
                    if ((q = (struct TYPE *)malloc(sizeof(struct TYPE))) ==
                        NULL) {
                        return (error("can not malloc in parse_standard_type"));
                    }
                    q->ttype = tp->ttype;
                    q->arraysize = tp->arraysize;
                    q->etp = tp->etp;
                    q->paratp = NULL;

                    if (i == 0) {
                        p->itp->paratp = q;
                    } else {
                        struct TYPE *r;
                        for (r = p->itp->paratp; r != NULL; r = r->paratp) {
                            if (r->paratp == NULL) {
                                r->paratp = q;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (ENABLE_PRETTY_PRINT) printf("%s", tokenstr[token]); 
        if (formal_params_flag == 1 || subpro_decl_flag == 1 || var_decl_flag == 1) strcat(casl_comment, tokenstr[token]);
        token = scan();
        int normal = 0;  // Declare the identifier normal
        return (NORMAL);
    } else {
        return (error("Standard type is not found"));
    }
}

// 配列型の構文解析関数
int parse_array_type() {
    array_flag = 1;

    struct TYPE *tp;
    if ((tp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
        return (error("can not malloc in parse_array_type"));
    }

    if (token != TARRAY) return (error("Keyword 'array' is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf("array"); strcat(casl_comment, "array");
    if (token != TLSQPAREN)
        return (error("Left square parenthesis is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(" [ "); strcat(casl_comment, " [ ");
    if (token != TNUMBER) return (error("Number is not found"));
    token = scan();

    // 配列型のサイズを記録
    tp->arraysize = num_attr;

    if (ENABLE_PRETTY_PRINT) printf("%d", num_attr);
    char tmpstring[MAXSTRSIZE];
    sprintf(tmpstring, "%d", num_attr);
    strcat(casl_comment, tmpstring);;
    if (token != TRSQPAREN)
        return (error("Right square parenthesis is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(" ] "); strcat(casl_comment, " ] ");
    if (token != TOF) return (error("Keyword 'of' is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf("of "); strcat(casl_comment, "of ");

    // 配列型の要素の型を記録
    struct TYPE *etp;
    if ((etp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
        return (error("can not malloc in parse_array_type"));
    }
    switch (token) {
        case TINTEGER:
            tp->ttype = TPARRAYINT;
            etp->ttype = TPINT;
            break;
        case TBOOLEAN:
            tp->ttype = TPARRAYBOOL;
            etp->ttype = TPBOOL;
            break;
        case TCHAR:
            tp->ttype = TPARRAYCHAR;
            etp->ttype = TPCHAR;
            break;
    }

    etp->arraysize = 0;
    etp->etp = NULL;
    etp->paratp = NULL;

    tp->etp = etp;
    tp->paratp = NULL;

    // 変数表を検索して，itpがNULLの要素に型を登録する
    if (strcmp(subpro_name, "") == 0) {
        struct ID *p;
        for (p = globalidroot; p != NULL; p = p->nextp) {
            if (p->itp == NULL) {
                p->itp = tp;
            }
        }
    } else {
        struct ID *p;
        for (p = localidroot; p != NULL; p = p->nextp) {
            if (p->itp == NULL) {
                p->itp = tp;
            }
        }
    }

    if (parse_standard_type() == ERROR) return (ERROR);

    array_flag = 0;
    return (NORMAL);
}

// 部分プログラム宣言部の構文解析関数

int parse_subpro_decl() {
    subpro_decl_flag = 1;

    indent_print(1);
    if (token != TPROCEDURE) return (error("Keyword 'procedure' is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf("procedure "); strcat(casl_comment, "procedure ");
    if (parse_procedure_name() == ERROR) return (ERROR);

    // strcat(casl_comment, subpro_name);

    if (token == TLPAREN) {
        if (parse_formal_params() == ERROR) return (ERROR);
    } else {
        struct TYPE *tp;
        if ((tp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
            return (error("can not malloc in parse_subpro_decl"));
        }
        tp->ttype = TPPROC;
        tp->arraysize = 0;
        tp->etp = NULL;
        tp->paratp = NULL;

        // 大域変数表を検索して，itpがNULLの要素に型を登録する
        struct ID *p;
        for (p = globalidroot; p != NULL; p = p->nextp) {
            if (p->itp == NULL) {
                p->itp = tp;
            }
        }
    }
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(";\n"); strcat(casl_comment, ";");

    // 仮引数部の変数リストを末尾から遡って，procnameがsubpro_nameのものそれぞれについて，
    // $$n%kazuyomikomi
    //      DC    0
    // みたいにcaslfpに書き込む
    struct ID *p;
    for (p = localidroot; p != NULL; p = p->nextp) {
        if (strcmp(p->procname, subpro_name) == 0) {
            fprintf(caslfp, "$$%s%%%s\n", p->name, p->procname);
            gen_code("DC", "0");
        }
    }

    if (ENABLE_PRETTY_PRINT_IN_CASL) fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");

    if (token == TVAR) {
        if (parse_var_decl() == ERROR) return (ERROR);

        // 副プログラム宣言内の変数表から，procnameがsubpro_nameかつ，isparaが0のものを
        // $data%goukei
        //      DC    0
        // みたいにcaslfpに書き込む
        struct ID *p;
        for (p = localidroot; p != NULL; p = p->nextp) {
            if (strcmp(p->procname, subpro_name) == 0 && p->ispara == 0) {
                fprintf(caslfp, "$%s%%%s\n", p->name, p->procname);
                gen_code("DC", "0");
            }
        }
        if (ENABLE_PRETTY_PRINT_IN_CASL) fprintf(caslfp, "%s\n", casl_comment);
        strcpy(casl_comment, "; ");
    }
    
    // 副プログラム宣言内でないことを示す
    subpro_decl_flag = 0;

    fprintf(caslfp, "$%s\n", subpro_name);

    // 仮引数が1個以上ある場合は，POP GR2を書き込む
    int formal_params_count = 0;
    struct ID *q;
    for (q = localidroot; q != NULL; q = q->nextp) {
        if (strcmp(q->procname, subpro_name) == 0 && q->ispara == 1) {
            formal_params_count++;
        }
    }
    if (formal_params_count > 0) {
        gen_code("POP", "GR2");
    }

    // 副プログラム宣言内の変数表を確認して，procnameがsubpro_nameかつisparaが1のものについて
    // POP GR1
    // ST GR1,$$s%goukei
    // みたいにcaslfpに書き込む
    struct ID *r;
    for (r = localidroot; r != NULL; r = r->nextp) {
        if (strcmp(r->procname, subpro_name) == 0 && r->ispara == 1) {
            gen_code("POP", "GR1");
            fprintf(caslfp, "\tST\tGR1,$$%s%%%s\n", r->name, r->procname);
        }
    }

    if(formal_params_count > 0) {
        gen_code("PUSH", "0,GR2");
    }

    indent = 1;
    if (parse_comp_stmt() == ERROR) return (ERROR);
    if (token != TSEMI) return (error("Semicolon is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(";\n");
    indent = 0;

    // 副プログラム宣言内の変数表を出力
    print_localidtab();

    // 副プログラム宣言内の変数表を解放
    free_localidtab();

    // 副プログラム宣言内の変数表を初期化
    init_localidtab();
    gen_code("RET", "");

    strcpy(subpro_name, "");
    return (NORMAL);
}

// 手続き名の構文解析関数
int parse_procedure_name() {
    if (token != TNAME) return (error("Procedure name is not found"));
    char tmpstring[MAXSTRSIZE];

    if (subpro_decl_flag == 1) {
        strcpy(subpro_name, string_attr);
    }

    strcpy(tmpstring, string_attr);

    if (strcmp(tmpstring, "") != 0 && subpro_decl_flag == 1 &&
        call_stmt_flag == 0) {
        struct TYPE *tp;
        if ((tp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) {
            return (error("can not malloc in parse_procedure_name"));
        }
        tp->ttype = TPPROC;
        tp->arraysize = 0;
        tp->etp = NULL;
        tp->paratp = NULL;

        if (set_globalidtab(string_attr, tp, 0, get_linenum()) == ERROR)
            return (ERROR);
    } else if (strcmp(tmpstring, "") != 0 && call_stmt_flag == 1) {
        // 手続き名がcall文で呼び出された場合は，手続き名で参照されたことを記録する
        struct ID *p;
        if ((p = search_globalidtab(tmpstring)) == NULL) {
            return (error("Procedure name is not defined"));
        }

        // 参照行番号を記録するための構造体を確保
        struct LINE *q;
        if ((q = (struct LINE *)malloc(sizeof(struct LINE))) == NULL) {
            return (error("can not malloc in parse_procedure_name"));
        }
        q->reflinenum = get_linenum();
        q->nextlinep = NULL;

        // 参照を記録する
        struct LINE *r;
        for (r = p->irefp; r != NULL; r = r->nextlinep) {
            if (r->nextlinep == NULL) {
                r->nextlinep = q;
                break;
            }
        }
        if (r == NULL) {
            p->irefp = q;
        }
    }
    if (ENABLE_PRETTY_PRINT) printf("%s", string_attr); strcat(casl_comment, string_attr);
    token = scan();
    return (NORMAL);
}

// 仮引数部の構文解析関数
int parse_formal_params() {
    formal_params_flag = 1;
    if (token != TLPAREN) return (error("Left parenthesis is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(" ( "); strcat(casl_comment, " ( ");
    if (parse_var_names() == ERROR) return (ERROR);
    if (token != TCOLON) return (error("Colon is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(" : "); strcat(casl_comment, " : ");
    if (parse_type() == ERROR) return (ERROR);
    while (token == TSEMI) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf("; "); strcat(casl_comment, "; ");
        if (parse_var_names() == ERROR) return (ERROR);
        if (token != TCOLON) return (error("Colon is not found"));
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" : "); strcat(casl_comment, " : ");
        if (parse_type() == ERROR) return (ERROR);
    }
    if (token != TRPAREN) return (error("Right parenthesis is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(" )"); strcat(casl_comment, " )");
    formal_params_flag = 0;
    return (NORMAL);
}

// 複合文の構文解析関数
int parse_comp_stmt() {
    if (token != TBEGIN) return (error("Keyword 'begin' is not found"));
    token = scan();
    int original_indent = indent;
    indent_print(indent);

    if (ENABLE_PRETTY_PRINT) printf("begin\n");
    strcat(casl_comment, "begin");
    if (ENABLE_PRETTY_PRINT_IN_CASL) fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");

    indent++;
    if (parse_stmt() == ERROR) return (ERROR);
    while (token == TSEMI) {
        if (empty_stmt_flag == 1) {
            indent_print(indent);
            empty_stmt_flag = 0;
        }
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(";\n");
        indent = original_indent + 1;
        if (parse_stmt() == ERROR) return (ERROR);
    }
    if (token != TEND) return (error("Keyword 'end' is not found"));
    if (empty_stmt_flag == 1) {
        empty_stmt_flag = 0;
    } else {
        if (ENABLE_PRETTY_PRINT) printf("\n");
    }
    indent_print(original_indent);
    token = scan();
    if (strcmp(subpro_name, "") != 0) {
        // gen_code("RET", "");
    }
    if (ENABLE_PRETTY_PRINT) printf("end");
    strcat(casl_comment, "end");
    if (ENABLE_PRETTY_PRINT_IN_CASL) fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");
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
    int label1, label2;
    indent_print(indent);
    if (token != TIF) return (error("Keyword 'if' is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf("if "); strcat(casl_comment, "if ");
    if_indent = indent;
    if (parse_expression() == ERROR) return (ERROR);
    // parse_expression()が条件式のコードを生成していると仮定できるので
    // 新たなラベルL0001を確保して
    label1 = get_label_num();
    // CPA     GR1,GR0
    gen_code("CPA", "GR1,GR0");
    // JZE     L0001
    gen_code_label("JZE", label1);
    // を生成する

    if (token != TTHEN) return (error("Keyword 'then' is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(" then\n"); strcat(casl_comment, " then");
    indent++;
    if (parse_stmt() == ERROR) return (ERROR);
    // parse_stmt()がthen節のコードを生成していると仮定できるので
    if (token == TELSE) {
        // 新たなラベルL0002を確保して
        label2 = get_label_num();
        // JUMP    L0002
        gen_code_label("JUMP", label2);
        // L0001
        gen_label(label1);
        // を生成する

        if (ENABLE_PRETTY_PRINT) printf("\n"); strcat(casl_comment, " ");
        token = scan();
        indent_print(if_indent);
        if (ENABLE_PRETTY_PRINT) printf("else\n"); strcat(casl_comment, "else");
        indent = if_indent + 1;
        if (parse_stmt() == ERROR) return (ERROR);
        // parse_stmt()がelse節のコードを生成していると仮定できるので
        // L0002
        gen_label(label2);
        // を生成する
    } else {
        // L0001
        gen_label(label1);
    }

    fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");
    return (NORMAL);
}

// 繰り返し文の構文解析関数
int parse_iter_stmt() {
    int roop_label;
    roop_label = get_label_num();
    gen_label(roop_label);

    while_flag = 1;
    indent_print(indent);
    if (token != TWHILE) return (error("Keyword 'while' is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf("while "); strcat(casl_comment, "while ");

    // parse_expression()が条件式のコードを生成していると仮定できるので
    // 新たなラベルL0003を確保して
    int label1 = get_label_num();
    break_label = label1;
    if (parse_expression() == ERROR) return (ERROR);
    // CPA     GR1,GR0
    gen_code("CPA", "GR1,GR0");
    // JZE     L0003
    gen_code_label("JZE", label1);


    if (token != TDO) return (error("Keyword 'do' is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(" do\n"); strcat(casl_comment, " do");

    fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");
    indent++;
    if (parse_stmt() == ERROR) return (ERROR);

    gen_code_label("JUMP", roop_label);
    gen_label(label1);

    indent--;
    indent--;
    // while_flag = 0;
    fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");
    return (NORMAL);
}

// 脱出文の構文解析関数
int parse_exit_stmt() {
    indent_print(indent);
    if (token != TBREAK) return (error("Keyword 'break' is not found"));

    gen_code_label("JUMP", break_label);

    token = scan();
    if (ENABLE_PRETTY_PRINT) printf("break");
    // indent--;
    if (while_flag == 0) {
        return (error("Break statement is not in while statement"));
    }
    return (NORMAL);
}

// 手続き呼び出し文の構文解析関数
int parse_call_stmt() {
    call_stmt_flag = 1;
    indent_print(indent);
    if (token != TCALL) return (error("Keyword 'call' is not found"));
    strcat(casl_comment, "call ");
    token = scan();
    char tmp_procname[MAXSTRSIZE];
    strcpy(tmp_procname, string_attr);
    if (parse_procedure_name() == ERROR) return (ERROR);
    if (token == TLPAREN) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" ( "); strcat(casl_comment, " ( ");
        if (parse_expressions() == ERROR) return (ERROR);
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" )"); strcat(casl_comment, " ) ");
    }
    indent--;

    // 大域変数表を検索して，procnameがtmp_procnameのものを探す
    struct ID *p;
    if ((p = search_globalidtab(tmp_procname)) == NULL) {
        return (error("Procedure name is not defined"));
    }
    fprintf(caslfp, "\tCALL\t$%s\n", tmp_procname);

    fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");
    call_stmt_flag = 0;

    return (NORMAL);
}

// 式の並びの構文解析関数
int parse_expressions() {
    if (parse_expression() == ERROR) return (ERROR);


    if (is_expression_opr== 1) {
        // int tmplabel = get_label_num();
        // gen_code_opr_label("LAD", "GR2", tmplabel);
        gen_code("LAD","GR2,=0"); //TODO: ここは要検討


        gen_code("ST", "GR1,0,GR2");
        gen_code("PUSH", "0,GR2");
        // TODO: mayuleo645のadd_DCListの記述を追加
    } else {
        gen_code("PUSH", "0,GR1");
    }


    while (token == TCOMMA) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" , "); strcat(casl_comment, " , ");
        if (parse_expression() == ERROR) return (ERROR);
    }
    return (NORMAL);
}

// 戻り文の構文解析関数
int parse_return_stmt() {
    indent_print(indent);
    gen_code("RET", "");
    if (token != TRETURN) return (error("Keyword 'return' is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf("return");
    indent--;
    return (NORMAL);
}

// 代入文の構文解析関数
int parse_assign_stmt() {
    indent_print(indent);
    if (parse_left_part() == ERROR) return (ERROR);
    if (token != TASSIGN) return (error("Assignment operator is not found"));
    token = scan();
    if (ENABLE_PRETTY_PRINT) printf(" := "); strcat(casl_comment, " := ");
    if (parse_expression() == ERROR) return (ERROR);

    if (is_vari_left_part == 0) {
        gen_code("POP", "GR2");
    }
    // fprintf(caslfp, ">>>>>>>>>>DEBUG POINT 0\n");
    // fprintf(caslfp, "is_vari_left_part = %d\n", is_vari_left_part);
    // fprintf(caslfp, "is_left_part = %d\n", is_left_part);
    if (is_vari_left_part == 1) {
        struct ID *i;
        char tmpstring[MAXSTRSIZE];

        if ((i = search_localidtab(left_variable)) != NULL) {
            sprintf(tmpstring, "$%s%%%s", i->name, i->procname);
            // fprintf(caslfp, ">>>>>>>>>>DEBUG POINT 1\n");
            // 左辺部が配列の場合
            if (is_left_array == 1) {
                char tmpstring2[MAXSTRSIZE];
                sprintf(tmpstring2, "GR1,%s,GR2", tmpstring);
                gen_code("ST", tmpstring2);
            } else {
                char tmpstring2[MAXSTRSIZE];
                sprintf(tmpstring2, "GR1,%s", tmpstring);
                gen_code("ST", tmpstring2);
            }
        
        } else if ((i = search_globalidtab(left_variable)) != NULL) {
            sprintf(tmpstring, "$%s", i->name);
            // 左辺部が配列の場合
            if (is_left_array == 1) {
                char tmpstring2[MAXSTRSIZE];
                sprintf(tmpstring2, "GR1,%s,GR2", tmpstring);
                gen_code("ST", tmpstring2);
            } else {
                char tmpstring2[MAXSTRSIZE];
                sprintf(tmpstring2, "GR1,%s", tmpstring);
                gen_code("ST", tmpstring2);
            }
        }
    } else if (is_left_array) {
        fprintf(caslfp, "\tST\tGR1,%s,GR2\n", current_array_name);
    } else {
        gen_code("ST", "GR1,0,GR2");
    }
    is_left_array = 0;
    is_vari_left_part = 0;

    if (ENABLE_PRETTY_PRINT_IN_CASL) fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");
    indent--;
    return (NORMAL);
}

// 左辺部の構文解析関数
int parse_left_part() {
    is_left_part = 1;
    if (parse_variable() == ERROR) return (ERROR);

    char tmpstring[MAXSTRSIZE];
    struct ID *i;
    if ((i = search_localidtab(left_variable)) != NULL) {
        if(i->ispara == 1) {
            // sprintf(tmpstring, "GR1,$%s%%%s<<<<", i->name, i->procname);// TODO:復活するかも
            // gen_code("LD", tmpstring);
            gen_code("PUSH", "0,GR1");
            // is_vari_left_part = 1;
        } else {
            is_vari_left_part = 1;
        }
    } else if ((i = search_globalidtab(left_variable)) != NULL) {
        is_vari_left_part = 1;
    }
    is_left_part = 0;
    return (NORMAL);
}

// 変数の構文解析関数
int parse_variable() {
    char variable_name[MAXSTRSIZE];
    sprintf(variable_name, "%s", string_attr);
    if (parse_var_name() == ERROR) return (ERROR);

    if (is_left_part == 1) {
        sprintf(left_variable, "%s", string_attr);
    }

    if (token == TLSQPAREN) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" [ ");
        if (parse_expression() == ERROR) return (ERROR);
        if (token != TRSQPAREN)
            return (error("Right square parenthesis is not found"));
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" ]");

        strcpy(current_array_name, variable_name);
        if(is_left_part == 1) {
            is_left_array = 1;
        }
        if(var_decl_flag == 0) {
            char tmpstring[MAXSTRSIZE];
            struct ID *i;
            if ((i = search_localidtab(variable_name)) != NULL) {
                sprintf(tmpstring, "GR2,%d", i->itp->arraysize);
                gen_code("LAD", tmpstring);
            } else if ((i = search_globalidtab(variable_name)) != NULL) {
                sprintf(tmpstring, "GR2,%d", i->itp->arraysize);
                gen_code("LAD", tmpstring);
            }

            gen_code("CPA", "GR2,GR1");
            gen_code("JMI", "EROV");
            gen_code("JZE", "EROV");
            gen_code("CPA", "GR1,GR0");
            gen_code("JMI", "EROV");
        }
    }
    return (NORMAL);
}

// 式の構文解析関数
int parse_expression() {
    int opr;
    is_expression_opr = 0;
    is_opr = 0;
    if (parse_simple_expression() == ERROR) return (ERROR);
    while (token == TEQUAL || token == TNOTEQ || token == TLE ||
           token == TLEEQ || token == TGR || token == TGREQ) {
        is_opr = 1;
        is_expression_opr = 1;
        opr = token;
        
        gen_code("PUSH", "0,GR1");


        if (ENABLE_PRETTY_PRINT) printf(" "); strcat(casl_comment, " ");
        if (parse_relational_op() == ERROR) return (ERROR);
        if (ENABLE_PRETTY_PRINT) printf(" "); strcat(casl_comment, " ");
        if (parse_simple_expression() == ERROR) return (ERROR);
        is_opr = 0;

        //TODO:ここに関係演算子の処理をかく
        gen_code("POP", "GR2");
        gen_code("CPA", "GR2,GR1");
        int label1 = get_label_num();
        int label2 = get_label_num();

        switch(current_relational_opr) {
            case TEQUAL:
                gen_code_label("JZE", label1);
                gen_code("LD","GR1,GR0");
                gen_code_label("JUMP", label2);
                gen_label(label1);
                gen_code("LAD", "GR1,1");
                gen_label(label2);
                break;
            case TNOTEQ:
                gen_code_label("JNZ", label1);
                gen_code("LD","GR1,GR0");
                gen_code_label("JUMP", label2);
                gen_label(label1);
                gen_code("LAD", "GR1,1");
                gen_label(label2);
                break;
            case TLE:
                gen_code_label("JMI", label1);
                gen_code("LD","GR1,GR0");
                gen_code_label("JUMP", label2);
                gen_label(label1);
                gen_code("LAD", "GR1,1");
                gen_label(label2);
                break;
            case TLEEQ:
                gen_code_label("JPL", label1);
                gen_code("LAD","GR1,1");
                gen_code_label("JUMP", label2);
                gen_label(label1);
                gen_code("LD", "GR1,GR0");
                gen_label(label2);
                break;
            case TGR:
                gen_code_label("JPL", label1);
                gen_code("LD","GR1,GR0");
                gen_code_label("JUMP", label2);
                gen_label(label1);
                gen_code("LAD", "GR1,1");
                gen_label(label2);
                break;
            case TGREQ:
                gen_code_label("JMI", label1);
                gen_code("LAD","GR1,1");
                gen_code_label("JUMP", label2);
                gen_label(label1);
                gen_code("LD", "GR1,GR0");
                gen_label(label2);
                break;
            default:
                error("Relational expression error");
            break;
        }

    }
    return (NORMAL);
}

// 単純式の構文解析関数
int parse_simple_expression() {
    // printf(">>>>>>parse_simple_expression\n");
    int opr;
    int is_minus = 0;
    if (token == TPLUS || token == TMINUS) {
        is_minus = 1;
        if (ENABLE_PRETTY_PRINT) printf("%s ", tokenstr[token]); strcat(casl_comment, tokenstr[token]);
        token = scan();
    }
    if (parse_term() == ERROR) return (ERROR);

    if (is_minus == 1) {
        gen_code("LAD", "GR2,-1");
        gen_code("MULA", "GR1,GR2");
        gen_code("JOV", "EOVF");
    }

    while (token == TPLUS || token == TMINUS || token == TOR) {

        is_expression_opr = 1;
        is_opr = 1;
        opr = token;
        gen_code("PUSH", "0,GR1");


        if (ENABLE_PRETTY_PRINT) printf(" "); strcat(casl_comment, " ");
        if (parse_additive_op() == ERROR) return (ERROR);
        if (ENABLE_PRETTY_PRINT) printf(" "); strcat(casl_comment, " ");
        if (parse_term() == ERROR) return (ERROR);

        gen_code("POP", "GR2");

        if (opr == TPLUS) {
            gen_code("ADDA", "GR1,GR2");
            gen_code("JOV", "EOVF");
        } else if (opr == TMINUS) {
            gen_code("SUBA", "GR2,GR1");
            gen_code("JOV", "EOVF");
            gen_code("LD", "GR1,GR2");
        } else if (opr == TOR) {
            gen_code("OR", "GR1,GR2");
        }
        is_opr = 0;
    }
    return (NORMAL);
}

// 項の構文解析関数
int parse_term() {
    // ここに演算子を覚える作業用の変数を追加する
    int opr;
    if (parse_factor() == ERROR) return (ERROR);
    // parse_factor()が部分式のコードを生成してくれる(式の結果はGR1に入っている)
    // 部分式の結果をスタックに退避
    // PUSH    0,GR1

    // gen_code(">>>>>PUSH", "0,GR1");

    struct ID *p;
    if ((p = search_localidtab(string_attr)) != NULL) {
        if (p->ispara == 1 && call_stmt_flag == 1) {
            gen_code("LD", "GR1,0,GR1");
        }
    }

    while (token == TSTAR || token == TDIV || token == TAND) {
        is_expression_opr = 1;
        is_opr = 1;
        // ここで演算子を覚える
        opr = token;
        gen_code("PUSH", "0,GR1");
        if (ENABLE_PRETTY_PRINT) printf(" "); strcat(casl_comment, " ");
        if (parse_multiplicative_op() == ERROR) return (ERROR);
        if (ENABLE_PRETTY_PRINT) printf(" "); strcat(casl_comment, " ");
        if (parse_factor() == ERROR) return (ERROR);
        // parse_factor()が部分式のコードを生成してくれる(式の結果はGR1に入っている)
        // ここで被演算子が2つ揃うので，次のようにコードを生成する
        // POP     GR2        退避した結果(左の因子)をGR2に戻す
        gen_code("POP", "GR2");
        // もしoprがTSTARなら，MULA    GR1,GR2
        if (opr == TSTAR) {
            gen_code("MULA", "GR1,GR2");
            gen_code("JOV", "EOVF");
        }
        // もしoprがTDIVなら，DIVA    GR2,GR1
        //                 LD      GR1,GR2
        else if (opr == TDIV) {
            gen_code("DIVA", "GR2,GR1");
            gen_code("JOV", "E0DIV");
            gen_code("LD", "GR1,GR2");
        }
        // もしoprがTANDなら，AND     GR1,GR2
        else if (opr == TAND) {
            gen_code("AND", "GR1,GR2");
        }
        // を生成する．(GR1に結果が入る)
        is_opr = 0;
    }
    return (NORMAL);
}

// 因子の構文解析関数
int parse_factor() {
    if (token == TNAME) {
        if (parse_variable() == ERROR) return (ERROR);
        char tmpstring[MAXSTRSIZE];
        struct ID *p;
        // ローカルを探して，無ければグローバルを探す
        if ((p = search_localidtab(string_attr)) != NULL) {
            sprintf(tmpstring, "$%s%%%s", p->name, p->procname);
            if (p->ispara == 0 && call_stmt_flag == 1) {
                fprintf(caslfp, "\tLAD\tGR1,%s\n", tmpstring);
            } else {
                // fprintf(caslfp, "\tLD\tGR1,%s<<<<<\n", tmpstring); //TODO: 復活するかも
            }
            if (p->ispara == 1 && call_stmt_flag == 0) {
                gen_code("LD", "GR1,0,GR1");
            }
        } 
        else if ((p = search_globalidtab(string_attr)) != NULL) {
            sprintf(tmpstring, "$%s", p->name);
            if (call_stmt_flag == 0) {
                // fprintf(caslfp, "\tLD\tGR1<<<<<<<1627,%s\n", tmpstring);
            } else {
                fprintf(caslfp, "\tLAD\tGR1,%s\n", tmpstring);
                if ((is_opr == 0) && !(token == TCOMMA || token == TRPAREN) && formal_params_flag == 0) {
                    gen_code("LD", "GR1,0,GR1");
                }
            }
        } else {
            return (error("Variable name is not defined"));
        }
        // free(p); // ここでfreeするとエラーが出る

    } else if (token == TNUMBER || token == TFALSE || token == TTRUE ||
               token == TSTRING) {



        if (token == TNUMBER) {
            fprintf(caslfp, "\tLAD\tGR1,%d\n", num_attr);
        } else if (token == TFALSE) {
            gen_code("LD", "GR1,0"); // もしかしたら"GR1,GR0"になるかも
            is_constant_false = 1;
        } else if (token == TTRUE) {
            gen_code("LAD", "GR1,1");
            is_constant_true = 1;
        } else if (token == TSTRING) {
            // 文字列の長さが1でなければエラー
            if (strlen(string_attr) != 3) {
                return (error("String length is not 1"));
            }
            // 文字列の1文字目をGR1にロード
            fprintf(caslfp, "\tLAD\tGR1,%d\n", string_attr[1]);
        }


        if (parse_constant() == ERROR) return (ERROR);
    } else if (token == TLPAREN) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf("( "); strcat(casl_comment, "( ");
        if (parse_expression() == ERROR) return (ERROR);
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" )"); strcat(casl_comment, " )");
    } else if (token == TNOT) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf("not "); strcat(casl_comment, "not ");
        if (parse_factor() == ERROR) return (ERROR);

        // NOT演算子のコードを生成
        int nextlabel = get_label_num();
        gen_code("LAD", "GR2,1");
        gen_code("CPA", "GR0,GR1");
        gen_code_label("JZE", nextlabel);
        gen_code("LD", "GR1,GR2");
        gen_label(nextlabel);
        gen_code("XOR","GR1,GR1");

    } else if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        int tmpresult = token;
        if (ENABLE_PRETTY_PRINT) printf("%s", tokenstr[token]); strcat(casl_comment, tokenstr[token]);

        // 標準型のparseを忘れていたので追加
        if (parse_standard_type() == ERROR) return (ERROR);

        // token = scan();
        if (token != TLPAREN) return (error("Left parenthesis is not found"));
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" ( "); strcat(casl_comment, " ( ");
        int tmpexpression = token;
        if (parse_expression() == ERROR) return (ERROR);
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" )"); strcat(casl_comment, " )");

        if (tmpexpression == TINTEGER) {
            if (tmpresult == TINTEGER) {
                // NONE
            } else if (tmpresult == TBOOLEAN) {
                gen_code("CPA", "GR1,GR0");
                int tmplabel = get_label_num();
                gen_code_label("JZE", tmplabel);
                gen_code("LAD", "GR1,1");
                gen_label(tmplabel);
            } else if (tmpresult == TCHAR) {
                gen_code("LAD", "GR2,127");
                gen_code("AND", "GR1,GR2");
            }
        } else if (tmpexpression == TBOOLEAN) {
            if (tmpresult == TINTEGER) {
                if (is_constant_true == 0 && is_constant_false == 0) {
                    gen_code("CPA", "GR1,GR0");
                    int tmplabel = get_label_num();
                    gen_code_label("JZE", tmplabel);
                    gen_code("LAD", "GR1,1");
                    gen_label(tmplabel);
                }
                is_constant_true = 0;
                is_constant_false = 0;
            } else if (tmpresult == TBOOLEAN) {
                // NONE
            } else if (tmpresult == TCHAR) {
                if (is_constant_true == 0 && is_constant_false == 0) {
                    gen_code("CPA", "GR1,GR0");
                    int tmplabel = get_label_num();
                    gen_code_label("JZE", tmplabel);
                    gen_code("LAD", "GR1,1");
                    gen_label(tmplabel);
                }
                is_constant_true = 0;
                is_constant_false = 0;
            }
        } else if (tmpexpression == TCHAR) {
            if (tmpresult == TINTEGER) {
                // NONE
            } else if (tmpresult == TBOOLEAN) {
                gen_code("CPA", "GR1,GR0");
                int tmplabel = get_label_num();
                gen_code_label("JZE", tmplabel);
                gen_code("LAD", "GR1,1");
                gen_label(tmplabel);
            } else if (tmpresult == TCHAR) {
                // NONE
            }
        }


    } else {
        return (error("Factor is not found"));
    }
    return (NORMAL);
}

// 定数の構文解析関数
int parse_constant() {
    if (token == TNUMBER) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf("%d", num_attr); 
        char tmpstring[MAXSTRSIZE];
        sprintf(tmpstring, "%d", num_attr);
        strcat(casl_comment, tmpstring);
    } else if (token == TSTRING) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf("\'%s\'", string_attr);
        char tmpstring[MAXSTRSIZE];
        sprintf(tmpstring, "\'%s\'", string_attr);
        strcat(casl_comment, tmpstring);
    } else if (token == TFALSE) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf("false"); strcat(casl_comment, "false");
    } else if (token == TTRUE) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf("true"); strcat(casl_comment, "true");
    } else {
        return (error("Constant is not found"));
    }
    return (NORMAL);
}

// 乗算演算子の構文解析関数
int parse_multiplicative_op() {
    if (token == TSTAR || token == TDIV || token == TAND) {
        if (ENABLE_PRETTY_PRINT) printf("%s", tokenstr[token]); strcat(casl_comment, tokenstr[token]);
        token = scan();
    } else {
        return (error("Multiplicative operator is not found"));
    }
    return (NORMAL);
}

// 加算演算子の構文解析関数
int parse_additive_op() {
    if (token == TPLUS || token == TMINUS || token == TOR) {
        if (ENABLE_PRETTY_PRINT) printf("%s", tokenstr[token]); strcat(casl_comment, tokenstr[token]);
        token = scan();
    } else {
        return (error("Additive operator is not found"));
    }
    return (NORMAL);
}

// 関係演算子の構文解析関数
int parse_relational_op() {
    current_relational_opr = token;
    if (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ ||
        token == TGR || token == TGREQ) {
        if (ENABLE_PRETTY_PRINT) printf("%s", tokenstr[token]); strcat(casl_comment, tokenstr[token]);
        token = scan();
    } else {
        return (error("Relational operator is not found"));
    }
    return (NORMAL);
}

// 入力文の構文解析関数
int parse_input_stmt() {
    int tmp = token;
    // strcat(casl_comment, "; ");
    indent_print(indent);
    if (token != TREAD && token != TREADLN)
        return (error("Keyword 'read' or 'readln' is not found"));
    if (ENABLE_PRETTY_PRINT) printf("%s", tokenstr[token]); strcat(casl_comment, tokenstr[token]);
    token = scan();
    if (token == TLPAREN) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" ( "); strcat(casl_comment, " ( ");

        // if (token == TNAME) {
        //     strcat(casl_comment, string_attr);
        //     // string_attrを局所変数表から検索して，
        //     // LD GR1,$$n%kazuyomikomi
        //     // みたいにcaslfpに書き込む
        //     struct ID *p;
        //     if ((p = search_localidtab(string_attr)) == NULL) {
        //         return (error("Variable name is not defined"));
        //     }
        //     if (p->ispara == 1) {
        //         fprintf(caslfp, "\tLD\tGR1,$$%s%%%s\n", p->name, p->procname);
        //     } else {
        //         fprintf(caslfp, "\tLD\tGR1,$%s%%%s\n", p->name, p->procname);
        //     }
        //     if (p->itp->ttype == TPINT) {
        //         gen_code("CALL", "READINT");
        //     } else if (p->itp->ttype == TPCHAR) {
        //         gen_code("CALL", "READCHAR");
        //     }
        // }
        

        if (parse_variable() == ERROR) return (ERROR);
        while (token == TCOMMA) {
            token = scan();
            if (ENABLE_PRETTY_PRINT) printf(" , "); strcat(casl_comment, " , ");
            if (parse_variable() == ERROR) return (ERROR);
        }
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" )"); strcat(casl_comment, " )");
    }
    gen_code("CALL", "READINT");
    if (tmp == TREADLN) {
        gen_code("CALL", "READLINE");
    }

    indent--;
    fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");
    return (NORMAL);
}

// 出力文の構文解析関数
int parse_output_stmt() {
    int tmp = token;
    indent_print(indent);
    // strcat(casl_comment, "; ");
    if (token != TWRITE && token != TWRITELN)
        return (error("Keyword 'write' or 'writeln' is not found"));
    if (ENABLE_PRETTY_PRINT) printf("%s", tokenstr[token]); strcat(casl_comment, tokenstr[token]);
    token = scan();
    if (token == TLPAREN) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" ( "); strcat(casl_comment, " ( ");
        if (parse_output_format() == ERROR) return (ERROR);
        // gen_code("LAD", "GR2,0");
        // gen_code("CALL", "WRITESTR");
        while (token == TCOMMA) {
            token = scan();
            if (ENABLE_PRETTY_PRINT) printf(" , "); strcat(casl_comment, " , ");
            if (parse_output_format() == ERROR) return (ERROR);
            // gen_code("LAD", "GR2,0");
            // gen_code("CALL", "WRITESTR");
        }

        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf(" )"); strcat(casl_comment, " )");
    }
    indent--;
    if (tmp == TWRITELN) {
        gen_code("CALL", "WRITELINE");
    }
    if (ENABLE_PRETTY_PRINT_IN_CASL) fprintf(caslfp, "%s\n", casl_comment);
    strcpy(casl_comment, "; ");
    return (NORMAL);
}

// 出力書式の構文解析関数
int parse_output_format() {
    int tmp = token;
    if (token == TPLUS || token == TMINUS || token == TNAME ||
        token == TNUMBER || token == TFALSE || token == TTRUE ||
        token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        if (parse_expression() == ERROR) return (ERROR);
        if (token == TCOLON) {
            token = scan();
            if (ENABLE_PRETTY_PRINT) printf(" : "); strcat(casl_comment, " : ");
            if (token != TNUMBER) return (error("Number is not found"));
            token = scan();
            if (ENABLE_PRETTY_PRINT) printf("%d", num_attr); 
            // num_attrを文字列に変換
            char num_attr_str[MAXSTRSIZE];
            sprintf(num_attr_str, "%d", num_attr);
            strcat(casl_comment, num_attr_str);
        }
    } else if (token == TSTRING) {
        token = scan();
        if (ENABLE_PRETTY_PRINT) printf("\'%s\'", string_attr);
        fprintf(caslfp, "\tLAD\tGR1,=\'%s\'\n", string_attr);
        strcat(casl_comment, "\'");
        strcat(casl_comment, string_attr);
        strcat(casl_comment, "\'");
    } else {
        return (error("Output format is not found"));
    }
    gen_code("LAD", "GR2,0");
    if (tmp == TSTRING) {
        gen_code("CALL", "WRITESTR");
    } else {
        gen_code("CALL", "WRITEINT");
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

    // 文字列"sample11pp.mpl"を，文字列"sample11pp.csl"みたいに変換する
    char caslfilename[MAXSTRSIZE];
    strcpy(caslfilename, np[1]);
    caslfilename[strlen(caslfilename) - 1] = 'l';
    caslfilename[strlen(caslfilename) - 2] = 's';
    caslfilename[strlen(caslfilename) - 3] = 'c';
    caslfilename[strlen(caslfilename) - 4] = '.';
    caslfilename[strlen(caslfilename)] = '\0';

    // 書き込むファイルのオープン
    if ((caslfp = fopen(caslfilename, "w")) == NULL) {
        error("File can not open.");
        return 0;
    }

    // 大域変数表の初期化
    init_globalidtab();

    // まず1つめのトークンを読んでおく
    token = scan();

    // 変数表の第1行を出力
    // printf("%-15s|%-30s|%-15s|%-10s\n", "Name", "Type", "Define", "References");

    // 構文解析
    parse_program();

    // ファイルのクローズ
    end_scan();

    // 書き込むファイルのクローズ
    fclose(caslfp);

    // 変数表の出力
    print_globalidtab();

    // 変数表の解放
    free_globalidtab();

    return 0;
}