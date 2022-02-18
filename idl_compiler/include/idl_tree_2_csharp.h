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

#ifndef IDL_TREE_2_CSHARP_H
#define IDL_TREE_2_CSHARP_H

#include "idl_tree.h"
#include "idl_tree_serializer.h"

/// small wrapper to migrate from old fprintf to fmt::fprintf, and add basic auto-indent
class CsharpFileWritter
{
	FILE* file = nullptr;
	std::string currentIndent;

public:
	CsharpFileWritter(FILE* file, size_t initialIndent) : file(file), currentIndent(initialIndent, '\t') {}

	void increment();
	void decrement();
	// mb: make variadic only if real necesity
	void writeBr(char brace);
	void write(const char* text);
	void write(const char* format, const char* arg0);
	void write(const char* format, const char* arg0, const char* arg1);
	void write(const char* format, const char* arg0, const char* arg1, const char* arg2);
	void write(const char* format, const char* arg0, const char* arg1, const char* arg2, const char* arg3);
	void write(const char* format, const char* arg0, const char* arg1, const char* arg2, const char* arg3, const char* arg4);
};


//shared by message and publishables

void checkCsharpStruct(CompositeType& s);
std::string getCSharpTypeName(CompositeType& s);
const char* getCSharpPrimitiveType(MessageParameterType::KIND kind);
const char* getIdlPrimitiveType(MessageParameterType::KIND kind);
const char* getIdlPrimitiveType2(MessageParameterType::KIND kind);

std::string generateCsharpDeclParams(CompositeType& s);
std::string generateCsharpCallerParams(CompositeType& s, bool valPrefix);

void generateCsharpStandardMethods(FILE* header, const char* type_name);
void generateCsharpStructEquivalentExpression(CsharpFileWritter& f, CompositeType& s);
void generateCsharpStructEquivalentMethod(FILE* header, CompositeType& s, const char* type_name);
void generateCsharpSimpleEquivalentMethod(FILE* header, const char* type_name, const char* member_name);
void generateCsharpInterfaceMember(FILE* header, MessageParameter& member);
void generateCsharpStructInterface(FILE* header, CompositeType& s, const char* type_name);
void generateCsharpStructImpl(FILE* header, CompositeType& s, const char* type_name, const char* interface_name);
void generateCsharpStructMessage(FILE* header, CompositeType& s, const char* type_name, const char* interface_name);
void generateCsharpUnionInterface(FILE* header, CompositeType& s);
void generateCsharpUnionImpl(FILE* header, CompositeType& s);
void generateCsharpUnionMessage(FILE* header, CompositeType& s);
void generateCsharpSubscriberFactoryMethod(FILE* header, MessageParameter& member);
void generateCsharpSubscriberEventHandler(FILE* header, MessageParameter& member);
void generateCsharpSubscriberMember(FILE* header, MessageParameter& member);
void generateCsharpPublisherMember(FILE* header, MessageParameter& member);
void generateCsharpPublisherCompose(FILE* header, CompositeType& s);
void generateCsharpSubscriberParseForStateSync(FILE* header, CompositeType& s);
void generateCsharpSubscriberParse1(FILE* header, CompositeType& s);
void generateCsharpSubscriberParse2(FILE* header, CompositeType& s);
void generateCsharpUnionSubscriber(FILE* header, CompositeType& s, const char* type_name);
void generateCsharpUnionPublisher(FILE* header, CompositeType& s, const char* type_name);

// code generation
void generateCsharp(FILE* header, Root& root, const std::string& metascope);
void generateCsharpMessages(FILE* header, Root& root, const std::string& metascope);
void generateCsharpPublishables(FILE* header, Root& root, const std::string& metascope);




#endif // IDL_TREE_2_CSHARP_H