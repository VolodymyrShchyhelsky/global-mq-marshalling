/* -------------------------------------------------------------------------------
* Copyright (c) 2020, OLogN Technologies AG
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*	 * Redistributions of source code must retain the above copyright
*	   notice, this list of conditions and the following disclaimer.
*	 * Redistributions in binary form must reproduce the above copyright
*	   notice, this list of conditions and the following disclaimer in the
*	   documentation and/or other materials provided with the distribution.
*	 * Neither the name of the OLogN Technologies AG nor the
*	   names of its contributors may be used to endorse or promote products
*	   derived from this software without specific prior written permission.
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


#include "../include/common.h"

#include "../include/raiistdiofile.h"
#include "../include/idl_tree_serializer.h"

#include "global_mq_parser_helper.h"
#include "../include/parser.h"
#include "lex.h"

using namespace std;

static const bool dbgEnableLeakDetector = true;
static const bool NotImplementedYet = false;

static vector<int> stateStack;

static set<YyBase*> dbgLeakDetector;

static bool errorFlag = false;

static vector<unique_ptr<yy_buffer_state, void(*)(yy_buffer_state*)>> bufferStack;
static vector<pair<string, int> > fileLineStack;

static yy_buffer_state* currentBufferState = 0;
static Root* currentRoot = 0;
static string currentFileName;


struct YyBase {
	Location location;
	YyBase() {
		if (dbgEnableLeakDetector)
			dbgLeakDetector.insert(this);
	}
	virtual ~YyBase() {
		if (dbgEnableLeakDetector)
			dbgLeakDetector.erase(this);
	}
	YyBase(const YyBase&) = delete;
	YyBase& operator=(const YyBase&) = delete;
};


static
void reportError(const Location& loc, const std::string& msg)
{
	errorFlag = true;
	fmt::print(stderr, "{} - {}\n", locationToString(loc), msg);
}

void dbgDumpLeaks()
{
	if (!dbgLeakDetector.empty()) {
		fmt::print(stderr, "Parser nodes leaked\n");
		for (set<YyBase*>::const_iterator it = dbgLeakDetector.begin(); it != dbgLeakDetector.end(); ++it) {
			reportError((*it)->location, typeid(**it).name());
		}
	}
}

template<class T>
struct YyPtr : public YyBase {
	unique_ptr<T> ptr;
	YyPtr(T* ptr) : ptr(ptr) {}
	T* operator->() const {
		return ptr.get();
	}
};

struct YyDataType : public YyBase {
	unique_ptr<MessageParameterType> dataType = unique_ptr<MessageParameterType>(new MessageParameterType());
};

struct YyEnumValues : public YyBase {
	map<string, uint32_t> enumValues;
};

struct YyIdentifierList : public YyBase {
	vector<string> ids;
};

struct YyToken : public YyBase {
	int token;
	std::string text;
	YyToken() : token(0) {}
};

struct YyIdentifier : public YyBase {
	std::string text;
};

struct YyIntegerLiteral : public YyBase {
	long long value;
};

struct YyArgumentList : public YyBase {
	map<string, Variant> arguments;
};

/*struct YyCharacterSet : public YyBase {
	CharacterSet charSet;
};*/


typedef YyIdentifier YyStringLiteral;

/*
	yystype_cast is used to cast YYSTYPE elements from the parser.
	The assert is to be sure the rule are assuming the correct type.
	The parser is not exception safe (plain C).
*/
template<typename T>
T yystype_cast(YYSTYPE yys)
{
	GLOBALMQASSERT(yys);
	T t = dynamic_cast<T>(yys);
	GLOBALMQASSERT(t);
	return t;
}

template<typename T, typename U = T>
T* getPointedFromYyPtr(YYSTYPE yys)
{
	GLOBALMQASSERT(yys);
	YyPtr<U>* t = dynamic_cast<YyPtr<U>*>(yys);
	GLOBALMQASSERT(t);
	T* t2 = dynamic_cast<T*>(t->ptr.get());
	GLOBALMQASSERT(t2);
	return t2;
}

