#include "core/ConfigPresets.hpp"
#include <algorithm>
#include <cctype>

namespace nlm {
    namespace config {
        // Helper function to convert string to lowercase
        std::string toLower(const std::string& str) {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return std::tolower(c); });
            return result;
        }
    }
    
    namespace config_utils {
        // Helper function to convert string to lowercase
        std::string toLower(const std::string& str) {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return std::tolower(c); });
            return result;
        }
    }
    
    // Add preset validation rules
    void validateConfig(const Config& config, std::vector<std::string>& errors) {
        errors.clear();
        
        auto validationResult = config.validate();
        if (!validationResult.valid) {
            errors = validationResult.errors;
        }
    }
    
    // Get configuration statistics
    ConfigUtils::ConfigStats getConfigStats(const Config& config) {
        ConfigUtils::ConfigStats stats;
        stats.totalEntries = config.getEntryCount();
        stats.validationScore = config.getValidationScore();
        stats.keys = config.getKeys();
        
        // Count entries by source
        for (const auto& source : {nlm::ConfigSource::Default, nlm::ConfigSource::File,
                                  nlm::ConfigSource::CommandLine, nlm::ConfigSource::Runtime}) {
            size_t count = config.getEntryCount(source);
            if (count > 0) {
                stats.entriesBySource[std::to_string(static_cast<int>(source))] = count;
            }
        }
        
        return stats;
    }

} // namespace nlm