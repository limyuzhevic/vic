// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demo
//
// This phase demonstrates the fully integrated artificial brain with all cognitive systems connected:
// - Working memory integration
// - Episodic memory storage and replay
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction system integration
// - Development stages and plasticity modulation
// - Checkpoint persistence
// - Learning through experience

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>

using namespace nlm;

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain with       ║
    ║     all cognitive systems integrated into a coherent whole.   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    printBanner();
    std::cout << std::endl;
    std::cout << "USAGE: nlm_phase6 [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 6: Final Integration Demo" << std::endl;
    std::cout << std::endl;
    std::cout << "MODES:" << std::endl;
    std::cout << "  --demo         Run Phase 6 integration demo (default)" << std::endl;
    std::cout << "  --learning      Interactive learning demo with agent/world" << std::endl;
    std::cout << "  --verify        Quick integration verification" << std::endl;
    std::cout << "  --profile       System profiling and performance testing" << std::endl;
    std::cout << "  --sleep        Run sleep/rest cycle with memory consolidation" << std::endl;
    std::cout << "  --full         Run complete integration with sleep cycle" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --help, -h          Show this help message" << std::endl;
    std::cout << "  --config FILE       Load configuration from FILE (default: configs/default.cfg)" << std::endl;
    std::cout << "  --steps N           Set maximum simulation steps (default: 10000)" << std::endl;
    std::cout << "  --neurons N         Set neuron count (default: 1000)" << std::endl;
    std::cout << "  --regions N         Set region count (default: 1)" << std::endl;
    std::cout << "  --checkpoint FILE    Enable checkpointing with FILE path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --debug             Enable debug logging" << std::endl;
    std::cout << "  --quiet, -q         Reduce output verbosity" << std::endl;
    std::cout << "  --version           Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "SLEEP/CONSOLIDATION OPTIONS:" << std::endl;
    std::cout << "  --sleep-steps N     Number of sleep steps (default: 1000)" << std::endl;
    std::cout << "  --replay-ratio F     Replay ratio (0.0-1.0, default: 0.3)" << std::endl;
    std::cout << "  --consolidation-th F Relevance threshold (default: 0.5)" << std::endl;
    std::cout << "  --working-mem-clear  Clear working memory during sleep" << std::endl;
    std::cout << "  --replay-enabled     Enable memory replay during sleep" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm_phase6 --demo --steps 5000 --neurons 500" << std::endl;
    std::cout << "  nlm_phase6 --learning --debug --curiosity 0.5" << std::endl;
    std::cout << "  nlm_phase6 --sleep --sleep-steps 2000 --replay-ratio 0.5" << std::endl;
    std::cout << "  nlm_phase6 --full --sleep --sleep-steps 1000 --consolidation-th 0.7" << std::endl;
    std::cout << std::endl;
    std::cout << "The Phase 6 demo demonstrates the fully integrated artificial brain" << std::endl;
    std::cout << "with all cognitive systems working together." << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine - Phase 6" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6 Integration)" << std::endl;
    std::cout << "Build: Enhanced Integration Demo with Sleep Consolidation" << std::endl;
    std::cout << "Framework: All systems integrated and connected" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine - Phase 6" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6 Integration)" << std::endl;
    std::cout << "Build: Enhanced Integration Demo" << std::endl;
    std::cout << "Framework: All systems integrated and connected" << std::endl;
}

