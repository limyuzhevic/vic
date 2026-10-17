// Utilities implementation for NLM
// Implementation of reusable utility functions

#include "Utilities.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace utils {

// StringUtils implementations
std::string StringUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string StringUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool StringUtils::isAlphanumeric(const std::string& str) {
    return std::all_of(str.begin(), str.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    });
}

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

std::string StringUtils::join(const std::vector<std::string>& strings, char delimiter) {
    if (strings.empty()) return "";
    
    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) ss << delimiter;
        ss << strings[i];
    }
    
    return ss.str();
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && 
           str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string StringUtils::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

// ConfigUtils implementations
bool ConfigUtils::isValidKey(const std::string& key) {
    if (key.empty()) return false;
    return std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    });
}

std::pair<std::string, std::string> ConfigUtils::parseKeyValue(const std::string& line) {
    std::pair<std::string, std::string> result;
    size_t pos = line.find('=');
    
    if (pos != std::string::npos) {
        result.first = trim(line.substr(0, pos));
        result.second = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (result.second.size() >= 2 && 
            ((result.second.front() == '"' && result.second.back() == '"') ||
             (result.second.front() == '\'' && result.second.back() == '\''))) {
            result.second = result.second.substr(1, result.second.size() - 2);
        }
    }
    
    return result;
}

bool ConfigUtils::inRange(double value, double min, double max) {
    return value >= min && value <= max;
}

bool ConfigUtils::matchesPattern(const std::string& value, const std::string& pattern) {
    if (pattern == "alphanumeric") {
        return isAlphanumeric(value);
    } else if (pattern == "alphanumeric_with_underscores") {
        return std::all_of(value.begin(), value.end(), [](char c) {
            return std::isalnum(c) || c == '_';
        });
    }
    return true; // Default: accept any pattern
}

// TimingUtils implementations
TimingUtils::BenchmarkTimer::BenchmarkTimer() {
    start_time = std::chrono::high_resolution_clock::now();
}

double TimingUtils::BenchmarkTimer::elapsed_ms() const {
    auto end_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end_time - start_time).count();
}

void TimingUtils::BenchmarkTimer::reset() {
    start_time = std::chrono::high_resolution_clock::now();
}

std::string TimingUtils::formatDuration(double milliseconds) {
    if (milliseconds < 1.0) {
        return std::to_string(milliseconds) + " ms";
    } else if (milliseconds < 1000.0) {
        return std::to_string(milliseconds) + " ms";
    } else {
        double seconds = milliseconds / 1000.0;
        if (seconds < 60.0) {
            return std::to_string(seconds) + " s";
        } else {
            double minutes = seconds / 60.0;
            return std::to_string(minutes) + " min";
        }
    }
}

// MemoryUtils implementations
void MemoryUtils::safeAssign(std::string& target, const std::string& source, size_t max_length) {
    if (source.length() <= max_length) {
        target = source;
    } else {
        target = source.substr(0, max_length);
    }
}

// MathUtils implementations
double MathUtils::clamp(double value, double min, double max) {
    return std::max(min, std::min(max, value));
}

double MathUtils::lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

double MathUtils::mapRange(double value, double in_min, double in_max, double out_min, double out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double MathUtils::normalize(double value, double min, double max) {
    if (max == min) return 0.0;
    return (value - min) / (max - min);
}

double MathUtils::mean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double MathUtils::standardDeviation(const std::vector<double>& values, double mean) {
    if (values.empty()) return 0.0;
    double sum = 0.0;
    for (double value : values) {
        sum += (value - mean) * (value - mean);
    }
    return std::sqrt(sum / values.size());
}

bool MathUtils::approximatelyEqual(double a, double b, double epsilon) {
    return std::abs(a - b) <= epsilon;
}

double MathUtils::roundToNearest(double value, double multiple) {
    return std::round(value / multiple) * multiple;
}

} // namespace utils
} // namespace nlm
