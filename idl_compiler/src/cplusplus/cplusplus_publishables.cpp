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

#include "cplusplus_idl_tree_serializer.h"
#include "idl_tree_common.h"
#include "file_writter.h"
namespace cplusplus
{
// const char* paramTypeToParser( MessageParameterType::KIND kind )
// {
// 	switch( kind )
// 	{
// 		case MessageParameterType::KIND::INTEGER: return "parseSignedInteger";
// 		case MessageParameterType::KIND::UINTEGER: return "parseUnsignedInteger";
// 		case MessageParameterType::KIND::REAL: return "parseReal";
// 		case MessageParameterType::KIND::CHARACTER_STRING: return "parseString";
// 		default: return nullptr;
// 	}
// }


string impl_parameterTypeToDescriptiveString( Root& s, const MessageParameterType& type )
{
	switch ( type.kind )
	{
		case MessageParameterType::KIND::INTEGER: return "INTEGER";
		case MessageParameterType::KIND::UINTEGER: return "UINTEGER";
		case MessageParameterType::KIND::REAL: return "REAL";
		case MessageParameterType::KIND::CHARACTER_STRING: return "CHARACTER_STRING";
		case MessageParameterType::KIND::BYTE_ARRAY: return "BYTE_ARRAY";
		case MessageParameterType::KIND::BLOB: return "BLOB";

		case MessageParameterType::KIND::STRUCT: 
			assert( type.structIdx <= s.structs.size() );
			return fmt::format( "STRUCT {}", s.structs[type.structIdx]->name );
		case MessageParameterType::KIND::DISCRIMINATED_UNION: 
			assert( type.structIdx <= s.structs.size() );
			return fmt::format( "DISCRIMINATED_UNION {}", s.structs[type.structIdx]->name );
		case MessageParameterType::KIND::VECTOR: 
			if ( type.vectorElemKind == MessageParameterType::KIND::STRUCT )
			{
				assert( type.structIdx <= s.structs.size() );
				return fmt::format( "VECTOR<STRUCT {}>", s.structs[type.structIdx]->name );
			}
			else if ( type.vectorElemKind == MessageParameterType::KIND::DISCRIMINATED_UNION )
			{
				assert( type.structIdx <= s.structs.size() );
				return fmt::format( "VECTOR<DISCRIMINATED_UNION {}>", s.structs[type.structIdx]->name );
			}
			else if ( type.vectorElemKind == MessageParameterType::KIND::ENUM )
				return fmt::format( "VECTOR<ENUM {}>", type.name );
			else
				return fmt::format( "VECTOR<{}>", impl_kindToString( type.vectorElemKind ) );
		case MessageParameterType::KIND::DICTIONARY: 
			if ( type.dictionaryValueKind == MessageParameterType::KIND::STRUCT )
			{
				assert( type.structIdx <= s.structs.size() );
				return fmt::format( "DICTIONARY<{}, STRUCT {}>", impl_kindToString( type.dictionaryKeyKind ), s.structs[type.structIdx]->name );
			}
			else if ( type.dictionaryValueKind == MessageParameterType::KIND::DISCRIMINATED_UNION )
			{
				assert( type.structIdx <= s.structs.size() );
				return fmt::format( "DICTIONARY<{}, DISCRIMINATED_UNION {}>", impl_kindToString( type.dictionaryKeyKind ), s.structs[type.structIdx]->name );
			}
			else if ( type.dictionaryValueKind == MessageParameterType::KIND::ENUM )
				return fmt::format( "DICTIONARY<{}, ENUM {}>", impl_kindToString( type.dictionaryKeyKind ), type.name );
			else
				return fmt::format( "DICTIONARY<{}, {}>", impl_kindToString( type.dictionaryKeyKind ), impl_kindToString( type.dictionaryValueKind ) );

		case MessageParameterType::KIND::ENUM: return fmt::format( "ENUM {}", type.name );
		case MessageParameterType::KIND::UNDEFINED: return "UNDEFINED";
		case MessageParameterType::KIND::EXTENSION: return "EXTENSION";
		default: assert( false ); return "";
	}
}


void impl_GeneratePublishableStateMemberGetter( FILE* header, Root& root, CompositeType& s, MessageParameter& param )
{
	assert( s.type == CompositeType::Type::publishable || ( ( s.type == CompositeType::Type::structure || s.type == CompositeType::Type::discriminated_union_case ) && s.isStruct4Publishing ) );
	if ( param.type.isNumericType() )
		fprintf( header, "\tauto get_%s() { return t.%s; }\n", param.name.c_str(), impl_memberOrAccessFunctionName( param ).c_str() );
	else if ( param.type.kind == MessageParameterType::KIND::VECTOR )
	{
		if ( param.type.vectorElemKind == MessageParameterType::KIND::STRUCT || param.type.vectorElemKind == MessageParameterType::KIND::DISCRIMINATED_UNION )
		{
			assert( root.structs.size() > param.type.structIdx );
			fprintf( header, "\tauto get_%s() { return globalmq::marshalling2::VectorOfStructRefWrapper<%s, %s_RefWrapper>(t.%s); }\n", 
				param.name.c_str(),  getVectorElementProcessor(param.type).c_str(), root.structs[param.type.structIdx]->name.c_str(), impl_memberOrAccessFunctionName( param ).c_str() );
		}
		else
			fprintf( header, "\tauto get_%s() { return globalmq::marshalling2::VectorOfSimpleTypeRefWrapper<%s>(t.%s); }\n", param.name.c_str(), getVectorElementProcessor(param.type).c_str(), impl_memberOrAccessFunctionName( param ).c_str() );
	}
	else if ( param.type.kind == MessageParameterType::KIND::DICTIONARY )
	{
		if ( param.type.dictionaryValueKind == MessageParameterType::KIND::STRUCT || param.type.dictionaryValueKind == MessageParameterType::KIND::DISCRIMINATED_UNION )
		{
			assert( root.structs.size() > param.type.structIdx );
			fprintf( header, "\tauto get_%s() { return globalmq::marshalling2::DictionaryOfStructRefWrapper<%s, %s_RefWrapper>(t.%s); }\n", 
				param.name.c_str(), getDictionaryKeyValueProcessor(param.type).c_str(), root.structs[param.type.structIdx]->name.c_str(), impl_memberOrAccessFunctionName( param ).c_str() );
		}
		else
			fprintf( header, "\tauto get_%s() { return globalmq::marshalling2::DictionaryOfSimpleTypeRefWrapper<%s>(t.%s); }\n", param.name.c_str(), getDictionaryKeyValueProcessor(param.type).c_str(), impl_memberOrAccessFunctionName( param ).c_str() );
	}
	else
		fprintf( header, "\tconst auto& get_%s() { return t.%s; }\n", param.name.c_str(), impl_memberOrAccessFunctionName( param ).c_str() );
}

void impl_GeneratePublishableStateMemberGetter4Set( FILE* header, Root& root, const char* rootTypeNameBase, MessageParameter& param, size_t idx )
{
	string rootType;
	string addr;
	string rootObjName;

	string idxStr = std::to_string(idx);

	if ( rootTypeNameBase == nullptr )
	{
		rootType = "RootT";
		addr = "address";
		rootObjName = "root";
	}
	else
	{
		rootType = fmt::format( "{}_WrapperForPublisher", rootTypeNameBase );
		addr = "GMQ_COLL vector<uint64_t>()";
		rootObjName = "*this";
	}
	if ( param.type.kind == MessageParameterType::KIND::STRUCT )
	{
		assert( param.type.structIdx < root.structs.size() );
//		fprintf( header, "\tauto get4set_%s() { return %s_RefWrapper4Set</*aaa*/%s, %s>(t.%s, *this, %s, %zd); }\n", 
//			param.name.c_str(), root.structs[param.type.structIdx]->name.c_str(), impl_templateMemberTypeName( "T", param ).c_str(), rootType.c_str(), impl_memberOrAccessFunctionName( param ).c_str(), addr.c_str(), idx );
		fprintf( header, "\tauto get4set_%s() { return %s_RefWrapper4Set<%s>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
			param.name.c_str(), root.structs[param.type.structIdx]->name.c_str(), rootType.c_str(), impl_memberOrAccessFunctionName( param ).c_str(), rootObjName.c_str(), addr.c_str(), idxStr.c_str() );
	}
	else if ( param.type.kind == MessageParameterType::KIND::DISCRIMINATED_UNION ) // TODO: revise DU
	{
		assert( param.type.structIdx < root.structs.size() );
		fprintf( header, "\tauto get4set_%s() { return %s_RefWrapper4Set<%s>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
			param.name.c_str(), root.structs[param.type.structIdx]->name.c_str(), rootType.c_str(), impl_memberOrAccessFunctionName( param ).c_str(), rootObjName.c_str(), addr.c_str(), idxStr.c_str() );
	}
	else if ( param.type.kind == MessageParameterType::KIND::VECTOR )
	{
		// const char* libType = paramTypeToLibType( param.type.vectorElemKind );
		// assert( libType != nullptr );
		switch( param.type.vectorElemKind )
		{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
				fprintf( header, "\tauto get4set_%s() { return globalmq::marshalling2::VectorRefWrapper4Set<%s, %s>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
					param.name.c_str(), getVectorElementProcessor(param.type).c_str(), rootType.c_str(), impl_memberOrAccessFunctionName( param ).c_str(), rootObjName.c_str(), addr.c_str(), idxStr.c_str() );
				break;
			case MessageParameterType::KIND::STRUCT:
			case MessageParameterType::KIND::DISCRIMINATED_UNION: // TODO: revise DU (lib kw: VectorOfStructRefWrapper4Set and around)
				fprintf( header, 
					"\tauto get4set_%s() { return globalmq::marshalling2::VectorOfStructRefWrapper4Set<%s, %s, %s_RefWrapper4Set<%s>>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
					param.name.c_str(),
					getVectorElementProcessor(param.type).c_str(), 
					rootType.c_str(), 
					param.type.name.c_str(), 
					rootType.c_str(), 
					impl_memberOrAccessFunctionName( param ).c_str(), 
					rootObjName.c_str(),
					addr.c_str(), idxStr.c_str() );
				break;
			default:
				assert( false ); // unexpected or not (yet) implemented
		}
	}
	else if ( param.type.kind == MessageParameterType::KIND::DICTIONARY )
	{
		// std::string libKeyType = dictionaryKeyTypeToLibTypeOrTypeProcessor( param.type, root );
		// std::string libValueType = dictionaryValueTypeToLibTypeOrTypeProcessor( param.type, root );
		switch( param.type.dictionaryValueKind )
		{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
				fprintf( header, "\tauto get4set_%s() { return globalmq::marshalling2::DictionaryRefWrapper4Set<%s, %s>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
					param.name.c_str(), getDictionaryKeyValueProcessor(param.type).c_str(), rootType.c_str(), impl_memberOrAccessFunctionName( param ).c_str(), rootObjName.c_str(), addr.c_str(), idxStr.c_str() );
				break;
			case MessageParameterType::KIND::STRUCT:
			case MessageParameterType::KIND::DISCRIMINATED_UNION:
				assert( param.type.structIdx < root.structs.size() );
				fprintf( header, 
					"\tauto get4set_%s() { return globalmq::marshalling2::DictionaryOfStructRefWrapper4Set<%s, %s, %s_RefWrapper4Set<%s>>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
					param.name.c_str(),
					getDictionaryKeyValueProcessor(param.type).c_str(), 
					rootType.c_str(), 
					param.type.name.c_str(), 
					rootType.c_str(), 
					impl_memberOrAccessFunctionName( param ).c_str(), 
					rootObjName.c_str(),
					addr.c_str(), idxStr.c_str() );
				break;
			default:
				assert( false ); // unexpected or not (yet) implemented
		}
	}
}


void impl_generateComposeFunctionForPublishableStruct_MemberIterationBlock( FILE* header, Root& root, CompositeType& obj, const char* offset )
{
	assert( obj.type != CompositeType::Type::discriminated_union );

	// const char* composer = "composer";

	for ( size_t i=0; i<obj.getMembers().size(); ++i )
	{
		auto& it = obj.getMembers()[i];
		assert( it != nullptr );
		auto& member = *it;

		if( i != 0)
		{
			fprintf( header, "\n" );
			fprintf( header, "%scomposer.nextElement();\n", offset );
			fprintf( header, "\n" );
		}

		fprintf( header, "%scomposer.namedParamBegin( \"%s\" );\n", offset, member.name.c_str() );
		fprintf( header, "%s%s::compose(composer, t.%s );\n", offset, getTypeProcessor(member.type).c_str(), impl_memberOrAccessFunctionName( member ).c_str());
	}
}

void impl_generateComposeFunctionForPublishableStruct( FILE* header, Root& root, CompositeType& obj, const std::string& composerType )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::message ||
			obj.type == CompositeType::Type::publishable );

	std::string typeName = getGeneratedTypeName(obj);
	fprintf( header, "\tstatic\n" );
	fprintf( header, "\tvoid compose( %s& composer, const %s& t )\n", composerType.c_str(), typeName.c_str() );
	fprintf( header, "\t{\n" );

	// if ( obj.type == CompositeType::Type::structure || obj.type == CompositeType::Type::discriminated_union )
	// {
	// 	fprintf( header, "\tstatic\n" );
	// 	fprintf( header, "\tvoid compose( %s& composer, const %s& t )\n", composerType.c_str(), getGeneratedTypeName(obj).c_str() );
	// 	fprintf( header, "\t{\n" );
	// }
	// else if ( obj.type == CompositeType::Type::publishable )
	// {
	// 	fprintf( header, "\ttemplate<class ComposerType>\n" );
	// 	fprintf( header, "\tvoid compose( ComposerType& composer )\n" );
	// 	fprintf( header, "\t{\n" );
	// 	// fprintf( header, "\t\t::globalmq::marshalling::impl::composeStructBegin( composer );\n" );
	// 	// fprintf( header, "\n" );
	// }
	// else
	// 	assert( false );


	fprintf( header, "\t\tcomposer.structBegin();\n" );
	fprintf( header, "\n" );

	if ( obj.isDiscriminatedUnion() )
	{
		fprintf( header, "\t\tuint64_t caseId = t.currentVariant();\n" );
		// fprintf( header, "\t\t::globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, caseId, \"caseId\", true );\n" );
		fprintf( header, "\t\tcomposer.namedParamBegin( \"caseId\" );\n" );
		fprintf( header, "\t\tcomposer.composeUnsignedInteger( caseId );\n" );

		fprintf( header, "\n" );
		fprintf( header, "\t\tcomposer.nextElement();\n" );
		fprintf( header, "\n" );

		fprintf( header, "\t\tif ( caseId != %s::Variants::unknown )\n", typeName.c_str() );
		fprintf( header, "\t\t{\n" );
		// fprintf( header, "\t\t\t::globalmq::marshalling::impl::composePublishableStructBegin( composer, \"caseData\" );\n" );
		fprintf( header, "\t\t\tcomposer.namedParamBegin( \"caseData\" );\n" );
		fprintf( header, "\t\t\tcomposer.structBegin();\n" );
		fprintf( header, "\t\t\tswitch ( caseId )\n" );
		fprintf( header, "\t\t\t{\n" );
		for ( auto& it: obj.getDiscriminatedUnionCases() )
		{
			std::string numId = std::to_string(it->numID);
			fprintf( header, "\t\t\t\tcase %s: // IDL CASE %s\n", numId.c_str(), it->name.c_str() );
			fprintf( header, "\t\t\t\t{\n" );
			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_generateComposeFunctionForPublishableStruct_MemberIterationBlock( header, root, cs, "\t\t\t\t\t" );
			fprintf( header, "\t\t\t\t\tbreak;\n" );
			fprintf( header, "\t\t\t\t}\n" );
		}
		fprintf( header, "\t\t\t\tdefault:\n" );
		fprintf( header, "\t\t\t\t\tthrow std::exception(); // unexpected\n" );
		fprintf( header, "\t\t\t}\n" );
		fprintf( header, "\t\t\tcomposer.structEnd();\n" );

		fprintf( header, "\t\t}\n" );
	}
	else
		impl_generateComposeFunctionForPublishableStruct_MemberIterationBlock( header, root, obj, "\t\t" );


	// if ( obj.type == CompositeType::Type::publishable )
	// {
	// 	fprintf( header, "\n" );
	// 	fprintf( header, "\t\t::globalmq::marshalling::impl::composeStructEnd( composer );\n" );
	// }
	fprintf( header, "\n" );
	fprintf( header, "\t\tcomposer.structEnd();\n" );

	fprintf( header, "\t}\n" );
}


