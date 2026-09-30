// Neural Activity Calculation Implementation

#include "NeuralActivityCalculator.hpp"

namespace nlm {

float NeuralActivityCalculator::calculateActivity(const std::vector<float>& values, const ActivityConfig& config) {
    if (values.empty()) return 0.0f;

    float result = 0.0f;

    switch (config.strategy) {
        case ActivityStrategy::MembranePotentialDeviation: {
            float sum = 0.0f;
            for (float value : values) {
                float dev = config.useAbsoluteValue ? std::abs(value) : value;
                sum += dev;
            }
            result = sum / values.size();
            break;
        }

        case ActivityStrategy::AverageFiringRate: {
            result = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
            break;
        }

        case ActivityStrategy::WeightedSum: {
            float sum = 0.0f;
            float weightSum = 0.0f;
            for (size_t i = 0; i < values.size(); ++i) {
                float weight = static_cast<float>(i + 1);
                sum += values[i] * weight;
                weightSum += weight;
            }
            result = weightSum > 0.0f ? sum / weightSum : 0.0f;
            break;
        }

        case ActivityStrategy::MaximumPotential: {
            result = *std::max_element(values.begin(), values.end());
            break;
        }

        case ActivityStrategy::RMS: {
            float sumSquares = 0.0f;
            for (float value : values) {
                sumSquares += value * value;
            }
            result = std::sqrt(sumSquares / values.size());
            break;
        }

        case ActivityStrategy::Variance: {
            float mean = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
            float sumSquares = 0.0f;
            for (float value : values) {
                float diff = value - mean;
                sumSquares += diff * diff;
            }
            result = sumSquares / values.size();
            break;
        }
    }

    result *= config.scaleFactor;

    if (config.threshold > 0.0f) {
        result = std::max(result, config.threshold);
    }

    if (config.clampToRange) {
        result = std::clamp(result, config.clampRange.first, config.clampRange.second);
    }

    return result;
}

float NeuralActivityCalculator::calculateActivity(const std::vector<float>& membranePotentials,
                                                   const std::vector<float>& restingPotentials,
                                                   const ActivityConfig& config) {
    if (membranePotentials.empty()) return 0.0f;

    std::vector<float> deviations;
    deviations.reserve(membranePotentials.size());

    for (size_t i = 0; i < membranePotentials.size(); ++i) {
        float deviation = membranePotentials[i] - 
                         (i < restingPotentials.size() ? restingPotentials[i] : 0.0f);
        deviations.push_back(std::abs(deviation));
    }

    return calculateActivity(deviations, config);
}

float NeuralActivityCalculator::averageActivity(const std::vector<float>& activities) {
    if (activities.empty()) return 0.0f;
    return std::accumulate(activities.begin(), activities.end(), 0.0f) / activities.size();
}

float NeuralActivityCalculator::normalizedActivity(const std::vector<float>& activities, float min, float max) {
    if (activities.empty()) return 0.0f;

    float minActivity = *std::min_element(activities.begin(), activities.end());
    float maxActivity = *std::max_element(activities.begin(), activities.end());

    if (maxActivity == minActivity) return 0.5f;

    float normalized = (activities[0] - minActivity) / (maxActivity - minActivity);
    return std::clamp(normalized, 0.0f, 1.0f);
}

float NeuralActivityCalculator::toFiringRate(float activity, float maxActivity, float maxFiringRate) {
    if (maxActivity <= 0.0f) return 0.0f;
    float rate = activity / maxActivity * maxFiringRate;
    return std::clamp(rate, 0.0f, maxFiringRate);
}

template<typename Iterator>
float NeuralActivityCalculator::rangeBasedActivity(Iterator begin, Iterator end, const ActivityConfig& config) {
    std::vector<float> values(begin, end);
    return calculateActivity(values, config);
}

float NeuralActivityCalculator::clampNeurallyPlausible(float value, float min, float max) {
    return std::clamp(value, min, max);
}

float NeuralActivityCalculator::deviationFromMean(const std::vector<float>& values, float mean) {
    float sum = 0.0f;
    for (float value : values) {
        float diff = value - mean;
        sum += std::abs(diff);
    }
    return sum / values.size();
}

float NeuralActivityCalculator::standardDeviation(const std::vector<float>& values, float mean) {
    if (values.empty()) return 0.0f;

    float sumSquares = 0.0f;
    for (float value : values) {
        float diff = value - mean;
        sumSquares += diff * diff;
    }
    return std::sqrt(sumSquares / values.size());
}

} // namespace nlm
