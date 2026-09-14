#pragma once

#include <memory>
#include <string>
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class NLMUtils {
public:
    NLMUtils();
    ~NLMUtils();
    
    // Initialize Phase 2 default configuration
    static void initializePhase2Config(std::shared_ptr<Config> config);
    
    // Setup global logger with console output
    static void setupGlobalLogger();
    
    // Print application banner
    static void printBanner();
    
    // Display configuration summary
    static void displayConfigSummary(const Config& config);
};

} // namespace nlm