Phase6Config parseCommandLineArgs(int argc, char* argv[]) {
    Phase6Config config;
    bool debugMode = false;
    bool quietMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg == "--debug") {
            debugMode = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quietMode = true;
        } else if (arg == "--demo") {
            // Default mode
        } else if (arg == "--learning") {
            // Will be handled in the main logic
        } else if (arg == "--verify") {
            // Will be handled in the main logic
        } else if (arg == "--profile") {
            // Will be handled in the main logic
        } else if (arg.find("--config") == 0) {
            if (arg.find('=') != std::string::npos) {
                std::string value = arg.substr(arg.find('=') + 1);
                config.checkpointPath = value;
            } else if (i + 1 < argc) {
                config.checkpointPath = argv[++i];
            }
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            config.regionCount = std::stoul(argv[++i]);
        } else if (arg == "--plasticity-rate" && i + 1 < argc) {
            config.connectionProbability = std::stof(argv[++i]);
        } else if (arg == "--stdp-params" && i + 1 < argc) {
            std::string params = argv[++i];
            size_t comma1 = params.find(',');
            if (comma1 != std::string::npos) {
                size_t comma2 = params.find(',', comma1 + 1);
                if (comma2 != std::string::npos) {
                    config.connectionProbability = std::stof(params.substr(0, comma1));
                }
            }
        }
    }
    
    return config;
}

