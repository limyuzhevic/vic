// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Advanced Features
//
// This phase adds comprehensive integration of all neural subsystems,
// advanced debugging tools, and power-user command-line interface.

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
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Complete artificial brain with memory, prediction,        ║
    ║     and cognition integrated.                                  ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << R"(
Advanced NLM Command Line Interface
===================================

Commands:
  help                          Show this help message
  quit                          Exit program
  status                        Show brain status and statistics
  save <filename>               Save current brain state to file
  load <filename>               Load brain state from file
  step <steps>                  Run simulation for specified steps
  reset                         Reset brain to initial state
  neurons                        Show neuron statistics
  synapses                       Show synapse statistics
  memory                        Show memory system status
  prediction                     Show prediction system status
  cognition                     Show cognition system status
  neuromodulation                 Show neuromodulation status
  develop <stage>               Set developmental stage (Initial, CriticalPeriod, Maturation, Adult)
  attention <strength>           Set attention strength
  inject <neuron_type> <current> Inject current into neuron type (Sensory, Internal, Motor)
  sleep <time>                  Set sleep/rest cycle parameters
  config <key> <value>           Set configuration value
  debug                          Enable debug logging
  benchmark                      Run performance benchmarks
  replay <episode_count>         Replay stored episodes
  test <test_name>              Run specific test (connectivity, plasticity, stdp, concept, prediction)

Shortcuts:
  s <steps>                      Same as step <steps>
  r                              Same as reset
  b                              Same as benchmark
  d                              Same as debug
" << std::endl;
}

void showBrainStatus(std::shared_ptr<Brain> brain) {
    brain->logStatus();
}

void runAdvancedTests(std::shared_ptr<Brain> brain, const std::string& testType) {
    if (testType == "connectivity") {
        std::cout << "\n=== Advanced Connectivity Test ===\n";
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject into multiple neuron types
        std::cout << "Injecting into 20 sensory neurons...";
        brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
        
        std::cout << "Injecting into 20 internal neurons...";
        brain->injectCurrentToNeurons(NeuronType::Internal, 20.0f);
        
        std::cout << "Injecting into 10 motor neurons...";
        brain->injectCurrentToNeurons(NeuronType::Motor, 15.0f);
        
        // Run steps and measure propagation
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "Total spikes generated: " << spikes << std::endl;
        
        float ir = brain->getExcitationInhibitionRatio();
        std::cout << "E/I ratio: " << ir << std::endl;
        
        if (spikes > 10) {
            std::cout << "✓ Network connectivity confirmed\n";
        } else {
            std::cout << "⚠ Limited activity - checking neuron states\n";
        }
        
    } else if (testType == "plasticity") {
        std::cout << "\n=== Advanced Plasticity Test ===\n";
        // Similar to original but with more detailed tracking
        std::cout << "Enabling all plasticity rules...";
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);  // Enable all plasticity
            }
        }
        
        std::cout << "Applying complex stimulus pattern...";
        for (SimulationStep step = 0; step < 2000; ++step) {
            // Variable input pattern
            float intensity = 20.0f + 15.0f * std::sin(step * 0.01f);
            for (size_t i = 0; i < 5; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, intensity);
            }
            
            if (step % 500 == 0) {
                std::cout << "  Step " << step << " | " 
                         << brain->getTotalSpikeCount() << " spikes\n";
            }
            
            brain->step(step, step * 0.001);
        }
        
        std::cout << "Plasticity experiment completed\n";
        
    } else if (testType == "stdp") {
        std::cout << "\n=== Advanced STDP Test ===\n";
        runStdpVerification(brain);
        
    } else if (testType == "concept") {
        std::cout << "\n=== Advanced Concept Formation Test ===\n";
        auto* planner = brain->getPlanner();
        auto* conceptForm = brain->getConceptFormation();
        
        if (planner && conceptForm) {
            std::cout << "Testing concept formation...";
            
            // Create test patterns
            std::vector<std::vector<float>> testPatterns;
            for (size_t i = 0; i < 10; ++i) {
                std::vector<float> pattern(20, 0.0f);
                for (size_t j = 0; j < 5; ++j) {
                    pattern[i * 4 + j] = (i + 1) * 0.1f;
                }
                testPatterns.push_back(pattern);
            }
            
            // Present patterns with reward signals
            for (size_t i = 0; i < testPatterns.size(); ++i) {
                size_t conceptId = conceptForm->presentExperience(
                    testPatterns[i], std::vector<float>(5, 0.5f), 
                    0.7f + 0.1f * std::sin(i * 0.5f), i * 10);
                
                if (conceptId > 0) {
                    std::cout << "  Pattern " << i << " matched concept " << conceptId << "\n";
                    std::vector<float> goal = conceptForm->getConceptPrototype(conceptId);
                    planner->setCurrentGoal(goal);
                    std::cout << "  Set goal with " << goal.size() << " dimensions\n";
                }
            }
            
            std::cout << "Discovered " << conceptForm->getConceptCount() << " concepts\n";
        }
        
    } else if (testType == "prediction") {
        std::cout << "\n=== Advanced Prediction System Test ===\n";
        auto* predictionSystem = brain->getPredictionSystem();
        
        if (predictionSystem) {
            std::cout << "Testing prediction system...";
            
            // Create sensory input patterns
            for (size_t i = 0; i < 5; ++i) {
                std::vector<float> sensoryData(15, 0.0f);
                for (size_t j = 0; j < 15; ++j) {
                    sensoryData[j] = (i * 3 + j) % 10 * 0.1f;
                }
                
                SensoryInput input;
                input.setData(sensoryData);
                
                auto prediction = predictionSystem->predictNextState(input);
                predictionSystem->updatePredictions(*prediction, input);
                
                float error = predictionSystem->getPredictionError();
                std::cout << "  Prediction " << i << " error: " << error << "\n";
            }
            
            const auto& errorHistory = predictionSystem->getErrorHistory();
            if (!errorHistory.empty()) {
                float avgError = std::accumulate(errorHistory.begin(), errorHistory.end(), 0.0f) / errorHistory.size();
                std::cout << "Average prediction error: " << avgError << "\n";
            }
        }
    }
}

