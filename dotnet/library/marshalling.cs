﻿/* -------------------------------------------------------------------------------
* Copyright (c) 2021, OLogN Technologies AG
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

using globalmq.marshalling.impl;
using System;
using System.Collections.Generic;

namespace globalmq.marshalling
{
    public interface ICompose
    {
    };

    public interface IParse
    {
    };

    interface AnyCollectionWrapperBase
    {
        public static readonly int unknown_size = -1;
    };

    interface CollectionWrapperBase : AnyCollectionWrapperBase { }

    interface SimpleTypeCollectionWrapperBase : AnyCollectionWrapperBase { }

    public class CollectionWrapperForComposing : CollectionWrapperBase, ICompose
    {
        SizeDelegate lsize_;
        NextDelegate lnext_;

        public delegate int SizeDelegate();
        public delegate void NextDelegate(ComposerBase composer, int ordinal);
        public CollectionWrapperForComposing(SizeDelegate lsize, NextDelegate lnext)
        {
            lsize_ = lsize;
            lnext_ = lnext;
        }
        public int size() { return lsize_(); }
        public void compose_next(ComposerBase composer, int ordinal)
        {
            lnext_(composer, ordinal);
        }
    };

    public class CollectionWrapperForParsing : CollectionWrapperBase, IParse
    {
        SizeDelegate lsize_;
        NextDelegate lnext_;
        public delegate void SizeDelegate(int size);
        public delegate void NextDelegate(ParserBase parser, int ordinal);

        public CollectionWrapperForParsing(SizeDelegate lsize, NextDelegate lnext)

        {
            lsize_ = lsize;
            lnext_ = lnext;
        }
        public void size_hint(int sz)
        {
            if (lsize_ != null)
                lsize_(sz);
        }
        public void parse_next(ParserBase parser, int ordinal)
        {
            lnext_(parser, ordinal);
        }
    };

    public class SimpleTypeCollection
    {
        public static ICompose makeComposer(IList<sbyte> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeSignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(IList<short> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeSignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(IList<int> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeSignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(IList<long> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeSignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(sbyte[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeSignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(short[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeSignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(int[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeSignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(long[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeSignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(IList<byte> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeUnsignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(IList<ushort> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeUnsignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(IList<uint> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeUnsignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(IList<ulong> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeUnsignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(byte[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeUnsignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(ushort[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeUnsignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(uint[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeUnsignedInteger(coll[ordinal]); });
        }
        public static ICompose makeComposer(ulong[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeUnsignedInteger(coll[ordinal]); });
        }

        public static ICompose makeComposer(IList<float> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeReal(coll[ordinal]); });
        }
        public static ICompose makeComposer(IList<double> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeReal(coll[ordinal]); });
        }
        public static ICompose makeComposer(float[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeReal(coll[ordinal]); });
        }
        public static ICompose makeComposer(double[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeReal(coll[ordinal]); });
        }
        public static ICompose makeComposer(IList<string> coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Count; }, (composer, ordinal) => { composer.composeString(coll[ordinal]); });
        }
        public static ICompose makeComposer(string[] coll)
        {
            return new CollectionWrapperForComposing(() => { return coll.Length; }, (composer, ordinal) => { composer.composeString(coll[ordinal]); });
        }




        public static IParse makeParser(IList<sbyte> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { sbyte value; parser.parseSignedInteger(out value); coll.Add(value); });
        }
        public static IParse makeParser(IList<short> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { short value; parser.parseSignedInteger(out value); coll.Add(value); });
        }
        public static IParse makeParser(IList<int> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { int value; parser.parseSignedInteger(out value); coll.Add(value); });
        }
        public static IParse makeParser(IList<long> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { long value; parser.parseSignedInteger(out value); coll.Add(value); });
        }
        public static IParse makeParser(IList<byte> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { byte value; parser.parseUnsignedInteger(out value); coll.Add(value); });
        }
        public static IParse makeParser(IList<ushort> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { ushort value; parser.parseUnsignedInteger(out value); coll.Add(value); });
        }
        public static IParse makeParser(IList<uint> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { uint value; parser.parseUnsignedInteger(out value); coll.Add(value); });
        }
        public static IParse makeParser(IList<ulong> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { ulong value; parser.parseUnsignedInteger(out value); coll.Add(value); });
        }
        public static IParse makeParser(IList<float> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { float value; parser.parseReal(out value); coll.Add(value); });
        }
        public static IParse makeParser(IList<double> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { double value; parser.parseReal(out value); coll.Add(value); });
        }
        public static IParse makeParser(IList<string> coll)
        {
            return new CollectionWrapperForParsing(null, (parser, ordinal) => { string value; parser.parseString(out value); coll.Add(value); });
        }
    };

    interface MessageWrapperBase
    {
    };


    public class MessageWrapperForComposing : MessageWrapperBase, ICompose
    {
        ComposeDelegate lcompose_;

        public delegate void ComposeDelegate(ComposerBase composer);

        public MessageWrapperForComposing(ComposeDelegate lcompose) { lcompose_ = lcompose; }

        public void compose(ComposerBase composer)
        {
            lcompose_(composer);
        }
    };

    public class MessageWrapperForParsing : MessageWrapperBase, IParse
    {
        ParseDelegate lparse_;

        public delegate void ParseDelegate(ParserBase composer);


        public MessageWrapperForParsing(ParseDelegate lparse) { lparse_ = lparse; }
        public void parse(ParserBase parser)
        {
            lparse_(parser);
        }
    };

    public class Base
    {
        public static ulong[] makeAddress(ulong[] baseAddress, ulong last)
        {
            ulong[] address = new ulong[baseAddress.Length + 1];
            baseAddress.CopyTo(address, 0);
            address[baseAddress.Length] = last;
            return address;
        }
        public static void composeAddressInPublishable(ComposerBase composer, ulong[] addr, ulong last)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.composeAddressInPublishable(gmqComposer, addr, last);
            else if (composer is JsonComposer jsonComposer)
                json.composeAddressInPublishable(jsonComposer, addr, last);
            else
                throw new Exception(); //TODO
        }

        public static void composeStateUpdateMessageBegin(ComposerBase composer)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.composeStateUpdateMessageBegin(gmqComposer);
            else if (composer is JsonComposer jsonComposer)
                json.composeStateUpdateMessageBegin(jsonComposer);
            else
                throw new Exception(); //TODO
        }
        public static void composeStateUpdateMessageEnd(ComposerBase composer)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.composeStateUpdateMessageEnd(gmqComposer);
            else if (composer is JsonComposer jsonComposer)
                json.composeStateUpdateMessageEnd(jsonComposer);
            else
                throw new Exception(); //TODO
        }

        public static void publishableComposeLeafeInteger(ComposerBase composer, long value)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.publishableComposeLeafeInteger(gmqComposer, value);
            else if (composer is JsonComposer jsonComposer)
                json.publishableComposeLeafeInteger(jsonComposer, value);
            else
                throw new Exception(); //TODO
        }
        public static void publishableComposeLeafeReal(ComposerBase composer, double value)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.publishableComposeLeafeReal(gmqComposer, value);
            else if (composer is JsonComposer jsonComposer)
                json.publishableComposeLeafeReal(jsonComposer, value);
            else
                throw new Exception(); //TODO
        }
        public static void publishableComposeLeafeString(ComposerBase composer, string value)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.publishableComposeLeafeString(gmqComposer, value);
            else if (composer is JsonComposer jsonComposer)
                json.publishableComposeLeafeString(jsonComposer, value);
            else
                throw new Exception(); //TODO
        }
        public static void publishableComposeLeafeStructBegin(ComposerBase composer)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.publishableComposeLeafeStructBegin(gmqComposer);
            else if (composer is JsonComposer jsonComposer)
                json.publishableComposeLeafeStructBegin(jsonComposer);
            else
                throw new Exception(); //TODO
        }
        public static void publishableComposeLeafeStructEnd(ComposerBase composer)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.publishableComposeLeafeStructEnd(gmqComposer);
            else if (composer is JsonComposer jsonComposer)
                json.publishableComposeLeafeStructEnd(jsonComposer);
            else
                throw new Exception(); //TODO
        }

        public static void publishableStructComposeInteger(ComposerBase composer, string name, long value, bool separator)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.publishableStructComposeInteger(gmqComposer, name, value, separator);
            else if (composer is JsonComposer jsonComposer)
                json.publishableStructComposeInteger(jsonComposer, name, value, separator);
            else
                throw new Exception(); //TODO
        }
        public static void publishableStructComposeUnsigned(ComposerBase composer, string name, ulong value, bool separator)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.publishableStructComposeUnsigned(gmqComposer, name, value, separator);
            else if (composer is JsonComposer jsonComposer)
                json.publishableStructComposeUnsigned(jsonComposer, name, value, separator);
            else
                throw new Exception(); //TODO
        }
        public static void publishableStructComposeReal(ComposerBase composer, string name, double value, bool separator)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.publishableStructComposeReal(gmqComposer, name, value, separator);
            else if (composer is JsonComposer jsonComposer)
                json.publishableStructComposeReal(jsonComposer, name, value, separator);
            else
                throw new Exception(); //TODO
        }
        public static void publishableStructComposeString(ComposerBase composer, string name, string value, bool separator)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.publishableStructComposeString(gmqComposer, name, value, separator);
            else if (composer is JsonComposer jsonComposer)
                json.publishableStructComposeString(jsonComposer, name, value, separator);
            else
                throw new Exception(); //TODO
        }
        public static void composeStructBegin(ComposerBase composer)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.composeStructBegin(gmqComposer);
            else if (composer is JsonComposer jsonComposer)
                json.composeStructBegin(jsonComposer);
            else
                throw new Exception(); //TODO
        }
        public static void composeStructEnd(ComposerBase composer)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.composeStructEnd(gmqComposer);
            else if (composer is JsonComposer jsonComposer)
                json.composeStructEnd(jsonComposer);
            else
                throw new Exception(); //TODO
        }

        public static void composePublishableStructBegin(ComposerBase composer, string name)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.composePublishableStructBegin(gmqComposer, name);
            else if (composer is JsonComposer jsonComposer)
                json.composePublishableStructBegin(jsonComposer, name);
            else
                throw new Exception(); //TODO
        }
        public static void composePublishableStructEnd(ComposerBase composer, bool separator)
        {
            if (composer is GmqComposer gmqComposer)
                gmq.composePublishableStructEnd(gmqComposer, separator);
            else if (composer is JsonComposer jsonComposer)
                json.composePublishableStructEnd(jsonComposer, separator);
            else
                throw new Exception(); //TODO
        }

    }



} // namespace globalmq::marshalling

namespace globalmq.marshalling.impl
{
    public class gmq
    {
        public static void parseGmqParam(GmqParser parser, IParse o)
        {
            if (o is CollectionWrapperForParsing coll)
            {
                int sz;
                parser.parseUnsignedInteger(out sz);
                coll.size_hint(sz);
                for (int i = 0; i < sz; ++i)
                    coll.parse_next(parser, i);

            }
            else if (o is MessageWrapperForParsing msg)
                msg.parse(parser);
            else
                throw new Exception(); //TODO
        }


        ///////////////////////////////////////////

        public static void composeParamToGmq(GmqComposer composer, ICompose o)
        {

            if (o is CollectionWrapperForComposing coll)
            {
                int collSz = coll.size();
                composer.composeUnsignedInteger((uint)collSz);
                for (int i = 0; i < collSz; ++i)
                    coll.compose_next(composer, i);
            }
            else if (o is MessageWrapperForComposing msg)
                msg.compose(composer);
            else
                throw new Exception(); //TODO
        }

        public static void handleMessage(GmqParser parser, MessageHandlerArray handlers)
        {
            ulong msgID;
            parser.parseUnsignedInteger(out msgID);
            handlers.handle(parser, msgID);
        }

        public static void composeStateUpdateMessageBegin(GmqComposer composer)
        {
            /* nothing */
        }
        public static void composeStateUpdateMessageEnd(GmqComposer composer)
        {
            /* nothing */
        }

        public static void composeAddressInPublishable(GmqComposer composer, ulong[] addr, ulong last)
        {
            int collSz = addr.Length;
            composer.composeUnsignedInteger((ulong)collSz + 1);
            for (int i = 0; i < collSz; ++i)
                composer.composeUnsignedInteger(addr[i]);
            composer.composeUnsignedInteger(last);
        }
        public static void publishableComposeLeafeInteger(GmqComposer composer, long value)
        {
            composer.composeSignedInteger(value);
        }
        public static void publishableComposeLeafeReal(GmqComposer composer, double value)
        {
            composer.composeReal(value);
        }

        public static void publishableComposeLeafeString(GmqComposer composer, string value)
        {
            composer.composeString(value);
        }
        public static void publishableComposeLeafeStructBegin(GmqComposer composer)
        {
            /* nothing */
        }
        public static void publishableComposeLeafeStructEnd(GmqComposer composer)
        {
            /* nothing */
        }
        public static void publishableStructComposeInteger(GmqComposer composer, string name, long value, bool separator)
        {
            composer.composeSignedInteger(value);
        }
        public static void publishableStructComposeUnsigned(GmqComposer composer, string name, ulong value, bool separator)
        {
            composer.composeUnsignedInteger(value);
        }
        public static void publishableStructComposeReal(GmqComposer composer, string name, double value, bool separator)
        {
            composer.composeReal(value);
        }
        public static void publishableStructComposeString(GmqComposer composer, string name, string value, bool separator)
        {
            composer.composeString(value);
        }
        public static void composePublishableStructBegin(GmqComposer composer, string name)
        {
            /* nothing */
        }
        public static void composePublishableStructEnd(GmqComposer composer, bool separator)
        {
            /* nothing */
        }
        public static void composeStructBegin(GmqComposer composer)
        {
            /* nothing */
        }
        public static void composeStructEnd(GmqComposer composer)
        {
            /* nothing */
        }


    }
    // class gmq
}

