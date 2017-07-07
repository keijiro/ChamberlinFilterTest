#pragma once

#include <cmath>

class ChamberlinFilter
{
private:

    float Fs_;
    float F_;
    float D_;
    float HP_;
    float Z1_;
    float Z2_;

public:

    ChamberlinFilter() : HP_(0), Z1_(0), Z2_(0) {}

    void initialize(float freq, float q, float sampleRate)
    {
        Fs_ = sampleRate;
        setCutoffFreq(freq, q);
    }

    float getLowPassOut() const { return Z2_; }
    float getBandPassOut() const { return Z1_; }
    float getHighPassOut() const { return HP_; }

    void setCutoffFreq(float freq, float q)
    {
        float Fc = 2.0f * std::sinf(3.14159265359f * freq / Fs_);
        Fc = std::fmaxf(0.0001f, std::fminf(1.0f, Fc));
        D_ = std::fminf(1.0f / q, 2.0f - Fc);
        F_ = (1.85f - 0.85f * D_ * Fc) * Fc;
    }

    void feedSample(float i)
    {
        float hp = i - Z1_ * D_ - Z2_;
        float bp = hp * F_ + Z1_;
        float lp = bp * F_ + Z2_;
        HP_ = hp;
        Z1_ = bp;
        Z2_ = lp;
    }
};