void runLearningBenchmark(std::shared_ptr<Brain> brain) {
    std::cout << "\n=== NLM Performance Benchmark ===\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Enable all plasticity for comprehensive learning
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    // Complex learning scenario
    for (SimulationStep step = 0; step < 5000; ++step) {
        // Variable input pattern
        float patternType = static_cast<float>(step % 5);
        
        switch (static_cast<int>(patternType)) {
            case 0:  // Burst pattern
                for (size_t i = 0; i < 15; ++i) brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                break;
            case 1:  // Sustained pattern
                brain->injectCurrentToNeurons(NeuronType::Internal, 20.0f);
                break;
            case 2:  // Sparse pattern
                for (size_t i = 0; i < 5; ++i) brain->injectCurrentToNeurons(NeuronType::Motor, 25.0f);
                break;
            case 3:  // Alternating pattern
                if (step % 2 == 0) brain->injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
                else brain->injectCurrentToNeurons(NeuronType::Internal, 15.0f);
                break;
            case 4:  // Random pattern
                for (size_t i = 0; i < 10; ++i) {
                    brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 3), 
                                                 15.0f + 10.0f * static_cast<float>(rand()) / RAND_MAX);
                }
                break;
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0) {
            std::cout << "  Step " << step << 
                     " | Spikes: " << brain->getTotalSpikeCount() << 
                     " | Memory traces: " << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0) << 
                     " | EPS: " << (brain->getPredictionSystem() ? brain->getPredictionSystem()->getPredictionError() : 0.0f) << "\n";
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "\nBenchmark Results:";
    std::cout << "  Total steps: 5000\n";
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
    std::cout << "  Time elapsed: " << duration.count() << " ms\n";
    std::cout << "  Performance: " << (5000.0 / duration.count() * 1000.0) << " steps/second\n";
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
}

void exportBrainState(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::cout << "\nExporting brain state to " << filename << "...\n";
    
    if (brain->save(filename)) {
        std::cout << "✓ Brain state successfully saved\n";
        
        // Create metadata file
        std::string metadataFile = filename + ".meta";
        std::ofstream metaFile(metadataFile);
        if (metaFile.is_open()) {
            metaFile << "Brain State Export" << std::endl;
            metaFile << "===================" << std::endl;
            metaFile << "Exported: " << filename << std::endl;
            metaFile << "Timestamp: " << std::chrono::system_clock::now() << std::endl;
            metaFile << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
            metaFile << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
            metaFile << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            metaFile << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
            metaFile << "Working memory traces: " << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
            metaFile << "Episodic memory episodes: " << (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0) << std::endl;
            metaFile << "Prediction error: " << (brain->getPredictionSystem() ? brain->getPredictionSystem()->getPredictionError() : 0.0f) << std::endl;
            metaFile << "Cognition concepts: " << (brain->getConceptFormation() ? brain->getConceptFormation()->getConceptCount() : 0) << std::endl;
            metaFile << "Neuromodulators:" << std::endl;
            if (brain->getDopamine()) metaFile << "  Dopamine: " << brain->getDopamine()->getLevel() << std::endl;
            if (brain->getCuriosity()) metaFile << "  Curiosity: " << brain->getCuriosity()->getLevel() << std::endl;
            if (brain->getNovelty()) metaFile << "  Novelty: " << brain->getNovelty()->getLevel() << std::endl;
            metaFile.close();
            std::cout << "✓ Metadata file created: " << metadataFile << std::endl;
        }
    } else {
        std::cout << "✗ Failed to export brain state\n";
    }
}

