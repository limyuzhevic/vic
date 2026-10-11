#include "AdvancedConfig.hpp"
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace nlm {

struct AdvancedConfig::Impl {
    std::unordered_map<std::string, double> performanceMetrics;
    std::unordered_map<std::string, size_t> statistics;
    std::string version;
    std::string author;
    std::string description;
};

AdvancedConfig::AdvancedConfig() : Config() {
    pImplAdvanced = std::make_unique<Impl>();
    pImplAdvanced->version = "1.0.0";
    pImplAdvanced->author = "";
    pImplAdvanced->description = "";
}

AdvancedConfig::~AdvancedConfig() = default;

AdvancedConfig::AdvancedConfig(AdvancedConfig&&) noexcept = default;
AdvancedConfig& AdvancedConfig::operator=(AdvancedConfig&&) noexcept = default;

std::string AdvancedConfig::detailedValidationReport() const {
    std::vector<ValidationError> errors = validateWithErrors();
    if (errors.empty()) {
        return "Configuration is valid.\nAll validation checks passed.";
    }
    
    std::ostringstream oss;
    oss << "Detailed Validation Report\n";
    oss << "=========================\n\n";
    oss << "Total errors found: " << errors.size() << "\n\n";
    
    // Group errors by key
    std::unordered_map<std::string, std::vector<ValidationError>> errorsByKey;
    for (const auto& error : errors) {
        errorsByKey[error.getKey()].push_back(error);
    }
    
    for (const auto& pair : errorsByKey) {
        oss << "Key: " << pair.first << "\n";
        for (const auto& error : pair.second) {
            oss << "  - " << error.what() << "\n";
        }
        oss << "\n";
    }
    
    // Add statistics
    oss << "\nConfiguration Statistics:";
    oss << "\n  Total entries: " << getEntryCount();
    oss << "\n  Sources: " << getSourceDistribution();
    
    return oss.str();
}

double AdvancedConfig::benchmark() const {
    // Simple benchmark based on configuration
    double score = 100.0;
    
    // Deduct points for complex configurations (more entries)
    size_t entryCount = getEntryCount();
    score -= std::log10(entryCount / 10.0) * 5;
    
    // Add points for optimization opportunities
    for (const auto& key : getKeys()) {
        auto valueOpt = get<std::string>(key);
        if (valueOpt.has_value() && valueOpt.value().empty()) {
            score += 10;
        }
    }
    
    return std::max(0.0, score);
}

std::vector<std::string> AdvancedConfig::findByCategory(const std::string& category) const {
    std::vector<std::string> matches;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            matches.push_back(entry.key);
        }
    }
    
    return matches;
}

std::vector<std::string> AdvancedConfig::findByType(const std::string& type) const {
    std::vector<std::string> matches;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.type == type) {
            matches.push_back(entry.key);
        }
    }
    
    return matches;
}

size_t AdvancedConfig::getEntryCount() const {
    return pImpl->entries.size();
}

std::string AdvancedConfig::getSourceDistribution() const {
    std::unordered_map<int, size_t> sourceCounts;
    
    for (const auto& entry : pImpl->entries) {
        sourceCounts[static_cast<int>(entry.source)]++;
    }
    
    std::ostringstream oss;
    oss << "Sources:";
    for (const auto& pair : sourceCounts) {
        oss << " " << static_cast<ConfigSource>(pair.first) << "(" << pair.second << "),";
    }
    std::string result = oss.str();
    if (!result.empty() && result.back() == ',') {
        result.pop_back();
    }
    return result;
}

std::string AdvancedConfig::getVersion() const {
    return pImplAdvanced->version;
}

void AdvancedConfig::setVersion(const std::string& version) {
    pImplAdvanced->version = version;
}

std::string AdvancedConfig::getAuthor() const {
    return pImplAdvanced->author;
}

void AdvancedConfig::setAuthor(const std::string& author) {
    pImplAdvanced->author = author;
}

std::string AdvancedConfig::getDescription() const {
    return pImplAdvanced->description;
}

void AdvancedConfig::setDescription(const std::string& description) {
    pImplAdvanced->description = description;
}

} // namespace nlm
