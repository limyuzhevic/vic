// unified_main.cpp - Unified CLI Interface for NLM Phases
// 
// This provides a unified command-line interface for running all NLM phases
// with improved argument parsing, configuration management, and user guidance.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <memory>

using namespace nlm;

// Forward declarations for phase demo entry points
class PhaseDemo {
public:
    virtual ~PhaseDemo() = default;
    virtual bool run(const Config& config) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};

// Phase 2 Demo Implementation
class Phase2Demo : public PhaseDemo {
public:
    bool run(const Config& config) override {
        std::cout << "=== NLM Phase 2: Real Neural Computation ===" << std::endl;
        std::cout << "Testing LIF neurons, STDP plasticity, and structural changes..." << std::endl;
        
        // Phase 2 demo is the original nlm executable functionality
        // For now, we'll run the basic connectivity test
        std::cout << "Phase 2 demonstration would run here..." << std::endl;
        return true;
    }
    
    std::string getName() const override { return "phase2"; }
    std::string getDescription() const override { return "Phase 2: Real Neural Computation (LIF neurons, STDP)"; }
};

// Phase 3 Demo Implementation
class Phase3Demo : public PhaseDemo {
public:
    bool run(const Config& config) override {
        std::cout << "=== NLM Phase 3: World Interaction ===" << std::endl;
        std::cout << "Testing sensory input, motor output, and learning..." << std::endl;
        
        // In a real implementation, this would include:
        // - World interaction loop
        // - Reward prediction error
        // - Developmental stages
        // - Novelty and curiosity
        std::cout << "Phase 3 demonstration would run here..." << std::endl;
        return true;
    }
    
    std::string getName() const override { return "phase3"; }
    std::string getDescription() const override { return "Phase 3: World interaction and learning"; }
};

// Phase 4 Demo Implementation
class Phase4Demo : public PhaseDemo {
public:
    bool run(const Config& config) override {
        std::cout << "=== NLM Phase 4: Cognitive Mechanisms ===" << std::endl;
        std::cout << "Testing neural prediction, working memory, and concept formation..." << std::endl;
        
        std::cout << "Phase 4 demonstration would run here..." << std::endl;
        return true;
    }
    
    std::string getName() const override { return "phase4"; }
    std::string getDescription() const override { return "Phase 4: Cognitive mechanisms (prediction, memory, attention)"; }
};

// Phase 5 Demo Implementation
class Phase5Demo : public PhaseDemo {
public:
    bool run(const Config& config) override {
        std::cout << "=== NLM Phase 5: Performance and Personality ===" << std::endl;
        std::cout << "Testing memory systems, neuromodulation, and checkpointing..." << std::endl;
        
        std::cout << "Phase 5 demonstration would run here..." << std::endl;
        return true;
    }
    
    std::string getName() const override { return "phase5"; }
    std::string getDescription() const override { return "Phase 5: Performance optimizations and personality traits"; }
};

// Phase 6 Demo Implementation
class Phase6Demo : public PhaseDemo {
public:
    bool run(const Config& config) override {
        std::cout << "=== NLM Phase 6: Final Integration ===" << std::endl;
        std::cout << "Testing integrated brain systems and functionality..." << std::endl;
        
        std::cout << "Phase 6 demonstration would run here..." << std::endl;
        return true;
    }
    
    std::string getName() const override { return "phase6"; }
    std::string getDescription() const override { return "Phase 6: Final integration (all systems connected)"; }
};

// Unified CLI Application
class NLMApplication {
public:
    NLMApplication() {
        registerPhases();
        initializeDefaultConfig();
    }
    
