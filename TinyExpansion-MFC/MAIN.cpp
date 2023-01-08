//#pragma once
#include"pch.h"
/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

//#ifndef _MAIN_H_
//#define _MAIN_H_
//
//
#include "globals.h"
#include"MAIN.H"
//#include "util.h"
#include "scan.h"
//#include "parse.h"
//#include "analyze.h"
//#include "cgen.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
//#define NO_ANALYZE FALSE
#define NO_ANALYZE TRUE//设置只生成语法树

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif


int indentno;
int lineno;
int EOF_flag;
int linepos;
int bufsize;

//extern int location;
//extern int indentno;
//extern int EOF_flag;
//extern int linepos;
//extern int bufsize;


/* allocate global variables */
//static int lineno = 0;
//static FILE * source;
//static FILE * listing;
//static FILE * code;
//extern int lineno;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
//int TraceParse = FALSE;
int TraceParse = TRUE;//输出语法树
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;

void freem(TreeNode* freen) {
    if (freen == nullptr) {
        return;
    }
    for (TreeNode* c : freen->child) {
        freem(c);
    }
    if (freen != nullptr) {
        free(freen->sibling);
    }
    free(freen);
}

//int main( int argc, char * argv[] )
void solve()
{ 
    //清空缓存
    //location = 0;
    indentno = 0;
    lineno = 0;
    EOF_flag = FALSE;
    linepos = 0;
    bufsize = 0;


    TreeNode * syntaxTree;
  char pgm[120]; /* source code file name */
  //if (argc != 2)
  //  { fprintf(stderr,"usage: %s <filename>\n",argv[0]);
  //    exit(1);
  //  }
  //strcpy(pgm,argv[1]) ;
  strcpy(pgm, "source.txt");//设置源文件文件名
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".tny");
  source = fopen(pgm,"r");
  if (source==NULL)
  { fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }
  //listing = stdout; /* send listing to screen */
  listing = fopen("SyntaxTree.txt", "w");//将输出从stdout改为文件名
  //fprintf(listing,"\nTINY COMPILATION: %s\n",pgm);
#if NO_PARSE
  while (getToken()!=ENDFILE);
#else
  syntaxTree = parse();
  if (TraceParse) {
    fprintf(listing,"\nSyntax tree:\n");
    printTree(syntaxTree);//输出语法树
  }
#if !NO_ANALYZE
  if (! Error)
  { if (TraceAnalyze) fprintf(listing,"\nBuilding Symbol Table...\n");
    buildSymtab(syntaxTree);
    if (TraceAnalyze) fprintf(listing,"\nChecking Types...\n");
    typeCheck(syntaxTree);
    if (TraceAnalyze) fprintf(listing,"\nType Checking Finished\n");
  }
#if !NO_CODE
  if (! Error)
  { char * codefile;
    int fnlen = strcspn(pgm,".");
    codefile = (char *) calloc(fnlen+4, sizeof(char));
    strncpy(codefile,pgm,fnlen);
    strcat(codefile,".tm");
    code = fopen(codefile,"w");
    if (code == NULL)
    { printf("Unable to open %s\n",codefile);
      exit(1);
    }
    codeGen(syntaxTree,codefile);
    fclose(code);
  }
#endif
#endif
#endif
  fclose(source);
  fclose(listing);//要关闭才有输出
  //return 0;
  freem(syntaxTree);
}

//#endif
