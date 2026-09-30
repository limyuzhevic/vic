// Debugging configuration parameters

#pragma once

#include <string>
#include <vector>
#include "core/Types/Types.hpp"

namespace nlm {

// Debug configuration structure
struct DebugConfig {
    // Verbosity levels
    int debug_level;
    bool enable_plasticity_debug;
    bool enable_validation_debug;
    bool enable_trace_events;
    bool enable_execution_profiling;
    
    // Targets for debugging
    std::vector<NeuronId> debug_neuron_ids;
    std::vector<SynapseId> debug_synapse_ids;
    
    // Output settings
    std::string trace_output_file;
    std::string profile_output_file;
    std::string debug_log_file;
    
    DebugConfig() : debug_level(0), enable_plasticity_debug(false),
                   enable_validation_debug(false), enable_trace_events(false),
                   enable_execution_profiling(false) {}
};

// Debug configuration manager
class ConfigDebug {
public:
    ConfigDebug() = default;
    ~ConfigDebug() = default;
    
    // Load from Config object
    void loadFromConfig(const Config& config);
    
    // Save to Config object
    void saveToConfig(Config& config) const;
    
    // Validate configuration
    bool validate() const;
    
    // Get parameters
    const DebugConfig& get() const { return params; }
    DebugConfig& get() { return params; }
    
    // Set parameters
    void set(const DebugConfig& d) { params = d; }
    
    // Update individual parameter
    template<typename T>
    void setParameter(const std::string& key, const T& value);
    
    // Get parameter
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue = T()) const;
    
private:
    DebugConfig params;
};

} // namespace nlm