template<typename T>
T* releasePointedFromYyPtr(YYSTYPE yys)
{
	GLOBALMQASSERT(yys);
	YyPtr<T>* t = dynamic_cast<YyPtr<T>*>(yys);
	GLOBALMQASSERT(t);
	return t->ptr.release();
}

static
void setLocation(Location& loc, int line)
{
	loc.fileName = currentFileName;
	loc.lineNumber = line;
}

static
string nameFromYyIdentifier(YyBase* id)
{
	return yystype_cast<YyIdentifier*>(id)->text;
}

static
Variant variantFromExpression(YYSTYPE expr)
{
	if (YyIntegerLiteral* intLit = dynamic_cast<YyIntegerLiteral*>(expr)) {
		Variant v;
		v.kind = Variant::NUMBER;
		v.numberValue = static_cast<double>(intLit->value);
		return v;
	}
	else if (YyStringLiteral* strLit = dynamic_cast<YyStringLiteral*>(expr)) {
		Variant v;
		v.kind = Variant::STRING;
		v.stringValue = strLit->text;
		return v;
	}
	else {
		reportError(expr->location, "Unsuported value");
		return Variant();
	}
}

static
double floatLiteralFromExpression(YYSTYPE expr)
{
	if (YyIntegerLiteral* intLit = dynamic_cast<YyIntegerLiteral*>(expr)) {
		return static_cast<double>(intLit->value);
	}
	else {
		reportError(expr->location, "Unsuported value");
		return 0;
	}
}

static
long long integerLiteralFromExpression(YYSTYPE expr, long long min_value, long long max_value)
{
	if (YyIntegerLiteral* intLit = dynamic_cast<YyIntegerLiteral*>(expr)) {
		if (intLit->value >= min_value && intLit->value <= max_value) {
			return intLit->value;
		}
		else {
			reportError(expr->location, fmt::format("Value outside valid range [{},{}]", min_value, max_value));
			return min_value;
		}
	}
	else {
		reportError(expr->location, "Unsuported value");
		return min_value;
	}
}

static
map<string, Variant> argumentListFromYy(YYSTYPE arg_list)
{
	YyArgumentList* al = yystype_cast<YyArgumentList*>(arg_list);
	map<string, Variant> result = std::move(al->arguments);

	return result;
}

//////////////////////////////////////////////////////////////////////////////
//					Lexer functions											//
//////////////////////////////////////////////////////////////////////////////


void pushState(int state)
{
	stateStack.push_back(state);
}

int popState()
{
	int s = stateStack.back();
	stateStack.pop_back();
	return s;
}


int yywrap()
{
	return 1;
}

void yyerror(const char* msg)
{
	Location l;
	setLocation(l, yylineno);

	reportError(l, msg);
}

void parserError(const char* msg, const char* text, int line)
{
	Location l;
	setLocation(l, line);

	reportError(l, fmt::format(msg, text));
}


void parserErrorUnknownChar(char text, int line)
{
	Location l;
	setLocation(l, line);

	reportError(l, fmt::format("Unknown char '0x{:x}'", static_cast<unsigned char>(text)));
}


void releaseYys(YYSTYPE yys)
{
	delete yys;
}

void releaseYys2(YYSTYPE yys0, YYSTYPE yys1)
{
	delete yys0;
	delete yys1;
}

void releaseYys3(YYSTYPE yys0, YYSTYPE yys1, YYSTYPE yys2)
{
	delete yys0;
	delete yys1;
	delete yys2;
}

void releaseYys4(YYSTYPE yys0, YYSTYPE yys1, YYSTYPE yys2, YYSTYPE yys3)
{
	delete yys0;
	delete yys1;
	delete yys2;
	delete yys3;
}

void releaseYys5(YYSTYPE yys0, YYSTYPE yys1, YYSTYPE yys2, YYSTYPE yys3, YYSTYPE yys4)
{
	delete yys0;
	delete yys1;
	delete yys2;
	delete yys3;
	delete yys4;
}

