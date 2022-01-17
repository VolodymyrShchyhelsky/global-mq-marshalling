//////////////////////////////////////////////////////////////
//
//  Do not edit! file automatically generated by idl_compiler
//
//////////////////////////////////////////////////////////////

using globalmq.marshalling;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace mtest
{

//////////////////////////////////////////////////////////////
//
//                 Messages
//
//////////////////////////////////////////////////////////////

//**********************************************************************
// STRUCT "point3D" Targets: JSON GMQ (3 parameters)
//  1. INTEGER x (REQUIRED)
//  2. INTEGER y (REQUIRED)
//  3. INTEGER z (REQUIRED)
//**********************************************************************

public class point3D : IEquatable<point3D>
{
	public Int64 x;
	public Int64 y;
	public Int64 z;

	public bool Equals(point3D other)
	{
		if (ReferenceEquals(this, other))
			return true;
		else if (ReferenceEquals(null, other))
			return false;
		else
			return
				this.x == other.x &&
				this.y == other.y &&
				this.z == other.z;
	}
	public override bool Equals(object obj)
	{
		return Equals(obj as point3D);
	}
	public static bool operator ==(point3D left, point3D right)
	{
		if (ReferenceEquals(left, right))
			return true;
		else if (ReferenceEquals(left, null))
			return false;
		else if (ReferenceEquals(null, right))
			return false;
		else
			return left.Equals(right);
	}
	public static bool operator !=(point3D left, point3D right)
	{
		return !(left == right);
	}
	public override int GetHashCode()
	{
		// TODO
		throw new InvalidOperationException();
	}
	public static void compose(ComposerBase composer, Int64 x, Int64 y, Int64 z)
	{
		if (composer is GmqComposer gmqC)
			compose(gmqC, x, y, z);
		else if (composer is JsonComposer jsonC)
			compose(jsonC, x, y, z);
		else
			throw new ArgumentException();
	}
	public static void compose(JsonComposer composer, Int64 x, Int64 y, Int64 z)
	{
		composer.append( "{\n  ");
		composer.addNamePart("x");
		composer.composeSignedInteger(x);
		composer.append( ",\n  " );
		composer.addNamePart("y");
		composer.composeSignedInteger(y);
		composer.append( ",\n  " );
		composer.addNamePart("z");
		composer.composeSignedInteger(z);
		composer.append( "\n}" );
	}
	public static void compose(GmqComposer composer, Int64 x, Int64 y, Int64 z)
	{
		composer.composeSignedInteger(x);
		composer.composeSignedInteger(y);
		composer.composeSignedInteger(z);
	}
	public static point3D parse(ParserBase parser)
	{
		point3D tmp = new point3D();
		parse(parser,
			x: ref tmp.x,
			y: ref tmp.y,
			z: ref tmp.z
		);
		return tmp;
	}
	protected static void parse(ParserBase parser, ref Int64 x, ref Int64 y, ref Int64 z)
	{
		if (parser is GmqParser gmqP)
			parse(gmqP, ref x, ref y, ref z);
		else if (parser is JsonParser jsonP)
			parse(jsonP, ref x, ref y, ref z);
		else
			throw new ArgumentException();
	}
	protected static void parse(JsonParser parser, ref Int64 x, ref Int64 y, ref Int64 z)
	{
		parser.skipDelimiter( '{' );
		while (true)
		{
			string key;
			parser.readKeyFromJson( out key );
			if ( key == "x" )
				parser.parseSignedInteger(out x);
			else if ( key == "y" )
				parser.parseSignedInteger(out y);
			else if ( key == "z" )
				parser.parseSignedInteger(out z);

			parser.skipSpacesEtc();
			if ( parser.isDelimiter( ',' ) )
			{
				parser.skipDelimiter( ',' );
				continue;
			}
			if ( parser.isDelimiter( '}' ) )
			{
				parser.skipDelimiter( '}' );
				break;
			}
			throw new FormatException(); // bad format
		}
	}
	protected static void parse(GmqParser parser, ref Int64 x, ref Int64 y, ref Int64 z)
	{
		parser.parseSignedInteger(out x);
		parser.parseSignedInteger(out y);
		parser.parseSignedInteger(out z);
	}
} // class point3D

//**********************************************************************
// STRUCT "point" NONEXTENDABLE Targets: JSON GMQ (2 parameters)
//  1. INTEGER x (REQUIRED)
//  2. INTEGER y (REQUIRED)
//**********************************************************************

public class point : IEquatable<point>
{
	public Int64 x;
	public Int64 y;

	public bool Equals(point other)
	{
		if (ReferenceEquals(this, other))
			return true;
		else if (ReferenceEquals(null, other))
			return false;
		else
			return
				this.x == other.x &&
				this.y == other.y;
	}
	public override bool Equals(object obj)
	{
		return Equals(obj as point);
	}
	public static bool operator ==(point left, point right)
	{
		if (ReferenceEquals(left, right))
			return true;
		else if (ReferenceEquals(left, null))
			return false;
		else if (ReferenceEquals(null, right))
			return false;
		else
			return left.Equals(right);
	}
	public static bool operator !=(point left, point right)
	{
		return !(left == right);
	}
	public override int GetHashCode()
	{
		// TODO
		throw new InvalidOperationException();
	}
	public static void compose(ComposerBase composer, Int64 x, Int64 y)
	{
		if (composer is GmqComposer gmqC)
			compose(gmqC, x, y);
		else if (composer is JsonComposer jsonC)
			compose(jsonC, x, y);
		else
			throw new ArgumentException();
	}
	public static void compose(JsonComposer composer, Int64 x, Int64 y)
	{
		composer.append( "{\n  ");
		composer.addNamePart("x");
		composer.composeSignedInteger(x);
		composer.append( ",\n  " );
		composer.addNamePart("y");
		composer.composeSignedInteger(y);
		composer.append( "\n}" );
	}
	public static void compose(GmqComposer composer, Int64 x, Int64 y)
	{
		composer.composeSignedInteger(x);
		composer.composeSignedInteger(y);
	}
	public static point parse(ParserBase parser)
	{
		point tmp = new point();
		parse(parser,
			x: ref tmp.x,
			y: ref tmp.y
		);
		return tmp;
	}
	protected static void parse(ParserBase parser, ref Int64 x, ref Int64 y)
	{
		if (parser is GmqParser gmqP)
			parse(gmqP, ref x, ref y);
		else if (parser is JsonParser jsonP)
			parse(jsonP, ref x, ref y);
		else
			throw new ArgumentException();
	}
	protected static void parse(JsonParser parser, ref Int64 x, ref Int64 y)
	{
		parser.skipDelimiter( '{' );
		while (true)
		{
			string key;
			parser.readKeyFromJson( out key );
			if ( key == "x" )
				parser.parseSignedInteger(out x);
			else if ( key == "y" )
				parser.parseSignedInteger(out y);

			parser.skipSpacesEtc();
			if ( parser.isDelimiter( ',' ) )
			{
				parser.skipDelimiter( ',' );
				continue;
			}
			if ( parser.isDelimiter( '}' ) )
			{
				parser.skipDelimiter( '}' );
				break;
			}
			throw new FormatException(); // bad format
		}
	}
	protected static void parse(GmqParser parser, ref Int64 x, ref Int64 y)
	{
		parser.parseSignedInteger(out x);
		parser.parseSignedInteger(out y);
	}
} // class point

//**********************************************************************
// STRUCT "message_one" Targets: JSON GMQ (10 parameters)
//  1. INTEGER firstParam (REQUIRED)
//  2. VECTOR<INTEGER> secondParam (REQUIRED)
//  3. VECTOR< STRUCT point3D> thirdParam (REQUIRED)
//  4. UINTEGER forthParam (REQUIRED)
//  5. CHARACTER_STRING fifthParam (REQUIRED)
//  6. VECTOR<NONEXTENDABLE STRUCT point> sixthParam (REQUIRED)
//  7. REAL seventhParam (REQUIRED)
//  8. STRUCT NONEXTENDABLE point eighthParam (REQUIRED)
//  9. STRUCT point3D ninethParam (REQUIRED)
//  10. VECTOR<REAL> tenthParam (REQUIRED)
//**********************************************************************

public class message_one : IEquatable<message_one>
{
	public Int64 firstParam;
	public List<Int64> secondParam;
	public List<point3D> thirdParam;
	public UInt64 forthParam;
	public String fifthParam;
	public List<point> sixthParam;
	public Double seventhParam;
	public point eighthParam;
	public point3D ninethParam;
	public List<Double> tenthParam;

	public bool Equals(message_one other)
	{
		if (ReferenceEquals(this, other))
			return true;
		else if (ReferenceEquals(null, other))
			return false;
		else
			return
				this.firstParam == other.firstParam &&
				Enumerable.SequenceEqual(this.secondParam, other.secondParam) &&
				Enumerable.SequenceEqual(this.thirdParam, other.thirdParam) &&
				this.forthParam == other.forthParam &&
				this.fifthParam == other.fifthParam &&
				Enumerable.SequenceEqual(this.sixthParam, other.sixthParam) &&
				this.seventhParam == other.seventhParam &&
				this.eighthParam.Equals(other.eighthParam) &&
				this.ninethParam.Equals(other.ninethParam) &&
				Enumerable.SequenceEqual(this.tenthParam, other.tenthParam);
	}
	public override bool Equals(object obj)
	{
		return Equals(obj as message_one);
	}
	public static bool operator ==(message_one left, message_one right)
	{
		if (ReferenceEquals(left, right))
			return true;
		else if (ReferenceEquals(left, null))
			return false;
		else if (ReferenceEquals(null, right))
			return false;
		else
			return left.Equals(right);
	}
	public static bool operator !=(message_one left, message_one right)
	{
		return !(left == right);
	}
	public override int GetHashCode()
	{
		// TODO
		throw new InvalidOperationException();
	}
	public static void compose(ComposerBase composer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		if (composer is GmqComposer gmqC)
			compose(gmqC, firstParam, secondParam, thirdParam, forthParam, fifthParam, sixthParam, seventhParam, eighthParam, ninethParam, tenthParam);
		else if (composer is JsonComposer jsonC)
			compose(jsonC, firstParam, secondParam, thirdParam, forthParam, fifthParam, sixthParam, seventhParam, eighthParam, ninethParam, tenthParam);
		else
			throw new ArgumentException();
	}
	public static void compose(JsonComposer composer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		composer.append( "{\n  ");
		composer.addNamePart("firstParam");
		composer.composeSignedInteger(firstParam);
		composer.append( ",\n  " );
		composer.addNamePart("secondParam");
		secondParam.composeJson(composer);
		composer.append( ",\n  " );
		composer.addNamePart("thirdParam");
		thirdParam.composeJson(composer);
		composer.append( ",\n  " );
		composer.addNamePart("forthParam");
		composer.composeUnsignedInteger(forthParam);
		composer.append( ",\n  " );
		composer.addNamePart("fifthParam");
		composer.composeString(fifthParam);
		composer.append( ",\n  " );
		composer.addNamePart("sixthParam");
		sixthParam.composeJson(composer);
		composer.append( ",\n  " );
		composer.addNamePart("seventhParam");
		composer.composeReal(seventhParam);
		composer.append( ",\n  " );
		composer.addNamePart("eighthParam");
		eighthParam.compose(composer);
		composer.append( ",\n  " );
		composer.addNamePart("ninethParam");
		ninethParam.compose(composer);
		composer.append( ",\n  " );
		composer.addNamePart("tenthParam");
		tenthParam.composeJson(composer);
		composer.append( "\n}" );
	}
	public static void compose(GmqComposer composer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		composer.composeSignedInteger(firstParam);
		secondParam.composeGmq(composer);
		thirdParam.composeGmq(composer);
		composer.composeUnsignedInteger(forthParam);
		composer.composeString(fifthParam);
		sixthParam.composeGmq(composer);
		composer.composeReal(seventhParam);
		eighthParam.compose(composer);
		ninethParam.compose(composer);
		tenthParam.composeGmq(composer);
	}
	public static message_one parse(ParserBase parser)
	{
		message_one tmp = new message_one();
		parse(parser,
			firstParam: ref tmp.firstParam,
			secondParam: new CollectionWrapperForParsing(
				() => { tmp.secondParam = new List<Int64>(); },
				(ParserBase parser, int ordinal) => { Int64 val; parser.parseSignedInteger(out val); tmp.secondParam.Add(val); }),
			thirdParam: new CollectionWrapperForParsing(
				() => { tmp.thirdParam = new List<point3D>(); },
				(ParserBase parser, int ordinal) => { point3D val = point3D.parse(parser); tmp.thirdParam.Add(val); }),
			forthParam: ref tmp.forthParam,
			fifthParam: ref tmp.fifthParam,
			sixthParam: new CollectionWrapperForParsing(
				() => { tmp.sixthParam = new List<point>(); },
				(ParserBase parser, int ordinal) => { point val = point.parse(parser); tmp.sixthParam.Add(val); }),
			seventhParam: ref tmp.seventhParam,
			eighthParam: new MessageWrapperForParsing(
				(ParserBase parser) => { tmp.eighthParam = point.parse(parser); }),
			ninethParam: new MessageWrapperForParsing(
				(ParserBase parser) => { tmp.ninethParam = point3D.parse(parser); }),
			tenthParam: new CollectionWrapperForParsing(
				() => { tmp.tenthParam = new List<Double>(); },
				(ParserBase parser, int ordinal) => { Double val; parser.parseReal(out val); tmp.tenthParam.Add(val); })
		);
		return tmp;
	}
	protected static void parse(ParserBase parser, ref Int64 firstParam, ICollectionParse secondParam, ICollectionParse thirdParam, ref UInt64 forthParam, ref String fifthParam, ICollectionParse sixthParam, ref Double seventhParam, IMessageParse eighthParam, IMessageParse ninethParam, ICollectionParse tenthParam)
	{
		if (parser is GmqParser gmqP)
			parse(gmqP, ref firstParam, secondParam, thirdParam, ref forthParam, ref fifthParam, sixthParam, ref seventhParam, eighthParam, ninethParam, tenthParam);
		else if (parser is JsonParser jsonP)
			parse(jsonP, ref firstParam, secondParam, thirdParam, ref forthParam, ref fifthParam, sixthParam, ref seventhParam, eighthParam, ninethParam, tenthParam);
		else
			throw new ArgumentException();
	}
	protected static void parse(JsonParser parser, ref Int64 firstParam, ICollectionParse secondParam, ICollectionParse thirdParam, ref UInt64 forthParam, ref String fifthParam, ICollectionParse sixthParam, ref Double seventhParam, IMessageParse eighthParam, IMessageParse ninethParam, ICollectionParse tenthParam)
	{
		parser.skipDelimiter( '{' );
		while (true)
		{
			string key;
			parser.readKeyFromJson( out key );
			if ( key == "firstParam" )
				parser.parseSignedInteger(out firstParam);
			else if ( key == "secondParam" )
				secondParam.parseJson(parser);
			else if ( key == "thirdParam" )
				thirdParam.parseJson(parser);
			else if ( key == "forthParam" )
				parser.parseUnsignedInteger(out forthParam);
			else if ( key == "fifthParam" )
				parser.parseString(out fifthParam);
			else if ( key == "sixthParam" )
				sixthParam.parseJson(parser);
			else if ( key == "seventhParam" )
				parser.parseReal(out seventhParam);
			else if ( key == "eighthParam" )
				eighthParam.parse(parser);
			else if ( key == "ninethParam" )
				ninethParam.parse(parser);
			else if ( key == "tenthParam" )
				tenthParam.parseJson(parser);

			parser.skipSpacesEtc();
			if ( parser.isDelimiter( ',' ) )
			{
				parser.skipDelimiter( ',' );
				continue;
			}
			if ( parser.isDelimiter( '}' ) )
			{
				parser.skipDelimiter( '}' );
				break;
			}
			throw new FormatException(); // bad format
		}
	}
	protected static void parse(GmqParser parser, ref Int64 firstParam, ICollectionParse secondParam, ICollectionParse thirdParam, ref UInt64 forthParam, ref String fifthParam, ICollectionParse sixthParam, ref Double seventhParam, IMessageParse eighthParam, IMessageParse ninethParam, ICollectionParse tenthParam)
	{
		parser.parseSignedInteger(out firstParam);
		secondParam.parseGmq(parser);
		thirdParam.parseGmq(parser);
		parser.parseUnsignedInteger(out forthParam);
		parser.parseString(out fifthParam);
		sixthParam.parseGmq(parser);
		parser.parseReal(out seventhParam);
		eighthParam.parse(parser);
		ninethParam.parse(parser);
		tenthParam.parseGmq(parser);
	}
} // class message_one

//////////////////////////////////////////////////////////////
//
//  Scopes:
//
//  test_gmq
//  {
//    point3D_alias
//    message_one_gmq
//  }
//
//  test_json
//  {
//    point_alias
//    message_one_json
//  }
//
//////////////////////////////////////////////////////////////

public class test_gmq
{
	public enum MsgId { point3D_alias = 1, message_one_gmq = 1 }

	public static MessageHandler makeMessageHandler( MsgId id, MessageHandler.HandlerDelegate handler )
	{
		return new MessageHandler((ulong)id, handler);
	}
	public static MessageHandler makeDefaultMessageHandler( MessageHandler.HandlerDelegate handler)
	{
		return new MessageHandler(MessageHandler.DefaultHandler, handler);
	}
	public static void handleMessage( BufferT buffer, params MessageHandler[] handlers )
	{
		handleMessage(buffer.getReadIterator(), handlers);
	}
	public static void handleMessage( ReadIteratorT riter, params MessageHandler[] handlers )
	{
		GmqParser parser = new GmqParser( riter );
		MessageHandler.gmq_handle( parser, handlers );
	}

//**********************************************************************
// MESSAGE "point3D_alias" Targets: GMQ (0 parameters)
//**********************************************************************

public class point3D_alias : point3D
{
	public new static point3D_alias parse(ParserBase parser)
	{
		point3D_alias tmp = new point3D_alias();
		parse(parser,
			x: ref tmp.x,
			y: ref tmp.y,
			z: ref tmp.z
		);
		return tmp;
	}
	public new static void compose(ComposerBase composer, Int64 x, Int64 y, Int64 z)
	{
		point3D.compose(composer, x, y, z);
	}
	protected new static void parse(ParserBase parser, ref Int64 x, ref Int64 y, ref Int64 z)
	{
		point3D.parse(parser, ref x, ref y, ref z);
	}
	public new static void compose(GmqComposer composer, Int64 x, Int64 y, Int64 z)
	{
		point3D.compose(composer, x, y, z);
	}
	protected new static void parse(GmqParser parser, ref Int64 x, ref Int64 y, ref Int64 z)
	{
		point3D.parse(parser, ref x, ref y, ref z);
	}
	public new static void compose(JsonComposer composer, Int64 x, Int64 y, Int64 z)
	{
		point3D.compose(composer, x, y, z);
	}
	protected new static void parse(JsonParser parser, ref Int64 x, ref Int64 y, ref Int64 z)
	{
		point3D.parse(parser, ref x, ref y, ref z);
	}
} // class point3D_alias

	public static void composeMessage_point3D_alias(BufferT buffer, Int64 x, Int64 y, Int64 z)
	{
		GmqComposer composer = new GmqComposer(buffer);

		composer.composeUnsignedInteger((UInt64)MsgId.point3D_alias);
		point3D_alias.compose(composer, x, y, z);
	}

//**********************************************************************
// MESSAGE "message_one_gmq" Targets: GMQ (0 parameters)
//**********************************************************************

public class message_one_gmq : message_one
{
	public new static message_one_gmq parse(ParserBase parser)
	{
		message_one_gmq tmp = new message_one_gmq();
		parse(parser,
			firstParam: ref tmp.firstParam,
			secondParam: new CollectionWrapperForParsing(
				() => { tmp.secondParam = new List<Int64>(); },
				(ParserBase parser, int ordinal) => { Int64 val; parser.parseSignedInteger(out val); tmp.secondParam.Add(val); }),
			thirdParam: new CollectionWrapperForParsing(
				() => { tmp.thirdParam = new List<point3D>(); },
				(ParserBase parser, int ordinal) => { point3D val = point3D.parse(parser); tmp.thirdParam.Add(val); }),
			forthParam: ref tmp.forthParam,
			fifthParam: ref tmp.fifthParam,
			sixthParam: new CollectionWrapperForParsing(
				() => { tmp.sixthParam = new List<point>(); },
				(ParserBase parser, int ordinal) => { point val = point.parse(parser); tmp.sixthParam.Add(val); }),
			seventhParam: ref tmp.seventhParam,
			eighthParam: new MessageWrapperForParsing(
				(ParserBase parser) => { tmp.eighthParam = point.parse(parser); }),
			ninethParam: new MessageWrapperForParsing(
				(ParserBase parser) => { tmp.ninethParam = point3D.parse(parser); }),
			tenthParam: new CollectionWrapperForParsing(
				() => { tmp.tenthParam = new List<Double>(); },
				(ParserBase parser, int ordinal) => { Double val; parser.parseReal(out val); tmp.tenthParam.Add(val); })
		);
		return tmp;
	}
	public new static void compose(ComposerBase composer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		message_one.compose(composer, firstParam, secondParam, thirdParam, forthParam, fifthParam, sixthParam, seventhParam, eighthParam, ninethParam, tenthParam);
	}
	protected new static void parse(ParserBase parser, ref Int64 firstParam, ICollectionParse secondParam, ICollectionParse thirdParam, ref UInt64 forthParam, ref String fifthParam, ICollectionParse sixthParam, ref Double seventhParam, IMessageParse eighthParam, IMessageParse ninethParam, ICollectionParse tenthParam)
	{
		message_one.parse(parser, ref firstParam, secondParam, thirdParam, ref forthParam, ref fifthParam, sixthParam, ref seventhParam, eighthParam, ninethParam, tenthParam);
	}
	public new static void compose(GmqComposer composer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		message_one.compose(composer, firstParam, secondParam, thirdParam, forthParam, fifthParam, sixthParam, seventhParam, eighthParam, ninethParam, tenthParam);
	}
	protected new static void parse(GmqParser parser, ref Int64 firstParam, ICollectionParse secondParam, ICollectionParse thirdParam, ref UInt64 forthParam, ref String fifthParam, ICollectionParse sixthParam, ref Double seventhParam, IMessageParse eighthParam, IMessageParse ninethParam, ICollectionParse tenthParam)
	{
		message_one.parse(parser, ref firstParam, secondParam, thirdParam, ref forthParam, ref fifthParam, sixthParam, ref seventhParam, eighthParam, ninethParam, tenthParam);
	}
	public new static void compose(JsonComposer composer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		message_one.compose(composer, firstParam, secondParam, thirdParam, forthParam, fifthParam, sixthParam, seventhParam, eighthParam, ninethParam, tenthParam);
	}
	protected new static void parse(JsonParser parser, ref Int64 firstParam, ICollectionParse secondParam, ICollectionParse thirdParam, ref UInt64 forthParam, ref String fifthParam, ICollectionParse sixthParam, ref Double seventhParam, IMessageParse eighthParam, IMessageParse ninethParam, ICollectionParse tenthParam)
	{
		message_one.parse(parser, ref firstParam, secondParam, thirdParam, ref forthParam, ref fifthParam, sixthParam, ref seventhParam, eighthParam, ninethParam, tenthParam);
	}
} // class message_one_gmq

	public static void composeMessage_message_one_gmq(BufferT buffer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		GmqComposer composer = new GmqComposer(buffer);

		composer.composeUnsignedInteger((UInt64)MsgId.message_one_gmq);
		message_one_gmq.compose(composer, firstParam, secondParam, thirdParam, forthParam, fifthParam, sixthParam, seventhParam, eighthParam, ninethParam, tenthParam);
	}

} // class test_gmq

public class test_json
{
	public enum MsgId { point_alias = 2, message_one_json = 2 }

	public static MessageHandler makeMessageHandler( MsgId id, MessageHandler.HandlerDelegate handler )
	{
		return new MessageHandler((ulong)id, handler);
	}
	public static MessageHandler makeDefaultMessageHandler( MessageHandler.HandlerDelegate handler)
	{
		return new MessageHandler(MessageHandler.DefaultHandler, handler);
	}
	public static void handleMessage( BufferT buffer, params MessageHandler[] handlers )
	{
		handleMessage(buffer.getReadIterator(), handlers);
	}
	public static void handleMessage( ReadIteratorT riter, params MessageHandler[] handlers )
	{
		JsonParser parser = new JsonParser( riter );
		MessageHandler.json_handle( parser, handlers );
	}

//**********************************************************************
// MESSAGE "point_alias" Targets: JSON (0 parameters)
//**********************************************************************

public class point_alias : point
{
	public new static point_alias parse(ParserBase parser)
	{
		point_alias tmp = new point_alias();
		parse(parser,
			x: ref tmp.x,
			y: ref tmp.y
		);
		return tmp;
	}
	public new static void compose(ComposerBase composer, Int64 x, Int64 y)
	{
		point.compose(composer, x, y);
	}
	protected new static void parse(ParserBase parser, ref Int64 x, ref Int64 y)
	{
		point.parse(parser, ref x, ref y);
	}
	public new static void compose(GmqComposer composer, Int64 x, Int64 y)
	{
		point.compose(composer, x, y);
	}
	protected new static void parse(GmqParser parser, ref Int64 x, ref Int64 y)
	{
		point.parse(parser, ref x, ref y);
	}
	public new static void compose(JsonComposer composer, Int64 x, Int64 y)
	{
		point.compose(composer, x, y);
	}
	protected new static void parse(JsonParser parser, ref Int64 x, ref Int64 y)
	{
		point.parse(parser, ref x, ref y);
	}
} // class point_alias

	public static void composeMessage_point_alias(BufferT buffer, Int64 x, Int64 y)
	{
		JsonComposer composer = new JsonComposer(buffer);

		composer.append("{\n  ");
		composer.addNamePart("msgid");
		composer.composeUnsignedInteger((UInt64)MsgId.point_alias);
		composer.append(",\n  ");
		composer.addNamePart("msgbody");
		point_alias.compose(composer, x, y);
		composer.append("\n}");
	}

//**********************************************************************
// MESSAGE "message_one_json" Targets: JSON (0 parameters)
//**********************************************************************

public class message_one_json : message_one
{
	public new static message_one_json parse(ParserBase parser)
	{
		message_one_json tmp = new message_one_json();
		parse(parser,
			firstParam: ref tmp.firstParam,
			secondParam: new CollectionWrapperForParsing(
				() => { tmp.secondParam = new List<Int64>(); },
				(ParserBase parser, int ordinal) => { Int64 val; parser.parseSignedInteger(out val); tmp.secondParam.Add(val); }),
			thirdParam: new CollectionWrapperForParsing(
				() => { tmp.thirdParam = new List<point3D>(); },
				(ParserBase parser, int ordinal) => { point3D val = point3D.parse(parser); tmp.thirdParam.Add(val); }),
			forthParam: ref tmp.forthParam,
			fifthParam: ref tmp.fifthParam,
			sixthParam: new CollectionWrapperForParsing(
				() => { tmp.sixthParam = new List<point>(); },
				(ParserBase parser, int ordinal) => { point val = point.parse(parser); tmp.sixthParam.Add(val); }),
			seventhParam: ref tmp.seventhParam,
			eighthParam: new MessageWrapperForParsing(
				(ParserBase parser) => { tmp.eighthParam = point.parse(parser); }),
			ninethParam: new MessageWrapperForParsing(
				(ParserBase parser) => { tmp.ninethParam = point3D.parse(parser); }),
			tenthParam: new CollectionWrapperForParsing(
				() => { tmp.tenthParam = new List<Double>(); },
				(ParserBase parser, int ordinal) => { Double val; parser.parseReal(out val); tmp.tenthParam.Add(val); })
		);
		return tmp;
	}
	public new static void compose(ComposerBase composer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		message_one.compose(composer, firstParam, secondParam, thirdParam, forthParam, fifthParam, sixthParam, seventhParam, eighthParam, ninethParam, tenthParam);
	}
	protected new static void parse(ParserBase parser, ref Int64 firstParam, ICollectionParse secondParam, ICollectionParse thirdParam, ref UInt64 forthParam, ref String fifthParam, ICollectionParse sixthParam, ref Double seventhParam, IMessageParse eighthParam, IMessageParse ninethParam, ICollectionParse tenthParam)
	{
		message_one.parse(parser, ref firstParam, secondParam, thirdParam, ref forthParam, ref fifthParam, sixthParam, ref seventhParam, eighthParam, ninethParam, tenthParam);
	}
	public new static void compose(GmqComposer composer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		message_one.compose(composer, firstParam, secondParam, thirdParam, forthParam, fifthParam, sixthParam, seventhParam, eighthParam, ninethParam, tenthParam);
	}
	protected new static void parse(GmqParser parser, ref Int64 firstParam, ICollectionParse secondParam, ICollectionParse thirdParam, ref UInt64 forthParam, ref String fifthParam, ICollectionParse sixthParam, ref Double seventhParam, IMessageParse eighthParam, IMessageParse ninethParam, ICollectionParse tenthParam)
	{
		message_one.parse(parser, ref firstParam, secondParam, thirdParam, ref forthParam, ref fifthParam, sixthParam, ref seventhParam, eighthParam, ninethParam, tenthParam);
	}
	public new static void compose(JsonComposer composer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		message_one.compose(composer, firstParam, secondParam, thirdParam, forthParam, fifthParam, sixthParam, seventhParam, eighthParam, ninethParam, tenthParam);
	}
	protected new static void parse(JsonParser parser, ref Int64 firstParam, ICollectionParse secondParam, ICollectionParse thirdParam, ref UInt64 forthParam, ref String fifthParam, ICollectionParse sixthParam, ref Double seventhParam, IMessageParse eighthParam, IMessageParse ninethParam, ICollectionParse tenthParam)
	{
		message_one.parse(parser, ref firstParam, secondParam, thirdParam, ref forthParam, ref fifthParam, sixthParam, ref seventhParam, eighthParam, ninethParam, tenthParam);
	}
} // class message_one_json

	public static void composeMessage_message_one_json(BufferT buffer, Int64 firstParam, ICollectionCompose secondParam, ICollectionCompose thirdParam, UInt64 forthParam, String fifthParam, ICollectionCompose sixthParam, Double seventhParam, IMessageCompose eighthParam, IMessageCompose ninethParam, ICollectionCompose tenthParam)
	{
		JsonComposer composer = new JsonComposer(buffer);

		composer.append("{\n  ");
		composer.addNamePart("msgid");
		composer.composeUnsignedInteger((UInt64)MsgId.message_one_json);
		composer.append(",\n  ");
		composer.addNamePart("msgbody");
		message_one_json.compose(composer, firstParam, secondParam, thirdParam, forthParam, fifthParam, sixthParam, seventhParam, eighthParam, ninethParam, tenthParam);
		composer.append("\n}");
	}

} // class test_json


} // namespace mtest

