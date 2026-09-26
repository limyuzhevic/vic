// NLM Utils Implementation
// String Utilities

#include "core/utils/Utils.hpp"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace utils {

// StringUtils implementation
std::string StringUtils::trim(const std::string& str, bool left, bool right) {
    std::string result = str;
    
    if (left) {
        result.erase(result.begin(), std::find_if(result.begin(), result.end(),
            [](unsigned char ch) { return !std::isspace(ch); }));
    }
    
    if (right) {
        result.erase(std::find_if(result.rbegin(), result.rend(),
            [](unsigned char ch) { return !std::isspace(ch); }).base(), result.end());
    }
    
    return result;
}

std::string StringUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char ch) { return std::tolower(ch); });
    return result;
}

std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char ch) { return std::toupper(ch); });
    return result;
}

std::string StringUtils::capitalize(const std::string& str) {
    if (str.empty()) return str;
    
    std::string result = str;
    result[0] = std::toupper(result[0]);
    return result;
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
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

std::string StringUtils::join(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) result += delimiter;
        result += strings[i];
    }
    return result;
}

size_t StringUtils::countOccurrences(const std::string& str, const std::string& substring) {
    if (substring.empty()) return 0;
    
    size_t count = 0;
    size_t pos = 0;
    while ((pos = str.find(substring, pos)) != std::string::npos) {
        ++count;
        pos += substring.size();
    }
    return count;
}

bool StringUtils::isValidFilename(const std::string& filename) {
    if (filename.empty() || filename.find('/') != std::string::npos || filename.find('\\') != std::string::npos) {
        return false;
    }
    
    for (char ch : filename) {
        if (ch == ':' || ch == '*' || ch == '?' || ch == '"' || ch == '<' || ch == '>' || ch == '|') {
            return false;
        }
    }
    return true;
}

bool StringUtils::isAlphaNumeric(const std::string& str) {
    return std::all_of(str.begin(), str.end(),
                      [](char ch) { return std::isalnum(ch); });
}

bool StringUtils::isWhitespace(const std::string& str) {
    return std::all_of(str.begin(), str.end(),
                      [](char ch) { return std::isspace(ch); });
}

std::string StringUtils::formatNumber(int number, const std::string& locale) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

std::string StringUtils::formatFloat(float value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string StringUtils::formatPercentage(float value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << (value * 100.0) << "%";
    return ss.str();
}

std::string StringUtils::formatTime(double seconds) {
    if (seconds < 60.0) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << seconds << "s";
        return ss.str();
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        double remaining = seconds - minutes * 60.0;
        std::stringstream ss;
        ss << minutes << "m " << std::fixed << std::setprecision(3) << remaining << "s";
        return ss.str();
    } else {
        int hours = static_cast<int>(seconds / 3600.0);
        double remaining = seconds - hours * 3600.0;
        int minutes = static_cast<int>(remaining / 60.0);
        remaining -= minutes * 60.0;
        std::stringstream ss;
        ss << hours << "h " << minutes << "m " << std::fixed << std::setprecision(3) << remaining << "s";
        return ss.str();
    }
}

std::string StringUtils::hashString(const std::string& str, const std::string& algorithm) {
    // Simple hash implementation
    std::hash<std::string> hasher;
    return std::to_string(hasher(str));
}

std::string StringUtils::urlEncode(const std::string& str) {
    std::ostringstream escaped;
    escaped.fill('0');
    
    for (std::string::const_iterator i = str.begin(), end = str.end(); i != end; ++i) {
        std::string::value_type c = (*i);
        
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << std::uppercase;
            escaped << std::hex << static_cast<int>(static_cast<unsigned char>(c));
            escaped << std::nouppercase;
        }
    }
    
    return escaped.str();
}

std::string StringUtils::urlDecode(const std::string& str) {
    std::ostringstream decoded;
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '%' && i + 2 < str.size()) {
            int hexCode;
            std::istringstream hexStream(str.substr(i + 1, 2));
            if (hexStream >> std::hex >> hexCode) {
                decoded << static_cast<char>(hexCode);
                i += 2;
            } else {
                decoded << str[i];
            }
        } else if (str[i] == '+') {
            decoded << ' ';
        } else {
            decoded << str[i];
        }
    }
    
    return decoded.str();
}

std::string StringUtils::base64Encode(const std::string& str) {
    // Simple base64 implementation (not for production use)
    static const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string result;
    int val = 0, valb = -6;
    
    for (unsigned char c : str) {
        val = (val << 8) + c;
        valb += 6;
        while (valb >= 0) {
            result.push_back(base64Chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6) {
        result.push_back(base64Chars[(val << 8) & 0x3F]);
    }
    
    while (result.size() % 4) {
        result.push_back('=');
    }
    
    return result;
}

std::string StringUtils::base64Decode(const std::string& str) {
    // Simple base64 implementation (not for production use)
    static const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string result;
    int val = 0, valb = -8;
    
    for (char c : str) {
        if (c == '=') break;
        
        size_t pos = base64Chars.find(c);
        if (pos == std::string::npos) continue;
        
        val = (val << 6) + pos;
        valb += 6;
        if (valb >= 0) {
            result.push_back(static_cast<char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    
    return result;
}

} // namespace utils
} // namespace nlm
