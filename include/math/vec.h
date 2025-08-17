#pragma once
#include <math.h>
#include <iostream>


#define PI 3.14159265358979323846f
//VECTOR 2
template<typename T>
struct vector2;
template<typename T>
struct vector3;
template<typename T>
struct vector4;


using bvec2 = vector2<bool>;
using bvec3 = vector3<bool>;
using bvec4 = vector4<bool>;

using vec2 = vector2<float>;
using vec3 = vector3<float>;
using vec4 = vector4<float>;

using ivec2 = vector2<int>;
using ivec3 = vector3<int>;
using ivec4 = vector4<int>;

using uvec2 = vector2<unsigned int>;
using uvec3 = vector3<unsigned int>;
using uvec4 = vector4<unsigned int>;




template<typename T>
struct vector2
{
	T x;
	T y;

	vector2() : x(0), y(0){}
	vector2(const T s): x(s), y(s){}
	vector2(const T x, const T y) : x(x), y(y) {}
	vector2(const vector2<T>& v) : x(v.x), y(v.y){}
	explicit vector2(const vector3<T>& v) : x(v.x), y(v.y) {};
	explicit vector2(const vector4<T>& v) : x(v.x), y(v.y) {};
	template<typename K>
	explicit vector2(const vector2<K>& v): x(static_cast<T>(v.x)), y(static_cast<T>(v.y)){}
	template<typename K>
	vector2& operator=(const vector2<K>& v);
	vector2& operator=(const vector2<T>& v);
	vector2& operator=(const vector3<T>& v);
	vector2& operator=(const vector4<T>& v);
	vector2& operator=(const T s);


	void operator*=(const T s);
	void operator*=(const vector2<T>& v);
	void operator/=(const T s);
	void operator/=(const vector2<T>& b);
	void operator+=(const vector2<T>& v);
	void operator+=(const T s);
	void operator-=(const vector2<T>& v);
	void operator-=(const T s);

	vector2 operator*(const T s) const;
	vector2 operator*(const vector2<T>& v) const;
	vector2 operator/(const T s) const;
	vector2 operator/(const vector2<T>& v) const;
	vector2 operator+(const vector2<T>& v) const;
	vector2 operator+(const T s) const;
	vector2 operator-(const vector2<T>& v) const;
	vector2 operator-(const T s) const;
	bool operator==(const vector2<T>& v) const ;
	bool operator!=(const vector2<T>& v) const ;
	T& operator[](const int index);
};

template<typename T>
struct vector3
{
	T x;
	T y;
	T z;

	vector3() : x(0), y(0), z(0) {}
	vector3(const T s) : x(s), y(s), z(s){}
	vector3(const T x, const T y, const T z) : x(x), y(y), z(z) {}
	vector3(const vector3<T>& v): x(v.x), y(v.y), z(v.z){}
	explicit vector3(const vector2<T>& v) : x(v.x), y(v.y), z(0) {};
	explicit vector3(const vector4<T>& v) : x(v.x), y(v.y), z(v.z) {};
	template<typename K>
	explicit vector3(const vector3<K>& v): x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)){}
	template<typename K>
	vector3& operator=(const vector3<K>& v);
	vector3& operator=(const T s);
	vector3& operator=(const vector3<T>& v);
	vector3& operator=(const vector2<T>& v);
	vector3& operator=(const vector4<T>& v);

	void operator*=(const T s);
	void operator*=(const vector3<T>& v);
	void operator/=(const T s);
	void operator/=(const vector3<T>& v);
	void operator+=(const vector3<T>& v);
	void operator+=(const T s);
	void operator-=(const vector3<T>& v);
	void operator-=(const T s);

	vector3 operator*(const T s) const;
	vector3 operator*(const vector3<T>& v) const;
	vector3 operator/(const T s) const;
	vector3 operator/(const vector3<T>& v) const;
	vector3 operator+(const vector3<T>& v) const;
	vector3 operator+(const T s) const;
	vector3 operator-(const vector3<T>& v) const;
	vector3 operator-(const T s) const;
	bool operator==(const vector3<T>& v) const;
	bool operator!=(const vector3<T>& v) const;
	T& operator[](const int index);
};

template<typename T>
struct vector4
{
	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;

