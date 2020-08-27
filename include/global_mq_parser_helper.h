/* -------------------------------------------------------------------------------
* Copyright (c) 2020, OLogN Technologies AG
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the OLogN Technologies AG nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL OLogN Technologies AG BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* -------------------------------------------------------------------------------*/


/*
This header is used to isolate lex/yacc parser from rest of the code.
It is necesary because of lex.yy.c and parse.tab.c being pure C code,
while the rest of the project is C++.
Also leaves function calls only at scan.l and parse.y files, and
all code goes inside parser_helper.cpp
*/

#ifndef PARSER_HELPER_H_INCLUDED
#define PARSER_HELPER_H_INCLUDED

struct YyBase;
typedef struct YyBase* YYSTYPE;
#define YYSTYPE_IS_DECLARED 1

void pushState(int state);
int popState();

extern "C" int yywrap();
void yyerror(const char* msg);

void parserError(const char* msg, const char* text, int line);
void parserErrorUnknownChar(char text, int line);

void releaseYys(YYSTYPE yys);
void releaseYys2(YYSTYPE yys0, YYSTYPE yys1);
void releaseYys3(YYSTYPE yys0, YYSTYPE yys1, YYSTYPE yys2);
void releaseYys4(YYSTYPE yys0, YYSTYPE yys1, YYSTYPE yys2, YYSTYPE yys3);
void releaseYys5(YYSTYPE yys0, YYSTYPE yys1, YYSTYPE yys2, YYSTYPE yys3, YYSTYPE yys4);
void releaseYys6(YYSTYPE yys0, YYSTYPE yys1, YYSTYPE yys2, YYSTYPE yys3, YYSTYPE yys4, YYSTYPE yys5);

YYSTYPE createYyToken(const char* text, int line, int token);

YYSTYPE createIdentifier(const char* text, int line);
YYSTYPE createIntegerLiteral(const char* text, int line);
YYSTYPE createHexIntegerLiteral(const char* text, int line);
YYSTYPE createStringLiteral(const char* text, int line);
YYSTYPE createZeroLiteral(const char* text, int line);

YYSTYPE addToFile(YYSTYPE file, YYSTYPE item);

void processLineDirective(YYSTYPE line_number, YYSTYPE file_name);

YYSTYPE addToMessage(YYSTYPE decl, YYSTYPE attr);

YYSTYPE createAttribute(YYSTYPE type, YYSTYPE id);

YYSTYPE createMessage(YYSTYPE token, YYSTYPE id);

YYSTYPE createIntegerTypeWithNoLimits(YYSTYPE token);
YYSTYPE createUnsignedIntegerTypeWithNoLimits(YYSTYPE token);
YYSTYPE createUnsignedIntegerTypeWithDefaultAndNoLimits(YYSTYPE token, YYSTYPE default_expr);
YYSTYPE createIntegerTypeWithDefaultAndNoLimits(YYSTYPE token, YYSTYPE default_expr);
YYSTYPE createIntegerType(YYSTYPE token, bool low_flag, YYSTYPE low_expr, YYSTYPE high_expr, bool high_flag);
YYSTYPE createIntegerTypeWithDefault(YYSTYPE token, bool low_flag, YYSTYPE low_expr, YYSTYPE high_expr, bool high_flag, YYSTYPE default_expr);
YYSTYPE createUnsignedIntegerType(YYSTYPE token, bool low_flag, YYSTYPE low_expr, YYSTYPE high_expr, bool high_flag);
YYSTYPE createUnsignedIntegerTypeWithDefault(YYSTYPE token, bool low_flag, YYSTYPE low_expr, YYSTYPE high_expr, bool high_flag, YYSTYPE default_expr);

YYSTYPE createCharacterStringType(YYSTYPE token, YYSTYPE charset, YYSTYPE min_expr, YYSTYPE max_expr);
YYSTYPE createByteArrayType(YYSTYPE token, YYSTYPE size_expr);

YYSTYPE createInlineEnum(YYSTYPE token, YYSTYPE opt_id, YYSTYPE values);
YYSTYPE addEnumValue(YYSTYPE list, YYSTYPE id, YYSTYPE int_lit);

YYSTYPE addIdentifier(YYSTYPE list, YYSTYPE id);
YYSTYPE addExpression(YYSTYPE list, YYSTYPE id, YYSTYPE expr);
YYSTYPE makeMinusIntLit(YYSTYPE int_lit);

#endif // PARSER_HELPER_H_INCLUDED