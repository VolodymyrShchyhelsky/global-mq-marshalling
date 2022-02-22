/* -------------------------------------------------------------------------------
* Copyright (c) 2022, OLogN Technologies AG
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

#include "idl_tree_2_csharp.h"
#include <fmt/printf.h>
//#include "idl_tree_serializer.h"

using namespace std;


CsharpWritter CsharpWritter::indent(size_t i)
{
	return {file, currentIndent.size() + i};
}


void CsharpWritter::write(const char* text)
{
	if(!currentIndent.empty() && text && *text != '\n')
		fmt::fprintf(file, currentIndent);
	fmt::fprintf(file, text);
}

void CsharpWritter::write(const char* format, const char* arg0)
{
	if (!currentIndent.empty())
		fmt::fprintf(file, currentIndent);
	fmt::fprintf(file, format, arg0);
}

void CsharpWritter::write(const char* format, const char* arg0, const char* arg1)
{
	if (!currentIndent.empty())
		fmt::fprintf(file, currentIndent);
	fmt::fprintf(file, format, arg0, arg1);
}
void CsharpWritter::write(const char* format, const char* arg0, const char* arg1, const char* arg2)
{
	if (!currentIndent.empty())
		fmt::fprintf(file, currentIndent);
	fmt::fprintf(file, format, arg0, arg1, arg2);
}
void CsharpWritter::write(const char* format, const char* arg0, const char* arg1, const char* arg2, const char* arg3)
{
	if (!currentIndent.empty())
		fmt::fprintf(file, currentIndent);
	fmt::fprintf(file, format, arg0, arg1, arg2, arg3);
}
void CsharpWritter::write(const char* format, const char* arg0, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	if (!currentIndent.empty())
		fmt::fprintf(file, currentIndent);
	fmt::fprintf(file, format, arg0, arg1, arg2, arg3, arg4);
}




void generateCsharp(FILE* file, Root& root, const std::string& metascope)
{
	CsharpWritter f0(file, 0);

	f0.write("//////////////////////////////////////////////////////////////\n");
	f0.write("//\n");
	f0.write("//  Do not edit! file automatically generated by idl_compiler\n");
	f0.write("//\n");
	f0.write("//////////////////////////////////////////////////////////////\n\n");

	f0.write("using globalmq.marshalling;\n");
	f0.write("using System;\n");
	f0.write("using System.Collections.Generic;\n");
	f0.write("using System.Diagnostics;\n");
	f0.write("using System.Linq;\n");
	f0.write("\n");

	if (!metascope.empty())
	{
		f0.write("namespace %s\n", metascope.c_str());
		f0.write("{\n");
		f0.write("\n");
	}

	CsharpWritter f = f0.indent(metascope.empty() ? 0 : 1);


	vector<CompositeType*> structsOrderedByDependency;
	std::unordered_set<size_t> collElementTypes;
	orderStructsByDependency(root.structs, structsOrderedByDependency, collElementTypes);

	f.write("//////////////////////////////////////////////////////////////\n");
	f.write("//\n");
	f.write("//                 Structures\n");
	f.write("//\n");
	f.write("//////////////////////////////////////////////////////////////\n\n");

	for (auto& it : structsOrderedByDependency)
	{
		assert(it != nullptr);
		assert(typeid(*(it)) == typeid(CompositeType));

		if (it->isStruct4Messaging || it->isStruct4Publishing)
		{
			checkCsharpStruct(*it);
			if (it->type == CompositeType::Type::structure)
			{

				std::string type_name = it->name;
				std::string interface_name = "I" + type_name;

				generateCsharpStructInterface(f, *it, type_name.c_str());
				generateCsharpStructImpl(f, *it, type_name.c_str(), interface_name.c_str());
			}
			else if (it->type == CompositeType::Type::discriminated_union)
			{
				generateCsharpUnionInterface(f, *it);
				generateCsharpUnionImpl(f, *it);
			}
			else
				assert(false);
		}
	}

	f.write("//////////////////////////////////////////////////////////////\n");
	f.write("//\n");
	f.write("//                 Messages\n");
	f.write("//\n");
	f.write("//////////////////////////////////////////////////////////////\n\n");

	for (auto& it : structsOrderedByDependency)
	{
		assert(it != nullptr);

		if (it->isStruct4Messaging)
		{
			checkCsharpStruct(*it);
			if (it->type == CompositeType::Type::structure)
			{
				std::string interface_name = "I" + it->name;
				generateCsharpStructMessage(f, *it, it->name.c_str(), interface_name.c_str());
			}
			else if (it->type == CompositeType::Type::discriminated_union)
				generateCsharpUnionMessage(f, *it);
			else
				assert(false);
		}
	}

	impl_insertScopeList(f.getFile(), root);

	for (auto& scope : root.scopes)
	{
		generateCsharpMessageScope(f, root, *scope);
	}

	f.write("//////////////////////////////////////////////////////////////\n");
	f.write("//\n");
	f.write("//                 Publishables\n");
	f.write("//\n");
	f.write("//////////////////////////////////////////////////////////////\n\n");

	for (auto it : structsOrderedByDependency)
	{
		assert(it != nullptr);
		if (it->isStruct4Publishing)
		{
			if (it->type == CompositeType::Type::structure)
			{
				generateCsharpStructSubscriber(f, *it, it->name.c_str());
				generateCsharpStructPublisher(f, *it, it->name.c_str());

			}
			else if (it->type == CompositeType::Type::discriminated_union)
			{
				generateCsharpUnionSubscriber(f, *it, it->name.c_str());
				generateCsharpUnionPublisher(f, *it, it->name.c_str());
			}
			else
				assert(false);
		}
	}

	for (auto& it : root.publishables)
	{
		assert(it != nullptr);
		assert(it->type == CompositeType::Type::publishable);

		checkCsharpStruct(*it);

		std::string type_name = it->name;
		std::string interface_name = "I" + type_name;

		generateCsharpStructInterface(f, *it, type_name.c_str());
		generateCsharpStructImpl(f, *it, type_name.c_str(), interface_name.c_str());

		impl_generatePublishableCommentBlock(f.getFile(), *it);

		generateCsharpStructSubscriber(f, *it, type_name.c_str());
		generateCsharpStructPublisher(f, *it, type_name.c_str());
		generateCsharpStructConcentrator(f, *it, type_name.c_str());
	}

	if (!root.publishables.empty())
		generateCsharpConcentratorFactory(f, root);

	if (!metascope.empty())
	{
		f0.write("\n");
		f0.write("} // namespace %s\n", metascope.c_str());
	}
	f0.write("\n");
}
