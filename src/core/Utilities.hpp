// NLM Utilities
// Common utilities and helper functions for improved code organization

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

// String utilities
class StringUtils {
public:
    static std::string toUpper(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string trim(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static std::string format(const std::string& format, ...);
    static std::string timeToString(time_t time);
    static std::string durationToString(double seconds);
};

// Math utilities
class MathUtils {
public:
    static constexpr float PI = 3.141592653589793f;
    static constexpr float E = 2.718281828459045f;
    
    static float clamp(float value, float min, float max);
    static float lerp(float a, float b, float t);
    static float map(float value, float inMin, float inMax, float outMin, float outMax);
    static float sigmoid(float x);
    static float tanh(float x);
    static float normalize(float value, float min, float max);
    static float sqrtApprox(float x);
    static float smoothStep(float edge0, float edge1, float x);
    static bool isPowerOfTwo(size_t x);
    static size_t nextPowerOfTwo(size_t x);
};

// Performance monitoring
class PerformanceMonitor {
public:
    struct Metrics {
        double cpuTime;
        double wallTime;
        size_t memoryUsage;
        size_t allocatedObjects;
        double lastFrameTime;
        double fps;
    };
    
    PerformanceMonitor();
    void startTiming();
    void stopTiming();
    double getElapsedTime() const;
    double getCPUTime() const;
    Metrics getMetrics() const;
    void reset();
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point cpuStartTime_;
    double elapsedTime_;
    double cpuTime_;
};

// Configuration parser
class ConfigParser {
public:
    static std::map<std::string, std::string> parseConfigFile(const std::string& filename);
    static bool parseKeyValue(const std::string& line, std::string& key, std::string& value);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static std::vector<std::string> readLines(const std::string& filename);
    static bool fileExists(const std::string& filename);
};

// Progress reporting
class ProgressReporter {
public:
    ProgressReporter(size_t totalSteps, size_t reportInterval = 100);
    void increment();
    void report(const std::string& message = "");
    void setStatus(const std::string& status);
    bool isComplete() const;
    double getProgress() const;
    
private:
    size_t currentStep_;
    size_t totalSteps_;
    size_t reportInterval_;
    std::string status_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Random number generator utilities
class RandomUtils {
public:
    static float randomFloat(float min = 0.0f, float max = 1.0f);
    static double randomDouble(double min = 0.0, double max = 1.0);
    static int randomInt(int min, int max);
    static bool randomBool(float probability = 0.5f);
    static void seedRandom(unsigned int seed = 0);
};

// Color utilities for console output
class ColorUtils {
public:
    enum Color {
        RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, GRAY, RESET
    };
    
    static void setColor(Color color);
    static std::string getColorCode(Color color);
    static std::string formatWithColor(const std::string& text, Color color);
    static std::string getSuccessPrefix();
    static std::string getErrorPrefix();
    static std::string getWarningPrefix();
    static std::string getInfoPrefix();
};

// Time utilities
class TimeUtils {
public:
    static std::string getCurrentTimestamp();
    static std::string getRelativeTime(double seconds);
    static double getCurrentTime();
    static void sleep(double seconds);
    static std::chrono::high_resolution_clock::time_point now();
    static double toMilliseconds(const std::chrono::high_resolution_clock::time_point& start);
    static double toSeconds(const std::chrono::high_resolution_clock::time_point& start);
};

// Stream utilities
class StreamUtils {
public:
    static std::string streamToString(std::istream& stream);
    static void writeToStream(std::ostream& stream, const std::string& data);
    static bool isStreamGood(std::istream& stream);
    static bool isStreamGood(std::ostream& stream);
    static void safeClose(std::ofstream& file);
    static void safeClose(std::ifstream& file);
};

// Math constants
namespace Constants {
    constexpr float DEFAULT_TIMESTEP = 0.001f;
    constexpr float DEFAULT_FPS = 60.0f;
    constexpr double DEFAULT_WALL_CLOCK_TOLERANCE = 0.001;
    constexpr size_t DEFAULT_BUFFER_SIZE = 1024;
    constexpr float DEFAULT_NOISE_LEVEL = 0.1f;
    constexpr float DEFAULT_DECAY_RATE = 0.99f;
    constexpr float DEFAULT_LEARNING_RATE = 0.01f;
    constexpr int MAX_THREADS = 8;
    constexpr size_t MAX_MEMORY_MB = 512;
}

} // namespace nlm