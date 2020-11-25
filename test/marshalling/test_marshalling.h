#ifndef _test_marshalling_h_guard
#define _test_marshalling_h_guard

#include <marshalling.h>

namespace m {

//////////////////////////////////////////////////////////////
//
//  Scopes:
//
//  scope_one
//  {
//    point3D_alias
//    point_alias
//  }
//
//  level_trace
//  {
//    LevelTraceData
//  }
//
//  infrastructural
//  {
//    PolygonSt
//    point
//    point3D
//  }
//
//  test_gmq
//  {
//    message_one
//  }
//
//  test_json
//  {
//    message_one
//  }
//
//////////////////////////////////////////////////////////////

using CharacterParam_Type = NamedParameter<struct CharacterParam_Struct>;
using ID_Type = NamedParameter<struct ID_Struct>;
using LineMap_Type = NamedParameter<struct LineMap_Struct>;
using ObstacleMap_Type = NamedParameter<struct ObstacleMap_Struct>;
using Points_Type = NamedParameter<struct Points_Struct>;
using PolygonMap_Type = NamedParameter<struct PolygonMap_Struct>;
using Size_Type = NamedParameter<struct Size_Struct>;
using X_Type = NamedParameter<struct X_Struct>;
using Y_Type = NamedParameter<struct Y_Struct>;
using Z_Type = NamedParameter<struct Z_Struct>;
using a_Type = NamedParameter<struct a_Struct>;
using b_Type = NamedParameter<struct b_Struct>;
using concaveMap_Type = NamedParameter<struct concaveMap_Struct>;
using eighthParam_Type = NamedParameter<struct eighthParam_Struct>;
using fifthParam_Type = NamedParameter<struct fifthParam_Struct>;
using firstParam_Type = NamedParameter<struct firstParam_Struct>;
using forthParam_Type = NamedParameter<struct forthParam_Struct>;
using jumpMap_Type = NamedParameter<struct jumpMap_Struct>;
using ninethParam_Type = NamedParameter<struct ninethParam_Struct>;
using obstacleMap_Type = NamedParameter<struct obstacleMap_Struct>;
using point_Type = NamedParameter<struct point_Struct>;
using polygonMap_Type = NamedParameter<struct polygonMap_Struct>;
using polygonSpeed_Type = NamedParameter<struct polygonSpeed_Struct>;
using portalMap_Type = NamedParameter<struct portalMap_Struct>;
using pt_Type = NamedParameter<struct pt_Struct>;
using secondParam_Type = NamedParameter<struct secondParam_Struct>;
using seventhParam_Type = NamedParameter<struct seventhParam_Struct>;
using sixthParam_Type = NamedParameter<struct sixthParam_Struct>;
using tenthParam_Type = NamedParameter<struct tenthParam_Struct>;
using thirdParam_Type = NamedParameter<struct thirdParam_Struct>;
using x_Type = NamedParameter<struct x_Struct>;
using y_Type = NamedParameter<struct y_Struct>;
using z_Type = NamedParameter<struct z_Struct>;

constexpr CharacterParam_Type::TypeConverter CharacterParam;
constexpr ID_Type::TypeConverter ID;
constexpr LineMap_Type::TypeConverter LineMap;
constexpr ObstacleMap_Type::TypeConverter ObstacleMap;
constexpr Points_Type::TypeConverter Points;
constexpr PolygonMap_Type::TypeConverter PolygonMap;
constexpr Size_Type::TypeConverter Size;
constexpr X_Type::TypeConverter X;
constexpr Y_Type::TypeConverter Y;
constexpr Z_Type::TypeConverter Z;
constexpr a_Type::TypeConverter a;
constexpr b_Type::TypeConverter b;
constexpr concaveMap_Type::TypeConverter concaveMap;
constexpr eighthParam_Type::TypeConverter eighthParam;
constexpr fifthParam_Type::TypeConverter fifthParam;
constexpr firstParam_Type::TypeConverter firstParam;
constexpr forthParam_Type::TypeConverter forthParam;
constexpr jumpMap_Type::TypeConverter jumpMap;
constexpr ninethParam_Type::TypeConverter ninethParam;
constexpr obstacleMap_Type::TypeConverter obstacleMap;
constexpr point_Type::TypeConverter point;
constexpr polygonMap_Type::TypeConverter polygonMap;
constexpr polygonSpeed_Type::TypeConverter polygonSpeed;
constexpr portalMap_Type::TypeConverter portalMap;
constexpr pt_Type::TypeConverter pt;
constexpr secondParam_Type::TypeConverter secondParam;
constexpr seventhParam_Type::TypeConverter seventhParam;
constexpr sixthParam_Type::TypeConverter sixthParam;
constexpr tenthParam_Type::TypeConverter tenthParam;
constexpr thirdParam_Type::TypeConverter thirdParam;
constexpr x_Type::TypeConverter x;
constexpr y_Type::TypeConverter y;
constexpr z_Type::TypeConverter z;

namespace scope_one {

using point3D_alias = impl::MessageName<1>;
using point_alias = impl::MessageName<2>;

template<class BufferT, class ... HandlersT >
void handleMessage( BufferT& buffer, HandlersT ... handlers )
{
	uint64_t msgID;

	JsonParser parser( buffer );
	parser.skipDelimiter('{');
	std::string key;
	parser.readKey(&key);
	if (key != "msgid")
		throw std::exception(); // bad format
	parser.readUnsignedIntegerFromJson(&msgID);
	parser.skipSpacesEtc();
	if (!parser.isDelimiter(','))
		throw std::exception(); // bad format
	parser.skipDelimiter(',');
	parser.readKey(&key);
	if (key != "msgbody")
		throw std::exception(); // bad format
	JsonParser p( parser );

	switch ( msgID )
	{
		case point3D_alias::id: impl::implHandleMessage<point3D_alias>( parser, handlers... ); break;
		case point_alias::id: impl::implHandleMessage<point_alias>( parser, handlers... ); break;
	}

	p.skipMessageFromJson();
	parser = p;

	if (!parser.isDelimiter('}'))
		throw std::exception(); // bad format
	parser.skipDelimiter('}');
}

template<typename msgID, class BufferT, typename ... Args>
void composeMessage( BufferT& buffer, Args&& ... args );

//**********************************************************************
// MESSAGE "point3D_alias" Targets: JSON (Alias of point3D)

//**********************************************************************

template<class ComposerT, typename ... Args>
void MESSAGE_point3D_alias_compose(ComposerT& composer, Args&& ... args)
{
	STRUCT_point3D_compose(composer, std::forward<Args>( args )...);
}

template<class ParserT, typename ... Args>
void MESSAGE_point3D_alias_parse(ParserT& p, Args&& ... args)
{
	STRUCT_point3D_parse(p, std::forward<Args>( args )...);
}

//**********************************************************************
// MESSAGE "point_alias" Targets: JSON (Alias of point)

//**********************************************************************

template<class ComposerT, typename ... Args>
void MESSAGE_point_alias_compose(ComposerT& composer, Args&& ... args)
{
	STRUCT_point_compose(composer, std::forward<Args>( args )...);
}

template<class ParserT, typename ... Args>
void MESSAGE_point_alias_parse(ParserT& p, Args&& ... args)
{
	STRUCT_point_parse(p, std::forward<Args>( args )...);
}

template<typename msgID, class BufferT, typename ... Args>
void composeMessage( BufferT& buffer, Args&& ... args )
{
	static_assert( std::is_base_of<impl::MessageNameBase, msgID>::value );
	m::JsonComposer composer( buffer );
	composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
	impl::json::composeNamedSignedInteger( composer, "msgid", msgID::id);
	composer.buff.append( ",\n  ", sizeof(",\n  ") - 1 );
	impl::json::addNamePart( composer, "msgbody" );
	if constexpr ( msgID::id == point3D_alias::id )
		MESSAGE_point3D_alias_compose( composer, std::forward<Args>( args )... );
	else if constexpr ( msgID::id == point_alias::id )
		MESSAGE_point_alias_compose( composer, std::forward<Args>( args )... );
	else
		static_assert( std::is_same<impl::MessageNameBase, msgID>::value, "unexpected value of msgID" ); // note: should be just static_assert(false,"..."); but it seems that in this case clang asserts yet before looking at constexpr conditions
	composer.buff.append( "\n}", 2 );
}

} // namespace scope_one 

namespace level_trace {

using LevelTraceData = impl::MessageName<1>;

template<class BufferT, class ... HandlersT >
void handleMessage( BufferT& buffer, HandlersT ... handlers )
{
	uint64_t msgID;

	JsonParser parser( buffer );
	parser.skipDelimiter('{');
	std::string key;
	parser.readKey(&key);
	if (key != "msgid")
		throw std::exception(); // bad format
	parser.readUnsignedIntegerFromJson(&msgID);
	parser.skipSpacesEtc();
	if (!parser.isDelimiter(','))
		throw std::exception(); // bad format
	parser.skipDelimiter(',');
	parser.readKey(&key);
	if (key != "msgbody")
		throw std::exception(); // bad format
	JsonParser p( parser );

	switch ( msgID )
	{
		case LevelTraceData::id: impl::implHandleMessage<LevelTraceData>( parser, handlers... ); break;
	}

	p.skipMessageFromJson();
	parser = p;

	if (!parser.isDelimiter('}'))
		throw std::exception(); // bad format
	parser.skipDelimiter('}');
}

template<typename msgID, class BufferT, typename ... Args>
void composeMessage( BufferT& buffer, Args&& ... args );

//**********************************************************************
// MESSAGE "LevelTraceData" Targets: JSON (2 parameters)
// 1. STRUCT CharacterParam (REQUIRED)
// 2. VECTOR< STRUCT POINT3DREAL> Points (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void MESSAGE_LevelTraceData_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::MessageType, CharacterParam_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::VectorOfMessageType, Points_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this MESSAGE assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "CharacterParam", arg_1_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_2_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "Points", arg_2_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void MESSAGE_LevelTraceData_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::MessageType, CharacterParam_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::VectorOfMessageType, Points_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this MESSAGE assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "CharacterParam" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		else if ( key == "Points" )
			impl::json::parseJsonParam<ParserT, arg_2_type, false>(arg_2_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

template<typename msgID, class BufferT, typename ... Args>
void composeMessage( BufferT& buffer, Args&& ... args )
{
	static_assert( std::is_base_of<impl::MessageNameBase, msgID>::value );
	m::JsonComposer composer( buffer );
	composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
	impl::json::composeNamedSignedInteger( composer, "msgid", msgID::id);
	composer.buff.append( ",\n  ", sizeof(",\n  ") - 1 );
	impl::json::addNamePart( composer, "msgbody" );
	if constexpr ( msgID::id == LevelTraceData::id )
		MESSAGE_LevelTraceData_compose( composer, std::forward<Args>( args )... );
	else
		static_assert( std::is_same<impl::MessageNameBase, msgID>::value, "unexpected value of msgID" ); // note: should be just static_assert(false,"..."); but it seems that in this case clang asserts yet before looking at constexpr conditions
	composer.buff.append( "\n}", 2 );
}

} // namespace level_trace 

namespace infrastructural {

using PolygonSt = impl::MessageName<2>;
using point = impl::MessageName<4>;
using point3D = impl::MessageName<5>;

template<class BufferT, class ... HandlersT >
void handleMessage( BufferT& buffer, HandlersT ... handlers )
{
	uint64_t msgID;

	JsonParser parser( buffer );
	parser.skipDelimiter('{');
	std::string key;
	parser.readKey(&key);
	if (key != "msgid")
		throw std::exception(); // bad format
	parser.readUnsignedIntegerFromJson(&msgID);
	parser.skipSpacesEtc();
	if (!parser.isDelimiter(','))
		throw std::exception(); // bad format
	parser.skipDelimiter(',');
	parser.readKey(&key);
	if (key != "msgbody")
		throw std::exception(); // bad format
	JsonParser p( parser );

	switch ( msgID )
	{
		case PolygonSt::id: impl::implHandleMessage<PolygonSt>( parser, handlers... ); break;
		case point::id: impl::implHandleMessage<point>( parser, handlers... ); break;
		case point3D::id: impl::implHandleMessage<point3D>( parser, handlers... ); break;
	}

	p.skipMessageFromJson();
	parser = p;

	if (!parser.isDelimiter('}'))
		throw std::exception(); // bad format
	parser.skipDelimiter('}');
}

template<typename msgID, class BufferT, typename ... Args>
void composeMessage( BufferT& buffer, Args&& ... args );

//**********************************************************************
// MESSAGE "PolygonSt" Targets: JSON (6 parameters)
// 1. VECTOR< STRUCT PolygonMap> polygonMap (REQUIRED)
// 2. VECTOR< STRUCT PolygonMap> concaveMap (REQUIRED)
// 3. VECTOR< STRUCT ObstacleMap> obstacleMap (REQUIRED)
// 4. VECTOR< STRUCT LineMap> portalMap (REQUIRED)
// 5. VECTOR< STRUCT LineMap> jumpMap (REQUIRED)
// 6. REAL polygonSpeed (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void MESSAGE_PolygonSt_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::VectorOfMessageType, polygonMap_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::VectorOfMessageType, concaveMap_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::VectorOfMessageType, obstacleMap_Type::Name>;
	using arg_4_type = NamedParameterWithType<impl::VectorOfMessageType, portalMap_Type::Name>;
	using arg_5_type = NamedParameterWithType<impl::VectorOfMessageType, jumpMap_Type::Name>;
	using arg_6_type = NamedParameterWithType<impl::RealType, polygonSpeed_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_4_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_5_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_6_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this MESSAGE assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "polygonMap", arg_1_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_2_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "concaveMap", arg_2_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_3_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "obstacleMap", arg_3_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_4_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "portalMap", arg_4_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_5_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "jumpMap", arg_5_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_6_type, true, FloatingDefault<0ll,-1023ll>, int, 0>(composer, "polygonSpeed", arg_6_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void MESSAGE_PolygonSt_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::VectorOfMessageType, polygonMap_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::VectorOfMessageType, concaveMap_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::VectorOfMessageType, obstacleMap_Type::Name>;
	using arg_4_type = NamedParameterWithType<impl::VectorOfMessageType, portalMap_Type::Name>;
	using arg_5_type = NamedParameterWithType<impl::VectorOfMessageType, jumpMap_Type::Name>;
	using arg_6_type = NamedParameterWithType<impl::RealType, polygonSpeed_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_4_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_5_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_6_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this MESSAGE assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "polygonMap" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		else if ( key == "concaveMap" )
			impl::json::parseJsonParam<ParserT, arg_2_type, false>(arg_2_type::nameAndTypeID, p, args...);
		else if ( key == "obstacleMap" )
			impl::json::parseJsonParam<ParserT, arg_3_type, false>(arg_3_type::nameAndTypeID, p, args...);
		else if ( key == "portalMap" )
			impl::json::parseJsonParam<ParserT, arg_4_type, false>(arg_4_type::nameAndTypeID, p, args...);
		else if ( key == "jumpMap" )
			impl::json::parseJsonParam<ParserT, arg_5_type, false>(arg_5_type::nameAndTypeID, p, args...);
		else if ( key == "polygonSpeed" )
			impl::json::parseJsonParam<ParserT, arg_6_type, false>(arg_6_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

//**********************************************************************
// MESSAGE "point" NONEXTENDABLE Targets: JSON (1 parameters)
// 1. STRUCT point (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void MESSAGE_point_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::MessageType, point_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this MESSAGE assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "point", arg_1_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void MESSAGE_point_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::MessageType, point_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this MESSAGE assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "point" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

//**********************************************************************
// MESSAGE "point3D" NONEXTENDABLE Targets: JSON (1 parameters)
// 1. STRUCT pt (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void MESSAGE_point3D_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::MessageType, pt_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this MESSAGE assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "pt", arg_1_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void MESSAGE_point3D_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::MessageType, pt_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this MESSAGE assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "pt" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

template<typename msgID, class BufferT, typename ... Args>
void composeMessage( BufferT& buffer, Args&& ... args )
{
	static_assert( std::is_base_of<impl::MessageNameBase, msgID>::value );
	m::JsonComposer composer( buffer );
	composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
	impl::json::composeNamedSignedInteger( composer, "msgid", msgID::id);
	composer.buff.append( ",\n  ", sizeof(",\n  ") - 1 );
	impl::json::addNamePart( composer, "msgbody" );
	if constexpr ( msgID::id == PolygonSt::id )
		MESSAGE_PolygonSt_compose( composer, std::forward<Args>( args )... );
	else if constexpr ( msgID::id == point::id )
		MESSAGE_point_compose( composer, std::forward<Args>( args )... );
	else if constexpr ( msgID::id == point3D::id )
		MESSAGE_point3D_compose( composer, std::forward<Args>( args )... );
	else
		static_assert( std::is_same<impl::MessageNameBase, msgID>::value, "unexpected value of msgID" ); // note: should be just static_assert(false,"..."); but it seems that in this case clang asserts yet before looking at constexpr conditions
	composer.buff.append( "\n}", 2 );
}

} // namespace infrastructural 

namespace test_gmq {

using message_one = impl::MessageName<3>;

template<class BufferT, class ... HandlersT >
void handleMessage( BufferT& buffer, HandlersT ... handlers )
{
	uint64_t msgID;

	GmqParser parser( buffer );
	parser.parseUnsignedInteger( &msgID );
	switch ( msgID )
	{
		case message_one::id: impl::implHandleMessage<message_one>( parser, handlers... ); break;
	}

}

template<typename msgID, class BufferT, typename ... Args>
void composeMessage( BufferT& buffer, Args&& ... args );

//**********************************************************************
// MESSAGE "message_one" Targets: GMQ (10 parameters)
// 1. INTEGER firstParam (REQUIRED)
// 2. VECTOR<INTEGER> secondParam (REQUIRED)
// 3. VECTOR< STRUCT point3D> thirdParam (REQUIRED)
// 4. UINTEGER forthParam (REQUIRED)
// 5. CHARACTER_STRING fifthParam (REQUIRED)
// 6. VECTOR<NONEXTENDABLE STRUCT point> sixthParam (REQUIRED)
// 7. REAL seventhParam (REQUIRED)
// 8. STRUCT eighthParam (REQUIRED)
// 9. STRUCT ninethParam (REQUIRED)
// 10. VECTOR<REAL> tenthParam (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void MESSAGE_message_one_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, firstParam_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::VectorOfSympleTypes<impl::SignedIntegralType>, secondParam_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::VectorOfMessageType, thirdParam_Type::Name>;
	using arg_4_type = NamedParameterWithType<impl::UnsignedIntegralType, forthParam_Type::Name>;
	using arg_5_type = NamedParameterWithType<impl::StringType, fifthParam_Type::Name>;
	using arg_6_type = NamedParameterWithType<impl::VectorOfNonextMessageTypes, sixthParam_Type::Name>;
	using arg_7_type = NamedParameterWithType<impl::RealType, seventhParam_Type::Name>;
	using arg_8_type = NamedParameterWithType<impl::NonextMessageType, eighthParam_Type::Name>;
	using arg_9_type = NamedParameterWithType<impl::MessageType, ninethParam_Type::Name>;
	using arg_10_type = NamedParameterWithType<impl::VectorOfSympleTypes<impl::RealType>, tenthParam_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_4_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_5_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_6_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_7_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_8_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_9_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_10_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::GMQ, "this MESSAGE assumes only GMQ protocol" );
	impl::gmq::composeParamToGmq<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, arg_1_type::nameAndTypeID, args...);
	impl::gmq::composeParamToGmq<ComposerT, arg_2_type, true, uint64_t, uint64_t, (uint64_t)(0)>(composer, arg_2_type::nameAndTypeID, args...);
	impl::gmq::composeParamToGmq<ComposerT, arg_3_type, true, uint64_t, uint64_t, (uint64_t)(0)>(composer, arg_3_type::nameAndTypeID, args...);
	impl::gmq::composeParamToGmq<ComposerT, arg_4_type, true, uint64_t, uint64_t, (uint64_t)(0)>(composer, arg_4_type::nameAndTypeID, args...);
	impl::gmq::composeParamToGmq<ComposerT, arg_5_type, true, uint64_t, uint64_t, (uint64_t)0>(composer, arg_5_type::nameAndTypeID, args...);
	impl::gmq::composeParamToGmq<ComposerT, arg_6_type, true, uint64_t, uint64_t, (uint64_t)(0)>(composer, arg_6_type::nameAndTypeID, args...);
	impl::gmq::composeParamToGmq<ComposerT, arg_7_type, true, FloatingDefault<0ll,-1023ll>, int, 0>(composer, arg_7_type::nameAndTypeID, args...);
	impl::gmq::composeParamToGmq<ComposerT, arg_8_type, true, uint64_t, uint64_t, (uint64_t)(0)>(composer, arg_8_type::nameAndTypeID, args...);
	impl::gmq::composeParamToGmq<ComposerT, arg_9_type, true, uint64_t, uint64_t, (uint64_t)(0)>(composer, arg_9_type::nameAndTypeID, args...);
	impl::gmq::composeParamToGmq<ComposerT, arg_10_type, true, uint64_t, uint64_t, (uint64_t)(0)>(composer, arg_10_type::nameAndTypeID, args...);
}

template<class ParserT, typename ... Args>
void MESSAGE_message_one_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, firstParam_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::VectorOfSympleTypes<impl::SignedIntegralType>, secondParam_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::VectorOfMessageType, thirdParam_Type::Name>;
	using arg_4_type = NamedParameterWithType<impl::UnsignedIntegralType, forthParam_Type::Name>;
	using arg_5_type = NamedParameterWithType<impl::StringType, fifthParam_Type::Name>;
	using arg_6_type = NamedParameterWithType<impl::VectorOfNonextMessageTypes, sixthParam_Type::Name>;
	using arg_7_type = NamedParameterWithType<impl::RealType, seventhParam_Type::Name>;
	using arg_8_type = NamedParameterWithType<impl::NonextMessageType, eighthParam_Type::Name>;
	using arg_9_type = NamedParameterWithType<impl::MessageType, ninethParam_Type::Name>;
	using arg_10_type = NamedParameterWithType<impl::VectorOfSympleTypes<impl::RealType>, tenthParam_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_4_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_5_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_6_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_7_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_8_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_9_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_10_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::GMQ, "this MESSAGE assumes only GMQ protocol" );
	impl::gmq::parseGmqParam<ParserT, arg_1_type, false>(p, arg_1_type::nameAndTypeID, args...);
	impl::gmq::parseGmqParam<ParserT, arg_2_type, false>(p, arg_2_type::nameAndTypeID, args...);
	impl::gmq::parseGmqParam<ParserT, arg_3_type, false>(p, arg_3_type::nameAndTypeID, args...);
	impl::gmq::parseGmqParam<ParserT, arg_4_type, false>(p, arg_4_type::nameAndTypeID, args...);
	impl::gmq::parseGmqParam<ParserT, arg_5_type, false>(p, arg_5_type::nameAndTypeID, args...);
	impl::gmq::parseGmqParam<ParserT, arg_6_type, false>(p, arg_6_type::nameAndTypeID, args...);
	impl::gmq::parseGmqParam<ParserT, arg_7_type, false>(p, arg_7_type::nameAndTypeID, args...);
	impl::gmq::parseGmqParam<ParserT, arg_8_type, false>(p, arg_8_type::nameAndTypeID, args...);
	impl::gmq::parseGmqParam<ParserT, arg_9_type, false>(p, arg_9_type::nameAndTypeID, args...);
	impl::gmq::parseGmqParam<ParserT, arg_10_type, false>(p, arg_10_type::nameAndTypeID, args...);
}

