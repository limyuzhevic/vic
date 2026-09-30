// Output configuration parameters

#pragma once

#include <string>

namespace nlm {

// Output configuration structure
struct OutputConfig {
    // Format and output
    std::string output_format;
    std::string output_prefix;
    size_t output_interval;
    bool verbose_output;
    bool quiet_output;
    bool progress_reporting;
    
    // Logging configuration
    std::string output_directory;
    bool enable_logging;
    std::string log_level;
    size_t max_log_file_size_mb;
    size_t log_file_rotation_count;
    bool log_to_console;
    bool log_to_file;
    
    OutputConfig() : output_format("text"), output_interval(100), verbose_output(false),
                    quiet_output(false), progress_reporting(true), enable_logging(true),
                    log_level("info"), max_log_file_size_mb(100), log_file_rotation_count(5),
                    log_to_console(true), log_to_file(true) {}
};

// Output configuration manager
class ConfigOutput {
public:
    ConfigOutput() = default;
    ~ConfigOutput() = default;
    
    // Load from Config object
    void loadFromConfig(const Config& config);
    
    // Save to Config object
    void saveToConfig(Config& config) const;
    
    // Validate configuration
    bool validate() const;
    
    // Get parameters
    const OutputConfig& get() const { return params; }
    OutputConfig& get() { return params; }
    
    // Set parameters
    void set(const OutputConfig& o) { params = o; }
    
    // Update individual parameter
    template<typename T>
    void setParameter(const std::string& key, const T& value);
    
    // Get parameter
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue = T()) const;
    
private:
    OutputConfig params;
};

} // namespace nlm