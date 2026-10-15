%option noyywrap noinput nounput nodefault
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Compiled through frontend.c after parser.c defines the YACC tokens. */

int token_line = 1;
int token_col = 1;
int lex_line = 1;
int lex_col = 1;

static char *copy_text(const char *source) {
    size_t length = strlen(source) + 1;
    char *result = (char *)malloc(length);
    if (!result) { fprintf(stderr, "Out of memory\n"); exit(4); }
    memcpy(result, source, length);
    return result;
}

#define YY_USER_ACTION do {                     \
    int n;                                       \
    token_line = lex_line;                       \
    token_col = lex_col;                         \
    for (n = 0; n < yyleng; ++n) {              \
        if (yytext[n] == '\n') {                 \
            ++lex_line; lex_col = 1;            \
        } else { ++lex_col; }                    \
    }                                           \
} while (0);
%}

%%
[ \t\r\n]+           { /* whitespace */ }
"//"[^\n]*          { /* comment */ }
"#"[^\n]*           { /* comment */ }
"sensor"             { return SENSOR; }
"device"             { return DEVICE; }
"number"             { return NUMBER; }
"switch"             { return SWITCH; }
"lock"               { return LOCK; }
"max_age"            { return MAX_AGE; }
"rule"               { return RULE; }
"when"               { return WHEN; }
"then"               { return THEN; }
"on_fault"           { return ON_FAULT; }
"<="                 { return LE; }
">="                 { return GE; }
"=="                 { return EQ; }
"!="                 { return NE; }
[0-9]+               { yylval.text = copy_text(yytext); return INT; }
[A-Za-z_][A-Za-z_0-9]* { yylval.text = copy_text(yytext); return IDENT; }
[{}:;.()+*/<>-]      { return yytext[0]; }
.                     { fprintf(stderr, "Lexical error at %d:%d: unexpected '%s'\n", token_line, token_col, yytext); exit(2); }
%%
