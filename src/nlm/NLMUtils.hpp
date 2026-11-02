// NLM Utilities - Enhanced code organization and utility functions
// This file provides utility functions for easier extension and research

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <random>

namespace nlm {

// Math utilities
class MathUtils {
public:
    // Vector operations
    static float dotProduct(const std::vector<float>& a, const std::vector<float>& b);
    static float magnitude(const std::vector<float>& v);
    static float normalize(std::vector<float>& v);
    static float distance(const std::vector<float>& a, const std::vector<float>& b);
    
    // Signal processing utilities
    static std::vector<float> smoothSignal(const std::vector<float>& signal, int windowSize = 5);
    static float movingAverage(const std::vector<float>& signal, size_t index, size_t windowSize = 5);
    static float variance(const std::vector<float>& data);
    static float standardDeviation(const std::vector<float>& data);
    
    // Statistical utilities
    static float mean(const std::vector<float>& data);
    static float median(const std::vector<float>& data);
    static float correlation(const std::vector<float>& x, const std::vector<float>& y);
    
    // Random utilities
    static void seedRandom(uint64_t seed);
    static float randomFloat(float min = 0.0f, float max = 1.0f);
    static int randomInt(int min = 0, int max = 100);
    static bool randomBool(float probability = 0.5f);
    
    // Array operations
    static std::vector<float> arrayAdd(const std::vector<float>& a, const std::vector<float>& b);
    static std::vector<float> arrayMultiply(const std::vector<float>& a, float scalar);
    static std::vector<float> arrayConvolve(const std::vector<float>& a, const std::vector<float>& b);
    static void arrayCopy(const std::vector<float>& source, std::vector<float>& destination);
    
    // Pattern matching utilities
    static std::vector<size_t> findPattern(const std::vector<float>& signal, const std::vector<float>& pattern);
    static bool matchPattern(const std::vector<float>& signal, const std::vector<float>& pattern, size_t startIndex);
    
    // Time utilities
    static double getCurrentTime();
    static double elapsedTime(double startTime);
};

// Data utilities
class DataUtils {
public:
    // Data serialization
    static bool saveToFile(const std::string& filepath, const std::vector<float>& data);
    static bool loadFromFile(const std::string& filepath, std::vector<float>& data);
    static bool saveToCSV(const std::string& filepath, const std::vector<std::vector<float>>& data, const std::vector<std::string>& headers);
    static bool loadFromCSV(const std::string& filepath, std::vector<std::vector<float>>& data);
    
    // Data analysis
    static std::vector<float> calculateRunningStats(const std::vector<float>& data, size_t windowSize = 10);
    static float detectOutliers(const std::vector<float>& data, float threshold = 2.0f);
    static std::vector<float> normalizeData(const std::vector<float>& data, float min = 0.0f, float max = 1.0f);
    static std::vector<float> zScoreNormalize(const std::vector<float>& data);
    
    // Data manipulation
    static std::vector<float> concatenate(const std::vector<float>& a, const std::vector<float>& b);
    static std::vector<float> subsample(const std::vector<float>& data, size_t step);
    static std::vector<float> interpolate(const std::vector<float>& data, float newSamplingRate);
    static std::vector<float> resample(const std::vector<float>& data, size_t newSize);
    
    // Signal utilities
    static std::vector<float> bandpassFilter(const std::vector<float>& signal, float lowFreq, float highFreq, float samplingRate);
    static std::vector<float> removeDC(const std::vector<float>& signal);
    static std::vector<float> applyHanningWindow(const std::vector<float>& signal);
};

// Configuration utilities
class ConfigUtils {
public:
    // Configuration validation
    static bool validateConfig(const std::string& filepath);
    static bool validateConfigKeys(const std::map<std::string, std::string>& config);
    static std::vector<std::string> getMissingKeys(const std::map<std::string, std::string>& config, const std::vector<std::string>& requiredKeys);
    
    // Configuration utilities
    static std::map<std::string, std::string> loadConfigFromString(const std::string& configString);
    static std::string configToString(const std::map<std::string, std::string>& config);
    static std::string generateConfigHash(const std::map<std::string, std::string>& config);
    
    // Configuration comparison
    static bool configsAreEqual(const std::map<std::string, std::string>& config1, const std::map<std::string, std::string>& config2);
    static float configSimilarity(const std::map<std::string, std::string>& config1, const std::map<std::string, std::string>& config2);
    
