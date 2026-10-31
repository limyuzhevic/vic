#include "CommonUtils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <cmath>
#include <vector>
#include <map>
#include <iomanip>
#include <cstdarg>
#include <cstring>
#include <random>
#include <sstream>
#include <iomanip>
#include <regex>
#include <codecvt>
#include <locale>

namespace nlm {

// ErrorHandler implementation
std::vector<ErrorHandler::ErrorInfo> ErrorHandler::errorHistory;
std::function<void(ErrorLevel, const std::string&, const std::string&)> ErrorHandler::errorHandler = nullptr;

void ErrorHandler::logError(ErrorLevel level, const std::string& message, const std::string& source) {
    ErrorInfo info(level, message, source);
    errorHistory.push_back(info);
    
    if (errorHandler) {
        errorHandler(level, message, source);
    }
    
    // Also print to stderr for critical errors
    if (level == ErrorLevel::FATAL || level == ErrorLevel::ERROR) {
        std::cerr << "[ERROR] " << message << std::endl;
        if (!source.empty()) {
            std::cerr << "  Source: " << source << std::endl;
        }
    }
}

void ErrorHandler::logDebug(const std::string& message, const std::string& source) {
    logError(ErrorLevel::DEBUG, message, source);
}

void ErrorHandler::logInfo(const std::string& message, const std::string& source) {
    logError(ErrorLevel::INFO, message, source);
}

void ErrorHandler::logWarning(const std::string& message, const std::string& source) {
    logError(ErrorLevel::WARNING, message, source);
}

void ErrorHandler::logError(const std::string& message, const std::string& source) {
    logError(ErrorLevel::ERROR, message, source);
}

void ErrorHandler::logFatal(const std::string& message, const std::string& source) {
    logError(ErrorLevel::FATAL, message, source);
}

std::vector<ErrorHandler::ErrorInfo> ErrorHandler::getRecentErrors(size_t limit) {
    if (errorHistory.size() <= limit) {
        return errorHistory;
    }
    
    size_t startIndex = errorHistory.size() - limit;
    return std::vector<ErrorInfo>(errorHistory.begin() + startIndex, errorHistory.end());
}

void ErrorHandler::clearErrorHistory() {
    errorHistory.clear();
}

void ErrorHandler::setErrorHandler(std::function<void(ErrorLevel, const std::string&, const std::string&)> handler) {
    errorHandler = std::move(handler);
}

// ValidationUtils implementation
bool ValidationUtils::isValidString(const std::string& str, size_t minLength, size_t maxLength) {
    if (str.length() < minLength || str.length() > maxLength) {
        return false;
    }
    return true;
}

bool ValidationUtils::isValidIdentifier(const std::string& identifier) {
    if (identifier.empty()) return false;
    if (!std::isalpha(identifier[0]) && identifier[0] != '_') return false;
    
    for (char c : identifier) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    return true;
}

bool ValidationUtils::isValidPath(const std::string& path) {
    if (path.empty()) return false;
    
    // Check for path traversal attempts
    if (path.find("..") != std::string::npos) return false;
    if (path.find("/") != std::string::npos || path.find("\\") != std::string::npos) {
        // Allow path separators, but check for suspicious patterns
        if (path.find("//") != std::string::npos || path.find("\\\\") != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

bool ValidationUtils::isValidRange(double value, double min, double max) {
    return value >= min && value <= max;
}

bool ValidationUtils::isValidPositive(int64_t value) {
    return value >= 0;
}

bool ValidationUtils::isValidPositive(double value) {
    return value >= 0.0;
}

bool ValidationUtils::isValidFloat(const std::string& str) {
    std::istringstream iss(str);
    double val;
    iss >> val;
    return !iss.fail() && iss.eof();
}

bool ValidationUtils::isValidConfigKey(const std::string& key) {
    if (key.empty()) return false;
    if (key.length() > 100) return false;
    
    // Check for valid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool ValidationUtils::isValidConfigValue(const std::string& key, const std::string& value) {
    // Basic validation - can be extended with key-specific validation
    if (value.empty()) return false;
    if (value.length() > 1000) return false;
    return true;
}

bool ValidationUtils::isValidStringArray(const std::vector<std::string>& array, size_t maxSize) {
    if (array.size() > maxSize) return false;
    
    for (const auto& str : array) {
        if (str.empty() || str.length() > 100) {
            return false;
        }
    }
    
    return true;
}

bool ValidationUtils::containsDuplicates(const std::vector<std::string>& array) {
    std::set<std::string> seen;
    for (const auto& str : array) {
        if (seen.find(str) != seen.end()) {
            return true;
        }
        seen.insert(str);
    }
    return false;
}

bool ValidationUtils::matchesPattern(const std::string& str, const std::string& pattern) {
    std::regex regex(pattern);
    return std::regex_match(str, regex);
}

bool ValidationUtils::isValidFileExtension(const std::string& filename, const std::vector<std::string>& extensions) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return false;
    
    std::string ext = filename.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
}

std::string ValidationUtils::trim(const std::string& str) {
    std::string result = str;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return std::isspace(ch);
    }));
    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base(), result.end());
    return result;
}

std::string ValidationUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ValidationUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool ValidationUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool ValidationUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> ValidationUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

std::string ValidationUtils::join(const std::vector<std::string>& parts, char delimiter) {
    std::stringstream ss;
    bool first = true;
    
    for (const auto& part : parts) {
        if (!first) ss << delimiter;
        ss << part;
        first = false;
    }
    
    return ss.str();
}