namespace globalmq.marshalling.impl
{
    public class json
    {
        public static void parseJson(JsonParser parser, CollectionWrapperForParsing coll)
        {
            bool newFormat = false;
            if (parser.isDelimiter('{'))
            {
                newFormat = true;
                parser.skipDelimiter('{');

                string key;
                parser.readKeyFromJson(out key);
                if (key != "size")
                    throw new Exception();

                int sz;
                parser.parseUnsignedInteger(out sz);
                coll.size_hint(sz);

                parser.skipSpacesEtc();
                parser.skipDelimiter(',');

                parser.readKeyFromJson(out key);
                if (key != "data")
                    throw new Exception();
            }

            parser.skipDelimiter('[');

            if (!parser.isDelimiter(']')) // there are some items there
            {
                for (int i = 0; ; ++i)
                {
                    coll.parse_next(parser, i);
                    if (parser.isDelimiter(','))
                    {
                        parser.skipDelimiter(',');
                        continue;
                    }
                    if (parser.isDelimiter(']'))
                    {
                        parser.skipDelimiter(']');
                        break;
                    }
                }
            }
            else
                parser.skipDelimiter(']');

            if (newFormat)
                parser.skipDelimiter('}');

        }

        public static void composeJson(JsonComposer composer, CollectionWrapperForComposing coll)
        {

            //mb: new format writes 'size' and 'data' for collections
            // that do have size on GMQ format.
            // So client perceived behaviour can be replicated at parsing

            composer.append('{');
            composer.addNamePart("size");

            int collSz = coll.size();
            composer.composeUnsignedInteger((ulong)collSz);
            composer.append(", ");
            composer.addNamePart("data");

            composer.append('[');
            for (int i = 0; i < collSz; ++i)
            {
                if (i != 0)
                    composer.append(", ");
                coll.compose_next(composer, i);
            }
            composer.append(']');
            composer.append('}');
        }

