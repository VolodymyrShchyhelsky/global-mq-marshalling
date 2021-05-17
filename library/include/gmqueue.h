/* -------------------------------------------------------------------------------
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

#ifndef GMQUEUE_H
#define GMQUEUE_H

#include "global_mq_common.h"
#include "marshalling.h"
#include <thread>
#include <mutex>
#include <condition_variable>

#ifndef GMQUEUE_CUSTOMIZED_Q_TYPES
class GMQueueStatePublisherSubscriberTypeInfo
{
public:
	using BufferT = globalmq::marshalling::Buffer;
	using ParserT = globalmq::marshalling::JsonParser<BufferT>;
	using ComposerT = globalmq::marshalling::JsonComposer<BufferT>;
//	using ParserT = globalmq::marshalling::GmqParser<BufferT>;
//	using ComposerT = globalmq::marshalling::GmqComposer<BufferT>;
	using StateSubscriberT = globalmq::marshalling::StateSubscriberBase<BufferT>;
	using StatePublisherT = globalmq::marshalling::StatePublisherBase<ComposerT>;
};
#else
#include GMQUEUE_CUSTOMIZED_Q_TYPES
#endif


namespace globalmq::marshalling {

using MessageBufferT = typename GMQueueStatePublisherSubscriberTypeInfo::BufferT;

struct GmqPathHelper
{
	struct PathComponents
	{
		GMQ_COLL string authority;
		bool furtherResolution = false;
		bool hasPort = false;
		uint16_t port = 0xFFFF;
//		GMQ_COLL string localPart; // TODO: revise
		GMQ_COLL string nodeName;
		GMQ_COLL string statePublisherName;
	};

	static GMQ_COLL string compose( GMQ_COLL string authority, GMQ_COLL string nodeName, GMQ_COLL string statePublisherName )
	{
		// TODO: check components
		GMQ_COLL string ret = "globalmq:";
		if ( !authority.empty() )
		{
			ret += "//";
			ret += authority;
		}
		assert( !nodeName.empty() );
		assert( !statePublisherName.empty() );
		ret += '/';
		ret += localPart( nodeName, statePublisherName );
		return ret;
	}

	static GMQ_COLL string compose( const PathComponents& components )
	{
		// TODO: check components
		GMQ_COLL string ret = "globalmq:";
		if ( !components.authority.empty() )
		{
			ret += "//";
			ret += components.authority;
		}
		if ( components.furtherResolution )
			ret += "!gmq";
		if ( components.hasPort )
		{
			auto str = fmt::format( ":{}", components.port );
			ret += str;
		}
		assert( !components.nodeName.empty() );
		assert( !components.statePublisherName.empty() );
		ret += '/';
		ret += localPart( components.nodeName, components.statePublisherName );
		return ret;
	}

	static GMQ_COLL string localPart( GMQ_COLL string nodeName, GMQ_COLL string statePublisherName )
	{
		return fmt::format( "{}?sp={}", nodeName, statePublisherName );
	}

	static GMQ_COLL string localPart( const PathComponents& components )
	{
		return fmt::format( "{}?sp={}", components.nodeName, components.statePublisherName );
	}

	static bool parse( GMQ_COLL string path, PathComponents& components )
	{
		size_t pos = path.find( "globalmq:" );
		if ( pos != 0 )
			return false;
		pos += sizeof( "globalmq:" ) - 1;
		if ( path.size() <= pos )
			return false;
		if ( path[pos++] != '/' )
			return false;
		if ( path[pos] == '/' ) // double-slash, authority component is present
		{
			++pos;
			size_t pos1 = path.find( "/", pos );
			if ( pos1 == GMQ_COLL string::npos )
				return false;
			components.authority = path.substr( pos, pos1 - pos );
			pos = pos1 + 1;
			pos1 = components.authority.find_last_of( ':' );
			if ( pos1 != GMQ_COLL string::npos )
			{
				char* end = nullptr;
				size_t port = strtol( components.authority.c_str() + pos1 + 1, &end, 10 );
				if ( components.authority.c_str() + pos1 + 1 == end )
					return false;
				if ( end - components.authority.c_str() < components.authority.size() ) // there are remaining chars
					return false;
				if ( port >= UINT16_MAX )
					return false;
				components.hasPort = true;
				components.port = (uint16_t)port;
				components.authority.erase( pos1 );
			}
			else
			{
				components.hasPort = false;
				components.port = 0xFFFF;
			}

			size_t pos2 = components.authority.find_last_of( '!' );
			if ( pos2 != GMQ_COLL string::npos )
			{
				if ( components.authority.size() - pos2 < sizeof( "gmq" ) - 1 )
					return false;
				if ( components.authority.substr( pos2 + 1 ) != "gmq" )
					return false;
				components.furtherResolution = true;
				components.authority.erase( pos2 );
			}
			else
			{
				components.furtherResolution = false;
			}
		}
		else
		{
			components.authority = "";
			components.hasPort = false;
			components.furtherResolution = false;
			components.port = 0xFFFF;
		}

		// node name
		size_t pos1 = path.find( '?', pos );
		if ( pos1 == GMQ_COLL string::npos )
			return false;
		components.nodeName = path.substr( pos, pos1 - pos );
		pos = pos1;

		// statePublisherName
		pos = path.find( "sp=", pos );
		if ( pos == GMQ_COLL string::npos )
			return false;
		pos += sizeof( "sp=" ) - 1;
		pos1 = path.find( '&', pos );
		if ( pos1 == GMQ_COLL string::npos )
			components.statePublisherName = path.substr( pos );
		else
			components.statePublisherName = path.substr( pos, pos1 - pos );
		return true;
	}
};

struct PublishableStateMessageHeader
{
	enum MsgType { undefined = 0, subscriptionRequest = 1, subscriptionResponse = 2, stateUpdate = 3, connectionRequest=4, connectionAccepted=5, connectionMessage=6 };
	MsgType type;
	uint64_t state_type_id; // Note: may be removed in future versions
	uint64_t priority;
	GMQ_COLL string path;  // subscriptionRequest only
	uint64_t ref_id_at_subscriber; // updatable
	uint64_t ref_id_at_publisher; // updatable

	struct UpdatedData
	{
		uint64_t ref_id_at_subscriber;
		uint64_t ref_id_at_publisher;
		bool update_ref_id_at_subscriber = false;
		bool update_ref_id_at_publisher = false;
	};

	template<class ParserT>
	void parse( ParserT& parser )
	{
		globalmq::marshalling::impl::parsePublishableStructBegin( parser, "hdr" );
		size_t msgType;
		globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &msgType, "msg_type" );
		globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &state_type_id, "state_type_id" );
		globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &priority, "priority" );
		switch ( msgType )
		{
			case MsgType::subscriptionRequest:
			case MsgType::connectionRequest:
			{
				type = (MsgType)(msgType);
				globalmq::marshalling::impl::publishableParseString<ParserT, GMQ_COLL string>( parser, &path, "path" );
				globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &ref_id_at_subscriber, "ref_id_at_subscriber" );
				break;
			}
			case MsgType::subscriptionResponse:
			case MsgType::stateUpdate:
			case MsgType::connectionAccepted:
			case MsgType::connectionMessage:
			{
				type = (MsgType)(msgType);
				globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &ref_id_at_subscriber, "ref_id_at_subscriber" );
				globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &ref_id_at_publisher, "ref_id_at_publisher" );
				break;
			}
			default:
				throw std::exception(); // TODO: ... (unknown msg type)
		}
		globalmq::marshalling::impl::parsePublishableStructEnd( parser );
	}

	template<class ParserT, class ComposerT>
	static void parseAndUpdate( ParserT& msgStartParser, ParserT& parser, typename ComposerT::BufferType& buff, const UpdatedData& udata )
	{
		ComposerT composer( buff );
//		ParserT parser2 = parser;
		globalmq::marshalling::impl::parsePublishableStructBegin( parser, "hdr" );
		size_t msgType;
		globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &msgType, "msg_type" );
		uint64_t dummy;
		GMQ_COLL string dummyStr;
		globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &dummy, "state_type_id" );
		globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &dummy, "priority" );
		switch ( msgType )
		{
			case MsgType::subscriptionRequest:
			case MsgType::connectionRequest:
			{
				globalmq::marshalling::impl::publishableParseString<ParserT, GMQ_COLL string>( parser, &dummyStr, "path" );
				assert( !udata.update_ref_id_at_publisher );
				size_t offset = parser.getCurrentOffset();
				::globalmq::marshalling::copy<typename ParserT::RiterT, typename ComposerT::BufferType>( msgStartParser.getIterator(), buff, offset );
				globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &dummy, "ref_id_at_subscriber" );
				if ( udata.update_ref_id_at_subscriber )
					globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, udata.ref_id_at_subscriber, "ref_id_at_subscriber", false );
				else
					globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, dummy, "ref_id_at_subscriber", false );
				break;
			}
			case MsgType::subscriptionResponse:
			case MsgType::stateUpdate:
			case MsgType::connectionAccepted:
			case MsgType::connectionMessage:
			{
				size_t offset = parser.getCurrentOffset();
				::globalmq::marshalling::copy<typename ParserT::RiterT, typename ComposerT::BufferType>( msgStartParser.getIterator(), buff, offset );
				globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &dummy, "ref_id_at_subscriber" );
				if ( udata.update_ref_id_at_subscriber )
					globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, udata.ref_id_at_subscriber, "ref_id_at_subscriber", true );
				else
					globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, dummy, "ref_id_at_subscriber", true );
				globalmq::marshalling::impl::publishableParseUnsignedInteger<ParserT, size_t>( parser, &dummy, "ref_id_at_publisher" );
				if ( udata.update_ref_id_at_publisher )
					globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, udata.ref_id_at_publisher, "ref_id_at_publisher", false );
				else
					globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, dummy, "ref_id_at_publisher", false );
				break;
			}
			default:
				throw std::exception(); // TODO: ... (unknown msg type)
		}
		::globalmq::marshalling::copy<typename ParserT::RiterT, typename ComposerT::BufferType>( parser.getIterator(), buff );
	}

	template<class ComposerT>
	void compose(ComposerT& composer, bool addSeparator) const
	{
		globalmq::marshalling::impl::composePublishableStructBegin( composer, "hdr" );
		globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, (uint32_t)(type), "msg_type", true );
		globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, state_type_id, "state_type_id", true );
		globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, priority, "priority", true );
		switch ( type )
		{
			case MsgType::subscriptionRequest:
			case MsgType::connectionRequest:
			{
				globalmq::marshalling::impl::publishableStructComposeString( composer, path, "path", true );
				globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, ref_id_at_subscriber, "ref_id_at_subscriber", false );
				break;
			}
			case MsgType::subscriptionResponse:
			case MsgType::stateUpdate:
			case MsgType::connectionAccepted:
			case MsgType::connectionMessage:
			{
				globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, ref_id_at_subscriber, "ref_id_at_subscriber", true );
				globalmq::marshalling::impl::publishableStructComposeUnsignedInteger( composer, ref_id_at_publisher, "ref_id_at_publisher", false );
				break;
			}
		}
		globalmq::marshalling::impl::composePublishableStructEnd( composer, addSeparator );
	}
};

template<class ComposerT>
void helperComposePublishableStateMessageBegin(ComposerT& composer, const PublishableStateMessageHeader& header)
{
	globalmq::marshalling::impl::composeStructBegin( composer );
	if ( header.type == PublishableStateMessageHeader::MsgType::subscriptionResponse || header.type == PublishableStateMessageHeader::MsgType::stateUpdate || header.type == PublishableStateMessageHeader::MsgType::connectionMessage )
	{
		header.compose( composer, true );
		globalmq::marshalling::impl::composeKey( composer, "data" );
		// next call would be generateXXXMessage()
	}
	else
		header.compose( composer, false );
}

template<class ComposerT>
void helperComposePublishableStateMessageEnd(ComposerT& composer)
{
	globalmq::marshalling::impl::composeStructEnd( composer );
}

template<class ParserT>
void helperParsePublishableStateMessageBegin( ParserT& parser, PublishableStateMessageHeader& header ) // leaves parser pos at the beginning of message data part (if any)
{
	globalmq::marshalling::impl::parseStructBegin( parser );
	header.parse( parser );
	if ( header.type == PublishableStateMessageHeader::MsgType::subscriptionResponse || header.type == PublishableStateMessageHeader::MsgType::stateUpdate || header.type == PublishableStateMessageHeader::MsgType::connectionMessage )
		globalmq::marshalling::impl::parseKey( parser, "data" );
}

template<class ParserT>
void helperParsePublishableStateMessageEnd(ParserT& parser)
{
	globalmq::marshalling::impl::parseStructEnd( parser );
}

template<class ParserT, class ComposerT>
void helperParseAndUpdatePublishableStateMessage( typename ParserT::BufferType& buffFrom, typename ComposerT::BufferType& buffTo, const PublishableStateMessageHeader::UpdatedData& udata )
{
	ParserT parser( buffFrom );
	ParserT parserCurrent( buffFrom );
	PublishableStateMessageHeader header;
	globalmq::marshalling::impl::parseStructBegin( parserCurrent );
	header.parseAndUpdate<ParserT, ComposerT>( parser, parserCurrent, buffTo, udata );
}




template<class InputBufferT, class ComposerT>
class StateConcentratorBase
{
	using OutputBufferT = typename ComposerT::BufferType;

public:
	virtual ~StateConcentratorBase() {}
	// as subscriber
	virtual void applyGmqMessageWithUpdates( GmqParser<InputBufferT>& parser ) = 0;
	virtual void applyJsonMessageWithUpdates( JsonParser<InputBufferT>& parser ) = 0;
	virtual void applyGmqStateSyncMessage( GmqParser<InputBufferT>& parser ) = 0;
	virtual void applyJsonStateSyncMessage( JsonParser<InputBufferT>& parser ) = 0;
	// as publisher
	virtual void generateStateSyncMessage( ComposerT& composer ) = 0;

	virtual const char* name() = 0;
};

template<class InputBufferT, class ComposerT>
class StateConcentratorFactoryBase
{
public:
	virtual StateConcentratorBase<InputBufferT, ComposerT>* createConcentrator( uint64_t typeID ) = 0;
};

class InProcessMessagePostmanBase
{
public:
	InProcessMessagePostmanBase() {};
	virtual void postMessage( MessageBufferT&& ) = 0;
	virtual void postInfrastructuralMessage( MessageBufferT&& ) = 0;
	virtual ~InProcessMessagePostmanBase() {}
};

struct AddressableLocation
{
	InProcessMessagePostmanBase* postman = nullptr;
	uint64_t reincarnation = 0;
};

struct SlotIdx
{
	static constexpr size_t invalid_idx = (size_t)(-1);
	static constexpr size_t invalid_reincarnation = (size_t)(-1);
	size_t idx = invalid_idx;
	uint64_t reincarnation = invalid_reincarnation;
	bool isInitialized() { return idx != invalid_idx && reincarnation != invalid_reincarnation; }
	bool operator == ( const SlotIdx& other ) { return idx == other.idx && reincarnation == other.reincarnation; }
	void invalidate() { idx = invalid_idx; reincarnation = invalid_reincarnation; }
};

class AddressableLocations // one per process; provides process-unique Slot with Postman and returns its SlotIdx
{
	GMQ_COLL vector<AddressableLocation> slots; // mx-protected!
public:
	SlotIdx add( InProcessMessagePostmanBase* postman )
	{ 
		// essentially add to slots and return its idx
		for ( size_t i = 0; i<slots.size(); ++i )
			if ( slots[i].postman == nullptr )
			{
				slots[i].postman = postman;
				++(slots[i].reincarnation);
				return SlotIdx({i,slots[i].reincarnation});
			}
		slots.push_back({postman, 0});
		return SlotIdx({slots.size() - 1, 0});
	}
	void remove( SlotIdx idx )
	{ 
		// find by idx.idx, check reincarnaion, set postman to null
		assert ( idx.idx < slots.size() ); 
		assert ( idx.reincarnation == slots[idx.idx].reincarnation ); 
		slots[idx.idx].postman = nullptr;
	}
	InProcessMessagePostmanBase* getPostman( SlotIdx idx )
	{
		// access, verify, return
		assert ( idx.idx < slots.size() ); 
		assert ( idx.reincarnation == slots[idx.idx].reincarnation ); 
		return slots[idx.idx].postman;
	}
};

template<class PlatformSupportT>
class GMQueue
{
	using InputBufferT = typename PlatformSupportT::BufferT;
	using ComposerT = typename PlatformSupportT::ComposerT;
	using ParserT = typename PlatformSupportT::ParserT;

	AddressableLocations addressableLocations;

	GMQ_COLL string myAuthority;
	bool isMyAuthority( GMQ_COLL string authority )
	{
		// TODO: regexp comparison (note: myAuthority could be '*', etc );
		return authority == myAuthority || authority.empty();
	}

	class ConcentratorWrapper
	{
		friend class GMQueue<PlatformSupportT>;

		StateConcentratorBase<InputBufferT, ComposerT>* ptr = nullptr;
		bool subscriptionResponseReceived = false;
		uint64_t idAtPublisher;

	public:
		struct SubscriberData
		{
			uint64_t ref_id_at_subscriber;
			uint64_t ref_id_at_publisher;
			SlotIdx senderSlotIdx;
		};
		GMQ_COLL vector<SubscriberData> subscribers;

	public:
		ConcentratorWrapper( StateConcentratorBase<InputBufferT, ComposerT>* ptr_ ) : ptr( ptr_ ) {}
		ConcentratorWrapper( const ConcentratorWrapper& ) = delete;
		ConcentratorWrapper& operator = ( const ConcentratorWrapper& ) = delete;
		ConcentratorWrapper( ConcentratorWrapper&& other ) { ptr = other.ptr; other.ptr = nullptr; }
		ConcentratorWrapper& operator = ( ConcentratorWrapper&& other ) { ptr = other.ptr; other.ptr = nullptr; return *this; }
		~ConcentratorWrapper() { if ( ptr ) delete ptr; }

		// Gmqueue part (indeed, we need it only if 'remove concentrator' event may actually happen (conditions?))
		GMQ_COLL string address;
		uint64_t id;

	public:
		uint64_t addSubscriber( SubscriberData sd )
		{
			subscribers.push_back( sd );
			return subscribers.size() - 1;
		}

		bool isSsubscriptionResponseReceived() { return subscriptionResponseReceived; }

		void generateStateSyncMessage( ComposerT& composer )
		{
			assert( ptr != nullptr );
			assert( subscriptionResponseReceived );
			ptr->generateStateSyncMessage( composer );
		}

		void onSubscriptionResponseMessage( ParserT& parser, uint64_t idAtPublisher_ ) 
		{
			assert( ptr != nullptr );
			assert( subscribers.size() != 0 ); // current implementation does not practically assume removing subscribers 
			if constexpr ( ParserT::proto == globalmq::marshalling::Proto::JSON )
				ptr->applyJsonStateSyncMessage( parser );
			else 
			{
				static_assert( ParserT::proto == globalmq::marshalling::Proto::GMQ );
				ptr->applyGmqStateSyncMessage( parser );
			}
			subscriptionResponseReceived = true;
			idAtPublisher = idAtPublisher_;
		}

		void onStateUpdateMessage( ParserT& parser ) 
		{
			assert( ptr != nullptr );
			if constexpr ( ParserT::proto == globalmq::marshalling::Proto::JSON )
				ptr->applyJsonMessageWithUpdates( parser );
			else 
			{
				static_assert( ParserT::proto == globalmq::marshalling::Proto::GMQ );
				ptr->applyGmqMessageWithUpdates( parser );
			}
		}
	};

	std::mutex mx;

	GMQ_COLL unordered_map<GMQ_COLL string, ConcentratorWrapper> addressesToStateConcentrators; // address to concentrator mapping, 1 - 1, mx-protected
	GMQ_COLL unordered_map<uint64_t, ConcentratorWrapper*> idToStateConcentrators; // id to concentrator mapping, many - 1, mx-protected
	uint64_t concentratorIDBase = 0;

//	GMQ_COLL unordered_map<GMQ_COLL string, AddressableLocation> namedRecipients; // node name to location, mx-protected
	GMQ_COLL unordered_map<GMQ_COLL string, SlotIdx> namedRecipients; // node name to location, mx-protected

	GMQ_COLL unordered_map<uint64_t, SlotIdx> senders; // node name to location, mx-protected
	uint64_t senderIDBase = 0;

	GMQ_COLL unordered_map<uint64_t, std::pair<uint64_t, uint64_t>> ID2ConcentratorSubscriberPairMapping;
	uint64_t publisherAndItsConcentratorBase = 0;

	StateConcentratorFactoryBase<InputBufferT, ComposerT>* stateConcentratorFactory = nullptr;

	void addConcentratorSubscriberPair( uint64_t id, uint64_t concentratorID, uint64_t subscriberDataID ) {
		auto ins = ID2ConcentratorSubscriberPairMapping.insert( std::make_pair( id, std::make_pair( concentratorID, subscriberDataID ) ) );
		assert( ins.second );
	}
	void removeConcentratorSubscriberPair( uint64_t ID ) {
		ID2ConcentratorSubscriberPairMapping.erase( ID );
	}
	std::pair<uint64_t, uint64_t> findConcentratorSubscriberPair( uint64_t ID ) {
		auto f = ID2ConcentratorSubscriberPairMapping.find( ID );
		if ( f != ID2ConcentratorSubscriberPairMapping.end() )
			return f->second;
		else
			throw std::exception();
	}

	// concentrators (address2concentrators)
	std::pair<ConcentratorWrapper*, bool> findOrAddStateConcentrator( GMQ_COLL string path, uint64_t stateTypeID ) {
		assert( !path.empty() );
		auto f = addressesToStateConcentrators.find( path );
		if ( f != addressesToStateConcentrators.end() )
			return std::make_pair(&(f->second), true);
		else
		{
			assert( !path.empty() );
			assert( stateConcentratorFactory != nullptr );
			auto concentrator = stateConcentratorFactory->createConcentrator( stateTypeID );
			assert( concentrator != nullptr );
			ConcentratorWrapper cwrapper( concentrator );
			auto ins = addressesToStateConcentrators.insert( std::make_pair( path, std::move( cwrapper ) ) );
			assert( ins.second );
			ConcentratorWrapper* c = &(ins.first->second);
			uint64_t concentratorID = ++concentratorIDBase;
			auto ins1 = idToStateConcentrators.insert( std::make_pair( concentratorID, c ) );
			assert( ins1.second );
			c->address = path;
			c->id = concentratorID;
			return std::make_pair(c, false);
		}
	}
	ConcentratorWrapper* findStateConcentrator( uint64_t id ) {
		assert( id != 0 );
		auto f = idToStateConcentrators.find( id );
		if ( f != idToStateConcentrators.end() )
			return f->second;
		else
			return nullptr;
	}
	/*void removeStateConcentrator( GMQ_COLL string path ) { // TODO: rework
		assert( !path.empty() );
		std::unique_lock<std::mutex> lock(mxAddressesToStateConcentrators);
		addressesToStateConcentrators.erase( path );
		idToStateConcentrators.erase( id );
	}*/

	// named local objects (namedRecipients)
	void addNamedLocation( GMQ_COLL string name, SlotIdx idx ) {
		assert( !name.empty() );
		auto ins = namedRecipients.insert( std::make_pair( name, idx ) );
		assert( ins.second );
	}
	void removeNamedLocation( GMQ_COLL string name ) {
		assert( !name.empty() );
		namedRecipients.erase( name );
	}

	public:
	SlotIdx locationNameToSlotIdx( GMQ_COLL string name ) {
		assert( !name.empty() );
		auto f = namedRecipients.find( name );
		if ( f != namedRecipients.end() )
			return f->second;
		else
			return SlotIdx();
	}

	private:
	uint64_t addSender( SlotIdx idx ) {
		uint64_t id = ++senderIDBase;
		auto ins = senders.insert( std::make_pair( id, idx ) );
		assert( ins.second );
		return id;
	}
	void removeSender( uint64_t id, SlotIdx idx ) {
		auto f = senders.find( id );
		assert( f != senders.end() );
		assert( f->second.idx == idx.idx );
		assert( f->second.reincarnation == idx.reincarnation );
		senders.erase( id );
	}
	public:
	SlotIdx senderIDToSlotIdx( uint64_t id ) {
		auto f = senders.find( id );
		if ( f != senders.end() )
			return f->second;
		else
			return SlotIdx();
	}

