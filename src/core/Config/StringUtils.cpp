#include "StringUtils.hpp"
#include <algorithm>
#include <cctype>
#include <random>
#include <chrono>
#include <sstream>

namespace nlm {

std::string StringUtils::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    // Trim leading whitespace
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    // Trim trailing whitespace
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
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

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool StringUtils::contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

std::string StringUtils::replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.size(), to);
        pos += to.size();
    }
    return str;
}

std::string StringUtils::escapeRegex(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '^': oss << "\^"; break;
            case '$': oss << "\$"; break;
            case '.': oss << "\."; break;
            case '*': oss << "\*"; break;
            case '+': oss << "\+"; break;
            case '?': oss << "\?"; break;
            case '|': oss << "\|"; break;
            case '(': oss << "\("; break;
            case ')': oss << "\)"; break;
            case '[': oss << "\["; break;
            case ']': oss << "\]"; break;
            case '{': oss << "\{"; break;
            case '}': oss << "\}"; break;
            case '\\': oss << "\\\\"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string StringUtils::generateRandomString(size_t length) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);
    
    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += chars[dist(generator)];
    }
    
    return result;
}

std::string StringUtils::generateCorrelationId() {
    return generateRandomString(32);
}

} // namespace nlm