        public static void parseJsonEntry(JsonParser parser, IParse o)
        {
            if (o is CollectionWrapperForParsing coll)
                parseJson(parser, coll);
            else if (o is MessageWrapperForParsing msg)
                msg.parse(parser);
            else
                throw new Exception(); //TODO
        }

        public static void composeParamToJson(JsonComposer composer, ICompose o)
        {

            if (o is CollectionWrapperForComposing coll)
                composeJson(composer, coll);
            else if (o is MessageWrapperForComposing msg)
                msg.compose(composer);
            else
                throw new Exception(); //TODO
        }

        public static void handleMessage(JsonParser parser, MessageHandlerArray handlers)
        {
            parser.skipDelimiter('{');
            string key;
            parser.readKeyFromJson(out key);
            if (key != "msgid")
                throw new Exception(); // bad format
            ulong msgID;
            parser.parseUnsignedInteger(out msgID);
            parser.skipSpacesEtc();
            parser.skipDelimiter(',');
            parser.readKeyFromJson(out key);
            if (key != "msgbody")
                throw new Exception(); // bad format

            bool handled = handlers.handle(parser, msgID);

            //TODO what to do if not handled???
            if (handled)
                parser.skipDelimiter('}');
        }

        public static void composeStateUpdateMessageBegin(JsonComposer composer)
        {
            composer.append("{\"changes\":[");
        }
        public static void composeStateUpdateMessageEnd(JsonComposer composer)
        {
            composer.append("{}]}");
        }

