// ConfigScripting.cpp - Implementation of the configuration scripting system

#include "core/Scripting/ConfigScripting.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Implementation of ConfigParser methods (virtual, no implementation needed)

// Implementation of JSONConfigParser methods

// All methods are implemented in the header file for performance

// Implementation of YAMLConfigParser methods

// All methods are implemented in the header file for performance

// Implementation of ConfigManager methods

// Implementation of ConfigTemplate methods

// Implementation of ExperimentConfigBuilder methods

// Helper function to escape JSON strings
std::string escapeJSON(const std::string& str) {
    std::string result;
    result.reserve(str.length() + 2);
    
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

} // namespace nlm