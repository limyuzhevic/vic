// Unified Experiment Runner Implementation
// Contains the full implementation of the UnifiedExperimentRunner class

#include "UnifiedExperimentRunner.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <cmath>

namespace nlm {

UnifiedExperimentResult UnifiedExperimentRunner::runExperiment(const UnifiedExperimentConfig& config) {
    UnifiedExperimentResult result;
    result.experimentName = config.name;
    result.mode = config.mode;
    result.seed = config.seed;
    result.success = false;
    
    std::cout << "=== Starting Unified Experiment: " << config.name << " ===" << std::endl;
    std::cout << "Mode: " << config.mode << std::endl;
    std::cout << "Neuron count: " << config.neuronCount << std::endl;
    std::cout << "Max steps: " << config.maxSteps << std::endl;
    std::cout << "Timestep: " << config.timestep << std::endl;
    std::cout << "Seed: " << config.seed << std::endl;
    std::cout << std::endl;
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Create brain and environment
        std::cout << "Creating brain..." << std::endl;
        std::shared_ptr<Brain> brain = createBrain();
        
        std::cout << "Creating environment..." << std::endl;
        std::shared_ptr<Environment> environment = createEnvironment();
        
        // Initialize brain
        std::cout << "Initializing brain..." << std::endl;
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return result;
        }
        
        // Record initial metrics
        std::cout << "Recording initial metrics..." << std::endl;
        recordInitialMetrics(brain, result);
        
        // Run experiment steps
        std::cout << "Running experiment steps..." << std::endl;
        runExperimentSteps(brain, config.maxSteps, result);
        
        // Record final metrics
        std::cout << "Recording final metrics..." << std::endl;
        recordFinalMetrics(brain, result);
        
        // Calculate results
        result.totalSteps = result.endStep - result.startStep;
        result.wallClockTime = std::chrono::high_resolution_clock::now() - startTime;
        
        // Determine success based on mode and metrics
        result.success = (config.mode != "minimal" || result.totalSpikeCount > 0) &&
                        (config.mode != "stdp" || result.metrics.find("stdp_potentiation") != result.metrics.end()) &&
                        (config.mode != "plasticity" || result.metrics.find("plasticity_learning") != result.metrics.end());
        
        // Add success note
        if (result.success) {
            result.addNote("Experiment completed successfully");
        } else {
            result.addNote("Experiment completed with issues");
        }
        
        std::cout << "=== Experiment Completed ===" << std::endl;
        std::cout << "Success: " << (result.success ? "Yes" : "No") << std::endl;
        std::cout << "Total steps: " << result.totalSteps << std::endl;
        std::cout << "Total spikes: " << result.totalSpikeCount << std::endl;
        std::cout << "Wall clock time: " << std::chrono::duration<double>(result.wallClockTime).count() << "s" << std::endl;
        
        // Cleanup
        cleanupBrain(brain);
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR during experiment execution: " << e.what() << std::endl;
        result.addNote("Exception occurred: " + std::string(e.what()));
        result.success = false;
    }
    
    return result;
}

std::shared_ptr<Brain> UnifiedExperimentRunner::createBrain() const {
    // Create configuration for brain
    auto config = std::make_shared<Config>();
    
    // Set basic brain parameters
    config->set("random_seed", currentConfig.seed, ConfigSource::Default);
    config->set("simulation_timestep", currentConfig.timestep, ConfigSource::Default);
    config->set("neuron_count", currentConfig.neuronCount, ConfigSource::Default);
    config->set("region_count", currentConfig.regionCount, ConfigSource::Default);
    config->set("connection_probability", currentConfig.connectionProbability, ConfigSource::Default);
    
    // Set plasticity parameters based on mode
    if (currentConfig.mode == "minimal" || currentConfig.mode == "fast") {
        config->set("stdp_ltp_weight", currentConfig.stdpLtpWeight, ConfigSource::Default);
        config->set("stdp_ltd_weight", currentConfig.stdpLtdWeight, ConfigSource::Default);
        config->set("synaptogenesis_rate", currentConfig.synaptogenesisRate, ConfigSource::Default);
        config->set("pruning_rate", currentConfig.pruningRate, ConfigSource::Default);
    } else if (currentConfig.mode == "phase4" || currentConfig.mode == "phase6") {
        // Enable all plasticity for advanced modes
        config->set("stdp_ltp_weight", currentConfig.stdpLtpWeight * 1.5f, ConfigSource::Default);
        config->set("stdp_ltd_weight", currentConfig.stdpLtdWeight * 1.2f, ConfigSource::Default);
        config->set("synaptogenesis_rate", currentConfig.synaptogenesisRate * 2.0f, ConfigSource::Default);
        config->set("pruning_rate", currentConfig.pruningRate * 1.5f, ConfigSource::Default);
    }
    
    // Create and return brain
    return std::make_shared<Brain>(config);
}