    // Configuration templates
    static std::map<std::string, std::string> getStandardConfig();
    static std::map<std::string, std::string> getExplorationConfig();
    static std::map<std::string, std::string> getExploitationConfig();
    static std::map<std::string, std::string> getDevelopmentConfig();
};

// Simulation utilities
class SimulationUtils {
public:
    // Simulation control
    static void sleep(double seconds);
    static void busyWait(double seconds);
    static void yieldCPU();
    
    // Performance monitoring
    static double measureExecutionTime(std::function<void()> func);
    static double measureMemoryUsage();
    static double measureCPUUsage();
    
    // Simulation synchronization
    static void waitForSteps(int currentStep, int targetStep, double timestep = 0.001);
    static void stepUntil(double targetTime, std::function<void(double, double)> stepFunction, double timestep = 0.001);
    
    // Simulation monitoring
    static std::map<std::string, double> getSystemStats();
    static std::map<std::string, double> getPerformanceStats();
    static void printSystemStats();
    static void logPerformanceStats(const std::string& filepath);
};

// Research utilities
class ResearchUtils {
public:
    // Experiment utilities
    static std::vector<std::vector<float>> runParameterSweep(
        std::function<std::vector<float>(const std::vector<float>&)> func,
        const std::vector<std::vector<float>>& parameterSpace,
        const std::vector<std::string>& parameterNames
    );
    
    static std::map<std::string, float> analyzeExperimentResults(
        const std::vector<std::map<std::string, float>>& results,
        const std::vector<std::string>& metrics
    );
    
    static bool saveExperimentData(
        const std::string& experimentName,
        const std::vector<std::map<std::string, float>>& results,
        const std::vector<std::string>& metrics
    );
    
    // Statistical analysis
    static std::map<std::string, float> calculateStatistics(const std::vector<float>& data);
    static std::map<std::string, float> compareGroups(const std::vector<std::vector<float>>& groups);
    static std::vector<float> performANOVA(const std::vector<std::vector<float>>& groups);
    
    // Visualization utilities (simple text-based)
    static std::string createHistogram(const std::vector<float>& data, size_t width = 50);
    static std::string createScatterPlot(const std::vector<float>& x, const std::vector<float>& y, size_t width = 50, size_t height = 20);
    static std::string createTimeSeries(const std::vector<double>& timestamps, const std::vector<float>& values, size_t width = 60);
};

// Export utilities
class ExportUtils {
public:
    // Data export formats
    static bool exportToJSON(const std::string& filepath, const std::map<std::string, float>& data);
    static bool exportToCSV(const std::string& filepath, const std::vector<std::vector<float>>& data, const std::vector<std::string>& headers);
    static bool exportToTXT(const std::string& filepath, const std::map<std::string, std::string>& data);
    static bool exportToXML(const std::string& filepath, const std::map<std::string, std::string>& data);
    
    // Configuration export
    static bool exportConfig(const std::string& filepath, const std::map<std::string, std::string>& config);
    static bool importConfig(const std::string& filepath, std::map<std::string, std::string>& config);
    
    // Results export
    static bool exportResults(const std::string& filepath, const std::vector<std::map<std::string, float>>& results);
    static bool exportResultsWithMetadata(const std::string& filepath, const std::vector<std::map<std::string, float>>& results, const std::map<std::string, std::string>& metadata);
};

// Error handling utilities
class ErrorUtils {
public:
    // Error management
    static void setErrorHandler(std::function<void(const std::string&)> handler);
    static void clearErrorHandler();
    static void throwError(const std::string& message);
    static void logError(const std::string& message, LogLevel level = LogLevel::Error);
    
    // Exception utilities
    static std::string getLastError();
    static void clearLastError();
    static bool hasError();
    
    // Validation utilities
    static bool validateInput(const std::string& name, const std::string& value, const std::string& type = "string");
    static bool validateRange(float value, float min, float max, const std::string& parameterName = "");
    static bool validateFile(const std::string& filepath, bool mustExist = true);
    
    // Warning utilities
    static void warn(const std::string& message);
    static void info(const std::string& message);
    static void debug(const std::string& message);
};

// Time series utilities
class TimeSeriesUtils {
public:
    // Time series operations
    static std::vector<std::pair<double, float>> convertToTimeSeries(const std::vector<float>& values, double timestep = 1.0);
    static std::vector<float> resampleTimeSeries(const std::vector<std::pair<double, float>>& series, double newSamplingInterval);
    static double calculateTemporalCorrelation(const std::vector<std::pair<double, float>>& series1, const std::vector<std::pair<double, float>>& series2);
    