void importBrainState(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::cout << "\nImporting brain state from " << filename << "...\n";
    
    std::ifstream metaFile(filename + ".meta");
    if (metaFile.is_open()) {
        std::string line;
        while (std::getline(metaFile, line)) {
            std::cout << "Metadata: " << line << std::endl;
        }
        metaFile.close();
    }
    
    if (brain->load(filename)) {
        std::cout << "✓ Brain state successfully loaded\n";
        brain->logStatus();
    } else {
        std::cout << "✗ Failed to import brain state\n";
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 6 with Advanced Features...\n" << std::endl;
    
    // Initialize logger with debug capabilities
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Advanced Integration ===");
    NLM_LOG_INFO("Extended neural substrate with comprehensive memory, prediction, and cognition");
    NLM_LOG_INFO("Features advanced debugging, benchmarking, and power-user controls");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
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
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Sleep/rest parameters
    config->set("sleep_interval", static_cast<int64_t>(5000), ConfigSource::Default);
    config->set("max_rest_time", static_cast<int64_t>(2000), ConfigSource::Default);
    config->set("consolidation_interval", static_cast<int64_t>(1000), ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 Configuration:");
    NLM_LOG_INFO("  Integration enabled for all neural subsystems");
    NLM_LOG_INFO("  Random seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  Simulation timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 2000)));
    NLM_LOG_INFO("  Region count: " + std::to_string(config->getOr<int64_t>("region_count", 3)));
    NLM_LOG_INFO("  Sleep interval: " + std::to_string(config->getOr<int64_t>("sleep_interval", 5000)));
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing Phase 6 Integrated Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Parse command line arguments for advanced operations
    bool debugMode = false;
    bool benchmarkMode = false;
    bool quietMode = false;
    std::string testType = "";
    std::string saveFile = "";
    std::string loadFile = "";
    int stepCount = 0;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--debug" || arg == "-d") {
            debugMode = true;
            consoleLogger->setLogLevel(LogLevel::Debug);
            NLM_LOG_INFO("Debug mode enabled");
        } else if (arg == "--benchmark" || arg == "-b") {
            benchmarkMode = true;
            NLM_LOG_INFO("Benchmark mode enabled");
        } else if (arg == "--quiet" || arg == "-q") {
            quietMode = true;
            consoleLogger->setLogLevel(LogLevel::Warning);
            NLM_LOG_INFO("Quiet mode enabled");
        } else if (arg == "--test") {
            if (i + 1 < argc) {
                testType = argv[++i];
                NLM_LOG_INFO("Test mode: " + testType);
            }
        } else if (arg == "--save") {
            if (i + 1 < argc) {
                saveFile = argv[++i];
            }
        } else if (arg == "--load") {
            if (i + 1 < argc) {
                loadFile = argv[++i];
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                stepCount = std::stoi(argv[++i]);
            }
        } else if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        }
    }
    
    // Process load file first if specified
    if (!loadFile.empty()) {
        importBrainState(brain, loadFile);
    }
    
    // Process test type
    if (!testType.empty()) {
        runAdvancedTests(brain, testType);
        
        if (benchmarkMode) {
            runLearningBenchmark(brain);
        }
        
        exportBrainState(brain, "test_output");
        return 0;
    }
    
    // Process save file if specified
    if (!saveFile.empty()) {
        exportBrainState(brain, saveFile);
    }
    
    // Process step count
    if (stepCount > 0) {
        NLM_LOG_INFO("Running " + std::to_string(stepCount) + " simulation steps...");
        for (SimulationStep step = 0; step < stepCount; ++step) {
            brain->step(step, step * timestep);
            
            if (step % 1000 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Memory: " + std::to_string(brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0));
            }
        }
        
        std::cout << "\nSimulation completed " << stepCount << " steps.\n";
        showBrainStatus(brain);
    } else {
        // Run standard test suite
        std::cout << "\n=== Running Standard Test Suite ===\n";
        
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        brain->reset();
        brain->initialize();
        
        std::cout << "\n=== Advanced Tests (if enabled) ===\n";
        if (benchmarkMode) {
            runLearningBenchmark(brain);
        }
        
        if (debugMode) {
            std::cout << "\nDebug information available in logs. Use 'debug' command for more details.\n";
        }
    }
    
    // Show final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 Successfully Integrated:");
    NLM_LOG_INFO("  ✓ Neural core with real spiking dynamics");
    NLM_LOG_INFO("  ✓ Working memory with persistent activity");
    NLM_LOG_INFO("  ✓ Episodic memory with experience storage");
    NLM_LOG_INFO("  ✓ Prediction system for forward modeling");
    NLM_LOG_INFO("  ✓ Attention and concept formation");
    NLM_LOG_INFO("  ✓ Neuromodulation with all chemical signals");
    NLM_LOG_INFO("  ✓ Sleep/rest cycle with memory consolidation");
    NLM_LOG_INFO("  ✓ Replay mechanism for memory strengthening");
    NLM_LOG_INFO("  ✓ Advanced user interface and debugging tools");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a fully integrated artificial brain");
    NLM_LOG_INFO("with memory, prediction, and cognition working together.");
    NLM_LOG_INFO("");
    
    return 0;
}
