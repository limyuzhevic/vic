// ============================================================================
// CONFIGURATION AND CONSTANT ACCESS UTILITIES IMPLEMENTATION
// ============================================================================

#include "Utilities.hpp"

namespace nlm {

// ============================================================================
// CONFIGURATION AND CONSTANT ACCESS UTILITIES IMPLEMENTATION
// ============================================================================

// Configuration value retrieval template implementation
template<typename T>
T getConfigValue(const Config& config, const std::string& key, const T& defaultValue) {
    if (config.has(key)) {
        // Implementation would depend on Config interface
        // For demonstration, we'll return default value
        // In a real implementation, this would parse the config value
        return defaultValue;
    }
    return defaultValue;
}

// Constant value retrieval template implementation
template<typename T>
T getConstantValue(const std::string& constantName) {
    // This implementation would depend on the Constants interface
    // For demonstration, throw exception if not found
    // In a real implementation, this would lookup from Constants.hpp
    throw std::invalid_argument("Constant " + constantName + " not found");
}

} // namespace nlm
