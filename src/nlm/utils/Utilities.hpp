// Utility functions for advanced interface
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// String utilities
class StringUtils {
public:
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string getTimeString(const std::chrono::system_clock::time_point& time);
    static std::string getDurationString(const std::chrono::system_clock::time_point& start,
                                         const std::chrono::system_clock::time_point& end);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static bool isNumber(const std::string& str);
    static bool isAlpha(const std::string& str);
    static bool isAlphanumeric(const std::string& str);
    static std::string formatString(const std::string& format, ...);
};

// Math utilities
class MathUtils {
public:
    static double clamp(double value, double min, double max);
    static double lerp(double a, double b, double t);
    static double map(double value, double inMin, double inMax, double outMin, double outMax);
    static double normalize(double value, double min, double max);
    static double randomDouble(double min = 0.0, double max = 1.0);
    static int randomInt(int min = 0, int max = 100);
    static double sigmoid(double x);
    static double tanh(double x);
    static double relu(double x);
    static double abs(double x);
    static double pow(double base, double exp);
    static double log(double x);
    static double sqrt(double x);
    static bool isEqual(double a, double b, double epsilon = 1e-6);
    static double round(double value, int decimals = 0);
    static double floor(double value);
    static double ceil(double value);
    static double mod(double a, double b);
};