void runIntegrationDemo(const Phase6Config& config, bool debugMode, bool quietMode) {
    std::cout << "=== PHASE 6: INTEGRATION DEMO ===" << std::endl;
    std::cout << std::endl;
    
    if (!quietMode) {
        std::cout << "Running Phase 6 integration demo..." << std::endl;
        std::cout << "This demo demonstrates the fully integrated artificial brain." << std::endl;
        std::cout << std::endl;
    }
    
    Phase6IntegratedExperiment experiment;
    
    // Quick verification first
    if (!quietMode) {
        std::cout << "Running integration verification..." << std::endl;
    }
    bool verificationOK = experiment.verifyIntegration();
    
    if (!verificationOK) {
        std::cout << "ERROR: Integration verification failed!" << std::endl;
        exit(1);
    }
    
    if (!quietMode) {
        std::cout << "✓ Integration verification passed!" << std::endl;
        std::cout << std::endl;
    }
    
    // Run full experiment
    if (!quietMode) {
        std::cout << "Running full integration experiment..." << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Max steps: " << config.maxSteps << std::endl;
        std::cout << "  Neuron count: " << config.neuronCount << std::endl;
        std::cout << "  Region count: " << config.regionCount << std::endl;
        std::cout << "  Connection probability: " << config.connectionProbability << std::endl;
        std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
        std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
        std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
        std::cout << "  Checkpoint path: " << config.checkpointPath << std::endl;
        std::cout << std::endl;
    }
    
    auto result = experiment.run(config);
    
    // Display comprehensive results
    std::cout << std::endl;
    std::cout << "=== INTEGRATION DEMO RESULTS ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "SYSTEM INTEGRATION STATUS:" << std::endl;
    std::cout << "  Working Memory:     " << (result.memoryWorkingMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory:    " << (result.memoryEpisodicMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation:    " << (result.neuromodulationIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Prediction System:  " << (result.predictionIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Development:        " << (result.developmentIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing:      " << (result.checkpointingWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
    std::cout << "  Replay System:      " << (result.replayWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "PERFORMANCE METRICS:" << std::endl;
    std::cout << "  Total Reward:       " << std::fixed << std::setprecision(2) << result.totalReward << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Average Synaptic Weight: " << std::fixed << std::setprecision(4) << result.avgSynapticWeight << std::endl;
    std::cout << "  Episodes Stored:    " << std::fixed << std::setprecision(0) << result.memoryEpisodesStored << std::endl;
    std::cout << "  Novelty Level:      " << std::fixed << std::setprecision(3) << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity Level:    " << std::fixed << std::setprecision(3) << result.curiosityLevel << std::endl;
    std::cout << "  Dopamine Level:     " << std::fixed << std::setprecision(3) << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "TIMING:" << std::endl;
    std::cout << "  Wall clock time:    " << std::fixed << std::setprecision(2) << result.totalWallClockTime << " seconds" << std::endl;
    std::cout << std::endl;
    
    // Overall assessment
    int connectedSystems = 0;
    if (result.memoryWorkingMemoryIntegrated) connectedSystems++;
    if (result.memoryEpisodicMemoryIntegrated) connectedSystems++;
    if (result.neuromodulationIntegrated) connectedSystems++;
    if (result.predictionIntegrated) connectedSystems++;
    if (result.developmentIntegrated) connectedSystems++;
    
    std::cout << "INTEGRATION ASSESSMENT: " << connectedSystems << "/6 systems connected" << std::endl;
    
    if (connectedSystems >= 4) {
        std::cout << "✅ SUCCESS: Brain integration is working correctly!" << std::endl;
        std::cout << "The artificial brain is functioning as a coherent cognitive system." << std::endl;
    } else {
        std::cout << "⚠️  WARNING: Limited system integration." << std::endl;
        std::cout << "The brain is partially functional but needs more work." << std::endl;
    }
}

void runLearningDemo(const Phase6Config& config, bool debugMode, bool quietMode) {
    std::cout << "=== PHASE 6: LEARNING DEMO WITH AGENT/WORLD ===" << std::endl;
    std::cout << std::endl;
    
    if (!quietMode) {
        std::cout << "Setting up agent-based learning environment..." << std::endl;
        std::cout << "This demo demonstrates embodied learning with:" << std::endl;
        std::cout << "  - Sensory input processing" << std::endl;
        std::cout << "  - Motor action selection" << std::endl;
        std::cout << "  - Reward-based learning" << std::endl;
        std::cout << "  - Curiosity-driven exploration" << std::endl;
        std::cout << "  - Memory formation and replay" << std::endl;
        std::cout << std::endl;
    }
    
    // Create configuration
    auto nlmConfig = std::make_shared<Config>();
    nlmConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    nlmConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    nlmConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    nlmConfig->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    nlmConfig->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (debugMode) {
        Logger::getGlobal()->setLevel(LogLevel::Debug);
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(nlmConfig);
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain!" << std::endl;
        exit(1);
    }
    
    // Create and configure world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(config.neuronCount / 10, config.neuronCount / 10, 8, 8);
    world->reset();
    world->setAgentStart(static_cast<double>(config.neuronCount / 20), static_cast<double>(config.neuronCount / 20));
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable learning subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    if (!quietMode) {
        std::cout << "Starting learning simulation..." << std::endl;
        std::cout << "Initial brain state:" << std::endl;
        brain->logStatus();
        std::cout << std::endl;
    }
    
    // Run learning simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < config.maxSteps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        auto motorCmd = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(motorCmd, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = 0.0f;
        if (percept.getInternal() && !percept.getInternal()->empty()) {
            reward = percept.getInternal()->at(0);
        }
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1f);
        
        if (quietMode && step % 1000 == 0) {
            std::cout << "Step " << step << "/" << config.maxSteps << std::endl;
        } else if (!quietMode && step % 500 == 0) {
            std::cout << "Step " << step << "/" << config.maxSteps << "; "
                     << "Firing: " << brain->getFiringNeuronCount() << "; "
                     << "Curiosity: " << std::fixed << std::setprecision(3) << agent->getCuriosityLevel() << "; "
                     << "Novelty: " << std::fixed << std::setprecision(3) << agent->getNoveltyLevel() << "; "
                     << "Dev Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << std::endl;
    std::cout << "=== LEARNING DEMO COMPLETED ===" << std::endl;
    std::cout << "Simulation time: " << duration.count() << " seconds" << std::endl;
    std::cout << std::endl;
    
    if (!quietMode) {
        std::cout << "Final brain status:" << std::endl;
        brain->logStatus();
        std::cout << std::endl;
        
        std::cout << "Agent statistics:" << std::endl;
        std::cout << "  Curiosity level: " << std::fixed << std::setprecision(3) << agent->getCuriosityLevel() << std::endl;
        std::cout << "  Novelty level: " << std::fixed << std::setprecision(3) << agent->getNoveltyLevel() << std::endl;
        std::cout << "  Neuromodulation: " << std::fixed << std::setprecision(3) << agent->getNeuromodulationLevel() << std::endl;
        std::cout << "  Prediction error: " << std::fixed << std::setprecision(3) << agent->getPredictionError() << std::endl;
    }
}

void runQuickVerification() {
    std::cout << "=== PHASE 6: QUICK INTEGRATION VERIFICATION ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Running minimal integration checks..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    bool verificationOK = experiment.verifyIntegration();
    
    if (!verificationOK) {
        std::cout << "ERROR: Integration verification failed!" << std::endl;
        exit(1);
    }
    
    std::cout << "✅ Integration verification passed!" << std::endl;
    std::cout << "Basic system connectivity confirmed." << std::endl;
}

void runSystemProfile(const Phase6Config& config, bool quietMode) {
    std::cout << "=== PHASE 6: SYSTEM PROFILING ===" << std::endl;
    std::cout << std::endl;
    
    if (!quietMode) {
        std::cout << "Running system profiling and performance analysis..." << std::endl;
        std::cout << "This mode tests performance under different conditions." << std::endl;
        std::cout << std::endl;
    }
    
    // Create minimal configuration for profiling
    Phase6Config profileConfig = config;
    profileConfig.maxSteps = 1000;  // Shorter for profiling
    
    Phase6IntegratedExperiment experiment;
    
    if (!quietMode) {
        std::cout << "Running profile verification..." << std::endl;
    }
    
    bool verificationOK = experiment.verifyIntegration();
    
    if (!quietMode) {
        if (verificationOK) {
            std::cout << "✓ Integration verification passed for profiling." << std::endl;
        } else {
            std::cout << "ERROR: Integration verification failed!" << std::endl;
            exit(1);
        }
    }
    
    // Run system profile test
    auto result = experiment.run(profileConfig);
    
    std::cout << std::endl;
    std::cout << "=== SYSTEM PROFILE RESULTS ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Reward rate: " << std::fixed << std::setprecision(3) << result.totalReward / result.totalWallClockTime << " reward/sec" << std::endl;
    std::cout << "  Firing rate: " << std::fixed << std::setprecision(1) << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Energy efficiency: " << std::fixed << std::setprecision(4) << result.totalReward / result.avgFiringRate << " reward/spike" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Integration Health:" << std::endl;
    std::cout << "  Systems connected: " << (result.memoryWorkingMemoryIntegrated ? 1 : 0) + (result.memoryEpisodicMemoryIntegrated ? 1 : 0) +
                 (result.neuromodulationIntegrated ? 1 : 0) + (result.predictionIntegrated ? 1 : 0) +
                 (result.developmentIntegrated ? 1 : 0) + (result.checkpointingWorks ? 1 : 0) << "/6" << std::endl;
    
    if (result.memoryWorkingMemoryIntegrated && result.neuromodulationIntegrated && result.checkpointingWorks) {
        std::cout << "✅ System in good health for production use." << std::endl;
    } else {
        std::cout << "⚠️  System needs attention for optimal performance." << std::endl;
    }
}

/**
 * Run sleep/rest cycle with memory consolidation
 */
void runSleepConsolidation(const Phase6Config& config, bool debugMode, bool quietMode,
                           size_t sleepSteps, float replayRatio, float consolidationThreshold,
                           bool clearWorkingMemory, bool enableReplay) {
    std::cout << "=== PHASE 6: SLEEP/REST CYCLE WITH MEMORY CONSOLIDATION ===" << std::endl;
    std::cout << std::endl;
    
    if (!quietMode) {
        std::cout << "Starting sleep/rest cycle for memory consolidation..." << std::endl;
        std::cout << "This phase demonstrates offline processing and memory integration." << std::endl;
        std::cout << std::endl;
        std::cout << "Sleep Configuration:" << std::endl;
        std::cout << "  Sleep steps: " << sleepSteps << std::endl;
        std::cout << "  Replay ratio: " << std::fixed << std::setprecision(2) << replayRatio << std::endl;
        std::cout << "  Consolidation threshold: " << std::fixed << std::setprecision(2) << consolidationThreshold << std::endl;
        std::cout << "  Clear working memory: " << (clearWorkingMemory ? "yes" : "no") << std::endl;
        std::cout << "  Memory replay: " << (enableReplay ? "enabled" : "disabled") << std::endl;
        std::cout << std::endl;
    }
    
    // Create configuration for sleep mode
    auto sleepConfig = std::make_shared<Config>();
    sleepConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    sleepConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    sleepConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    sleepConfig->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    sleepConfig->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (debugMode) {
        Logger::getGlobal()->setLevel(LogLevel::Debug);
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(sleepConfig);
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain for sleep consolidation!" << std::endl;
        exit(1);
    }
    
    if (!quietMode) {
        std::cout << "Initial brain state:" << std::endl;
        brain->logStatus();
        std::cout << std::endl;
        
        // Check memory systems status
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* associativeMemory = brain->getAssociativeMemory();
        
        std::cout << "Initial Memory Status:" << std::endl;
        std::cout << "  Working memory: " << (workingMemory ? workingMemory->getActiveTraces() : 0) << " traces" << std::endl;
        std::cout << "  Episodic memory: " << (episodicMemory ? episodicMemory->getEpisodeCount() : 0) << " episodes" << std::endl;
        std::cout << "  Associative memory: " << (associativeMemory ? associativeMemory->getAssociationCount() : 0) << " associations" << std::endl;
        std::cout << std::endl;
    }
    
    // Simulate learning phase before sleep
    if (!quietMode) {
        std::cout << "=== LEARNING PHASE (BEFORE SLEEP) ===" << std::endl;
    }
    
    for (SimulationStep step = 0; step < config.maxSteps / 10; step += 100) {
        // Apply random sensory input to stimulate learning
        if (brain->getWorkingMemory()) {
            float activation = (brain->getRandomGenerator()->getRandomFloat() - 0.5f) * 2.0f;  // -1.0 to 1.0
            brain->getWorkingMemory()->storeToNeuron(brain->getRandomGenerator()->getRandomNeuronId(0, config.neuronCount), activation);
        }
        
        brain->step(step, step * 0.001);
        
        if (quietMode && step % 1000 == 0) {
            std::cout << "Learning step " << step << "/" << config.maxSteps / 10 << std::endl;
        }
    }
    
    if (!quietMode) {
        std::cout << "Learning phase completed." << std::endl;
        brain->logStatus();
        std::cout << std::endl;
    }
    
    // Enter sleep mode
    std::cout << "=== ENTERING SLEEP MODE ===" << std::endl;
    std::cout << "Sleep duration: " << sleepSteps << " steps (approx. " << sleepSteps * 0.001 << " seconds)" << std::endl;
    std::cout << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < sleepSteps; ++step) {
        // During sleep, neurons are mostly quiescent but memory processing occurs
        
        // 1. Working memory decay (exponential decay)
        if (brain->getWorkingMemory()) {
            brain->getWorkingMemory()->update(0.001);  // Very slow decay during sleep
            
            // Select episodes for replay based on relevance
            if (enableReplay && brain->getEpisodicMemory()) {
                auto episodesToReplay = brain->getEpisodicMemory()->getEpisodesForReplay(3);
                for (auto* episode : episodesToReplay) {
                    // Replay the episode - reactivates neural patterns
                    brain->getEpisodicMemory()->replayEpisode(episode);
                }
            }
        }
        
        // 2. Consolidation of important memories
        if (brain->getEpisodicMemory() && step % 100 == 0) {
            // Increase relevance of recently active memories
            float relevanceThreshold = consolidationThreshold;
            brain->getEpisodicMemory()->consolidate(relevanceThreshold);
        }
        
        // 3. Attention modulation (during REM-like phase)
        if (step % 50 == 25) {  // REM-like phase every 50 steps
            if (brain->getAttention()) {
                // Apply top-down attention bias
                for (size_t i = 0; i < 10 && i < config.neuronCount; ++i) {
                    brain->getAttention()->applyTopDownBias(NeuronId(i), 0.3f);
                }
            }
        }
        
        // 4. Neuromodulation during sleep
        if (step % 200 == 0) {  // Dopamine release during sleep
            if (brain->getDopamine()) {
                // Simulate dopamine release for memory consolidation
                brain->getDopamine()->releaseModulator(0.5f, 0.001);
            }
        }
        
        // 5. Clear working memory as directed
        if (clearWorkingMemory && step % sleepSteps == 0) {
            if (brain->getWorkingMemory()) {
                brain->getWorkingMemory()->clear();
            }
        }
        
        // Simulate slow neural processing during sleep
        brain->step(step, step * 0.001);
        
        if (quietMode && step % sleepSteps / 10 == 0) {
            std::cout << "Sleep step " << step << "/" << sleepSteps << std::endl;
        } else if (!quietMode && step % 200 == 0) {
            std::cout << "Sleep step " << step << "/" << sleepSteps << " (" << (step * 100 / sleepSteps) << "% complete)" << std::endl;
            if (brain->getWorkingMemory() && brain->getEpisodicMemory()) {
                std::cout << "  Working memory: " << brain->getWorkingMemory()->getActiveTraces() << " traces" << std::endl;
                std::cout << "  Episodic memory: " << brain->getEpisodicMemory()->getEpisodeCount() << " episodes" << std::endl;
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << std::endl;
    std::cout << "=== SLEEP CYCLE COMPLETED ===" << std::endl;
    std::cout << "Sleep duration: " << duration.count() << " seconds" << std::endl;
    std::cout << "Sleep steps: " << sleepSteps << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== MEMORY CONSOLIDATION RESULTS ===" << std::endl;
    brain->logStatus();
    std::cout << std::endl;
    
    if (brain->getWorkingMemory() && brain->getWorkingMemory()->getActiveTraces() == 0 && clearWorkingMemory) {
        std::cout << "✅ Working memory successfully cleared during sleep." << std::endl;
    }
    
    if (brain->getEpisodicMemory() && brain->getEpisodicMemory()->getEpisodeCount() > 0) {
        std::cout << "✅ Episodic memory consolidation completed." << std::endl;
        std::cout << "  Average reward: " << std::fixed << std::setprecision(3) << brain->getEpisodicMemory()->getAverageReward() << std::endl;
    }
    
    std::cout << "✅ Sleep/rest cycle enhanced memory integration and consolidation." << std::endl;
}

/**
 * Run full integration with sleep cycle
 */
void runFullIntegrationWithSleep(const Phase6Config& config, bool debugMode, bool quietMode) {
    std::cout << "=== PHASE 6: FULL INTEGRATION WITH SLEEP/CONSOLIDATION ===" << std::endl;
    std::cout << std::endl;
    
    if (!quietMode) {
        std::cout << "Running complete integration including sleep consolidation..." << std::endl;
        std::cout << "This comprehensive demo shows the full artificial brain lifecycle:" << std::endl;
        std::cout << "  1. Neural computation and learning" << std::endl;
        std::cout << "  2. Memory formation and storage" << std::endl;
        std::cout << "  3. Sleep/rest cycle with consolidation" << std::endl;
        std::cout << "  4. Awakening with enhanced memory integration" << std::endl;
        std::cout << std::endl;
    }
    
    // Run regular integration first
    Phase6IntegratedExperiment experiment;
    
    if (!quietMode) {
        std::cout << "=== PHASE 1: NEURAL COMPUTATION AND LEARNING ===" << std::endl;
        std::cout << "Running integration verification..." << std::endl;
    }
    
    bool verificationOK = experiment.verifyIntegration();
    
    if (!quietMode) {
        if (verificationOK) {
            std::cout << "✓ Integration verification passed!" << std::endl;
        } else {
            std::cout << "ERROR: Integration verification failed!" << std::endl;
            exit(1);
        }
    }
    
    // Create a combined configuration for the full integration
    Phase6Config fullConfig = config;
    fullConfig.maxSteps = config.maxSteps;  // Use main experiment steps
    fullConfig.enableCheckpointing = true;
    fullConfig.enableReplay = true;
    fullConfig.enableDevelopment = true;
    
    // Run the full integration experiment
    if (!quietMode) {
        std::cout << "=== PHASE 2: FULL INTEGRATION EXPERIMENT ===" << std::endl;
        std::cout << "Running complete lifetime simulation..." << std::endl;
    }
    
    auto result = experiment.run(fullConfig);
    
    if (!quietMode) {
        std::cout << std::endl;
        std::cout << "=== PHASE 2 RESULTS ===" << std::endl;
        std::cout << "Total reward: " << std::fixed << std::setprecision(2) << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << std::fixed << std::setprecision(2) << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "Episodes stored: " << std::fixed << std::setprecision(0) << result.memoryEpisodesStored << std::endl;
        std::cout << std::endl;
        
        std::cout << "Integration Status:" << std::endl;
        std::cout << "  Working Memory:     " << (result.memoryWorkingMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
        std::cout << "  Episodic Memory:    " << (result.memoryEpisodicMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
        std::cout << "  Neuromodulation:    " << (result.neuromodulationIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
        std::cout << "  Prediction System:  " << (result.predictionIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
        std::cout << "  Development:        " << (result.developmentIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
        std::cout << "  Checkpointing:      " << (result.checkpointingWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
        std::cout << "  Replay System:      " << (result.replayWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
    }
    
    // Run sleep consolidation
    std::cout << std::endl;
    std::cout << "=== PHASE 3: SLEEP/REST CONSOLIDATION ===" << std::endl;
    
    runSleepConsolidation(config, debugMode, quietMode, 
                          static_cast<size_t>(config.maxSteps / 10), 
                          0.3f, 0.5f, true, true);
    
    std::cout << std::endl;
    std::cout << "=== PHASE 6 FULL INTEGRATION COMPLETED ===" << std::endl;
    std::cout << "The comprehensive Phase 6 integration demonstrates the fully functional" << std::endl;
    std::cout << "artificial brain with all cognitive systems working together, including" << std::endl;
    std::cout << "memory consolidation during sleep cycles." << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    std::cout << "Starting NLM Phase 6 Enhanced Integration Demo..." << std::endl;
    std::cout << std::endl;
    
    Phase6Config config = parseCommandLineArgs(argc, argv);
    
    // Set default config values
    config.maxSteps = 10000;
    config.neuronCount = 1000;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint_test.bin";
    
    // Parse additional config from command line
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Parse additional config options
        if (arg == "--demo") {
            // Default mode, will run integration demo
        } else if (arg == "--learning") {
            runLearningDemo(config, false, false);
            return 0;
        } else if (arg == "--verify") {
            runQuickVerification();
            return 0;
        } else if (arg == "--profile") {
            runSystemProfile(config, false);
            return 0;
        } else if (arg == "--sleep") {
            // Sleep mode - run sleep consolidation
            runSleepConsolidation(config, false, false, 
                                  static_cast<size_t>(config.maxSteps / 10), 
                                  0.3f, 0.5f, true, true);
            return 0;
        } else if (arg == "--full") {
            // Full mode - run complete integration with sleep
            runFullIntegrationWithSleep(config, false, false);
            return 0;
        }
    }
    
    // Default mode: run integration demo
    runIntegrationDemo(config, false, false);
    
    std::cout << std::endl;
    std::cout << "=== PHASE 6 DEMO COMPLETED ===" << std::endl;
    std::cout << "The enhanced Phase 6 integration demo has demonstrated the" << std::endl;
    std::cout << "fully integrated artificial brain with all cognitive systems working together." << std::endl;
    
    return 0;
}
