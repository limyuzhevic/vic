// NLM - Beginner-Friendly Main Entry Point
// 
// This provides a simple, beginner-friendly interface to NLM Phase 6
// Supports multiple modes and configuration presets for different user levels

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration Demo                            ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << "=== NLM Phase 6 Beginner Demo ===" << std::endl;
    std::cout << "Usage: ./nlm [OPTIONS]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "BASIC OPTIONS:" << std::endl;
    std::cout << "  --help                  Show this help message" << std::endl;
    std::cout << "  --quick                 Run fast integration test (skip details)" << std::endl;
    std::cout << "  --detailed              Run full detailed demonstration" << std::endl;
    std::cout << "  --beginner              Use beginner configuration (small, simple)" << std::endl;
    std::cout << "  --advanced              Use advanced configuration (large, complex)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "CUSTOMIZATION:" << std::endl;
    std::cout << "  --steps N               Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --neurons N             Neuron count (default: 500)" << std::endl;
    std::cout << "  --regions N             Region count (default: 1)" << std::endl;
    std::cout << "  --checkpoint PATH       Checkpoint file path" << std::endl;
    std::cout << "  --verbose               Enable verbose output" << std::endl;
    std::cout << "  --silent                Minimize output" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "PRESETS:" << std::endl;
    std::cout << "  --preset beginner       Beginner preset (200 neurons, 500 steps)" << std::endl;
    std::cout << "  --preset standard       Standard preset (500 neurons, 2000 steps)" << std::endl;
    std::cout << "  --preset research       Research preset (1000 neurons, 5000 steps)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  ./nlm --beginner                     # Simple demonstration" << std::endl;
    std::cout << "  ./nlm --quick                       # Fast integration test" << std::endl;
    std::cout << "  ./nlm --detailed --verbose          # Full demonstration with details" << std::endl;
    std::cout << "  ./nlm --preset research --verbose   # Large-scale experiment" << std::endl;
}

// Simple configuration presets
void applyPreset(Config& config, const std::string& preset) {
    if (preset == "beginner") {
        config->set("neuron_count", size_t(200), ConfigSource::Preset);
        config->set("max_steps", uint64_t(500), ConfigSource::Preset);
        config->set("simulation_timestep", 0.001, ConfigSource::Preset);
        config->set("random_seed", uint64_t(42), ConfigSource::Preset);
        config->set("enable_reward_modulation", true, ConfigSource::Preset);
        config->set("enable_structural_plasticity", false, ConfigSource::Preset);
        config->set("enable_development", true, ConfigSource::Preset);
        config->set("enable_curiosity", true, ConfigSource::Preset);
        config->set("enable_novelty", false, ConfigSource::Preset);
        std::cout << "Using BEGINNER preset (small, simple brain)" << std::endl;
    } else if (preset == "standard") {
        config->set("neuron_count", size_t(500), ConfigSource::Preset);
        config->set("max_steps", uint64_t(2000), ConfigSource::Preset);
        config->set("simulation_timestep", 0.001, ConfigSource::Preset);
        config->set("random_seed", uint64_t(42), ConfigSource::Preset);
        config->set("enable_reward_modulation", true, ConfigSource::Preset);
        config->set("enable_structural_plasticity", false, ConfigSource::Preset);
        config->set("enable_development", true, ConfigSource::Preset);
        config->set("enable_curiosity", true, ConfigSource::Preset);
        config->set("enable_novelty", true, ConfigSource::Preset);
        std::cout << "Using STANDARD preset (balanced configuration)" << std::endl;
    } else if (preset == "research") {
        config->set("neuron_count", size_t(1000), ConfigSource::Preset);
        config->set("max_steps", uint64_t(5000), ConfigSource::Preset);
        config->set("simulation_timestep", 0.001, ConfigSource::Preset);
        config->set("random_seed", uint64_t(42), ConfigSource::Preset);
        config->set("connection_probability", 0.15f, ConfigSource::Preset);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Preset);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Preset);
        config->set("enable_reward_modulation", true, ConfigSource::Preset);
        config->set("enable_structural_plasticity", true, ConfigSource::Preset);
        config->set("enable_development", true, ConfigSource::Preset);
        config->set("enable_curiosity", true, ConfigSource::Preset);
        config->set("enable_novelty", true, ConfigSource::Preset);
        std::cout << "Using RESEARCH preset (large-scale experiment)" << std::endl;
    } else {
        std::cerr << "Unknown preset: " << preset << std::endl;
        std::cerr << "Available presets: beginner, standard, research" << std::endl;
        throw std::runtime_error("Invalid preset name");
    }
}

