// NLM Phase 2 Enhanced Main - Demonstration Driver
// 
// Enhanced main program that uses the new experiment framework.
// 
// This version provides:
// 1. Better separation of concerns with experiment classes
// 2. Enhanced error handling and validation
// 3. Improved reporting and user feedback
// 4. Integration with existing experiment runner patterns
// 5. Better configuration management
// 6. Statistical validation and analysis
// 7. Progressive enhancement over original implementation

#include "experiments/Phase2Demo.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char** argv) {
    // Create and configure the enhanced demo
    nlm::NlmPhase2Demo demo;
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    demo.setConfigFile(configFile);
    
    // Run all demonstrations
    bool success = demo.runAllDemonstrations();
    
    if (success) {
        std::cout << "\n" << demo.getDemoStatus() << std::endl;
        return 0;
    } else {
        std::cerr << "\nERROR: NLM Phase 2 demonstrations failed!" << std::endl;
        return 1;
    }
}