void impl_generateParseFunctionForPublishableStructStateSync_MemberIterationBlock( FILE* header, Root& root, CompositeType& obj, std::string offset, const std::string& parserType )
{
	assert( obj.type != CompositeType::Type::discriminated_union );

	std::string templParent = getGeneratedTypeName(obj);

	for ( size_t i=0; i<obj.getMembers().size(); ++i )
	{
		assert( obj.getMembers()[i] != nullptr );
		auto& member = *(obj.getMembers()[i]);

		if(i != 0)
			fprintf( header, "%sparser.nextElement();\n\n", offset.c_str() );
			

		fprintf( header, "%sparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s%s::parseForStateSyncOrMessageInDepth( parser, t.%s );\n", offset.c_str(), getTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	}
}

void impl_generateParseFunctionBodyForPublishableStructStateSyncOrMessageInDepth( FILE* header, Root& root, CompositeType& obj, const std::string& parserType )
{
	assert( obj.type == CompositeType::Type::message ||
			obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::publishable );

	std::string typeName = getGeneratedTypeName(obj);
	if ( obj.isDiscriminatedUnion() )
	{
		fprintf( header, "\t\tparser.namedParamBegin( \"caseId\" );\n");
		fprintf( header, "\t\tuint64_t caseId = parser.parseUnsignedInteger();\n" );
		fprintf( header, "\t\tt.initAs( (typename %s::Variants)(caseId) );\n", typeName.c_str() );
		fprintf( header, "\t\tparser.nextElement();\n");
		fprintf( header, "\t\tif ( caseId != %s::Variants::unknown )\n", typeName.c_str() );
		fprintf( header, "\t\t{\n" );

		fprintf( header, "\t\t\tparser.namedParamBegin( \"caseData\" );\n");
		fprintf( header, "\t\t\tparser.structBegin();\n" );
		fprintf( header, "\t\t\tswitch ( caseId )\n" );
		fprintf( header, "\t\t\t{\n" );

		for ( auto& it: obj.getDiscriminatedUnionCases() )
		{
			std::string numId = std::to_string(it->numID);
			fprintf( header, "\t\t\t\tcase %s: // IDL CASE %s\n", numId.c_str(), it->name.c_str() );
			fprintf( header, "\t\t\t\t{\n" );

			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_generateParseFunctionForPublishableStructStateSync_MemberIterationBlock( header, root, cs, "\t\t\t\t\t", parserType );

			fprintf( header, "\t\t\t\t\tbreak;\n" );
			fprintf( header, "\t\t\t\t}\n" );
		}

		fprintf( header, "\t\t\t\tdefault:\n" );
		fprintf( header, "\t\t\t\t\tthrow std::exception(); // unexpected\n" );
		fprintf( header, "\t\t\t}\n" );
		fprintf( header, "\t\t\tparser.structEnd();\n" );

		fprintf( header, "\t\t}\n" );
	}
	else
		impl_generateParseFunctionForPublishableStructStateSync_MemberIterationBlock( header, root, obj, "\t\t", parserType );
}

void impl_generateParseFunctionForPublishableStructStateSyncOrMessageInDepth( FILE* header, Root& root, CompositeType& obj, const std::string& parserType )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::message ||
			obj.type == CompositeType::Type::publishable );

	fprintf( header, "\tstatic\n" );
	fprintf( header, "\tvoid parseForStateSyncOrMessageInDepth( %s& parser, %s& t )\n", parserType.c_str(), getGeneratedTypeName(obj).c_str() );
	fprintf( header, "\t{\n" );
	fprintf( header, "\t\tparser.structBegin();\n" );
	fprintf( header, "\n" );

	impl_generateParseFunctionBodyForPublishableStructStateSyncOrMessageInDepth( header, root, obj, parserType );

	fprintf( header, "\n" );
	fprintf( header, "\t\tparser.structEnd();\n" );

	fprintf( header, "\t}\n" );
}



void impl_generateParseFunctionForPublishableState( FILE* header, Root& root, CompositeType& obj, bool addFullUpdateNotifierBlock, const std::string& parserType )
{
	assert( obj.type == CompositeType::Type::publishable );
	fprintf( header, "\tvoid parseStateSyncMessage( %s& parser )\n", parserType.c_str());
	fprintf( header, "\t{\n" );

//	fprintf( header, "\t\t%s::parseForStateSyncOrMessageInDepth( parser, t );\n", getHelperClassName(obj).c_str() );

	if ( addFullUpdateNotifierBlock )
	{
		fprintf( header, "\n" );
		fprintf( header, "\t\tif constexpr ( has_full_update_notifier )\n" );
		fprintf( header, "\t\t\tt.notifyFullyUpdated();\n" );
	}
	fprintf( header, "\t}\n" );
}

void impl_generatePublishableStructForwardDeclaration( FILE* header, Root& root, CompositeType& obj )
{
	// if ( obj.isDiscriminatedUnion() )
	// 	fprintf( header, "class %s;\n", getHelperClassName( obj ).c_str() );
	// else
		fprintf( header, "struct %s;\n", getHelperClassName( obj ).c_str() );
}

void impl_GeneratePublishableStructCopyFn_MemberIterationBlock( FILE* header, Root& root, CompositeType& s, std::string offset )
{
	assert( s.type != CompositeType::Type::discriminated_union );

	for ( size_t i=0; i<s.getMembers().size(); ++i )
	{
		assert( s.getMembers()[i] != nullptr );
		auto& member = *(s.getMembers()[i]);

		fprintf( header, "%s%s::copy( src.%s, dst.%s );\n", offset.c_str(), getTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );

	}
}

void impl_GeneratePublishableStructCopyFn( FILE* header, Root& root, CompositeType& s )
{
	fprintf( header, "\ttemplate<typename UserT>\n" );
	fprintf( header, "\tstatic void copy(const UserT& src, UserT& dst) {\n" );

	if ( s.isDiscriminatedUnion() )
	{
		fprintf( header, "\t\tauto srcCaseId = src.currentVariant();\n" );
		fprintf( header, "\t\tdst.initAs( srcCaseId );\n" );

		fprintf( header, "\t\tif ( srcCaseId != UserT::Variants::unknown )\n" );
		fprintf( header, "\t\t\tswitch ( srcCaseId )\n" );
		fprintf( header, "\t\t\t{\n" );

		for ( auto& it: s.getDiscriminatedUnionCases() )
		{
			std::string numId = std::to_string(it->numID);
			fprintf( header, "\t\t\t\tcase %s: // IDL CASE %s\n", numId.c_str(), it->name.c_str() );
			fprintf( header, "\t\t\t\t{\n" );

			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_GeneratePublishableStructCopyFn_MemberIterationBlock( header, root, cs, "\t\t\t\t\t" );

			fprintf( header, "\t\t\t\t\tbreak;\n" );
			fprintf( header, "\t\t\t\t}\n" );
		}

		fprintf( header, "\t\t\t\tdefault:\n" );
		fprintf( header, "\t\t\t\t\tthrow std::exception(); // unexpected\n" );
		fprintf( header, "\t\t\t}\n" );
	}
	else
		impl_GeneratePublishableStructCopyFn_MemberIterationBlock( header, root, s, "\t\t" );

	fprintf( header, "\t}\n" );
}

void impl_GeneratePublishableStructIsSameFn_MemberIterationBlock( FILE* header, Root& root, CompositeType& s, std::string offset )
{
	assert( s.type != CompositeType::Type::discriminated_union );

	for ( size_t i=0; i<s.getMembers().size(); ++i )
	{
		assert( s.getMembers()[i] != nullptr );
		auto& member = *(s.getMembers()[i]);

		fprintf( header, "%sif( ! %s::isSame( s1.get_%s(), s2.get_%s() ) ) return false;\n", offset.c_str(), getTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	}
}

void impl_GeneratePublishableStructIsSameFn( FILE* header, Root& root, CompositeType& s )
{
	fprintf( header, "\ttemplate<typename UserT1, typename UserT2>\n" );
	fprintf( header, "\tstatic bool isSame(const UserT1& s1, const UserT2& s2) {\n" );

	if ( s.isDiscriminatedUnion() )
	{
		fprintf( header, "\t\tif ( static_cast<uint64_t>(s1.currentVariant()) != static_cast<uint64_t>(s2.currentVariant()) )\n" );
		fprintf( header, "\t\t\treturn false;\n" );

		fprintf( header, "\t\tif ( s1.currentVariant() != UserT1::Variants::unknown )\n" );
		fprintf( header, "\t\t\tswitch ( s1.currentVariant() )\n" );
		fprintf( header, "\t\t\t{\n" );

		for ( auto& it: s.getDiscriminatedUnionCases() )
		{
			std::string numId = std::to_string(it->numID);
			fprintf( header, "\t\t\t\tcase %s: // IDL CASE %s\n", numId.c_str(), it->name.c_str() );
			fprintf( header, "\t\t\t\t{\n" );

			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_GeneratePublishableStructIsSameFn_MemberIterationBlock( header, root, cs, "\t\t\t\t\t" );

			fprintf( header, "\t\t\t\t\tbreak;\n" );
			fprintf( header, "\t\t\t\t}\n" );
		}

		fprintf( header, "\t\t\t\tdefault:\n" );
		fprintf( header, "\t\t\t\t\tthrow std::exception(); // unexpected\n" );
		fprintf( header, "\t\t\t}\n" );
	}
	else
		impl_GeneratePublishableStructIsSameFn_MemberIterationBlock( header, root, s, "\t\t" );

	fprintf( header, "\t\treturn true;\n" );
	fprintf( header, "\t}\n" );
}

void impl_generatePublishableStruct( FILE* header, Root& root, CompositeType& obj, const GenerationConfig& config )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::message ||
			obj.type == CompositeType::Type::publishable );

	fprintf( header, "struct %s : public ::globalmq::marshalling::impl::StructType\n", getHelperClassName( obj ).c_str() );
	fprintf( header, "{\n" );

	fprintf( header, "\tusing CppType = %s;\n", getGeneratedTypeName(obj).c_str() );
	

	for(auto& each : config.parserNames)
		impl_generateParseFunctionForPublishableStructStateSyncOrMessageInDepth( header, root, obj, each );
	fprintf( header, "\n" );

	for (auto& each : config.composerNames)
	{
		impl_generateComposeFunctionForPublishableStruct(header, root, obj, each);
		fprintf(header, "\n");
	}

	if ( obj.isStruct4Publishing || obj.type == CompositeType::Type::publishable )
	{
		// for (auto& each : config.parserNames)
		// {
		// 	impl_generateHelperParseStateSync( header, root, obj, each );
		// 	fprintf(header, "\n");

		// 	impl_generateParseFunctionForPublishableStruct(header, root, obj, each);
		// 	fprintf(header, "\n");

		// 	impl_generateContinueParsingFunctionForPublishableStruct(header, root, obj, each);
		// 	fprintf(header, "\n");
		// }
		impl_GeneratePublishableStructCopyFn( header, root, obj );
		fprintf( header, "\n" );
		impl_GeneratePublishableStructIsSameFn( header, root, obj );
	}

	fprintf( header, "};\n\n" );
}

