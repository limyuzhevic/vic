// NLM (熙然) - Neural Learning Machine
// Advanced Demo: Comprehensive CLI and Research Features
// Phase 2+: Enhanced Neural Computation with Advanced Options

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <chrono>
#include <algorithm>

using namespace nlm;

// Forward declarations
void printBanner(const std::string& title = "NLM - Neural Learning Machine");
void printHelp();
struct AdvancedConfig;
void configureBrainForResearch(std::shared_ptr<Brain> brain, const AdvancedConfig& config);
void runResearchExperiments(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, const AdvancedConfig& config);
void exportAnalysisResults(const std::string& filename, const std::vector<std::string>& data);
void importConfigurationFromFile(std::shared_ptr<Config> config, const std::string& filepath);
void setupAdvancedLogging(const std::string& logFile, LogLevel level);
void profileSimulation(std::shared_ptr<Brain> brain, uint64_t steps, std::chrono::high_resolution_clock::time_point& startTime);

// Configuration structure for advanced usage
struct AdvancedConfig {
    std::string configFile;
    std::string outputFile;
    std::string checkpointFile;
    std::string logFile;
    uint64_t simulationSteps = 1000;
    uint64_t recordingInterval = 100;
    double timestep = 0.001;
    uint64_t neuronCount = 1000;
    uint64_t regionCount = 2;
    float connectionProbability = 0.1f;
    uint64_t randomSeed = 42;
    std::string mode = "demo"; // demo, research, benchmark, debug
    bool enableProfiling = false;
    bool enableCheckpointing = true;
    bool enableLogging = true;
    bool exportResults = true;
    std::string analysisType = "comprehensive"; // basic, comprehensive, neuroscience
    uint64_t memoryRecordingInterval = 100;
    uint64_t plasticityRecordingInterval = 50;
    uint64_t neuromodulationRecordingInterval = 25;
};

AdvancedConfig parseCommandLine(int argc, char** argv);

