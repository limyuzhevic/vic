#include <map>
#include <string>
#include <vector>
#include <optional>

namespace nlm {

// Helper function to split a key into parts using dot notation
inline std::vector<std::string> splitKey(const std::string& key) {
    std::vector<std::string> parts;
    std::string part;
    for (char c : key) {
        if (c == '.') {
            if (!part.empty()) {
                parts.push_back(part);
                part.clear();
            }
        } else {
            part += c;
        }
    }
    if (!part.empty()) {
        parts.push_back(part);
    }
    return parts;
}

// Helper function to join key parts with dots
inline std::string joinKey(const std::vector<std::string>& parts) {
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) result += ".";
        result += parts[i];
    }
    return result;
}

// Inlined helper functions in Config.hpp
inline std::vector<std::string> Config::splitKey(const std::string& key) {
    return splitKey(key);
}

inline std::string Config::joinKey(const std::vector<std::string>& parts) {
    return joinKey(parts);
}

} // namespace nlm
