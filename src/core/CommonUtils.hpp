#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <functional>
#include <map>
#include <variant>

namespace nlm {

// Common types
using String = std::string;
using ErrorMessage = std::string;
using WarningMessage = std::string;
using ValidatorFunction = std::function<bool(const std::string&, const std::string&)>;

// Custom exceptions
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationException : public NLMException {
public:
    explicit ConfigValidationException(const std::string& message) : NLMException(message) {}
};

class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message) : NLMException(message) {}
};

// Error handling utilities
class ErrorHandler {
public:
    enum class ErrorLevel { DEBUG, INFO, WARNING, ERROR, FATAL };
    
    struct ErrorInfo {
        ErrorLevel level;
        std::string message;
        std::string source;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> context;
        
        ErrorInfo(ErrorLevel lvl, const std::string& msg, const std::string& src = "")
            : level(lvl), message(msg), source(src), timestamp(std::chrono::system_clock::now()) {}
    };
    
    static void logError(ErrorLevel level, const std::string& message, const std::string& source = "");
    static void logDebug(const std::string& message, const std::string& source = "");
    static void logInfo(const std::string& message, const std::string& source = "");
    static void logWarning(const std::string& message, const std::string& source = "");
    static void logError(const std::string& message, const std::string& source = "");
    static void logFatal(const std::string& message, const std::string& source = "");
    
    static std::vector<ErrorInfo> getRecentErrors(size_t limit = 100);
    static void clearErrorHistory();
    static void setErrorHandler(std::function<void(ErrorLevel, const std::string&, const std::string&)> handler);
    
private:
    static std::vector<ErrorInfo> errorHistory;
    static std::function<void(ErrorLevel, const std::string&, const std::string&)> errorHandler;
};

// Validation utilities
class ValidationUtils {
public:
    // String validation
    static bool isValidString(const std::string& str, size_t minLength = 1, size_t maxLength = 1000);
    static bool isValidIdentifier(const std::string& identifier);
    static bool isValidPath(const std::string& path);
    
    // Numeric validation
    static bool isValidRange(double value, double min, double max);
    static bool isValidPositive(int64_t value);
    static bool isValidPositive(double value);
    static bool isValidFloat(const std::string& str);
    
    // Configuration validation
    static bool isValidConfigKey(const std::string& key);
    static bool isValidConfigValue(const std::string& key, const std::string& value);
    
    // Array validation
    static bool isValidStringArray(const std::vector<std::string>& array, size_t maxSize = 100);
    static bool containsDuplicates(const std::vector<std::string>& array);
    
    // Pattern matching
    static bool matchesPattern(const std::string& str, const std::string& pattern);
    static bool isValidFileExtension(const std::string& filename, const std::vector<std::string>& extensions);
    
    // Helper functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& parts, char delimiter);
    
    // Configuration validation
    static bool validateConfigValue(const std::string& key, const std::string& value);
    static bool validateConfigFile(const std::string& filepath);
    
    // Type conversion with validation
    static bool safeStringToInt(const std::string& str, int64_t& result, int64_t min = INT64_MIN, int64_t max = INT64_MAX);
    static bool safeStringToDouble(const std::string& str, double& result, double min = -1e100, double max = 1e100);
    static bool safeStringToBool(const std::string& str, bool& result);
};

// Logging utilities
class LoggingUtils {
public:
    // Log file management
    static bool setupLogFile(const std::string& filename, bool append = false);
    static void logToFile(const std::string& message);
    static void closeLogFile();
    static bool isLogFileOpen();
    
    // Console output formatting
    static void setConsoleColor(int color);
    static void resetConsoleColor();
    static void setLogLevel(ErrorHandler::ErrorLevel level);
    static ErrorHandler::ErrorLevel getLogLevel();
    
    // Formatted logging
    static void logFormatted(const std::string& format, ...);
    static void logWithTimestamp(const std::string& message);
    static void logWithContext(const std::string& message, const std::map<std::string, std::string>& context);
    
    // Performance logging
    static void logPerformance(const std::string& operation, double duration, const std::string& details = "");
    static void logMemoryUsage(const std::string& context = "");
    
    // Network logging (for future distributed features)
    static void logNetworkEvent(const std::string& event, const std::string& details = "");
};

// String utilities
class StringUtils {
public:
    // String manipulation
    static std::string trim(const std::string& str);
    static std::string ltrim(const std::string& str);
    static std::string rtrim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string capitalize(const std::string& str);
    static std::string camelCaseToSnakeCase(const std::string& str);
    static std::string snakeCaseToCamelCase(const std::string& str);
    
