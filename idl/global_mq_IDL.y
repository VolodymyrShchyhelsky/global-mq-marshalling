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



%token KW_MESSAGE
%token KW_HASH_LINE
%token KW_DEFAULT
%token KW_ENUM
%token KW_BYTE_ARRAY

%token KW_NUMERIC KW_INT
%token KW_FILE KW_CLASS_UPP

%token IDENTIFIER
%token KW_IDENTIFIER
%token STRING_LITERAL INTEGER_LITERAL CHAR_LITERAL
%token KW_INTEGER KW_UINTEGER
%token KW_CHARACTER_STRING

%error-verbose
%start file
%destructor { releaseYys($$); } <>

%{
#include "global_mq_parser_helper.h"
extern int yylex();
%}

%%

file : { $$ = 0; }
	| file line_directive { $$ = 0; releaseYys2($1, $2); }
    | file message { $$ = addToFile($1, $2); }
;

line_directive
	: KW_HASH_LINE INTEGER_LITERAL ';' { $$ = 0; processLineDirective($2, 0); releaseYys2($1, $3); }
	| KW_HASH_LINE INTEGER_LITERAL STRING_LITERAL ';' { $$ = 0; processLineDirective($2, $3); releaseYys2($1, $4); }
;

message_begin
	: KW_MESSAGE IDENTIFIER '{' { $$ = createMessage($1, $2); releaseYys($3); }
	| message_begin data_type IDENTIFIER ';' { $$ = addToMessage($1, createAttribute($2, $3)); releaseYys($4); }
;

message
	: message_begin '}' ';' { $$ = $1; releaseYys2($2, $3); }
;

data_type
    : integer_type
    | unsigned_integer_type
/*    | character_string_type*/
    | byte_array_type
	| inline_enum_type
;

integer_type
	: KW_INTEGER { $$ = createIntegerTypeWithNoLimits($1); }
	| KW_INTEGER KW_DEFAULT '=' expr { $$ = createIntegerTypeWithDefaultAndNoLimits($1, $4); releaseYys2($2, $3); }
	| KW_INTEGER '[' expr ',' expr ']' { $$ = createIntegerType($1, true, $3, $5, true); releaseYys3($2, $4, $6); }
	| KW_INTEGER '(' expr ',' expr ']' { $$ = createIntegerType($1, false, $3, $5, true); releaseYys3($2, $4, $6); }
	| KW_INTEGER '[' expr ',' expr ')' { $$ = createIntegerType($1, true, $3, $5, false); releaseYys3($2, $4, $6); }
	| KW_INTEGER '(' expr ',' expr ')' { $$ = createIntegerType($1, false, $3, $5, false); releaseYys3($2, $4, $6); }
	| KW_INTEGER '[' expr ',' expr ']' KW_DEFAULT '=' expr { $$ = createIntegerTypeWithDefault($1, true, $3, $5, true, $9); releaseYys5($2, $4, $6, $7, $8); }
	| KW_INTEGER '(' expr ',' expr ']' KW_DEFAULT '=' expr { $$ = createIntegerTypeWithDefault($1, false, $3, $5, true, $9); releaseYys5($2, $4, $6, $7, $8); }
	| KW_INTEGER '[' expr ',' expr ')' KW_DEFAULT '=' expr { $$ = createIntegerTypeWithDefault($1, true, $3, $5, false, $9); releaseYys5($2, $4, $6, $7, $8); }
	| KW_INTEGER '(' expr ',' expr ')' KW_DEFAULT '=' expr { $$ = createIntegerTypeWithDefault($1, false, $3, $5, false, $9); releaseYys5($2, $4, $6, $7, $8); }
;