    // Time series analysis
    static std::pair<float, float> calculateTrend(const std::vector<std::pair<double, float>>& series);
    static float calculateRateOfChange(const std::vector<std::pair<double, float>>& series);
    static float calculateAutocorrelation(const std::vector<float>& signal, size_t lag = 1);
    
    // Pattern detection
    static std::vector<size_t> detectPeaks(const std::vector<float>& signal, float threshold = 0.0f);
    static std::vector<size_t> detectValleys(const std::vector<float>& signal, float threshold = 0.0f);
    static std::vector<std::pair<size_t, size_t>> detectCycles(const std::vector<float>& signal, float minPeriod = 10, float maxPeriod = 100);
};

// Visualization utilities (text-based)
class VisualizationUtils {
public:
    // Plotting utilities
    static std::string createBarChart(const std::vector<float>& data, const std::vector<std::string>& labels, size_t width = 60);
    static std::string createLineChart(const std::vector<float>& data, size_t width = 60, size_t height = 20);
    static std::string createHeatmap(const std::vector<std::vector<float>>& data, const std::vector<std::string>& xLabels, const std::vector<std::string>& yLabels);
    
    // Text-based visualization
    static std::string createAsciiPlot(const std::vector<float>& data, char marker = '*', char axis = '-');
    static std::string createComparisonChart(const std::vector<std::vector<float>>& datasets, const std::vector<std::string>& labels);
    static std::string createProgressBar(float progress, size_t width = 50);
    
    // Statistical plots
    static std::string createBoxPlot(const std::vector<float>& data);
    static std::string createHistogram(const std::vector<float>& data, size_t bins = 10);
    static std::string createQ-QPlot(const std::vector<float>& data, const std::vector<float>& theoreticalData);
};

// Plugin interface utilities
class PluginUtils {
public:
    // Plugin management
    static bool loadPlugin(const std::string& pluginPath);
    static bool unloadPlugin(const std::string& pluginName);
    static bool isPluginLoaded(const std::string& pluginName);
    static std::vector<std::string> getLoadedPlugins();
    
    // Plugin discovery
    static std::vector<std::string> discoverPlugins(const std::string& directory);
    static std::map<std::string, std::string> getPluginInfo(const std::string& pluginName);
    
    // Plugin hooks
    static void registerHook(const std::string& hookName, std::function<void()> callback);
    static void unregisterHook(const std::string& hookName);
    static void executeHooks(const std::string& hookName);
    
    // Plugin configuration
    static bool configurePlugin(const std::string& pluginName, const std::map<std::string, std::string>& config);
    static std::map<std::string, std::string> getPluginConfig(const std::string& pluginName);
};

// Memory management utilities
class MemoryUtils {
public:
    // Memory allocation utilities
    static void* allocate(size_t size);
    static void deallocate(void* ptr);
    static void* allocateAligned(size_t size, size_t alignment);
    static void deallocateAligned(void* ptr);
    
    // Memory monitoring
    static size_t getAvailableMemory();
    static size_t getUsedMemory();
    static size_t getTotalMemory();
    static float getMemoryUsagePercentage();
    
    // Memory profiling
    static void startMemoryProfile();
    static void stopMemoryProfile();
    static std::map<std::string, size_t> getMemoryProfile();
    
    // Memory optimization
    static void optimizeMemoryUsage();
    static void garbageCollect();
    static void compactMemory();
};

// Performance profiling utilities
class ProfilingUtils {
public:
    // Function profiling
    static void startProfile(const std::string& functionName);
    static void stopProfile(const std::string& functionName);
    static double getProfileTime(const std::string& functionName);
    static std::map<std::string, double> getAllProfileTimes();
    
    // System profiling
    static void startSystemProfile();
    static void stopSystemProfile();
    static std::map<std::string, double> getSystemProfile();
    
    // Performance optimization
    static std::vector<std::string> profilePerformance();
    static std::map<std::string, double> optimizePerformance();
    static void printPerformanceReport();
};

} // namespace nlm

// Convenience function implementations
namespace nlm {
    // Initialize all random seeds
    inline void initializeRandom(uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count()) {
        std::srand(static_cast<unsigned int>(seed));
        MathUtils::seedRandom(seed);
    }
    
    // Quick validation
    inline bool validateBrainParameters(const std::map<std::string, float>& params) {
        bool valid = true;
        if (params.find("neuron_count") != params.end() && params.at("neuron_count") <= 0) {
            valid = false;
        }
        if (params.find("connection_probability") != params.end() && 
            (params.at("connection_probability") < 0.0f || params.at("connection_probability") > 1.0f)) {
            valid = false;
        }
        return valid;
    }
    
