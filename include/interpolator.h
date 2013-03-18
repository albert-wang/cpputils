#include <boost/function.hpp>
#include <boost/signals2.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

namespace Engine
{
	namespace Detail
	{
		struct InterpolatorBase
		{
			boost::function<void (float)> tick;
			boost::signals2::signal<void (void)> finish;
			boost::function<float (float)> interpolate;

			void update(float);
			float current;
			float speed;
			float maximum;
		};
	}

	class InterpolatorHandle
	{
	public:
		InterpolatorHandle(boost::weak_ptr<Detail::InterpolatorBase> base);

		void setSpeed(float speed);
		void setDuration(float duration);

		template<typename T>
		InterpolatorHandle& update(const T& t) 
		{
			boost::shared_ptr<Detail::InterpolatorBase> strong = target.lock();
			if (strong) 
			{
				strong->tick = boost::function<void (float)>(t);
			}

			return *this;
		}

		template<typename T>
		InterpolatorHandle& finish(const T& t)
		{
			boost::shared_ptr<Detail::InterpolatorBase> strong = target.lock();
			if (strong) 
			{
				strong->finish.connect(t);
			}

			return *this;
		}

		template<typename T>
		InterpolatorHandle& interpolate(const T& t)
		{
			boost::shared_ptr<Detail::InterpolatorBase> strong = target.lock();
			if (strong) 
			{
				strong->interpolate = boost::function<float (float)>(t);
			}

			return *this;
		}

		void stop();
		void terminate();
	private:
		boost::weak_ptr<Detail::InterpolatorBase> target;
	};

	class InterpolatorCollection
	{
	public:
		/*
		 * This takes in 3 functions, F = [0 - 1] -> float, Step = float -> any, and Finish = () -> any
		 *  and performs Step(F(time)) until a predetermined time has passed, 
		 */

		//The most common methods - lerp, smooth, and timer are provided as first class methods here.
		InterpolatorHandle lerp(float low, float high); 

		void update(float time);
	private:
		boost::mutex pendingMutex;
		std::vector<boost::shared_ptr<Detail::InterpolatorBase>> pending;

		boost::mutex updateMutex;
		std::vector<boost::shared_ptr<Detail::InterpolatorBase>> interpolators;
	};
}