void releaseYys6(YYSTYPE yys0, YYSTYPE yys1, YYSTYPE yys2, YYSTYPE yys3, YYSTYPE yys4, YYSTYPE yys5)
{
	delete yys0;
	delete yys1;
	delete yys2;
	delete yys3;
	delete yys4;
	delete yys5;
}

YYSTYPE createYyToken(const char* text, int line, int token)
{
	YyToken* yy = new YyToken();
	setLocation(yy->location, line);
	yy->token = token;
	yy->text = text;

	return yy;
}

YYSTYPE createIdentifier(const char* text, int line)
{
	YyIdentifier* yy = new YyIdentifier();
	setLocation(yy->location, line);
	yy->text = text;
	return yy;
}

YYSTYPE createIntegerLiteral(const char* text, int line)
{
	YyIntegerLiteral* yy = new YyIntegerLiteral();

	setLocation(yy->location, line);
	yy->value =  atoll(text);

	return yy;
}

YYSTYPE createHexIntegerLiteral(const char* text, int line)
{
	parserError("Hex integer literal not implemente yet!", text, line);

	return 0;
}


YYSTYPE createZeroLiteral(const char* text, int line)
{
	return createIntegerLiteral(text, line);
}


YYSTYPE createStringLiteral(const char* text, int line)
{
	string t = text;
	if(t.size() >= 2)
		t = t.substr(1, t.size() - 2);

	YyStringLiteral* yy = new YyStringLiteral();
	setLocation(yy->location, line);
	yy->text = t;
	return yy;
}


//////////////////////////////////////////////////////////////////////////////

YYSTYPE addToFile(YYSTYPE file, YYSTYPE item)
{
	GLOBALMQASSERT(!file);
	unique_ptr<YyBase> d0(item);

	Message* s = releasePointedFromYyPtr<Message>(item);
	currentRoot->messages.push_back(unique_ptr<Message>(s));

	return 0;
}

void processLineDirective(YYSTYPE line_number, YYSTYPE file_name)
{
	unique_ptr<YyBase> d0(line_number);
	unique_ptr<YyBase> d1(file_name);

	YyIntegerLiteral* intLit = yystype_cast<YyIntegerLiteral*>(line_number);
	if (intLit->value > 0 && intLit->value < INT_MAX) {
		yylineno = static_cast<int>(intLit->value - 1);

		if (file_name)
		{
			YyStringLiteral* strLit = yystype_cast<YyStringLiteral*>(file_name);
			currentFileName = strLit->text;
		}
	}
}


YYSTYPE addToMessage(YYSTYPE decl, YYSTYPE attr)
{
	unique_ptr<YyBase> d0(decl);
	unique_ptr<YyBase> d1(attr);

	Message* yy = getPointedFromYyPtr<Message>(decl);

	MessageParameter* a = releasePointedFromYyPtr<MessageParameter>(attr);

	yy->members.push_back(unique_ptr<MessageParameter>(a));

	return d0.release();
}

static
MessageParameter* makeDataMember(YYSTYPE type, YYSTYPE id)
{
	MessageParameter* yy = new MessageParameter();
	yy->location = id->location;
	yy->name = nameFromYyIdentifier(id);
	yy->extendTo = false;

	YyDataType* dt = yystype_cast<YyDataType*>(type);
	yy->type = std::move(*(dt->dataType));

	return yy;
}

YYSTYPE createAttribute(YYSTYPE type, YYSTYPE id)
{
	unique_ptr<YyBase> d0(type);
	unique_ptr<YyBase> d1(id);

	MessageParameter* att = makeDataMember(type, id);

	return new YyPtr<MessageParameter>(att);
}

YYSTYPE createMessage(YYSTYPE token, YYSTYPE id)
{
	unique_ptr<YyBase> d0(token);
	unique_ptr<YyBase> d1(id);

	Message* yy = new Message();

	yy->location = id->location;
	yy->name = nameFromYyIdentifier(id);

	return new YyPtr<Message>(yy);
}

