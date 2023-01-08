#include"pch.h"
/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType2 token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * regular_exp(void);
static TreeNode * or_exp(void);
static TreeNode * join_exp(void);
static TreeNode * exp(void);
static TreeNode * logical_exp(void);//添加逻辑表达式
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);

static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

static void match(TokenType2 expected)
//{ if (token == expected) token = getToken();
{
    if (token == expected) {
        token = getToken();
    }
  else {
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(listing,"      ");
  }
}

TreeNode * stmt_sequence(void)
{ TreeNode * t = statement();
  TreeNode * p = t;
  while ((token!=ENDFILE) && (token!=END) &&
         (token!=ELSE) && (token!=UNTIL))
  { TreeNode * q;
    match(SEMI);
    q = statement();
    if (q!=NULL) {
      if (t==NULL) t = p = q;
      else /* now p cannot be NULL either */
      { p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}


//P394 
//lineno: 961
TreeNode * statement(void)
{ TreeNode * t = NULL;
  switch (token) {
    case IF : t = if_stmt(); break;
    case REPEAT : t = repeat_stmt(); break;
    case ID : t = assign_stmt(); break;
    case READ : t = read_stmt(); break;
    case WRITE : t = write_stmt(); break;
    default : syntaxError("unexpected token -> ");
              printToken(token,tokenString);
              token = getToken();
              break;
  } /* end case */
  return t;
}


//P394 
//lineno: 977
TreeNode * if_stmt(void)//修改if语句格式
{ TreeNode * t = newStmtNode(IfK);
  match(IF);
  //if (t!=NULL) t->child[0] = exp();
  if (t != NULL) {
      match(LPAREN);//匹配左括号
      t->child[0] = exp();
      match(RPAREN);//匹配右括号
  }
  //match(THEN);
  if (t!=NULL) t->child[1] = stmt_sequence();
  if (token==ELSE) {
    match(ELSE);
    if (t!=NULL) t->child[2] = stmt_sequence();
  }
  //match(END);
  return t;
}

//P394 
//lineno:991
TreeNode * repeat_stmt(void)
{ TreeNode * t = newStmtNode(RepeatK);
  match(REPEAT);
  if (t!=NULL) t->child[0] = stmt_sequence();
  match(UNTIL);
  if (t!=NULL) t->child[1] = exp();
  return t;
}

TreeNode * assign_stmt(void)
{ TreeNode * t = newStmtNode(AssignK);
  if ((t!=NULL) && (token==ID))
    t->attr.name = copyString(tokenString);
  match(ID);
  //判断是否正则表达式语句
  if (token == REG_ASSIGN && t!=NULL) {
      t->kind.stmt = RegularK;
      match(REG_ASSIGN);
      t->child[0] = regular_exp();
  }
  else if (t != NULL) {

        match(ASSIGN);
      t->child[0] = exp();
  }
  return t;
}


TreeNode * read_stmt(void)
{ TreeNode * t = newStmtNode(ReadK);
  match(READ);
  if ((t!=NULL) && (token==ID))
    t->attr.name = copyString(tokenString);
  match(ID);
  return t;
}

TreeNode * write_stmt(void)
{ TreeNode * t = newStmtNode(WriteK);
  match(WRITE);
  if (t!=NULL) t->child[0] = exp();
  return t;
}


TreeNode* regular_exp(void)
{ TreeNode * t = or_exp();
  while ((token==REG_OR)) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = or_exp();
    }
  }
  return t;
}

TreeNode* or_exp(void)
{ TreeNode * t = join_exp();
  while ((token==REG_JOIN)) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = join_exp();
    }
  }
  return t;
}

TreeNode * join_exp(void)
{ TreeNode * t = NULL;
  switch (token) {
    case NUM ://正则表达式中的数字
      t = newExpNode(ConstK);
      if ((t!=NULL) && (token==NUM))
        t->attr.val = atoi(tokenString);
      match(NUM);
      break;
    case ID ://正则表达式中的字母
      t = newExpNode(IdK);
      if ((t!=NULL) && (token==ID))
        t->attr.name = copyString(tokenString);
      match(ID);
      break;
    case LPAREN ://左括号
      match(LPAREN);
      t = regular_exp();
      match(RPAREN);
      break;
    case REG_CLOSURE://正则表达式闭包运算符
        match(REG_CLOSURE);
        t = join_exp();
        break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
    }
  return t;
}


TreeNode * exp(void)
//{ TreeNode * t = simple_exp();
//  if ((token==LT)||(token==EQ)) {
//    TreeNode * p = newExpNode(OpK);
//    if (p!=NULL) {
//      p->child[0] = t;
//      p->attr.op = token;
//      t = p;
//    }
//    match(token);
//    if (t!=NULL)
//      t->child[1] = simple_exp();
//  }
//  return t;
//}
//修改exp，加入逻辑表达式
{ TreeNode * t = logical_exp();
  while ((token==AND)||(token==OR)||(token==NOT)) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = logical_exp();
    }
    //match(token);
    //if (t!=NULL)
    //  t->child[1] = logical_exp();
  }
  return t;
}


//加入逻辑表达式
TreeNode* logical_exp(void)
{ TreeNode * t = simple_exp();
  if ((token==LT)||(token==EQ)||(token==NEQ)||(token==LTE)||(token==GT)||(token==GTE)) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    if (t!=NULL)
      t->child[1] = simple_exp();
  }
  return t;
}


TreeNode * simple_exp(void)
{ TreeNode * t = term();
  while ((token==PLUS)||(token==MINUS))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term();
    }
  }
  return t;
}

TreeNode * term(void)
{ TreeNode * t = factor();
  while ((token==TIMES)||(token==OVER)||(token==POWER)||(token==REMAIN))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}

TreeNode * factor(void)
{ TreeNode * t = NULL;
  switch (token) {
    case NUM :
      t = newExpNode(ConstK);
      if ((t!=NULL) && (token==NUM))
        t->attr.val = atoi(tokenString);
      match(NUM);
      break;
    case ID :
      t = newExpNode(IdK);
      if ((t!=NULL) && (token==ID))
        t->attr.name = copyString(tokenString);
      match(ID);
      break;
    case LPAREN :
      match(LPAREN);
      t = exp();
      match(RPAREN);
      break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
    }
  return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly 
 * constructed syntax tree
 */
TreeNode * parse(void)
{ TreeNode * t;
  token = getToken();
  t = stmt_sequence();
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
