// NLM Utils Implementation
// Math Utilities

#include "core/utils/Utils.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {
namespace utils {

// MathUtils implementation

double MathUtils::clamp(double value, double min, double max) {
    return std::max(min, std::min(max, value));
}

double MathUtils::lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

double MathUtils::mapValue(double value, double inMin, double inMax, double outMin, double outMax) {
    if (inMin == inMax) return outMin;
    return outMin + (outMax - outMin) * ((value - inMin) / (inMax - inMin));
}

double MathUtils::smoothStep(double edge0, double edge1, double x) {
    double t = std::clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

double MathUtils::mean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double MathUtils::median(std::vector<double> values) {
    if (values.empty()) return 0.0;
    size_t n = values.size();
    std::sort(values.begin(), values.end());
    
    if (n % 2 == 0) {
        return (values[n / 2 - 1] + values[n / 2]) / 2.0;
    } else {
        return values[n / 2];
    }
}

double MathUtils::variance(const std::vector<double>& values) {
    if (values.size() <= 1) return 0.0;
    
    double m = mean(values);
    double sum = 0.0;
    for (double x : values) {
        sum += (x - m) * (x - m);
    }
    return sum / (values.size() - 1);
}

double MathUtils::standardDeviation(const std::vector<double>& values) {
    return std::sqrt(variance(values));
}

double MathUtils::percentile(std::vector<double> values, double percentile) {
    if (values.empty()) return 0.0;
    
    percentile = std::clamp(percentile, 0.0, 100.0);
    std::sort(values.begin(), values.end());
    
    double position = (percentile / 100.0) * (values.size() - 1);
    size_t lower = static_cast<size_t>(position);
    double weight = position - lower;
    
    if (lower + 1 < values.size()) {
        return values[lower] * (1.0 - weight) + values[lower + 1] * weight;
    } else {
        return values[lower];
    }
}

double MathUtils::magnitude(const std::vector<double>& vector) {
    double sum = 0.0;
    for (double x : vector) {
        sum += x * x;
    }
    return std::sqrt(sum);
}

double MathUtils::dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
    double sum = 0.0;
    size_t size = std::min(a.size(), b.size());
    for (size_t i = 0; i < size; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

double MathUtils::crossProduct(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() >= 2 && b.size() >= 2) {
        return a[0] * b[1] - a[1] * b[0];
    }
    return 0.0;
}

std::vector<double> MathUtils::normalize(const std::vector<double>& vector) {
    double mag = magnitude(vector);
    if (mag == 0.0) return vector;
    
    std::vector<double> result = vector;
    for (double& x : result) {
        x /= mag;
    }
    return result;
}

double MathUtils::distance(const std::vector<double>& a, const std::vector<double>& b) {
    double sum = 0.0;
    size_t size = std::min(a.size(), b.size());
    for (size_t i = 0; i < size; ++i) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

double MathUtils::randomDouble(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

int MathUtils::randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

bool MathUtils::randomBool(double probability) {
    return randomDouble(0.0, 1.0) < probability;
}

std::vector<int> MathUtils::randomSample(const std::vector<int>& population, 
                                        size_t sampleSize, bool withReplacement) {
    std::vector<int> result = population;
    if (withReplacement) {
        result.clear();
        for (size_t i = 0; i < sampleSize; ++i) {
            result.push_back(randomInt(0, static_cast<int>(population.size()) - 1));
        }
    } else {
        std::shuffle(result.begin(), result.end(), std::mt19937(std::random_device()()));
        if (sampleSize < result.size()) {
            result.resize(sampleSize);
        }
    }
    return result;
}

std::vector<double> MathUtils::normalizeArray(const std::vector<double>& values, 
                                              double targetMean, double targetStd) {
    if (values.empty()) return values;
    
    double currentMean = mean(values);
    double currentStd = standardDeviation(values);
    
    std::vector<double> result;
    for (double x : values) {
        double normalized = (x - currentMean) / (currentStd == 0.0 ? 1.0 : currentStd);
        if (targetMean != 0.0 || targetStd != 1.0) {
            normalized = normalized * targetStd + targetMean;
        }
        result.push_back(normalized);
    }
    
    return result;
}

std::vector<double> MathUtils::smoothArray(const std::vector<double>& values, 
                                           size_t windowSize) {
    if (values.empty() || windowSize <= 1) return values;
    
    std::vector<double> result = values;
    windowSize = std::min(windowSize, values.size() / 2 + 1);
    
    for (size_t i = 0; i < values.size(); ++i) {
        size_t left = std::max(size_t(0), i - windowSize / 2);
        size_t right = std::min(values.size(), i + windowSize / 2 + 1);
        
        double sum = 0.0;
        for (size_t j = left; j < right; ++j) {
            sum += values[j];
        }
        
        result[i] = sum / (right - left);
    }
    
    return result;
}

double MathUtils::calculateMovingAverage(const std::vector<double>& values, 
                                        size_t windowSize) {
    if (values.empty() || windowSize == 0) return 0.0;
    
    windowSize = std::min(windowSize, values.size());
    double sum = 0.0;
    for (size_t i = 0; i < windowSize; ++i) {
        sum += values[values.size() - 1 - i];
    }
    
    return sum / windowSize;
}

double MathUtils::sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

double MathUtils::relu(double x) {
    return std::max(0.0, x);
}

double MathUtils::elu(double x, double alpha) {
    return (x >= 0.0) ? x : alpha * (std::exp(x) - 1.0);
}

double MathUtils::softplus(double x) {
    return std::log(1.0 + std::exp(x));
}

double MathUtils::gaussian(double x, double mean, double sigma) {
    double coeff = 1.0 / (std::sqrt(2.0 * M_PI) * sigma);
    double exponent = -0.5 * std::pow((x - mean) / sigma, 2.0);
    return coeff * std::exp(exponent);
}

} // namespace utils
} // namespace nlm
