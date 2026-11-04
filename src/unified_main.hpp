// unified_main.hpp - Header for unified CLI interface
//
// This provides the unified CLI interface declaration for running all NLM phases.

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <memory>
#include <map>

using namespace nlm;

// Forward declarations
class PhaseDemo;

class NLMApplication {
public:
    NLMApplication();
    ~NLMApplication();
    
    // Delete copy operations
    NLMApplication(const NLMApplication&) = delete;
    NLMApplication& operator=(const NLMApplication&) = delete;
    
    // Parse command line arguments
    bool parseCommandLine(int argc, char** argv);
    
    // Run the application
    bool run();
    
    // Show help information
    void showHelp() const;
    
    // List available phases
    void listPhases() const;
    
    // Show version information
    void showVersion() const;
    
    // Get the current configuration
    const Config& getConfig() const { return *config_; }
    
private:
    void registerPhases();
    void initializeDefaultConfig();
    std::string getCurrentPhase() const;
    void setConfigValue(const std::string& key, const std::string& value);
    std::string normalizeKey(const std::string& key) const;
    
    std::unique_ptr<Config> config_;
    std::map<std::string, std::unique_ptr<PhaseDemo>> phaseDemos_;
};
