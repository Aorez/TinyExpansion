/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
   { START,INASSIGN,INCOMMENT,INNUM,INID,DONE }
   StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{ if (!(linepos < bufsize))
  { lineno++;
    if (fgets(lineBuf,BUFLEN-1,source))
    { if (EchoSource) fprintf(listing,"%4d: %s",lineno,lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    }
    else
    { EOF_flag = TRUE;
      return EOF;
    }
  }
  else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{ if (!EOF_flag) linepos-- ;}

/* lookup table of reserved words */
static struct
    { char* str;
      TokenType tok;
    } reservedWords[MAXRESERVED]
   = {{"if",IF},{"then",THEN},{"else",ELSE},{"end",END},
      {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
      {"write",WRITE},
    //加入逻辑运算符
       {"and", AND},{"or",OR},{"not",NOT}
    };

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup (char * s)
{ int i;
  for (i=0;i<MAXRESERVED;i++)
    if (!strcmp(s,reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the 
 * next token in source file
 */
TokenType getToken(void)
{  /* index for storing into tokenString */
   int tokenStringIndex = 0;
   /* holds current token to be returned */
   TokenType currentToken;
   /* current state - always begins at START */
   StateType state = START;
   /* flag to indicate save to tokenString */
   int save;
   while (state != DONE)
   { int c = getNextChar();
     save = TRUE;
     switch (state)
     { case START:
         if (isdigit(c))
             state = INNUM;
         else if (isalpha(c))
             state = INID;
         else if (c == ':')
             state = ASSIGN;//冒号等号修改为正则表达式的赋值
         else if (c == '=')//修改为等号
             state = INASSIGN;
         else if (c == '-')//减号单独判断，因为加上了减法赋值号
             state = MINUS;
         else if (c == '>') {
             state = GT;//加入大于号
         }
         else if (c == '<') {
             state = LT;//小于号单独判断
         }
         else if ((c == ' ') || (c == '\t') || (c == '\n'))
           save = FALSE;
         else if (c == '{')
         { save = FALSE;
           state = INCOMMENT;
         }
         else
         { state = DONE;
           switch (c)
           { case EOF:
               save = FALSE;
               currentToken = ENDFILE;
               break;
             //case '='://两个等号表示相等符号
             //  currentToken = EQ;
             //  break;
             //case '<':
             //  currentToken = LT;
             //  break;
             case '+':
               currentToken = PLUS;
               break;
             //case '-':
             //  currentToken = MINUS;
             //  break;
             case '*':
               currentToken = TIMES;
               break;
             case '/':
               currentToken = OVER;
               break;
             case '%'://取余
                 currentToken = REMAIN;
                 break;
             case '^'://乘方
                 currentToken = POWER;
                 break;
             case '(':
               currentToken = LPAREN;
               break;
             case ')':
               currentToken = RPAREN;
               break;
             case ';':
               currentToken = SEMI;
               break;
             case '|'://正则或
                 currentToken = REG_OR;
                 break;
             case '&'://正则连接
                 currentToken = REG_JOIN;
                 break;
             case '#'://正则闭包
                 currentToken = REG_CLOSURE;
                 break;
             default:
               currentToken = ERROR;
               break;
           }
         }
         break;
       case INCOMMENT:
         save = FALSE;
         if (c == EOF)
         { state = DONE;
           currentToken = ENDFILE;
         }
         else if (c == '}') state = START;
         break;
       case INASSIGN:
           state = DONE;
           if (c == '=') {
               currentToken = EQ;//两个等号是相等符号
           }
           else {
               ungetNextChar();
               currentToken = ASSIGN;//一个等号表示赋值
           }
         //if (c == '=')
         //  currentToken = ASSIGN;
         //else
         //{ /* backup in the input */
         //  ungetNextChar();
         //  save = FALSE;
         //  currentToken = ERROR;
         //}
         break;
         //这些case值本来应该统一用StateType，但是TokenType中的数字不冲突，因此也可以用
       case ASSIGN:
           state = DONE;
           if (c == '=') {
               currentToken = REG_ASSIGN;//正则表达式赋值
           }
           else {
               ungetNextChar();
               save = FALSE;
               currentToken = ERROR;
           }
           break;
       case MINUS:
           state = DONE;
           if (c == '=') {
               currentToken = ASSIGN;//减法赋值号
           }
           else {
               ungetNextChar();
               currentToken = MINUS;
           }
           break;
       case GT:
           state = DONE;
           if (c == '=') {
               currentToken = GTE;//大于等于
           }
           else {
               ungetNextChar();
               currentToken = GT;
           }
           break;
       case LT:
           state = DONE;
           if (c == '=') {
               currentToken = LTE;//小于等于
           }
           else if (c == '>') {
               currentToken = NEQ;//不等于
           }
           else {
               ungetNextChar();
               currentToken = LT;
           }
           break;
       case INNUM:
         if (!isdigit(c))
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = NUM;
         }
         break;
       case INID:
         if (!isalpha(c))
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = ID;
         }
         break;
       case DONE:
       default: /* should never happen */
         fprintf(listing,"Scanner Bug: state= %d\n",state);
         state = DONE;
         currentToken = ERROR;
         break;
     }
     if ((save) && (tokenStringIndex <= MAXTOKENLEN))
       tokenString[tokenStringIndex++] = (char) c;
     if (state == DONE)
     { tokenString[tokenStringIndex] = '\0';
       if (currentToken == ID)
         currentToken = reservedLookup(tokenString);
     }
   }
   if (TraceScan) {
     fprintf(listing,"\t%d: ",lineno);
     printToken(currentToken,tokenString);
   }
   return currentToken;
} /* end getToken */