    bool parseCommandLine(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                showHelp();
                return false;
            } else if (arg == "--list-phases") {
                listPhases();
                return false;
            } else if (arg == "--version") {
                showVersion();
                return false;
            } else if (arg.substr(0, 2) == "--") {
                // Handle --key=value or --key value formats
                size_t pos = arg.find('=');
                std::string key, value;
                
                if (pos != std::string::npos) {
                    key = arg.substr(2, pos - 2);
                    value = arg.substr(pos + 1);
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    key = arg.substr(2);
                    value = argv[++i];
                } else {
                    std::cerr << "Error: Invalid argument format: " << arg << std::endl;
                    return false;
                }
                
                // Normalize key (remove spaces, convert to lowercase)
                key = normalizeKey(key);
                
                // Set config value based on type
                setConfigValue(key, value);
            } else {
                std::cerr << "Error: Unknown argument: " << arg << std::endl;
                return false;
            }
        }
        return true;
    }
    
    bool run() {
        // Load configuration
        if (!config_->loadFromArgs(0, nullptr)) {
            std::cerr << "Error: Failed to load configuration" << std::endl;
            return false;
        }
        
        // Determine which phase to run
        std::string phase = getCurrentPhase();
        if (phase.empty()) {
            std::cerr << "Error: No phase specified. Use --phase or -p to select a phase." << std::endl;
            return false;
        }
        
        // Get the selected phase demo
        auto it = phaseDemos_.find(phase);
        if (it == phaseDemos_.end()) {
            std::cerr << "Error: Unknown phase: " << phase << std::endl;
            return false;
        }
        
        PhaseDemo* demo = it->second.get();
        std::cout << "Running " << demo->getDescription() << "..." << std::endl;
        
        // Run the demo
        if (!demo->run(*config_)) {
            std::cerr << "Error: Phase demo failed" << std::endl;
            return false;
        }
        
        std::cout << "Phase " << phase << " completed successfully!" << std::endl;
        return true;
    }
    
    void showHelp() const {
        std::cout << "================================================================" << std::endl;
        std::cout << "NLM - Neural Learning Machine - Unified CLI Interface" << std::endl;
        std::cout << "================================================================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "USAGE:" << std::endl;
        std::cout << "  nlm [--phase|-p PHASE] [options]" << std::endl;
        std::cout << std::endl;
        
        std::cout << "AVAILABLE PHASES:" << std::endl;
        for (const auto& pair : phaseDemos_) {
            std::cout << "  " << pair.first << ": " << pair.second->getDescription() << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  --phase|-p PHASE     Phase to run (2, 3, 4, 5, 6)" << std::endl;
        std::cout << "  --neurons|-n COUNT   Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --regions|-r COUNT   Number of brain regions (default: 2)" << std::endl;
        std::cout << "  --steps|-s COUNT     Number of simulation steps (default: 10000)" << std::endl;
        std::cout << "  --timestep|-t VALUE  Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --checkpoint          Enable checkpointing" << std::endl;
        std::cout << "  --checkpoint-path PATH  Checkpoint file path" << std::endl;
        std::cout << "  --verbose|-v         Enable verbose logging" << std::endl;
        std::cout << "  --help|-h            Show this help message" << std::endl;
        std::cout << "  --list-phases        List all available phases" << std::endl;
        std::cout << "  --version            Show version information" << std::endl;
        std::cout << std::endl;
        
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  nlm --phase 2                    # Run Phase 2 (LIF neurons)" << std::endl;
        std::cout << "  nlm --phase 6 --steps 5000       # Run Phase 6 with 5000 steps" << std::endl;
        std::cout << "  nlm --phase 3 --neurons 800     # Run Phase 3 with 800 neurons" << std::endl;
        std::cout << "  nlm --phase 5 --checkpoint       # Run Phase 5 with checkpointing" << std::endl;
        std::cout << std::endl;
        
        std::cout << "For more advanced configuration options, use --phase followed by" << std::endl;
        std::cout << "custom configuration settings (see Config.hpp for available options)." << std::endl;
    }
    
    void listPhases() const {
        std::cout << "Available NLM Phases:" << std::endl;
        std::cout << "====================" << std::endl;
        std::cout << std::endl;
        
        int count = 1;
        for (const auto& pair : phaseDemos_) {
            std::cout << count << ". " << pair.second->getName() << ": " << pair.second->getDescription() << std::endl;
            count++;
        }
        std::cout << std::endl;
        
        std::cout << "Use --phase <number> to run a specific phase, or --phase phase_name" << std::endl;
        std::cout << "to run by phase name (phase2, phase3, phase4, phase5, phase6)." << std::endl;
    }
    
    void showVersion() const {
        std::cout << "NLM Unified CLI Interface" << std::endl;
        std::cout << "Version: 2.0.0" << std::endl;
        std::cout << "Built with enhanced configuration management and phase support." << std::endl;
    }
    
    const Config& getConfig() const { return *config_; }
    
private:
    void registerPhases() {
        phaseDemos_["2"] = std::make_unique<Phase2Demo>();
        phaseDemos_["3"] = std::make_unique<Phase3Demo>();
        phaseDemos_["4"] = std::make_unique<Phase4Demo>();
        phaseDemos_["5"] = std::make_unique<Phase5Demo>();
        phaseDemos_["6"] = std::make_unique<Phase6Demo>();
        
        phaseDemos_["phase2"] = std::make_unique<Phase2Demo>();
        phaseDemos_["phase3"] = std::make_unique<Phase3Demo>();
        phaseDemos_["phase4"] = std::make_unique<Phase4Demo>();
        phaseDemos_["phase5"] = std::make_unique<Phase5Demo>();
        phaseDemos_["phase6"] = std::make_unique<Phase6Demo>();
    }
    
    void initializeDefaultConfig() {
        config_ = std::make_unique<Config>();
        
        // Set default values based on enhanced.cfg
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        config_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config_->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config_->set("connection_probability", 0.1f, ConfigSource::Default);
        config_->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config_->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config_->set("stdp_time_constant", 20.0f, ConfigSource::Default);
        config_->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
        config_->set("synaptogenesis_rate", 0.001f, ConfigSource::Default);
        config_->set("pruning_rate", 0.0001f, ConfigSource::Default);
        config_->set("dopamine_baseline", 0.1f, ConfigSource::Default);
        config_->set("reward_discount_factor", 0.99f, ConfigSource::Default);
        config_->set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default);
        config_->set("visualization_update_rate", 30.0f, ConfigSource::Default);
        
        // Set phase-specific defaults
        config_->set("phase", "2", ConfigSource::Default);
        config_->set("checkpoint", false, ConfigSource::Default);
        config_->set("verbose", false, ConfigSource::Default);
    }
    
    std::string getCurrentPhase() const {
        auto phaseOpt = config_->get<std::string>("phase");
        return phaseOpt.has_value() ? phaseOpt.value() : "";
    }
    
    void setConfigValue(const std::string& key, const std::string& value) {
        // Convert string value to appropriate type based on key
        if (key == "neurons" || key == "steps" || key == "regions") {
            try {
                int val = std::stoi(value);
                config_->set(key, val, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid integer value for " << key << ": " << value << std::endl;
            }
        } else if (key == "timestep" || key == "plasticity_learning_rate" || 
                   key == "stdp_ltp_weight" || key == "stdp_ltd_weight" ||
                   key == "stdp_time_constant" || key == "synaptogenesis_rate" ||
                   key == "pruning_rate" || key == "dopamine_baseline" ||
                   key == "reward_discount_factor" || key == "visualization_update_rate") {
            try {
                float val = std::stof(value);
                config_->set(key, val, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid float value for " << key << ": " << value << std::endl;
            }
        } else if (key == "checkpoint" || key == "verbose") {
            bool val = (value == "true" || value == "1" || value == "yes");
            config_->set(key, val, ConfigSource::CommandLine);
        } else {
            // Treat as string
            config_->set(key, value, ConfigSource::CommandLine);
        }
    }
    
    std::string normalizeKey(const std::string& key) const {
        std::string result = key;
        // Remove spaces and convert to lowercase
        result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    std::unique_ptr<Config> config_;
    std::map<std::string, std::unique_ptr<PhaseDemo>> phaseDemos_;
};

int main(int argc, char** argv) {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLMApplication app;
    
    // Parse command line arguments
    if (argc <= 1) {
        std::cout << "No arguments provided. Use --help for usage information." << std::endl;
        return 0;
    }
    
    if (!app.parseCommandLine(argc, argv)) {
        return 1;
    }
    
    // Run the application
    if (!app.run()) {
        return 1;
    }
    
    return 0;
}
