#include <boost/preprocessor.hpp>
#include <cassert>
#include <cmath>
#include <iostream>

#pragma once
#pragma warning(push)
#pragma warning(disable: 4127) //Conditional expression is constant.

class Matrix2;
class Matrix3;
class Matrix4;

bool fequals(float, float);

#define VECTOR_MATH_VEC_ELEM(r, op, elem) \
	elem op other. elem;

#define VECTOR_MATH_SCALAR_ELEM(r, op, elem) \
	elem op other;

#define VECTOR_COMPARE_ELEM(r, op, elem) \
	result = result && (elem op other. elem);

#define INVOKE(macro, n, tuple, data) \
	BOOST_PP_SEQ_FOR_EACH(macro, data, BOOST_PP_TUPLE_TO_SEQ(n, tuple))

#define INVOKE2(macro, data) \
	INVOKE(macro, 2, (x, y), data)

#define INVOKE3(macro, data) \
	INVOKE(macro, 3, (x, y, z), data)

#define INVOKE4(macro, data) \
	INVOKE(macro, 3, (x, y, z), data)

#define MATH_OP(n, op) \
			BOOST_PP_CAT(GVector, n)& operator op(const BOOST_PP_CAT(GVector, n)& other) { BOOST_PP_CAT(INVOKE, n)(VECTOR_MATH_VEC_ELEM, op); return *this; } \
			BOOST_PP_CAT(GVector, n)& operator op(T other) { BOOST_PP_CAT(INVOKE, n)(VECTOR_MATH_SCALAR_ELEM, op); return *this; }

#define LOGIC_OP(n, op) \
			bool operator op(const BOOST_PP_CAT(GVector, n)& other) { bool result = true; BOOST_PP_CAT(INVOKE, n)(VECTOR_COMPARE_ELEM, op); return result; }

#define INDEX(n) \
			T& operator[](unsigned int i) { assert(i < n); return this->*ComponentPointers[i]; } \
			const T& operator[](unsigned int i) const { assert(i < n); return this->*ComponentPointers[i]; }

namespace Math
{
	template<typename T>
	class GVector2
	{
	public:
		typedef T GVector2<T>::*MemberPointer;

		GVector2()
			:x(T())
			,y(T())
		{}

		GVector2(T x, T y)
			:x(x)
			,y(y)
		{}

		MATH_OP(2, +=)
		MATH_OP(2, -=)
		MATH_OP(2, *=)
		MATH_OP(2, /=)

		LOGIC_OP(2, !=)
		LOGIC_OP(2, ==)
		LOGIC_OP(2, >)
		LOGIC_OP(2, >=)
		LOGIC_OP(2, <)
		LOGIC_OP(2, <=)

		INDEX(2)

		float magnitude() const
		{
			T squared = dot(*this, *this);
			return std::sqrt(static_cast<float>(squared));
		}

		void normalize() const
		{
			float mag = magnitude();
			x /= mag;
			y /= mag;
		}

		T * data() const
		{
			return &x;
		}

		T x;
		T y;
	private:
		static const MemberPointer ComponentPointers[2];;
	};

	template<typename T>
	const typename GVector2<T>::MemberPointer GVector2<T>::ComponentPointers[2] = { &GVector2<T>::x, &GVector2<T>::y };

	template<typename T>
	class GVector3
	{
	public:
		typedef T GVector3<T>::*MemberPointer;

		GVector3()
			:x(T())
			,y(T())
			,z(T())
		{}

		GVector3(const GVector2<T>& a, T z)
			:x(a.x)
			,y(a.y)
			,z(z)
		{}

		GVector3(T x, const GVector2<T>& a)
			:x(x)
			,y(a.x)
			,z(a.y)
		{}

		GVector3(T x, T y, T z)
			:x(x)
			,y(y)
			,z(z)
		{}

#ifdef USE_BULLET_CONVERSIONS
		//This is implicit on purpose.
		GVector3(const btVector3& in)
			:x(in.getX())
			,y(in.getY())
			,z(in.getZ())
		{}

		operator btVector3() const
		{
			assert(adjustedLen == 3);
			return btVector3(x, y, z);
		}
#endif

		MATH_OP(3, +=)
		MATH_OP(3, -=)
		MATH_OP(3, *=)
		MATH_OP(3, /=)

		LOGIC_OP(3, !=)
		LOGIC_OP(3, ==)
		LOGIC_OP(3, >)
		LOGIC_OP(3, >=)
		LOGIC_OP(3, <)
		LOGIC_OP(3, <=)

		INDEX(3)

		float magnitude() const
		{
			T squared = dot(*this, *this);
			return std::sqrt(static_cast<float>(squared));
		}

		void normalize() const
		{
			float mag = magnitude();
			x /= mag;
			y /= mag;
			z /= mag;
		}

		GVector3 perpendicularTo() const
		{
			GVector3 res = cross(*this, GVector3<T>(0, 0, 1));
			if (dot(res, res) == 0)
			{
				res = cross(*this, GVector3<T>(0, 1, 0));
			}

			return res;
		}

		T * data() const
		{
			return &x;
		}

		T x;
		T y;
		T z;
	private:
		static const MemberPointer ComponentPointers[3];
	};

	template<typename T>
	const typename GVector3<T>::MemberPointer GVector3<T>::ComponentPointers[3] = { &GVector3<T>::x, &GVector3<T>::y, &GVector3<T>::z };

	template<typename T>
	class GVector4
	{
	public:
		typedef T GVector4<T>::*MemberPointer;

		GVector4()
			:x(T())
			,y(T())
			,z(T())
			,w(T(1))
		{}

		GVector4(const GVector3<T>& a, T w)
			:x(a.x)
			,y(a.y)
			,z(a.z)
			,w(w)
		{}