public:
	GMQueue() {}
	~GMQueue()
	{ 
		std::unique_lock<std::mutex> lock(mx);

		if ( stateConcentratorFactory != nullptr )
			delete stateConcentratorFactory;
	}

	template<class StateFactoryT>
	void initStateConcentratorFactory() // Note: potentially, temporary solution
	{
		std::unique_lock<std::mutex> lock(mx);

		assert( stateConcentratorFactory == nullptr ); // must be called just once
		stateConcentratorFactory = new StateFactoryT;
	}
	void setAuthority( GMQ_COLL string authority )
	{ 
		std::unique_lock<std::mutex> lock(mx);

		assert( myAuthority.empty() ); // set just once
		myAuthority = authority;
	}

	void postMessage( MessageBufferT&& msg, uint64_t senderID, SlotIdx senderSlotIdx )
	{
		std::unique_lock<std::mutex> lock(mx);

		SlotIdx senderIdx = senderIDToSlotIdx( senderID );
		assert( senderIdx.idx == senderSlotIdx.idx );
		assert( senderIdx.reincarnation == senderSlotIdx.reincarnation );
		PublishableStateMessageHeader mh;
		ParserT parser( msg );
		helperParsePublishableStateMessageBegin( parser, mh );
		switch ( mh.type )
		{
			case PublishableStateMessageHeader::MsgType::subscriptionRequest:
			{
				GmqPathHelper::PathComponents pc;
				bool pathOK = GmqPathHelper::parse( mh.path, pc );
				if ( !pathOK )
					throw std::exception(); // TODO: ... (bad path)

				GMQ_COLL string addr = GmqPathHelper::localPart( pc );
				if ( isMyAuthority( pc.authority ) ) // local
				{
					assert( !pc.nodeName.empty() );

					auto findCr = findOrAddStateConcentrator( addr, mh.state_type_id );
					ConcentratorWrapper* concentrator = findCr.first;
					assert( concentrator != nullptr );

					typename ConcentratorWrapper::SubscriberData sd;
					sd.ref_id_at_subscriber = mh.ref_id_at_subscriber;
					sd.ref_id_at_publisher = ++publisherAndItsConcentratorBase;
					sd.senderSlotIdx = senderSlotIdx;
					uint64_t sid = concentrator->addSubscriber( sd );
					addConcentratorSubscriberPair( sd.ref_id_at_publisher, sid, concentrator->id );

					if ( findCr.second )
					{
						if ( concentrator->isSsubscriptionResponseReceived() )
						{
							PublishableStateMessageHeader hdrBack;
							hdrBack.type = PublishableStateMessageHeader::MsgType::subscriptionResponse;
							hdrBack.priority = mh.priority;
							hdrBack.ref_id_at_subscriber = mh.ref_id_at_subscriber;
							hdrBack.ref_id_at_publisher = sd.ref_id_at_publisher;

							typename ComposerT::BufferType msgBack;
							ComposerT composer( msgBack );
							helperComposePublishableStateMessageBegin( composer, hdrBack );
							concentrator->generateStateSyncMessage( composer );
							helperComposePublishableStateMessageEnd( composer );

							InProcessMessagePostmanBase* postman = addressableLocations.getPostman( senderSlotIdx );
							postman->postMessage( std::move( msgBack ) );
						}
					}
					else
					{
						// TODO: revise!
//						concentrator->addSubscriber( rpi );
						SlotIdx targetIdx = locationNameToSlotIdx( pc.nodeName );
						if ( targetIdx.idx == SlotIdx::invalid_idx )
							throw std::exception(); // TODO: post permanent error message to sender instead or in addition

						globalmq::marshalling::PublishableStateMessageHeader::UpdatedData ud;
						ud.ref_id_at_subscriber = concentrator->id;
						ud.update_ref_id_at_subscriber = true;

						typename ComposerT::BufferType msgForward;
						helperParseAndUpdatePublishableStateMessage<ParserT, ComposerT>( msg, msgForward, ud );

						InProcessMessagePostmanBase* postman = addressableLocations.getPostman( targetIdx );
						postman->postMessage( std::move( msgForward ) );
					}
				}
				else
				{
					assert( false ); // not yet implemented (but up to Postman should go along the same lines)
				}
				break;
			}
			case PublishableStateMessageHeader::MsgType::subscriptionResponse:
			{
				ConcentratorWrapper* concentrator = findStateConcentrator( mh.ref_id_at_publisher );
				concentrator->onSubscriptionResponseMessage( parser, mh.ref_id_at_publisher );

				// forward message to all concentrator's subscribers
				PublishableStateMessageHeader::UpdatedData ud;
				ud.update_ref_id_at_publisher = true;
				ud.update_ref_id_at_subscriber = true;
				for ( auto& subscriber : concentrator->subscribers )
				{
					ud.ref_id_at_subscriber = subscriber.ref_id_at_subscriber;
					ud.ref_id_at_publisher = subscriber.ref_id_at_publisher;
					typename ComposerT::BufferType msgForward;
					helperParseAndUpdatePublishableStateMessage<ParserT, ComposerT>( msg, msgForward, ud );

					InProcessMessagePostmanBase* postman = addressableLocations.getPostman( subscriber.senderSlotIdx );
					postman->postMessage( std::move( msgForward ) );
				}

				break;
			}
			case PublishableStateMessageHeader::MsgType::stateUpdate: // so far we have the same processing
			{
				ConcentratorWrapper* concentrator = findStateConcentrator( mh.ref_id_at_publisher );
				if ( concentrator == nullptr )
					throw std::exception(); // TODO: ?
				concentrator->onStateUpdateMessage( parser );

				// forward message to all concentrator's subscribers
				PublishableStateMessageHeader::UpdatedData ud;
				ud.update_ref_id_at_publisher = true;
				ud.update_ref_id_at_subscriber = true;
				for ( auto& subscriber : concentrator->subscribers )
				{
					ud.ref_id_at_subscriber = subscriber.ref_id_at_subscriber;
					ud.ref_id_at_publisher = subscriber.ref_id_at_publisher;
					typename ComposerT::BufferType msgForward;
					helperParseAndUpdatePublishableStateMessage<ParserT, ComposerT>( msg, msgForward, ud );

					InProcessMessagePostmanBase* postman = addressableLocations.getPostman( subscriber.senderSlotIdx );
					postman->postMessage( std::move( msgForward ) );
				}

				break;
			}
			default:
				throw std::exception(); // TODO: ... (unknown msg type)
		}
	}

	void postInfrastructuralMessage( MessageBufferT&& msg, GMQ_COLL string nodeName )
	{
		std::unique_lock<std::mutex> lock(mx);

		assert( !nodeName.empty() );

		SlotIdx targetIdx = locationNameToSlotIdx( nodeName );
		if ( targetIdx.idx == SlotIdx::invalid_idx )
			throw std::exception(); // TODO: post permanent error message to sender instead or in addition

		InProcessMessagePostmanBase* postman = addressableLocations.getPostman( targetIdx );
		postman->postInfrastructuralMessage( std::move( msg ) );
	}

	uint64_t add( GMQ_COLL string name, InProcessMessagePostmanBase* postman, SlotIdx& idx )
	{
		assert( !name.empty() );
		idx = addressableLocations.add( postman );
		addNamedLocation( name, idx );
		return addSender( idx );
	}
	uint64_t add( InProcessMessagePostmanBase* postman, SlotIdx& idx )
	{
		idx = addressableLocations.add( postman );
		return addSender( idx );
	}
	void remove( GMQ_COLL string name, SlotIdx idx )
	{
		if ( !name.empty() )
			removeNamedLocation( name );
		addressableLocations.remove( idx );
		// TODO: revise and finalize implementation
	}

	template<class PostmanT, class ... Args>
	static InProcessMessagePostmanBase* allocPostman(Args&& ... args)
	{
		static_assert( alignof( PostmanT ) <= sizeof(::std::max_align_t) );
		PostmanT* ret = static_cast<PostmanT *>( ::malloc( sizeof(PostmanT) ) );
		new (ret) PostmanT( std::forward<Args>( args )...);
		return ret;
	}

};