bool ValidationUtils::validateConfigValue(const std::string& key, const std::string& value) {
    // Key-specific validation
    if (key == "random_seed") {
        try {
            size_t pos;
            int64_t num = std::stoll(value, &pos);
            return pos == value.length() && num >= 0;
        } catch (...) {
            return false;
        }
    } else if (key == "simulation_timestep") {
        try {
            size_t pos;
            double num = std::stod(value, &pos);
            return pos == value.length() && num > 0.0 && num <= 1.0;
        } catch (...) {
            return false;
        }
    } else if (key == "neuron_count") {
        try {
            size_t pos;
            int64_t num = std::stoll(value, &pos);
            return pos == value.length() && num > 0 && num <= 100000;
        } catch (...) {
            return false;
        }
    }
    
    return true; // Default validation passes
}

bool ValidationUtils::validateConfigFile(const std::string& filepath) {
    if (!isValidPath(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as key=value format
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            return false; // Invalid format
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (!isValidConfigKey(key) || !isValidConfigValue(key, value)) {
            return false;
        }
    }
    
    return true;
}

bool ValidationUtils::safeStringToInt(const std::string& str, int64_t& result, int64_t min, int64_t max) {
    try {
        size_t pos;
        int64_t num = std::stoll(str, &pos);
        if (pos != str.length()) return false;
        if (num < min || num > max) return false;
        result = num;
        return true;
    } catch (...) {
        return false;
    }
}

bool ValidationUtils::safeStringToDouble(const std::string& str, double& result, double min, double max) {
    try {
        size_t pos;
        double num = std::stod(str, &pos);
        if (pos != str.length()) return false;
        if (num < min || num > max) return false;
        result = num;
        return true;
    } catch (...) {
        return false;
    }
}

bool ValidationUtils::safeStringToBool(const std::string& str, bool& result) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes") {
        result = true;
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no") {
        result = false;
        return true;
    }
    return false;
}

// StringUtils implementation
std::string StringUtils::trim(const std::string& str) {
    return ValidationUtils::trim(str);
}

std::string StringUtils::ltrim(const std::string& str) {
    size_t start = 0;
    while (start < str.length() && std::isspace(str[start])) {
        start++;
    }
    return str.substr(start);
}

std::string StringUtils::rtrim(const std::string& str) {
    size_t end = str.length();
    while (end > 0 && std::isspace(str[end - 1])) {
        end--;
    }
    return str.substr(0, end);
}

std::string StringUtils::toLower(const std::string& str) {
    return ValidationUtils::toLower(str);
}

std::string StringUtils::toUpper(const std::string& str) {
    return ValidationUtils::toUpper(str);
}

std::string StringUtils::capitalize(const std::string& str) {
    if (str.empty()) return str;
    
    std::string result = toLower(str);
    result[0] = std::toupper(result[0]);
    return result;
}

std::string StringUtils::camelCaseToSnakeCase(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (std::isupper(str[i])) {
            if (i > 0) result += '_';
            result += std::tolower(str[i]);
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string StringUtils::snakeCaseToCamelCase(const std::string& str) {
    std::string result;
    bool capitalizeNext = false;
    
    for (char c : str) {
        if (c == '_') {
            capitalizeNext = true;
        } else {
            if (capitalizeNext) {
                result += std::toupper(c);
                capitalizeNext = false;
            } else {
                result += c;
            }
        }
    }
    
    return result;
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return ValidationUtils::startsWith(str, prefix);
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return ValidationUtils::endsWith(str, suffix);
}

std::string StringUtils::repeat(const std::string& str, size_t count) {
    std::string result;
    for (size_t i = 0; i < count; ++i) {
        result += str;
    }
    return result;
}

std::string StringUtils::join(const std::vector<std::string>& parts, char delimiter) {
    return ValidationUtils::join(parts, delimiter);
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

std::string StringUtils::format(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    // Determine buffer size needed
    va_list args_copy;
    va_copy(args_copy, args);
    size_t bufferSize = std::vsnprintf(nullptr, 0, format.c_str(), args_copy) + 1;
    va_end(args_copy);
    
    std::vector<char> buffer(bufferSize);
    std::vsnprintf(buffer.data(), bufferSize, format.c_str(), args);
    va_end(args);
    
    return std::string(buffer.data());
}

std::string StringUtils::generateRandomString(size_t length) {
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, chars.length() - 1);
    
    std::string result;
    for (size_t i = 0; i < length; ++i) {
        result += chars[dis(gen)];
    }
    
    return result;
}

std::string StringUtils::generateUUID() {
    // Simple UUID generation (not cryptographically secure)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);
    
    std::stringstream ss;
    int count = 0;
    while (count < 32) {
        if (count == 8 || count == 12 || count == 16 || count == 20) {
            ss << "-";
        }
        int digit = dis(gen);
        if (count == 12 || count == 16 || count == 20 || count == 24) {
            digit = dis2(gen);
        }
        ss << std::hex << digit;
        count++;
    }
    
    return ss.str();
}

std::string StringUtils::hashString(const std::string& str) {
    // Simple hash (not secure)
    std::hash<std::string> hasher;
    return std::to_string(hasher(str));
}

std::string StringUtils::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return "";
    return filename.substr(dotPos + 1);
}

std::string StringUtils::getFileNameWithoutExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return filename;
    return filename.substr(0, dotPos);
}

std::string StringUtils::getFileName(const std::string& filepath) {
    size_t slashPos = filepath.find_last_of('/');
    size_t backslashPos = filepath.find_last_of('\\');
    size_t pos = std::max(slashPos, backslashPos);
    
    if (pos == std::string::npos) return filepath;
    return filepath.substr(pos + 1);
}

std::string StringUtils::getDirectoryName(const std::string& filepath) {
    size_t slashPos = filepath.find_last_of('/');
    size_t backslashPos = filepath.find_last_of('\\');
    size_t pos = std::max(slashPos, backslashPos);
    
    if (pos == std::string::npos) return "";
    return filepath.substr(0, pos);
}

} // namespace nlm