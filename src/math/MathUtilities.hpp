#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <numeric>

namespace nlm {

/**
 * Mathematical utilities for neural network operations
 * Provides centralized implementations of common mathematical functions
 * to eliminate code duplication across the codebase
 */
class MathUtilities {
public:
    // Static methods - no instance required
    static MathUtilities& getInstance() {
        static MathUtilities instance;
        return instance;
    }

    /**
     * Compute cosine similarity between two vectors
     * 
     * @param a First vector
     * @param b Second vector
     * @return Cosine similarity value between -1.0 and 1.0
     * @throws std::invalid_argument if vectors have different sizes
     */
    static double cosineSimilarity(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size for cosine similarity");
        }
        
        if (a.empty()) {
            return 0.0;
        }
        
        double dotProduct = 0.0;
        double normA = 0.0;
        double normB = 0.0;
        
        for (size_t i = 0; i < a.size(); ++i) {
            dotProduct += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        if (normA < 1e-12 || normB < 1e-12) {
            return 0.0;
        }
        
        return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
    }

    /**
     * Compute Euclidean distance between two vectors
     * 
     * @param a First vector
     * @param b Second vector
     * @return Euclidean distance
     */
    static double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size for Euclidean distance");
        }
        
        if (a.empty()) {
            return 0.0;
        }
        
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            double diff = a[i] - b[i];
            sum += diff * diff;
        }
        
        return std::sqrt(sum);
    }

    /**
     * Compute Manhattan distance between two vectors
     * 
     * @param a First vector
     * @param b Second vector
     * @return Manhattan distance
     */
    static double manhattanDistance(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size for Manhattan distance");
        }
        
        double distance = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            distance += std::abs(a[i] - b[i]);
        }
        
        return distance;
    }

    /**
     * Normalize a vector to unit length
     * 
     * @param input Vector to normalize
     * @return Normalized vector
     */
    static std::vector<double> normalizeVector(const std::vector<double>& input) {
        if (input.empty()) {
            return input;
        }
        
        double norm = 0.0;
        for (double value : input) {
            norm += value * value;
        }
        
        norm = std::sqrt(norm);
        if (norm < 1e-12) {
            return input; // Zero vector, return as-is
        }
        
        std::vector<double> result;
        result.reserve(input.size());
        for (double value : input) {
            result.push_back(value / norm);
        }
        
        return result;
    }

    /**
     * Compute dot product of two vectors
     * 
     * @param a First vector
     * @param b Second vector
     * @return Dot product
     */
    static double dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size for dot product");
        }
        
        return std::inner_product(a.begin(), a.end(), b.begin(), 0.0);
    }

    /**
     * Compute L2 norm (Euclidean norm) of a vector
     * 
     * @param input Vector
     * @return L2 norm
     */
    static double vectorNorm(const std::vector<double>& input) {
        return std::sqrt(std::inner_product(input.begin(), input.end(), input.begin(), 0.0));
    }

    /**
     * Clamp values in a vector to specified range
     * 
     * @param input Vector to clamp
     * @param min Minimum value
     * @param max Maximum value
     * @return Clamped vector
     */
    static std::vector<double> clampVector(const std::vector<double>& input, double min, double max) {
        std::vector<double> result;
        result.reserve(input.size());
        
        for (double value : input) {
            result.push_back(std::max(min, std::min(value, max)));
        }
        
        return result;
    }

    /**
     * Compute sigmoid function for a scalar value
     * 
     * @param x Input value
     * @return Sigmoid(x) = 1 / (1 + exp(-x))
     */
    static double sigmoid(double x) {
        return 1.0 / (1.0 + std::exp(-x));
    }

    /**
     * Compute derivative of sigmoid function
     * 
     * @param x Input value
     * @return Derivative of sigmoid
     */
    static double sigmoidDerivative(double x) {
        double s = sigmoid(x);
        return s * (1.0 - s);
    }

    /**
     * Compute mean of a vector
     * 
     * @param input Vector
     * @return Mean value
     */
    static double vectorMean(const std::vector<double>& input) {
        if (input.empty()) {
            return 0.0;
        }
        return std::accumulate(input.begin(), input.end(), 0.0) / input.size();
    }

    /**
     * Compute standard deviation of a vector
     * 
     * @param input Vector
     * @return Standard deviation
     */
    static double vectorStdDev(const std::vector<double>& input) {
        if (input.size() <= 1) {
            return 0.0;
        }
        
        double mean = vectorMean(input);
        double sumSq = 0.0;
        for (double value : input) {
            sumSq += (value - mean) * (value - mean);
        }
        
        return std::sqrt(sumSq / (input.size() - 1));
    }

    /**
     * Check if two vectors are approximately equal
     * 
     * @param a First vector
     * @param b Second vector
     * @param tolerance Tolerance for equality
     * @return True if vectors are approximately equal
     */
    static bool vectorsApproxEqual(const std::vector<double>& a, const std::vector<double>& b, double tolerance = 1e-10) {
        if (a.size() != b.size()) {
            return false;
        }
        
        for (size_t i = 0; i < a.size(); ++i) {
            if (std::abs(a[i] - b[i]) > tolerance) {
                return false;
            }
        }
        
        return true;
    }

    /**
     * Interpolate between two vectors
     * 
     * @param a First vector
     * @param b Second vector
     * @param t Interpolation factor (0 = a, 1 = b)
     * @return Interpolated vector
     */
    static std::vector<double> interpolate(const std::vector<double>& a, const std::vector<double>& b, double t) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size for interpolation");
        }
        
        std::vector<double> result;
        result.reserve(a.size());
        
        for (size_t i = 0; i < a.size(); ++i) {
            result.push_back(a[i] + t * (b[i] - a[i]));
        }
        
        return result;
    }

    /**
     * Find minimum value in a vector
     * 
     * @param input Vector
     * @return Minimum value
     */
    static double vectorMin(const std::vector<double>& input) {
        if (input.empty()) {
            throw std::invalid_argument("Cannot find minimum of empty vector");
        }
        return *std::min_element(input.begin(), input.end());
    }

    /**
     * Find maximum value in a vector
     * 
     * @param input Vector
     * @return Maximum value
     */
    static double vectorMax(const std::vector<double>& input) {
        if (input.empty()) {
            throw std::invalid_argument("Cannot find maximum of empty vector");
        }
        return *std::max_element(input.begin(), input.end());
    }

    /**
     * Compute variance of a vector
     * 
     * @param input Vector
     * @return Variance
     */
    static double vectorVariance(const std::vector<double>& input) {
        if (input.size() <= 1) {
            return 0.0;
        }
        
        double mean = vectorMean(input);
        double sumSq = 0.0;
        for (double value : input) {
            sumSq += (value - mean) * (value - mean);
        }
        
        return sumSq / (input.size() - 1);
    }

private:
    MathUtilities() = default; // Private constructor for singleton pattern
    ~MathUtilities() = default;
};

} // namespace nlm