int main(int argc, char** argv) {
    printBanner("NLM - Advanced Neural Learning Machine with Research Features");
    
    // Parse command line arguments
    AdvancedConfig config = parseCommandLine(argc, argv);
    
    // Setup advanced logging
    if (config.enableLogging) {
        setupAdvancedLogging(config.logFile, LogLevel::Info);
    }
    
    NLM_LOG_INFO("=== NLM Advanced Configuration ===");
    NLM_LOG_INFO("Mode: " + config.mode);
    NLM_LOG_INFO("Simulation steps: " + std::to_string(config.simulationSteps));
    NLM_LOG_INFO("Neuron count: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("Region count: " + std::to_string(config.regionCount));
    NLM_LOG_INFO("Connection probability: " + std::to_string(config.connectionProbability));
    NLM_LOG_INFO("Random seed: " + std::to_string(config.randomSeed));
    NLM_LOG_INFO("Checkpointing: " + std::string(config.enableCheckpointing ? "enabled" : "disabled"));
    NLM_LOG_INFO("Profiling: " + std::string(config.enableProfiling ? "enabled" : "disabled"));
    NLM_LOG_INFO("Output file: " + config.outputFile);
    NLM_LOG_INFO("===============================");
    
    // Load base configuration
    auto baseConfig = std::make_shared<Config>();
    
    // Load from config file if specified
    if (!config.configFile.empty()) {
        NLM_LOG_INFO("Loading configuration from: " + config.configFile);
        if (baseConfig->loadFromFile(config.configFile)) {
            NLM_LOG_INFO("Successfully loaded configuration");
        } else {
            NLM_LOG_WARNING("Failed to load configuration file, using defaults");
        }
    }
    
    // Override with command line args
    baseConfig->set("random_seed", static_cast<int64_t>(config.randomSeed), ConfigSource::CommandLine);
    baseConfig->set("simulation_timestep", config.timestep, ConfigSource::CommandLine);
    baseConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::CommandLine);
    baseConfig->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::CommandLine);
    baseConfig->set("connection_probability", config.connectionProbability, ConfigSource::CommandLine);
    
    // Initialize simulation clock
    SimulationClock clock(config.timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(config.timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain (Phase 2+: Advanced)...");
    auto brain = std::make_shared<Brain>(baseConfig);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Create world for interaction
    SimpleWorld world;
    world.configure(100.0f, 100.0f, 20, 20);
    world.setRandomSeed(config.randomSeed);
    
    // Create agent brain interface
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Setup based on mode
    if (config.mode == "research") {
        configureBrainForResearch(brain, config);
    } else if (config.mode == "debug") {
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        agent.enableDevelopment(true);
    }
    
    // Collect simulation data for analysis
    std::vector<std::string> simulationData;
    std::vector<std::string> plasticityData;
    std::vector<std::string> neuromodulationData;
    std::vector<std::string> memoryData;
    
    uint64_t step = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Starting simulation (mode: " + config.mode + ")...");
    
    // Main simulation loop with data collection
    for (step = 0; step < config.simulationSteps; ++step) {
        // Get sensory input
        const SensoryPercept& percept = world.getSensoryPercept();
        
        // Process input through agent
        agent.processSensoryInput(percept);
        
        // Run brain simulation steps
        for (int i = 0; i < 10; ++i) {
            brain->step(step * 10 + i, static_cast<Timestamp>((step * 10 + i) * config.timestep));
        }
        
        // Decode motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action in world
        ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
        
        // Apply reward modulation
        agent.applyRewardModulation(result.reward, 0.0f);
        
        // Update world
        world.update(config.timestep);
        
        // Record data at specified intervals
        if (step % config.recordingInterval == 0) {
            std::string dataLine = "Step " + std::to_string(step) +
                " reward=" + std::to_string(result.reward) +
                " action=" + std::to_string(static_cast<int>(cmd)) +
                " firing=" + std::to_string(brain->getFiringNeuronCount()) +
                " spikes=" + std::to_string(brain->getTotalSpikeCount()) +
                " workingMem=" + std::to_string(brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryNeuronCount() : 0);
            simulationData.push_back(dataLine);
            
            if (config.enableProfiling && step % config.plasticityRecordingInterval == 0) {
                // Record plasticity statistics
                float plasticityActivity = brain->getExcitationInhibitionRatio();
                std::string plasticityLine = "Plasticity step " + std::to_string(step) + 
                    " E/I ratio=" + std::to_string(plasticityActivity);
                plasticityData.push_back(plasticityLine);
            }
            
            if (config.enableProfiling && step % config.neuromodulationRecordingInterval == 0) {
                // Record neuromodulation statistics
                float novelty = agent.getNoveltyLevel();
                float curiosity = agent.getCuriosityLevel();
                std::string neuromodLine = "Neuromodulation step " + std::to_string(step) +
                    " novelty=" + std::to_string(novelty) +
                    " curiosity=" + std::to_string(curiosity);
                neuromodulationData.push_back(neuromodLine);
            }
            
            if (config.enableProfiling && step % config.memoryRecordingInterval == 0) {
                // Record memory statistics
                size_t workingMem = brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryNeuronCount() : 0;
                size_t episodicMem = brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0;
                std::string memLine = "Memory step " + std::to_string(step) +
                    " working=" + std::to_string(workingMem) +
                    " episodic=" + std::to_string(episodicMem);
                memoryData.push_back(memLine);
            }
        }
        
        // Run research experiments if in research mode
        if (config.mode == "research" && step > config.simulationSteps / 2) {
            runResearchExperiments(brain, world, config);
        }
        
        // Log progress periodically
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + "/" + std::to_string(config.simulationSteps) +
                " | Reward: " + std::to_string(result.reward) +
                " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    // Profile simulation if enabled
    if (config.enableProfiling) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        NLM_LOG_INFO("Simulation completed in " + std::to_string(duration.count()) + " ms");
        NLM_LOG_INFO("Average steps per second: " + std::to_string(static_cast<double>(config.simulationSteps) * 1000.0 / duration.count()));
    }
    
    // Checkpoint if enabled
    if (config.enableCheckpointing) {
        const std::string checkpointFile = config.checkpointFile.empty() ? "/tmp/nlm_advanced_demo.ckpt" : config.checkpointFile;
        bool saved = brain->save(checkpointFile);
        NLM_LOG_INFO("Checkpoint saved: " + std::string(saved ? "yes" : "NO"));
    }
    
    // Export results if enabled
    if (config.exportResults) {
        if (!config.outputFile.empty()) {
            // Export simulation results
            exportAnalysisResults(config.outputFile + ".csv", simulationData);
            
            // Export specialized analysis files
            if (!plasticityData.empty()) {
                exportAnalysisResults(config.outputFile + "_plasticity.csv", plasticityData);
            }
            if (!neuromodulationData.empty()) {
                exportAnalysisResults(config.outputFile + "_neuromodulation.csv", neuromodulationData);
            }
            if (!memoryData.empty()) {
                exportAnalysisResults(config.outputFile + "_memory.csv", memoryData);
            }
            
            NLM_LOG_INFO("Results exported to " + config.outputFile + ".*");
        }
    }
    
    // Final statistics
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== FINAL SIMULATION STATISTICS ===");
    NLM_LOG_INFO("Total steps completed: " + std::to_string(step));
    NLM_LOG_INFO("Total spikes generated: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("Final E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    NLM_LOG_INFO("Data records collected: " + std::to_string(simulationData.size()));
    NLM_LOG_INFO("");
    
    brain->logStatus();
    
    NLM_LOG_INFO("Advanced simulation complete. Check output files for detailed analysis.")
    
    return 0;
}

void printBanner(const std::string& title) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     " NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2+: Advanced Neural Computation                     ║
    ║                                                               ║
    ║     Enhanced features for research and advanced users         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << "  Advanced neural simulation with comprehensive CLI options" << std::endl;
    std::cout << "  for research, debugging, and detailed analysis.\n" << std::endl;
}

void printHelp() {
    std::cout << "=== NLM Advanced CLI Help ===\n" << std::endl;
    std::cout << "Usage: nlm_advanced [OPTIONS]\n\n";
    
    std::cout << "Mode Options:\n";
    std::cout << "  --mode demo              Demo mode (default) - basic functionality\n";
    std::cout << "  --mode research          Research mode - advanced experiments\n";
    std::cout << "  --mode debug             Debug mode - enhanced logging and checks\n";
    std::cout << "  --mode benchmark         Benchmark mode - performance testing\n\n";
    
    std::cout << "Configuration Options:\n";
    std::cout << "  --config file.cfg        Load configuration from file\n";
    std::cout << "  --neurons N              Number of neurons (default: 1000)\n";
    std::cout << "  --regions R              Number of regions (default: 2)\n";
    std::cout << "  --connect PROB           Connection probability (default: 0.1)\n";
    std::cout << "  --seed SEED              Random seed (default: 42)\n";
    std::cout << "  --timestep T              Simulation timestep (default: 0.001)\n\n";
    
    std::cout << "Output Options:\n";
    std::cout << "  --output FILE            Output file prefix for results\n";
    std::cout << "  --checkpoint FILE        Checkpoint file path\n";
    std::cout << "  --log FILE               Log file path\n";
    std::cout << "  --no-export              Disable result export\n\n";
    
    std::cout << "Performance Options:\n";
    std::cout << "  --steps N                Number of simulation steps (default: 1000)\n";
    std::cout << "  --profile                 Enable simulation profiling\n";
    std::cout << "  --checkpoint             Enable checkpointing\n\n";
    
    std::cout << "Analysis Options:\n";
    std::cout << "  --analysis basic         Analysis type: basic (default)\n";
    std::cout << "  --analysis comprehensive Comprehensive analysis with all metrics\n";
    std::cout << "  --analysis neuroscience   Neuroscience-focused analysis\n\n";
    
    std::cout << "Debug Options:\n";
    std::cout << "  --debug                  Enable debug logging\n";
    std::cout << "  --verbose                Enable verbose output\n\n";
    
    std::cout << "Example Usage:\n";
    std::cout << "  nlm_advanced --mode research --steps 5000 --neurons 2000 --output results/ \
      --config configs/phase6_advanced.cfg --profile\n";
    std::cout << "  nlm_advanced --mode debug --log debug.log --verbose --steps 2000\n";
    std::cout << "  nlm_advanced --mode benchmark --steps 10000 --profile\n\n";
}

AdvancedConfig parseCommandLine(int argc, char** argv) {
    AdvancedConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                config.mode = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                config.outputFile = argv[++i];
            }
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) {
                config.checkpointFile = argv[++i];
            }
        } else if (arg == "--log") {
            if (i + 1 < argc) {
                config.logFile = argv[++i];
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                config.neuronCount = std::stoull(argv[++i]);
            }
        } else if (arg == "--regions") {
            if (i + 1 < argc) {
                config.regionCount = std::stoull(argv[++i]);
            }
        } else if (arg == "--connect") {
            if (i + 1 < argc) {
                config.connectionProbability = std::stof(argv[++i]);
            }
        } else if (arg == "--seed") {
            if (i + 1 < argc) {
                config.randomSeed = std::stoull(argv[++i]);
            }
        } else if (arg == "--timestep") {
            if (i + 1 < argc) {
                config.timestep = std::stod(argv[++i]);
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                config.simulationSteps = std::stoull(argv[++i]);
            }
        } else if (arg == "--profile") {
            config.enableProfiling = true;
        } else if (arg == "--checkpoint") {
            config.enableCheckpointing = true;
        } else if (arg == "--no-export") {
            config.exportResults = false;
        } else if (arg == "--analysis") {
            if (i + 1 < argc) {
                config.analysisType = argv[++i];
            }
        } else if (arg == "--debug") {
            config.logFile = "debug.log";
        } else if (arg == "--verbose") {
            // Enable verbose logging
        } else {
            NLM_LOG_WARNING("Unknown argument: " + arg);
        }
    }
    
    // Set default log file if not specified
    if (config.logFile.empty()) {
        config.logFile = "nlm_advanced.log";
    }
    
    // Set default output file if not specified
    if (config.outputFile.empty()) {
        config.outputFile = "nlm_advanced_results";
    }
    
    return config;
}