struct InProcTransferrable
{
	GMQ_COLL string name;
	uint64_t id;
	void serialize( uint8_t* buff, size_t maxSz ) // NOTE: temporary solution
	{
		assert( name.size() + 1 + sizeof( id ) <= maxSz );
		memcpy( buff, &id, sizeof( id ) );
		memcpy( buff + sizeof( id ), name.c_str(), name.size() );
		buff[ sizeof( id ) + name.size() ] = 0;
	}
	void deserilaize( uint8_t* buff ) // NOTE: temporary solution
	{
		memcpy( &id, buff, sizeof( id ) );
		name = (char*)(buff + sizeof( id ) );
	}
};

template<class PlatformSupportT>
class GMQTransportBase
{
protected:
	GMQueue<PlatformSupportT>& gmq;
	GMQ_COLL string name;
	SlotIdx idx;
	uint64_t id;
	GMQTransportBase( GMQueue<PlatformSupportT>& queue, GMQ_COLL string name_, SlotIdx idx_, uint64_t id_ ) : gmq( queue ), name( name_ ), idx( idx_ ), id ( id_ ) {}

public:
	GMQTransportBase( GMQueue<PlatformSupportT>& queue ) : gmq( queue ) {}
	virtual ~GMQTransportBase() {
		if ( idx.isInitialized() )
			gmq.remove( name, idx );
	}
protected:
	GMQTransportBase( GMQueue<PlatformSupportT>& queue, GMQ_COLL string name_, InProcessMessagePostmanBase* postman ) : gmq( queue ), name( name_ ) {
		assert( !name_.empty() );
		id = gmq.add( name, postman, idx );
	};
	GMQTransportBase( GMQueue<PlatformSupportT>& queue, InProcessMessagePostmanBase* postman ) : gmq( queue ) {
		id = gmq.add( postman, idx );
	};

public:
	void postMessage( MessageBufferT&& msg ){
		assert( idx.isInitialized() );
		gmq.postMessage( std::move( msg ), id, idx );
	}
	void postInfrastructuralMessage( MessageBufferT&& msg, GMQ_COLL string nodeName ){
		assert( idx.isInitialized() );
		gmq.postMessage( std::move( msg ), nodeName );
	}

public:
	InProcTransferrable makeTransferrable()
	{
		InProcTransferrable ret;
		ret.name = name;
		ret.id = id;
		idx.invalidate();
		return ret;
	}

	void restore( const InProcTransferrable& t, GMQueue<PlatformSupportT>& queue_ ) {
		assert( &gmq == &queue_ );
		assert( !idx.isInitialized() );
		if ( t.name.empty() )
		{
			idx = gmq.senderIDToSlotIdx( t.id );
			assert( idx.isInitialized() );
			id = t.id;
		}
		else
		{
			idx = gmq.locationNameToSlotIdx( t.name );
			assert( idx.isInitialized() );
			SlotIdx idx2 = gmq.senderIDToSlotIdx( t.id );
			assert( idx2.isInitialized() );
			assert( idx == idx2 );
			name = t.name;
			id = t.id;
		}
	}
};



} // namespace globalmq::marshalling

#endif // GMQUEUE_H