        public static void composeAddressInPublishable(JsonComposer composer, ulong[] addr, ulong last)
        {
            composer.append('{');
            composer.addNamePart("addr");
            composer.append('[');
            int collSz = addr.Length;
            for (int i = 0; i < collSz; ++i)
            {
                composer.composeUnsignedInteger(addr[i]);
                composer.append(", ");
            }
            composer.composeUnsignedInteger(last);
            composer.append(']');
            composer.append(',');
        }

        public static void publishableComposeLeafeInteger(JsonComposer composer, long value)
        {
            composer.addNamePart("value");
            composer.composeSignedInteger(value);

            composer.append("}");
            composer.append(",");
        }
        public static void publishableComposeLeafeReal(JsonComposer composer, double value)
        {
            composer.addNamePart("value");
            composer.composeReal(value);

            composer.append("}");
            composer.append(",");
        }

        public static void publishableComposeLeafeString(JsonComposer composer, string value)
        {
            composer.addNamePart("value");
            composer.composeString(value);

            composer.append("}");
            composer.append(",");
        }
        public static void publishableComposeLeafeStructBegin(JsonComposer composer)
        {
            composer.addNamePart("value");
            composer.append("{");
        }
        public static void publishableComposeLeafeStructEnd(JsonComposer composer)
        {
            composer.append("}");
            composer.append("}");
            composer.append(",");
        }
        public static void publishableStructComposeInteger(JsonComposer composer, string name, long value, bool separator)
        {
            composer.addNamePart(name);
            composer.composeSignedInteger(value);
            if (separator)
                composer.append(",");
        }
        public static void publishableStructComposeUnsigned(JsonComposer composer, string name, ulong value, bool separator)
        {
            composer.addNamePart(name);
            composer.composeUnsignedInteger(value);
            if (separator)
                composer.append(",");
        }
        public static void publishableStructComposeReal(JsonComposer composer, string name, double value, bool separator)
        {
            composer.addNamePart(name);
            composer.composeReal(value);
            if (separator)
                composer.append(",");
        }
        public static void publishableStructComposeString(JsonComposer composer, string name, string value, bool separator)
        {
            composer.addNamePart(name);
            composer.composeString(value);
            if (separator)
                composer.append(",");
        }