template<typename msgID, class BufferT, typename ... Args>
void composeMessage( BufferT& buffer, Args&& ... args )
{
	static_assert( std::is_base_of<impl::MessageNameBase, msgID>::value );
	m::GmqComposer composer( buffer );
	impl::composeUnsignedInteger( composer, msgID::id );
	if constexpr ( msgID::id == message_one::id )
		MESSAGE_message_one_compose( composer, std::forward<Args>( args )... );
	else
		static_assert( std::is_same<impl::MessageNameBase, msgID>::value, "unexpected value of msgID" ); // note: should be just static_assert(false,"..."); but it seems that in this case clang asserts yet before looking at constexpr conditions
}

} // namespace test_gmq 

namespace test_json {

using message_one = impl::MessageName<3>;

template<class BufferT, class ... HandlersT >
void handleMessage( BufferT& buffer, HandlersT ... handlers )
{
	uint64_t msgID;

	JsonParser parser( buffer );
	parser.skipDelimiter('{');
	std::string key;
	parser.readKey(&key);
	if (key != "msgid")
		throw std::exception(); // bad format
	parser.readUnsignedIntegerFromJson(&msgID);
	parser.skipSpacesEtc();
	if (!parser.isDelimiter(','))
		throw std::exception(); // bad format
	parser.skipDelimiter(',');
	parser.readKey(&key);
	if (key != "msgbody")
		throw std::exception(); // bad format
	JsonParser p( parser );

	switch ( msgID )
	{
		case message_one::id: impl::implHandleMessage<message_one>( parser, handlers... ); break;
	}

	p.skipMessageFromJson();
	parser = p;

	if (!parser.isDelimiter('}'))
		throw std::exception(); // bad format
	parser.skipDelimiter('}');
}

template<typename msgID, class BufferT, typename ... Args>
void composeMessage( BufferT& buffer, Args&& ... args );

//**********************************************************************
// MESSAGE "message_one" Targets: JSON (10 parameters)
// 1. INTEGER firstParam (REQUIRED)
// 2. VECTOR<INTEGER> secondParam (REQUIRED)
// 3. VECTOR< STRUCT point3D> thirdParam (REQUIRED)
// 4. UINTEGER forthParam (REQUIRED)
// 5. CHARACTER_STRING fifthParam (REQUIRED)
// 6. VECTOR<NONEXTENDABLE STRUCT point> sixthParam (REQUIRED)
// 7. REAL seventhParam (REQUIRED)
// 8. STRUCT eighthParam (REQUIRED)
// 9. STRUCT ninethParam (REQUIRED)
// 10. VECTOR<REAL> tenthParam (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void MESSAGE_message_one_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, firstParam_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::VectorOfSympleTypes<impl::SignedIntegralType>, secondParam_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::VectorOfMessageType, thirdParam_Type::Name>;
	using arg_4_type = NamedParameterWithType<impl::UnsignedIntegralType, forthParam_Type::Name>;
	using arg_5_type = NamedParameterWithType<impl::StringType, fifthParam_Type::Name>;
	using arg_6_type = NamedParameterWithType<impl::VectorOfNonextMessageTypes, sixthParam_Type::Name>;
	using arg_7_type = NamedParameterWithType<impl::RealType, seventhParam_Type::Name>;
	using arg_8_type = NamedParameterWithType<impl::NonextMessageType, eighthParam_Type::Name>;
	using arg_9_type = NamedParameterWithType<impl::MessageType, ninethParam_Type::Name>;
	using arg_10_type = NamedParameterWithType<impl::VectorOfSympleTypes<impl::RealType>, tenthParam_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_4_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_5_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_6_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_7_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_8_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_9_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_10_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this MESSAGE assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "firstParam", arg_1_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_2_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "secondParam", arg_2_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_3_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "thirdParam", arg_3_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_4_type, true, uint64_t, uint64_t, (uint64_t)(0)>(composer, "forthParam", arg_4_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_5_type, true, uint64_t, uint64_t, (uint64_t)(0)>(composer, "fifthParam", arg_5_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_6_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "sixthParam", arg_6_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_7_type, true, FloatingDefault<0ll,-1023ll>, int, 0>(composer, "seventhParam", arg_7_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_8_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "eighthParam", arg_8_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_9_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "ninethParam", arg_9_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_10_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "tenthParam", arg_10_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void MESSAGE_message_one_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, firstParam_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::VectorOfSympleTypes<impl::SignedIntegralType>, secondParam_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::VectorOfMessageType, thirdParam_Type::Name>;
	using arg_4_type = NamedParameterWithType<impl::UnsignedIntegralType, forthParam_Type::Name>;
	using arg_5_type = NamedParameterWithType<impl::StringType, fifthParam_Type::Name>;
	using arg_6_type = NamedParameterWithType<impl::VectorOfNonextMessageTypes, sixthParam_Type::Name>;
	using arg_7_type = NamedParameterWithType<impl::RealType, seventhParam_Type::Name>;
	using arg_8_type = NamedParameterWithType<impl::NonextMessageType, eighthParam_Type::Name>;
	using arg_9_type = NamedParameterWithType<impl::MessageType, ninethParam_Type::Name>;
	using arg_10_type = NamedParameterWithType<impl::VectorOfSympleTypes<impl::RealType>, tenthParam_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_4_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_5_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_6_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_7_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_8_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_9_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_10_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this MESSAGE assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "firstParam" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		else if ( key == "secondParam" )
			impl::json::parseJsonParam<ParserT, arg_2_type, false>(arg_2_type::nameAndTypeID, p, args...);
		else if ( key == "thirdParam" )
			impl::json::parseJsonParam<ParserT, arg_3_type, false>(arg_3_type::nameAndTypeID, p, args...);
		else if ( key == "forthParam" )
			impl::json::parseJsonParam<ParserT, arg_4_type, false>(arg_4_type::nameAndTypeID, p, args...);
		else if ( key == "fifthParam" )
			impl::json::parseJsonParam<ParserT, arg_5_type, false>(arg_5_type::nameAndTypeID, p, args...);
		else if ( key == "sixthParam" )
			impl::json::parseJsonParam<ParserT, arg_6_type, false>(arg_6_type::nameAndTypeID, p, args...);
		else if ( key == "seventhParam" )
			impl::json::parseJsonParam<ParserT, arg_7_type, false>(arg_7_type::nameAndTypeID, p, args...);
		else if ( key == "eighthParam" )
			impl::json::parseJsonParam<ParserT, arg_8_type, false>(arg_8_type::nameAndTypeID, p, args...);
		else if ( key == "ninethParam" )
			impl::json::parseJsonParam<ParserT, arg_9_type, false>(arg_9_type::nameAndTypeID, p, args...);
		else if ( key == "tenthParam" )
			impl::json::parseJsonParam<ParserT, arg_10_type, false>(arg_10_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

template<typename msgID, class BufferT, typename ... Args>
void composeMessage( BufferT& buffer, Args&& ... args )
{
	static_assert( std::is_base_of<impl::MessageNameBase, msgID>::value );
	m::JsonComposer composer( buffer );
	composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
	impl::json::composeNamedSignedInteger( composer, "msgid", msgID::id);
	composer.buff.append( ",\n  ", sizeof(",\n  ") - 1 );
	impl::json::addNamePart( composer, "msgbody" );
	if constexpr ( msgID::id == message_one::id )
		MESSAGE_message_one_compose( composer, std::forward<Args>( args )... );
	else
		static_assert( std::is_same<impl::MessageNameBase, msgID>::value, "unexpected value of msgID" ); // note: should be just static_assert(false,"..."); but it seems that in this case clang asserts yet before looking at constexpr conditions
	composer.buff.append( "\n}", 2 );
}

} // namespace test_json 

//**********************************************************************
// STRUCT "CharacterParam" Targets: JSON (2 parameters)
// 1. INTEGER ID (REQUIRED)
// 2. STRUCT Size (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void STRUCT_CharacterParam_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, ID_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::MessageType, Size_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "ID", arg_1_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_2_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "Size", arg_2_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void STRUCT_CharacterParam_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, ID_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::MessageType, Size_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "ID" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		else if ( key == "Size" )
			impl::json::parseJsonParam<ParserT, arg_2_type, false>(arg_2_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

//**********************************************************************
// STRUCT "Size" Targets: JSON (3 parameters)
// 1. REAL X (REQUIRED)
// 2. REAL Y (REQUIRED)
// 3. REAL Z (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void STRUCT_Size_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::RealType, X_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::RealType, Y_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::RealType, Z_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, FloatingDefault<0ll,-1023ll>, int, 0>(composer, "X", arg_1_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_2_type, true, FloatingDefault<0ll,-1023ll>, int, 0>(composer, "Y", arg_2_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_3_type, true, FloatingDefault<0ll,-1023ll>, int, 0>(composer, "Z", arg_3_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void STRUCT_Size_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::RealType, X_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::RealType, Y_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::RealType, Z_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "X" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		else if ( key == "Y" )
			impl::json::parseJsonParam<ParserT, arg_2_type, false>(arg_2_type::nameAndTypeID, p, args...);
		else if ( key == "Z" )
			impl::json::parseJsonParam<ParserT, arg_3_type, false>(arg_3_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

//**********************************************************************
// STRUCT "POINT3DREAL" Targets: JSON (3 parameters)
// 1. REAL X (REQUIRED)
// 2. REAL Y (REQUIRED)
// 3. REAL Z (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void STRUCT_POINT3DREAL_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::RealType, X_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::RealType, Y_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::RealType, Z_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, FloatingDefault<0ll,-1023ll>, int, 0>(composer, "X", arg_1_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_2_type, true, FloatingDefault<0ll,-1023ll>, int, 0>(composer, "Y", arg_2_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_3_type, true, FloatingDefault<0ll,-1023ll>, int, 0>(composer, "Z", arg_3_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void STRUCT_POINT3DREAL_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::RealType, X_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::RealType, Y_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::RealType, Z_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "X" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		else if ( key == "Y" )
			impl::json::parseJsonParam<ParserT, arg_2_type, false>(arg_2_type::nameAndTypeID, p, args...);
		else if ( key == "Z" )
			impl::json::parseJsonParam<ParserT, arg_3_type, false>(arg_3_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

//**********************************************************************
// STRUCT "LineMap" Targets: JSON (1 parameters)
// 1. VECTOR< STRUCT Line> LineMap (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void STRUCT_LineMap_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::VectorOfMessageType, LineMap_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "LineMap", arg_1_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void STRUCT_LineMap_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::VectorOfMessageType, LineMap_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "LineMap" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

//**********************************************************************
// STRUCT "Line" Targets: JSON (2 parameters)
// 1. VECTOR<NONEXTENDABLE STRUCT Vertex> a (REQUIRED)
// 2. VECTOR<NONEXTENDABLE STRUCT Vertex> b (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void STRUCT_Line_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::VectorOfNonextMessageTypes, a_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::VectorOfNonextMessageTypes, b_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "a", arg_1_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_2_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "b", arg_2_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void STRUCT_Line_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::VectorOfNonextMessageTypes, a_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::VectorOfNonextMessageTypes, b_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "a" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		else if ( key == "b" )
			impl::json::parseJsonParam<ParserT, arg_2_type, false>(arg_2_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

//**********************************************************************
// STRUCT "ObstacleMap" Targets: JSON (1 parameters)
// 1. VECTOR< STRUCT PolygonMap> ObstacleMap (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void STRUCT_ObstacleMap_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::VectorOfMessageType, ObstacleMap_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "ObstacleMap", arg_1_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void STRUCT_ObstacleMap_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::VectorOfMessageType, ObstacleMap_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "ObstacleMap" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

//**********************************************************************
// STRUCT "PolygonMap" Targets: JSON (1 parameters)
// 1. VECTOR<NONEXTENDABLE STRUCT Vertex> PolygonMap (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void STRUCT_PolygonMap_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::VectorOfNonextMessageTypes, PolygonMap_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "PolygonMap", arg_1_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void STRUCT_PolygonMap_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::VectorOfNonextMessageTypes, PolygonMap_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "PolygonMap" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

//**********************************************************************
// STRUCT "Vertex" NONEXTENDABLE Targets: JSON (3 parameters)
// 1. INTEGER x (REQUIRED)
// 2. INTEGER y (REQUIRED)
// 3. INTEGER z (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void STRUCT_Vertex_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, x_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::SignedIntegralType, y_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::SignedIntegralType, z_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ComposerT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "x", arg_1_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_2_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "y", arg_2_type::nameAndTypeID, args...);
composer.buff.append( ",\n  ", 4 );
impl::json::composeParamToJson<ComposerT, arg_3_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "z", arg_3_type::nameAndTypeID, args...);
composer.buff.append( "\n}", 2 );}

template<class ParserT, typename ... Args>
void STRUCT_Vertex_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, x_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::SignedIntegralType, y_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::SignedIntegralType, z_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	static_assert( ParserT::proto == Proto::JSON, "this STRUCT assumes only JSON protocol" );
	p.skipDelimiter( '{' );
	for ( ;; )
	{
		std::string key;
		p.readKey( &key );
		if ( key == "x" )
			impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
		else if ( key == "y" )
			impl::json::parseJsonParam<ParserT, arg_2_type, false>(arg_2_type::nameAndTypeID, p, args...);
		else if ( key == "z" )
			impl::json::parseJsonParam<ParserT, arg_3_type, false>(arg_3_type::nameAndTypeID, p, args...);
		p.skipSpacesEtc();
		if ( p.isDelimiter( ',' ) )
		{
			p.skipDelimiter( ',' );
			continue;
		}
		if ( p.isDelimiter( '}' ) )
		{
			p.skipDelimiter( '}' );
			break;
		}
		throw std::exception(); // bad format
	}
}

//**********************************************************************
// STRUCT "point" NONEXTENDABLE Targets: JSON GMQ (2 parameters)
// 1. INTEGER x (REQUIRED)
// 2. INTEGER y (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void STRUCT_point_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, x_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::SignedIntegralType, y_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	if constexpr( ComposerT::proto == Proto::GMQ )
	{
		impl::gmq::composeParamToGmq<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, arg_1_type::nameAndTypeID, args...);
		impl::gmq::composeParamToGmq<ComposerT, arg_2_type, true, int64_t, int64_t, (int64_t)(0)>(composer, arg_2_type::nameAndTypeID, args...);
	}
	else
	{
		static_assert( ComposerT::proto == Proto::JSON );
		composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
		impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "x", arg_1_type::nameAndTypeID, args...);
		composer.buff.append( ",\n  ", 4 );
		impl::json::composeParamToJson<ComposerT, arg_2_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "y", arg_2_type::nameAndTypeID, args...);
		composer.buff.append( "\n}", 2 );
	}
}

template<class ParserT, typename ... Args>
void STRUCT_point_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, x_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::SignedIntegralType, y_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	if constexpr( ParserT::proto == Proto::GMQ )
	{
		impl::gmq::parseGmqParam<ParserT, arg_1_type, false>(p, arg_1_type::nameAndTypeID, args...);
		impl::gmq::parseGmqParam<ParserT, arg_2_type, false>(p, arg_2_type::nameAndTypeID, args...);
	}
	else
	{
		static_assert( ParserT::proto == Proto::JSON );
		p.skipDelimiter( '{' );
		for ( ;; )
		{
			std::string key;
			p.readKey( &key );
			if ( key == "x" )
				impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
			else if ( key == "y" )
				impl::json::parseJsonParam<ParserT, arg_2_type, false>(arg_2_type::nameAndTypeID, p, args...);
			p.skipSpacesEtc();
			if ( p.isDelimiter( ',' ) )
			{
				p.skipDelimiter( ',' );
				continue;
			}
			if ( p.isDelimiter( '}' ) )
			{
				p.skipDelimiter( '}' );
				break;
			}
			throw std::exception(); // bad format
		}
	}
}