void impl_GeneratePublishableStateMemberSetter( FILE* header, Root& root, bool forRoot, MessageParameter& param, size_t idx )
{
//	assert( (forRoot && rootName != nullptr) || (forRoot == false && rootName == nullptr) );
	const char* composer = forRoot ? "composer" : "root.getComposer()";
	const char* composerT = forRoot ? "ComposerT" : "decltype(root.getComposer())";
	const char* addrVector = forRoot ? "GMQ_COLL vector<uint64_t>()" : "address";

	fprintf( header, "\tvoid set_%s( %s val) { \n", param.name.c_str(), impl_templateMemberTypeName( "T", param ).c_str() );
	fprintf( header, "\t\tt.%s = val; \n", impl_memberOrAccessFunctionName( param ).c_str() );

	std::string idxStr = std::to_string(idx);
	fprintf( header, "\t\t%s.changeBegin( %s, %s );\n", composer, addrVector, idxStr.c_str() );
	fprintf( header, "\t\t%s.nextElement();\n", composer );
	fprintf( header, "\t\t%s.leafeBegin();\n", composer );
	fprintf( header, "\t\t%s::compose(%s, t.%s );\n", getTypeProcessor(param.type).c_str(), composer, impl_memberOrAccessFunctionName( param ).c_str() );
	fprintf( header, "\t\t%s.changeEnd();\n", composer );
	
	fprintf( header, "\t}\n" );
}

void impl_GeneratePublishableStateMemberAccessors_MemberIterationBlock( FILE* header, Root& root, CompositeType& s, bool forRoot, const char* rootName, bool allowSeters, size_t idxBase )
{
	assert( s.type != CompositeType::Type::discriminated_union );
	for ( size_t i=0; i<s.getMembers().size(); ++i )
	{
		auto& it = s.getMembers()[i];
		assert( it != nullptr );
		impl_GeneratePublishableStateMemberGetter( header, root, s, *it );
		if ( allowSeters )
		{
			impl_GeneratePublishableStateMemberSetter( header, root, forRoot, *it, idxBase + i);
			impl_GeneratePublishableStateMemberGetter4Set( header, root, rootName, *it, idxBase + i );
		}
	}
}

void impl_GeneratePublishableStateMemberAccessors( FILE* header, Root& root, CompositeType& s, bool allowSeters )
{
	assert( s.type == CompositeType::Type::publishable || s.type == CompositeType::Type::structure || s.isDiscriminatedUnion() );
	bool forRoot = s.type == CompositeType::Type::publishable;
	const char* rootName = forRoot ? s.name.c_str() : nullptr;
//	const char* rootName = forRoot ? s.name.c_str() : "RootT";
	if ( s.isDiscriminatedUnion() )
	{
		fprintf( header, "\tauto get_currentVariant() { return t.currentVariant(); }\n" );
		if ( allowSeters )
		{
			const char* composer = forRoot ? "composer" : "root.getComposer()";
			const char* composerT = forRoot ? "ComposerT" : "decltype(root.getComposer())";
			const char* addrVector = forRoot ? "GMQ_COLL vector<uint64_t>()" : "address";
			fprintf( header, "\tvoid set_currentVariant( typename T::Variants v ) { \n" );
			fprintf( header, "\t\tt.initAs( v ); \n" );

			std::string idxStr = std::to_string(0);
			fprintf( header, "\t\t%s.changeBegin( %s, %s );\n", composer, addrVector, idxStr.c_str() );
			fprintf( header, "\t\t%s.nextElement();\n", composer );
			fprintf( header, "\t\t%s.leafeBegin();\n", composer );
			fprintf( header, "\t\t%s.composeUnsignedInteger((uint64_t)(t.currentVariant()));\n", composer );
			fprintf( header, "\t\t%s.changeEnd();\n", composer );
			fprintf( header, "\t}\n" );
		}

		for ( auto& it: s.getDiscriminatedUnionCases() )
		{
			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_GeneratePublishableStateMemberAccessors_MemberIterationBlock( header, root, cs,  forRoot, rootName,  allowSeters, 1 );
		}
	}
	else
		impl_GeneratePublishableStateMemberAccessors_MemberIterationBlock( header, root, s,  forRoot, rootName,  allowSeters, 0 );
}

void impl_GenerateApplyUpdateMessageMemberFn( FILE* header, Root& root, CompositeType& s, bool addNotifications, const std::string& parserType )
{
	fprintf( header, "\tvoid applyMessageWithUpdates(%s& parser)\n", parserType.c_str() );
	fprintf( header, "\t{\n" );
	fprintf( header, "\t\tparser.stateUpdateBegin();\n" );
	fprintf( header, "\t\tbool changed = false;\n" );
	fprintf( header, "\t\tGMQ_COLL vector<uint64_t> addr;\n" );
	fprintf( header, "\t\twhile( parser.changeBegin( addr ) )\n" );
	fprintf( header, "\t\t{\n" );
	fprintf( header, "\t\t\tGMQ_ASSERT( addr.size() );\n" );

	assert( s.type != CompositeType::Type::discriminated_union ); // not implemented

	std::string structName = getHelperClassName(s);
//	fprintf( header, "\t\t\t%s::parse(parser, t, addr, 0);\n", structName.c_str() );

	fprintf( header, "\t\t\taddr.clear();\n" );
	fprintf( header, "\t\t\tparser.changeEnd();\n" );
	fprintf( header, "\t\t}\n" );
	fprintf( header, "\t\tparser.stateUpdateEnd();\n" );

	if ( addNotifications )
	{
		fprintf( header, "\t\tif constexpr ( has_update_notifier )\n" );
		fprintf( header, "\t\t{\n" );
		fprintf( header, "\t\t\tif ( changed )\n" );
		fprintf( header, "\t\t\t\tt.notifyUpdated();\n" );
		fprintf( header, "\t\t}\n" );
	}

	fprintf( header, "\t}\n" );
}

void impl_GeneratePublishableStateWrapperForPublisher( FILE* header, Root& root, CompositeType& s, const GenerationConfig& config )
{
	assert( s.type == CompositeType::Type::publishable );

	fprintf( header, "template<class ComposerT>\n" );
	fprintf( header, "class %s_WrapperForPublisher : public globalmq::marshalling::StatePublisherBase<ComposerT>\n", s.name.c_str() );
	fprintf( header, "{\n" );
	fprintf( header, "\tusing BufferT = typename ComposerT::BufferType;\n" );
	fprintf( header, "\tusing RegistryT = globalmq::marshalling::PublisherRegistryBase<ComposerT>;\n" );
	fprintf( header, "\tusing T = %s;\n", getGeneratedTypeName(s).c_str() );
	fprintf( header, "\tRegistryT* publishableRegistry = nullptr;\n" );
	fprintf( header, "\tBufferT buffer;\n" );
	fprintf( header, "\tComposerT composer;\n" );
	fprintf( header, "\tT t;\n" );

	// impl_GeneratePublishableStateMemberPresenceCheckingBlock( header, root, s );
//	impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock( header, root, s, "\t" );

	fprintf( header, "\npublic:\n" );
	fprintf( header, "\tstatic constexpr uint64_t numTypeID = %lld;\n", s.numID );
	fprintf( header, "\tstatic constexpr const char* stringTypeID = \"%s\";\n", s.name.c_str() );
	fprintf( header, "\n" );
	// fprintf( header, "\ttemplate<class ... ArgsT>\n" );
	// fprintf( header, "\t%s_WrapperForPublisher( ArgsT&& ... args ) : t( std::forward<ArgsT>( args )... ), composer( buffer ) {}\n", s.name.c_str() );
	fprintf( header, "\t%s_WrapperForPublisher() : composer( buffer ) {}\n", s.name.c_str() );
	fprintf( header, "\t%s_WrapperForPublisher( T arg ) : t( std::move( arg ) ), composer( buffer ) {}\n", s.name.c_str() );
	fprintf( header, "\t%s_WrapperForPublisher( RegistryT* registry ) : publishableRegistry(registry), composer( buffer ) { if(publishableRegistry) { publishableRegistry->addPublisher(this); } }\n", s.name.c_str() );
	fprintf( header, "\t%s_WrapperForPublisher( RegistryT* registry, T arg ) : publishableRegistry(registry), t( std::move( arg ) ), composer( buffer ) { if(publishableRegistry) { publishableRegistry->addPublisher(this); } }\n", s.name.c_str() );
	fprintf( header, "\tvirtual ~%s_WrapperForPublisher() { if(publishableRegistry) { publishableRegistry->removePublisher(this); } }\n", s.name.c_str() );
	fprintf( header, "\tconst T& getState() { return t; }\n" );
	fprintf( header, "\tComposerT& getComposer() { return composer; }\n" );
	fprintf( header, "\tvirtual void startTick( BufferT&& buff ) override { buffer = std::move( buff ); composer.reset(); composer.stateUpdateBegin(); }\n" );
	fprintf( header, "\tvirtual BufferT&& endTick() override { composer.stateUpdateEnd(); return std::move( buffer ); }\n" );
	fprintf( header, "\tvirtual const char* publishableName() override { return stringTypeID; }\n" );
	fprintf( header, "\tvirtual uint64_t stateTypeID() override { return numTypeID; }\n" );
 	fprintf( header, "\tvirtual void generateStateSyncMessage(ComposerT& composer) override { compose(composer); }\n" );

	impl_GeneratePublishableStateMemberAccessors( header, root, s, true );
	fprintf( header, "\n" );

	for (auto& each : config.composerNames)
		fprintf( header, "\tvoid compose( %s& composer ) { %s::compose( composer, t ); }\n", each.c_str(), getHelperClassName(s).c_str() );



	fprintf( header, "};\n\n" );
}



void impl_GeneratePublishableStructWrapperForwardDeclaration( FILE* header, Root& root, CompositeType& s )
{
	assert( s.type == CompositeType::Type::structure || s.type == CompositeType::Type::discriminated_union );

	fprintf( header, "class %s_RefWrapper;\n", s.name.c_str() );
}

void impl_GeneratePublishableStructWrapper( FILE* header, Root& root, CompositeType& s )
{
	assert( s.type == CompositeType::Type::structure || s.type == CompositeType::Type::discriminated_union );

	fprintf( header, "class %s_RefWrapper\n", s.name.c_str() );
	fprintf( header, "{\n" );
	fprintf( header, "\tusing T = %s;\n", getGeneratedTypeName(s).c_str() );
	fprintf( header, "\tT& t;\n" );

	// impl_GeneratePublishableStateMemberPresenceCheckingBlock( header, root, s );
//	impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock( header, root, s, "\t" );

	fprintf( header, "\npublic:\n" );
	fprintf( header, "\t%s_RefWrapper( T& actual ) : t( actual ) {}\n", s.name.c_str() );

	impl_GeneratePublishableStateMemberAccessors( header, root, s, false );

	fprintf( header, "};\n\n" );
}

void impl_GeneratePublishableStructWrapper4SetForwardDeclaration( FILE* header, Root& root, CompositeType& s )
{
	assert( s.type == CompositeType::Type::structure || s.type == CompositeType::Type::discriminated_union );

	fprintf( header, "template<class RootT> class %s_RefWrapper4Set;\n", s.name.c_str() );
}

