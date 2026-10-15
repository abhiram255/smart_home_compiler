/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 2
#define YYMINOR 0

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 0

#line 3 "parser.y"
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
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 39 "parser.y"
typedef union YYSTYPE { char *text; } YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 64 "parser.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

#if !(defined(yylex) || defined(YYSTATE))
int YYLEX_DECL();
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define IDENT 257
#define INT 258
#define SENSOR 259
#define DEVICE 260
#define NUMBER 261
#define SWITCH 262
#define LOCK 263
#define MAX_AGE 264
#define RULE 265
#define WHEN 266
#define THEN 267
#define ON_FAULT 268
#define LE 269
#define GE 270
#define EQ 271
#define NE 272
#define YYERRCODE 256
typedef int YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    9,    9,   11,   11,    1,    1,   10,   10,   12,
    8,    8,    6,    7,    7,    2,    2,    2,    2,    2,
    2,    3,    3,    3,    4,    4,    4,    5,    5,
};
static const YYINT yylen[] = {                            2,
    2,    0,    2,    7,    5,    1,    1,    1,    2,   13,
    0,    3,    3,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    3,    3,    1,    3,    3,    1,    3,
};
static const YYINT yydefred[] = {                         2,
    0,    0,    0,    0,    0,    0,    3,    8,    0,    0,
    0,    9,    0,    0,    0,    0,    6,    7,    0,    0,
    0,    5,    0,    0,   17,   19,   20,   21,   16,   18,
    0,    4,   28,    0,    0,    0,   25,    0,    0,    0,
    0,    0,    0,   29,    0,    0,    0,   26,   27,    0,
    0,    0,    0,   14,   15,   13,    0,    0,    0,   10,
   12,
};
static const YYINT yydgoto[] = {                          1,
   19,   31,   35,   36,   37,   51,   56,   58,    2,    6,
    7,    8,
};
static const YYINT yysindex[] = {                         0,
    0, -244, -240, -235, -232, -236,    0,    0,  -27,  -21,
  -83,    0, -220, -229, -224, -221,    0,    0,  -15, -212,
 -211,    0,  -59,  -11,    0,    0,    0,    0,    0,    0,
  -40,    0,    0,  -40,  -32,  -28,    0,  -13, -218,  -40,
  -40,  -40,  -40,    0, -207,  -28,  -28,    0,    0,    5,
   -7, -239, -222,    0,    0,    0, -207,  -72,   -3,    0,
    0,
};
static const YYINT yyrindex[] = {                         0,
    0,    0,    0,    0,    0,   54,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -39,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -36,  -33,    0,    0,    0,
    0,    0,  -70,    0,    0,    0,    0,    0,    0,    0,
    0,
};
static const YYINT yygindex[] = {                         0,
    0,    0,   23,   -5,   -4,    1,    0,    0,    0,    0,
    0,   53,
};
#define YYTABLESIZE 218
static const YYINT yytable[] = {                         34,
   29,   22,   30,   22,   23,   22,   23,   24,   23,   24,
   40,   24,   41,   42,    3,    4,    9,   54,   43,   22,
    5,   10,   23,   55,   11,   24,   39,   44,    5,   40,
   13,   41,   17,   18,   46,   47,   14,   48,   49,   15,
   16,   20,   21,   22,   23,   57,   24,   32,   45,   50,
   52,   53,   60,    1,   11,   61,   38,   59,   12,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   25,
   26,   27,   28,    0,    0,    0,    0,   33,
};
static const YYINT yycheck[] = {                         40,
   60,   41,   62,   43,   41,   45,   43,   41,   45,   43,
   43,   45,   45,   42,  259,  260,  257,  257,   47,   59,
  265,  257,   59,  263,  257,   59,   59,   41,  265,   43,
   58,   45,  262,  263,   40,   41,   58,   42,   43,  123,
  261,  266,  264,   59,  257,  268,  258,   59,  267,  257,
   46,   59,  125,    0,  125,   59,   34,   57,    6,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  269,
  270,  271,  272,   -1,   -1,   -1,   -1,  258,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 272
#define YYUNDFTOKEN 287
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
#ifndef NULL
#define NULL (void*)0
#endif
static const char *const yyname[] = {

"end-of-file",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"'('","')'","'*'","'+'",NULL,
"'-'","'.'","'/'",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"':'","';'",
"'<'",NULL,"'>'",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
"'{'",NULL,"'}'",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,"IDENT","INT","SENSOR","DEVICE","NUMBER",
"SWITCH","LOCK","MAX_AGE","RULE","WHEN","THEN","ON_FAULT","LE","GE","EQ","NE",
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : program",
"program : declarations rules",
"declarations :",
"declarations : declarations declaration",
"declaration : SENSOR IDENT ':' NUMBER MAX_AGE INT ';'",
"declaration : DEVICE IDENT ':' device_type ';'",
"device_type : SWITCH",
"device_type : LOCK",
"rules : rule",
"rules : rules rule",
"rule : RULE IDENT '{' WHEN IDENT comparator expression ';' THEN action ';' fault_action '}'",
"fault_action :",
"fault_action : ON_FAULT action ';'",
"action : IDENT '.' command_name",
"command_name : IDENT",
"command_name : LOCK",
"comparator : '<'",
"comparator : LE",
"comparator : '>'",
"comparator : GE",
"comparator : EQ",
"comparator : NE",
"expression : term",
"expression : expression '+' term",
"expression : expression '-' term",
"term : factor",
"term : term '*' factor",
"term : term '/' factor",
"factor : INT",
"factor : '(' expression ')'",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;
int      yynerrs;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;
#line 133 "parser.y"

void yyerror(const char *message) {
    fprintf(stderr, "Syntax error at %d:%d: %s\n", token_line, token_col, message);
}
#line 322 "parser.c"

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == NULL)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == NULL)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != NULL)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    /* yym is set below */
    /* yyn is set below */
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        yychar = YYLEX;
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);

    switch (yyn)
    {
case 4:
#line 58 "parser.y"
	{
            append_item(&sensors_json, make_text("{\"name\":\"%s\",\"type\":\"number\",\"max_age\":%s}", yystack.l_mark[-5].text, yystack.l_mark[-1].text));
            free(yystack.l_mark[-5].text); free(yystack.l_mark[-1].text);
        }
#line 527 "parser.c"
break;
case 5:
#line 63 "parser.y"
	{
            append_item(&devices_json, make_text("{\"name\":\"%s\",\"type\":\"%s\"}", yystack.l_mark[-3].text, yystack.l_mark[-1].text));
            free(yystack.l_mark[-3].text); free(yystack.l_mark[-1].text);
        }
#line 535 "parser.c"
break;
case 6:
#line 70 "parser.y"
	{ yyval.text = make_text("switch"); }
#line 540 "parser.c"
break;
case 7:
#line 71 "parser.y"
	{ yyval.text = make_text("lock"); }
#line 545 "parser.c"
break;
case 10:
#line 81 "parser.y"
	{
            append_item(&rules_json, make_text(
                "{\"name\":\"%s\",\"sensor\":\"%s\",\"comparator\":\"%s\",\"threshold\":%s,\"then_action\":%s,\"fault_action\":%s}",
                yystack.l_mark[-11].text, yystack.l_mark[-8].text, yystack.l_mark[-7].text, yystack.l_mark[-6].text, yystack.l_mark[-3].text, yystack.l_mark[-1].text));
            free(yystack.l_mark[-11].text); free(yystack.l_mark[-8].text); free(yystack.l_mark[-7].text); free(yystack.l_mark[-6].text); free(yystack.l_mark[-3].text); free(yystack.l_mark[-1].text);
        }
#line 555 "parser.c"
break;
case 11:
#line 90 "parser.y"
	{ yyval.text = make_text("null"); }
#line 560 "parser.c"
break;
case 12:
#line 91 "parser.y"
	{ yyval.text = yystack.l_mark[-1].text; }
#line 565 "parser.c"
break;
case 13:
#line 96 "parser.y"
	{
            yyval.text = make_text("{\"device\":\"%s\",\"command\":\"%s\"}", yystack.l_mark[-2].text, yystack.l_mark[0].text);
            free(yystack.l_mark[-2].text); free(yystack.l_mark[0].text);
        }
#line 573 "parser.c"
break;
case 14:
#line 103 "parser.y"
	{ yyval.text = yystack.l_mark[0].text; }
#line 578 "parser.c"
break;
case 15:
#line 104 "parser.y"
	{ yyval.text = make_text("lock"); }
#line 583 "parser.c"
break;
case 16:
#line 108 "parser.y"
	{ yyval.text = make_text("<"); }
#line 588 "parser.c"
break;
case 17:
#line 109 "parser.y"
	{ yyval.text = make_text("<="); }
#line 593 "parser.c"
break;
case 18:
#line 110 "parser.y"
	{ yyval.text = make_text(">"); }
#line 598 "parser.c"
break;
case 19:
#line 111 "parser.y"
	{ yyval.text = make_text(">="); }
#line 603 "parser.c"
break;
case 20:
#line 112 "parser.y"
	{ yyval.text = make_text("=="); }
#line 608 "parser.c"
break;
case 21:
#line 113 "parser.y"
	{ yyval.text = make_text("!="); }
#line 613 "parser.c"
break;
case 22:
#line 117 "parser.y"
	{ yyval.text = yystack.l_mark[0].text; }
#line 618 "parser.c"
break;
case 23:
#line 118 "parser.y"
	{ yyval.text = make_text("{\"op\":\"+\",\"left\":%s,\"right\":%s}", yystack.l_mark[-2].text, yystack.l_mark[0].text); free(yystack.l_mark[-2].text); free(yystack.l_mark[0].text); }
#line 623 "parser.c"
break;
case 24:
#line 119 "parser.y"
	{ yyval.text = make_text("{\"op\":\"-\",\"left\":%s,\"right\":%s}", yystack.l_mark[-2].text, yystack.l_mark[0].text); free(yystack.l_mark[-2].text); free(yystack.l_mark[0].text); }
#line 628 "parser.c"
break;
case 25:
#line 123 "parser.y"
	{ yyval.text = yystack.l_mark[0].text; }
#line 633 "parser.c"
break;
case 26:
#line 124 "parser.y"
	{ yyval.text = make_text("{\"op\":\"*\",\"left\":%s,\"right\":%s}", yystack.l_mark[-2].text, yystack.l_mark[0].text); free(yystack.l_mark[-2].text); free(yystack.l_mark[0].text); }
#line 638 "parser.c"
break;
case 27:
#line 125 "parser.y"
	{ yyval.text = make_text("{\"op\":\"/\",\"left\":%s,\"right\":%s}", yystack.l_mark[-2].text, yystack.l_mark[0].text); free(yystack.l_mark[-2].text); free(yystack.l_mark[0].text); }
#line 643 "parser.c"
break;
case 28:
#line 129 "parser.y"
	{ yyval.text = yystack.l_mark[0].text; }
#line 648 "parser.c"
break;
case 29:
#line 130 "parser.y"
	{ yyval.text = yystack.l_mark[-1].text; }
#line 653 "parser.c"
break;
#line 655 "parser.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            yychar = YYLEX;
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