//**********************************************************************
// STRUCT "point3D" Targets: JSON GMQ (3 parameters)
// 1. INTEGER x (REQUIRED)
// 2. INTEGER y (REQUIRED)
// 3. INTEGER z (REQUIRED)

//**********************************************************************

template<class ComposerT, typename ... Args>
void STRUCT_point3D_compose(ComposerT& composer, Args&& ... args)
{
	static_assert( std::is_base_of<ComposerBase, ComposerT>::value, "Composer must be one of GmqComposer<> or JsonComposer<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, x_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::SignedIntegralType, y_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::SignedIntegralType, z_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	if constexpr( ComposerT::proto == Proto::GMQ )
	{
		impl::gmq::composeParamToGmq<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, arg_1_type::nameAndTypeID, args...);
		impl::gmq::composeParamToGmq<ComposerT, arg_2_type, true, int64_t, int64_t, (int64_t)(0)>(composer, arg_2_type::nameAndTypeID, args...);
		impl::gmq::composeParamToGmq<ComposerT, arg_3_type, true, int64_t, int64_t, (int64_t)(0)>(composer, arg_3_type::nameAndTypeID, args...);
	}
	else
	{
		static_assert( ComposerT::proto == Proto::JSON );
		composer.buff.append( "{\n  ", sizeof("{\n  ") - 1 );
		impl::json::composeParamToJson<ComposerT, arg_1_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "x", arg_1_type::nameAndTypeID, args...);
		composer.buff.append( ",\n  ", 4 );
		impl::json::composeParamToJson<ComposerT, arg_2_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "y", arg_2_type::nameAndTypeID, args...);
		composer.buff.append( ",\n  ", 4 );
		impl::json::composeParamToJson<ComposerT, arg_3_type, true, int64_t, int64_t, (int64_t)(0)>(composer, "z", arg_3_type::nameAndTypeID, args...);
		composer.buff.append( "\n}", 2 );
	}
}

