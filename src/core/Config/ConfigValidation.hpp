// Validation configuration parameters

#pragma once

#include <string>
#include <map>

namespace nlm {

// Validation configuration structure
struct ValidationConfig {
    // Validation modes
    bool strict_mode;
    bool skip_invalid_configurations;
    bool attempt_fix_invalid;
    bool validate_before_execution;
    
    // Thresholds and tolerances
    std::map<std::string, double> validation_thresholds;
    size_t max_consecutive_errors;
    double tolerance_numeric;
    double tolerance_memory;
    double tolerance_performance;
    
    ValidationConfig() : strict_mode(false), skip_invalid_configurations(false),
                        attempt_fix_invalid(false), validate_before_execution(true),
                        max_consecutive_errors(3), tolerance_numeric(0.01),
                        tolerance_memory(0.1), tolerance_performance(0.05) {}
};

// Validation configuration manager
class ConfigValidation {
public:
    ConfigValidation() = default;
    ~ConfigValidation() = default;
    
    // Load from Config object
    void loadFromConfig(const Config& config);
    
    // Save to Config object
    void saveToConfig(Config& config) const;
    
    // Validate configuration
    bool validate() const;
    
    // Get parameters
    const ValidationConfig& get() const { return params; }
    ValidationConfig& get() { return params; }
    
    // Set parameters
    void set(const ValidationConfig& v) { params = v; }
    
    // Update individual parameter
    template<typename T>
    void setParameter(const std::string& key, const T& value);
    
    // Get parameter
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue = T()) const;
    
private:
    ValidationConfig params;
};

} // namespace nlm