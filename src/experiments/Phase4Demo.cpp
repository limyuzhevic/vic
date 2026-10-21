#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "core/CLI/CLI.hpp"

using namespace nlm;

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printResult(const std::string& name, float value, float max = 1.0f) {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

}
    return 0;
}

// Enhanced main entry point for Phase 4 demo with CLI support
int main(int argc, char* argv[]) {
    std::string programName = "nlm_phase4_demo";
    std::string version = "0.1.0";
    std::string description = "NLM Phase 4: Emerging Cognition Demo - Demonstrates cognitive mechanisms emergence";
    
    nlm::ArgumentParser parser(programName, version, description);
    
    // Add Phase 4 specific arguments
    parser.addArgument('', 'trials', "Number of trials for experiments (10-10000)", "50",
                      true, "positive-int");
    parser.addArgument('', 'enable-prediction', "Enable prediction experiments", "",
                      false, "");
    parser.addArgument('', 'enable-memory', "Enable memory experiments", "",
                      false, "");
    
    parser.parse(argc, argv);
    
    // Check for help/version
    if (parser.hasArgument("help")) {
        parser.printHelp();
        return 0;
    }
    if (parser.hasArgument("version")) {
        parser.printVersion();
        return 0;
    }
    
    // Use logger controller if log-level is set
    std::string logLevel = parser.getValue("log-level");
    std::string logFile = parser.getValue("log-file");
    std::string logColors = !parser.hasArgument("no-colors") ? "true" : "false";
    
    if (!logLevel.empty()) {
        bool enableColors = (logColors == "true");
        nlm::LoggerController loggerController;
        loggerController.configure(logLevel, logFile, enableColors);
    }
    
    size_t numTrials = parser.getInt("trials");
    
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    
    // Parse command line arguments safely
    if (argc > 1) {
        try {
            size_t pos;
            size_t val = std::stoull(argv[1], &pos);
            if (pos == strlen(argv[1])) {  // Ensure full string was consumed
                if (val > 0 && val <= 10000) {  // Reasonable range check
                    numTrials = val;
                } else {
                    std::cerr << "Error: numTrials must be between 1 and 10000, got " << val << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: Invalid numTrials format: " << argv[1] << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid numTrials: " << e.what() << std::endl;
            return 1;
        }
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);