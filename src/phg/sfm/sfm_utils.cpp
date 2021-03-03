#include "sfm_utils.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>


// pseudorandom number generator
uint64_t xorshift64(uint64_t *state)
{
    if (*state == 0) {
        *state = 1;
    }

    uint64_t x = *state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return *state = x;
}

void phg::randomSample(std::vector<int> &dst, int max_id, int sample_size, uint64_t *state)
{
    dst.clear();

    const int max_attempts = 1000;

    for (int i = 0; i < sample_size; ++i) {
        for (int k = 0; k < max_attempts; ++k) {
            int v = xorshift64(state) % max_id;
            if (dst.empty() || std::find(dst.begin(), dst.end(), v) == dst.end()) {
                dst.push_back(v);
                break;
            }
        }
        if (dst.size() < i + 1) {
            throw std::runtime_error("Failed to sample ids");
        }
    }
}

// проверяет, что расстояние от точки до линии меньше порога
bool phg::epipolarTest(const cv::Vec2d &pt0, const cv::Vec2d &pt1, const cv::Matx33d &F, double t)
{
    const auto l = F * cv::Vec3d(pt0(0), pt0(1), 1.0);          // line from matrix + point
    const auto f = (cv::Vec3d(pt1(0), pt1(1), 1.0).t() * l)(0); // signed distance multiplied by normal vector length
    const auto n2 = (l(0) * l(0) + l(1) * l(1));                // squared normal vector
    
    if (false) {
        std::cout << "calculated signed distance=" 
                << f << " / " << std::sqrt(n2) << " = " << f / std::sqrt(n2)
                << " from line=" << l << " vs " << t << std::endl;
    }
    
    return f * f < t * t * n2;                                  // avoid division and sqrt in comparison
}