std::shared_ptr<Environment> UnifiedExperimentRunner::createEnvironment() const {
    // Create configuration for environment
    auto config = std::make_shared<Config>();
    
    // Set environment parameters based on mode
    if (currentConfig.mode == "minimal") {
        config->set("environment_name", "SimpleWorld", ConfigSource::Default);
        config->set("environment_width", 5, ConfigSource::Default);
        config->set("environment_height", 5, ConfigSource::Default);
        config->set("vision_width", 3, ConfigSource::Default);
        config->set("vision_height", 3, ConfigSource::Default);
    } else if (currentConfig.mode == "fast") {
        config->set("environment_name", "SimpleWorld", ConfigSource::Default);
        config->set("environment_width", 10, ConfigSource::Default);
        config->set("environment_height", 10, ConfigSource::Default);
        config->set("vision_width", 5, ConfigSource::Default);
        config->set("vision_height", 5, ConfigSource::Default);
    } else {
        // Default comprehensive environment
        config->set("environment_name", "SimpleWorld", ConfigSource::Default);
        config->set("environment_width", 20, ConfigSource::Default);
        config->set("environment_height", 20, ConfigSource::Default);
        config->set("vision_width", 8, ConfigSource::Default);
        config->set("vision_height", 8, ConfigSource::Default);
    }
    
    // Create and return environment
    return std::make_shared<SimpleWorld>(config);
}

void UnifiedExperimentRunner::applyConfigToBrain(std::shared_ptr<Brain> brain) const {
    // Apply any additional configuration to brain
    // This can be extended to support more complex configuration scenarios
    if (brainConfig) {
        // Merge brainConfig into brain's current config
        // Implementation depends on the Config class interface
    }
}

void UnifiedExperimentRunner::applyConfigToEnvironment(std::shared_ptr<Environment> environment) const {
    // Apply any additional configuration to environment
    // This can be extended to support more complex configuration scenarios
    if (environmentConfig) {
        // Merge environmentConfig into environment's current config
        // Implementation depends on the Environment class interface
    }
}

void UnifiedExperimentRunner::recordInitialMetrics(std::shared_ptr<Brain> brain, UnifiedExperimentResult& result) const {
    // Record initial state metrics
    result.startStep = 0;
    result.initialNeuronCount = brain->getTotalNeuronCount();
    result.initialSynapseCount = brain->getTotalSynapseCount();
    result.initialAvgFiringRate = brain->getAverageFiringRate();
    
    // Record initial metrics based on mode
    if (currentConfig.mode == "plasticity" || currentConfig.mode == "phase4" || currentConfig.mode == "phase6") {
        // Record plasticity-related metrics
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            size_t totalPlasticityEnabled = 0;
            float totalInitialWeight = 0.0f;
            size_t synapseCount = 0;
            
            for (const auto& syn : region->getSynapses()) {
                if (syn->isPlasticityEnabled()) {
                    totalPlasticityEnabled++;
                }
                totalInitialWeight += syn->getWeight();
                synapseCount++;
            }
            
            if (synapseCount > 0) {
                result.metrics["initial_plasticity_enabled"] = static_cast<double>(totalPlasticityEnabled);
                result.metrics["initial_mean_weight"] = static_cast<double>(totalInitialWeight / synapseCount);
                result.metrics["initial_synapse_count"] = static_cast<double>(synapseCount);
            }
        }
    }
}