unsigned_integer_type
	: KW_UINTEGER { $$ = createUnsignedIntegerTypeWithNoLimits($1); }
	| KW_UINTEGER KW_DEFAULT '=' expr { $$ = createUnsignedIntegerTypeWithDefaultAndNoLimits($1, $4); releaseYys2($2, $3); }
	| KW_UINTEGER '[' expr ',' expr ']' { $$ = createUnsignedIntegerType($1, true, $3, $5, true); releaseYys3($2, $4, $6); }
	| KW_UINTEGER '(' expr ',' expr ']' { $$ = createUnsignedIntegerType($1, false, $3, $5, true); releaseYys3($2, $4, $6); }
	| KW_UINTEGER '[' expr ',' expr ')' { $$ = createUnsignedIntegerType($1, true, $3, $5, false); releaseYys3($2, $4, $6); }
	| KW_UINTEGER '(' expr ',' expr ')' { $$ = createUnsignedIntegerType($1, false, $3, $5, false); releaseYys3($2, $4, $6); }
	| KW_UINTEGER '[' expr ',' expr ']' KW_DEFAULT '=' expr { $$ = createUnsignedIntegerTypeWithDefault($1, true, $3, $5, true, $9); releaseYys5($2, $4, $6, $7, $8); }
	| KW_UINTEGER '(' expr ',' expr ']' KW_DEFAULT '=' expr { $$ = createUnsignedIntegerTypeWithDefault($1, false, $3, $5, true, $9); releaseYys5($2, $4, $6, $7, $8); }
	| KW_UINTEGER '[' expr ',' expr ')' KW_DEFAULT '=' expr { $$ = createUnsignedIntegerTypeWithDefault($1, true, $3, $5, false, $9); releaseYys5($2, $4, $6, $7, $8); }
	| KW_UINTEGER '(' expr ',' expr ')' KW_DEFAULT '=' expr { $$ = createUnsignedIntegerTypeWithDefault($1, false, $3, $5, false, $9); releaseYys5($2, $4, $6, $7, $8); }
;

/*
character_string_type
	: KW_CHARACTER_STRING '{' STRING_LITERAL '}' { $$ = createCharacterStringType($1, $3, 0, 0); releaseYys2($2, $4); }
	| KW_CHARACTER_STRING '{' STRING_LITERAL '}' '[' expr ',' expr ']' { $$ = createCharacterStringType($1, $3, $6, $8); releaseYys5($2, $4, $5, $7, $9); }
	| KW_CHARACTER_STRING '{' character_set '}' { $$ = createCharacterStringType($1, $3, 0, 0); releaseYys2($2, $4); }
	| KW_CHARACTER_STRING '{' character_set '}' '[' expr ',' expr ']' { $$ = createCharacterStringType($1, $3, $6, $8); releaseYys5($2, $4, $5, $7, $9); }
;*/

byte_array_type
	: KW_BYTE_ARRAY '[' expr ']' { $$ = createByteArrayType($1, $3); releaseYys2($2, $4); }
;

inline_enum_type
	: KW_ENUM qname '{' enum_values '}' { $$ = createInlineEnum($1, $2, $4); releaseYys2($3, $5); }
;

enum_values
	: IDENTIFIER '=' INTEGER_LITERAL { $$ = addEnumValue(0, $1, $3); releaseYys($2); }
	| enum_values ',' IDENTIFIER '=' INTEGER_LITERAL { $$ = addEnumValue($1, $3, $5); releaseYys2($2, $4); }
	| KW_IDENTIFIER '(' STRING_LITERAL ')' '=' INTEGER_LITERAL { $$ = addEnumValue(0, $3, $6); releaseYys4($1, $2, $4, $5); }
	| enum_values ',' KW_IDENTIFIER '(' STRING_LITERAL ')' '=' INTEGER_LITERAL { $$ = addEnumValue($1, $5, $8); releaseYys3($2, $3, $4); releaseYys2($6, $7); }
;

expr
	: INTEGER_LITERAL { $$ = $1; }
	| '+' INTEGER_LITERAL { $$ = $2; releaseYys($1); }
	| '-' INTEGER_LITERAL { $$ = makeMinusIntLit($2); releaseYys($1); }
	| STRING_LITERAL { $$ = $1; }
;

qname
	: IDENTIFIER { $$ = $1; }
;