	vector4() : x(0), y(0), z(0), w(0) {}
	vector4(const T s) : x(s), y(s), z(s), w(s) {}
	vector4(const T x, const T y, const T z, const T w) : x(x), y(y), z(z), w(w) {}
	vector4(const vector4<T>& v): x(v.x), y(v.y), z(v.z), w(v.w){}
	explicit vector4(const vector3<T>& v) : x(v.x), y(v.y), z(v.z), w(0) {};
	explicit vector4(const vector2<T>& v) : x(v.x), y(v.y), z(0), w(0) {};
	template<typename K>
	explicit vector4(const vector4<K>& v): x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)), w(static_cast<T>(v.w)) {}
	template<typename K>
	vector4& operator=(const vector4<K>& v);
	vector4& operator=(const T s);
	vector4& operator=(const vector4<T>& v);
	vector4& operator=(const vector2<T>& v);
	vector4& operator=(const vector3<T>& v);

	void operator*=(const T s);
	void operator*=(const vector4<T>& v);
	void operator/=(const T s);
	void operator/=(const vector4<T>& v);
	void operator+=(const vector4<T>& v);
	void operator+=(const T s);
	void operator-=(const vector4<T>& v);
	void operator-=(const T s);

	vector4 operator*(const T s) const;
	vector4 operator*(const vector4<T>& v) const;
	vector4 operator/(const T s) const;
	vector4 operator/(const vector4<T>& s) const;
	vector4 operator+(const vector4<T>& v) const;
	vector4 operator+(const T s) const;
	vector4 operator-(const vector4<T>& v) const;
	vector4 operator-(const T s) const;
	bool operator==(const vector4<T>& v) const;
	bool operator!=(const vector4<T>& v) const;
	T& operator[](const int index);
};

//VECTOR 2
template<typename T>
template<typename K>
inline vector2<T>& vector2<T>::operator=(const vector2<K>& v){x = static_cast<T>(v.x); y = static_cast<T>(v.y); return *this;}
template<typename T>
inline vector2<T>& vector2<T>::operator=(const T s) { x = s; y = s; return *this;}
template<typename T>
inline vector2<T>& vector2<T>::operator=(const vector3<T>& v) { x = v.x; y = v.y; return *this;}
template<typename T>
inline vector2<T>& vector2<T>::operator=(const vector4<T>& v) { x = v.x; y = v.y; return *this;}
template<typename T>
inline vector2<T>& vector2<T>::operator=(const vector2<T>& v) { x = v.x; y = v.y; return *this;}
template<typename T>
inline void vector2<T>::operator*=(const T s)
{
	x *= s;
	y *= s;
}
template<typename T>
inline void vector2<T>::operator*=(const vector2<T>& v)
{
	x *= v.x;
	y *= v.y;
}
template<typename T>
inline void vector2<T>::operator/=(const T s)
{
	x /= s;
	y /= s;
}
template<typename T>
inline void vector2<T>::operator/=(const vector2<T>& v)
{
	x /= v.x;
	y /= v.y;
}
template<typename T>
inline void vector2<T>::operator+=(const vector2<T>& v)
{
	x += v.x;
	y += v.y;
}
template<typename T>
inline void vector2<T>::operator+=(const T s)
{
	x += s;
	y += s;
}
template<typename T>
inline void vector2<T>::operator-=(const vector2<T>& v)
{
	x -= v.x;
	y -= v.y;
}
template<typename T>
inline void vector2<T>::operator-=(const T s)
{
	x -= s;
	y -= s;
}
template<typename T>
inline vector2<T> vector2<T>::operator*(const T s) const
{
	return vector2<T>(x * s, y * s);
}
template<typename T>
inline vector2<T> vector2<T>::operator*(const vector2<T>& v) const
{
	return vector2<T>(x * v.x, y * v.y);
}
template<typename T>
inline vector2<T> vector2<T>::operator/(const T s) const
{
	return vector2<T>(x / s, y / s);
}
template<typename T>
inline vector2<T> vector2<T>::operator/(const vector2<T>& v) const
{
	return vector2<T>(x/v.x, y/v.y);
}
template<typename T>
inline vector2<T> vector2<T>::operator+(const vector2<T>& v) const
{
	return vector2<T>(x + v.x, y + v.y);
}
template<typename T>
inline vector2<T> vector2<T>::operator+(const T s) const
{
	return vector2<T>(x + s, y + s);
}
template<typename T>
inline vector2<T> vector2<T>::operator-(const vector2<T>& v) const
{
	return vector2<T>(x - v.x, y - v.y);
}
template<typename T>
inline vector2<T> vector2<T>::operator-(const T s) const
{
	return vector2<T>(x - s, y - s);
}
template<typename T>
inline bool vector2<T>::operator==(const vector2<T>& v) const
{
	return (x == v.x && y == v.y);
}
template<typename T>
inline bool vector2<T>::operator!=(const vector2<T>& v) const
{
	return !(x == v.x && y == v.y);
}
template<typename T>
inline T& vector2<T>::operator[](const int index)
{
	return(&x)[index];
}


