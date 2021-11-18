#pragma once


// Class to handle miscellaneous functions and data structures needed across multiple classes
class Utility
{
public:
    Utility();
    static inline int GetNumericalPrecisionForUI() { return 2; }; // For setting the numerical precison displayed by UI
    static inline float GetPi() { return 3.1415926535897931; };
    static inline float ToDegrees(float r) { return r * 180.0f / GetPi(); };
    static inline float ToRadians(float d) { return d / 180.0f * GetPi(); };


    template<typename T>
    static T WrapAngle(T theta) noexcept
    {
        const T twoPi = (T)2 * (T)Utility::GetPi();
        const T mod = fmod(theta, twoPi);
        if (mod > (T)Utility::GetPi())
        {
            return mod - twoPi;
        }
        else if (mod < -(T)Utility::GetPi())
        {
            return mod + twoPi;
        }
        return mod;
    }
    
private:

    
};

