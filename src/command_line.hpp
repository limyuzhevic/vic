// NLM Command Line Interface
// Main entry point for NLM command-line operations
// Provides multiple execution modes, configuration options, and advanced features

#ifndef NLM_COMMAND_LINE_HPP
#define NLM_COMMAND_LINE_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>

#include "cli/options.hpp"
#include "cli/mode_runner.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

class CommandLine {
public:
    CommandLine();
    ~CommandLine();
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Execute the parsed command
    int execute();
    
    // Display help information
    void showHelp() const;
    
    // Get current options
    const CommandLineOptions& getOptions() const { return options_; }
    
    // Set configuration
    void setConfig(std::shared_ptr<Config> config) { config_ = config; }
    
    // Add custom mode
    void addMode(const std::string& name, std::function<bool(const CommandLineOptions&, std::shared_ptr<Config>)> modeFunc);
    
private:
    CommandLineOptions options_;
    std::shared_ptr<Config> config_;
    std::unordered_map<std::string, std::function<bool(const CommandLineOptions&, std::shared_ptr<Config>)>> modes_;
    std::unique_ptr<ModeRunner> modeRunner_;
    
    // Initialize default modes
    void initializeModes();
    
    // Execute mode by name
    bool executeMode(const std::string& modeName);
};

} // namespace nlm

#endif // NLM_COMMAND_LINE_HPP