static
string unquote(string data) {

	for (auto it = data.begin(); it != data.end(); ++it) {
		if (*it == '\\') {
			it = data.erase(it);
			if (it != data.end())
				++it;
		}
	}
	return data;
}




YyDataType* createIntegerTypeImplBase(YYSTYPE token, bool isSigned)
{
	YyDataType* yy = new YyDataType();

	yy->dataType->kind = isSigned ? MessageParameterType::UINTEGER : MessageParameterType::INTEGER;

	return yy;
}

void setDefaultValueForInteger(YYSTYPE token, YyDataType* yy, YYSTYPE default_expr)
{
	double h = floatLiteralFromExpression(default_expr);
	yy->dataType->numericalDefault = h;
	yy->dataType->hasDefault = true;

	bool withinLow = ( !yy->dataType->hasLimits ) || ( yy->dataType->lowLimit.inclusive ? yy->dataType->lowLimit.value <= yy->dataType->numericalDefault : yy->dataType->lowLimit.value < yy->dataType->numericalDefault );
	if (!withinLow)
		reportError(token->location, "Default value cannot be less than low limit");

	bool withinHigh = ( !yy->dataType->hasLimits ) || ( yy->dataType->highLimit.inclusive ? yy->dataType->highLimit.value >= yy->dataType->numericalDefault : yy->dataType->highLimit.value > yy->dataType->numericalDefault );
	if (!withinLow)
		reportError(token->location, "Default value cannot be greater than high limit");
}

void setLimitsForInteger(YYSTYPE token, YyDataType* yy, bool low_flag, YYSTYPE low_expr, YYSTYPE high_expr, bool high_flag)
{
	yy->dataType->hasLimits = true;

	double l = floatLiteralFromExpression(low_expr);
	yy->dataType->lowLimit.inclusive = low_flag;
	yy->dataType->lowLimit.value = l;

	double h = floatLiteralFromExpression(high_expr);
	yy->dataType->highLimit.inclusive = high_flag;
	yy->dataType->highLimit.value = h;

	if (!(yy->dataType->lowLimit.value < yy->dataType->highLimit.value))
		reportError(token->location, "Low limit must be less than high limit");
}


YYSTYPE createIntegerTypeWithNoLimits(YYSTYPE token)
{
	unique_ptr<YyBase> d0(token);

	return createIntegerTypeImplBase(token, true);
}

YYSTYPE createIntegerTypeWithDefaultAndNoLimits(YYSTYPE token, YYSTYPE default_expr)
{
	unique_ptr<YyBase> d0(token);
	unique_ptr<YyBase> d3(default_expr);

	YyDataType* yy = createIntegerTypeImplBase(token, true);
	setDefaultValueForInteger(token, yy, default_expr);
	return yy;
}

YYSTYPE createUnsignedIntegerTypeWithDefaultAndNoLimits(YYSTYPE token, YYSTYPE default_expr)
{
	unique_ptr<YyBase> d0(token);
	unique_ptr<YyBase> d3(default_expr);

	YyDataType* yy = createIntegerTypeImplBase(token, false);
	setDefaultValueForInteger(token, yy, default_expr);
	return yy;
}

YYSTYPE createUnsignedIntegerTypeWithNoLimits(YYSTYPE token)
{
	unique_ptr<YyBase> d0(token);

	return createIntegerTypeImplBase(token, false);
}

YYSTYPE createIntegerType(YYSTYPE token, bool low_flag, YYSTYPE low_expr, YYSTYPE high_expr, bool high_flag)
{
	unique_ptr<YyBase> d0(token);
	unique_ptr<YyBase> d1(low_expr);
	unique_ptr<YyBase> d2(high_expr);

	YyDataType* yy = createIntegerTypeImplBase(token, true);
	setLimitsForInteger(token, yy, low_flag, low_expr, high_expr, high_flag);
	return yy;
}