void impl_GeneratePublishableStructWrapper4Set( FILE* header, Root& root, CompositeType& s )
{
	assert( s.type == CompositeType::Type::structure || s.type == CompositeType::Type::discriminated_union );

	fprintf( header, "template<class RootT>\n" );
	fprintf( header, "class %s_RefWrapper4Set\n", s.name.c_str() );
	fprintf( header, "{\n" );
	fprintf( header, "\tusing T = %s;\n", getGeneratedTypeName(s).c_str() );
	fprintf( header, "\tT& t;\n" );
	fprintf( header, "\tRootT& root;\n" );
	fprintf( header, "\tGMQ_COLL vector<uint64_t> address;\n" );

	// impl_GeneratePublishableStateMemberPresenceCheckingBlock( header, root, s );

	fprintf( header, "\npublic:\n" );
	fprintf( header, "\t%s_RefWrapper4Set( T& actual, RootT& root_, GMQ_COLL vector<uint64_t>&& address_ )\n", s.name.c_str() );
	fprintf( header, "\t\t: t( actual ), root( root_ ), address( std::move(address_) )\n" );
	fprintf( header, "\t\t{ }\n" );

	impl_GeneratePublishableStateMemberAccessors( header, root, s, true );

	fprintf( header, "};\n\n" );
}


void generatePublishable( FILE* header, Root& root, CompositeType& s, const GenerationConfig& config )
{
	bool checked = impl_checkParamNameUniqueness(s);
	checked = impl_checkFollowingExtensionRules(s) && checked;
	if ( !checked )
		throw std::exception();

	assert( ( config.platformPrefix.size() == 0 && config.classNotifierName.size() == 0 ) || ( config.platformPrefix.size() != 0 && config.classNotifierName.size() != 0 ) );
	bool generatePlatformSpec = config.platformPrefix.size() != 0 && config.classNotifierName.size() != 0;

	impl_generatePublishableCommentBlock( header, s );

	impl_GeneratePublishableStateWrapperForPublisher( header, root, s, config );
	// if ( generatePlatformSpec )
	// 	impl_GeneratePublishableStatePlatformWrapperForPublisher( header, root, s, config.platformPrefix, config.classNotifierName );

	// impl_GeneratePublishableStateWrapperForSubscriber( header, root, s, config );
	// if ( generatePlatformSpec )
	// 	impl_GeneratePublishableStatePlatformWrapperForSubscriber( header, root, s, config.platformPrefix, config.classNotifierName );

	// impl_GeneratePublishableStateWrapperForConcentrator( header, root, s, config );
}

             //////////////////////////////////////////
////////////// new subscriber model below this line //////////////
            //////////////////////////////////////////

