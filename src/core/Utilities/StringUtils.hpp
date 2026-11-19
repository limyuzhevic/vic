#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <limits>

namespace nlm {

// Helper function to convert strings to different types

template<typename T>
T convertToType(const std::string& str, const T& defaultValue) {
    std::stringstream ss(str);
    T result;
    if (ss >> result) {
        // Check if there are remaining characters that aren't whitespace
        char remaining;
        if (ss >> remaining) {
            return defaultValue; // Extra characters, return default
        }
        return result;
    }
    return defaultValue;
}

// Template specializations for common types

template<>
inline int64_t convertToType<int64_t>(const std::string& str, const int64_t& defaultValue) {
    try {
        size_t pos;
        int64_t result = std::stoll(str, &pos);
        if (pos == str.size()) {
            return result;
        }
    } catch (const std::exception&) {}
    return defaultValue;
}

template<>
inline float convertToType<float>(const std::string& str, const float& defaultValue) {
    try {
        size_t pos;
        float result = std::stof(str, &pos);
        if (pos == str.size()) {
            return result;
        }
    } catch (const std::exception&) {}
    return defaultValue;
}

template<>
inline double convertToType<double>(const std::string& str, const double& defaultValue) {
    try {
        size_t pos;
        double result = std::stod(str, &pos);
        if (pos == str.size()) {
            return result;
        }
    } catch (const std::exception&) {}
    return defaultValue;
}

template<>
inline std::string convertToType<std::string>(const std::string& str, const std::string&) {
    return str;
}

// Helper function to trim whitespace
inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Helper function to check if string is boolean
template<>
inline bool convertToType<bool>(const std::string& str, const bool& defaultValue) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lowerStr == "true" || lowerStr == "1" || lowerStr == "yes" || 
        lowerStr == "on" || lowerStr == "enable") {
        return true;
    } else if (lowerStr == "false" || lowerStr == "0" || lowerStr == "no" || 
               lowerStr == "off" || lowerStr == "disable") {
        return false;
    }
    
    return defaultValue;
}

} // namespace nlm
