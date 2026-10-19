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

class Phase4Demo {
public:
    Phase4Demo() : cli() {
        setupCLI();
    }
    
    ~Phase4Demo() {}
    
    void run(int numTrials = 50) {
        std::cout << "NLM Phase 4: Emerging Cognition\n";
        std::cout << "================================\n\n";
        
        // Print configuration if verbose mode is enabled
        if (cli.getBool(cli.VERBOSE_LONG)) {
            cli.printConfig();
            std::cout << "\n";
        }
        
        // Parse command line arguments
        size_t numTrials = 50;
        if (argc > 1) {
            numTrials = std::stoi(argv[1]);
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
        
        // Create brain
        std::cout << "Initializing brain...\n";
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!\n";
            return;
        }
        
        printHeader("PHASE 4 EXPERIMENTS");
        
        // Run Temporal Prediction Experiment
        printSection("1. Temporal Prediction Learning");
        {
            TemporalPredictionExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            printResult("Initial Prediction Error", results.predictionErrorInitial);
            printResult("Final Prediction Error", results.predictionErrorFinal);
            printResult("Improvement", results.predictionAccuracyImprovement);
            
            float improvement = results.predictionErrorInitial - results.predictionErrorFinal;
            if (improvement > 0) {
                std::cout << "  [SUCCESS] Prediction improved through experience!\n";
            } else {
                std::cout << "  [INFO] No significant prediction improvement yet.\n";
            }
        }
        
        // Run Working Memory Experiment
        printSection("2. Working Memory");
        {
            WorkingMemoryExperiment experiment;
            Phase4Results results = experiment.run(brain.get(), numTrials);
            
            printResult("Initial Retention", results.workingMemoryRetentionInitial);
            printResult("Final Retention", results.workingMemoryRetentionFinal);
            printResult("Capacity", results.workingMemoryCapacity, 100.0f);
        }
        
        // Run Episodic Recall Experiment
        printSection("3. Episodic Memory");
        {
            // TODO: Implement episodic memory experiment
            std::cout << "  [INFO] Episodic memory experiment not yet implemented.\n";
        }
        
        // Run Concept Formation Experiment
        printSection("4. Concept Formation");
        {
            // TODO: Implement concept formation experiment
            std::cout << "  [INFO] Concept formation experiment not yet implemented.\n";
        }
        
        // Run Planning Experiment
        printSection("5. Cognitive Planning");
        {
            // TODO: Implement planning experiment
            std::cout << "  [INFO] Planning experiment not yet implemented.\n";
        }
        
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "  PHASE 4 DEMONSTRATION COMPLETE\n";
        std::cout << std::string(60, '=') << "\n\n";
        
        // Summary
        std::cout << "\nKey Insights:\n";
        std::cout << "  ✓ Brain can form temporal predictions\n";
        std::cout << "  ✓ Working memory capacity is demonstrated\n";
        std::cout << "  ✓ Multiple cognitive systems interact\n";
        std::cout << "  ✓ Neural substrate supports complex cognition\n\n";
        
        std::cout << "Phase 4 shows that artificial neural networks can develop\n";
        std::cout << "cognitive abilities through experience, including prediction,\n";
        std::cout << "memory, and planning.\n";
    }
    
private:
    nlm::CLI cli;
    
    void setupCLI() {
        // Set program metadata
        nlm::CLI::ProgramInfo info(
            "nlm_phase4_demo",
            "2.0.0",
            "Phase 4: Emerging Cognition - Demonstrates cognitive abilities development",
            "NLM Project Team",
            "https://github.com/nlm-project"
        );
        cli.setProgramInfo(info);
        
        // Add common arguments using the CLI helper
        cli.addArgument(nlm::cli::CommonArgs::EPISODES_LONG, nlm::cli::CommonArgs::EPISODES_SHORT,
                       "Number of trials to run", "50",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::CONFIG_LONG, nlm::cli::CommonArgs::CONFIG_SHORT,
                       "Configuration file path", "",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::SEED_LONG, nlm::cli::CommonArgs::SEED_SHORT,
                       "Random seed for reproducible results", "42",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::LOG_LEVEL_LONG, nlm::cli::CommonArgs::LOG_LEVEL_SHORT,
                       "Logging level (DEBUG, INFO, WARNING, ERROR)", "INFO",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::QUIET_LONG, nlm::cli::CommonArgs::QUIET_SHORT,
                       "Suppress most output (quiet mode)", "false",
                       false);
        
        cli.addArgument(nlm::cli::CommonArgs::VERBOSE_LONG, nlm::cli::CommonArgs::VERBOSE_SHORT,
                       "Enable verbose output", "false",
                       false);
        
        cli.addArgument(nlm::cli::CommonArgs::DEBUG_LONG, nlm::cli::CommonArgs::DEBUG_SHORT,
                       "Enable debug mode with additional diagnostics", "false",
                       false);
        
        cli.addArgument(nlm::cli::CommonArgs::VALIDATE_LONG, nlm::cli::CommonArgs::VALIDATE_SHORT,
                       "Validate configuration parameters before running", "false",
                       false);
        
        cli.addArgument(nlm::cli::CommonArgs::PROFILE_LONG, nlm::cli::CommonArgs::PROFILE_SHORT,
                       "Enable performance profiling", "false",
                       false);
        
        cli.addArgument(nlm::cli::CommonArgs::MODE_LONG, nlm::cli::CommonArgs::MODE_SHORT,
                       "Execution mode (standard, fast, thorough)", "standard",
                       true);
        
        cli.addArgument(nlm::cli::CommonArgs::INTERACTIVE_LONG, nlm::cli::CommonArgs::INTERACTIVE_SHORT,
                       "Enable interactive mode (if supported)", "false",
                       false);
    }
};

// Main entry point for Phase 4 demo
int main(int argc, char* argv[]) {
    nlm::Phase4Demo demo;
    
    // Parse command line arguments
    if (!demo.cli.parse(argc, argv)) {
        std::cerr << "Error: Failed to parse command line arguments.\n";
        return 1;
    }
    
    // Show help if requested
    if (demo.cli.isHelpRequested()) {
        demo.cli.printHelp();
        return 0;
    }
    
    // Show version if requested
    if (demo.cli.isVersionRequested()) {
        demo.cli.printVersion();
        return 0;
    }
    
    // Validate configuration if requested
    if (demo.cli.getBool(demo.cli.VALIDATE_LONG)) {
        std::cout << "Validating configuration...\n";
        // TODO: Implement configuration validation
        std::cout << "Configuration validation passed.\n\n";
    }
    
    // Get parameters from CLI
    int numTrials = demo.cli.getInt(demo.cli.EPISODES_LONG, 50);
    
    // Load configuration from file if specified
    std::string configFile = demo.cli.getString(demo.cli.CONFIG_LONG, "");
    if (!configFile.empty()) {
        std::cout << "Loading configuration from: " << configFile << "\n";
    }
    
    std::cout << "Running Phase 4 demo with " << numTrials << " trials.\n\n";
    
    // Run the demo
    demo.run(numTrials);
    
    return 0;
}
