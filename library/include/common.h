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

#ifndef COMMON_H
#define COMMON_H

#include <fmt/format.h>

/////////////////////////////////////////
// means for replacing std:: by nodecpp::
#ifdef GMQ_USE_EXTERNAL_DEFINITIONS
#include GMQ_USE_EXTERNAL_DEFINITIONS
#endif // GMQ_USE_EXTERNAL_DEFINITIONS

#ifndef GMQ_ASSERT
#include <assert.h>
#define GMQ_ASSERT( condition, ... ) assert( condition )
#endif // GMQ_ASSERT

#ifndef GMQ_COLL
#include <string>
#define GMQ_COLL std::
#endif // GMQ_COLL
/////////////////////////////////////////


namespace m {

	static constexpr size_t MIN_BUFFER = 1024;

	class Buffer {
	private:
		size_t _size = 0;
		size_t _capacity = 0;
		std::unique_ptr<uint8_t[]> _data;

	private:
		void ensureCapacity(size_t sz) { // NOTE: may invalidate pointers
			if (_data == nullptr) {
				reserve(sz);
			}
			else if (sz > _capacity) {
				size_t cp = std::max(sz, MIN_BUFFER);
				std::unique_ptr<uint8_t[]> tmp(new uint8_t[cp]);
				memcpy(tmp.get(), _data.get(), _size);
				_capacity = cp;
				_data = std::move(tmp);
			}
		}

	public:
		Buffer() {}
		Buffer(size_t res) { reserve(res); }
		Buffer(Buffer&& p) {
			std::swap(_size, p._size);
			std::swap(_capacity, p._capacity);
			std::swap(_data, p._data);
		}
		Buffer& operator = (Buffer&& p) {
			std::swap(_size, p._size);
			std::swap(_capacity, p._capacity);
			std::swap(_data, p._data);
			return *this;
		}
		Buffer(const Buffer&) = delete;
		Buffer& operator = (const Buffer& p) = delete;

		void reserve(size_t sz) {
			GMQ_ASSERT( _size == 0 ); 
			GMQ_ASSERT( _capacity == 0 );
			GMQ_ASSERT( _data == nullptr );

			size_t cp = std::max(sz, MIN_BUFFER);
			std::unique_ptr<uint8_t[]> tmp(new uint8_t[cp]);

			_capacity = cp;
			_data = std::move(tmp);
		}

		void append(const void* dt, size_t sz) { // NOTE: may invalidate pointers
			ensureCapacity(_size + sz);
			memcpy(end(), dt, sz);
			_size += sz;
		}

		void trim(size_t sz) { // NOTE: keeps pointers
			GMQ_ASSERT( sz <= _size );
			GMQ_ASSERT( _data != nullptr || (_size == 0 && sz == 0) );
			_size -= sz;
		}

		void clear() {
			trim(size());
		}

		void set_size(size_t sz) { // NOTE: keeps pointers
			GMQ_ASSERT( sz <= _capacity );
			GMQ_ASSERT( _data != nullptr );
			_size = sz;
		}

		size_t size() const { return _size; }
		bool empty() const { return _size == 0; }
		size_t capacity() const { return _capacity; }
		uint8_t* begin() { return _data.get(); }
		const uint8_t* begin() const { return _data.get(); }
		uint8_t* end() { return _data.get() + _size; }
		const uint8_t* end() const { return _data.get() + _size; }

		// TODO: revise and add other relevant calls
		size_t writeInt8( int8_t val, size_t pos ) {
			ensureCapacity(pos + 1);
			*reinterpret_cast<uint8_t*>(begin() + pos ) = val;
			if ( _size < pos + 1 )
				_size = pos + 1;
			return pos + 1;
		}
		size_t appendUint8( int8_t val ) {
			ensureCapacity(size() + 1);
			*reinterpret_cast<uint8_t*>(begin() + size() ) = val;
			return ++_size;
		}

		uint32_t readUInt8(size_t offset) const {
			GMQ_ASSERT( offset + 1 <= _size );
			return *(begin() + offset);
		}

		size_t appendString( const GMQ_COLL string& str ) {
			append( str.c_str(), str.size() );
			return _size;
		}
	};
	
} // namespace m


#endif // COMMON_H
