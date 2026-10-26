#include "core/Utilities.hpp"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <random>
#include <iomanip>
#include <cstdarg>
#include <cstdio>
#include <regex>

namespace nlm {
namespace string_utils {
    
    String trim(const String& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == String::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    String toLower(const String& str) {
        String result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    String toUpper(const String& str) {
        String result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    }
    
    String capitalize(const String& str) {
        if (str.empty()) return str;
        String result = toLower(str);
        result[0] = std::toupper(result[0]);
        return result;
    }
    
    String capitalizeFirst(const String& str) {
        if (str.empty()) return str;
        String result = str;
        result[0] = std::toupper(result[0]);
        return result;
    }
    
    bool startsWith(const String& str, const String& prefix) {
        return str.size() >= prefix.size() && 
               std::equal(prefix.begin(), prefix.end(), str.begin());
    }
    
    bool endsWith(const String& str, const String& suffix) {
        return str.size() >= suffix.size() && 
               std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    }
    
    String removeExtension(const String& filename) {
        size_t pos = filename.find_last_of(".");
        if (pos == String::npos) return filename;
        return filename.substr(0, pos);
    }
    
    String getFileExtension(const String& filename) {
        size_t pos = filename.find_last_of(".");
        if (pos == String::npos) return "";
        return filename.substr(pos + 1);
    }
    
    String getFileNameWithoutExtension(const String& filename) {
        size_t pos = filename.find_last_of(".");
        if (pos == String::npos) return filename;
        return filename.substr(0, pos);
    }
    
    String escapeHtml(const String& str) {
        String result;
        for (char c : str) {
            switch (c) {
                case '&': result += "&amp;"; break;
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&#39;"; break;
                default: result += c;
            }
        }
        return result;
    }
    
    String replaceAll(const String& str, const String& from, const String& to) {
        String result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != String::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }
    
    std::vector<String> split(const String& str, char delimiter) {
        std::vector<String> tokens;
        std::stringstream ss(str);
        String token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(trim(token));
        }
        return tokens;
    }
    
    std::vector<String> split(const String& str, const String& delimiter) {
        std::vector<String> tokens;
        if (delimiter.empty()) {
            tokens.push_back(str);
            return tokens;
        }
        
        size_t pos = 0;
        String token;
        while ((pos = str.find(delimiter)) != String::npos) {
            token = str.substr(0, pos);
            tokens.push_back(trim(token));
            str.erase(0, pos + delimiter.length());
        }
        tokens.push_back(trim(str));
        return tokens;
    }
    
    String join(const std::vector<String>& strings, const String& delimiter) {
        if (strings.empty()) return "";
        String result = strings[0];
        for (size_t i = 1; i < strings.size(); ++i) {
            result += delimiter + strings[i];
        }
        return result;
    }
    
    bool iequals(const String& a, const String& b) {
        if (a.size() != b.size()) return false;
        return std::equal(a.begin(), a.end(), b.begin(),
                         [](char a, char b) { return std::tolower(a) == std::tolower(b); });
    }
    
    String generateUUID() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<int> dis(0, 15);
        static const char hexDigits[] = "0123456789abcdef";
        
        String uuid;
        uuid.reserve(36); // Standard UUID length is 36 characters
        for (int i = 0; i < 8; ++i) {
            uuid += hexDigits[dis(gen)];
        }
        uuid += "-";
        for (int i = 0; i < 4; ++i) {
            uuid += hexDigits[dis(gen)];
        }
        uuid += "-";
        for (int i = 0; i < 4; ++i) {
            uuid += hexDigits[dis(gen)];
        }
        uuid += "-";
        for (int i = 0; i < 4; ++i) {
            uuid += hexDigits[dis(gen)];
        }
        uuid += "-";
        for (int i = 0; i < 12; ++i) {
            uuid += hexDigits[dis(gen)];
        }
        return uuid;
    }
    
    String format(const String& format, ...) {
        va_list args;
        va_start(args, format);
        
        // First pass to get buffer size
        va_list argsCopy;
        va_copy(argsCopy, args);
        int size = std::vsnprintf(nullptr, 0, format.c_str(), argsCopy) + 1;
        va_end(argsCopy);
        
        std::vector<char> buffer(size);
        std::vsnprintf(buffer.data(), size, format.c_str(), args);
        va_end(args);
        
        return String(buffer.data());
    }
    
    String formatv(const String& format, std::vector<String> args) {
        String result = format;
        for (size_t i = 0; i < args.size(); ++i) {
            String placeholder = "{" + std::to_string(i) + "}";
            size_t pos = result.find(placeholder);
            while (pos != String::npos) {
                result.replace(pos, placeholder.length(), args[i]);
                pos = result.find(placeholder, pos + args[i].length());
            }
        }
        return result;
    }
    
    bool isDigits(const String& str) {
        return std::all_of(str.begin(), str.end(), ::isdigit);
    }
    
    int toInt(const String& str, int defaultValue) {
        try {
            return std::stoi(str);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    double toDouble(const String& str, double defaultValue) {
        try {
            return std::stod(str);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    bool toBool(const String& str, bool defaultValue) {
        String lower = toLower(str);
        if (lower == "true" || lower == "yes" || lower == "1") return true;
        if (lower == "false" || lower == "no" || lower == "0") return false;
        return defaultValue;
    }
    
} // namespace string_utils
} // namespace nlm