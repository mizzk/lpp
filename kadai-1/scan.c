#include <ctype.h>

#include "token-list.h"

FILE *fp = NULL; /* File pointer */
int cbuf;        /* Character buffer */

/* Open file */
int init_scan(char *filename) {
    if ((fp = fopen(filename, "r")) == NULL) {
        error("File can not open.");
        cbuf = EOF;
        return -1;
    }
    
    cbuf = fgetc(fp);
    return 0;
}

/* トークンを一つスキャンする関数*/
int scan(void) {
    char buffer[MAXSTRSIZE];
    int buffer_index = 0;

    while (1) {
        if (cbuf == ' ' || cbuf == '\t') { //空白とタブは読み飛ばす
            cbuf = fgetc(fp);
            continue;
        }else if(cbuf == '\r') { //改行コードは読み飛ばす
            cbuf = fgetc(fp);
            if (cbuf == '\n') {
                cbuf = fgetc(fp);
            }
            // TODO:ここで行番号をインクリメントする
            continue;
        } else if(cbuf == '\n') {
            cbuf = fgetc(fp);
            if (cbuf == '\r') {
                cbuf = fgetc(fp);
            } 
            // TODO:ここで行番号をインクリメントする
            continue;
        } else if(cbuf == '{') { //注釈文は読み飛ばす
            while(cbuf != '}') {
                cbuf = fgetc(fp);
                if (cbuf == EOF) {
                    error("Comment is not closed.");
                    return -1;
                }
            }
            continue;
        } else if(cbuf == '/') {
            cbuf = fgetc(fp);
            if (cbuf == '*') {
                while(cbuf != '*') {
                    cbuf = fgetc(fp);
                    if (cbuf == '*') {
                        cbuf = fgetc(fp);
                        if (cbuf == '/') {
                            cbuf = fgetc(fp);
                            break;
                        }
                    } else if (cbuf == EOF) {
                        error("Comment is not closed.");
                        return -1;
                    }
                }
                continue;
            }
        }else if (isalpha(cbuf)) {             // 英字の場合
            while (isalnum(cbuf)) {             // 英数字が続く限り
                buffer[buffer_index++] = cbuf;  // バッファに格納
                cbuf = fgetc(fp);
            }
            buffer[buffer_index] = '\0';  // 文字列を終端

            // キーワードかどうか判定
            for (int i = 0; i < KEYWORDSIZE; i++) {
                if (strcmp(buffer, key[i].keyword) == 0) {
                    // キーワードの場合は，keytokenを返す
                    return key[i].keytoken;
                }
            }
            // キーワードでない場合は，string_attrに格納してTNAMEを返す
            strcpy(string_attr, buffer);
            return TNAME;

        } else if (isdigit(cbuf)) {             // 数字の場合
            while (isdigit(cbuf)) {             // 数字が続く限り
                buffer[buffer_index++] = cbuf;  // バッファに格納
                cbuf = fgetc(fp);
            }
            buffer[buffer_index] = '\0';  // 文字列を終端

            // 数字の場合は，num_attrに格納してTNUMBERを返す
            num_attr = atoi(buffer);

            // 32768より大きい場合はエラー
            if (num_attr > 32768) {
                error("Number is too large.");
                return -1;
            }
            return TNUMBER;

        } else if (cbuf == '\'') {  // 文字列の場合
            cbuf = fgetc(fp);
            while (cbuf != '\'') {              // 文字列が終わるまで
                buffer[buffer_index++] = cbuf;  // バッファに格納
                cbuf = fgetc(fp);
            }
            buffer[buffer_index] = '\0';  // 文字列を終端

            // 文字列の場合は，string_attrに格納してTSTRINGを返す
            strcpy(string_attr, buffer);
            return TSTRING;
        } else if (cbuf == '+') {
            cbuf = fgetc(fp);
            return TPLUS;
        } else if (cbuf == '-') {
            cbuf = fgetc(fp);
            return TMINUS;
        } else if (cbuf == '*') {
            cbuf = fgetc(fp);
            return TSTAR;
        } else if (cbuf == '=') {
            cbuf = fgetc(fp);
            return TEQUAL;
        } else if (cbuf == '<') {
            cbuf = fgetc(fp);
            if (cbuf == '>') {
                cbuf = fgetc(fp);
                return TNOTEQ;
            } else if (cbuf == '=') {
                cbuf = fgetc(fp);
                return TLEEQ;
            } else {
                return TLE;
            }
        } else if (cbuf == '>') {
            cbuf = fgetc(fp);
            if (cbuf == '=') {
                cbuf = fgetc(fp);
                return TGREQ;
            } else {
                return TGR;
            }
        } else if (cbuf == '(') {
            cbuf = fgetc(fp);
            return TLPAREN;
        } else if (cbuf == ')') {
            cbuf = fgetc(fp);
            return TRPAREN;
        } else if (cbuf == '[') {
            cbuf = fgetc(fp);
            return TLSQPAREN;
        } else if (cbuf == ']') {
            cbuf = fgetc(fp);
            return TRSQPAREN;
        } else if (cbuf == ':') {
            cbuf = fgetc(fp);
            if (cbuf == '=') {
                cbuf = fgetc(fp);
                return TASSIGN;
            } else {
                return TCOLON;
            }
        } else if (cbuf == '.') {
            cbuf = fgetc(fp);
            return TDOT;
        } else if (cbuf == ',') {
            cbuf = fgetc(fp);
            return TCOMMA;
        } else if (cbuf == ';') {
            cbuf = fgetc(fp);
            return TSEMI;
        } else if (cbuf == EOF) {
            return -1;
        } else {
            // どのパターンにも当てはまらない場合は読み飛ばす
            error("Invalid character.");
            cbuf = fgetc(fp);
            continue;
        }
    }
}

/* 行番号関数 */
int get_linenum(void) { return 0; }

/* Close file */
void end_scan(void) {
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
    return;
}