YYSTYPE createIntegerTypeWithDefault(YYSTYPE token, bool low_flag, YYSTYPE low_expr, YYSTYPE high_expr, bool high_flag, YYSTYPE default_expr )
{
	unique_ptr<YyBase> d0(token);
	unique_ptr<YyBase> d1(low_expr);
	unique_ptr<YyBase> d2(high_expr);
	unique_ptr<YyBase> d3(default_expr);

	YyDataType* yy = createIntegerTypeImplBase(token, true);
	setLimitsForInteger(token, yy, low_flag, low_expr, high_expr, high_flag);
	setDefaultValueForInteger(token, yy, default_expr);

	return yy;
}

YYSTYPE createUnsignedIntegerType(YYSTYPE token, bool low_flag, YYSTYPE low_expr, YYSTYPE high_expr, bool high_flag)
{
	unique_ptr<YyBase> d0(token);
	unique_ptr<YyBase> d1(low_expr);
	unique_ptr<YyBase> d2(high_expr);

	YyDataType* yy = createIntegerTypeImplBase(token, false);
	setLimitsForInteger(token, yy, low_flag, low_expr, high_expr, high_flag);
	return yy;
}

YYSTYPE createUnsignedIntegerTypeWithDefault(YYSTYPE token, bool low_flag, YYSTYPE low_expr, YYSTYPE high_expr, bool high_flag, YYSTYPE default_expr )
{
	unique_ptr<YyBase> d0(token);
	unique_ptr<YyBase> d1(low_expr);
	unique_ptr<YyBase> d2(high_expr);
	unique_ptr<YyBase> d3(default_expr);

	YyDataType* yy = createIntegerTypeImplBase(token, false);
	setLimitsForInteger(token, yy, low_flag, low_expr, high_expr, high_flag);
	setDefaultValueForInteger(token, yy, default_expr);

	return yy;
}

YYSTYPE createCharacterStringType(YYSTYPE token, YYSTYPE charset, YYSTYPE min_expr, YYSTYPE max_expr)
{
	unique_ptr<YyBase> d0(token);
	unique_ptr<YyBase> d1(charset);
	unique_ptr<YyBase> d2(min_expr);
	unique_ptr<YyBase> d3(max_expr);

	YyDataType* yy = new YyDataType();

	yy->dataType->kind = MessageParameterType::CHARACTER_STRING;
//	yy->dataType->characterSet = getCharacterSet(charset);

	if (min_expr)
	{
		yy->dataType->stringMinSize = static_cast<uint32_t>(integerLiteralFromExpression(min_expr, 0, UINT32_MAX));
		yy->dataType->stringMaxSize = static_cast<uint32_t>(integerLiteralFromExpression(max_expr, yy->dataType->stringMinSize, UINT32_MAX));
	}

	return yy;
}

YYSTYPE createByteArrayType(YYSTYPE token, YYSTYPE size_expr)
{
	unique_ptr<YyBase> d0(token);
	unique_ptr<YyBase> d1(size_expr);

	YyDataType* yy = new YyDataType();

	yy->dataType->kind = MessageParameterType::BYTE_ARRAY;

	if (size_expr)
		yy->dataType->arrayFixedaxSize = static_cast<uint32_t>(integerLiteralFromExpression(size_expr, 1, UINT32_MAX));

	return yy;
}

YYSTYPE createInlineEnum(YYSTYPE token, YYSTYPE opt_id, YYSTYPE values)
{
	unique_ptr<YyBase> d0(token);
	unique_ptr<YyBase> d1(opt_id);
	unique_ptr<YyBase> d2(values);

	YyDataType* yy = new YyDataType();

	yy->dataType->kind = MessageParameterType::ENUM;
	if (opt_id)
		yy->dataType->name = nameFromYyIdentifier(opt_id);

	YyEnumValues* v = yystype_cast<YyEnumValues*>(values);
	yy->dataType->enumValues = v->enumValues;

	return yy;
}

