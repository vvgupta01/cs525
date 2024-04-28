#include "utils.h"

#include <assert.h>

#include <algorithm>
#include <numeric>

std::random_device rd;
std::mt19937 gen(rd());

bool rand_bool() {
    return rand() > RAND_MAX / 2;
}

int rand_uniform(int min, int max) {
    auto dist = std::uniform_int_distribution(min, max);
    return dist(gen);
}

std::discrete_distribution<> get_rand_discrete(std::vector<uint32_t>& weights) {
    return std::discrete_distribution<>(weights.begin(), weights.end());
}

int sample_rand_discrete(std::discrete_distribution<>& dist) {
    return dist(gen);
}

matrix generate_uniform_demands(uint32_t N, uint32_t T, uint32_t max_demand) {
    matrix demands(T, std::vector<uint32_t>(N));

    for (uint32_t t = 0; t < T; ++t) {
        for (uint32_t i = 0; i < N; ++i) {
            demands[t][i] = rand_uniform(0, (int)max_demand);
        }
    }
    return demands;
}

std::vector<float> welfares(matrix& demands, matrix& allocations) {
    uint32_t N = demands[0].size();
    std::vector<float> welfares(N);

    for (uint32_t i = 0; i < N; ++i) {
        uint64_t used = 0, total_demand = 0;
        for (uint32_t t = 0; t < demands.size(); ++t) {
            used += std::min(demands[t][i], allocations[t][i]);
            total_demand += demands[t][i];
        }
        welfares[i] = total_demand > 0 ? (float)used / total_demand : 1;
    }
    return welfares;
}

std::vector<float> welfares(matrix& demands, matrix& allocations,
                            matrix& payments, fi valuation) {
    uint32_t N = demands[0].size();
    std::vector<float> welfares(N);

    for (uint32_t i = 0; i < N; ++i) {
        uint64_t actual = 0, expected = 0;
        for (uint32_t t = 0; t < demands.size(); ++t) {
            actual += std::min(demands[t][i], allocations[t][i] * valuation(demands[t][i]) / payments[t][i]);
            expected += demands[t][i];
        }
        welfares[i] = expected > 0 ? (float)actual / expected : 1;
    }
    return welfares;
}

float fairness(std::vector<float>& welfares, size_t si) {
    auto minmax = std::minmax_element(welfares.begin() + si, welfares.end());
    if (*minmax.second == 0) {
        return 1;
    }
    return *minmax.first / *minmax.second;
}

// float fairness(matrix& demands, matrix& allocations) {
//     uint64_t min_used = std::numeric_limits<uint64_t>::max(), max_used = 0;

//     for (uint32_t i = 0; i < demands[0].size(); ++i) {
//         uint64_t used = 0;
//         for (uint32_t t = 0; t < demands.size(); ++t) {
//             used += std::min(demands[t][i], allocations[t][i]);
//         }
//         min_used = std::min(min_used, used);
//         max_used = std::max(max_used, used);
//     }
//     return max_used > 0 ? (float)min_used / max_used : 1;
// }

float instant_fairness(std::vector<uint32_t>& demands, std::vector<uint32_t>& allocations, size_t si) {
    float min_welfare = 1, max_welfare = 0;
    for (uint32_t i = si; i < demands.size(); ++i) {
        float welfare = 1;
        if (demands[i] > 0) {
            welfare = (float)std::min(demands[i], allocations[i]) / demands[i];
        }

        min_welfare = std::min(min_welfare, welfare);
        max_welfare = std::max(max_welfare, welfare);
    }
    return max_welfare > 0 ? min_welfare / max_welfare : 1;
}

float instant_fairness(std::vector<uint32_t>& demands, std::vector<uint32_t>& allocations,
                       std::vector<uint32_t>& payments, fi valuation, size_t si) {
    float min_welfare = 1, max_welfare = 0;
    for (uint32_t i = si; i < demands.size(); ++i) {
        float welfare = 1;
        if (demands[i] > 0) {
            uint32_t val = std::min(demands[i], allocations[i]) * (float)valuation(demands[i]) / payments[i];
            welfare = (float)std::min(demands[i], val) / demands[i];
        }

        min_welfare = std::min(min_welfare, welfare);
        max_welfare = std::max(max_welfare, welfare);
    }
    return max_welfare > 0 ? min_welfare / max_welfare : 1;
}

float utilization(std::vector<uint32_t>& demands, std::vector<uint32_t>& allocations, uint64_t blocks) {
    assert(blocks > 0);

    uint64_t used = 0;
    for (uint32_t i = 0; i < demands.size(); ++i) {
        used += std::min(demands[i], allocations[i]);
    }
    return (float)used / blocks;
}

float range_average(std::vector<float>& arr, size_t a, size_t b) {
    assert(b >= a);

    float sum = 0.0;
    for (size_t i = a; i < b; ++i) {
        sum += arr[i];
    }
    return b > a ? sum / (b - a) : 0;
}