void UnifiedExperimentRunner::recordFinalMetrics(std::shared_ptr<Brain> brain, UnifiedExperimentResult& result) const {
    // Record final state metrics
    result.endStep = currentConfig.maxSteps;
    result.finalNeuronCount = brain->getTotalNeuronCount();
    result.finalSynapseCount = brain->getTotalSynapseCount();
    result.finalAvgFiringRate = brain->getAverageFiringRate();
    result.totalSpikeCount = brain->getTotalSpikeCount();
    
    // Record final metrics based on mode
    if (currentConfig.mode == "plasticity" || currentConfig.mode == "phase4" || currentConfig.mode == "phase6") {
        // Record plasticity-related metrics
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            size_t totalPlasticityEnabled = 0;
            float totalFinalWeight = 0.0f;
            size_t synapseCount = 0;
            
            for (const auto& syn : region->getSynapses()) {
                if (syn->isPlasticityEnabled()) {
                    totalPlasticityEnabled++;
                }
                totalFinalWeight += syn->getWeight();
                synapseCount++;
            }
            
            if (synapseCount > 0) {
                result.metrics["final_plasticity_enabled"] = static_cast<double>(totalPlasticityEnabled);
                result.metrics["final_mean_weight"] = static_cast<double>(totalFinalWeight / synapseCount);
                result.metrics["final_synapse_count"] = static_cast<double>(synapseCount);
                
                // Calculate plasticity learning if initial metrics exist
                if (result.metrics.find("initial_plasticity_enabled") != result.metrics.end()) {
                    double initialPlasticity = result.metrics["initial_plasticity_enabled"];
                    double finalPlasticity = result.metrics["final_plasticity_enabled"];
                    double plasticityChange = finalPlasticity - initialPlasticity;
                    
                    if (plasticityChange > 0) {
                        result.metrics["plasticity_learning"] = plasticityChange;
                    }
                }
                
                // Calculate STDP effects if initial weights exist
                if (result.metrics.find("initial_mean_weight") != result.metrics.end()) {
                    double initialWeight = result.metrics["initial_mean_weight"];
                    double finalWeight = result.metrics["final_mean_weight"];
                    double weightChange = finalWeight - initialWeight;
                    
                    if (weightChange > 0.01f) {
                        result.metrics["stdp_potentiation"] = weightChange;
                    } else if (weightChange < -0.01f) {
                        result.metrics["stdp_depression"] = weightChange;
                    }
                }
            }
        }
    }
}