// Validate configuration
bool validateConfig(const Config& config) {
    bool isValid = true;
    
    if (config.has("neuron_count")) {
        size_t neurons = config.getOr<size_t>("neuron_count", 0);
        if (neurons == 0) {
            std::cerr << "ERROR: neuron_count should be greater than 0" << std::endl;
            isValid = false;
        } else if (neurons > 10000) {
            std::cerr << "WARNING: Large neuron count may require more memory" << std::endl;
        }
    }
    
    if (config.has("max_steps")) {
        uint64_t steps = config.getOr<uint64_t>("max_steps", 0);
        if (steps == 0) {
            std::cerr << "ERROR: max_steps should be greater than 0" << std::endl;
            isValid = false;
        }
    }
    
    if (config.has("simulation_timestep")) {
        double timestep = config.getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0 || timestep > 1.0) {
            std::cerr << "ERROR: simulation_timestep should be between 0 and 1" << std::endl;
            isValid = false;
        }
    }
    
    return isValid;
}

// Print configuration summary
void printConfigSummary(const Config& config) {
    std::cout << "=== Configuration Summary ===" << std::endl;
    std::cout << "Neurons: " << config.getOr<size_t>("neuron_count", 0) << std::endl;
    std::cout << "Regions: " << config.getOr<size_t>("region_count", 0) << std::endl;
    std::cout << "Max Steps: " << config.getOr<uint64_t>("max_steps", 0) << std::endl;
    std::cout << "Timestep: " << config.getOr<double>("simulation_timestep", 0.0) << "s" << std::endl;
    std::cout << "Reward Modulation: " << (config.getOr<bool>("enable_reward_modulation", false) ? "ON" : "OFF") << std::endl;
    std::cout << "Development: " << (config.getOr<bool>("enable_development", false) ? "ON" : "OFF") << std::endl;
    std::cout << "Curiosity: " << (config.getOr<bool>("enable_curiosity", false) ? "ON" : "OFF") << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        printBanner();
        
        // Initialize configuration
        auto config = std::make_shared<Config>();
        
        // Parse command line arguments
        std::string mode = "quick";
        std::string preset = "standard";
        bool verbose = false;
        bool silent = false;
        uint64_t customSteps = 0;
        size_t customNeurons = 0;
        size_t customRegions = 0;
        std::string checkpointPath = "";
        
        // Parse arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                printHelp();
                return 0;
            } else if (arg == "--quick") {
                mode = "quick";
            } else if (arg == "--detailed") {
                mode = "detailed";
            } else if (arg == "--beginner") {
                applyPreset(config, "beginner");
            } else if (arg == "--advanced") {
                applyPreset(config, "advanced");
            } else if (arg == "--preset") {
                if (i + 1 < argc) {
                    preset = argv[++i];
                    applyPreset(config, preset);
                } else {
                    std::cerr << "ERROR: Missing preset name after --preset" << std::endl;
                    return 1;
                }
            } else if (arg == "--steps" && i + 1 < argc) {
                customSteps = std::stoul(argv[++i]);
            } else if (arg == "--neurons" && i + 1 < argc) {
                customNeurons = std::stoul(argv[++i]);
            } else if (arg == "--regions" && i + 1 < argc) {
                customRegions = std::stoul(argv[++i]);
            } else if (arg == "--checkpoint" && i + 1 < argc) {
                checkpointPath = argv[++i];
            } else if (arg == "--verbose") {
                verbose = true;
            } else if (arg == "--silent") {
                silent = true;
            } else {
                std::cerr << "ERROR: Unknown option: " << arg << std::endl;
                std::cerr << "Use --help for usage information." << std::endl;
                return 1;
            }
        }
        
        // Apply custom settings
        if (customSteps > 0) {
            config->set("max_steps", customSteps, ConfigSource::User);
            std::cout << "Custom steps: " << customSteps << std::endl;
        }
        if (customNeurons > 0) {
            config->set("neuron_count", customNeurons, ConfigSource::User);
            std::cout << "Custom neurons: " << customNeurons << std::endl;
        }
        if (customRegions > 0) {
            config->set("region_count", customRegions, ConfigSource::User);
            std::cout << "Custom regions: " << customRegions << std::endl;
        }
        if (!checkpointPath.empty()) {
            config->set("checkpoint_path", checkpointPath, ConfigSource::User);
            std::cout << "Custom checkpoint: " << checkpointPath << std::endl;
        }
        
        // Validate configuration
        if (!validateConfig(config)) {
            std::cerr << "ERROR: Invalid configuration" << std::endl;
            return 1;
        }
        
        // Set up logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Debug : LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Print mode and configuration
        std::cout << "=== NLM Phase 6 Demo ===" << std::endl;
        std::cout << "Mode: " << mode << std::endl;
        printConfigSummary(config);
        
        // Create and run experiment
        Phase6IntegratedExperiment experiment;
        
        if (mode == "quick" || mode == "silent") {
            std::cout << "Running quick integration test..." << std::endl;
            bool success = experiment.verifyIntegration();
            if (success) {
                std::cout << "✓ Integration verification passed!" << std::endl;
                
                // Run key subsystem tests
                experiment.testMemoryIntegration();
                experiment.testNeuromodulationIntegration();
                experiment.testCheckpointing();
                
                std::cout << "✓ Quick demo completed successfully!" << std::endl;
            } else {
                std::cerr << "✗ Integration verification failed!" << std::endl;
                return 1;
            }
        } else {
            std::cout << "Running detailed demonstration..." << std::endl;
            
            // Integration verification
            std::cout << "1. Integration verification..." << std::endl;
            if (!experiment.verifyIntegration()) {
                std::cerr << "ERROR: Integration verification failed!" << std::endl;
                return 1;
            }
            
            // Individual system tests
            std::cout << "2. Memory integration test..." << std::endl;
            experiment.testMemoryIntegration();
            
            std::cout << "3. Neuromodulation integration test..." << std::endl;
            experiment.testNeuromodulationIntegration();
            
            std::cout << "4. Checkpoint test..." << std::endl;
            experiment.testCheckpointing();
            
            std::cout << "5. Replay test..." << std::endl;
            experiment.testReplay();
            
            // Full experiment
            std::cout << "6. Full integration experiment..." << std::endl;
            
            Phase6Config phase6Config;
            phase6Config.neuronCount = config->getOr<size_t>("neuron_count", 500);
            phase6Config.maxSteps = config->getOr<uint64_t>("max_steps", 2000);
            phase6Config.enableCheckpointing = true;
            phase6Config.enableReplay = true;
            phase6Config.enableDevelopment = true;
            if (checkpointPath.empty()) {
                phase6Config.checkpointPath = "./checkpoint_test.bin";
            } else {
                phase6Config.checkpointPath = checkpointPath;
            }
            
            auto result = experiment.run(phase6Config);
            
            // Print results
            std::cout << "=== EXPERIMENT RESULTS ===" << std::endl;
            std::cout << "Total Reward: " << result.totalReward << std::endl;
            std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
            std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
            std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
            std::cout << std::endl;
            
            std::cout << "=== INTEGRATION STATUS ===" << std::endl;
            std::cout << "Memory Systems: " << (result.memoryWorkingMemoryIntegrated ? "✓" : "✗") << std::endl;
            std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "✓" : "✗") << std::endl;
            std::cout << "Prediction: " << (result.predictionIntegrated ? "✓" : "✗") << std::endl;
            std::cout << "Development: " << (result.developmentIntegrated ? "✓" : "✗") << std::endl;
            std::cout << "Checkpointing: " << (result.checkpointingWorks ? "✓" : "✗") << std::endl;
            
            std::cout << "=== Demo Complete ===" << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}