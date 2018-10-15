
#pragma once

#include <cmath>

struct float3
{
	float	x, y, z;
};

inline float3 operator + (const float3 &a, const float3 &b)
{ return {a.x+b.x, a.y+b.y, a.z+b.z}; }

inline float3 operator - (const float3 &a, const float3 &b)
{ return {a.x-b.x, a.y-b.y, a.z-b.z}; }

inline float3 operator * (const float3 &a, const float &b)
{ return {a.x*b, a.y*b, a.z*b}; }

inline float3 operator * (const float &a, const float3 &b)
{ return {a*b.x, a*b.y, a*b.z}; }

inline float3 operator * (const float3 &a, const float3 &b)
{ return {a.x*b.x, a.y*b.y, a.z*b.z}; }

inline float3 operator / (const float3 &a, const float &b)
{ return {a.x/b, a.y/b, a.z/b}; }

inline float3 operator / (const float &a, const float3 &b)
{ return {a/b.x, a/b.y, a/b.z}; }

inline float3 operator / (const float3 &a, const float3 &b)
{ return {a.x/b.x, a.y/b.y, a.z/b.z}; }

inline float dot (const float3 &a, const float3 &b)
{ return a.x*b.x + a.y*b.y + a.z*b.z; }

inline float length2 (const float3 &a)
{ return dot (a,a); }

inline float length (const float3 &a)
{ return std::sqrt (dot (a,a)); }

inline float distance (const float3 &a, const float3 &b)
{ return length (b-a); }

inline float3 normalize (const float3 &a)
{
	float n = length (a);
	return n > 0.0f ? a/n : float3 {0.0f,0.0f,0.0f};
}

inline float3 cross (const float3 &a, const float3 &b)
{ return { a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x }; }

inline float component (const float3 &a, int comp)
{ return ((const float*)(&a.x))[comp]; }
