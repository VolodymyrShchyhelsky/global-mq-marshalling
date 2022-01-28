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

using globalmq.marshalling;
using System;
using System.Collections.Generic;
using Xunit;

namespace TestProject1
{

    public class test_publishable_six
    {
        private const string Path = "test_publishable_six.json";
        private const string Path1 = "test_publishable_six_update1.json";
        private const string Path2 = "test_publishable_six_update2.json";
        private const string Path3 = "test_publishable_six_update3.json";

        public static mtest.publishable.StructSix_impl GetPublishableSix()
        {
            mtest.publishable.StructSix_impl data = new mtest.publishable.StructSix_impl();

            data.name = "TheName";
            
            data.basic = data.make_basic();
            data.basic.anInt = -100000;
            data.basic.anUInt = 100000;
            data.basic.aReal = 3.14;
            data.basic.aString = "basic string";

            data.aggregate = data.make_aggregate();
            data.aggregate.name = "aggregate name";

            data.aggregate.theAggregate = data.aggregate.make_theAggregate();
            data.aggregate.theAggregate.anInt = -100;
            data.aggregate.theAggregate.anUInt = 100;
            data.aggregate.theAggregate.aReal = 100;
            data.aggregate.theAggregate.aString = "basic string inside aggregate";
            data.aggregate.lastValue = 0;

            return data;
        }


        [Fact]
        public static void TestJsonComposeStateSync()
        {
            mtest.publishable.StructSix_impl data = GetPublishableSix();

            mtest.publishable.StructSix_publisher publ = new mtest.publishable.StructSix_publisher(data, null, new UInt64[] { });

            SimpleBuffer buffer = new SimpleBuffer();
            JsonPublishableComposer composer = new JsonPublishableComposer();
            
            composer.startTick(buffer);
            publ.generateStateSyncMessage(composer);

            // uncomment to update file
            //buffer.writeToFile(Path);

            Assert.Equal(buffer, SimpleBuffer.readFromFile(Path));
        }

        [Fact]
        public static void TestJsonParseStateSync()
        {
            mtest.publishable.StructSix_impl data = new mtest.publishable.StructSix_impl();

            mtest.publishable.StructSix_subscriber subs = new mtest.publishable.StructSix_subscriber(data);

            SimpleBuffer buffer = SimpleBuffer.readFromFile(Path);
            JsonPublishableParser parser = new JsonPublishableParser(buffer.getReadIterator());

            subs.applyStateSyncMessage(parser);

            Assert.Equal(data, GetPublishableSix());
        }

        [Fact]
        public static void TestJsonComposeUpdate1()
        {
            mtest.publishable.StructSix_impl data = GetPublishableSix();

            mtest.publishable.StructSix_publisher publ = new mtest.publishable.StructSix_publisher(data, new JsonPublishableComposer(), new UInt64[] { });

            SimpleBuffer buffer = new SimpleBuffer();
            //JsonPublishableComposer composer = new JsonPublishableComposer();

            publ.startTick(buffer);
            publ.aggregate.theAggregate.anInt = -101;
            publ.endTick();

            // uncomment to update file
            //buffer.writeToFile(Path1);

            Assert.Equal(buffer, SimpleBuffer.readFromFile(Path1));
        }

        [Fact]
        public static void TestJsonParseUpdate1()
        {
            mtest.publishable.StructSix_impl data = GetPublishableSix();
            mtest.publishable.StructSix_subscriber subs = new mtest.publishable.StructSix_subscriber(data);

            SimpleBuffer buffer = SimpleBuffer.readFromFile(Path1);
            JsonPublishableParser parser = new JsonPublishableParser(buffer.getReadIterator());

            subs.applyMessageWithUpdates(parser);

            mtest.publishable.StructSix_impl data2 = GetPublishableSix();

            Assert.NotEqual(data, data2);

            data2.aggregate.theAggregate.anInt = -101;
            Assert.Equal(data, data2);
        }

        public static void doUpdate2(mtest.publishable.IStructSix data)
        {
            //modify substructure inside vector
            data.aggregate.name = "changed name";

            data.basic.anInt = -2;
            data.basic.anUInt = 3;
            data.basic.aReal = 4.0;

            mtest.publishable.IBasicTypes aggr = data.aggregate.make_theAggregate();
            aggr.anInt = -200;
            aggr.anUInt = 300;
            aggr.aReal = 400.0;
            aggr.aString = "new part";
            data.aggregate.theAggregate = aggr;
        }

        [Fact]
        public static void TestJsonComposeUpdate2()
        {
            mtest.publishable.StructSix_impl data = GetPublishableSix();

            mtest.publishable.StructSix_publisher publ = new mtest.publishable.StructSix_publisher(data, new JsonPublishableComposer(), new UInt64[] { });

            SimpleBuffer buffer = new SimpleBuffer();
            //JsonPublishableComposer composer = new JsonPublishableComposer();

            publ.startTick(buffer);

            doUpdate2(publ);

            publ.endTick();

            // uncomment to update file
            //buffer.writeToFile(Path2);

            Assert.Equal(buffer, SimpleBuffer.readFromFile(Path2));
        }

        [Fact]
        public static void TestJsonParseUpdate2()
        {
            mtest.publishable.StructSix_impl data = GetPublishableSix();
            mtest.publishable.StructSix_subscriber subs = new mtest.publishable.StructSix_subscriber(data);

            SimpleBuffer buffer = SimpleBuffer.readFromFile(Path2);
            JsonPublishableParser parser = new JsonPublishableParser(buffer.getReadIterator());

            subs.applyMessageWithUpdates(parser);

            mtest.publishable.StructSix_impl data2 = GetPublishableSix();

            Assert.NotEqual(data, data2);

            doUpdate2(data2);
 
            Assert.Equal(data, data2);
        }

        [Fact]
        public static void TestComposeNoChangeUpdate3()
        {
            mtest.publishable.StructSix_impl data = GetPublishableSix();

            mtest.publishable.StructSix_publisher publ = new mtest.publishable.StructSix_publisher(data, new JsonPublishableComposer(), new UInt64[] { });

            SimpleBuffer buffer = new SimpleBuffer();
            //JsonPublishableComposer composer = new JsonPublishableComposer();

            publ.startTick(buffer);
            // no change
            publ.endTick();

            // uncomment to update file
            //buffer.writeToFile(Path3);

            Assert.Equal(buffer, SimpleBuffer.readFromFile(Path3));
        }

        [Fact]
        public static void TestJsonParseNoChangeUpdate3()
        {
            mtest.publishable.StructSix_impl data = GetPublishableSix();
            mtest.publishable.StructSix_subscriber subs = new mtest.publishable.StructSix_subscriber(data);

            SimpleBuffer buffer = SimpleBuffer.readFromFile(Path3);
            JsonPublishableParser parser = new JsonPublishableParser(buffer.getReadIterator());

            subs.applyMessageWithUpdates(parser);

            mtest.publishable.StructSix_impl data2 = GetPublishableSix();

            Assert.Equal(data, data2);
        }
    }

}