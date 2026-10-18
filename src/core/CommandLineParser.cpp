#include "CommandLineParser.hpp"
#include <algorithm>
#include <cctype>

namespace nlm {

// Helper function to trim whitespace
std::string trim(const std::string& str) {
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

} // namespace nlm