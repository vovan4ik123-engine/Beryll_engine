#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"
#include "beryll/core/Log.h"

namespace Utils
{
    // quaternion(cos(angle/2), sin(angle/2) * axis)

    // w=1 x=0 y=0 z=0 identity quaternion = no rotation

    // for make vector unit:
    // 1.
    // std::sqrt(x*x + y*y + z*z + w*w) - calculate vector magnitude = square root
    // w,x,y,z / square root - divide all components by square root(magnitude)
    // 2.
    // 1 / std::sqrt(x*x + y*y + z*z + w*w) - calculate inverse square root once
    // x * inverse square root - multiply by inverse square root (faster than division)

    // if we need keep vector original magnitude we can make four component vector and store it in fourth component

    // vectors division = quaternion
    // v1 * quaternion = v2
    // v2 / v1 = quaternion

    //http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/#how-do-i-create-a-quaternion-in-c-

    // quaternion::mix(quat1, quat2, 0.5f); // or whatever factor = interpolation between two quaternions

    // quat combined_rotation = applied_second_rotation * applied_first_rotation; - order as for matrices

    class Quaternion
    {
    public:
        Quaternion() = delete;
        ~Quaternion() = delete;

        // normalized linear interpolation. much faster than rotation on sphere
        static aiQuaternion nlerp(aiQuaternion& a, aiQuaternion& b, const float blend)
        {
            // we can check without sqrt if length = 1. because sqrt(1) = 1. Oo
            float aLength = a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w;
            if( !(aLength > 0.9999f && aLength <  1.0001f) )
            {
                a.Normalize();
            }

            float bLength = b.x * b.x + b.y * b.y + b.z * b.z + b.w * b.w;
            if( !(bLength > 0.9999f && bLength <  1.0001f) )
            {
                b.Normalize();
            }

            aiQuaternion result;
            float dotProduct = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
            float oneMinusBlend = 1.0f - blend;

            if (dotProduct < 0.0f)
            {
                result.x = a.x * oneMinusBlend + blend * -b.x;
                result.y = a.y * oneMinusBlend + blend * -b.y;
                result.z = a.z * oneMinusBlend + blend * -b.z;
                result.w = a.w * oneMinusBlend + blend * -b.w;
            }
            else
            {
                result.x = a.x * oneMinusBlend + blend * b.x;
                result.y = a.y * oneMinusBlend + blend * b.y;
                result.z = a.z * oneMinusBlend + blend * b.z;
                result.w = a.w * oneMinusBlend + blend * b.w;
            }

            return result.Normalize();
        }
    };
}