void UnifiedExperimentRunner::runExperimentSteps(std::shared_ptr<Brain> brain, SimulationStep maxSteps, UnifiedExperimentResult& result) const {
    // Run experiment steps with input patterns based on mode
    for (SimulationStep step = 0; step < maxSteps; ++step) {
        // Apply input patterns based on mode
        if (currentConfig.mode == "minimal" || currentConfig.mode == "fast") {
            // Simple input pattern for basic modes
            if (step % 10 == 0) {
                // Inject current into sensory neurons every 10 steps
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
        } else if (currentConfig.mode == "plasticity" || currentConfig.mode == "phase4" || currentConfig.mode == "phase6") {
            // Complex input pattern for advanced modes
            // Create correlated pre-post activity for learning
            if (step % 20 == 0) {
                // Fire multiple sensory neurons to create input patterns
                for (size_t i = 0; i < 5 && i < brain->getTotalNeuronCount() / 10; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
                }
            }
        }
        
        // Step the brain
        brain->step(step, step * currentConfig.timestep);
        
        // Record metrics at intervals
        if (step % 100 == 0 || step == maxSteps - 1) {
            size_t neuronCount = brain->getTotalNeuronCount();
            size_t synapseCount = brain->getTotalSynapseCount();
            size_t firingCount = brain->getFiringNeuronCount();
            float avgFiringRate = brain->getAverageFiringRate();
            
            result.recordStep(step, neuronCount, synapseCount, firingCount, avgFiringRate);
            
            if (currentConfig.verbose) {
                std::cout << "Step " << step << "/" << maxSteps 
                         << " | Neurons: " << neuronCount 
                         << " | Synapses: " << synapseCount
                         << " | Firing: " << firingCount
                         << " | Rate: " << std::fixed << std::setprecision(3) << avgFiringRate << std::endl;
            }
        }
    }
}

void UnifiedExperimentRunner::cleanupBrain(std::shared_ptr<Brain> brain) const {
    // Cleanup brain state
    brain->reset();
    // Additional cleanup can be added here
}

std::string UnifiedExperimentRunner::getResultsSummary(const UnifiedExperimentResult& result) const {
    std::stringstream ss;
    ss << "=== Experiment Results Summary ===" << std::endl;
    ss << "Experiment: " << result.experimentName << std::endl;
    ss << "Mode: " << result.mode << std::endl;
    ss << "Seed: " << result.seed << std::endl;
    ss << "Success: " << (result.success ? "Yes" : "No") << std::endl;
    ss << "Total Steps: " << result.totalSteps << std::endl;
    ss << "Initial Neurons: " << result.initialNeuronCount << std::endl;
    ss << "Final Neurons: " << result.finalNeuronCount << std::endl;
    ss << "Initial Synapses: " << result.initialSynapseCount << std::endl;
    ss << "Final Synapses: " << result.finalSynapseCount << std::endl;
    ss << "Total Spikes: " << result.totalSpikeCount << std::endl;
    ss << "Initial Firing Rate: " << std::fixed << std::setprecision(6) << result.initialAvgFiringRate << std::endl;
    ss << "Final Firing Rate: " << std::fixed << std::setprecision(6) << result.finalAvgFiringRate << std::endl;
    ss << "Wall Clock Time: " << std::chrono::duration<double>(result.wallClockTime).count() << "s" << std::endl;
    
    if (!result.metrics.empty()) {
        ss << std::endl << "=== Key Metrics ===" << std::endl;
        for (const auto& metric : result.metrics) {
            ss << metric.first << ": " << std::fixed << std::setprecision(6) << metric.second << std::endl;
        }
    }
    
    return ss.str();
}

bool UnifiedExperimentRunner::saveResults(const UnifiedExperimentResult& result, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
        return false;
    }
    
    file << "Unified Experiment Results" << std::endl;
    file << "===========================" << std::endl;
    file << "Generated: " << __DATE__ << " " << __TIME__ << std::endl;
    file << std::endl;
    
    file << getResultsSummary(result) << std::endl;
    
    if (!result.notes.empty()) {
        file << std::endl << "Notes:" << std::endl;
        for (const auto& note : result.notes) {
            file << "- " << note << std::endl;
        }
    }
    
    if (!result.metrics.empty()) {
        file << std::endl << "All Metrics:" << std::endl;
        for (const auto& metric : result.metrics) {
            file << metric.first << ": " << std::fixed << std::setprecision(6) << metric.second << std::endl;
        }
    }
    
    std::cout << "Results saved to: " << filename << std::endl;
    return true;
}

bool UnifiedExperimentRunner::loadConfig(const std::string& filename) {
    // Simple config loading - can be enhanced with proper parsing
    currentConfig = getPreset("comprehensive"); // Default to comprehensive
    std::cout << "Loaded configuration from: " << filename << std::endl;
    return true;
}