YYSTYPE addEnumValue(YYSTYPE list, YYSTYPE id, YYSTYPE int_lit)
{
	unique_ptr<YyBase> d0(list);
	unique_ptr<YyBase> d1(id);
	unique_ptr<YyBase> d2(int_lit);

	YyEnumValues* yy = 0;
	if (list)
		yy = yystype_cast<YyEnumValues*>(list);
	else
	{
		yy = new YyEnumValues();
		d0.reset(yy);
	}

	string name = nameFromYyIdentifier(id);
	uint32_t value = static_cast<uint32_t>(integerLiteralFromExpression(int_lit, 0, UINT32_MAX));

	yy->enumValues.emplace(name, value);

	return d0.release();
}


YYSTYPE addIdentifier(YYSTYPE list, YYSTYPE id)
{
	unique_ptr<YyBase> d0(list ? list : new YyIdentifierList());
	unique_ptr<YyBase> d1(id);

	YyIdentifierList* yy = yystype_cast<YyIdentifierList*>(d0.get());

	string value = nameFromYyIdentifier(id);
	yy->ids.push_back(value);

	return d0.release();
}


YYSTYPE addExpression(YYSTYPE list, YYSTYPE id, YYSTYPE expr)
{
	unique_ptr<YyBase> d0(list ? list : new YyArgumentList());
	unique_ptr<YyBase> d1(id);
	unique_ptr<YyBase> d2(expr);

	YyArgumentList* yy = yystype_cast<YyArgumentList*>(d0.get());

	string name = nameFromYyIdentifier(id);
	Variant value = variantFromExpression(expr);
	yy->arguments.emplace(name, value);

	return d0.release();
}

YYSTYPE makeMinusIntLit(YYSTYPE int_lit)
{
	YyIntegerLiteral* yy = yystype_cast<YyIntegerLiteral*>(int_lit);
	yy->value = -(yy->value);
	return yy;
}


//////////////////////////////////////////////////////////////////////////////

class State {
private:
	yy_buffer_state* bufferState;
	string fileName;
	int lineNumber;
	Root* root;
public:
	State(yy_buffer_state* nextBufferState, const string& nextFileName,
		  int nextLineNumber, Root* nextRoot) :
		bufferState(currentBufferState),
		fileName(currentFileName),
		lineNumber(yylineno),
		root(currentRoot) {
		currentBufferState = nextBufferState;
		currentFileName = nextFileName;
		yylineno = nextLineNumber;
		currentRoot = nextRoot;

		yy_switch_to_buffer(currentBufferState);
	}

	~State() {
		currentBufferState = bufferState;
		currentFileName = fileName;
		yylineno = lineNumber;
		currentRoot = root;

		if (currentBufferState)
			yy_switch_to_buffer(currentBufferState);
	}

	typedef unique_ptr<yy_buffer_state, void(*)(yy_buffer_state*)> BufferOwner;
};


static
void parseSourceFileInternal()
{
	int err = yyparse();

	if (errorFlag | (err != 0) )
		throw ParserException(fmt::format("Errors found while parsing file '{}'.", currentFileName));
}

Root* parseSourceFile(const string& fileName, bool debugDump)
{
	yydebug = static_cast<int>(debugDump);

	if (fileName.empty())
		throw ParserException("Missing input file name");

	RaiiStdioFile file(fopen(fileName.c_str(), "r"));

	if (!file)
		throw ParserException(fmt::format("Failed to open file '{}'.", fileName));

	State::BufferOwner buff(yy_create_buffer(file, FILE_BUFFER_SIZE), &yy_delete_buffer);

	if (!buff)
		throw ParserException(fmt::format("Failed to allocate read buffer for file '{}'.", fileName));

	unique_ptr<Root> root(new Root());

	State st(buff.get(), fileName, 1, root.get());
	parseSourceFileInternal();

	return root.release();
}

//////////////////////////////////////////////////////////////////////////////