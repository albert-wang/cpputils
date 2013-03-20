#include <boost/any.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <set>
#include <queue>

#pragma once
namespace Engine
{
	namespace Threading
	{
		struct Message
		{
			Message();

			operator bool() const;

			boost::uint32_t type;
			
			boost::int32_t integralData;
			boost::any arbitraryData;

			template<typename T>
			T arbitrary() const
			{
				return boost::any_cast<T>(arbitraryData);
			}
		};

		class Producer;
		class Channel : boost::noncopyable
		{
			friend class Producer;
		public:
			Channel();
			~Channel();

			Message pop();
			void push(const Message& msg);
			void unattach(Producer * prod);
		private:
			void attach(Producer * prod);

			boost::mutex mutex;
			std::set<Producer *> producers;
			std::queue<Message> messageQueue;
		};

		class Producer : boost::noncopyable
		{
			friend class Channel;
		public:
			Producer();
			~Producer();

			void subscribe(Channel * ch);

			template<typename T>
			void send(boost::uint32_t type, boost::int32_t data, const T& t)
			{
				Message msg;
				msg.type = type;
				msg.integralData = data;
				msg.arbitraryData = t;

				boost::mutex::scoped_lock lock(mutex);
				for (auto it = channels.begin(); it != channels.end(); ++it)
				{
					(*it)->push(msg);
				}
			}
		private:
			void unsubscribe(Channel * ch);

			boost::mutex mutex;
			std::set<Channel *> channels;
		};


		class Bidirectional
			: public Producer
			, public Channel
		{};
	}
}
