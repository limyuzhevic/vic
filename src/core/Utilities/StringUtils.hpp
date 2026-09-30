#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <chrono>
#include <random>

namespace nlm {
namespace Core {
namespace Utilities {

/**
 * String manipulation utility functions for neural network simulations
 */
class StringUtils {
public:
    // Case conversion
    static std::string toUpper(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string capitalize(const std::string& str);
    static std::string capitalizeFirst(const std::string& str);
    static std::string titleCase(const std::string& str);

    // Trimming and formatting
    static std::string trim(const std::string& str);
    static std::string ltrim(const std::string& str);
    static std::string rtrim(const std::string& str);
    static std::string padLeft(const std::string& str, size_t width, char padding = ' ');
    static std::string padRight(const std::string& str, size_t width, char padding = ' ');
    static std::string center(const std::string& str, size_t width, char padding = ' ');

    // String manipulation
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    static std::string remove(const std::string& str, const std::string& chars);
    static std::string removeAll(const std::string& str, const std::string& chars);
    static std::string substring(const std::string& str, size_t start, size_t length = -1);
    static std::string left(const std::string& str, size_t length);
    static std::string right(const std::string& str, size_t length);
    static std::string repeat(const std::string& str, size_t count);

    // Splitting and joining
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    static std::vector<std::string> splitWhitespace(const std::string& str);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static std::string join(const std::string& delimiter, const std::vector<std::string>& strings);
    static std::string join(const std::vector<std::string>& strings);

    // Formatting
    static std::string format(const std::string& format, ...);
    static std::string formatNumber(int number, char thousandsSeparator = ',');
    static std::string formatNumber(double number, int precision = 2, char decimalSeparator = '.');
    static std::string formatTime(double seconds);
    static std::string formatBytes(size_t bytes);
    static std::string formatPercentage(double ratio, int precision = 2);

    // String inspection
    static bool contains(const std::string& str, const std::string& substring);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static bool isEmpty(const std::string& str);
    static bool isBlank(const std::string& str);
    static size_t countOccurrences(const std::string& str, const std::string& substring);
    static size_t findFirstOf(const std::string& str, const std::string& chars);
    static size_t findLastOf(const std::string& str, const std::string& chars);
    static bool isAlpha(const std::string& str);
    static bool isDigit(const std::string& str);
    static bool isAlphanumeric(const std::string& str);
    static bool isLowercase(const std::string& str);
    static bool isUppercase(const std::string& str);

    // Special string operations
    static std::string generateUUID();
    static std::string generateRandomString(size_t length);
    static std::string generateRandomString(size_t length, 
                                           const std::string& charset);
    static std::string reverse(const std::string& str);
    static std::string toHex(const std::string& input);
    static std::string fromHex(const std::string& input);
    static std::string toBase64(const std::string& input);
    static std::string fromBase64(const std::string& input);

    // Neural network specific
    static std::string formatNeurons(size_t count);
    static std::string formatConnections(size_t count);
    static std::string formatWeights(double weight);
    static std::string formatActivations(const std::vector<double>& activations);
    static std::string formatErrors(const std::vector<double>& errors);

    // String comparison
    static int compareIgnoreCase(const std::string& a, const std::string& b);
    static bool equalsIgnoreCase(const std::string& a, const std::string& b);
    static bool startsWithIgnoreCase(const std::string& str, const std::string& prefix);
    static bool endsWithIgnoreCase(const std::string& str, const std::string& suffix);

    // Safe string operations
    static std::string safeSubstring(const std::string& str, size_t start, size_t length = -1);
    static std::string safeReplace(const std::string& str, const std::string& from, 
                                  const std::string& to, bool all = false);
    static std::string safeJoin(const std::vector<std::string>& strings, 
                               const std::string& delimiter = "");

    // Neural network naming conventions
    static std::string formatRegionName(const std::string& prefix, size_t id);
    static std::string formatConnectionName(const std::string& from, const std::string& to);
    static std::string formatSynapseName(size_t sourceId, size_t targetId);
    static std::string getDefaultName(const std::string& type, size_t id);

    // Encoding and decoding
    static std::string escapeSpecialChars(const std::string& input);
    static std::string unescapeSpecialChars(const std::string& input);
    static std::string normalizeWhitespace(const std::string& input);
    static std::string removeBOM(const std::string& input);

    // Performance optimization
    static std::string formatEfficient(const std::string& format, ...);
    static std::string formatToStream(const std::string& format, std::ostream& stream);
    static void formatToBuffer(std::string& buffer, const std::string& format, ...);

    // Memory efficiency
    static std::string createSharedString(const std::string& str);
    static std::string internString(const std::string& str);

    // String stream utilities
    static std::string stringify(int value);
    static std::string stringify(double value);
    static std::string stringify(bool value);
    static std::string stringify(const std::string& value);
    static std::string stringify(const std::vector<double>& values);
    static std::string stringify(const std::vector<int>& values);

    // Neural network parameter strings
    static std::string formatLearningRate(double rate);
    static std::string formatThreshold(double threshold);
    static std::string formatWeight(double weight);
    static std::string formatBias(double bias);
    static std::string formatError(double error);
    static std::string formatActivation(double activation);

    // String statistics
    static double calculateEntropy(const std::string& str);
    static double calculateGini(const std::string& str);
    static size_t calculateLength(const std::string& str);
    static size_t calculateUniqueChars(const std::string& str);
};

} // namespace Utils
} // namespace Core
} // namespace nlm