void impl_generateApplyUpdateForSimpleType2( FileWritter f, MessageParameter& member, const std::string& parserType )
{
	f.write("\tauto oldVal = this->%s;\n", impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\tthis->%s = %s::parse(parser);\n", impl_memberOrAccessFunctionName(member).c_str(), getTypeProcessor(member.type).c_str());
	f.write("\tbool currentChanged = oldVal != this->%s;\n", impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\tif ( currentChanged )\n" );
	f.write("\t{\n" );
	f.write("\t\tchanged = true;\n" );
	f.write("\t\tthis->notifyUpdated_%s( oldVal );\n", member.name.c_str() );
	f.write("\t}\n" );
}

void impl_generateApplyUpdateForStructItself2( FileWritter f, MessageParameter& member, const std::string& parserType )
{
	f.write("{\n" );
	f.write("\tbool changedCurrent = %s::parse_notify( parser, this->%s_lazy() );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\tif ( changedCurrent )\n" );
	f.write("\t{\n" );
	f.write("\t\tchanged = true;\n" );
	f.write("\t\tthis->notifyUpdated_%s();\n", member.name.c_str() );
	f.write("\t}\n" );
	f.write("}\n" );
}


void impl_generateApplyUpdateForFurtherProcessingInStruct2( FileWritter f, MessageParameter& member, const std::string& parserType )
{
	f.write("{\n" );
	f.write("\tbool changedCurrent = %s::parse_notify( parser, this->%s_lazy() );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\tif ( changedCurrent )\n" );
	f.write("\t{\n" );
	f.write("\t\t\tchanged = true;\n" );
	f.write("\t\tthis->notifyUpdated_%s();\n", member.name.c_str() );
	f.write("\t}\n" );
	f.write("}\n" );
}


void impl_generateApplyUpdateForFurtherProcessingInStruct3( FileWritter f, MessageParameter& member, const std::string& parserType )
{
	f.write("bool changedCurrent = %s::parse_continue( parser, this->%s_lazy(), addr, offset + 1 );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("if ( changedCurrent )\n" );
	f.write("{\n" );
	f.write("\tchanged = true;\n" );
	f.write("\tthis->notifyUpdated_%s();\n", member.name.c_str() );
	f.write("}\n" );
}

void impl_generateApplyUpdateForFurtherProcessingInDictionary2( FileWritter f, Root& root, MessageParameter& member, const std::string& parserType )
{

	bool simpleType = member.type.dictionaryValueKind == MessageParameterType::KIND::INTEGER ||
						member.type.dictionaryValueKind == MessageParameterType::KIND::UINTEGER ||
						member.type.dictionaryValueKind == MessageParameterType::KIND::REAL ||
						member.type.dictionaryValueKind == MessageParameterType::KIND::CHARACTER_STRING;

	f.write("bool currentChanged = false;\n" );
	f.write("if ( addr.size() > offset + 2 ) // update for a value of a particular dictionary element\n" );
	f.write("{\n" );

	if ( simpleType )
		f.write("\tthrow std::exception(); // deeper address is unrelated to simple type of dictionary values (IDL type of t.%s elements is %s)\n", member.name.c_str(), impl_kindToString( member.type.dictionaryValueKind ) );
	else
	{
		f.write("\toffset += 1;\n" );
		f.write("\tauto key = %s::fromAddress(addr, offset);\n", getValueProcessor(member.type.dictionaryKeyKind) );
		f.write("\tauto& value = this->%s[key];\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\tcurrentChanged = %s::parse_continue( parser, *value, addr, offset );\n", getDictionaryValueSubscriberTypeProcessor( member.type ).c_str() );
		f.write("\tif ( currentChanged )\n" );
		f.write("\t{\n" );
		f.write("\t\tthis->notifyValueUpdated_%s( key );\n", member.name.c_str() );
		f.write("\t}\n" );
	}
				
	f.write("}\n" );

	f.write("else if ( addr.size() == offset + 2 ) // update of one or more elelments as a whole\n" );
	f.write("{\n" );
	f.write("\tuint64_t action = addr[offset + 1];\n" );

	f.write("\tswitch ( action )\n" );
	f.write("\t{\n" );

	f.write("\tcase ActionOnDictionary::remove:\n" );
	f.write("\t{\n" );
	f.write("\t\tparser.nextElement();\n" );
	f.write("\t\tauto key = %s::parse_key( parser );\n", getSubscriberTypeProcessor( member.type ).c_str() );
	f.write("\t\tauto f = this->%s.find( key );\n", impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\t\tif ( f == this->%s.end() )\n", impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\t\t\tthrow std::exception();\n" );
	f.write("\t\tauto oldVal = std::move(f->second);\n" );
	f.write("\t\tthis->%s.erase( key );\n", impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\t\tthis->notifyRemoved_%s( key, std::move(oldVal) );\n", member.name.c_str() );
	f.write("\t\tcurrentChanged = true;\n" );
	f.write("\t\tbreak;\n" );
	f.write("\t}\n" );

	f.write("\tcase ActionOnDictionary::update_value:\n" );
	f.write("\t{\n" );
	f.write("\t\tparser.nextElement();\n" );
	f.write("\t\tauto key = %s::parse_key( parser );\n", getSubscriberTypeProcessor( member.type ).c_str() );
	f.write("\t\tauto f = this->%s.find( key );\n", impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\t\tif ( f == this->%s.end() )\n", impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\t\t\tthrow std::exception();\n" );
	f.write("\t\tparser.nextElement();\n" );
	f.write("\t\tparser.namedParamBegin(\"value\");\n" );

	if(simpleType)
	{
		f.write("\t\tauto oldVal = this->%s[key];\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\tauto newVal = %s::parse( parser );\n", getDictionaryValueSubscriberTypeProcessor(member.type).c_str() );
		f.write("\t\tcurrentChanged = newVal != oldVal;\n" );

		f.write("\t\tif ( currentChanged )\n" );
		f.write("\t\t{\n" );
		f.write("\t\t\tthis->%s[key] = newVal;\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\t\tthis->notifyValueUpdated_%s( key, oldVal );\n", member.name.c_str() );
		f.write("\t\t}\n" );
	}
	else
	{
		f.write("\t\tauto& value = this->%s[key];\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\tcurrentChanged = %s::parse_notify( parser, *value );\n", getDictionaryValueSubscriberTypeProcessor(member.type).c_str() );
		f.write("\t\tif ( currentChanged )\n" );
		f.write("\t\t{\n" );
		f.write("\t\t\tthis->notifyValueUpdated_%s( key );\n", member.name.c_str() );
		f.write("\t\t}\n" );
	}

	f.write("\t\tbreak;\n" );
	f.write("\t}\n" );

	f.write("\tcase ActionOnDictionary::insert:\n" );
	f.write("\t{\n" );
	f.write("\t\tparser.nextElement();\n" );
	f.write("\t\tauto key = %s::parse_key( parser );\n", getSubscriberTypeProcessor( member.type ).c_str() );
	f.write("\t\tparser.nextElement();\n" );
	f.write("\t\tparser.namedParamBegin(\"value\");\n" );

	if(simpleType)
		f.write("\t\tauto newVal = %s::parse(parser);\n", getDictionaryValueSubscriberTypeProcessor(member.type).c_str() );
	else
	{
		f.write("\t\tauto newVal = this->makeValue_%s();\n", member.name.c_str() );
		f.write("\t\t%s::parse_notify(parser, *newVal);\n", getDictionaryValueSubscriberTypeProcessor(member.type).c_str() );
	}

	f.write("\t\tthis->%s.insert( GMQ_COLL make_pair( key, std::move(newVal) ) );\n", impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\t\tthis->notifyInserted_%s( key );\n", member.name.c_str() );
	f.write("\t\tcurrentChanged = true;\n" );
	f.write("\t\tbreak;\n" );
	f.write("\t}\n" );
	f.write("\tdefault:\n" );
	f.write("\t\tthrow std::exception();\n" );
	f.write("\t}\n" );
	f.write("}\n" );
	f.write("else if ( addr.size() == offset + 1 ) // replacement of the whole dictionary\n" );
	f.write("{\n" );
	f.write("\tparser.nextElement();\n" );
	f.write("\tparser.leafeBegin();\n" );


	if(simpleType)
		f.write("\tauto newVal = %s::parse( parser );\n", getSubscriberTypeProcessor( member.type ).c_str() );
	else
	{
		f.write("\t%s newVal;\n", getSubscriberCppType( member.type ).c_str() );
		f.write("\t%s::parse_notify( parser, newVal, [this]() { return this->makeValue_%s(); } );\n", getSubscriberTypeProcessor( member.type ).c_str(), member.name.c_str() );
	}

	f.write("\tcurrentChanged = true;\n" );
	f.write("\tthis->%s = std::move(newVal);\n", impl_memberOrAccessFunctionName( member ).c_str() );

	f.write("\n" );
	f.write("}\n" );
	f.write("else // bad address\n" );
	f.write("\tthrow std::exception();" );

	f.write("\n" );
	f.write("if ( currentChanged )\n" );
	f.write("{\n" );
	f.write("\tchanged = true;\n" );
	f.write("\tthis->notifyUpdated_%s();\n", member.name.c_str() );
	f.write("}\n" );
}

void impl_generateApplyUpdateForFurtherProcessingInVector2( FileWritter f, Root& root, MessageParameter& member, const std::string& parserType )
{
	bool simpleType = member.type.vectorElemKind == MessageParameterType::KIND::INTEGER ||
						member.type.vectorElemKind == MessageParameterType::KIND::UINTEGER ||
						member.type.vectorElemKind == MessageParameterType::KIND::REAL ||
						member.type.vectorElemKind == MessageParameterType::KIND::CHARACTER_STRING;

	string elemProc = getVectorElemSubscriberTypeProcessor(member.type);

	f.write("bool currentChanged = false;\n" );
	f.write("if ( addr.size() > offset + 2 ) // update for a member of a particular vector element\n" );
	f.write("{\n" );


	if ( simpleType )
		f.write("\tthrow std::exception(); // deeper address is unrelated to simple type of vector elements (IDL type of t.%s elements is %s)\n", member.name.c_str(), impl_kindToString( member.type.vectorElemKind ) );
	else
	{
		f.write("\tsize_t pos = addr[offset + 1];\n" );
		f.write("\tif ( pos > this->%s.size() )\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\tthrow std::exception();\n" );

		f.write("\tauto& value = this->%s[pos];\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\tcurrentChanged = %s::parse_continue( parser, *value, addr, offset + 2 );\n", getVectorElemSubscriberTypeProcessor( member.type ).c_str() );
		f.write("\tif ( currentChanged )\n" );
		f.write("\t{\n" );
		f.write("\t\tthis->notifyElementUpdated_%s(pos);\n", member.name.c_str() );
		f.write("\t}\n" );
	}
				
	f.write("}\n" );
	f.write("else if ( addr.size() == offset + 2 ) // update of one or more elelments as a whole\n" );
	f.write("{\n" );
	f.write("\tsize_t pos = addr[offset + 1];\n" );
	f.write("\tif ( pos > this->%s.size() )\n", impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\t\tthrow std::exception();\n" );
	f.write("\tparser.nextElement();\n" );
	f.write("\tuint64_t action = parser.parseAction();\n" );
	f.write("\tswitch ( action )\n" );
	f.write("\t{\n" );
	f.write("\tcase ActionOnVector::remove_at:\n" );
	f.write("\t{\n" );

	if(simpleType)
	{
		f.write("\t\tauto oldVal = this->%s[pos];\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\tthis->%s.erase( this->%s.begin() + pos );\n", impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\tthis->notifyErased_%s( pos, oldVal );\n", member.name.c_str() );
	}
	else
	{
		f.write("\t\tauto oldVal = std::move(this->%s[pos]);\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\tthis->%s.erase( this->%s.begin() + pos );\n", impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\tthis->notifyErased_%s( pos, std::move(oldVal) );\n", member.name.c_str() );
	}
	f.write("\t\tcurrentChanged = true;\n" );
	f.write("\t\tbreak;\n" );
	f.write("\t}\n" );
	f.write("\tcase ActionOnVector::update_at:\n" );
	f.write("\t{\n" );
	f.write("\t\tparser.nextElement();\n" );
	f.write("\t\tparser.leafeBegin();\n" );

	if(simpleType)
	{
		f.write("\t\tauto oldVal = this->%s[pos];\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\tauto newVal = %s::parse( parser );\n", elemProc.c_str() );
		f.write("\t\tcurrentChanged = newVal != oldVal;\n" );

		f.write("\t\tif ( currentChanged )\n" );
		f.write("\t\t{\n" );
		f.write("\t\t\tthis->%s[pos] = newVal;\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\t\tthis->notifyElementUpdated_%s( pos, oldVal );\n", member.name.c_str() );
		f.write("\t\t}\n" );
	}
	else
	{

		f.write("\t\tauto& value = this->%s[pos];\n", impl_memberOrAccessFunctionName( member ).c_str() );
		f.write("\t\tcurrentChanged = %s::parse_notify( parser, *value );\n", elemProc.c_str() );
		f.write("\t\tif ( currentChanged )\n" );
		f.write("\t\t{\n" );
		f.write("\t\t\tthis->notifyElementUpdated_%s( pos );\n", member.name.c_str() );
		f.write("\t\t}\n" );
	}
	f.write("\t\tbreak;\n" );
	f.write("\t}\n" );
	f.write("\tcase ActionOnVector::insert_single_before:\n" );
	f.write("\t{\n" );
	f.write("\t\tparser.nextElement();\n" );
	f.write("\t\tparser.leafeBegin();\n" );

	if(simpleType)
	{
		f.write("\t\tauto newVal = %s::parse(parser);\n", elemProc.c_str() );
	}
	else
	{
		f.write("\t\tauto newVal = this->makeElement_%s();\n", member.name.c_str() );
		f.write("\t\t%s::parse_notify(parser, *newVal);\n", elemProc.c_str() );
	}

	f.write("\t\tthis->%s.insert( this->%s.begin() + pos, std::move(newVal) );\n", impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\t\tthis->notifyInserted_%s( pos );\n", member.name.c_str() );
	f.write("\t\tcurrentChanged = true;\n" );
	f.write("\t\tbreak;\n" );
	f.write("\t}\n" );
	f.write("\tdefault:\n" );
	f.write("\t\tthrow std::exception();\n" );
	f.write("\t}\n" );
	f.write("}\n" );
	f.write("else if ( addr.size() == offset + 1 ) // replacement of the whole vector\n" );
	f.write("{\n" );
	f.write("\tparser.nextElement();\n" );
	f.write("\tparser.leafeBegin();\n" );
	if(simpleType)
	{
		f.write("\tauto newVal = %s::parse( parser );\n", getSubscriberTypeProcessor( member.type ).c_str() );
	}
	else
	{
		f.write("\t%s newVal;\n", getSubscriberCppType( member.type ).c_str() );
		f.write("\t%s::parse_notify( parser, newVal, [this]() { return this->makeElement_%s(); } );\n", getSubscriberTypeProcessor( member.type ).c_str(), member.name.c_str() );
	}

	f.write("\tcurrentChanged = !%s::isSame( newVal, this->%s );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\tif(currentChanged)\n" );
	f.write("\t\tthis->%s = std::move(newVal);\n", impl_memberOrAccessFunctionName( member ).c_str() );
	f.write("\n" );
	f.write("}\n" );

	f.write("else // unexpected addresss size\n" );
	f.write("\tthrow std::exception();\n" );

	f.write("\n" );
	f.write("if ( currentChanged )\n" );
	f.write("{\n" );
	f.write("\tchanged = true;\n" );
	f.write("\tthis->notifyUpdated_%s();\n", member.name.c_str() );
	f.write("}\n" );
}

void impl_generateContinueParsingFunctionForPublishableStruct_MemberIterationBlock2( FileWritter f, Root& root, CompositeType& obj, const char* offset, size_t idxBase, const std::string& parserType )
{
	assert( obj.type != CompositeType::Type::discriminated_union );

	for ( size_t i=0; i<obj.getMembers().size(); ++i )
	{
		std::string numId = std::to_string(idxBase + i);
		f.write("case %s:\n", numId.c_str() );
		f.write("{\n" );
		assert( obj.getMembers()[i] != nullptr );
		auto& member = *(obj.getMembers()[i]);
		switch ( member.type.kind )
		{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
			{
				f.write("\tif ( addr.size() == offset + 1 )\n" );
				f.write("\t{\n" );
				f.write("\t\tparser.nextElement();\n" );
				f.write("\t\tparser.leafeBegin();\n" );
				impl_generateApplyUpdateForSimpleType2( f.indent(2), member, parserType );
				f.write("\t}\n" );
				f.write("\telse // deeper address not supported for this type\n" );
				f.write("\t\tthrow std::exception(); // bad format, TODO: ...\n" );
				break;
			}
			case  MessageParameterType::KIND::STRUCT:
			case  MessageParameterType::KIND::DISCRIMINATED_UNION:
			{
				f.write("\tif ( addr.size() == offset + 1 ) // we have to parse and apply changes of this child\n" );
				f.write("\t{\n" );
				f.write("\t\tparser.nextElement();\n" );
				f.write("\t\tparser.leafeBegin();\n" );

				impl_generateApplyUpdateForStructItself2( f.indent(2), member, parserType ); // TODO: revise DU: we may need something DU_spec here

				f.write("\t}\n" );
				f.write("\telse // let child continue parsing\n" );
				f.write("\t{\n" );

				impl_generateApplyUpdateForFurtherProcessingInStruct3( f.indent(2), member, parserType );

				f.write("\t}\n" );
				break;
			}
			case MessageParameterType::KIND::VECTOR:
				impl_generateApplyUpdateForFurtherProcessingInVector2( f.indent(1), root, member, parserType );
				break;
			case MessageParameterType::KIND::DICTIONARY:
				impl_generateApplyUpdateForFurtherProcessingInDictionary2( f.indent(2), root, member, parserType );
				break;
			default:
				assert( false );
		}
		f.write("\tbreak;\n" );
		f.write("}\n" );
	}
}

void impl_generateApplyUpdateForDiscriminatedUnionVariant2( FileWritter f, bool addReportChanges )
{
	f.write("parser.nextElement();\n" );
	f.write("parser.leafeBegin();\n");
	f.write("uint64_t newVal = parser.parseUnsignedInteger();\n" );

	f.write("auto oldVal = this->currentVariant();\n" );
	f.write("bool currentChanged = oldVal != newVal;\n" );
	f.write("if ( currentChanged )\n" );
	f.write("{\n" );
	f.write("\tthis->initAs( newVal );\n" );
	f.write("\tchanged = true;\n" );
	f.write("\tthis->notifyUpdated_currentVariant( oldVal );\n" );
	f.write("}\n" );
}

void impl_generateContinueParsingFunctionForPublishableStruct2( FileWritter f, Root& root, CompositeType& obj, const std::string& parserType )
{
	f.write("\tbool parse_continue( %s& parser, GMQ_COLL vector<uint64_t>& addr, uint64_t offset )\n", parserType.c_str() );
	f.write("\t{\n" );
	f.write("\t\tbool changed = false;\n" );

	if ( obj.isDiscriminatedUnion() )
	{
		f.write("\t\tGMQ_ASSERT( addr.size() );\n" );
		f.write("\t\tif ( addr[offset] == 0 ) // changing current variant\n" );
		f.write("\t\t{\n" );
		f.write("\t\t\tif ( addr.size() > offset + 1 )\n" );
		f.write("\t\t\t\tthrow std::exception(); // bad format, TODO: ...\n" );
		impl_generateApplyUpdateForDiscriminatedUnionVariant2( f.indent(3), true );
		f.write("\t\t}\n" );

		f.write("\t\telse // updating particular members in particular cases\n" );

		f.write("\t\t{\n" );
		f.write("\t\t\tswitch ( this->currentVariant() )\n" );
		f.write("\t\t\t{\n" );
		for ( auto& it: obj.getDiscriminatedUnionCases() )
		{
			f.write("\t\t\t\tcase Variants::%s: // IDL CASE %s\n", it->name.c_str(), it->name.c_str() );
			f.write("\t\t\t\t{\n" );
			f.write("\t\t\t\t\tswitch ( addr[offset] )\n" );
			f.write("\t\t\t\t\t{\n" );
			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_generateContinueParsingFunctionForPublishableStruct_MemberIterationBlock2( f.indent(6), root, cs, "\t\t\t\t\t\t", 1, parserType );
			f.write("\t\t\t\t\t\tdefault:\n" );
			f.write("\t\t\t\t\t\t\tthrow std::exception(); // unexpected\n" );
			f.write("\t\t\t\t\t}\n" );
			f.write("\t\t\t\t\tbreak;\n" );
			f.write("\t\t\t\t}\n" );
		}
		f.write("\t\t\t\tdefault:\n" );
		f.write("\t\t\t\t\tthrow std::exception(); // unexpected\n" );
		f.write("\t\t\t}\n" );
		f.write("\t\t}\n" );
	}
	else
	{
		f.write("\t\tGMQ_ASSERT( addr.size() );\n" );
		f.write("\t\tswitch ( addr[offset] )\n" );
		f.write("\t\t{\n" );
		impl_generateContinueParsingFunctionForPublishableStruct_MemberIterationBlock2( f.indent(3), root, obj, "\t\t\t", 0, parserType );
		f.write("\t\t\tdefault:\n" );
		f.write("\t\t\t\tthrow std::exception(); // unexpected\n" );
		f.write("\t\t}\n" );
	}
	f.write("\t\tif ( changed )\n" );
	f.write("\t\t\tthis->notifyUpdated();\n" );
	f.write("\t\treturn changed;\n" );
	f.write("\t}\n" );
}


void impl_generateParseFunctionForPublishableStruct_MemberIterationBlock2( FileWritter f, Root& root, CompositeType& obj, const std::string& parserType )
{
	assert( obj.type != CompositeType::Type::discriminated_union );

	for ( size_t i=0; i<obj.getMembers().size(); ++i )
	{
		assert( obj.getMembers()[i] != nullptr );
		auto& member = *(obj.getMembers()[i]);

		if(i != 0)
			f.write("parser.nextElement();\n" );

		f.write("parser.namedParamBegin( \"%s\" );\n", member.name.c_str());
		f.write("{\n" );
		switch ( member.type.kind )
		{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
				impl_generateApplyUpdateForSimpleType2( f, member, parserType );
				break;
			case  MessageParameterType::KIND::STRUCT:
			case  MessageParameterType::KIND::DISCRIMINATED_UNION:
				impl_generateApplyUpdateForFurtherProcessingInStruct2( f, member, parserType );
				break;
			case MessageParameterType::KIND::VECTOR:
				switch(member.type.vectorElemKind)
				{
				case MessageParameterType::KIND::INTEGER:
				case MessageParameterType::KIND::UINTEGER:
				case MessageParameterType::KIND::REAL:
				case MessageParameterType::KIND::CHARACTER_STRING:
					f.write("\tauto newVal = %s::parse(parser);\n", getSubscriberTypeProcessor(member.type).c_str() );
					break;
				case  MessageParameterType::KIND::STRUCT:
				case  MessageParameterType::KIND::DISCRIMINATED_UNION:

					f.write("\t%s newVal;\n", getSubscriberCppType( member.type ).c_str() );
					f.write("\t%s::parse_notify( parser, newVal, [this]() { return this->makeElement_%s(); } );\n", getSubscriberTypeProcessor( member.type ).c_str(), member.name.c_str() );
					break;
				default:
					assert( false );
				}
				f.write("\tbool currentChanged = !%s::isSame( newVal, this->%s );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				f.write("\tif(currentChanged)\n" );
				f.write("\t{\n" );
				f.write("\t\tthis->%s = std::move(newVal);\n", impl_memberOrAccessFunctionName( member ).c_str() );
				f.write("\t\tthis->notifyUpdated_%s();\n", member.name.c_str());
				f.write("\t\tchanged = true;\n", member.name.c_str());
				f.write("\t}\n" );
				break;
			case MessageParameterType::KIND::DICTIONARY:
				switch(member.type.dictionaryValueKind)
				{
				case MessageParameterType::KIND::INTEGER:
				case MessageParameterType::KIND::UINTEGER:
				case MessageParameterType::KIND::REAL:
				case MessageParameterType::KIND::CHARACTER_STRING:
					f.write("\tauto newVal = %s::parse(parser);\n", getSubscriberTypeProcessor(member.type).c_str() );
					break;
				case  MessageParameterType::KIND::STRUCT:
				case  MessageParameterType::KIND::DISCRIMINATED_UNION:

					f.write("\t%s newVal;\n", getSubscriberCppType( member.type ).c_str() );
					f.write("\t%s::parse_notify( parser, newVal, [this]() { return this->makeValue_%s(); } );\n", getSubscriberTypeProcessor( member.type ).c_str(), member.name.c_str() );
					break;
				default:
					assert( false );
				}
				f.write("\tthis->%s = std::move(newVal);\n", impl_memberOrAccessFunctionName( member ).c_str() );
				f.write("\tthis->notifyUpdated_%s();\n", member.name.c_str());
				f.write("\tchanged = true;\n", member.name.c_str());
				break;
			default:
				assert( false );
		}
		f.write("}\n" );
	}
}

void impl_generateParseFunctionForPublishableStruct2( FileWritter f, Root& root, CompositeType& obj, const std::string& parserType )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::publishable );

	std::string typeName = getGeneratedTypeName(obj);
	f.write("\tbool parse_notify( %s& parser )\n", parserType.c_str() );
	f.write("\t{\n" );
	f.write("\t\tbool changed = false;\n" );
	
	f.write("\t\tparser.structBegin();\n" );

	if ( obj.isDiscriminatedUnion() )
	{
		f.write("\t\tparser.namedParamBegin( \"caseId\" );\n" );
		f.write("\t\tuint64_t caseId = parser.parseUnsignedInteger();\n" );
		f.write("\t\tthis->initAs( caseId );\n" );
		f.write("\t\tparser.nextElement();\n");
		f.write("\t\tif ( this->currentVariant() != Variants::unknown )\n" );
		f.write("\t\t{\n" );

		f.write("\t\t\tparser.namedParamBegin( \"caseData\" );\n");
		f.write("\t\t\tparser.structBegin();\n" );
		f.write("\t\t\tswitch ( this->currentVariant() )\n" );
		f.write("\t\t\t{\n" );

		for ( auto& it: obj.getDiscriminatedUnionCases() )
		{
			std::string numId = std::to_string(it->numID);
			f.write("\t\t\t\tcase Variants::%s: // IDL CASE %s\n", it->name.c_str(), it->name.c_str() );
			f.write("\t\t\t\t{\n" );
			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_generateParseFunctionForPublishableStruct_MemberIterationBlock2( f.indent(5), root, cs, parserType );
			f.write("\t\t\t\t\tbreak;\n" );
			f.write("\t\t\t\t}\n" );
		}

		f.write("\t\t\t\tdefault:\n" );
		f.write("\t\t\t\t\tthrow std::exception(); // unexpected\n" );
		f.write("\t\t\t}\n" );
		f.write("\t\t\tparser.structEnd();\n" );

		f.write("\t\t}\n" );
	}
	else
	{
		impl_generateParseFunctionForPublishableStruct_MemberIterationBlock2( f.indent(2), root, obj, parserType);
	}

	f.write("\n" );

	f.write("\t\tparser.structEnd();\n" );

	f.write("\t\tif ( changed )\n" );
	f.write("\t\t\tthis->notifyUpdated();\n" );
	f.write("\t\treturn changed;\n" );
	f.write("\t}\n" );
}

void impl_generateParseFunctionForPublishableStructStateSync_MemberIterationBlock2( FileWritter f, Root& root, CompositeType& obj, const std::string& parserType )
{
	assert(	obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union_case ||
			obj.type == CompositeType::Type::publishable );

	for ( size_t i=0; i<obj.getMembers().size(); ++i )
	{
		assert( obj.getMembers()[i] != nullptr );
		auto& member = *(obj.getMembers()[i]);

		if(i != 0)
			f.write("parser.nextElement();\n\n" );
			

		f.write("parser.namedParamBegin( \"%s\" );\n", member.name.c_str() );

		switch ( member.type.kind )
		{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
				f.write("%s::parse( parser, this->%s );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				break;
			case  MessageParameterType::KIND::STRUCT:
			case  MessageParameterType::KIND::DISCRIMINATED_UNION:
				f.write("%s::parse_state_sync( parser, this->%s_lazy() );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				break;
			case MessageParameterType::KIND::VECTOR:
				switch ( member.type.vectorElemKind )
				{
					case MessageParameterType::KIND::INTEGER:
					case MessageParameterType::KIND::UINTEGER:
					case MessageParameterType::KIND::REAL:
					case MessageParameterType::KIND::CHARACTER_STRING:
						f.write("%s::parse( parser, this->%s );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
						break;
					case  MessageParameterType::KIND::STRUCT:
					case  MessageParameterType::KIND::DISCRIMINATED_UNION:
						f.write("%s::parse_state_sync( parser, this->%s, [this]() { return this->makeElement_%s(); } );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), member.name.c_str() );
						break;
					default:
						assert( false );
				}
				break;
			case MessageParameterType::KIND::DICTIONARY:
				switch ( member.type.dictionaryValueKind )
				{
					case MessageParameterType::KIND::INTEGER:
					case MessageParameterType::KIND::UINTEGER:
					case MessageParameterType::KIND::REAL:
					case MessageParameterType::KIND::CHARACTER_STRING:
						f.write("%s::parse( parser, this->%s );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
						break;
					case  MessageParameterType::KIND::STRUCT:
					case  MessageParameterType::KIND::DISCRIMINATED_UNION:
						f.write("%s::parse_state_sync( parser, this->%s, [this]() { return this->makeValue_%s(); } );\n", getSubscriberTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), member.name.c_str() );
						break;
					default:
						assert( false );
				}
				break;
			default:
				assert( false );
		}


	}
}

void impl_generateParseFunctionBodyForPublishableStructStateSyncOrMessageInDepth2( FileWritter f, Root& root, CompositeType& obj, const std::string& parserType )
{
	assert(	obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::publishable );

	if ( obj.isDiscriminatedUnion() )
	{
		f.write("\t\tparser.namedParamBegin( \"caseId\" );\n");
		f.write("\t\tuint64_t caseId = parser.parseUnsignedInteger();\n" );
		f.write("\t\tthis->initAs( caseId );\n" );
		f.write("\t\tparser.nextElement();\n");
		f.write("\t\tif ( this->currentVariant() != Variants::unknown )\n" );
		f.write("\t\t{\n" );

		f.write("\t\t\tparser.namedParamBegin( \"caseData\" );\n");
		f.write("\t\t\tparser.structBegin();\n" );
		f.write("\t\t\tswitch ( this->currentVariant() )\n" );
		f.write("\t\t\t{\n" );

		for ( auto& it: obj.getDiscriminatedUnionCases() )
		{
			f.write("\t\t\t\tcase Variants::%s: // IDL CASE %s\n", it->name.c_str(), it->name.c_str() );
			f.write("\t\t\t\t{\n" );

			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_generateParseFunctionForPublishableStructStateSync_MemberIterationBlock2( f.indent(5), root, cs, parserType );

			f.write("\t\t\t\t\tbreak;\n" );
			f.write("\t\t\t\t}\n" );
		}

		f.write("\t\t\t\tdefault:\n" );
		f.write("\t\t\t\t\tthrow std::exception(); // unexpected\n" );
		f.write("\t\t\t}\n" );
		f.write("\t\t\tparser.structEnd();\n" );

		f.write("\t\t}\n" );
	}
	else
		impl_generateParseFunctionForPublishableStructStateSync_MemberIterationBlock2( f.indent(2), root, obj, parserType );
}

void impl_generateParseFunctionForPublishableStructStateSyncOrMessageInDepth2( FileWritter f, Root& root, CompositeType& obj, const std::string& parserType )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::publishable );

	f.write("\tvoid parse_state_sync( %s& parser )\n", parserType.c_str() );
	f.write("\t{\n" );
	f.write("\t\tparser.structBegin();\n" );
	f.write("\n" );

	impl_generateParseFunctionBodyForPublishableStructStateSyncOrMessageInDepth2( f, root, obj, parserType );

	f.write("\n" );
	f.write("\t\tparser.structEnd();\n" );

	f.write("\t}\n" );
}


void impl_GenerateApplyUpdateMessageMemberFn2( FileWritter f )
{
	f.write("\ttemplate<class ParserT>\n" );
	f.write("\tvoid applyMessageWithUpdates(ParserT& parser)\n" );
	f.write("\t{\n" );
	f.write("\t\tparser.stateUpdateBegin();\n" );
	f.write("\t\tGMQ_COLL vector<uint64_t> addr;\n" );
	f.write("\t\twhile( parser.changeBegin( addr ) )\n" );
	f.write("\t\t{\n" );
	f.write("\t\t\tGMQ_ASSERT( addr.size() );\n" );

	f.write("\t\t\tthis->parse_continue(parser, addr, 0);\n" );

	f.write("\t\t\taddr.clear();\n" );
	f.write("\t\t\tparser.changeEnd();\n" );
	f.write("\t\t}\n" );
	f.write("\t\tparser.stateUpdateEnd();\n" );

	f.write("\t}\n" );
}

void impl_generateParseFunctionForPublishableState2( FileWritter f )
{
	f.write("\ttemplate<class ParserT>\n" );
	f.write("\tvoid parseStateSyncMessage( ParserT& parser )\n" );
	f.write("\t{\n" );

	f.write("\t\tthis->parse_state_sync( parser );\n" );
	f.write("\t\tthis->notifyFullyUpdated();\n" );
	f.write("\t}\n" );
}



void impl_generateComposeFunctionForPublishableStruct_MemberIterationBlock2( FileWritter f, CompositeType& obj )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union_case ||
			obj.type == CompositeType::Type::publishable );

	for ( size_t i=0; i<obj.getMembers().size(); ++i )
	{
		auto& it = obj.getMembers()[i];
		assert( it != nullptr );
		auto& member = *it;

		if( i != 0)
		{
			f.write("\n" );
			f.write("composer.nextElement();\n" );
			f.write("\n" );
		}

		f.write("composer.namedParamBegin( \"%s\" );\n", member.name.c_str() );

		switch ( member.type.kind )
		{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
				f.write("%s::compose(composer, this->%s );\n", getSubscriberTypeProcessor(member.type).c_str(), impl_memberOrAccessFunctionName( member ).c_str());
				break;
			case  MessageParameterType::KIND::STRUCT:
			case  MessageParameterType::KIND::DISCRIMINATED_UNION:
				f.write("%s::compose(composer, this->%s_lazy() );\n", getSubscriberTypeProcessor(member.type).c_str(), impl_memberOrAccessFunctionName( member ).c_str());
				break;
			case MessageParameterType::KIND::VECTOR:
			case MessageParameterType::KIND::DICTIONARY:
				f.write("%s::compose(composer, this->%s );\n", getSubscriberTypeProcessor(member.type).c_str(), impl_memberOrAccessFunctionName( member ).c_str());
				break;
			default:
				assert( false );
		}

	}
}

void impl_generateComposeFunctionForPublishableStruct2( FileWritter f, CompositeType& obj, const std::string& composerType )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::publishable );

	f.write("\tvoid compose( %s& composer )\n", composerType.c_str() );
	f.write("\t{\n" );

	f.write("\t\tcomposer.structBegin();\n" );
	f.write("\n" );

	if ( obj.isDiscriminatedUnion() )
	{
		f.write("\t\tVariants caseId = this->currentVariant();\n" );
		f.write("\t\tcomposer.namedParamBegin( \"caseId\" );\n" );
		f.write("\t\tcomposer.composeUnsignedInteger( caseId );\n" );

		f.write("\n" );
		f.write("\t\tcomposer.nextElement();\n" );
		f.write("\n" );

		f.write("\t\tif ( caseId != Variants::unknown )\n" );
		f.write("\t\t{\n" );
		f.write("\t\t\tcomposer.namedParamBegin( \"caseData\" );\n" );
		f.write("\t\t\tcomposer.structBegin();\n" );
		f.write("\t\t\tswitch ( caseId )\n" );
		f.write("\t\t\t{\n" );
		for ( auto& it: obj.getDiscriminatedUnionCases() )
		{
			f.write("\t\t\t\tcase Variants::%s: // IDL CASE %s\n", it->name.c_str(), it->name.c_str() );
			f.write("\t\t\t\t{\n" );
			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_generateComposeFunctionForPublishableStruct_MemberIterationBlock2( f.indent(5), cs );
			f.write("\t\t\t\t\tbreak;\n" );
			f.write("\t\t\t\t}\n" );
		}
		f.write("\t\t\t\tdefault:\n" );
		f.write("\t\t\t\t\tthrow std::exception(); // unexpected\n" );
		f.write("\t\t\t}\n" );
		f.write("\t\t\tcomposer.structEnd();\n" );

		f.write("\t\t}\n" );
	}
	else
		impl_generateComposeFunctionForPublishableStruct_MemberIterationBlock2( f.indent(2), obj );

	f.write("\n" );
	f.write("\t\tcomposer.structEnd();\n" );

	f.write("\t}\n" );
}


void impl_SubscriberVirtualHandlers_Members( FileWritter f, CompositeType& obj )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union_case ||
			obj.type == CompositeType::Type::publishable );

	for (size_t i = 0; i < obj.getMembers().size(); ++i)
	{
		assert(obj.getMembers()[i] != nullptr);
		auto &member = *(obj.getMembers()[i]);

		switch (member.type.kind)
		{
		case MessageParameterType::KIND::INTEGER:
		case MessageParameterType::KIND::UINTEGER:
		case MessageParameterType::KIND::REAL:
		case MessageParameterType::KIND::CHARACTER_STRING:
			f.write("virtual void notifyUpdated_%s(%s oldVal) {}\n", member.name.c_str(), getCppType(member.type.kind).c_str());
			break;
		case MessageParameterType::KIND::STRUCT:
		case MessageParameterType::KIND::DISCRIMINATED_UNION:
			f.write("virtual void notifyUpdated_%s() {}\n", member.name.c_str());
			break;
		case MessageParameterType::KIND::VECTOR:
			switch (member.type.vectorElemKind)
			{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
				f.write("virtual void notifyUpdated_%s() {}\n", member.name.c_str());
				f.write("virtual void notifyElementUpdated_%s(size_t ix, %s oldVal) {}\n", member.name.c_str(), getVectorElemSubscriberCppType(member.type).c_str());
				f.write("virtual void notifyInserted_%s(size_t ix) {}\n", member.name.c_str());
				f.write("virtual void notifyErased_%s(size_t ix, %s oldVal) {}\n", member.name.c_str(), getVectorElemSubscriberCppType(member.type).c_str());
				break;
			case MessageParameterType::KIND::STRUCT:
			case MessageParameterType::KIND::DISCRIMINATED_UNION:
				f.write("virtual void notifyUpdated_%s() {}\n", member.name.c_str());
				f.write("virtual void notifyElementUpdated_%s(size_t ix) {}\n", member.name.c_str());
				f.write("virtual void notifyInserted_%s(size_t ix) {}\n", member.name.c_str());
				f.write("virtual void notifyErased_%s(size_t ix, %s oldVal) {}\n", member.name.c_str(), getVectorElemSubscriberCppType(member.type).c_str());
				break;
			default:
				assert(false);
			}
			break;
		case MessageParameterType::KIND::DICTIONARY:
		{
			string keyType = MessageParameterType::isNumericType(member.type.dictionaryKeyKind) ? getCppType(member.type.dictionaryKeyKind) :
				fmt::format("const {}&", getCppType(member.type.dictionaryKeyKind));
			switch (member.type.dictionaryValueKind)
			{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
				f.write("virtual void notifyUpdated_%s() {}\n", member.name.c_str());
				f.write("virtual void notifyValueUpdated_%s(%s key, %s oldVal) {}\n", member.name.c_str(), keyType.c_str(), getDictionaryValueSubscriberCppType(member.type).c_str());
				f.write("virtual void notifyInserted_%s(%s key) {}\n", member.name.c_str(), keyType.c_str());
				f.write("virtual void notifyRemoved_%s(%s key, %s oldVal) {}\n", member.name.c_str(), keyType.c_str(), getDictionaryValueSubscriberCppType(member.type).c_str());
				break;
			case MessageParameterType::KIND::STRUCT:
			case MessageParameterType::KIND::DISCRIMINATED_UNION:
				f.write("virtual void notifyUpdated_%s() {}\n", member.name.c_str());
				f.write("virtual void notifyValueUpdated_%s(%s key) {}\n", member.name.c_str(), keyType.c_str());
				f.write("virtual void notifyInserted_%s(%s key) {}\n", member.name.c_str(), keyType.c_str());
				f.write("virtual void notifyRemoved_%s(%s key, %s oldVal) {}\n", member.name.c_str(), keyType.c_str(), getDictionaryValueSubscriberCppType(member.type).c_str());
				break;
			default:
				assert(false);
			}
			break;
		}
		default:
			assert(false);
		}
	}
}

void impl_SubscriberVirtualFactories( FileWritter f, CompositeType& obj )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union_case ||
			obj.type == CompositeType::Type::publishable );

	for (size_t i = 0; i < obj.getMembers().size(); ++i)
	{
		assert(obj.getMembers()[i] != nullptr);
		auto &member = *(obj.getMembers()[i]);

		switch (member.type.kind)
		{
		case MessageParameterType::KIND::INTEGER:
		case MessageParameterType::KIND::UINTEGER:
		case MessageParameterType::KIND::REAL:
		case MessageParameterType::KIND::CHARACTER_STRING:
			break;
		case MessageParameterType::KIND::STRUCT:
		case MessageParameterType::KIND::DISCRIMINATED_UNION:
			f.write("virtual %s make_%s() { return %s{new %s()}; }\n", getSubscriberCppType(member.type).c_str(), member.name.c_str(), getSubscriberCppType(member.type).c_str(), getSubscriberClassName(member.type.name).c_str());
			break;
		case MessageParameterType::KIND::VECTOR:
			switch (member.type.vectorElemKind)
			{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
				break;
			case MessageParameterType::KIND::STRUCT:
			case MessageParameterType::KIND::DISCRIMINATED_UNION:
				f.write("virtual %s makeElement_%s() { return %s{new %s()}; }\n", getVectorElemSubscriberCppType(member.type).c_str(), member.name.c_str(), getVectorElemSubscriberCppType(member.type).c_str(), getSubscriberClassName(member.type.name).c_str());
				break;
			default:
				assert(false);
			}
			break;
		case MessageParameterType::KIND::DICTIONARY:
			switch (member.type.dictionaryValueKind)
			{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
				break;
			case MessageParameterType::KIND::STRUCT:
			case MessageParameterType::KIND::DISCRIMINATED_UNION:
				f.write("virtual %s makeValue_%s() { return %s{new %s()}; }\n", getDictionaryValueSubscriberCppType(member.type).c_str(), member.name.c_str(), getDictionaryValueSubscriberCppType(member.type).c_str(), getSubscriberClassName(member.type.name).c_str());
				break;
			default:
				assert(false);
			}
			break;
		default:
			assert(false);
		}
	}
}

void impl_SubscriberVirtualHandlers( FileWritter f, CompositeType& obj )
{
	assert(	obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::publishable );



	f.write("////////////////////////////// begin user override section //////////////////////////////\n");


	if ( obj.isDiscriminatedUnion() )
	{
		for ( auto& it: obj.getDiscriminatedUnionCases() )
		{
			f.write("// IDL CASE %s\n", it->name.c_str() );

			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_SubscriberVirtualFactories( f, cs );

		}
	}
	else
		impl_SubscriberVirtualFactories( f, obj );


	if(obj.type == CompositeType::Type::publishable)
		f.write("virtual void notifyFullyUpdated() {}\n");

	f.write("virtual void notifyUpdated() {}\n");
	if ( obj.isDiscriminatedUnion() )
	{
		f.write("virtual void notifyUpdated_currentVariant(Variants oldVal) {}\n");

		for ( auto& it: obj.getDiscriminatedUnionCases() )
		{
			f.write("// IDL CASE %s\n", it->name.c_str() );

			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_SubscriberVirtualHandlers_Members( f, cs );

		}
	}
	else
		impl_SubscriberVirtualHandlers_Members( f, obj );

	f.write("//////////////////////////////  end user override section  //////////////////////////////\n");
}


void impl_SubscriberGetters_Members( FileWritter f, CompositeType& obj )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union_case ||
			obj.type == CompositeType::Type::publishable );

	for (size_t i = 0; i < obj.getMembers().size(); ++i)
	{
		assert(obj.getMembers()[i] != nullptr);
		auto &member = *(obj.getMembers()[i]);

		switch (member.type.kind)
		{
		case MessageParameterType::KIND::INTEGER:
		case MessageParameterType::KIND::UINTEGER:
		case MessageParameterType::KIND::REAL:
			f.write("\tauto get_%s() const { return this->%s; }\n", member.name.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
			break;
		case MessageParameterType::KIND::CHARACTER_STRING:
			f.write("\tconst auto& get_%s() const { return this->%s; }\n", member.name.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
			break;
		case MessageParameterType::KIND::STRUCT:
		case MessageParameterType::KIND::DISCRIMINATED_UNION:
			f.write("\tauto& %s_lazy()\n", member.name.c_str() );
			f.write("\t{\n" );
			f.write("\t\tif(!this->%s)\n", impl_memberOrAccessFunctionName( member ).c_str() );
			f.write("\t\t\tthis->%s = make_%s();\n",impl_memberOrAccessFunctionName( member ).c_str(), member.name.c_str() );
			f.write("\t\treturn *(this->%s);\n", impl_memberOrAccessFunctionName( member ).c_str() );
			f.write("\t}\n" );
			f.write("\tconst auto& get_%s() const { const_cast<ThisType*>(this)->%s_lazy(); return this->%s; }\n", member.name.c_str(), member.name.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
			break;
		case MessageParameterType::KIND::VECTOR:
		case MessageParameterType::KIND::DICTIONARY:
			f.write("\tconst auto& get_%s() const { return this->%s; }\n", member.name.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
			break;
		default:
			assert(false);
		}
	}
}


void impl_SubscriberGetters( FileWritter f, CompositeType& obj )
{
	assert(	obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::publishable );

	if ( obj.isDiscriminatedUnion() )
	{
		f.write("//Variants currentVariant();\n");

		for ( auto& it: obj.getDiscriminatedUnionCases() )
		{
			f.write("// IDL CASE %s\n", it->name.c_str() );

			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_SubscriberGetters_Members( f, cs );

		}
	}
	else
		impl_SubscriberGetters_Members( f, obj );
}


void impl_GeneratePublishableStructIsSameFn_MemberIterationBlock2( FileWritter f, CompositeType& s, bool isPlainStruct )
{
	assert( s.type != CompositeType::Type::discriminated_union );

	for ( size_t i=0; i<s.getMembers().size(); ++i )
	{
		assert( s.getMembers()[i] != nullptr );
		auto& member = *(s.getMembers()[i]);

		if(isPlainStruct)
			f.write("if( ! %s::isSame( s1.get_%s(), s2.%s ) ) return false;\n", getSubscriberTypeProcessor( member.type ).c_str(), member.name.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
		else
			f.write("if( ! %s::isSame( s1.get_%s(), s2.get_%s() ) ) return false;\n", getSubscriberTypeProcessor( member.type ).c_str(), member.name.c_str(), member.name.c_str() );
	}
}

void impl_GeneratePublishableStructIsSameFn3( FileWritter f, CompositeType& s, bool isPlainStruct )
{
	if ( s.isDiscriminatedUnion() )
	{
		f.write("\t\tif ( static_cast<uint64_t>(s1.currentVariant()) != static_cast<uint64_t>(s2.currentVariant()) )\n" );
		f.write("\t\t\treturn false;\n" );

		f.write("\t\tswitch ( s1.currentVariant() )\n" );
		f.write("\t\t{\n" );
		f.write("\t\tcase Variants::unknown: break;\n" );

		for ( auto& it: s.getDiscriminatedUnionCases() )
		{
			std::string numId = std::to_string(it->numID);
			f.write("\t\tcase Variants::%s: // IDL CASE %s\n", it->name.c_str(), it->name.c_str() );
			f.write("\t\t{\n" );

			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_GeneratePublishableStructIsSameFn_MemberIterationBlock2( f.indent(3), cs, isPlainStruct );

			f.write( "\t\t\tbreak;\n" );
			f.write( "\t\t}\n" );
		}

		f.write( "\t\tdefault:\n" );
		f.write( "\t\t\treturn false; // unexpected\n" );
		f.write("\t\t}\n" );
	}
	else
		impl_GeneratePublishableStructIsSameFn_MemberIterationBlock2( f.indent(2), s, isPlainStruct );

	f.write("\t\treturn true;\n" );
}

void impl_GeneratePublishableStructIsSameFn2( FileWritter f, CompositeType& s )
{
	string genName = getGeneratedTypeName(s);

	f.write("\tstatic bool isSame(const ThisType& s1, const ThisType& s2)\n" );
	f.write( "\t{\n" );
	impl_GeneratePublishableStructIsSameFn3(f, s, false);
	f.write( "\t}\n" );

	f.write("\tstatic bool isSame(const ThisType& s1, const %s& s2)\n", genName.c_str() );
	f.write( "\t{\n" );
	impl_GeneratePublishableStructIsSameFn3(f, s, true);
	f.write( "\t}\n" );

	f.write("\tstatic bool isSame(const GMQ_COLL unique_ptr<ThisType>& s1, const GMQ_COLL unique_ptr<ThisType>& s2) { return isSame(*s1, *s2); }\n" );
	f.write("\tstatic bool isSame(const GMQ_COLL unique_ptr<ThisType>& s1, const %s& s2) { return isSame(*s1, s2); }\n", genName.c_str() );

	f.write("\tbool operator==(const ThisType& other) const { return isSame( *this, other ); }\n" );
	f.write("\tbool operator!=(const ThisType& other) const { return !this->operator==(other); }\n" );

	f.write("\tbool operator==(const %s& other) const { return isSame( *this, other ); }\n", genName.c_str() );
	f.write("\tbool operator!=(const %s& other) const { return !this->operator==(other); }\n", genName.c_str() );
}

void impl_generateSubscriberStruct( FileWritter f, Root& root, CompositeType& obj, const GenerationConfig& config )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::publishable );

	assert ( obj.isStruct4Publishing || obj.type == CompositeType::Type::publishable );

	string typeName = getSubscriberClassName( obj.name );
	if(obj.type == CompositeType::Type::discriminated_union)
	{
		f.write("class %s : private  %s_subscriber_base\n", typeName.c_str(), obj.name.c_str() );
	}
	else if( obj.type == CompositeType::Type::publishable )
	{
		f.write("class %s : public globalmq::marshalling::StateSubscriberBase<globalmq::marshalling::Buffer>\n", typeName.c_str() );
	}
	else
	{
		f.write("class %s\n", typeName.c_str() );
	}




	f.write("{\n" );
	f.write("public:\n" );
	f.write("\n" );
	impl_SubscriberVirtualHandlers( f.indent(), obj );

	f.write("\tusing ThisType = %s;\n", typeName.c_str() );
	f.write("\tusing CppType = %s;\n", typeName.c_str() );

	f.write("private:\n" );
	if( obj.type == CompositeType::Type::publishable )
		f.write("\tglobalmq::marshalling::SubscriberRegistryBase* const publishableRegistry = nullptr;\n" );

	if(!obj.isDiscriminatedUnion())
	{
		for ( auto& mbit: obj.getMembers() )
		{
			assert( mbit != nullptr );
			auto& m = *mbit;
			assert( m.type.kind != MessageParameterType::KIND::UNDEFINED );
			if ( m.type.kind == MessageParameterType::KIND::EXTENSION )
				continue;


			if ( m.type.kind == MessageParameterType::KIND::INTEGER || m.type.kind == MessageParameterType::KIND::UINTEGER ||
				m.type.kind == MessageParameterType::KIND::REAL || m.type.kind == MessageParameterType::KIND::ENUM)
				f.write("\t%s %s = 0;\n", getSubscriberCppType( m.type ).c_str(), m.name.c_str() );
			else
				f.write("\t%s %s;\n", getSubscriberCppType( m.type ).c_str(), m.name.c_str() );
		}
	}

	f.write("\npublic:\n" );

	if( obj.type == CompositeType::Type::publishable )
	{
		f.write("\tvirtual ~%s() { if(publishableRegistry) publishableRegistry->removeSubscriber(this); }\n", typeName.c_str() );
		f.write("\t%s() {}\n", typeName.c_str() );
		f.write("\t%s(globalmq::marshalling::SubscriberRegistryBase* registry) : publishableRegistry(registry) { if(publishableRegistry) publishableRegistry->addSubscriber(this); }\n", typeName.c_str() );
		f.write("\tvoid subscribe(const GMQ_COLL string& path) { if(!publishableRegistry) throw std::exception(); publishableRegistry->pathSubscribe(this, path); }\n" );
	}
	else
	{
		f.write("\tvirtual ~%s() {}\n", typeName.c_str() );
		f.write("\t%s() {}\n", typeName.c_str() );
	}

	f.write("\n" );

	impl_SubscriberGetters( f, obj );
	impl_GeneratePublishableStructIsSameFn2( f, obj );
	
	if( obj.type == CompositeType::Type::publishable )
	{
		std::string numStr = std::to_string(obj.numID);
		f.write("\n" );
		f.write("\t//////////////////////////////\n" );
		f.write("\tvirtual const char* publishableName() override { return \"%s\"; }\n", obj.name.c_str() );
		f.write("\tvirtual uint64_t stateTypeID() override { return %s; }\n", numStr.c_str() );
		f.write("\n" );
		f.write("\t// old interface\n" );
		f.write("\tvirtual void applyGmqMessageWithUpdates( globalmq::marshalling::GmqParser<globalmq::marshalling::Buffer>& parser ) override { throw std::exception(); }\n" );
		f.write("\tvirtual void applyJsonMessageWithUpdates( globalmq::marshalling::JsonParser<globalmq::marshalling::Buffer>& parser ) override { throw std::exception(); }\n" );
		f.write("\tvirtual void applyGmqStateSyncMessage( globalmq::marshalling::GmqParser<globalmq::marshalling::Buffer>& parser ) override { throw std::exception(); }\n" );
		f.write("\tvirtual void applyJsonStateSyncMessage( globalmq::marshalling::JsonParser<globalmq::marshalling::Buffer>& parser ) override { throw std::exception(); }\n" );
		f.write("\n" );
		f.write("\t// new interface\n" );
		f.write("\tvirtual void publishableApplyUpdates( globalmq::marshalling2::ParserBase& parser ) override\n" );
		f.write("\t{\n" );
		for (auto& each : config.parserNames)
			f.write("\t\tif(auto ptr = dynamic_cast<%s*>(&parser)) { applyMessageWithUpdates(*ptr); return; }\n", each.c_str() );

		f.write("\t\tthrow std::exception();\n" );
		f.write("\t}\n" );

		f.write("\tvirtual void publishableApplyStateSync( globalmq::marshalling2::ParserBase& parser ) override\n" );
		f.write("\t{\n" );
		for (auto& each : config.parserNames)
			f.write("\t\tif(auto ptr = dynamic_cast<%s*>(&parser)) { parseStateSyncMessage(*ptr); return; }\n", each.c_str() );

		f.write("\t\tthrow std::exception();\n" );
		f.write("\t}\n" );
		f.write("\t////////////////////////////////\n" );
		f.write("\n" );

		impl_GenerateApplyUpdateMessageMemberFn2(f);

		f.write("\n");

		impl_generateParseFunctionForPublishableState2(f);

		f.write("\n");
	}
	

	for (auto& each : config.parserNames)
	{
		impl_generateParseFunctionForPublishableStructStateSyncOrMessageInDepth2(f, root, obj, each);
		f.write("\n");

		f.write("\tstatic\n" );
		f.write("\tvoid parse_state_sync( %s& parser, ThisType& tt ) { tt.parse_state_sync(parser); }\n", each.c_str() );
		f.write("\n");

		impl_generateParseFunctionForPublishableStruct2(f, root, obj, each);
		f.write("\n");

		f.write("\tstatic\n" );
		f.write("\tbool parse_notify( %s& parser, ThisType& tt ) { return tt.parse_notify(parser); }\n", each.c_str() );
		f.write("\n");

		impl_generateContinueParsingFunctionForPublishableStruct2(f, root, obj, each);
		f.write("\n");

		f.write("\tstatic\n" );
		f.write("\tbool parse_continue( %s& parser, ThisType& tt, GMQ_COLL vector<uint64_t>& addr, uint64_t offset ) { return tt.parse_continue(parser, addr, offset); }\n", each.c_str() );
		f.write("\n");
	}

	// compose is used by concentrator only, but we put it here to be able to access private members
	for (auto& each : config.composerNames)
	{
		impl_generateComposeFunctionForPublishableStruct2( f, obj, each );
		
		f.write("\n");

		f.write("\tstatic\n" );
		f.write("\tvoid compose( %s& composer, ThisType& tt ) { tt.compose(composer); }\n", each.c_str() );
		f.write("\n");
	}


	f.write("};\n\n" );
}

void impl_generateConcentratorStruct( FileWritter f, CompositeType& obj, const GenerationConfig& config )
{
	assert( obj.type == CompositeType::Type::publishable );


	string typeName = fmt::format("{}_concentrator", obj.name);
	
	f.write("// template parameters are not really used but needed for legacy compatibility\n" );
	f.write("template<class InputBufferT, class ComposerT>\n" );
	f.write("class %s final : private  %s_subscriber, public globalmq::marshalling::StateConcentratorBase<InputBufferT, ComposerT>\n", typeName.c_str(), obj.name.c_str() );

	f.write("{\n" );
	f.write("public:\n" );
	f.write("\tusing ThisType = %s;\n", typeName.c_str() );
	f.write("\tusing CppType = ThisType;\n" );

	f.write("\n" );

	f.write("public:\n" );

	f.write("\t%s() {}\n", typeName.c_str() );


	if( obj.type == CompositeType::Type::publishable )
	{
		f.write("\npublic:\n" );

		f.write("\t//////////////////////////////\n" );
		f.write("\tvirtual const char* publishableName() override { return %s_subscriber::publishableName(); }\n", obj.name.c_str() );
		// f.write("\tvirtual uint64_t publishableId() override { return %s_subscriber::stateTypeID(); }\n", obj.name.c_str() );
		f.write("\n" );

		f.write("\t// old interface\n" );
		f.write("\tvirtual void applyGmqMessageWithUpdates( globalmq::marshalling::GmqParser<globalmq::marshalling::Buffer>& parser ) override { throw std::exception(); }\n" );
		f.write("\tvirtual void applyJsonMessageWithUpdates( globalmq::marshalling::JsonParser<globalmq::marshalling::Buffer>& parser ) override { throw std::exception(); }\n" );
		f.write("\tvirtual void applyGmqStateSyncMessage( globalmq::marshalling::GmqParser<globalmq::marshalling::Buffer>& parser ) override { throw std::exception(); }\n" );
		f.write("\tvirtual void applyJsonStateSyncMessage( globalmq::marshalling::JsonParser<globalmq::marshalling::Buffer>& parser ) override { throw std::exception(); }\n" );
		f.write("\tvirtual void generateStateSyncMessage( ComposerT& composer ) override { throw std::exception(); }\n" );

		f.write("\n" );
		f.write("\t// new interface\n" );
		f.write("\tvirtual void publishableApplyUpdates( globalmq::marshalling2::ParserBase& parser ) override { %s_subscriber::publishableApplyUpdates(parser); }\n", obj.name.c_str() );

		f.write("\tvirtual void publishableApplyStateSync( globalmq::marshalling2::ParserBase& parser ) override { %s_subscriber::publishableApplyStateSync(parser); }\n", obj.name.c_str() );

		f.write("\tvirtual void publishableGenerateStateSync( globalmq::marshalling2::ComposerBase& composer ) override\n" );
		f.write("\t{\n" );
		for (auto& each : config.composerNames)
			f.write("\t\tif(auto ptr = dynamic_cast<%s*>(&composer)) { compose(*ptr); return; }\n", each.c_str() );

		f.write("\t\tthrow std::exception();\n" );
		f.write("\t}\n" );
		f.write("\t//////////////////////////////\n" );
	}
	
	f.write("};\n\n" );
}


void generatePublishable2Struct( FILE* header, Root& root, CompositeType& obj, const GenerationConfig& config )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::publishable );

	assert ( obj.isStruct4Publishing || obj.type == CompositeType::Type::publishable );

	FileWritter f(header, 0);

	if(obj.isDiscriminatedUnion())
		generateDiscriminatedUnionObject(header, obj, true);

	impl_generateSubscriberStruct(f, root, obj, config);

	if(obj.type == CompositeType::Type::publishable)
		impl_generateConcentratorStruct(f, obj, config);
}

}//namespace cplusplus