void configureBrainForResearch(std::shared_ptr<Brain> brain, const AdvancedConfig& config) {
    NLM_LOG_INFO("Configuring brain for research mode...");
    
    // Enable all plasticity systems
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    // Setup neuromodulation for research
    if (auto* dopamine = brain->getDopamine()) {
        dopamine->setLevel(0.1f);
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->setLevel(0.5f);
    }
    
    // Setup memory systems for research
    if (auto* workingMem = brain->getWorkingMemory()) {
        workingMem->setCapacity(config.neuronCount / 20);
    }
    
    if (auto* episodicMem = brain->getEpisodicMemory()) {
        episodicMem->setMaxEpisodes(config.neuronCount / 10);
    }
    
    NLM_LOG_INFO("Research configuration complete.");
}

void runResearchExperiments(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, const AdvancedConfig& config) {
    NLM_LOG_INFO("Running research experiments...");
    
    // Example research experiment: Test pattern discrimination
    if (brain->getWorkingMemory()) {
        // Store some test patterns in working memory
        std::vector<NeuronId> testPatterns;
        
        // This would be expanded with actual research experiments
        // For now, just log that research mode is active
        NLM_LOG_INFO("Research experiments would run here with pattern discrimination, learning curves, etc.");
    }
}

void exportAnalysisResults(const std::string& filename, const std::vector<std::string>& data) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "Timestamp,Data\n";
        for (size_t i = 0; i < data.size(); ++i) {
            file << i << "," << data[i] << "\n";
        }
        file.close();
        NLM_LOG_INFO("Exported " + std::to_string(data.size()) + " records to " + filename);
    } else {
        NLM_LOG_ERROR("Failed to export analysis results to " + filename);
    }
}

void importConfigurationFromFile(std::shared_ptr<Config> config, const std::string& filepath) {
    if (config->loadFromFile(filepath)) {
        NLM_LOG_INFO("Configuration imported from " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to import configuration from " + filepath);
    }
}

void setupAdvancedLogging(const std::string& logFile, LogLevel level) {
    // This would setup advanced logging configuration
    // In a real implementation, this would configure file logging, rotation, etc.
    NLM_LOG_INFO("Advanced logging setup for file: " + logFile);
}

void profileSimulation(std::shared_ptr<Brain> brain, uint64_t steps, std::chrono::high_resolution_clock::time_point& startTime) {
    // Simulation profiling implementation
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("Simulation Profile:");
    NLM_LOG_INFO("  Total steps: " + std::to_string(steps));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("  Wall clock time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("  Steps per second: " + std::to_string(static_cast<double>(steps) * 1000.0 / duration.count()));
    NLM_LOG_INFO("  Memory usage estimate: Not implemented");
}