#pragma once

#include <string>

namespace nlm {

class StringUtils {
public:
    // String manipulation functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static bool contains(const std::string& str, const std::string& substr);
    static std::string replaceAll(std::string str, const std::string& from, const std::string& to);
    static std::string escapeRegex(const std::string& str);
    
    // Generation functions
    static std::string generateRandomString(size_t length);
    static std::string generateCorrelationId();
};

} // namespace nlm