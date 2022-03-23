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

namespace cplusplus
{
const char* paramTypeToParser( MessageParameterType::KIND kind )
{
	switch( kind )
	{
		case MessageParameterType::KIND::INTEGER: return "parseSignedInteger";
		case MessageParameterType::KIND::UINTEGER: return "parseUnsignedInteger";
		case MessageParameterType::KIND::REAL: return "parseReal";
		case MessageParameterType::KIND::CHARACTER_STRING: return "parseString";
		default: return nullptr;
	}
}


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

const char* paramTypeToLeafeParser( MessageParameterType::KIND kind )
{
	switch( kind )
	{
		case MessageParameterType::KIND::INTEGER: return "publishableParseLeafeInteger";
		case MessageParameterType::KIND::UINTEGER: return "publishableParseLeafeUnsignedInteger";
		case MessageParameterType::KIND::REAL: return "publishableParseLeafeReal";
		case MessageParameterType::KIND::CHARACTER_STRING: return "publishableParseLeafeString";
		default: return nullptr;
	}
}

std::string impl_generateComposeFunctionNameForStructMemeberOfPublishable( MessageParameter& s )
{
	if ( s.type.kind == MessageParameterType::KIND::STRUCT )
		return fmt::format( "publishable_{}_{}_compose", CompositeType::type2string( CompositeType::Type::structure ), s.type.name );
	else if ( s.type.kind == MessageParameterType::KIND::DISCRIMINATED_UNION )
		return fmt::format( "publishable_{}_{}_compose", CompositeType::type2string( CompositeType::Type::discriminated_union ), s.type.name );
	else
	{
		assert( false );
		return "";
	}
}

std::string impl_generateComposeFunctionNameForPublishableStruct( CompositeType& s )
{
	assert( s.type == CompositeType::Type::structure || s.isDiscriminatedUnion() );
	return fmt::format( "publishable_{}_{}_compose", s.type2string(), s.name );
}

std::string impl_generateParseFunctionNameForStructMemeberOfPublishable( MessageParameter& s )
{
	if ( s.type.kind == MessageParameterType::KIND::STRUCT )
		return fmt::format( "publishable_{}_{}_parse", CompositeType::type2string( CompositeType::Type::structure ), s.type.name );
	else if ( s.type.kind == MessageParameterType::KIND::DISCRIMINATED_UNION )
		return fmt::format( "publishable_{}_{}_parse", CompositeType::type2string( CompositeType::Type::discriminated_union ), s.type.name );
	else
	{
		assert( false );
		return "";
	}
}

std::string impl_generateParseFunctionNameForPublishableStruct( CompositeType& s )
{
	assert( s.type == CompositeType::Type::structure || s.isDiscriminatedUnion() );
	return fmt::format( "publishable_{}_{}_parse", s.type2string(), s.name );
}


void impl_GeneratePublishableStateMemberPresenceCheckingBlock( FILE* header, Root& root, CompositeType& s )
{
	assert( s.type == CompositeType::Type::publishable || ( ( s.type == CompositeType::Type::structure || s.type == CompositeType::Type::discriminated_union ) && s.isStruct4Publishing ) );
	if ( s.isDiscriminatedUnion() )
	{
		// TODO: revise DU
		/*for ( auto& it: s.getDiscriminatedUnionCases() )
		{
			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			for ( auto& it : cs.getMembers() )
			{
				assert( it != nullptr );
				fprintf( header, "\tstatic constexpr bool has_%s = has_%s_getter<T>;\n", it->name.c_str(), it->name.c_str() );
				fprintf( header, "\tstatic_assert( has_%s, \"type T must have member function T::get_%s() of a return type corresponding to IDL type %s\" );\n", it->name.c_str(), it->name.c_str(), impl_parameterTypeToDescriptiveString( root, it->type ).c_str() );
				fprintf( header, "\tstatic constexpr bool has_%s = has_%s_setter<T>;\n", it->name.c_str(), it->name.c_str() );
				fprintf( header, "\tstatic_assert( has_%s, \"type T must have void member function T::set_%s with a single input parameter of a type corresponding to IDL type %s\" );\n", it->name.c_str(), it->name.c_str(), impl_parameterTypeToDescriptiveString( root, it->type ).c_str() );
			}
		}*/
	}
	else
	{
		for ( auto& it : s.getMembers() )
		{
			assert( it != nullptr );
			fprintf( header, "\tstatic constexpr bool has_%s = has_%s_member<T>;\n", it->name.c_str(), it->name.c_str() );
			fprintf( header, "\tstatic_assert( has_%s, \"type T must have member T::%s of a type corresponding to IDL type %s\" );\n", it->name.c_str(), it->name.c_str(), impl_parameterTypeToDescriptiveString( root, it->type ).c_str() );
		}
	}
	fprintf( header, "\n" );
}

void impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock_MemberIterationBlock( FILE* header, Root& root, CompositeType& s, const char* offset )
{
	// NOTE: in case of updates here revise impl_GenerateDiscriminatedUnionVariantUpdateNotifierPresenceCheckingBlock() as well
	assert( s.type != CompositeType::Type::discriminated_union );
	for ( auto& member : s.getMembers() )
	{
		assert( member != nullptr );
		fprintf( header, "%sstatic constexpr bool has_void_update_notifier_for_%s = has_void_update_notifier_call_for_%s<T>;\n", offset, member->name.c_str(), member->name.c_str() );
		fprintf( header, "%sstatic constexpr bool has_update_notifier_for_%s = has_update_notifier_call_for_%s<T, %s>;\n", offset, member->name.c_str(), member->name.c_str(), impl_templateMemberTypeName( "T", *member).c_str() );
		fprintf( header, "%sstatic constexpr bool has_any_notifier_for_%s = has_void_update_notifier_for_%s || has_update_notifier_for_%s;\n", 
			offset, member->name.c_str(), member->name.c_str(), member->name.c_str()
		);
		if ( member->type.kind == MessageParameterType::KIND::VECTOR )
		{
			fprintf( header, "%susing %sT = %s;\n", offset, member->name.c_str(), impl_templateMemberTypeName( "T", *member).c_str() );
			fprintf( header, "%sstatic constexpr bool has_void_insert_notifier_for_%s = has_void_insert_notifier_call_for_%s<T>;\n", offset, member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_insert_notifier2_for_%s = has_insert_notifier_call2_for_%s<T>;\n", offset, member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_insert_notifier3_for_%s = has_insert_notifier_call3_for_%s<T, GMQ_COLL vector<%sT>&>;\n", offset, member->name.c_str(), member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_void_erased_notifier_for_%s = has_void_erased_notifier_call_for_%s<T>;\n", offset, member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_erased_notifier2_for_%s = has_erased_notifier_call2_for_%s<T>;\n", offset, member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_erased_notifier3_for_%s = has_erased_notifier_call3_for_%s<T, GMQ_COLL vector<%sT>&>;\n", offset, member->name.c_str(), member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_void_element_updated_notifier_for_%s = has_element_updated_void_notifier_call_for_%s<T>;\n", offset, member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_element_updated_notifier_for_%s = has_element_updated_notifier_call_for_%s<T>;\n", offset, member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_full_element_updated_notifier_for_%s = has_full_element_updated_notifier_call_for_%s<T, %sT&>;\n", offset, member->name.c_str(), member->name.c_str(), member->name.c_str() );
		}
		else if ( member->type.kind == MessageParameterType::KIND::DICTIONARY )
		{
			fprintf( header, "%susing %sT = %s;\n", offset, member->name.c_str(), impl_templateMemberTypeName( "T", *member).c_str() );
			fprintf( header, "%s//// TODO: revise notifier list !!!!!!!!!!!!!!!!!!!!!!!!!\n", offset );
			fprintf( header, "%sstatic constexpr bool has_void_insert_notifier_for_%s = has_void_insert_notifier_call_for_%s<T>;\n", offset, member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_insert_notifier2_for_%s = has_insert_notifier_call2_for_%s<T, typename %sT::key_type&>;\n", offset, member->name.c_str(), member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_insert_notifier3_for_%s = has_insert_notifier_call3_for_%s<T, typename %sT::key_type&, typename %sT::mapped_type&>;\n", offset, member->name.c_str(), member->name.c_str(), member->name.c_str(), member->name.c_str() );

			fprintf( header, "%sstatic constexpr bool has_void_removed_notifier_for_%s = has_void_removed_notifier_call_for_%s<T>;\n", offset, member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_removed_notifier2_for_%s = has_removed_notifier_call2_for_%s<T, typename %sT::key_type>;\n", offset, member->name.c_str(), member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_removed_notifier3_for_%s = has_removed_notifier_call3_for_%s<T, typename %sT::key_type, typename %sT::mapped_type&>;\n", offset, member->name.c_str(), member->name.c_str(), member->name.c_str(), member->name.c_str() );

			fprintf( header, "%sstatic constexpr bool has_void_value_updated_notifier_for_%s = has_value_updated_void_notifier_call_for_%s<T>;\n", offset, member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_value_updated_notifier_for_%s = has_value_updated_notifier_call_for_%s<T, typename %sT::key_type&>;\n", offset, member->name.c_str(), member->name.c_str(), member->name.c_str() );
			fprintf( header, "%sstatic constexpr bool has_full_value_updated_notifier_for_%s = has_full_value_updated_notifier_call_for_%s<T, typename %sT::key_type&, typename %sT::mapped_type&>;\n", offset, member->name.c_str(), member->name.c_str(), member->name.c_str(), member->name.c_str() );
		}
	}
}

void impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock( FILE* header, Root& root, CompositeType& s, const char* offset )
{
	/* Short summary:
	*  template<typename StateT> ...
	*	has_void_update_notifier_call_for_%s            notifyUpdated_%s<T>()
	*	has_update_notifier_call_for_%s                 notifyUpdated_%s<T, MemberT>(MemberT&)
	*	// for vectors
	*	has_void_insert_notifier_call_for_%s            notifyInserted_%s<T>()
	*	has_insert_notifier_call_for_%s                 notifyInserted_%s<T>(index_type_for_array_notifiers, index_type_for_array_notifiers)
	*	has_void_erased_notifier_call_for_%s            notifyErased_%s<T>()
	*	has_erased_notifier_call2_for_%s                notifyErased_%s<T>(index_type_for_array_notifiers, index_type_for_array_notifiers)
	*	has_erased_notifier_call3_for_%s                notifyErased_%s<T, MemberT>(index_type_for_array_notifiers, index_type_for_array_notifiers, MemberT>()	
	*	has_element_updated_void_notifier_call_for_%s   notifyElementUpdated_%s<T>()
	*	has_element_updated_notifier_call_for_%s        notifyElementUpdated_%s<T>(index_type_for_array_notifiers)
	*	has_full_element_updated_notifier_call_for_%s   notifyElementUpdated_%s<T, MemberT>(index_type_for_array_notifiers, MemberT)
	*	// for dictionaries
	*	has_void_insert_notifier_call_for_%s            notifyInserted_%s<T>()
	*	has_insert_notifier_call_for_%s                 notifyInserted_%s<T>(index_type_for_array_notifiers, index_type_for_array_notifiers)
	*	has_void_removed_notifier_call_for_%s           notifyRemoved_%s<T>()
	*	has_removed_notifier_call2_for_%s               notifyRemoved_%s<T>(index_type_for_array_notifiers, index_type_for_array_notifiers)
	*	has_removed_notifier_call3_for_%s               notifyRemoved_%s<T, MemberT>(index_type_for_array_notifiers, index_type_for_array_notifiers, MemberT>()	
	*	has_value_updated_void_notifier_call_for_%s     notifyValueUpdated_%s<T>()
	*	has_value_updated_notifier_call_for_%s          notifyValueUpdated_%s<T>(index_type_for_array_notifiers)
	*	has_full_value_updated_notifier_call_for_%s     notifyValueUpdated_%s<T, MemberT>(index_type_for_array_notifiers, MemberT)
	*/
	assert( s.type == CompositeType::Type::publishable || ( ( s.type == CompositeType::Type::structure || s.isDiscriminatedUnion() ) && s.isStruct4Publishing ) ); // TODO: revise DU
	if ( s.isDiscriminatedUnion() )
	{
		for ( auto& it: s.getDiscriminatedUnionCases() )
		{
			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock_MemberIterationBlock( header, root, cs, offset );
		}
	}
	else
		impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock_MemberIterationBlock( header, root, s, offset );
	fprintf( header, "%sstatic constexpr bool has_full_update_notifier = has_full_update_notifier_call<T>;\n", offset );
	fprintf( header, "%sstatic constexpr bool has_update_notifier = has_update_notifier_call<T>;\n", offset );
	fprintf( header, "\n" );
}

void impl_generateApplyUpdateForSimpleType( FILE* header, MessageParameter& member, bool addReportChanges, bool isLeafe, std::string offset, const std::string& parserType )
{
	string parseProcessorType = isLeafe ? paramTypeToLeafeParser( member.type.kind ) : paramTypeToParser( member.type.kind );
	if ( addReportChanges )
		fprintf( header, "%sif constexpr( has_any_notifier_for_%s || reportChanges || has_update_notifier )\n", offset.c_str(), member.name.c_str() );
	else
		fprintf( header, "%sif constexpr( has_any_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\t%s oldVal = t.%s;\n", offset.c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	// if ( isLeafe )
	// 	fprintf( header, "%s\tparser.leafeBegin();\n", offset.c_str() );
	// else
		// fprintf( header, "%s\tparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\tt.%s = parser.%s();\n", offset.c_str(), impl_memberOrAccessFunctionName(member).c_str(), paramTypeToParser(member.type.kind));
	fprintf( header, "%s\tbool currentChanged = oldVal != t.%s;\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\tif ( currentChanged )\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	if ( addReportChanges )
	{
		fprintf( header, "%s\t\tif constexpr ( reportChanges || has_update_notifier )\n", offset.c_str() );
		fprintf( header, "%s\t\t\tchanged = true;\n", offset.c_str() );
	}
	fprintf( header, "%s\t\tif constexpr ( has_void_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\tt.notifyUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\tif constexpr ( has_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\tt.notifyUpdated_%s( oldVal );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );
	fprintf( header, "%selse\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str());
	// fprintf( header, "%s\tparser.nextElement();\n", offset.c_str() );
	// if (isLeafe)
	// 	fprintf( header, "%s\tparser.leafeBegin();\n", offset.c_str());
	// else
		// fprintf( header, "%s\tparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str());
	fprintf( header, "%s\tt.%s = parser.%s();\n", offset.c_str(), impl_memberOrAccessFunctionName(member).c_str(), paramTypeToParser(member.type.kind));
	fprintf( header, "%s}\n", offset.c_str());
}

void impl_generateApplyUpdateForSimpleTypeNoNotifiers( FILE* header, MessageParameter& member, bool isLeafe, std::string offset)
{
	// fprintf( header, "%s\tparser.nextElement();\n", offset.c_str() );
	// if (isLeafe)
	// 	fprintf(header, "%s\tparser.leafeBegin();\n", offset.c_str());
	// else
	// 	fprintf(header, "%s\tparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str());
	fprintf(header, "%s\tt.%s = parser.%s();\n", offset.c_str(), impl_memberOrAccessFunctionName(member).c_str(), paramTypeToParser(member.type.kind));
}

void impl_generateApplyUpdateForStructItself( FILE* header, MessageParameter& member, bool addReportChanges, std::string offset, const std::string& parserType )
{

	fprintf( header, "%sif constexpr( has_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\t%s temp_%s;\n", offset.c_str(), impl_templateMemberTypeName( "T", member).c_str(), member.name.c_str() );
//	fprintf( header, "%s\t%s::copy<%s, %s>( t.%s, temp_%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_templateMemberTypeName( "T", member).c_str(), member.name.c_str(), member.name.c_str() );
	fprintf( header, "%s\t%s::copy<%s>( t.%s, temp_%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), member.name.c_str() );
	fprintf( header, "%s\tbool changedCurrent = %s::parse<%s, bool>( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\tif ( changedCurrent )\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	if ( addReportChanges )
	{
		fprintf( header, "%s\t\tif constexpr ( reportChanges || has_update_notifier )\n", offset.c_str() );
		fprintf( header, "%s\t\t\tchanged = true;\n", offset.c_str() );
	}
	fprintf( header, "%s\t\tif constexpr( has_void_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\tt.notifyUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\tif constexpr( has_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\tt.notifyUpdated_%s( temp_%s );\n", offset.c_str(), member.name.c_str(), member.name.c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );

	fprintf( header, "%selse if constexpr( has_void_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tbool changedCurrent = %s::parse<%s, bool>( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\tif ( changedCurrent )\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	if ( addReportChanges )
	{
		fprintf( header, "%s\t\tif constexpr ( reportChanges || has_update_notifier )\n", offset.c_str() );
		fprintf( header, "%s\t\t\tchanged = true;\n", offset.c_str() );
	}
	fprintf( header, "%s\t\tt.notifyUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );

	fprintf( header, "%selse\n", offset.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	if ( addReportChanges )
	{
		fprintf( header, "%s\tif constexpr ( reportChanges || has_update_notifier )\n", offset.c_str() );
		fprintf( header, "%s\t\tchanged = %s::parse<%s, bool>( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
		fprintf( header, "%s\telse\n", offset.c_str() );
		fprintf( header, "%s\t\t%s::parse( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	}
	else
		fprintf( header, "%s\t%s::parse( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s}\n", offset.c_str() );
}

void impl_generateApplyUpdateForStructItselfNoNotifiers( FILE* header, MessageParameter& member, std::string offset )
{
	fprintf( header, "%s%s::parse( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
}

void impl_generateApplyUpdateForFurtherProcessingInStruct( FILE* header, MessageParameter& member, bool addOffsetInAddr, bool addReportChanges, bool forwardAddress, std::string offset, const std::string& parserType )
{
	assert( (!addOffsetInAddr) || (addOffsetInAddr && forwardAddress) );
	const char* offsetPlusStr = addOffsetInAddr ? "offset + " : "";
	fprintf( header, "%sif constexpr( has_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\t%s temp_%s;\n", offset.c_str(), impl_templateMemberTypeName( "T", member).c_str(), member.name.c_str() );
//	fprintf( header, "%s\t%s::copy<%s, %s>( t.%s, temp_%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), member.name.c_str() );
	fprintf( header, "%s\t%s::copy<%s>( t.%s, temp_%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), member.name.c_str() );
	if ( forwardAddress )
		fprintf( header, "%s\tbool changedCurrent = %s::parse<%s, bool>( parser, t.%s, addr, %s1 );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), offsetPlusStr );
	else
		fprintf( header, "%s\tbool changedCurrent = %s::parse<%s, bool>( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\tif ( changedCurrent )\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	fprintf( header, "%s\t\tchanged = true;\n", offset.c_str() );
	if ( addReportChanges )
	{
		fprintf( header, "%s\t\tif constexpr ( reportChanges || has_update_notifier )\n", offset.c_str() );
		fprintf( header, "%s\t\t\tchanged = true;\n", offset.c_str() );
	}
	fprintf( header, "%s\t\tif constexpr( has_void_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\tt.notifyUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\tif constexpr( has_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\tt.notifyUpdated_%s( temp_%s );\n", offset.c_str(), member.name.c_str(), member.name.c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );

	fprintf( header, "%selse if constexpr( has_void_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	if ( forwardAddress )
		fprintf( header, "%s\tbool changedCurrent = %s::parse<%s, bool>( parser, t.%s, addr, %s1 );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), offsetPlusStr );
	else
		fprintf( header, "%s\tbool changedCurrent = %s::parse<%s, bool>( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\tif ( changedCurrent )\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	if ( addReportChanges )
	{
		fprintf( header, "%s\t\tif constexpr ( reportChanges || has_update_notifier )\n", offset.c_str() );
		fprintf( header, "%s\t\t\tchanged = true;\n", offset.c_str() );
	}
	fprintf( header, "%s\t\tt.notifyUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );

	if ( addReportChanges )
	{
		fprintf( header, "%selse if constexpr ( reportChanges || has_update_notifier )\n", offset.c_str() );
		if ( forwardAddress )
			fprintf( header, "%s\tchanged = %s::parse<%s, bool>( parser, t.%s, addr, %s1 );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), offsetPlusStr );
		else
			fprintf( header, "%s\tchanged = %s::parse<%s, bool>( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_templateMemberTypeName( "T", member).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
		fprintf( header, "%selse\n", offset.c_str() );
		if ( forwardAddress )
			fprintf( header, "%s\t%s::parse( parser, t.%s, addr, %s1 );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), offsetPlusStr );
		else
			fprintf( header, "%s\t%s::parse( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	}
	else
	{
		fprintf( header, "%selse\n", offset.c_str() );
		if ( forwardAddress )
			fprintf( header, "%s\t%s::parse( parser, t.%s, addr, %s1 );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), offsetPlusStr );
		else
			fprintf( header, "%s\t%s::parse( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	}
}

void impl_generateApplyUpdateForFurtherProcessingInStructNoNotifiers( FILE* header, MessageParameter& member, bool addOffsetInAddr, bool forwardAddress, std::string offset )
{
	assert( (!addOffsetInAddr) || (addOffsetInAddr && forwardAddress) );
	const char* offsetPlusStr = addOffsetInAddr ? "offset + " : "";

	if ( forwardAddress )
		fprintf( header, "%s%s::parse( parser, t.%s, addr, %s1 );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), offsetPlusStr );
	else
		fprintf( header, "%s%s::parse( parser, t.%s );\n", offset.c_str(), getHelperClassName( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
}

void impl_generateApplyUpdateForFurtherProcessingInDictionary( FILE* header, Root& root, MessageParameter& member, bool addOffsetInAddr, bool addReportChanges, std::string offset, const std::string& parserType )
{
//	assert( (!addOffsetInAddr) || (addOffsetInAddr && forwardAddress) );
	const char* offsetPlusStr = addOffsetInAddr ? "offset + " : "";

	fprintf( header, "%s%s oldDictionaryVal;\n", offset.c_str(),  impl_templateMemberTypeName( "T", member).c_str() );
	fprintf( header, "%sbool currentChanged = false;\n", offset.c_str() );
	fprintf( header, "%sconstexpr bool alwaysCollectChanges = has_any_notifier_for_%s;\n", offset.c_str(), member.name.c_str() );
//	fprintf( header, "%sif constexpr( alwaysCollectChanges )\n", offset.c_str() );
//	fprintf( header, "%s\t::globalmq::marshalling::impl::copyDictionary<%s, %s, %s>( t.%s, oldDictionaryVal );\n", offset.c_str(), impl_templateMemberTypeName( "T", member).c_str(), dictionaryKeyTypeToLibTypeOrTypeProcessor( member.type, root ).c_str(), dictionaryValueTypeToLibTypeOrTypeProcessor( member.type, root ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );

fprintf( header, "%s//~~~~~~~~~~XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n", offset.c_str() );

//	const char* libType = paramTypeToLibType( member.type.dictionaryValueKind );
	assert( member.type.structIdx < root.structs.size() );
	fprintf( header, "%sif ( addr.size() > %s1 ) // one of actions over elements of the dictionary\n", offset.c_str(), offsetPlusStr );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tsize_t action = addr[%s1];\n", offset.c_str(), offsetPlusStr );

	fprintf( header, "%s\tif ( addr.size() > %s2 ) // update for a value of a particular dictionary element\n", offset.c_str(), offsetPlusStr );
	fprintf( header, "%s\t{\n", offset.c_str() );

	if ( member.type.dictionaryValueKind == MessageParameterType::KIND::STRUCT || member.type.dictionaryValueKind == MessageParameterType::KIND::DISCRIMINATED_UNION )
	{
		fprintf( header, "%s\t\tthrow std::exception(); // not implemented yet\n", offset.c_str() );
		fprintf( header, "%s\t\t//typename %s::value_type& value = t.%s[pos];\n", offset.c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	}
	else
		fprintf( header, "%s\t\tthrow std::exception(); // deeper address is unrelated to simple type of dictionary values (IDL type of t.%s elements is %s)\n", offset.c_str(), member.name.c_str(), impl_kindToString( member.type.dictionaryValueKind ) );
				
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s\telse // update of one or more elelments as a whole\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	fprintf( header, "%s\t\tswitch ( action )\n", offset.c_str() );
	fprintf( header, "%s\t\t{\n", offset.c_str() );

	fprintf( header, "%s\t\t\tcase ActionOnDictionary::remove:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::key_type key;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseKey( parser, key );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\tauto f = t.%s.find( key );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tif ( f == t.%s.end() )\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\t\tthrow std::exception();\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::mapped_type oldVal;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\toldVal = f->second;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tt.%s.erase( key );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tif constexpr ( has_removed_notifier3_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\tt.notifyRemoved_%s( key, oldVal );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\tif constexpr ( has_removed_notifier2_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\tt.notifyRemoved_%s( key );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\tif constexpr ( has_void_removed_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\tt.notifyRemoved_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\tif constexpr ( alwaysCollectChanges )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tcurrentChanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\tcase ActionOnDictionary::update_value:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::key_type key;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseKey( parser, key );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\tauto f = t.%s.find( key );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tif ( f == t.%s.end() )\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\t\tthrow std::exception();\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\ttypename %s::mapped_type& value = f->second;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );

	fprintf( header, "%s\t\t\t\ttypename %s::mapped_type oldValue;\n", offset.c_str(), impl_templateMemberTypeName( "T", member, true ).c_str() );
	switch ( member.type.dictionaryValueKind )
	{
		case MessageParameterType::KIND::INTEGER:
		case MessageParameterType::KIND::UINTEGER:
		case MessageParameterType::KIND::REAL:
		case MessageParameterType::KIND::CHARACTER_STRING:
			fprintf( header, "%s\t\t\t\toldValue = value;\n", offset.c_str() );
			break;
		case MessageParameterType::KIND::STRUCT:
		case MessageParameterType::KIND::DISCRIMINATED_UNION:
			fprintf( header, "%s\t\t\t\t%s::copy( value, oldValue );\n", offset.c_str(), getHelperClassName( *(root.structs[member.type.structIdx]) ).c_str() );
			break;
		default:
			fprintf( header, "%s\t\t\t\tstatic_assert(false);\n", offset.c_str() );
			break;
	}
				
	fprintf( header, "%s\t\t\t\tif constexpr ( has_full_value_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\t\tcurrentChanged = globalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseValueAndCompare( parser, value, oldValue );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\t\tif ( currentChanged )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tchanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tt.notifyValueUpdated_%s( key, oldValue );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tif constexpr ( has_value_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\t\tt.notifyValueUpdated_%s( key );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tif constexpr ( has_void_value_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\t\tt.notifyValueUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str()  );
				
	fprintf( header, "%s\t\t\t\telse if constexpr ( has_value_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tcurrentChanged = globalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseValueAndCompare( parser, value, oldValue );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\t\tif ( currentChanged )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tchanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tt.notifyValueUpdated_%s( key );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tif constexpr ( has_void_value_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\t\tt.notifyValueUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\telse if constexpr ( has_void_value_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tcurrentChanged = globalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseValueAndCompare( parser, value, oldValue );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\t\tif ( currentChanged )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tchanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tt.notifyValueUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\telse\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tif constexpr ( alwaysCollectChanges )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tcurrentChanged = globalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseValueAndCompare( parser, value, oldValue );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\t\telse\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseValue( parser, value );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\tcase ActionOnDictionary::insert:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::key_type key;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseKey( parser, key );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::mapped_type value;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseValue( parser, value );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );

	fprintf( header, "%s\t\t\t\tif constexpr ( has_insert_notifier3_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t%s oldVal;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::copy( t.%s, oldVal );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\t\tt.%s.insert( std::make_pair( key, value ) );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				
	fprintf( header, "%s\t\t\t\t\tt.notifyInserted_%s( key, oldVal );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\tif constexpr ( has_insert_notifier2_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tt.notifyInserted_%s( key );\n", offset.c_str(), member.name.c_str() );
				
	fprintf( header, "%s\t\t\t\t\tif constexpr ( has_void_insert_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tt.notifyInserted_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\telse\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\t\tt.%s.insert( std::make_pair( key, value ) );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\t\tif constexpr ( has_insert_notifier2_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tt.notifyInserted_%s( key );\n", offset.c_str(), member.name.c_str() );
				
	fprintf( header, "%s\t\t\t\t\tif constexpr ( has_void_insert_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tt.notifyInserted_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );
				
	fprintf( header, "%s\t\t\t\tif constexpr ( alwaysCollectChanges )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tcurrentChanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\tdefault:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tthrow std::exception();\n", offset.c_str() );
	fprintf( header, "%s\t\t}\n", offset.c_str() );
	// fprintf( header, "%s\t\t::globalmq::marshalling::impl::parseStateUpdateBlockEnd( parser );\n", offset.c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
			
//fprintf( header, "//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );
	fprintf( header, "%selse // replacement of the whole dictionary\n", offset.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tparser.leafeBegin();\n", offset.c_str() );
	fprintf( header, "\n" );
	fprintf( header, "%s\tif constexpr( alwaysCollectChanges )\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	fprintf( header, "%s\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parse( parser, t.%s );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\tcurrentChanged = !globalmq::marshalling2::PublishableDictionaryProcessor2<%s>::isSame( oldDictionaryVal, t.%s );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s\telse\n", offset.c_str() );
	fprintf( header, "%s\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parse( parser, t.%s );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "\n" );
	// fprintf( header, "%s\t::globalmq::marshalling::impl::publishableParseLeafeDictionaryEnd( parser );\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );
	fprintf( header, "\n" );
	fprintf( header, "%sif ( currentChanged )\n", offset.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tchanged = true;\n", offset.c_str() );
	fprintf( header, "%s\tif constexpr( has_void_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\tt.notifyUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\tif constexpr( has_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\tt.notifyUpdated_%s( oldDictionaryVal );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );

	if ( addReportChanges )
	{
		fprintf( header, "\n" );
		fprintf( header, "%sif constexpr( alwaysCollectChanges )\n", offset.c_str() );
		fprintf( header, "%s\treturn currentChanged;\n", offset.c_str() );
	}
}

void impl_generateApplyUpdateForFurtherProcessingInVector( FILE* header, Root& root, MessageParameter& member, bool addOffsetInAddr, bool addReportChanges, std::string offset, const std::string& parserType )
{
//	assert( (!addOffsetInAddr) || (addOffsetInAddr && forwardAddress) );
	const char* offsetPlusStr = addOffsetInAddr ? "offset + " : "";

	fprintf( header, "%s%s oldVectorVal;\n", offset.c_str(),  impl_templateMemberTypeName( "T", member).c_str() );
	fprintf( header, "%sbool currentChanged = false;\n", offset.c_str() );
	fprintf( header, "%sconstexpr bool alwaysCollectChanges = has_any_notifier_for_%s;\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%sif constexpr( alwaysCollectChanges )\n", offset.c_str() );
	fprintf( header, "%s\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::copy( t.%s, oldVectorVal );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );

//fprintf( header, "//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n", offset.c_str() );

//	const char* libType = paramTypeToLibType( member.type.vectorElemKind );
	assert( member.type.structIdx < root.structs.size() );
	fprintf( header, "%sif ( addr.size() > %s1 ) // one of actions over elements of the vector\n", offset.c_str(), offsetPlusStr );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tsize_t pos = addr[%s1];\n", offset.c_str(), offsetPlusStr );
	fprintf( header, "%s\tif ( pos > t.%s.size() )\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\tthrow std::exception();\n", offset.c_str() );

	fprintf( header, "%s\tif ( addr.size() > %s2 ) // update for a member of a particular vector element\n", offset.c_str(), offsetPlusStr );
	fprintf( header, "%s\t{\n", offset.c_str() );

	if ( member.type.vectorElemKind == MessageParameterType::KIND::STRUCT || member.type.vectorElemKind == MessageParameterType::KIND::DISCRIMINATED_UNION )
	{
		fprintf( header, "%s\t\ttypename %s::value_type& value = t.%s[pos];\n", offset.c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );

		fprintf( header, "%s\t\tif constexpr ( has_full_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t{\n", offset.c_str() );
		fprintf( header, "%s\t\t\ttypename %s::value_type oldValue;\n", offset.c_str(), impl_templateMemberTypeName( "T", member, true ).c_str() );
		switch ( member.type.vectorElemKind )
		{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
				fprintf( header, "%s\t\t\toldValue = value;\n", offset.c_str() );
				break;
			case MessageParameterType::KIND::STRUCT:
			case MessageParameterType::KIND::DISCRIMINATED_UNION:
				fprintf( header, "%s\t\t\t%s::copy( value, oldValue );\n", offset.c_str(), getHelperClassName( *(root.structs[member.type.structIdx]) ).c_str() );
				break;
			default:
				fprintf( header, "%s\t\t\t\tstatic_assert(false);\n", offset.c_str() );
				break;
		}
		fprintf( header, "%s\t\t\tcurrentChanged = %s::parse<typename %s::value_type, bool>( parser, value, addr, %s2 );\n", offset.c_str(), getHelperClassName( *(root.structs[member.type.structIdx]) ).c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), offsetPlusStr );
		fprintf( header, "%s\t\t\tif ( currentChanged )\n", offset.c_str() );
		fprintf( header, "%s\t\t\t{\n", offset.c_str() );
		fprintf( header, "%s\t\t\t\tchanged = true;\n", offset.c_str() );
		fprintf( header, "%s\t\t\t\tt.notifyElementUpdated_%s( pos, oldValue );\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t\t\tif constexpr ( has_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t\t\t\tt.notifyElementUpdated_%s();\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t\t\tif constexpr ( has_void_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t\t\t\tt.notifyElementUpdated_%s();\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t\t}\n", offset.c_str() );
		fprintf( header, "%s\t\t}\n", offset.c_str() );

		fprintf( header, "%s\t\telse if constexpr ( has_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t{\n", offset.c_str() );
		fprintf( header, "%s\t\t\tcurrentChanged = %s::parse<typename %s::value_type, bool>( parser, value, addr, %s2 );\n", offset.c_str(), getHelperClassName( *(root.structs[member.type.structIdx]) ).c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), offsetPlusStr );
		fprintf( header, "%s\t\t\tif ( currentChanged )\n", offset.c_str() );
		fprintf( header, "%s\t\t\t{\n", offset.c_str() );
		fprintf( header, "%s\t\t\t\tchanged = true;\n", offset.c_str() );
		fprintf( header, "%s\t\t\t\tt.notifyElementUpdated_%s( pos );\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t\t\tif constexpr ( has_void_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t\t\t\tt.notifyElementUpdated_%s();\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t\t}\n", offset.c_str() );
		fprintf( header, "%s\t\t}\n", offset.c_str() );

		fprintf( header, "%s\t\telse if constexpr ( has_void_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t{\n", offset.c_str() );
		fprintf( header, "%s\t\t\tcurrentChanged = %s::parse<typename %s::value_type, bool>( parser, value, addr, %s2 );\n", offset.c_str(), getHelperClassName( *(root.structs[member.type.structIdx]) ).c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), offsetPlusStr );
		fprintf( header, "%s\t\t\tif ( currentChanged )\n", offset.c_str() );
		fprintf( header, "%s\t\t\t{\n", offset.c_str() );
		fprintf( header, "%s\t\t\t\tchanged = true;\n", offset.c_str() );
		fprintf( header, "%s\t\t\t\tt.notifyElementUpdated_%s();\n", offset.c_str(), member.name.c_str() );
		fprintf( header, "%s\t\t\t}\n", offset.c_str() );
		fprintf( header, "%s\t\t}\n", offset.c_str() );

		fprintf( header, "%s\t\telse\n", offset.c_str() );
		fprintf( header, "%s\t\t{\n", offset.c_str() );
		fprintf( header, "%s\t\t\tif constexpr ( alwaysCollectChanges )\n", offset.c_str() );
		fprintf( header, "%s\t\t\t\tcurrentChanged = %s::parse<typename %s::value_type, bool>( parser, value, addr, %s2 );\n", offset.c_str(), getHelperClassName( *(root.structs[member.type.structIdx]) ).c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), offsetPlusStr );
		fprintf( header, "%s\t\t\telse\n", offset.c_str() );
		fprintf( header, "%s\t\t\t\t%s::parse<typename %s::value_type>( parser, value, addr, %s2 );\n", offset.c_str(), getHelperClassName( *(root.structs[member.type.structIdx]) ).c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), offsetPlusStr );
		fprintf( header, "%s\t\t}\n", offset.c_str() );
	}
	else
		fprintf( header, "%s\t\tthrow std::exception(); // deeper address is unrelated to simple type of vector elements (IDL type of t.%s elements is %s)\n", offset.c_str(), member.name.c_str(), impl_kindToString( member.type.vectorElemKind ) );
				
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s\telse // update of one or more elelments as a whole\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	fprintf( header, "%s\t\tparser.nextElement();\n", offset.c_str() );
	fprintf( header, "%s\t\tuint64_t action = parser.parseAction();\n", offset.c_str() );
	fprintf( header, "%s\t\tswitch ( action )\n", offset.c_str() );
	fprintf( header, "%s\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\tcase ActionOnVector::remove_at:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t%s oldVal;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::copy( t.%s, oldVal );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tt.%s.erase( t.%s.begin() + pos );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tif constexpr ( has_erased_notifier3_for_%s )\n", offset.c_str(), member.name.c_str() );
//	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tt.notifyErased_%s( pos, oldVal );\n", offset.c_str(), member.name.c_str() );
//	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tif constexpr ( has_erased_notifier2_for_%s )\n", offset.c_str(), member.name.c_str() );
//	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
//	fprintf( header, "%s\t\t\t\t\tt.%s.erase( t.%s.begin() + pos );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\t\tt.notifyErased_%s( pos );\n", offset.c_str(), member.name.c_str() );
//	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tif constexpr ( has_void_erased_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
//	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
//	fprintf( header, "%s\t\t\t\t\tt.%s.erase( t.%s.begin() + pos );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\t\tt.notifyErased_%s();\n", offset.c_str(), member.name.c_str() );
//	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tif constexpr ( alwaysCollectChanges )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tcurrentChanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\tcase ActionOnVector::update_at:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tparser.nextElement();\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tparser.leafeBegin();\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::value_type& value = t.%s[pos];\n", offset.c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );

	fprintf( header, "%s\t\t\t\ttypename %s::value_type oldValue;\n", offset.c_str(), impl_templateMemberTypeName( "T", member, true ).c_str() );
	switch ( member.type.vectorElemKind )
	{
		case MessageParameterType::KIND::INTEGER:
		case MessageParameterType::KIND::UINTEGER:
		case MessageParameterType::KIND::REAL:
		case MessageParameterType::KIND::CHARACTER_STRING:
			fprintf( header, "%s\t\t\t\toldValue = value;\n", offset.c_str() );
			break;
		case MessageParameterType::KIND::STRUCT:
		case MessageParameterType::KIND::DISCRIMINATED_UNION:
			fprintf( header, "%s\t\t\t\t%s::copy( value, oldValue );\n", offset.c_str(), getHelperClassName( *(root.structs[member.type.structIdx]) ).c_str() );
			break;
		default:
			fprintf( header, "%s\t\t\t\tstatic_assert(false);\n", offset.c_str() );
			break;
	}
				
	fprintf( header, "%s\t\t\t\tif constexpr ( has_full_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\t\tcurrentChanged = globalmq::marshalling2::PublishableVectorProcessor2<%s>::parseSingleValueAndCompare( parser, value, oldValue );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\t\tif ( currentChanged )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tchanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tt.notifyElementUpdated_%s( pos, oldValue );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tif constexpr ( has_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\t\tt.notifyElementUpdated_%s( pos );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tif constexpr ( has_void_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\t\tt.notifyElementUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str()  );
				
	fprintf( header, "%s\t\t\t\telse if constexpr ( has_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tcurrentChanged = globalmq::marshalling2::PublishableVectorProcessor2<%s>::parseSingleValueAndCompare( parser, value, oldValue );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\t\tif ( currentChanged )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tchanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tt.notifyElementUpdated_%s( pos );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tif constexpr ( has_void_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t\t\tt.notifyElementUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\telse if constexpr ( has_void_element_updated_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tcurrentChanged = globalmq::marshalling2::PublishableVectorProcessor2<%s>::parseSingleValueAndCompare( parser, value, oldValue );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\t\tif ( currentChanged )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tchanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tt.notifyElementUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\telse\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tif constexpr ( alwaysCollectChanges )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tcurrentChanged = globalmq::marshalling2::PublishableVectorProcessor2<%s>::parseSingleValueAndCompare( parser, value, oldValue );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\t\t\tchanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\telse\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::parseSingleValue( parser, value );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\tcase ActionOnVector::insert_single_before:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tparser.nextElement();\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tparser.leafeBegin();\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::value_type value;\n", offset.c_str(), impl_templateMemberTypeName( "T", member, true ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::parseSingleValue( parser, value );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str() );

	fprintf( header, "%s\t\t\t\tif constexpr ( has_insert_notifier3_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\t%s oldVal;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\t\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::copy( t.%s, oldVal );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				
	fprintf( header, "%s\t\t\t\t\tt.notifyInserted_%s( pos, oldVal );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\tif constexpr ( has_insert_notifier2_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\tt.notifyInserted_%s( pos );\n", offset.c_str(), member.name.c_str() );
				
	fprintf( header, "%s\t\t\t\tif constexpr ( has_void_insert_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\t\t\t\tt.notifyInserted_%s();\n", offset.c_str(), member.name.c_str() );
				
	fprintf( header, "%s\t\t\t\tt.%s.insert( t.%s.begin() + pos, value );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				
	fprintf( header, "%s\t\t\t\tif constexpr ( alwaysCollectChanges )\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\t\tcurrentChanged = true;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\tdefault:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tthrow std::exception();\n", offset.c_str() );
	fprintf( header, "%s\t\t}\n", offset.c_str() );
	// fprintf( header, "%s\t\t::globalmq::marshalling::impl::parseStateUpdateBlockEnd( parser );\n", offset.c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
			
//fprintf( header, "//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );
	fprintf( header, "%selse // replacement of the whole vector\n", offset.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tparser.nextElement();\n", offset.c_str() );
	fprintf( header, "%s\tparser.leafeBegin();\n", offset.c_str() );
	// fprintf( header, "%s\t::globalmq::marshalling::impl::publishableParseLeafeVectorBegin( parser );\n", offset.c_str() );
	fprintf( header, "\n" );
	fprintf( header, "%s\tif constexpr( alwaysCollectChanges )\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	fprintf( header, "%s\t\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::parse( parser, t.%s );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\tcurrentChanged = !globalmq::marshalling2::PublishableVectorProcessor2<%s>::isSame( oldVectorVal, t.%s );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s\telse\n", offset.c_str() );
	fprintf( header, "%s\t\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::parse( parser, t.%s );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "\n" );
	// fprintf( header, "%s\t::globalmq::marshalling::impl::publishableParseLeafeVectorEnd( parser );\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );
	fprintf( header, "\n" );
	fprintf( header, "%sif ( currentChanged )\n", offset.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tchanged = true;\n", offset.c_str() );
	fprintf( header, "%s\tif constexpr( has_void_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\tt.notifyUpdated_%s();\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\tif constexpr( has_update_notifier_for_%s )\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s\t\tt.notifyUpdated_%s( oldVectorVal );\n", offset.c_str(), member.name.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );

	if ( addReportChanges )
	{
		fprintf( header, "\n" );
		fprintf( header, "%sif constexpr( alwaysCollectChanges )\n", offset.c_str() );
		fprintf( header, "%s\treturn currentChanged;\n", offset.c_str() );
	}
}

void impl_generateApplyUpdateForFurtherProcessingInDictionaryNoNotifiers( FILE* header, Root& root, MessageParameter& member, bool addOffsetInAddr, std::string offset, const std::string& parserType )
{
//	assert( (!addOffsetInAddr) || (addOffsetInAddr && forwardAddress) );
	const char* offsetPlusStr = addOffsetInAddr ? "offset + " : "";

//	const char* libType = paramTypeToLibType( member.type.vectorElemKind );
	assert( member.type.structIdx < root.structs.size() );
	fprintf( header, "%sif ( addr.size() > %s1 ) // one of actions over elements of the dictionary\n", offset.c_str(), offsetPlusStr );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tsize_t action = addr[%s1];\n", offset.c_str(), offsetPlusStr );

	fprintf( header, "%s\tif ( addr.size() > %s2 ) // update for a value of a particular dictionary element\n", offset.c_str(), offsetPlusStr );
	fprintf( header, "%s\t{\n", offset.c_str() );

	if ( member.type.dictionaryValueKind == MessageParameterType::KIND::STRUCT || member.type.dictionaryValueKind == MessageParameterType::KIND::DISCRIMINATED_UNION )
	{
		fprintf( header, "%s\t\tthrow std::exception(); // so far unsupported and, therefore, unexpected\n", offset.c_str() );
	}
	else
		fprintf( header, "%s\t\tthrow std::exception(); // deeper address is unrelated to simple type of dictionary values (IDL type of t.%s elements is %s)\n", offset.c_str(), member.name.c_str(), impl_kindToString( member.type.dictionaryValueKind ) );
				
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s\telse // update of one or more elelments as a whole\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	fprintf( header, "%s\t\tswitch ( action )\n", offset.c_str() );
	fprintf( header, "%s\t\t{\n", offset.c_str() );

	fprintf( header, "%s\t\t\tcase ActionOnDictionary::remove:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::key_type key;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseKey( parser, key );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\tauto f = t.%s.find( key );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tif ( f == t.%s.end() )\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\t\tthrow std::exception();\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tt.%s.erase( key );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\tcase ActionOnDictionary::update_value:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::key_type key;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseKey( parser, key );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\tauto f = t.%s.find( key );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tif ( f == t.%s.end() )\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\t\tthrow std::exception();\n", offset.c_str() );

	fprintf( header, "%s\t\t\t\ttypename %s::mapped_type& value = f->second;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseValue( parser, value );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\tcase ActionOnDictionary::insert:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::key_type key;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseKey( parser, key );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::mapped_type value;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parseValue( parser, value );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\tt.%s.insert( std::make_pair( key, value ) );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );
	
	fprintf( header, "%s\t\t\tdefault:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tthrow std::exception();\n", offset.c_str() );
	fprintf( header, "%s\t\t}\n", offset.c_str() );
	// fprintf( header, "%s\t\t::globalmq::marshalling::impl::parseStateUpdateBlockEnd( parser );\n", offset.c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
			
	fprintf( header, "%s}\n", offset.c_str() );
	
	fprintf( header, "%selse // replacement of the whole dictionary\n", offset.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\t::globalmq::marshalling::impl::publishableParseLeafeDictionaryBegin( parser );\n", offset.c_str() );
	fprintf( header, "\n" );
	fprintf( header, "%s\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parse( parser, t.%s );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "\n" );
	fprintf( header, "%s\t::globalmq::marshalling::impl::publishableParseLeafeDictionaryEnd( parser );\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );
	fprintf( header, "\n" );
}

void impl_generateApplyUpdateForFurtherProcessingInVectorNoNotifiers( FILE* header, Root& root, MessageParameter& member, bool addOffsetInAddr, std::string offset, const std::string& parserType )
{
//	assert( (!addOffsetInAddr) || (addOffsetInAddr && forwardAddress) );
	const char* offsetPlusStr = addOffsetInAddr ? "offset + " : "";

//	const char* libType = paramTypeToLibType( member.type.vectorElemKind );
	assert( member.type.structIdx < root.structs.size() );
	fprintf( header, "%sif ( addr.size() > %s1 ) // one of actions over elements of the vector\n", offset.c_str(), offsetPlusStr );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tsize_t pos = addr[%s1];\n", offset.c_str(), offsetPlusStr );
	fprintf( header, "%s\tif ( pos > t.%s.size() )\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\tthrow std::exception();\n", offset.c_str() );

	fprintf( header, "%s\tif ( addr.size() > %s2 ) // update for a member of a particular vector element\n", offset.c_str(), offsetPlusStr );
	fprintf( header, "%s\t{\n", offset.c_str() );

	if ( member.type.vectorElemKind == MessageParameterType::KIND::STRUCT || member.type.vectorElemKind == MessageParameterType::KIND::DISCRIMINATED_UNION )
	{
		fprintf( header, "%s\t\ttypename %s::value_type& value = t.%s[pos];\n", offset.c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
		fprintf( header, "%s\t\t%s::parse<typename %s::value_type>( parser, value, addr, %s2 );\n", offset.c_str(), getHelperClassName( *(root.structs[member.type.structIdx]) ).c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), offsetPlusStr );
	}
	else
		fprintf( header, "%s\t\tthrow std::exception(); // deeper address is unrelated to simple type of vector elements (IDL type of t.%s elements is %s)\n", offset.c_str(), member.name.c_str(), impl_kindToString( member.type.vectorElemKind ) );
				
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s\telse // update of one or more elelments as a whole\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	// fprintf( header, "%s\t\tsize_t action;\n", offset.c_str() );
	fprintf( header, "%s\t\tparser.nextElement();\n", offset.c_str() );
	fprintf( header, "%s\t\tuint64_t action = parser.parseAction();\n", offset.c_str() );
	fprintf( header, "%s\t\tswitch ( action )\n", offset.c_str() );
	fprintf( header, "%s\t\t{\n", offset.c_str() );

	fprintf( header, "%s\t\t\tcase ActionOnVector::remove_at:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tt.%s.erase( t.%s.begin() + pos );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\tcase ActionOnVector::update_at:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	// fprintf( header, "%s\t\t\t\t::globalmq::marshalling::impl::publishableParseLeafeValueBegin( parser );\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tparser.nextElement();\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tparser.leafeBegin();\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::value_type& value = t.%s[pos];\n", offset.c_str(), impl_templateMemberTypeName( "T", member, true ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::parseSingleValue( parser, value );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str() );
	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );

	fprintf( header, "%s\t\t\tcase ActionOnVector::insert_single_before:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t{\n", offset.c_str() );
	// fprintf( header, "%s\t\t\t\t::globalmq::marshalling::impl::publishableParseLeafeValueBegin( parser );\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tparser.nextElement();\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tparser.leafeBegin();\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\ttypename %s::value_type value;\n", offset.c_str(), impl_templateMemberTypeName( "T", member, true ).c_str() );
	fprintf( header, "%s\t\t\t\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::parseSingleValue( parser, value );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str() );
				
	fprintf( header, "%s\t\t\t\tt.%s.insert( t.%s.begin() + pos, value );\n", offset.c_str(), impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s\t\t\t\tbreak;\n", offset.c_str() );
	fprintf( header, "%s\t\t\t}\n", offset.c_str() );
	fprintf( header, "%s\t\t\tdefault:\n", offset.c_str() );
	fprintf( header, "%s\t\t\t\tthrow std::exception();\n", offset.c_str() );
	fprintf( header, "%s\t\t}\n", offset.c_str() );
	// fprintf( header, "%s\t\t::globalmq::marshalling::impl::parseStateUpdateBlockEnd( parser );\n", offset.c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
			
	fprintf( header, "%s}\n", offset.c_str() );
	fprintf( header, "%selse // replacement of the whole vector\n", offset.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tparser.nextElement();\n", offset.c_str() );
	fprintf( header, "%s\tparser.leafeBegin();\n", offset.c_str() );
	fprintf( header, "%s\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::parse( parser, t.%s );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	fprintf( header, "%s}\n", offset.c_str() );
	fprintf( header, "\n" );
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
			fprintf( header, "\tauto get_%s() { return globalmq::marshalling::VectorOfStructRefWrapper<%s_RefWrapper<typename %s::value_type>, %s>(t.%s); }\n", 
				param.name.c_str(), root.structs[param.type.structIdx]->name.c_str(), impl_templateMemberTypeName( "T", param, true ).c_str(), impl_templateMemberTypeName( "T", param).c_str(), impl_memberOrAccessFunctionName( param ).c_str() );
		}
		else
			fprintf( header, "\tauto get_%s() { return globalmq::marshalling::VectorOfSimpleTypeRefWrapper(t.%s); }\n", param.name.c_str(), impl_memberOrAccessFunctionName( param ).c_str() );
	}
	else if ( param.type.kind == MessageParameterType::KIND::DICTIONARY )
	{
		if ( param.type.dictionaryValueKind == MessageParameterType::KIND::STRUCT || param.type.dictionaryValueKind == MessageParameterType::KIND::DISCRIMINATED_UNION )
		{
			assert( root.structs.size() > param.type.structIdx );
			fprintf( header, "\tauto get_%s() { return globalmq::marshalling::DictionaryOfStructRefWrapper<%s_RefWrapper<typename %s::value_type>, %s>(t.%s); }\n", 
				param.name.c_str(), root.structs[param.type.structIdx]->name.c_str(), impl_templateMemberTypeName( "T", param, true ).c_str(), impl_templateMemberTypeName( "T", param).c_str(), impl_memberOrAccessFunctionName( param ).c_str() );
		}
		else
			fprintf( header, "\tauto get_%s() { return globalmq::marshalling::DictionaryOfSimpleTypeRefWrapper<%s>(t.%s); }\n", param.name.c_str(), impl_templateMemberTypeName( "T", param).c_str(), impl_memberOrAccessFunctionName( param ).c_str() );
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
		fprintf( header, "\tauto get4set_%s() { return %s_RefWrapper4Set<%s, %s>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
			param.name.c_str(), root.structs[param.type.structIdx]->name.c_str(), impl_templateMemberTypeName( "T", param ).c_str(), rootType.c_str(), impl_memberOrAccessFunctionName( param ).c_str(), rootObjName.c_str(), addr.c_str(), idxStr.c_str() );
	}
	else if ( param.type.kind == MessageParameterType::KIND::DISCRIMINATED_UNION ) // TODO: revise DU
	{
		assert( param.type.structIdx < root.structs.size() );
		fprintf( header, "\tauto get4set_%s() { return %s_RefWrapper4Set<%s, %s>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
			param.name.c_str(), root.structs[param.type.structIdx]->name.c_str(), impl_templateMemberTypeName( "T", param ).c_str(), rootType.c_str(), impl_memberOrAccessFunctionName( param ).c_str(), rootObjName.c_str(), addr.c_str(), idxStr.c_str() );
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
				fprintf( header, "\tauto get4set_%s() { return globalmq::marshalling2::VectorRefWrapper4Set<%s, %s, %s>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
					param.name.c_str(), impl_templateMemberTypeName( "T", param ).c_str(), getVectorElementProcessor(param.type).c_str(), rootType.c_str(), impl_memberOrAccessFunctionName( param ).c_str(), rootObjName.c_str(), addr.c_str(), idxStr.c_str() );
				break;
			case MessageParameterType::KIND::STRUCT:
			case MessageParameterType::KIND::DISCRIMINATED_UNION: // TODO: revise DU (lib kw: VectorOfStructRefWrapper4Set and around)
				fprintf( header, 
					"\tauto get4set_%s() { return globalmq::marshalling2::VectorOfStructRefWrapper4Set<%s, %s, %s, %s_RefWrapper4Set<typename %s::value_type, %s>>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
					param.name.c_str(), impl_templateMemberTypeName( "T", param ).c_str(),
					getVectorElementProcessor(param.type).c_str(), 
					rootType.c_str(), 
					param.type.name.c_str(), 
					impl_templateMemberTypeName( "T", param, true ).c_str(),
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
				fprintf( header, "\tauto get4set_%s() { return globalmq::marshalling2::DictionaryRefWrapper4Set<%s, %s, %s>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
					param.name.c_str(), impl_templateMemberTypeName( "T", param ).c_str(), getDictionaryKeyValueProcessor(param.type).c_str(), rootType.c_str(), impl_memberOrAccessFunctionName( param ).c_str(), rootObjName.c_str(), addr.c_str(), idxStr.c_str() );
				break;
			case MessageParameterType::KIND::STRUCT:
			case MessageParameterType::KIND::DISCRIMINATED_UNION:
				assert( param.type.structIdx < root.structs.size() );
				fprintf( header, 
					"\tauto get4set_%s() { return globalmq::marshalling2::DictionaryRefWrapper4Set<%s, %s, %s, %s_RefWrapper4Set<typename %s::value_type, %s>>(t.%s, %s, globalmq::marshalling2::makeAddress(%s, %s)); }\n", 
					param.name.c_str(), impl_templateMemberTypeName( "T", param ).c_str(),
					getDictionaryKeyValueProcessor(param.type).c_str(), 
					rootType.c_str(), 
					param.type.name.c_str(), 
					impl_templateMemberTypeName( "T", param, true ).c_str(),
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

void impl_generateContinueParsingFunctionForPublishableStruct_MemberIterationBlock( FILE* header, Root& root, CompositeType& obj, const char* offset, size_t idxBase, const std::string& parserType )
{
	assert( obj.type != CompositeType::Type::discriminated_union );

	for ( size_t i=0; i<obj.getMembers().size(); ++i )
	{
		std::string numId = std::to_string(idxBase + i);
		fprintf( header, "%scase %s:\n", offset, numId.c_str() );
		fprintf( header, "%s{\n", offset );
		assert( obj.getMembers()[i] != nullptr );
		auto& member = *(obj.getMembers()[i]);
		switch ( member.type.kind )
		{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
			{
				fprintf( header, "%s\tif ( addr.size() > offset + 1 )\n", offset );
				fprintf( header, "%s\t\tthrow std::exception(); // bad format, TODO: ...\n", offset );
				fprintf( header, "%s\t\tparser.nextElement();\n", offset );
				fprintf( header, "%s\t\tparser.leafeBegin();\n", offset );
				impl_generateApplyUpdateForSimpleType( header, member, true, true, std::string( offset ) + '\t', parserType );
				break;
			}
			case  MessageParameterType::KIND::STRUCT:
			case  MessageParameterType::KIND::DISCRIMINATED_UNION:
			{
				fprintf( header, "%s\tif ( addr.size() == offset + 1 ) // we have to parse and apply changes of this child\n", offset );
				fprintf( header, "%s\t{\n", offset );
				fprintf( header, "%s\t\tparser.nextElement();\n", offset );
				fprintf( header, "%s\t\tparser.leafeBegin();\n", offset );

				impl_generateApplyUpdateForStructItself( header, member, true, std::string( offset) + "\t\t", parserType ); // TODO: revise DU: we may need something DU_spec here

				// fprintf( header, "%s\t\t::globalmq::marshalling::impl::publishableParseLeafeStructEnd( parser );\n", offset );
				fprintf( header, "%s\t}\n", offset );
				fprintf( header, "%s\telse // let child continue parsing\n", offset );
				fprintf( header, "%s\t{\n", offset );

				impl_generateApplyUpdateForFurtherProcessingInStruct( header, member, true, true, true, std::string( offset) + "\t\t", parserType );

				fprintf( header, "%s\t}\n", offset );
				break;
			}
			case MessageParameterType::KIND::VECTOR:
			{
				fprintf( header, "%s\t{\n", offset );
//				impl_generateApplyUpdateForFurtherProcessingInVector( header, root, member, false, false );
				impl_generateApplyUpdateForFurtherProcessingInVector( header, root, member, true, false, std::string( offset ) + "\t\t", parserType );
				fprintf( header, "%s\t}\n", offset );
				break;
			}
			case MessageParameterType::KIND::DICTIONARY:
			{
				fprintf( header, "%s\t{\n", offset );
//				impl_generateApplyUpdateForFurtherProcessingInVector( header, root, member, false, false );
				impl_generateApplyUpdateForFurtherProcessingInDictionary( header, root, member, true, false, std::string( offset ) + "\t\t", parserType );
				fprintf( header, "%s\t}\n", offset );
				break;
			}
			default:
				assert( false );
		}
		fprintf( header, "%s\tbreak;\n", offset );
		fprintf( header, "%s}\n", offset );
	}
}

void impl_generateApplyUpdateForDiscriminatedUnionVariant( FILE* header, bool addReportChanges, std::string offset )
{
	fprintf( header, "%sparser.nextElement();\n", offset.c_str() );
	fprintf( header, "%sparser.leafeBegin();\n", offset.c_str());
	fprintf( header, "%suint64_t newVar = parser.parseUnsignedInteger();\n", offset.c_str() );

	if ( addReportChanges )
		fprintf( header, "%sif constexpr( has_any_notifier_for_currentVariant || reportChanges || has_update_notifier )\n", offset.c_str() );
	else
		fprintf( header, "%sif constexpr( has_any_notifier_for_currentVariant )\n", offset.c_str() );
	fprintf( header, "%s{\n", offset.c_str() );
	fprintf( header, "%s\tauto oldVal = t.currentVariant();\n", offset.c_str() );
	fprintf( header, "%s\tt.initAs( (typename T::Variants)( newVar ) );\n", offset.c_str() );
	fprintf( header, "%s\tbool currentChanged = oldVal != t.currentVariant();\n", offset.c_str() );
	fprintf( header, "%s\tif ( currentChanged )\n", offset.c_str() );
	fprintf( header, "%s\t{\n", offset.c_str() );
	if ( addReportChanges )
	{
		fprintf( header, "%s\t\tif constexpr ( reportChanges || has_update_notifier )\n", offset.c_str() );
		fprintf( header, "%s\t\t\tchanged = true;\n", offset.c_str() );
	}
	fprintf( header, "%s\t\tif constexpr ( has_void_update_notifier_for_currentVariant )\n", offset.c_str() );
	fprintf( header, "%s\t\t\tt.notifyUpdated_currentVariant();\n", offset.c_str() );
	fprintf( header, "%s\t\tif constexpr ( has_update_notifier_for_currentVariant )\n", offset.c_str() );
	fprintf( header, "%s\t\t\tt.notifyUpdated_currentVariant( oldVal );\n", offset.c_str() );
	fprintf( header, "%s\t}\n", offset.c_str() );
	fprintf( header, "%s}\n", offset.c_str() );
	fprintf( header, "%selse\n", offset.c_str() );

	fprintf( header, "%s\tt.initAs( (typename T::Variants)( newVar ) );\n", offset.c_str() );
}

void impl_GenerateDiscriminatedUnionVariantUpdateNotifierPresenceCheckingBlock( FILE* header, const char* offset )
{
	// along lines of impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock()
	fprintf( header, "%sstatic constexpr bool has_void_update_notifier_for_currentVariant = has_void_update_notifier_call_for_currentVariant<T>;\n", offset );
	fprintf( header, "%sstatic constexpr bool has_update_notifier_for_currentVariant = has_update_notifier_call_for_currentVariant<T, typename T::Variants>;\n", offset );
	fprintf( header, "%sstatic constexpr bool has_any_notifier_for_currentVariant = has_void_update_notifier_for_currentVariant || has_update_notifier_for_currentVariant;\n", offset );
}

void impl_generateContinueParsingFunctionForPublishableStruct( FILE* header, Root& root, CompositeType& obj, const std::string& parserType )
{
	fprintf( header, "\ttemplate<class T, class RetT = void>\n" );
	fprintf( header, "\tstatic\n" );
	fprintf( header, "\tRetT parse( %s& parser, T& t, GMQ_COLL vector<uint64_t>& addr, uint64_t offset )\n", parserType.c_str() );
	fprintf( header, "\t{\n" );
//	fprintf( header, "\t\t//****  ContinueParsing  **************************************************************************************************************************************************************\n" );
	fprintf( header, "\t\tstatic_assert( std::is_same<RetT, bool>::value || std::is_same<RetT, void>::value );\n" );
	fprintf( header, "\t\tconstexpr bool reportChanges = std::is_same<RetT, bool>::value;\n" );
	fprintf( header, "\t\tbool changed = false;\n" );

	impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock( header, root, obj, "\t\t" );


	if ( obj.isDiscriminatedUnion() )
	{
		impl_GenerateDiscriminatedUnionVariantUpdateNotifierPresenceCheckingBlock( header, "\t\t" );
		fprintf( header, "\t\tGMQ_ASSERT( addr.size() );\n" );
		fprintf( header, "\t\tif ( addr[offset] == 0 ) // changing current variant\n" );
		fprintf( header, "\t\t{\n" );
		fprintf( header, "\t\t\tif ( addr.size() > offset + 1 )\n" );
		fprintf( header, "\t\t\t\tthrow std::exception(); // bad format, TODO: ...\n" );
		impl_generateApplyUpdateForDiscriminatedUnionVariant( header, true, "\t\t\t" );
		fprintf( header, "\t\t}\n" );

		fprintf( header, "\t\telse // updating particular members in particular cases\n" );

		fprintf( header, "\t\t{\n" );
		fprintf( header, "\t\t\tswitch ( t.currentVariant() )\n" );
		fprintf( header, "\t\t\t{\n" );
		for ( auto& it: obj.getDiscriminatedUnionCases() )
		{
			std::string numId = std::to_string(it->numID);
			fprintf( header, "\t\t\t\tcase %s: // IDL CASE %s\n", numId.c_str(), it->name.c_str() );
			fprintf( header, "\t\t\t\t{\n" );
			fprintf( header, "\t\t\t\t\tswitch ( addr[offset] )\n" );
			fprintf( header, "\t\t\t\t\t{\n" );
			assert( it != nullptr );
			CompositeType& cs = *it;
			assert( cs.type == CompositeType::Type::discriminated_union_case );
			impl_generateContinueParsingFunctionForPublishableStruct_MemberIterationBlock( header, root, cs, "\t\t\t\t\t\t", 1, parserType );
			fprintf( header, "\t\t\t\t\t\tdefault:\n" );
			fprintf( header, "\t\t\t\t\t\t\tthrow std::exception(); // unexpected\n" );
			fprintf( header, "\t\t\t\t\t}\n" );
			fprintf( header, "\t\t\t\t\tbreak;\n" );
			fprintf( header, "\t\t\t\t}\n" );
		}
		fprintf( header, "\t\t\t\tdefault:\n" );
		fprintf( header, "\t\t\t\t\tthrow std::exception(); // unexpected\n" );
		fprintf( header, "\t\t\t}\n" );
		fprintf( header, "\t\t}\n" );
//		fprintf( header, "\t\t::globalmq::marshalling::impl::parsePublishableStructEnd( parser );\n" );
	}
	else
	{
		fprintf( header, "\t\tGMQ_ASSERT( addr.size() );\n" );
		fprintf( header, "\t\tswitch ( addr[offset] )\n" );
		fprintf( header, "\t\t{\n" );
		impl_generateContinueParsingFunctionForPublishableStruct_MemberIterationBlock( header, root, obj, "\t\t\t", 0, parserType );
		fprintf( header, "\t\t\tdefault:\n" );
		fprintf( header, "\t\t\t\tthrow std::exception(); // unexpected\n" );
		fprintf( header, "\t\t}\n" );
	}

	fprintf( header, "\t\tif constexpr ( has_update_notifier )\n" );
	fprintf( header, "\t\t{\n" );
	fprintf( header, "\t\t\tif ( changed )\n" );
	fprintf( header, "\t\t\t\tt.notifyUpdated();\n" );
	fprintf( header, "\t\t}\n" );

	fprintf( header, "\t\tif constexpr ( reportChanges )\n" );
	fprintf( header, "\t\t\treturn changed;\n" );
	fprintf( header, "\t}\n" );
}

void impl_generateParseFunctionForPublishableStruct_MemberIterationBlock( FILE* header, Root& root, CompositeType& obj, std::string offset, const std::string& parserType )
{
	assert( obj.type != CompositeType::Type::discriminated_union );

	for ( size_t i=0; i<obj.getMembers().size(); ++i )
	{
		assert( obj.getMembers()[i] != nullptr );
		auto& member = *(obj.getMembers()[i]);

		if(i != 0)
			fprintf( header, "%sparser.nextElement();\n", offset.c_str() );

		switch ( member.type.kind )
		{
			case MessageParameterType::KIND::INTEGER:
			case MessageParameterType::KIND::UINTEGER:
			case MessageParameterType::KIND::REAL:
			case MessageParameterType::KIND::CHARACTER_STRING:
			{
				fprintf( header, "%sparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str());
				impl_generateApplyUpdateForSimpleType( header, member, true, false, offset, parserType );
				fprintf( header, "\n" );
				break;
			}
			case  MessageParameterType::KIND::STRUCT:
			{
				fprintf( header, "%sparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str());
				fprintf( header, "%sparser.structBegin();\n", offset.c_str());

				impl_generateApplyUpdateForFurtherProcessingInStruct( header, member, false, true, false, offset, parserType );

				fprintf( header, "%sparser.structEnd();\n", offset.c_str() );
				break;
			}
			case  MessageParameterType::KIND::DISCRIMINATED_UNION:
			{
				fprintf(header, "%sparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str());
				fprintf(header, "%sparser.structBegin();\n", offset.c_str());

				impl_generateApplyUpdateForFurtherProcessingInStruct( header, member, false, true, false, offset, parserType ); // TODO: revise DU: we may need something DU-spec here

				fprintf(header, "%sparser.structEnd();\n", offset.c_str());
				break;
			}
			case MessageParameterType::KIND::VECTOR:
			{
				assert( member.type.structIdx < root.structs.size() );
				
				fprintf( header, "%sif constexpr( reportChanges || has_update_notifier )\n", offset.c_str() );
				fprintf( header, "%s{\n", offset.c_str() );
				fprintf( header, "%s\t%s oldVectorVal;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
				fprintf( header, "%s\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::copy( t.%s, oldVectorVal );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				fprintf( header, "%s\tparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str() );
				fprintf( header, "%s\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::parse( parser, t.%s );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				fprintf( header, "%s\tbool currentChanged = !::globalmq::marshalling2::PublishableVectorProcessor2<%s>::isSame( oldVectorVal, t.%s );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				fprintf( header, "%s\tchanged = changed || currentChanged;\n", offset.c_str() );
				fprintf( header, "%s}\n", offset.c_str() );
				fprintf( header, "%selse\n", offset.c_str() );
				fprintf( header, "%s{\n", offset.c_str() );
				fprintf( header, "%s\tparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str());
				fprintf( header, "%s\tglobalmq::marshalling2::PublishableVectorProcessor2<%s>::parse( parser, t.%s );\n", offset.c_str(), getVectorElementProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				fprintf( header, "%s}\n", offset.c_str() );
				fprintf( header, "\n" );

				break;
			}
			case MessageParameterType::KIND::DICTIONARY:
			{
				assert( member.type.structIdx < root.structs.size() );
				
				fprintf( header, "//// TODO: revise notifier list !!!!!!!!!!!!!!!!!!!!!!!!!\n" );
				fprintf( header, "%sif constexpr( reportChanges || has_update_notifier )\n", offset.c_str() );
				fprintf( header, "%s{\n", offset.c_str() );
				fprintf( header, "%s\t%s oldDictionaryVal;\n", offset.c_str(), impl_templateMemberTypeName( "T", member ).c_str() );
				fprintf( header, "%s\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::copy( t.%s, oldDictionaryVal );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				fprintf(header, "%s\tparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str());
				fprintf( header, "%s\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parse( parser, t.%s );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				fprintf( header, "%s\tbool currentChanged = !globalmq::marshalling2::PublishableDictionaryProcessor2<%s>::isSame( oldDictionaryVal, t.%s );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				fprintf( header, "%s\tchanged = changed || currentChanged;\n", offset.c_str() );
				fprintf( header, "%s}\n", offset.c_str() );
				fprintf( header, "%selse\n", offset.c_str() );
				fprintf( header, "%s{\n", offset.c_str() );
				fprintf(header, "%s\tparser.namedParamBegin( \"%s\" );\n", offset.c_str(), member.name.c_str());
				fprintf( header, "%s\tglobalmq::marshalling2::PublishableDictionaryProcessor2<%s>::parse( parser, t.%s );\n", offset.c_str(), getDictionaryKeyValueProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
				fprintf( header, "%s}\n", offset.c_str() );
				fprintf( header, "\n" );

				break;
			}
			default:
				assert( false );
		}
	}
}

void impl_generateParseFunctionForPublishableStruct( FILE* header, Root& root, CompositeType& obj, const std::string& parserType )
{
	assert( obj.type == CompositeType::Type::structure ||
			obj.type == CompositeType::Type::discriminated_union ||
			obj.type == CompositeType::Type::publishable );

	std::string typeName = getGeneratedTypeName(obj);
	fprintf( header, "\ttemplate<class T, class RetT = void>\n" );
	fprintf( header, "\tstatic\n" );
	fprintf( header, "\tRetT parse( %s& parser, T& t )\n", parserType.c_str() );
	fprintf( header, "\t{\n" );
	fprintf( header, "\t\tstatic_assert( std::is_same<RetT, bool>::value || std::is_same<RetT, void>::value );\n" );
	fprintf( header, "\t\tconstexpr bool reportChanges = std::is_same<RetT, bool>::value;\n" );
	fprintf( header, "\t\tbool changed = false;\n" );

	impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock( header, root, obj, "\t\t" );

	fprintf( header, "\t\tparser.structBegin();\n" );

	if ( obj.isDiscriminatedUnion() )
	{
		fprintf( header, "\t\tparser.namedParamBegin( \"caseId\" );\n" );
		fprintf( header, "\t\tuint64_t caseId = parser.parseUnsignedInteger();\n" );
		fprintf( header, "\t\tt.initAs( (typename %s::Variants)(caseId) );\n", typeName.c_str() );
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
			impl_generateParseFunctionForPublishableStruct_MemberIterationBlock( header, root, cs, "\t\t\t\t\t", parserType );
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
	{
		impl_generateParseFunctionForPublishableStruct_MemberIterationBlock( header, root, obj, "\t\t", parserType);
	}

	fprintf( header, "\n" );

	fprintf( header, "\t\tparser.structEnd();\n" );

	fprintf( header, "\t\tif constexpr ( has_update_notifier )\n" );
	fprintf( header, "\t\t{\n" );
	fprintf( header, "\t\t\tif ( changed )\n" );
	fprintf( header, "\t\t\t\tt.notifyUpdated();\n" );
	fprintf( header, "\t\t}\n" );

	fprintf( header, "\t\tif constexpr ( reportChanges )\n" );
	fprintf( header, "\t\t\treturn changed;\n" );
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

	fprintf( header, "\t\t%s::parseForStateSyncOrMessageInDepth( parser, t );\n", getHelperClassName(obj).c_str() );

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

		fprintf( header, "%sif( ! %s::isSame( s1.%s, s2.%s ) ) return false;\n", offset.c_str(), getTypeProcessor( member.type ).c_str(), impl_memberOrAccessFunctionName( member ).c_str(), impl_memberOrAccessFunctionName( member ).c_str() );
	}
}

void impl_GeneratePublishableStructIsSameFn( FILE* header, Root& root, CompositeType& s )
{
	fprintf( header, "\ttemplate<typename UserT>\n" );
	fprintf( header, "\tstatic bool isSame(const UserT& s1, const UserT& s2) {\n" );

	if ( s.isDiscriminatedUnion() )
	{
		fprintf( header, "\t\tif ( s1.currentVariant() != s2.currentVariant() )\n" );
		fprintf( header, "\t\t\treturn false;\n" );

		fprintf( header, "\t\tif ( s1.currentVariant() != UserT::Variants::unknown )\n" );
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

	fprintf( header, "\tusing ProcessorType = %s;\n", getGeneratedTypeName(obj).c_str() );
	

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
		for (auto& each : config.parserNames)
		{
			impl_generateParseFunctionForPublishableStruct(header, root, obj, each);
			fprintf(header, "\n");

			impl_generateContinueParsingFunctionForPublishableStruct(header, root, obj, each);
			fprintf(header, "\n");
		}
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
			fprintf( header, "\tvoid set_currentVariant( decltype(T::v) v ) { \n" );
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
	fprintf( header, "\t\t\t%s::parse(parser, t, addr, 0);\n", structName.c_str() );

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

	fprintf( header, "template<class T, class ComposerT>\n" );
	fprintf( header, "class %s_WrapperForPublisher : public globalmq::marshalling::StatePublisherBase<ComposerT>\n", s.name.c_str() );
	fprintf( header, "{\n" );
	fprintf( header, "\tT t;\n" );
	fprintf( header, "\tusing BufferT = typename ComposerT::BufferType;\n" );
	fprintf( header, "\tComposerT composer;\n" );

	// impl_GeneratePublishableStateMemberPresenceCheckingBlock( header, root, s );
//	impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock( header, root, s, "\t" );

	fprintf( header, "\npublic:\n" );
	fprintf( header, "\tstatic constexpr uint64_t numTypeID = %lld;\n", s.numID );
	fprintf( header, "\tstatic constexpr const char* stringTypeID = \"%s\";\n", s.name.c_str() );
	fprintf( header, "\n" );
	fprintf( header, "\ttemplate<class ... ArgsT>\n" );
	fprintf( header, "\t%s_WrapperForPublisher( ArgsT&& ... args ) : t( std::forward<ArgsT>( args )... ) {}\n", s.name.c_str() );
	fprintf( header, "\tconst T& getState() { return t; }\n" );
	fprintf( header, "\tComposerT& getComposer() { return composer; }\n" );
	fprintf( header, "\tvoid startTick( BufferT&& buff ) { composer.setBuffer( std::move( buff ) ); composer.stateUpdateBegin(); }\n" );
	fprintf( header, "\tBufferT&& endTick() { composer.stateUpdateEnd(); return composer.getBuffer(); }\n" );
	fprintf( header, "\tconst char* name() { return stringTypeID; }\n" );
	fprintf( header, "\tvirtual uint64_t stateTypeID() { return numTypeID; }\n" );

	impl_GeneratePublishableStateMemberAccessors( header, root, s, true );
	fprintf( header, "\n" );

	for (auto& each : config.composerNames)
		fprintf( header, "\tvoid compose( %s& composer ) { %s::compose( composer, t ); }\n", each.c_str(), getHelperClassName(s).c_str() );



	fprintf( header, "};\n\n" );
}

void impl_GeneratePublishableStatePlatformWrapperForPublisher( FILE* header, Root& root, CompositeType& s, std::string platformPrefix, std::string classNotifierName )
{
	assert( s.type == CompositeType::Type::publishable );

	fprintf( header, "template<class T, class RegistrarT>\n" );
	fprintf( header, "class %s_%sWrapperForPublisher : public %s_WrapperForPublisher<T, typename %s::ComposerT>\n", s.name.c_str(), platformPrefix.c_str(), s.name.c_str(), classNotifierName.c_str() );
	fprintf( header, "{\n" );
	fprintf( header, "\tusing ComposerT = typename %s::ComposerT;\n", classNotifierName.c_str() );
	fprintf( header, "\tRegistrarT& registrar;\n" );
	fprintf( header, "public:\n" );
	fprintf( header, "\tusing BufferT = typename %s::ComposerT::BufferType;\n", classNotifierName.c_str() );
	fprintf( header, "\ttemplate<class ... ArgsT>\n" );
	fprintf( header, "\t%s_%sWrapperForPublisher( RegistrarT& registrar_, ArgsT&& ... args ) : %s_WrapperForPublisher<T, typename %s::ComposerT>( std::forward<ArgsT>( args )... ), registrar( registrar_ )\n", s.name.c_str(), platformPrefix.c_str(), s.name.c_str(), classNotifierName.c_str() );
	fprintf( header, "\t{ \n" );
	fprintf( header, "\t\tregistrar.add( this );\n" );
	fprintf( header, "\t}\n" );
	fprintf( header, "\n" );
	fprintf( header, "\tvirtual ~%s_%sWrapperForPublisher()\n", s.name.c_str(), platformPrefix.c_str() );
	fprintf( header, "\t{ \n" );
	fprintf( header, "\t\tregistrar.remove( this );\n" );
	fprintf( header, "\t}\n" );
	fprintf( header, "\n" );

	fprintf( header, "\tvirtual void startTick( BufferT&& buff ) { %s_WrapperForPublisher<T, ComposerT>::startTick( std::move( buff ) ); }\n", s.name.c_str() );
	fprintf( header, "\tvirtual BufferT&& endTick() { return  %s_WrapperForPublisher<T, ComposerT>::endTick(); }\n", s.name.c_str() );
	fprintf( header, "\tvirtual void generateStateSyncMessage(ComposerT& composer) { %s_WrapperForPublisher<T, ComposerT>::compose(composer); }\n", s.name.c_str() );
	fprintf( header, "\tvirtual const char* name() { return %s_WrapperForPublisher<T, ComposerT>::name(); }\n", s.name.c_str() );

	fprintf( header, "};\n\n" );
}

void impl_GeneratePublishableStateWrapperForSubscriber( FILE* header, Root& root, CompositeType& s, const GenerationConfig& config)
{
	assert( s.type == CompositeType::Type::publishable );

	fprintf( header, "template<class T, class BufferT>\n" );
	fprintf( header, "class %s_WrapperForSubscriber : public globalmq::marshalling::StateSubscriberBase<BufferT>\n", s.name.c_str() );
	fprintf( header, "{\n" );
	fprintf( header, "\tT t;\n" );

	impl_GeneratePublishableStateMemberPresenceCheckingBlock( header, root, s );
	impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock( header, root, s, "\t" );

	fprintf( header, "\npublic:\n" );
	fprintf( header, "\tstatic constexpr uint64_t numTypeID = %lld;\n", s.numID );
	fprintf( header, "\tstatic constexpr const char* stringTypeID = \"%s\";\n", s.name.c_str() );
	fprintf( header, "\n" );
	fprintf( header, "\tstatic constexpr bool reportChanges = false;\n" );
	fprintf( header, "\tbool changed = false;\n" );
	fprintf( header, "\n" );
	fprintf( header, "\ttemplate<class ... ArgsT>\n" );
	fprintf( header, "\t%s_WrapperForSubscriber( ArgsT&& ... args ) : t( std::forward<ArgsT>( args )... ) {}\n", s.name.c_str() );
	fprintf( header, "\tconst T& getState() { return t; }\n" );
	fprintf( header, "\tvirtual void applyGmqMessageWithUpdates( globalmq::marshalling::GmqParser<BufferT>& parser ) override { throw std::exception(); }\n" );
	fprintf( header, "\tvirtual void applyJsonMessageWithUpdates( globalmq::marshalling::JsonParser<BufferT>& parser ) override { throw std::exception(); }\n" );
	fprintf( header, "\tvirtual void applyGmqStateSyncMessage( globalmq::marshalling::GmqParser<BufferT>& parser ) override { throw std::exception(); }\n" );
	fprintf( header, "\tvirtual void applyJsonStateSyncMessage( globalmq::marshalling::JsonParser<BufferT>& parser ) override { throw std::exception(); }\n" );
	fprintf( header, "\n" );
	fprintf( header, "\tvirtual void applyMessageWithUpdates( globalmq::marshalling2::ParserBase& parser ) override\n" );
	fprintf( header, "\t{\n" );
	for (auto& each : config.parserNames)
		fprintf( header, "\t\tif(auto ptr = dynamic_cast<%s*>(&parser)) { applyMessageWithUpdates(*ptr); return; }\n", each.c_str() );

	fprintf( header, "\t\tthrow std::exception();\n" );
	fprintf( header, "\t}\n" );

	fprintf( header, "\tvirtual void applyStateSyncMessage( globalmq::marshalling2::ParserBase& parser ) override\n" );
	fprintf( header, "\t{\n" );
	for (auto& each : config.parserNames)
		fprintf( header, "\t\tif(auto ptr = dynamic_cast<%s*>(&parser)) { parseStateSyncMessage(*ptr); return; }\n", each.c_str() );

	fprintf( header, "\t\tthrow std::exception();\n" );
	fprintf( header, "\t}\n" );
	fprintf( header, "\n" );
	fprintf( header, "\tvirtual const char* name() { return stringTypeID; }\n" );
	fprintf( header, "\tvirtual uint64_t stateTypeID() { return numTypeID; }\n" );
	fprintf( header, "\n" );

	for (auto& each : config.parserNames)
	{
		impl_GenerateApplyUpdateMessageMemberFn(header, root, s, true, each);

		fprintf(header, "\n");

		impl_generateParseFunctionForPublishableState(header, root, s, true, each);

		fprintf(header, "\n");
	}

	impl_GeneratePublishableStateMemberAccessors(header, root, s, false);

	fprintf( header, "};\n\n" );
}

void impl_GeneratePublishableStatePlatformWrapperForSubscriber( FILE* header, Root& root, CompositeType& s, std::string platformPrefix, std::string classNotifierName )
{
	assert( s.type == CompositeType::Type::publishable );

	fprintf( header, "template<class T, class RegistrarT>\n" );
	fprintf( header, "class %s_%sWrapperForSubscriber : public %s_WrapperForSubscriber<T, typename %s::BufferT>\n", s.name.c_str(), platformPrefix.c_str(), s.name.c_str(), classNotifierName.c_str() );
	fprintf( header, "{\n" );
	fprintf( header, "\tRegistrarT& registrar;\n" );
	fprintf( header, "public:\n" );
	fprintf( header, "\ttemplate<class ... ArgsT>\n" );
	fprintf( header, "\t%s_%sWrapperForSubscriber( RegistrarT& registrar_, ArgsT&& ... args ) : %s_WrapperForSubscriber<T, typename %s::BufferT>( std::forward<ArgsT>( args )... ), registrar( registrar_ )\n", s.name.c_str(), platformPrefix.c_str(), s.name.c_str(), classNotifierName.c_str() );
	fprintf( header, "\t{ \n" );
	fprintf( header, "\t\tregistrar.add( this );\n" );
	fprintf( header, "\t}\n" );
	fprintf( header, "\n" );
	fprintf( header, "\tvirtual ~%s_%sWrapperForSubscriber()\n", s.name.c_str(), platformPrefix.c_str() );
	fprintf( header, "\t{ \n" );
	fprintf( header, "\t\tregistrar.remove( this );\n" );
	fprintf( header, "\t}\n" );
	fprintf( header, "\n" );
	fprintf( header, "\tvirtual void applyGmqMessageWithUpdates( globalmq::marshalling::GmqParser<typename %s::BufferT>& parser ) \n", classNotifierName.c_str() );
	fprintf( header, "\t{\n" );
	fprintf( header, "\t\t%s_WrapperForSubscriber<T, typename %s::BufferT>::applyMessageWithUpdates(parser);\n", s.name.c_str(), classNotifierName.c_str() );
	fprintf( header, "\t}\n" );
	fprintf( header, "\n" );
	fprintf( header, "\tvirtual void applyJsonMessageWithUpdates( globalmq::marshalling::JsonParser<typename %s::BufferT>& parser )\n", classNotifierName.c_str() );
	fprintf( header, "\t{\n" );
	fprintf( header, "\t\t%s_WrapperForSubscriber<T, typename %s::BufferT>::applyMessageWithUpdates(parser);\n", s.name.c_str(), classNotifierName.c_str() );
	fprintf( header, "\t}\n" );
	fprintf( header, "\n" );
	fprintf( header, "\tvirtual void applyGmqStateSyncMessage( globalmq::marshalling::GmqParser<typename %s::BufferT>& parser ) \n", classNotifierName.c_str() );
	fprintf( header, "\t{\n" );
	fprintf( header, "\t\t%s_WrapperForSubscriber<T, typename %s::BufferT>::parseStateSyncMessage(parser);\n", s.name.c_str(), classNotifierName.c_str() );
	fprintf( header, "\t}\n" );
	fprintf( header, "\n" );
	fprintf( header, "\tvirtual void applyJsonStateSyncMessage( globalmq::marshalling::JsonParser<typename %s::BufferT>& parser )\n", classNotifierName.c_str() );
	fprintf( header, "\t{\n" );
	fprintf( header, "\t\t%s_WrapperForSubscriber<T, typename %s::BufferT>::parseStateSyncMessage(parser);\n", s.name.c_str(), classNotifierName.c_str() );
	fprintf( header, "\t}\n" );
	fprintf( header, "\tvirtual const char* name()\n" );
	fprintf( header, "\t{\n" );
	fprintf( header, "\t\treturn %s_WrapperForSubscriber<T, typename %s::BufferT>::name();\n", s.name.c_str(), classNotifierName.c_str() );
	fprintf( header, "\t}\n" );
	fprintf( header, "\tvoid subscribe(GMQ_COLL string path)\n" );
	fprintf( header, "\t{\n" );
	fprintf( header, "\t\tregistrar.subscribe( this, path );\n" );
	fprintf( header, "\t}\n" );
	fprintf( header, "};\n\n" );
}

void impl_GeneratePublishableStateWrapperForConcentrator( FILE* header, Root& root, CompositeType& s, const GenerationConfig& config)
{
	assert( s.type == CompositeType::Type::publishable );

	std::string structName = getHelperClassName(s);

	fprintf( header, "template<class T, class InputBufferT, class ComposerT>\n" );
	fprintf( header, "class %s_WrapperForConcentrator : public globalmq::marshalling::StateConcentratorBase<InputBufferT, ComposerT>\n", s.name.c_str() );
	fprintf( header, "{\n" );
	fprintf( header, "\tT t;\n" );
	fprintf( header, "\tusing BufferT = typename ComposerT::BufferType;\n" );

	impl_GeneratePublishableStateMemberPresenceCheckingBlock( header, root, s );

	fprintf( header, "\npublic:\n" );
	fprintf( header, "\tstatic constexpr uint64_t numTypeID = %lld;\n", s.numID );
	fprintf( header, "\n" );
	fprintf( header, "\t%s_WrapperForConcentrator() {}\n", s.name.c_str() );
	fprintf( header, "\tconst char* name() {return \"%s\";}\n", s.name.c_str() );
	fprintf( header, "\t\n" );

	fprintf( header, "\t// Acting as publisher\n" );
	fprintf( header, "\tvirtual void generateStateSyncMessage( globalmq::marshalling2::ComposerBase& composer ) override\n" );

	fprintf( header, "\t{\n" );
	for (auto& each : config.composerNames)
		fprintf( header, "\t\tif(auto ptr = dynamic_cast<%s*>(&composer)) { compose(*ptr); return; }\n", each.c_str() );

	fprintf( header, "\t\tthrow std::exception();\n" );
	fprintf( header, "\t}\n" );

	for (auto& each : config.composerNames)
		fprintf( header, "\tvoid compose( %s& composer ) { %s::compose( composer, t ); }\n", each.c_str(), structName.c_str() );

	fprintf( header, "\n" );

	fprintf( header, "\t// Acting as subscriber\n" );
	fprintf( header, "\tvirtual void applyGmqMessageWithUpdates( globalmq::marshalling::GmqParser<BufferT>& parser ) override { throw std::exception(); }\n" );
	fprintf( header, "\tvirtual void applyJsonMessageWithUpdates( globalmq::marshalling::JsonParser<BufferT>& parser ) override { throw std::exception(); }\n" );
	fprintf( header, "\tvirtual void applyGmqStateSyncMessage( globalmq::marshalling::GmqParser<BufferT>& parser ) override { throw std::exception(); }\n" );
	fprintf( header, "\tvirtual void applyJsonStateSyncMessage( globalmq::marshalling::JsonParser<BufferT>& parser ) override { throw std::exception(); }\n" );
	fprintf( header, "\n" );

	fprintf( header, "\tvirtual void applyMessageWithUpdates( globalmq::marshalling2::ParserBase& parser ) override\n" );
	fprintf( header, "\t{\n" );
	for (auto& each : config.parserNames)
		fprintf( header, "\t\tif(auto ptr = dynamic_cast<%s*>(&parser)) { applyMessageWithUpdates(*ptr); return; }\n", each.c_str() );

	fprintf( header, "\t\tthrow std::exception();\n" );
	fprintf( header, "\t}\n" );

	fprintf( header, "\tvirtual void applyStateSyncMessage( globalmq::marshalling2::ParserBase& parser ) override\n" );
	fprintf( header, "\t{\n" );
	for (auto& each : config.parserNames)
		fprintf( header, "\t\tif(auto ptr = dynamic_cast<%s*>(&parser)) { parseStateSyncMessage(*ptr); return; }\n", each.c_str() );

	fprintf( header, "\t\tthrow std::exception();\n" );
	fprintf( header, "\t}\n" );
	fprintf( header, "\n" );


	for (auto& each : config.parserNames)
	{
		impl_GenerateApplyUpdateMessageMemberFn(header, root, s, false, each);

		fprintf(header, "\n");

		impl_generateParseFunctionForPublishableState(header, root, s, false, each);

		fprintf(header, "\n");
	}

	fprintf( header, "};\n\n" );
}

void impl_GeneratePublishableStructWrapperForwardDeclaration( FILE* header, Root& root, CompositeType& s )
{
	assert( s.type == CompositeType::Type::structure || s.type == CompositeType::Type::discriminated_union );

	fprintf( header, "template<class T> class %s_RefWrapper;\n", s.name.c_str() );
}

void impl_GeneratePublishableStructWrapper( FILE* header, Root& root, CompositeType& s )
{
	assert( s.type == CompositeType::Type::structure || s.type == CompositeType::Type::discriminated_union );

	fprintf( header, "template<class T>\n" );
	fprintf( header, "class %s_RefWrapper\n", s.name.c_str() );
	fprintf( header, "{\n" );
	fprintf( header, "\tT& t;\n" );

	impl_GeneratePublishableStateMemberPresenceCheckingBlock( header, root, s );
//	impl_GeneratePublishableMemberUpdateNotifierPresenceCheckingBlock( header, root, s, "\t" );

	fprintf( header, "\npublic:\n" );
	fprintf( header, "\t%s_RefWrapper( T& actual ) : t( actual ) {}\n", s.name.c_str() );

	impl_GeneratePublishableStateMemberAccessors( header, root, s, false );

	fprintf( header, "};\n\n" );
}

void impl_GeneratePublishableStructWrapper4SetForwardDeclaration( FILE* header, Root& root, CompositeType& s )
{
	assert( s.type == CompositeType::Type::structure || s.type == CompositeType::Type::discriminated_union );

	fprintf( header, "template<class T, class RootT> class %s_RefWrapper4Set;\n", s.name.c_str() );
}

void impl_GeneratePublishableStructWrapper4Set( FILE* header, Root& root, CompositeType& s )
{
	assert( s.type == CompositeType::Type::structure || s.type == CompositeType::Type::discriminated_union );

	fprintf( header, "template<class T, class RootT>\n" );
	fprintf( header, "class %s_RefWrapper4Set\n", s.name.c_str() );
	fprintf( header, "{\n" );
	fprintf( header, "\tT& t;\n" );
	fprintf( header, "\tRootT& root;\n" );
	fprintf( header, "\tGMQ_COLL vector<uint64_t> address;\n" );

	impl_GeneratePublishableStateMemberPresenceCheckingBlock( header, root, s );

	fprintf( header, "\npublic:\n" );
	fprintf( header, "\t%s_RefWrapper4Set( T& actual, RootT& root_, GMQ_COLL vector<uint64_t>&& address_ )\n", s.name.c_str() );
	fprintf( header, "\t\t: t( actual ), root( root_ ), address( std::move(address_) )\n" );
	fprintf( header, "\t\t{ }\n" );

	impl_GeneratePublishableStateMemberAccessors( header, root, s, true );

	fprintf( header, "};\n\n" );
}


void collectMemberNamesFromPublishableObjects( vector<unique_ptr<CompositeType>> &structs, set<string>& names )
{
	for ( auto& s : structs )
	{
		assert( s != nullptr );
		assert( s->type == CompositeType::structure || s->type == CompositeType::Type::discriminated_union || s->type == CompositeType::publishable );
		if ( s->type == CompositeType::publishable || ( s->type == CompositeType::structure && s->isStruct4Publishing ) )
			for ( auto& member : s->getMembers() )
			{
				assert( member != nullptr );
				names.insert( member->name );
			}
		else if ( s->type == CompositeType::discriminated_union && s->isStruct4Publishing )
			for ( auto& cs : s->getDiscriminatedUnionCases() )
				for ( auto& member : cs->getMembers() )
				{
					assert( member != nullptr );
					names.insert( member->name );
				}
	}
}

void collectVectorAndDictionaryMemberNamesFromPublishableObjects( vector<unique_ptr<CompositeType>> &structs, set<string>& vectorNames, set<string>& dictionaryNames )
{
	for ( auto& s : structs )
	{
		assert( s != nullptr );
		assert( s->type == CompositeType::structure || s->type == CompositeType::discriminated_union || s->type == CompositeType::publishable );
		if ( s->type == CompositeType::publishable || ( s->type == CompositeType::structure && s->isStruct4Publishing ) )
			for ( auto& member : s->getMembers() )
			{
				assert( member != nullptr );
				if ( member->type.kind == MessageParameterType::KIND::VECTOR )
					vectorNames.insert( member->name );
				else if ( member->type.kind == MessageParameterType::KIND::DICTIONARY )
					dictionaryNames.insert( member->name );
			}
		else if ( s->type == CompositeType::discriminated_union && s->isStruct4Publishing )
			for ( auto& cs : s->getDiscriminatedUnionCases() )
				for ( auto& member : cs->getMembers() )
				{
					assert( member != nullptr );
					if ( member->type.kind == MessageParameterType::KIND::VECTOR )
						vectorNames.insert( member->name );
					else if ( member->type.kind == MessageParameterType::KIND::DICTIONARY )
						dictionaryNames.insert( member->name );
				}
	}
}

void generateNotifierPresenceTesterBlock( FILE* header, Root& root )
{
	set<string> names;
	collectMemberNamesFromPublishableObjects( root.publishables, names );
	collectMemberNamesFromPublishableObjects( root.structs, names );
	names.insert( "currentVariant" ); // TODO: if there are DUs...
	set<string> namesOfVectors;
	set<string> namesOfDictionaries;
	collectVectorAndDictionaryMemberNamesFromPublishableObjects( root.publishables, namesOfVectors, namesOfDictionaries );
	collectVectorAndDictionaryMemberNamesFromPublishableObjects( root.structs, namesOfVectors, namesOfDictionaries );

	fprintf( header, "// member update notifier presence checks\n" );
	fprintf( header, "using index_type_for_array_notifiers = size_t&;\n" );
	fprintf( header, "template<typename T> concept has_full_update_notifier_call = requires(T t) { { t.notifyFullyUpdated() }; };\n" );
	fprintf( header, "template<typename T> concept has_update_notifier_call = requires(T t) { { t.notifyUpdated() }; };\n" );
	for ( auto& name : names )
	{
		fprintf( header, 
			"template<typename T> concept has_void_update_notifier_call_for_%s = requires(T t) { { t.notifyUpdated_%s() }; };\n",
			name.c_str(), name.c_str()
		);
		fprintf( header, 
			"template<typename StateT, typename MemberT> concept has_update_notifier_call_for_%s = requires { { std::declval<StateT>().notifyUpdated_%s(std::declval<MemberT>()) }; };\n",
			name.c_str(), name.c_str()
		);

		if ( namesOfVectors.find( name ) != namesOfVectors.end() )
		{
			fprintf( header, 
				"template<typename T> concept has_element_updated_void_notifier_call_for_%s = requires(T t) { { t.notifyElementUpdated_%s() }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT> concept has_element_updated_notifier_call_for_%s = requires { { std::declval<StateT>().notifyElementUpdated_%s(std::declval<index_type_for_array_notifiers>()) }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT, typename MemberT> concept has_full_element_updated_notifier_call_for_%s = requires { { std::declval<StateT>().notifyElementUpdated_%s(std::declval<index_type_for_array_notifiers>(), std::declval<MemberT>()) }; };\n",
				name.c_str(), name.c_str()
			);

			fprintf( header, 
				"template<typename T> concept has_void_insert_notifier_call_for_%s = requires(T t) { { t.notifyInserted_%s() }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT> concept has_insert_notifier_call2_for_%s = requires { { std::declval<StateT>().notifyInserted_%s(std::declval<index_type_for_array_notifiers>(), std::declval<index_type_for_array_notifiers>()) }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT, typename MemberT> concept has_insert_notifier_call3_for_%s = requires { { std::declval<StateT>().notifyInserted_%s(std::declval<index_type_for_array_notifiers>(), std::declval<index_type_for_array_notifiers>()), std::declval<MemberT>() }; };\n",
				name.c_str(), name.c_str()
			);

			fprintf( header, 
				"template<typename T> concept has_void_erased_notifier_call_for_%s = requires(T t) { { t.notifyErased_%s() }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT> concept has_erased_notifier_call2_for_%s = requires { { std::declval<StateT>().notifyErased_%s(std::declval<index_type_for_array_notifiers>(), std::declval<index_type_for_array_notifiers>()) }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT, typename MemberT> concept has_erased_notifier_call3_for_%s = requires { { std::declval<StateT>().notifyErased_%s(std::declval<index_type_for_array_notifiers>(), std::declval<index_type_for_array_notifiers>(), std::declval<MemberT>()) }; };\n",
				name.c_str(), name.c_str()
			);
		}

		if ( namesOfDictionaries.find( name ) != namesOfDictionaries.end() )
		{
			fprintf( header, "\t//// TODO: revise notifier list !!!!!!!!!!!!!!!!!!!!!!!!!\n" );
			fprintf( header, 
				"template<typename T> concept has_value_updated_void_notifier_call_for_%s = requires(T t) { { t.notifyValueUpdated_%s() }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT, typename KeyT> concept has_value_updated_notifier_call_for_%s = requires { { std::declval<StateT>().notifyValueUpdated_%s(std::declval<KeyT>()) }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT, typename KeyT, typename ValueT> concept has_full_value_updated_notifier_call_for_%s = requires { { std::declval<StateT>().notifyValueUpdated_%s(std::declval<KeyT>(), std::declval<ValueT>()) }; };\n",
				name.c_str(), name.c_str()
			);

			fprintf( header, 
				"template<typename T> concept has_void_insert_notifier_call_for_%s = requires(T t) { { t.notifyInserted_%s() }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT, typename KeyT> concept has_insert_notifier_call2_for_%s = requires { { std::declval<StateT>().notifyInserted_%s(std::declval<KeyT>()) }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT, typename KeyT, typename ValueT> concept has_insert_notifier_call3_for_%s = requires { { std::declval<StateT>().notifyInserted_%s(std::declval<KeyT>(), std::declval<ValueT>()) }; };\n",
				name.c_str(), name.c_str()
			);

			fprintf( header, 
				"template<typename T> concept has_void_removed_notifier_call_for_%s = requires(T t) { { t.notifyRemoved_%s() }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT, typename KeyT> concept has_removed_notifier_call2_for_%s = requires { { std::declval<StateT>().notifyErased_%s(std::declval<KeyT>()) }; };\n",
				name.c_str(), name.c_str()
			);
			fprintf( header, 
				"template<typename StateT, typename KeyT, typename ValueT> concept has_removed_notifier_call3_for_%s = requires { { std::declval<StateT>().notifyErased_%s(std::declval<KeyT>(), std::declval<ValueT>()) }; };\n",
				name.c_str(), name.c_str()
			);
		}
	}
	fprintf( header, "\n" );
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
	if ( generatePlatformSpec )
		impl_GeneratePublishableStatePlatformWrapperForPublisher( header, root, s, config.platformPrefix, config.classNotifierName );

	impl_GeneratePublishableStateWrapperForSubscriber( header, root, s, config );
	if ( generatePlatformSpec )
		impl_GeneratePublishableStatePlatformWrapperForSubscriber( header, root, s, config.platformPrefix, config.classNotifierName );

	impl_GeneratePublishableStateWrapperForConcentrator( header, root, s, config );
}

}//namespace cplusplus
