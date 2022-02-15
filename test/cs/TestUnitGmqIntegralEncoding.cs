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
    /// <summary>
    /// Tests to match Json parser/composer from C# to C++
    /// </summary>
    public class TestUnitGmqIntegralEncoding
    {

        [Theory]
        [InlineData(0)]
        [InlineData(1)]
        [InlineData(2)]
        [InlineData(126)]
        [InlineData(127)]
        [InlineData(128)]
        [InlineData(UInt64.MaxValue)]
        public static void TestVlqEncoding(UInt64 val)
        {
            SimpleBuffer buffer = new SimpleBuffer();
            IntegralVlq.writeVlqIntegral(buffer, val);

            UInt64 result = IntegralVlq.readVlqIntegral(buffer.getReadIterator());

            Assert.Equal<UInt64>(val, result);
        }

        [Theory]
        [InlineData(0)]
        [InlineData(1)]
        [InlineData(2)]
        [InlineData(126)]
        [InlineData(127)]
        [InlineData(128)]
        [InlineData(Int64.MaxValue)]
        [InlineData(Int64.MinValue)]
        [InlineData(-128)]
        [InlineData(-127)]
        [InlineData(-1)]
        public static void TestZigZagEncoding(Int64 val)
        {
            UInt64 uns = IntegralVlq.zigzagEncode(val);
            Int64 result = IntegralVlq.zigzagDecode(uns);

            Assert.Equal<Int64>(val, result);
        }

    }

}
