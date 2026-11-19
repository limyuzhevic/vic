#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <limits>

namespace nlm {

/**
 * Mathematical utility functions for neural network computations
 */
class MathUtils {
public:
    // Activation function utilities
    static float sigmoid(float x);
    static float sigmoidDerivative(float x);
    static float relu(float x);
    static float reluDerivative(float x);
    static float tanh(float x);
    static float tanhDerivative(float x);
    
    // Statistical functions
    static float mean(const std::vector<float>& values);
    static float variance(const std::vector<float>& values);
    static float standardDeviation(const std::vector<float>& values);
    static float median(const std::vector<float>& values);
    
    // Vector operations
    static std::vector<float> add(const std::vector<float>& a, const std::vector<float>& b);
    static std::vector<float> multiply(const std::vector<float>& a, const std::vector<float>& b);
    static std::vector<float> scalarMultiply(float scalar, const std::vector<float>& vec);
    static float dotProduct(const std::vector<float>& a, const std::vector<float>& b);
    static float vectorNorm(const std::vector<float>& vec);
    
    // Comparison and thresholding
    static std::vector<float> applyThreshold(const std::vector<float>& values, float threshold);
    static std::vector<float> clamp(const std::vector<float>& values, float min, float max);
    static std::vector<float> normalize(const std::vector<float>& values, float min, float max);
    
    // Random number generation
    static void seedRandom(unsigned int seed);
    static float randomFloat(float min, float max);
    static int randomInt(int min, int max);
    
    // Time utilities
    static uint64_t getCurrentTimestamp();
    static double getElapsedSeconds(uint64_t startTime);
    
    // Bit manipulation
    static bool isPowerOfTwo(int n);
    static int nextPowerOfTwo(int n);
    
    // Memory utilities
    static size_t getMemoryUsageMB();
};

/**
 * Performance timing utilities
 */
class PerformanceTimer {
public:
    PerformanceTimer();
    ~PerformanceTimer();
    
    // Start timing
    void start();
    
    // Stop timing and return elapsed time
    double stop();
    
    // Get current elapsed time
    double elapsed() const;
    
    // Check if timer is running
    bool isRunning() const;
    
    // Reset timer
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Debugging and logging utilities
 */
class DebugUtils {
public:
    // Assert with message
    static void assert(bool condition, const std::string& message);
    
    // Log performance metrics
    static void logPerformance(const std::string& operation, double timeMs);
    
    // Check for memory issues
    static void checkMemoryLeak(size_t expectedAllocation, size_t actualAllocation);
    
    // Validate vector operations
    static bool validateVector(const std::vector<float>& vec, size_t expectedSize);
    
    // Generate debug output
    static std::string generateDebugInfo(const std::string& prefix = "DEBUG");
};

/**
 * Neural network specific utilities
 */
class NeuralUtils {
public:
    // Weight initialization
    static float initializeWeight(std::mt19937& rng);
    static void initializeWeights(std::vector<float>& weights, std::mt19937& rng);
    static void initializeWeights(std::vector<std::vector<float>>& weights, std::mt19937& rng);
    
    // Spike timing utilities
    static bool isSpikeTimeClose(float time1, float time2, float tolerance = 1.0f);
    static float calculateSpikeFrequency(const std::vector<float>& spikeTimes);
    static std::vector<float> filterSpikeTimes(const std::vector<float>& spikeTimes, float minInterval);
    
    // Synapse utilities
    static float calculateSynapticStrength(float weight, float efficacy, float eligibilityTrace);
    static bool isValidSynapticConnection(float weight, float minWeight = -1.0f, float maxWeight = 1.0f);
    
    // Neuron state utilities
    static bool isNeuronFiring(float membranePotential, float threshold);
    static float calculateFiringRate(const std::vector<float>& spikeTimes, double duration);
    
    // Network topology utilities
    static size_t calculateTotalConnections(const std::vector<size_t>& layerSizes);
    static std::vector<size_t> calculateLayerOutputSizes(const std::vector<size_t>& layerSizes);
};

/**
 * File system utilities
 */
class FileUtils {
public:
    // File operations
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
    static std::string getFileExtension(const std::string& filename);
    static std::string getFileNameWithoutExtension(const std::string& filename);
    
    // Path manipulation
    static std::string joinPaths(const std::string& base, const std::string& filename);
    static std::string getDirectoryPath(const std::string& path);
    static std::string getAbsolutePath(const std::string& path);
    
    // Configuration file utilities
    static bool isConfigurationFile(const std::string& filename);
    static std::vector<std::string> parseConfigurationFile(const std::string& filename);
    static bool saveConfigurationFile(const std::string& filename, const std::vector<std::string>& content);
};

/**
 * Collection utilities (commonly used STL algorithms)
 */
class CollectionUtils {
public:
    // Array operations
    template<typename T>
    static std::vector<T> toVector(const T* array, size_t size);
    
    // Mathematical operations on collections
    template<typename T>
    static T sum(const std::vector<T>& values);
    
    template<typename T>
    static T product(const std::vector<T>& values);
    
    template<typename T>
    static T max(const std::vector<T>& values);
    
    template<typename T>
    static T min(const std::vector<T>& values);
    
    // Statistical operations
    template<typename T>
    static double average(const std::vector<T>& values);
    
    template<typename T>
    static double variance(const std::vector<T>& values);
    
    // Collection manipulation
    template<typename T>
    static void shuffle(std::vector<T>& vec, std::mt19937& rng);
    
    template<typename T>
    static void sortDescending(std::vector<T>& vec);
    
    template<typename T>
    static void sortAscending(std::vector<T>& vec);
    
    // Unique elements
    template<typename T>
    static std::vector<T> getUniqueElements(const std::vector<T>& vec);
    
    // Filtering
    template<typename T>
    static std::vector<T> filter(const std::vector<T>& vec, std::function<bool(const T&)> predicate);
    
    // Transformation
    template<typename T, typename U>
    static std::vector<U> transform(const std::vector<T>& vec, std::function<U(const T&)> func);
};

} // namespace nlm
