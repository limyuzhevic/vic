#pragma once

#include <string>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace nlm {

/**
 * String utility functions for common operations
 */
class StringUtils {
public:
    /**
     * Trim whitespace from both ends of a string
     * @param str Input string
     * @return Trimmed string
     */
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    /**
     * Convert string to lowercase
     * @param str Input string
     * @return Lowercase string
     */
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    /**
     * Convert string to uppercase
     * @param str Input string
     * @return Uppercase string
     */
    static std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    }
    
    /**
     * Check if string starts with prefix
     * @param str Input string
     * @param prefix Prefix to check
     * @return true if string starts with prefix
     */
    static bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && 
               str.compare(0, prefix.size(), prefix) == 0;
    }
    
    /**
     * Check if string ends with suffix
     * @param str Input string
     * @param suffix Suffix to check
     * @return true if string ends with suffix
     */
    static bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && 
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    /**
     * Split string by delimiter
     * @param str Input string
     * @param delimiter Delimiter character
     * @return Vector of split strings
     */
    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;
        
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
        
        return result;
    }
    
    /**
     * Join vector of strings with delimiter
     * @param strings Vector of strings to join
     * @param delimiter Delimiter string
     * @return Joined string
     */
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
        if (strings.empty()) return "";
        
        std::ostringstream oss;
        oss << strings[0];
        for (size_t i = 1; i < strings.size(); ++i) {
            oss << delimiter << strings[i];
        }
        
        return oss.str();
    }
    
    /**
     * Replace all occurrences of substring with replacement
     * @param str Input string
     * @param from Substring to replace
     * @param to Replacement string
     * @return String with replacements made
     */
    static std::string replaceAll(const std::string& str, const std::string& from, 
                                const std::string& to) {
        if (from.empty()) return str;
        
        std::string result = str;
        size_t pos = 0;
        
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        
        return result;
    }
    
    /**
     * Check if string contains only digits
     * @param str Input string
     * @return true if string contains only digits
     */
    static bool isDigitsOnly(const std::string& str) {
        return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
    }
    
    /**
     * Check if string contains only alphanumeric characters
     * @param str Input string
     * @return true if string is alphanumeric
     */
    static bool isAlphanumeric(const std::string& str) {
        return !str.empty() && std::all_of(str.begin(), str.end(), 
            [](unsigned char c) { return std::isalnum(c); });
    }
    
    /**
     * Parse integer from string with error checking
     * @param str Input string
     * @param out Reference to output integer
     * @param base Number base (default 10)
     * @return true if parsing succeeded
     */
    static bool parseInt(const std::string& str, int& out, int base = 10) {
        try {
            size_t pos;
            out = std::stoi(str, pos, base);
            return pos == str.size();
        } catch (const std::exception&) {
            return false;
        }
    }
    
    /**
     * Parse float from string with error checking
     * @param str Input string
     * @param out Reference to output float
     * @return true if parsing succeeded
     */
    static bool parseFloat(const std::string& str, float& out) {
        try {
            out = std::stof(str);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    /**
     * Parse double from string with error checking
     * @param str Input string
     * @param out Reference to output double
     * @return true if parsing succeeded
     */
    static bool parseDouble(const std::string& str, double& out) {
        try {
            out = std::stod(str);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    /**
     * Check if string equals another (case-insensitive)
     * @param str1 First string
     * @param str2 Second string
     * @return true if strings are equal (case-insensitive)
     */
    static bool equalsIgnoreCase(const std::string& str1, const std::string& str2) {
        return toLower(str1) == toLower(str2);
    }
    
    /**
     * Format string with arguments (similar to printf)
     * @param format Format string
     * @param args Variable arguments
     * @return Formatted string
     */
    template<typename... Args>
    static std::string format(const std::string& format, Args&&... args) {
        // Note: This is a simple implementation. For production, consider using
        // a proper formatting library like fmtlib
        return format; // TODO: Implement proper formatting
    }
};

} // namespace nlm
