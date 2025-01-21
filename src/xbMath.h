#ifndef XBMATH_H // include guard begin
#define XBMATH_H // include guard

#include "constants.h"

#include <stdint.h> // defines fixed size types, C++ version is <cstdint>

//NOTE[ALEX]: declare small inline functions in the header

inline uint32_t safeTruncateUInt64(uint64_t value)
{
    xbAssert(value <= 0xFFFFFFFF);
    return (uint32_t)value;
}

inline int32_t roundF32toI32(float value)
{
    value += 0.5f;
    return (int32_t)value;
}

inline uint32_t roundF32toU32(float value)
{
    value += 0.5f;
    return (uint32_t)value;
}

inline int32_t minI32(int32_t a, int32_t b)
{
    if (a < b) { return a; }
    else       { return b; }
}

inline int32_t maxI32(int32_t a, int32_t b)
{
    if (a > b) { return a; }
    else       { return b; }
}

inline uint8_t maxU8(uint8_t a, uint8_t b)
{
    if (a > b) { return a; }
    else       { return b; }
}

inline float minF32(float a, float b)
{
    if (a < b) { return a; }
    else       { return b; }
}

inline float maxF32(float a, float b)
{
    if (a > b) { return a; }
    else       { return b; }
}

inline float clampF32(float value, float min, float max)
{
    value = minF32(value, max);
    value = maxF32(value, min);
    return value;
}

inline uint8_t lerpU8(uint8_t a, uint8_t b, float t)
{
    t = clampF32(t, 0.0f, 1.0f);
    return a * (1.0f-t) + b * t;
}

inline uint32_t lerpU32(uint32_t a, uint32_t b, float t)
{
    t = clampF32(t, 0.0f, 1.0f);
    return a * (1.0f-t) + b * t;
}

#endif // include guard end