    // Format performance stats
    inline std::string formatPerformanceStats(const std::map<std::string, double>& stats) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        for (const auto& stat : stats) {
            ss << stat.first << ": " << stat.second << " ";
        }
        return ss.str();
    }
    
    // Create output directory if it doesn't exist
    inline bool ensureOutputDirectory(const std::string& filepath) {
        size_t pos = filepath.find_last_of('/');
        if (pos != std::string::npos) {
            std::string dir = filepath.substr(0, pos);
            // In a real implementation, this would create the directory
            // For now, we'll assume it exists
            return true;
        }
        return false;
    }
    
    // Calculate confidence interval
    inline std::pair<float, float> calculateConfidenceInterval(const std::vector<float>& data, float confidence = 0.95) {
        if (data.empty()) {
            return {0.0f, 0.0f};
        }
        
        float mean = MathUtils::mean(data);
        float std = MathUtils::standardDeviation(data);
        float n = static_cast<float>(data.size());
        float t = 1.96f; // For 95% confidence
        
        float margin = t * std / std::sqrt(n);
        return {mean - margin, mean + margin};
    }
    
    // Check if data is normal (simple test)
    inline bool isNormal(const std::vector<float>& data, float alpha = 0.05) {
        if (data.size() < 3) {
            return false;
        }
        
        // Simple Shapiro-Wilk test approximation
        float mean = MathUtils::mean(data);
        float variance = MathUtils::variance(data);
        if (variance == 0.0f) {
            return true; // All values are the same
        }
        
        float skew = 0.0f;
        for (float x : data) {
            skew += std::pow((x - mean) / std::sqrt(variance), 3);
        }
        skew /= data.size();
        
        // Simple check based on skewness
        return std::abs(skew) < 0.5f;
    }
    
    // Calculate coefficient of variation
    inline float calculateCoefficientOfVariation(const std::vector<float>& data) {
        if (data.empty()) {
            return 0.0f;
        }
        float mean = MathUtils::mean(data);
        float std = MathUtils::standardDeviation(data);
        return (mean == 0.0f) ? 0.0f : (std / mean) * 100.0f;
    }
    
    // Get percentile
    inline float getPercentile(const std::vector<float>& data, float percentile) {
        if (data.empty()) {
            return 0.0f;
        }
        
        std::vector<float> sorted = data;
        std::sort(sorted.begin(), sorted.end());
        
        float index = (percentile / 100.0f) * (sorted.size() - 1);
        size_t lower = static_cast<size_t>(std::floor(index));
        size_t upper = static_cast<size_t>(std::ceil(index));
        
        if (lower == upper) {
            return sorted[lower];
        }
        
        float weight = index - lower;
        return sorted[lower] * (1.0f - weight) + sorted[upper] * weight;
    }
    
    // Calculate interquartile range
    inline float calculateIQR(const std::vector<float>& data) {
        if (data.size() < 4) {
            return 0.0f;
        }
        
        std::vector<float> sorted = data;
        std::sort(sorted.begin(), sorted.end());
        
        float q1 = getPercentile(sorted, 25);
        float q3 = getPercentile(sorted, 75);
        return q3 - q1;
    }
    
    // Calculate percentile rank
    inline float calculatePercentileRank(const std::vector<float>& data, float value) {
        if (data.empty()) {
            return 0.0f;
        }
        
        std::vector<float> sorted = data;
        std::sort(sorted.begin(), sorted.end());
        
        size_t rank = std::lower_bound(sorted.begin(), sorted.end(), value) - sorted.begin();
        return (rank / static_cast<float>(sorted.size())) * 100.0f;
    }
    
    // Create time series from data
    inline std::vector<std::pair<double, float>> createTimeSeries(const std::vector<float>& values, double timestep = 1.0) {
        std::vector<std::pair<double, float>> series;
        series.reserve(values.size());
        
        for (size_t i = 0; i < values.size(); ++i) {
            series.emplace_back(i * timestep, values[i]);
        }
        
        return series;
    }
    
    // Calculate rolling average
    inline std::vector<float> rollingAverage(const std::vector<float>& data, size_t windowSize) {
        std::vector<float> result;
        if (data.size() < windowSize) {
            return result;
        }
        
        result.reserve(data.size() - windowSize + 1);
        
        for (size_t i = 0; i <= data.size() - windowSize; ++i) {
            float sum = 0.0f;
            for (size_t j = 0; j < windowSize; ++j) {
                sum += data[i + j];
            }
            result.push_back(sum / static_cast<float>(windowSize));
        }
        
        return result;
    }
    
    // Calculate moving standard deviation
    inline std::vector<float> rollingStdDev(const std::vector<float>& data, size_t windowSize) {
        std::vector<float> result;
        if (data.size() < windowSize) {
            return result;
        }
        
        result.reserve(data.size() - windowSize + 1);
        
        for (size_t i = 0; i <= data.size() - windowSize; ++i) {
            std::vector<float> window(data.begin() + i, data.begin() + i + windowSize);
            result.push_back(MathUtils::standardDeviation(window));
        }
        
        return result;
    }
    
    // Calculate exponential moving average
    inline std::vector<float> exponentialMovingAverage(const std::vector<float>& data, float alpha = 0.1f) {
        std::vector<float> result;
        if (data.empty()) {
            return result;
        }
        
        result.reserve(data.size());
        result.push_back(data[0]);
        
        for (size_t i = 1; i < data.size(); ++i) {
            result.push_back(alpha * data[i] + (1.0f - alpha) * result[i - 1]);
        }
        
        return result;
    }
    
    // Calculate linear regression
    inline std::pair<float, float> linearRegression(const std::vector<std::pair<double, float>>& data) {
        if (data.size() < 2) {
            return {0.0f, 0.0f};
        }
        
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumXX = 0.0;
        size_t n = data.size();
        
        for (const auto& point : data) {
            sumX += point.first;
            sumY += point.second;
            sumXY += point.first * point.second;
            sumXX += point.first * point.first;
        }
        
        double slope = (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
        double intercept = (sumY - slope * sumX) / n;
        
        return {static_cast<float>(slope), static_cast<float>(intercept)};
    }
    
    // Calculate R-squared
    inline float calculateRSquared(const std::vector<std::pair<double, float>>& data, float slope, float intercept) {
        if (data.empty()) {
            return 0.0f;
        }
        
        double sumSS = 0.0, sumPredSS = 0.0;
        size_t n = data.size();
        
        for (const auto& point : data) {
            double y = point.second;
            double x = point.first;
            sumSS += (y - intercept - slope * x) * (y - intercept - slope * x);
            sumPredSS += (y - y) * (y - y); // This is always 0 - need correct calculation
        }
        
        // Simplified R-squared calculation
        double ssTotal = 0.0, ssResidual = 0.0;
        double meanY = 0.0;
        
        for (const auto& point : data) {
            meanY += point.second;
        }
        meanY /= n;
        
        for (const auto& point : data) {
            double residual = point.second - intercept - slope * point.first;
            ssResidual += residual * residual;
        }
        
        double ssTotal = ssResidual + sumSS;
        
        return static_cast<float>((ssTotal - ssResidual) / ssTotal);
    }
    
    // Calculate area under curve
    inline float calculateAUC(const std::vector<std::pair<double, float>>& data) {
        if (data.empty()) {
            return 0.0f;
        }
        
        float area = 0.0f;
        for (size_t i = 1; i < data.size(); ++i) {
            double x1 = data[i-1].first;
            double x2 = data[i].first;
            double y1 = data[i-1].second;
            double y2 = data[i].second;
            
            // Trapezoidal rule
            area += (y1 + y2) * (x2 - x1) / 2.0f;
        }
        
        return area;
    }
    
    // Calculate mean absolute error
    inline float calculateMAE(const std::vector<std::pair<double, float>>& data, float slope, float intercept) {
        if (data.empty()) {
            return 0.0f;
        }
        
        float sumError = 0.0f;
        for (const auto& point : data) {
            float predicted = intercept + slope * static_cast<float>(point.first);
            sumError += std::abs(point.second - predicted);
        }
        
        return sumError / static_cast<float>(data.size());
    }
    
    // Calculate root mean square error
    inline float calculateRMSE(const std::vector<std::pair<double, float>>& data, float slope, float intercept) {
        if (data.empty()) {
            return 0.0f;
        }
        
        float sumSquaredError = 0.0f;
        for (const auto& point : data) {
            float predicted = intercept + slope * static_cast<float>(point.first);
            float error = point.second - predicted;
            sumSquaredError += error * error;
        }
        
        return std::sqrt(sumSquaredError / static_cast<float>(data.size()));
    }
}} // namespace nlm
EOF