        public static void composePublishableStructBegin(JsonComposer composer, string name)
        {
            composer.addNamePart(name);
            composer.append("{");
        }
        public static void composePublishableStructEnd(JsonComposer composer, bool separator)
        {
            composer.append("}");
            if (separator)
                composer.append(",");
        }
        public static void composeStructBegin(JsonComposer composer)
        {
            composer.append("{");
        }
        public static void composeStructEnd(JsonComposer composer)
        {
            composer.append("}");
        }

    }//json
}

namespace globalmq.marshalling
{
    public class MessageHandler
    {
        public static ulong DefaultHandler = ulong.MaxValue;
        public ulong msgID { get; set; }
        HandlerDelegate lhandler_;

        public delegate void HandlerDelegate(ParserBase parser);
        public MessageHandler(ulong msgID, HandlerDelegate lhandler)
        {
            this.msgID = msgID;
            lhandler_ = lhandler;
        }

        public void handle(ParserBase parser)
        {
            lhandler_(parser);
        }

    }

    // TODO: mb this class is to keep array out of user sight,
    // so we can refactor how we handle this without breaking user code
    // because I want to delay performance tunning until better measurements
    // can be made
    public class MessageHandlerArray
    {
        MessageHandler[] handlers;

        MessageHandlerArray(MessageHandler[] handlers)
        {
            this.handlers = handlers;
        }
        public bool handle(ParserBase parser, ulong msgID)
        {
            // TODO improve
            MessageHandler defaultHandler = null;

            for (int i = 0; i != handlers.Length; ++i)
            {
                if (msgID == handlers[i].msgID)
                {
                    handlers[i].handle(parser);
                    return true;
                }
                else if (handlers[i].msgID == MessageHandler.DefaultHandler)
                {
                    defaultHandler = handlers[i];
                }
            }

            if (defaultHandler != null)
            {
                defaultHandler.handle(parser);
                return true;
            }

            return false;
        }
        public static MessageHandlerArray make(MessageHandler[] handlers)
        {
            return new MessageHandlerArray(handlers);
        }
    }
}