// File utilities
class FileUtils {
public:
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    static uintmax_t fileSize(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static std::string getDirectory(const std::string& path);
    static std::vector<std::string> getFilesInDirectory(const std::string& directory);
    static std::vector<std::string> getFilesWithExtension(const std::string& directory, const std::string& extension);
    static bool readFile(const std::string& path, std::string& content);
    static bool writeFile(const std::string& path, const std::string& content, bool overwrite = false);
    static bool appendFile(const std::string& path, const std::string& content);
    static bool deleteFile(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool copyFile(const std::string& source, const std::string& destination);
    static bool moveFile(const std::string& source, const std::string& destination);
    static std::string getTempDirectory();
    static std::string getCurrentWorkingDirectory();
    static bool changeWorkingDirectory(const std::string& path);
};

// Collection utilities
class CollectionUtils {
public:
    template<typename T>
    static std::vector<T> removeDuplicates(const std::vector<T>& collection);
    
    template<typename T>
    static std::vector<T> filter(const std::vector<T>& collection, const std::function<bool(const T&)>& predicate);
    
    template<typename T, typename U>
    static std::vector<U> map(const std::vector<T>& collection, const std::function<U(const T&)>& func);
    
    template<typename T>
    static T reduce(const std::vector<T>& collection, const std::function<T(const T&, const T&)>& func, T initial);
    
    template<typename T, typename U>
    static std::vector<U> flatten(const std::vector<std::vector<T>>& collection, const std::function<U(const T&)>& func);
    
    template<typename T>
    static bool contains(const std::vector<T>& collection, const T& value);
    
    template<typename T>
    static size_t count(const std::vector<T>& collection, const std::function<bool(const T&)>& predicate);
    
    template<typename K, typename V>
    static V getWithDefault(const std::unordered_map<K, V>& map, const K& key, const V& defaultValue);
    
    template<typename K, typename V>
    static bool containsKey(const std::unordered_map<K, V>& map, const K& key);
    
    template<typename T>
    static std::vector<T> sort(const std::vector<T>& collection, const std::function<bool(const T&, const T&)>& compare);
    
    template<typename T>
    static void sortInPlace(std::vector<T>& collection, const std::function<bool(const T&, const T&)>& compare);
    
    template<typename T>
    static std::vector<T> shuffle(const std::vector<T>& collection);
    
    template<typename T>
    static void shuffleInPlace(std::vector<T>& collection);
    
    template<typename T>
    static T max(const std::vector<T>& collection);
    
    template<typename T>
    static T min(const std::vector<T>& collection);
    
    template<typename T>
    static double average(const std::vector<T>& collection);
    
    template<typename T>
    static double median(const std::vector<T>& collection);
    
    template<typename T>
    static double stddev(const std::vector<T>& collection);
    
    template<typename T>
    static T mode(const std::vector<T>& collection);
    
    template<typename T>
    static std::vector<T> unique(const std::vector<T>& collection);
    
    template<typename T>
    static std::vector<T> intersect(const std::vector<T>& a, const std::vector<T>& b);
    
    template<typename T>
    static std::vector<T> difference(const std::vector<T>& a, const std::vector<T>& b);
    
    template<typename T>
    static std::vector<T> union(const std::vector<T>& a, const std::vector<T>& b);
};

// Time utilities
class TimeUtils {
public:
    static std::chrono::system_clock::time_point now();
    static double elapsedSeconds(const std::chrono::system_clock::time_point& start);
    static double elapsedMilliseconds(const std::chrono::system_clock::time_point& start);
    static std::string formatTime(const std::chrono::system_clock::time_point& time, const std::string& format = "%Y-%m-%d %H:%M:%S");
    static std::chrono::system_clock::time_point addTime(const std::chrono::system_clock::time_point& time, const std::chrono::system_clock::duration& duration);
    static bool wait(double seconds);
    static bool waitUntil(const std::chrono::system_clock::time_point& timePoint);
    static double parseTime(const std::string& timeStr, const std::string& format = "%H:%M:%S");
    static std::string currentTimeString();
    static std::string currentDateString();
    static std::string currentDateTimeString();
    static std::string currentTimestamp();
};

// JSON utilities
class JsonUtils {
public:
    static std::string objectToJson(const std::unordered_map<std::string, std::string>& obj);
    static std::string objectToJson(const std::unordered_map<std::string, double>& obj);
    static std::string objectToJson(const std::unordered_map<std::string, int>& obj);
    static std::string objectToJson(const std::unordered_map<std::string, bool>& obj);
    static std::string objectToJson(const std::unordered_map<std::string, std::vector<std::string>>& obj);
    static std::string objectToJson(const std::unordered_map<std::string, std::vector<double>>& obj);
    static std::string objectToJson(const std::unordered_map<std::string, std::vector<int>>& obj);
    static bool parseJson(const std::string& json, std::unordered_map<std::string, std::string>& obj);
    static bool parseJson(const std::string& json, std::unordered_map<std::string, double>& obj);
    static bool parseJson(const std::string& json, std::unordered_map<std::string, int>& obj);
    static bool parseJson(const std::string& json, std::unordered_map<std::string, bool>& obj);
    static std::string prettyPrintJson(const std::string& json);
    static std::string minifyJson(const std::string& json);
    static bool validateJson(const std::string& json);
    static std::string getJsonValue(const std::string& json, const std::string& key);
    static bool setJsonValue(std::string& json, const std::string& key, const std::string& value);
    static bool mergeJson(std::string& baseJson, const std::string& patchJson);
};

// Color utilities
class ColorUtils {
public:
    enum class Color {
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
        GRAY,
        BRIGHT_RED,
        BRIGHT_GREEN,
        BRIGHT_YELLOW,
        BRIGHT_BLUE,
        BRIGHT_MAGENTA,
        BRIGHT_CYAN,
        BRIGHT_WHITE
    };
    
    static std::string colorize(const std::string& text, Color color);
    static std::string colorize(const std::string& text, const std::string& hexCode);
    static std::string resetColor();
    static std::string bold(const std::string& text);
    static std::string underline(const std::string& text);
    static std::string italic(const std::string& text);
    static std::string bright(const std::string& text);
    static std::string dim(const std::string& text);
    static std::string getColorCode(Color color);
    static std::string getHexFromColor(Color color);
    static Color getColorFromHex(const std::string& hex);
    static std::string rgbToHex(int r, int g, int b);
    static std::string interpolateColor(const std::string& color1, const std::string& color2, double ratio);
    static std::string blendColors(const std::vector<std::string>& colors, double ratio = 0.5);
    static std::string getContrastColor(const std::string& backgroundColor);
};

// Pattern matching utilities
class PatternUtils {
public:
    static bool matchPattern(const std::string& text, const std::string& pattern);
    static bool matchGlob(const std::string& text, const std::string& pattern);
    static bool matchRegex(const std::string& text, const std::string& regex);
    static std::string replacePattern(const std::string& text, const std::string& pattern, const std::string& replacement);
    static std::vector<std::string> extractPatterns(const std::string& text, const std::string& pattern);
    static bool startsWithAny(const std::string& text, const std::vector<std::string>& prefixes);
    static bool endsWithAny(const std::string& text, const std::vector<std::string>& suffixes);
    static bool containsAny(const std::string& text, const std::vector<std::string>& substrings);
    static std::string sanitizeFilename(const std::string& filename);
    static std::string generateUniqueId();
    static std::string generateHash(const std::string& input);
    static bool isValidEmail(const std::string& email);
    static bool isValidUrl(const std::string& url);
    static bool isValidIpAddress(const std::string& ip);
    static bool isValidPort(int port);
    static std::string escapeHtml(const std::string& text);
    static std::string escapeSql(const std::string& text);
    static std::string escapeRegex(const std::string& text);
    static std::string escapeShell(const std::string& text);
    static std::string escapeJson(const std::string& text);
    static std::string escapeXml(const std::string& text);
};

} // namespace nlm