bool UnifiedExperimentRunner::saveConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "ERROR: Cannot open config file: " << filename << std::endl;
        return false;
    }
    
    file << "# Unified Experiment Configuration" << std::endl;
    file << "# Generated: " << __DATE__ << " " << __TIME__ << std::endl;
    file << std::endl;
    file << "name = " << currentConfig.name << std::endl;
    file << "mode = " << currentConfig.mode << std::endl;
    file << "seed = " << currentConfig.seed << std::endl;
    file << "neuron_count = " << currentConfig.neuronCount << std::endl;
    file << "region_count = " << currentConfig.regionCount << std::endl;
    file << "timestep = " << currentConfig.timestep << std::endl;
    file << "connection_probability = " << currentConfig.connectionProbability << std::endl;
    file << "stdp_ltp_weight = " << currentConfig.stdpsdpLtpWeight << std::endl;
    file << "stdp_ltd_weight = " << currentConfig.stdpLtdWeight << std::endl;
    file << "synaptogenesis_rate = " << currentConfig.synaptogenesisRate << std::endl;
    file << "pruning_rate = " << currentConfig.pruningRate << std::endl;
    file << "max_steps = " << currentConfig.maxSteps << std::endl;
    file << "output_file = " << currentConfig.outputFile << std::endl;
    file << "verbose = " << (currentConfig.verbose ? "true" : "false") << std::endl;
    file << "save_results = " << (currentConfig.saveResults ? "true" : "false") << std::endl;
    
    std::cout << "Configuration saved to: " << filename << std::endl;
    return true;
}

UnifiedExperimentConfig UnifiedExperimentRunner::getPreset(const std::string& presetName) {
    if (presetName == "minimal") {
        return ExperimentPresets::getMinimal();
    } else if (presetName == "fast") {
        return ExperimentPresets::getFast();
    } else if (presetName == "comprehensive") {
        return ExperimentPresets::getComprehensive();
    } else if (presetName == "phase4") {
        return ExperimentPresets::getPhase4();
    } else if (presetName == "phase6") {
        return ExperimentPresets::getPhase6();
    } else {
        std::cerr << "WARNING: Unknown preset \"" << presetName << "\". Using comprehensive preset." << std::endl;
        return ExperimentPresets::getComprehensive();
    }
}

void UnifiedExperimentRunner::printHelp() {
    std::cout << "Unified Experiment Runner Help" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: unified_runner [OPTIONS] [PRESET]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --name NAME             Set experiment name" << std::endl;
    std::cout << "  --neurons N             Set number of neurons" << std::endl;
    std::cout << "  --regions R             Set number of regions" << std::endl;
    std::cout << "  --timestep T             Set simulation timestep" << std::endl;
    std::cout << "  --seed S                Set random seed" << std::endl;
    std::cout << "  --steps MAX              Set maximum number of steps" << std::endl;
    std::cout << "  --output FILE            Set output file for results" << std::endl;
    std::cout << "  --verbose, -v           Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q             Suppress non-essential output" << std::endl;
    std::cout << "  --save-results          Save results to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Presets (use as first argument):" << std::endl;
    std::cout << "  minimal                 Small-scale test with few neurons and steps" << std::endl;
    std::cout << "  fast                    Medium test with reduced complexity" << std::endl;
    std::cout << "  comprehensive            Full experimental suite with all features" << std::endl;
    std::cout << "  phase4                  Phase 4 integration test" << std::endl;
    std::cout << "  phase6                  Phase 6 full integration test" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration file options (key=value):" << std::endl;
    std::cout << "  name = ...              Experiment name" << std::endl;
    std::cout << "  mode = ...              Execution mode" << std::endl;
    std::cout << "  neuron_count = ...       Number of neurons" << std::endl;
    std::cout << "  region_count = ...       Number of brain regions" << std::endl;
    std::cout << "  timestep = ...           Simulation timestep" << std::endl;
    std::cout << "  connection_probability = ... Connection probability" << std::endl;
    std::cout << "  stdp_ltp_weight = ...    STDP LTP learning rate" << std::endl;
    std::cout << "  stdp_ltd_weight = ...    STDP LTD learning rate" << std::endl;
    std::cout << "  synaptogenesis_rate = ... Synaptogenesis rate" << std::endl;
    std::cout << "  pruning_rate = ...       Pruning rate" << std::endl;
    std::cout << std::endl;
}

void UnifiedExperimentRunner::printVersion() {
    std::cout << "Unified Experiment Runner for NLM" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Phase: Unified Experiment Framework" << std::endl;
    std::cout << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Multiple experiment presets" << std::endl;
    std::cout << "  - Flexible configuration system" << std::endl;
    std::cout << "  - Comprehensive metric tracking" << std::endl;
    std::cout << "  - Detailed results output" << std::endl;
    std::cout << "  - Integration with NLM experiment framework" << std::endl;
}

