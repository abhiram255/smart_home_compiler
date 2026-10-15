#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* One translation unit lets the Flex scanner use YACC tokens directly. */
#include "parser.c"
#include "lexer.c"

extern FILE *yyin;
extern char *yytext;
extern int token_line, token_col, lex_line, lex_col;
extern char *sensors_json, *devices_json, *rules_json;
int yylex(void);
int yyparse(void);

static const char *token_name(int token) {
    switch (token) {
        case SENSOR: return "sensor";
        case DEVICE: return "device";
        case NUMBER: return "number";
        case SWITCH: return "switch";
        case LOCK: return "lock";
        case MAX_AGE: return "max_age";
        case RULE: return "rule";
        case WHEN: return "when";
        case THEN: return "then";
        case ON_FAULT: return "on_fault";
        case IDENT: return "IDENT";
        case INT: return "INT";
        case LE: case GE: case EQ: case NE: return "OP";
        default: return "OP";
    }
}

int main(int argc, char **argv) {
    int token;
    if (argc != 3 || (strcmp(argv[1], "--tokens") && strcmp(argv[1], "--parse"))) {
        fprintf(stderr, "Usage: frontend --tokens|--parse input.home\n");
        return 1;
    }
    yyin = fopen(argv[2], "rb");
    if (!yyin) { perror(argv[2]); return 1; }
    if (!strcmp(argv[1], "--tokens")) {
        while ((token = yylex()) != 0) {
            printf("%d:%d  %-10s %s\n", token_line, token_col, token_name(token), yytext);
            if (token == IDENT || token == INT) free(yylval.text);
        }
        printf("%d:%d  EOF\n", lex_line, lex_col);
        fclose(yyin);
        return 0;
    }
    if (yyparse()) { fclose(yyin); return 3; }
    printf("{\"sensors\":[%s],\"devices\":[%s],\"rules\":[%s]}\n",
           sensors_json ? sensors_json : "",
           devices_json ? devices_json : "",
           rules_json ? rules_json : "");
    fclose(yyin);
    return 0;
}
