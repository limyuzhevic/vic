// Mode execution framework for NLM
// Handles execution of different modes with benchmark support

#ifndef NLM_CLI_MODE_RUNNER_HPP
#define NLM_CLI_MODE_RUNNER_HPP

#include <string>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <functional>

#include "cli/options.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

class ModeRunner {
public:
    ModeRunner();
    ~ModeRunner();
    
    // Run a mode by name
    bool runMode(const std::string& modeName, const CommandLineOptions& options, std::shared_ptr<Config> config);
    
    // Built-in mode execution functions
    bool runBasicMode(const CommandLineOptions& options, std::shared_ptr<Config> config);
    bool runFullMode(const CommandLineOptions& options, std::shared_ptr<Config> config);
    bool runPhase3Mode(const CommandLineOptions& options, std::shared_ptr<Config> config);
    bool runPhase4Mode(const CommandLineOptions& options, std::shared_ptr<Config> config);
    bool runPhase6Mode(const CommandLineOptions& options, std::shared_ptr<Config> config);
    
    // Internal implementation methods
    bool runBasicModeInternal(const CommandLineOptions& options, std::shared_ptr<Config> config);
    bool runFullModeInternal(const CommandLineOptions& options, std::shared_ptr<Config> config);
    bool runPhase3ModeInternal(const CommandLineOptions& options, std::shared_ptr<Config> config);
    bool runPhase4ModeInternal(const CommandLineOptions& options, std::shared_ptr<Config> config);
    bool runPhase6ModeInternal(const CommandLineOptions& options, std::shared_ptr<Config> config);
    
    // Benchmark result handling
    void saveBenchmarkResult(const BenchmarkResult& result, const std::string& filename);
    
private:
    // Map of mode names to function pointers
    using ModeFunction = bool (ModeRunner::*)(const CommandLineOptions&, std::shared_ptr<Config>);
    std::unordered_map<std::string, ModeFunction> modeFunctions_;
    
    // Setup mode functions
    void setupModeFunctions();
};

} // namespace nlm

#endif // NLM_CLI_MODE_RUNNER_HPP