    // String operations
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static size_t countOccurrences(const std::string& str, const std::string& substr);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& parts, char delimiter);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    // Pattern matching
    static bool matchesRegex(const std::string& str, const std::string& regex);
    static std::string escapeRegex(const std::string& str);
    
    // Encoding
    static std::string urlEncode(const std::string& str);
    static std::string urlDecode(const std::string& str);
    static std::string htmlEscape(const std::string& str);
    static std::string htmlUnescape(const std::string& str);
    
    // Formatting
    static std::string format(const std::string& format, ...);
    static std::string repeat(const std::string& str, size_t count);
    static std::string padLeft(const std::string& str, size_t width, char pad = ' ');
    static std::string padRight(const std::string& str, size_t width, char pad = ' ');
    
    // Validation
    static bool isValidEmail(const std::string& email);
    static bool isValidURL(const std::string& url);
    static bool isValidIP(const std::string& ip);
    static bool isValidUUID(const std::string& uuid);
    
    // Text analysis
    static double calculateLevenshteinDistance(const std::string& str1, const std::string& str2);
    static double calculateJaroSimilarity(const std::string& str1, const std::string& str2);
    static double calculateJaccardSimilarity(const std::string& str1, const std::string& str2);
    
    // Case conversion
    static std::string toCamelCase(const std::string& str);
    static std::string toPascalCase(const std::string& str);
    static std::string toKebabCase(const std::string& str);
    static std::string toSCREAMCase(const std::string& str);
    
    // Other utilities
    static std::string generateRandomString(size_t length);
    static std::string generateUUID();
    static std::string hashString(const std::string& str);
    static std::string getFileExtension(const std::string& filename);
    static std::string getFileNameWithoutExtension(const std::string& filename);
    static std::string getFileName(const std::string& filepath);
    static std::string getDirectoryName(const std::string& filepath);
};

// Math utilities
class MathUtils {
public:
    // Basic arithmetic with validation
    static double clamp(double value, double min, double max);
    static int64_t clamp(int64_t value, int64_t min, int64_t max);
    static bool isWithinRange(double value, double min, double max);
    static bool isWithinRange(int64_t value, int64_t min, int64_t max);
    
    // Trigonometric functions
    static double sin(double angle); // angle in radians
    static double cos(double angle); // angle in radians
    static double tan(double angle); // angle in radians
    static double asin(double value);
    static double acos(double value);
    static double atan(double value);
    static double atan2(double y, double x);
    
    // Hyperbolic functions
    static double sinh(double value);
    static double cosh(double value);
    static double tanh(double value);
    
    // Exponential and logarithmic functions
    static double exp(double value);
    static double log(double value);
    static double log10(double value);
    static double pow(double base, double exponent);
    static double sqrt(double value);
    static double cbrt(double value);
    
    // Mathematical constants
    static constexpr double PI = 3.14159265358979323846;
    static constexpr double E = 2.71828182845904523536;
    static constexpr double SQRT2 = 1.41421356237309504880;
    static constexpr double EULER = 0.57721566490153286060;
    
    // Statistical functions
    static double mean(const std::vector<double>& values);
    static double variance(const std::vector<double>& values);
    static double standardDeviation(const std::vector<double>& values);
    static double median(const std::vector<double>& values);
    static double mode(const std::vector<double>& values);
    
    // Probability functions
    static double normalPDF(double x, double mean = 0.0, double stddev = 1.0);
    static double normalCDF(double x, double mean = 0.0, double stddev = 1.0);
    static double gammaFunction(double x);
    static double betaFunction(double x, double y);
    
    // Array operations
    static double sum(const std::vector<double>& values);
    static double product(const std::vector<double>& values);
    static double max(const std::vector<double>& values);
    static double min(const std::vector<double>& values);
    static size_t argMax(const std::vector<double>& values);
    static size_t argMin(const std::vector<double>& values);
    
    // Vector operations
    static double dotProduct(const std::vector<double>& a, const std::vector<double>& b);
    static double vectorMagnitude(const std::vector<double>& vector);
    static double cosineSimilarity(const std::vector<double>& a, const std::vector<double>& b);
    static std::vector<double> normalizeVector(const std::vector<double>& vector);
    
    // Interpolation
    static double linearInterpolation(double x, double x0, double y0, double x1, double y1);
    static double cubicInterpolation(double x, double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3);
    
    // Integration
    static double integrateTrapezoidal(std::function<double(double)> func, double a, double b, size_t n = 1000);
    static double integrateSimpson(std::function<double(double)> func, double a, double b, size_t n = 1000);
    
    // Number theory
    static bool isPrime(int64_t n);
    static int64_t gcd(int64_t a, int64_t b);
    static int64_t lcm(int64_t a, int64_t b);
    static int64_t factorial(int64_t n);
    static int64_t fibonacci(int64_t n);
};

// Time utilities
class TimeUtils {
public:
    // Time management
    static std::string getCurrentTimestamp();
    static std::chrono::system_clock::time_point getCurrentTime();
    static double getElapsedSeconds(const std::chrono::system_clock::time_point& start);
    static double getElapsedMilliseconds(const std::chrono::system_clock::time_point& start);
    static std::string formatDuration(double seconds);
    
    // Timer utilities
    class Timer {
    public:
        Timer();
        ~Timer();
        
        void start();
        void stop();
        void reset();
        double elapsedSeconds() const;
        double elapsedMilliseconds() const;
        bool isRunning() const;
        
    private:
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        bool running;
    };
    
    // Scheduling
    static bool waitFor(double seconds);
    static bool sleepFor(double seconds);
    static bool sleepUntil(const std::chrono::system_clock::time_point& time);
    
    // Time conversion
    static std::string toString(const std::chrono::system_clock::time_point& time);
    static std::chrono::system_clock::time_point fromString(const std::string& timeStr);
    static double toSeconds(const std::chrono::system_clock::duration& duration);
    static double toMilliseconds(const std::chrono::system_clock::duration& duration);
    static double toMicroseconds(const std::chrono::system_clock::duration& duration);
};

} // namespace nlm