//VECTOR 3
template<typename T>
template<typename K>
inline vector3<T>& vector3<T>::operator=(const vector3<K>& v){ x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = static_cast<T>(v.z); return *this;}
template<typename T>
inline vector3<T>& vector3<T>::operator=(const T s) { x = s; y = s; z = s; return *this;}
template<typename T>
inline vector3<T>& vector3<T>::operator=(const vector3<T>& v) { x = v.x; y = v.y; z = v.z; return *this;}
template<typename T>
inline vector3<T>& vector3<T>::operator=(const vector2<T>& v) { x = v.x; y = v.y; return *this;}
template<typename T>
inline vector3<T>& vector3<T>::operator=(const vector4<T>& v) { x = v.x; y = v.y; z = v.z; return *this;}
template<typename T>
inline void vector3<T>::operator*=(const T s)
{
	x *= s;
	y *= s;
	z *= s;
}
template<typename T>
inline void vector3<T>::operator*=(const vector3<T>& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
}
template<typename T>
inline void vector3<T>::operator/=(const T s)
{
	x /= s;
	y /= s;
	z /= s;
}
template<typename T>
inline void vector3<T>::operator/=(const vector3<T>& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
}
template<typename T>
inline void vector3<T>::operator+=(const vector3<T>& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}
template<typename T>
inline void vector3<T>::operator+=(const T s)
{
	x += s;
	y += s;
	z += s;
}
template<typename T>
inline void vector3<T>::operator-=(const vector3<T>& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}
template<typename T>
inline void vector3<T>::operator-=(const T s)
{
	x -= s;
	y -= s;
	z -= s;
}
template<typename T>
inline vector3<T> vector3<T>::operator*(const T s) const
{
	return vector3<T>(x * s, y * s, z * s);
}
template<typename T>
inline vector3<T> vector3<T>::operator*(const vector3<T>& v) const
{
	return vector3<T>(x * v.x, y * v.y, z * v.z);
}
template<typename T>
inline vector3<T> vector3<T>::operator/(const T s) const
{
	return vector3<T>(x / s, y / s, z / s);
}
template<typename T>
inline vector3<T> vector3<T>::operator/(const vector3<T>& v) const
{
	return vector3<T>(x / v.x, y / v.y, z / v.z);
}
template<typename T>
inline vector3<T> vector3<T>::operator+(const vector3<T>& v) const
{
	return vector3<T>(x + v.x, y + v.y, z + v.z);
}
template<typename T>
inline vector3<T> vector3<T>::operator+(const T s) const
{
	return vector3<T>(x + s, y + s, z + s);
}
template<typename T>
inline vector3<T> vector3<T>::operator-(const vector3<T>& v) const
{
	return vector3<T>(x - v.x, y - v.y, z - v.z);
}
template<typename T>
inline vector3<T> vector3<T>::operator-(const T s) const
{
	return vector3<T>(x - s, y - s, z - s);
}
template<typename T>
inline bool vector3<T>::operator==(const vector3<T>& v) const
{
	return (x == v.x && y == v.y && z == v.z);
}
template<typename T>
inline bool vector3<T>::operator!=(const vector3<T>& v) const
{
	return !(x == v.x && y == v.y && z == v.z);

}
template<typename T>
inline T& vector3<T>::operator[](const int index)
{
	return(&x)[index];
}


//VECTOR 4
template<typename T>
template<typename K>
inline vector4<T>& vector4<T>::operator=(const vector4<K>& v){ x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = static_cast<T>(v.z); w = static_cast<T>(v.w); return *this;}
template<typename T>
inline vector4<T>& vector4<T>::operator=(const T s) { x = s; y = s; z = s; w = s; return *this;}
template<typename T>
inline vector4<T>& vector4<T>::operator=(const vector2<T>& v) { x = v.x; y = v.y; return *this;}
template<typename T>
inline vector4<T>& vector4<T>::operator=(const vector4<T>& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this;}
template<typename T>
inline vector4<T>& vector4<T>::operator=(const vector3<T>& v) { x = v.x; y = v.y; z = v.z; return *this;}
template<typename T>
inline void vector4<T>::operator*=(const T s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
}
template<typename T>
inline void vector4<T>::operator*=(const vector4<T>& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.x;
	w *= v.w;
}
template<typename T>
inline void vector4<T>::operator/=(const T s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
}
template<typename T>
inline void vector4<T>::operator/=(const vector4<T>& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
}
template<typename T>
inline void vector4<T>::operator+=(const vector4<T>& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}
template<typename T>
inline void vector4<T>::operator+=(const T s)
{
	x += s;
	y += s;
	z += s;
	w += s;
}
template<typename T>
inline void vector4<T>::operator-=(const vector4<T>& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}
template<typename T>
inline void vector4<T>::operator-=(const T s)
{
	x -= s;
	y -= s;
	z -= s;
	w -= s;
}
template<typename T>
inline vector4<T> vector4<T>::operator*(const T s) const
{
	return vector4<T>(x * s, y * s, z * s, w * s);
}
template<typename T>
inline vector4<T> vector4<T>::operator*(const vector4<T>& v) const
{
	return vector4<T>(x * v.x, y * v.y, z * v.z, w * v.w);
}
template<typename T>
inline vector4<T> vector4<T>::operator/(const T s) const
{
	return vector4<T>(x / s, y / s, z / s, w / s);
}
template<typename T>
inline vector4<T> vector4<T>::operator/(const vector4<T>& v) const
{
	return vector4<T>(x / v.x, y / v.y, z / v.z, w / v.w);
}
template<typename T>
inline vector4<T> vector4<T>::operator+(const vector4<T>& v) const
{
	return vector4<T>(x + v.x, y + v.y, z + v.z, w + v.w);
}
template<typename T>
inline vector4<T> vector4<T>::operator+(const T s) const
{
	return vector4<T>(x + s, y + s, z + s, w + s);
}
template<typename T>
inline vector4<T> vector4<T>::operator-(const vector4<T>& v) const
{
	return vector4<T>(x - v.x, y - v.y, z - v.z, w - v.w);
}
template<typename T>
inline vector4<T> vector4<T>::operator-(const T s) const
{
	return vector4<T>(x - s, y - s, z - s, w - s);
}
template<typename T>
inline bool vector4<T>::operator==(const vector4<T>& v) const
{
	return (x == v.x && y == v.y && z == v.z && w == v.w);
}
template<typename T>
inline bool vector4<T>::operator!=(const vector4<T>& v) const
{
	return !(x == v.x && y == v.y && z == v.z && w == v.w);
}
template<typename T>
inline T& vector4<T>::operator[](const int index)
{
	return(&x)[index];
}



