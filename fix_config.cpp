#include "src/core/Config/Config.hpp"

// Add stream operators for types used in ConfigValue
namespace nlm {
    // Stream operator for std::vector<std::string>
    inline std::ostream& operator<<(std::ostream& oss, const std::vector<std::string>& vec) {
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
        return oss;
    }
    
    // Stream operator for std::vector<int>
    inline std::ostream& operator<<(std::ostream& oss, const std::vector<int>& vec) {
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss;
    }
    
    // Stream operator for std::vector<double>
    inline std::ostream& operator<<(std::ostream& oss, const std::vector<double>& vec) {
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss;
    }
}