		GVector4(T x, const GVector3<T>& a)
			:x(x)
			,y(a.x)
			,z(a.y)
			,w(a.z)
		{}

		GVector4(const GVector2<T>& a, T z, T w = T(1))
			:x(a.x)
			,y(a.y)
			,z(z)
			,w(w)
		{}

		GVector4(T x, const GVector2<T>& a, T w = T(1))
			:x(x)
			,y(a.x)
			,z(a.y)
			,w(w)
		{}

		GVector4(T x, T y, const GVector2<T>& a)
			:x(x)
			,y(y)
			,z(a.x)
			,w(a.y)
		{}

		GVector4(T x, T y, T z, T w = T(1))
			:x(x)
			,y(y)
			,z(z)
			,w(w)
		{}


#ifdef USE_BULLET_CONVERSIONS
		//This is implicit on purpose.
		GVector4(const btVector3& in)
			:x(in.getX())
			,y(in.getY())
			,z(in.getZ())
			,w(T(1))
		{}

		operator btVector3() const
		{
			assert(adjustedLen == 3);
			return btVector3(x, y, z);
		}
#endif

		MATH_OP(4, +=)
		MATH_OP(4, -=)
		MATH_OP(4, *=)
		MATH_OP(4, /=)

		LOGIC_OP(4, !=)
		LOGIC_OP(4, ==)
		LOGIC_OP(4, >)
		LOGIC_OP(4, >=)
		LOGIC_OP(4, <)
		LOGIC_OP(4, <=)

		INDEX(4)

		float magnitude() const
		{
			T squared = dot(*this, *this);
			return std::sqrt(static_cast<float>(squared));
		}

		void normalize() const
		{
			float mag = magnitude();
			x /= mag;
			y /= mag;
			z /= mag;
		}

		GVector4 perpendicularTo() const
		{
			GVector4 res = cross(*this, GVector4<T>(0, 0, 1));
			if (dot(res, res) == 0)
			{
				res = cross(*this, GVector4<T>(0, 1, 0));
			}

			return res;
		}

		T * data() const
		{
			return &x;
		}

		T x;
		T y;
		T z;
		T w;
	private:
		static const MemberPointer ComponentPointers[4];
	};

	template<typename T>
	const typename GVector4<T>::MemberPointer GVector4<T>::ComponentPointers[4] = { &GVector4<T>::x, &GVector4<T>::y, &GVector4<T>::z, &GVector4<T>::w };

	template<typename T, unsigned int i>
	struct VectorTrait
	{};

	template<typename T>
	struct VectorTrait<T, 2>
	{
		typedef GVector2<T> type;
	};

	template<typename T>
	struct VectorTrait<T, 3>
	{
		typedef GVector3<T> type;
	};

	template<typename T>
	struct VectorTrait<T, 4>
	{
		typedef GVector4<T> type;
	};

	//Free operators
#define FREE_OPERATOR(op, accop) 					\
	template<typename T, unsigned int L>			\
	typename VectorTrait<T, L>::type operator op(const typename VectorTrait<T, L>::type& a, const typename VectorTrait<T, L>::type& b) \
	{												\
		typename VectorTrait<T, L>::type result(a);	\
		result accop b;								\
		return result;								\
	}												\
	template<typename T, unsigned int L>					\
	typename VectorTrait<T, L>::type operator op(const typename VectorTrait<T, L>::type& a, T b) \
	{												\
		typename VectorTrait<T, L>::type result(a);	\
		result accop b;								\
		return result;								\
	}

	FREE_OPERATOR(+, +=)
	FREE_OPERATOR(-, -=)
	FREE_OPERATOR(*, *=)
	FREE_OPERATOR(/, /=)

	//Multiply and Add have T, Vec<T> implementations too.
	template<typename T, unsigned int L>
	typename VectorTrait<T, L>::type operator+(T b, const typename VectorTrait<T, L>::type& a)
	{
		typename VectorTrait<T, L>::type result(a);
		result += b;
		return result;
	}

	template<typename T, unsigned int L>
	typename VectorTrait<T, L>::type operator*(T b, const typename VectorTrait<T, L>::type& a)
	{
		typename VectorTrait<T, L>::type result(a);
		result *= b;
		return result;
	}

	template<typename T, unsigned int L>
	typename VectorTrait<T, L>::type unit(const typename VectorTrait<T, L>::type& a)
	{
		typename VectorTrait<T, L>::type result(a);
		result.normalize();
		return result;
	}

	template<typename T>
	T dot(const GVector2<T>& a, const GVector2<T>& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	template<typename T>
	T dot(const GVector3<T>& a, const GVector3<T>& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	template<typename T>
	T dot(const GVector4<T>& a, const GVector4<T>& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	template<typename T>
	GVector3<T> cross(const GVector3<T>& a, const GVector3<T>& b)
	{
		T x = a.y * b.z - a.z * b.y;
		T y = a.z * b.x - a.x * b.z;
		T z = a.x * b.y - a.y * b.x;

		return GVector3<T>(x, y, z);
	}

	template<typename T>
	GVector4<T> cross(const GVector4<T>& a, const GVector4<T>& b)
	{
		T x = a.y * b.z - a.z * b.y;
		T y = a.z * b.x - a.x * b.z;
		T z = a.x * b.y - a.y * b.x;

		return GVector4<T>(x, y, z, 1);
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& o, const GVector2<T>& a)
	{
		o << "[" << a.x <<", " << a.y << "]";
		return o;
	}
}


typedef Math::GVector2<float> Vector2;
typedef Math::GVector3<float> Vector3;
typedef Math::GVector4<float> Vector4;