//Other Operations
template<typename T>
inline T clamp(T value, T min, T max) { return std::min(std::max(value, min), max);}
template<typename T>
inline T mix(T a, T b, float amount) { return a + amount * (b - a); }
template <typename T>
inline T min(T a, T b){return a < b? a: b;}
template <typename T>
inline T max(T a, T b){return a > b? a: b;}
template<typename T>
inline float length(const vector2<T>& v) { return std::hypot(v.x, v.y); }
template<typename T>
inline float length(const vector3<T>& v) { return std::hypot(std::hypot(v.x, v.y), v.z); }
template<typename T>
inline float length(const vector4<T>& v) { return std::hypot(std::hypot(std::hypot(v.x, v.y), v.z), v.w); }

template<typename T>
inline T dot(const vector2<T>& L, const vector2<T>& R) { return L.x * R.x + L.y * R.y; }
template<typename T>
inline T dot(const vector3<T>& L, const vector3<T>& R) { return L.x * R.x + L.y * R.y + L.z * R.z; }
template<typename T>
inline T dot(const vector4<T>& L, const vector4<T>& R) { return L.x * R.x + L.y * R.y + L.z * R.z + L.w * R.w; }
template<typename T>
inline vector3<T> cross(const vector3<T>& a, const vector3<T>& b)
{
	return vector3<T>(
		a.y * b.z - b.y * a.z,
		b.x * a.z - a.x * b.z,
		a.x * b.y - b.x * a.y
		);
}

inline vector2<float> normalize(const vector2<float>& v) { float d = 1.0f/length(v); return v * d; }
inline vector3<float> normalize(const vector3<float>& v) { float d = 1.0f/length(v); return v * d; }
inline vector4<float> normalize(const vector4<float>& v) { float d = 1.0f/length(v); return v * d; }
inline vector2<float> floor(const vector2<float>& v) {return vector2<float>(floorf(v.x), floorf(v.y));}
inline vector3<float> floor(const vector3<float>& v) {return vector3<float>(floorf(v.x), floorf(v.y), floorf(v.z));}
inline vector4<float> floor(const vector4<float>& v) {return vector4<float>(floorf(v.x), floorf(v.y), floorf(v.z), floorf(v.w));}


//NONMEMBER
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const vector2<T>& v) {
	os << '<' << v.x << ", " << v.y << '>';
	return os;
}
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const vector3<T>& v) {
	os << '<' << v.x << ", " << v.y << ", " << v.z << '>';
	return os;
}
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const vector4<T>& v) {
	os << '<' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << '>';
	return os;
}
template<typename K, typename T>
inline vector2<T> operator*(const K s, const vector2<T>& v)
{
	return vector2<T>(s * v.x, s * v.y);
}
template<typename K, typename T>
inline vector3<T> operator*(const K s, const vector3<T>& v)
{
	return vector3<T>(s * v.x, s * v.y, s * v.z);
}
template<typename K, typename T>
inline vector4<T> operator*(const K s, const vector4<T>& v)
{
	return vector4<T>(s * v.x, s * v.y, s * v.z, s * v.w);
}
