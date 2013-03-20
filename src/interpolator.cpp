#include "interpolator.h"
#include <cassert>

namespace Engine
{
	namespace Detail
	{
		void InterpolatorBase::update(float dt) 
		{
			assert(interpolate);
			if (current == -1.0f)
			{
				//This is an error.
				return;
			}

			if (current >= maximum) 
			{
				float result = interpolate(maximum);
				if (tick) 
				{
					tick(result);
				}

				finish();

				current = -1.0f;
				return;
			}

			float result = interpolate(current);
			if (tick)
			{
				tick(result);
			}

			current += speed * dt;
		}
	}

	struct Lerp
	{
		float operator()(float current)
		{
			return low + (high - low) * current;
		}

		float low;
		float high;
	};

	InterpolatorHandle::InterpolatorHandle(boost::weak_ptr<Detail::InterpolatorBase> base)
		:target(base)
	{}

	void InterpolatorHandle::setSpeed(float spd)
	{
		boost::shared_ptr<Detail::InterpolatorBase> strong = target.lock();
		if (strong) 
		{
			strong->speed = spd;
		}
	}

	void InterpolatorHandle::setDuration(float dur)
	{
		boost::shared_ptr<Detail::InterpolatorBase> strong = target.lock();
		if (strong) 
		{
			strong->speed = strong->maximum / dur;
		}
	}

	void InterpolatorHandle::stop() 
	{
		boost::shared_ptr<Detail::InterpolatorBase> strong = target.lock();
		if (strong) 
		{
			strong->current = strong->maximum;
		}
	}

	void InterpolatorHandle::terminate()
	{
		boost::shared_ptr<Detail::InterpolatorBase> strong = target.lock();
		if (strong) 
		{
			strong->current = -1.0f;
		}
	}

	bool InterpolatorHandle::valid() const
	{
		return !!target.lock();
	}

	InterpolatorHandle InterpolatorCollection::lerp(float low, float high) 
	{
		Lerp lerp;
		lerp.low = low;
		lerp.high = high;

		boost::shared_ptr<Detail::InterpolatorBase> target = boost::make_shared<Detail::InterpolatorBase>();
		target->interpolate = lerp;
		target->current = 0.0f;
		target->maximum = 1.0f;
		target->speed = 1.0f;
		
		{
			boost::mutex::scoped_lock lock(pendingMutex);
			pending.push_back(target);
		}

		InterpolatorHandle handle(target);
		handle.update(lerp);

		return handle;
	}

	void InterpolatorCollection::update(float time)
	{
		{
			boost::mutex::scoped_lock lock(updateMutex);
			std::copy(pending.begin(), pending.end(), std::back_inserter(interpolators));
			pending.clear();
		}

		for (size_t i = 0; i < interpolators.size(); ++i)
		{
			interpolators[i]->update(time);
		}
	}
}

