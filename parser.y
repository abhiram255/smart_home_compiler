/* Regenerate with: byacc -o parser.c parser.y (no header is needed). */
%{
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char *message);
extern int token_line, token_col;

char *sensors_json = NULL;
char *devices_json = NULL;
char *rules_json = NULL;

static char *make_text(const char *format, ...) {
    va_list args, copy;
    int length;
    char *result;
    va_start(args, format);
    va_copy(copy, args);
    length = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    result = (char *)malloc((size_t)length + 1);
    if (!result) { fprintf(stderr, "Out of memory\n"); exit(4); }
    vsnprintf(result, (size_t)length + 1, format, args);
    va_end(args);
    return result;
}

static void append_item(char **list, char *item) {
    char *joined = *list ? make_text("%s,%s", *list, item) : make_text("%s", item);
    free(*list);
    free(item);
    *list = joined;
}
%}

%union { char *text; }
%token <text> IDENT INT
%token SENSOR DEVICE NUMBER SWITCH LOCK MAX_AGE RULE WHEN THEN ON_FAULT
%token LE GE EQ NE
%type <text> device_type comparator expression term factor action command_name fault_action
%start program

%%
program:
    declarations rules
    ;

declarations:
    /* empty */
    | declarations declaration
    ;

declaration:
    SENSOR IDENT ':' NUMBER MAX_AGE INT ';'
        {
            append_item(&sensors_json, make_text("{\"name\":\"%s\",\"type\":\"number\",\"max_age\":%s}", $2, $6));
            free($2); free($6);
        }
    | DEVICE IDENT ':' device_type ';'
        {
            append_item(&devices_json, make_text("{\"name\":\"%s\",\"type\":\"%s\"}", $2, $4));
            free($2); free($4);
        }
    ;

device_type:
    SWITCH { $$ = make_text("switch"); }
    | LOCK { $$ = make_text("lock"); }
    ;

rules:
    rule
    | rules rule
    ;

rule:
    RULE IDENT '{' WHEN IDENT comparator expression ';' THEN action ';' fault_action '}'
        {
            append_item(&rules_json, make_text(
                "{\"name\":\"%s\",\"sensor\":\"%s\",\"comparator\":\"%s\",\"threshold\":%s,\"then_action\":%s,\"fault_action\":%s}",
                $2, $5, $6, $7, $10, $12));
            free($2); free($5); free($6); free($7); free($10); free($12);
        }
    ;

fault_action:
    /* empty */ { $$ = make_text("null"); }
    | ON_FAULT action ';' { $$ = $2; }
    ;

action:
    IDENT '.' command_name
        {
            $$ = make_text("{\"device\":\"%s\",\"command\":\"%s\"}", $1, $3);
            free($1); free($3);
        }
    ;

command_name:
    IDENT { $$ = $1; }
    | LOCK { $$ = make_text("lock"); }
    ;

comparator:
    '<' { $$ = make_text("<"); }
    | LE { $$ = make_text("<="); }
    | '>' { $$ = make_text(">"); }
    | GE { $$ = make_text(">="); }
    | EQ { $$ = make_text("=="); }
    | NE { $$ = make_text("!="); }
    ;

expression:
    term { $$ = $1; }
    | expression '+' term { $$ = make_text("{\"op\":\"+\",\"left\":%s,\"right\":%s}", $1, $3); free($1); free($3); }
    | expression '-' term { $$ = make_text("{\"op\":\"-\",\"left\":%s,\"right\":%s}", $1, $3); free($1); free($3); }
    ;

term:
    factor { $$ = $1; }
    | term '*' factor { $$ = make_text("{\"op\":\"*\",\"left\":%s,\"right\":%s}", $1, $3); free($1); free($3); }
    | term '/' factor { $$ = make_text("{\"op\":\"/\",\"left\":%s,\"right\":%s}", $1, $3); free($1); free($3); }
    ;

factor:
    INT { $$ = $1; }
    | '(' expression ')' { $$ = $2; }
    ;
%%

void yyerror(const char *message) {
    fprintf(stderr, "Syntax error at %d:%d: %s\n", token_line, token_col, message);
}
