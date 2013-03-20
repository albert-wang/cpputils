#include "channel.h"

namespace Engine
{
	namespace Threading
	{
		Message::Message()
			:type(0)
			,integralData(0)
		{}

		Message::operator bool() const
		{
			return type != 0;
		}

		Channel::Channel()
		{}

		Channel::~Channel()
		{
			boost::mutex::scoped_lock lock(mutex);

			std::set<Producer *> cpy = producers;
			producers.clear();

			for (auto it = cpy.begin(); it != cpy.end(); ++it)
			{
				(*it)->unsubscribe(this);
			}
		}

		void Channel::attach(Producer * prod)
		{
			boost::mutex::scoped_lock lock(mutex);
			producers.insert(prod);
		}

		void Channel::unattach(Producer * prod)
		{
			boost::mutex::scoped_lock lock(mutex);
			producers.erase(prod);
		}

		Message Channel::pop() 
		{
			Message msg;
			boost::mutex::scoped_lock lock(mutex);
			if (messageQueue.empty())
			{
				return msg;
			}
			else
			{
				Message result = messageQueue.front();
				messageQueue.pop();
				return result;
			}
		}

		void Channel::push(const Message& msg)
		{
			boost::mutex::scoped_lock lock(mutex);
			messageQueue.push(msg);
		}

		Producer::Producer()
		{}

		Producer::~Producer()
		{
			boost::mutex::scoped_lock lock(mutex);
			for (auto it = channels.begin(); it != channels.end(); ++it)
			{
				(*it)->unattach(this);
			}
		}

		void Producer::subscribe(Channel * ch)
		{
			boost::mutex::scoped_lock lock(mutex);
			ch->attach(this);
			channels.insert(ch);
		}

		void Producer::unsubscribe(Channel * ch)
		{
			boost::mutex::scoped_lock lock(mutex);
			channels.erase(ch);
		}
	}
}