template<class ParserT, typename ... Args>
void STRUCT_point3D_parse(ParserT& p, Args&& ... args)
{
	static_assert( std::is_base_of<ParserBase, ParserT>::value, "Parser must be one of GmqParser<> or JsonParser<>" );

	using arg_1_type = NamedParameterWithType<impl::SignedIntegralType, x_Type::Name>;
	using arg_2_type = NamedParameterWithType<impl::SignedIntegralType, y_Type::Name>;
	using arg_3_type = NamedParameterWithType<impl::SignedIntegralType, z_Type::Name>;

	constexpr size_t matchCount = isMatched(arg_1_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_2_type::nameAndTypeID, Args::nameAndTypeID...) + 
		isMatched(arg_3_type::nameAndTypeID, Args::nameAndTypeID...);
	constexpr size_t argCount = sizeof ... (Args);
	if constexpr ( argCount != 0 )
		ensureUniqueness(args.nameAndTypeID...);
	static_assert( argCount == matchCount, "unexpected arguments found" );

	if constexpr( ParserT::proto == Proto::GMQ )
	{
		impl::gmq::parseGmqParam<ParserT, arg_1_type, false>(p, arg_1_type::nameAndTypeID, args...);
		impl::gmq::parseGmqParam<ParserT, arg_2_type, false>(p, arg_2_type::nameAndTypeID, args...);
		impl::gmq::parseGmqParam<ParserT, arg_3_type, false>(p, arg_3_type::nameAndTypeID, args...);
	}
	else
	{
		static_assert( ParserT::proto == Proto::JSON );
		p.skipDelimiter( '{' );
		for ( ;; )
		{
			std::string key;
			p.readKey( &key );
			if ( key == "x" )
				impl::json::parseJsonParam<ParserT, arg_1_type, false>(arg_1_type::nameAndTypeID, p, args...);
			else if ( key == "y" )
				impl::json::parseJsonParam<ParserT, arg_2_type, false>(arg_2_type::nameAndTypeID, p, args...);
			else if ( key == "z" )
				impl::json::parseJsonParam<ParserT, arg_3_type, false>(arg_3_type::nameAndTypeID, p, args...);
			p.skipSpacesEtc();
			if ( p.isDelimiter( ',' ) )
			{
				p.skipDelimiter( ',' );
				continue;
			}
			if ( p.isDelimiter( '}' ) )
			{
				p.skipDelimiter( '}' );
				break;
			}
			throw std::exception(); // bad format
		}
	}
}


} // namespace m

#endif // _test_marshalling_h_guard