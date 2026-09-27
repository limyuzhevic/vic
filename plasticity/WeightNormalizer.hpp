#pragma once

#include "WeightNormalizer.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>

namespace nlm {

class WeightNormalizer {
public:
    struct NormalizationParams {
        float minWeight{-1.0f};
        float maxWeight{1.0f};
        float targetEIRatio{1.0f};
        bool preserveSign{true};
        float convergenceThreshold{1e-6f};
        size_t maxIterations{100};
    };

    WeightNormalizer();
    ~WeightNormalizer();

    // Disable copying, enable moving
    WeightNormalizer(const WeightNormalizer&) = delete;
    WeightNormalizer& operator=(const WeightNormalizer&) = delete;
    WeightNormalizer(WeightNormalizer&&) noexcept;
    WeightNormalizer& operator=(WeightNormalizer&&) noexcept;

    // Configuration
    void configure(const NormalizationParams& params);
    const NormalizationParams& getParams() const;

    // Normalization methods
    float normalizeWeight(float weight) const;
    void normalizeWeights(std::vector<float>& weights);
    float normalizeForEIRatio(const std::vector<float>& weights, float targetRatio) const;
    void normalizeNetwork(std::vector<std::vector<float>>& adjacencyMatrix);

    // Advanced normalization
    void normalizeEigenvalue(const std::vector<std::vector<float>>& adjacencyMatrix,
                             std::vector<std::vector<float>>& normalizedMatrix);
    void normalizeSpectralRadius(const std::vector<std::vector<float>>& adjacencyMatrix,
                                 std::vector<std::vector<float>>& normalizedMatrix,
                                 float targetRadius = 0.9f);

    // Optimization
    void enableAdaptiveNormalization(bool enable);
    void setNormalizationPrecision(float precision);
    bool checkConvergence(const std::vector<float>& weights) const;

    // Performance
    size_t getNormalizationCount() const;
    double getAverageNormalizationTime() const;
    std::string getStatistics() const;

    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