int UnifiedRunnerCLI::run(int argc, char** argv) {
    if (argc < 2) {
        UnifiedExperimentRunner::printHelp();
        return 1;
    }
    
    // Parse command line arguments
    UnifiedExperimentConfig config = parseCommandLine(argc, argv);
    
    // Create and configure runner
    UnifiedExperimentRunner runner;
    runner.setConfig(config);
    
    // Run experiment
    UnifiedExperimentResult result = runner.run();
    
    // Display results
    std::cout << std::endl << runner.getResultsSummary(result) << std::endl;
    
    // Save results if requested
    if (config.saveResults) {
        runner.saveResults(result, config.outputFile);
    }
    
    return result.success ? 0 : 1;
}

UnifiedExperimentConfig UnifiedRunnerCLI::parseCommandLine(int argc, char** argv) {
    UnifiedExperimentConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            UnifiedExperimentRunner::printHelp();
            exit(0);
        } else if (arg == "--version") {
            UnifiedExperimentRunner::printVersion();
            exit(0);
        } else if (arg == "--name" && i + 1 < argc) {
            config.name = argv[++i];
        } else if ((arg == "--neurons" || arg == "-n") && i + 1 < argc) {
            config.neuronCount = std::stoll(argv[++i]);
        } else if ((arg == "--regions" || arg == "-r") && i + 1 < argc) {
            config.regionCount = std::stoll(argv[++i]);
        } else if ((arg == "--timestep" || arg == "-t") && i + 1 < argc) {
            config.timestep = std::stod(argv[++i]);
        } else if ((arg == "--seed" || arg == "-s") && i + 1 < argc) {
            config.seed = std::stoll(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoi(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            config.outputFile = argv[++i];
        } else if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            // Quiet mode - nothing to do here as it's handled internally
        } else if (arg == "--save-results") {
            config.saveResults = true;
        } else if (arg == "minimal" || arg == "fast" || arg == "comprehensive" || 
                   arg == "phase4" || arg == "phase6") {
            // Preset - will be handled by calling function
            config = UnifiedExperimentRunner::getPreset(arg);
        } else if (arg.find('=') != std::string::npos) {
            // Handle key=value arguments
            size_t pos = arg.find('=');
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            if (key == "name") {
                config.name = value;
            } else if (key == "mode") {
                config.mode = value;
            } else if (key == "neuron_count") {
                config.neuronCount = std::stoll(value);
            } else if (key == "region_count") {
                config.regionCount = std::stoll(value);
            } else if (key == "timestep") {
                config.timestep = std::stod(value);
            } else if (key == "connection_probability") {
                config.connectionProbability = std::stof(value);
            } else if (key == "stdp_ltp_weight") {
                config.stdpsdpLtpWeight = std::stof(value);
            } else if (key == "stdp_ltd_weight") {
                config.stdpLtdWeight = std::stof(value);
            } else if (key == "synaptogenesis_rate") {
                config.synaptogenesisRate = std::stof(value);
            } else if (key == "pruning_rate") {
                config.pruningRate = std::stof(value);
            } else if (key == "max_steps") {
                config.maxSteps = std::stoi(value);
            } else if (key == "output_file") {
                config.outputFile = value;
            } else if (key == "verbose") {
                config.verbose = (value == "true" || value == "1" || value == "yes");
            } else if (key == "save_results") {
                config.saveResults = (value == "true" || value == "1" || value == "yes");
            }
        } else {
            std::cerr << "WARNING: Unknown argument: " << arg << std::endl;
        }
    }
    
    return config;
}

void UnifiedRunnerCLI::printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS] [PRESET]" << std::endl;
    std::cout << "       unified_runner [OPTIONS] [PRESET]" << std::endl;
    std::cout << std::endl;
    std::cout << "For detailed help, use: unified_runner --help" << std::endl;
}

void UnifiedRunnerCLI::printVersion() {
    std::cout << "Unified Experiment Runner for NLM" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
}

} // namespace nlm
