#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <functional>

namespace nlm {

class MathUtils {
public:
    // Statistical operations
    template<typename T>
    static T mean(const std::vector<T>& values);
    
    template<typename T>
    static T median(std::vector<T> values);
    
    template<typename T>
    static T standardDeviation(const std::vector<T>& values, T mean);
    
    template<typename T>
    static T variance(const std::vector<T>& values, T mean);
    
    // Array operations
    template<typename T>
    static T sum(const std::vector<T>& values);
    
    template<typename T>
    static T product(const std::vector<T>& values);
    
    template<typename T>
    static T min(const std::vector<T>& values);
    
    template<typename T>
    static T max(const std::vector<T>& values);
    
    template<typename T>
    static std::vector<T> clamp(const std::vector<T>& values, T min, T max);
    
    // Linear algebra
    template<typename T>
    static T dotProduct(const std::vector<T>& a, const std::vector<T>& b);
    
    template<typename T>
    static std::vector<T> normalize(const std::vector<T>& vector, T length = 1.0);
    
    static float sigmoid(float x);
    static float relu(float x);
    static float tanh(float x);
    
    // Probability and randomness
    template<typename T>
    static T interpolate(T start, T end, float alpha);
    
    static float randomFloat(float min = 0.0f, float max = 1.0f);
    static int randomInt(int min, int max);
    
    // Rounding and precision
    static int roundToInt(float value);
    static float roundToFloat(float value, int decimalPlaces);
    
    // Distance calculations
    static float euclideanDistance(const std::vector<float>& a, const std::vector<float>& b);
    static float manhattanDistance(const std::vector<float>& a, const std::vector<float>& b);
    static float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);
    
    // Threshold and binning
    static std::vector<int> binData(const std::vector<float>& data, int numBins);
    static std::vector<float> createThresholds(float min, float max, int numThresholds);

private:
    static std::mt19937 randomGenerator_;
};

} // namespace nlm
