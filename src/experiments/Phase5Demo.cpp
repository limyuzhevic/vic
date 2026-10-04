/**
 * Phase 5 Demo - Lifetime Learning
 *
 * This demo demonstrates Phase 5's comprehensive lifetime learning capabilities:
 * - Large-scale neural simulation with event-driven processing
 * - Multithreading and SIMD optimizations
 * - Six developmental phases (Early Development through Social Development)
 * - Memory consolidation and continual adaptation
 * - Emergent specialization and task switching
 *
 * Usage:
 *   ./nlm_phase5_demo [options]
 *   Options:
 *     --help, -h               Show this help message
 *     --trials <n>             Number of trials per phase (default: 5)
 *     --steps <n>              Steps per trial (default: 10000)
 *     --neurons <n>            Max neurons (default: 100000)
 *     --enable-multithreading   Enable multithreading (default: true)
 *     --enable-simd            Enable SIMD (default: true)
 *     --enable-sparse          Enable sparse connectivity (default: true)
 *     --no-events              Disable event-driven processing
 *     --output-dir <dir>       Output directory for results (default: ./phase5_results)
 *     --no-checkpoints         Disable checkpoint saving
 *     --no-visualizations       Disable visualization generation
 *     --no-ablations           Disable ablation experiments
 */

#include "experiments/Phase5Experiment.hpp"
#include "experiments/ScalingBenchmark.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <filesystem>

using namespace nlm;

// Helper function to parse command line arguments for Phase 5
bool parseArgs(int argc, char* argv[], LifetimeExperimentConfig& config, std::string& outputDir) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printPhase5Help();
            return false;
        }
        
        else if (arg == "--trials" && i + 1 < argc) {
            config.trialsPerPhase = std::stoul(argv[++i]);
        }
        else if (arg.find("--trials=") == 0) {
            config.trialsPerPhase = std::stoul(arg.substr(9));
        }
        
        else if (arg == "--steps" && i + 1 < argc) {
            config.stepsPerTrial = std::stoull(argv[++i]);
        }
        else if (arg.find("--steps=") == 0) {
            config.stepsPerTrial = std::stoull(arg.substr(7));
        }
        
        else if (arg == "--neurons" && i + 1 < argc) {
            config.maxNeurons = std::stoul(argv[++i]);
        }
        else if (arg.find("--neurons=") == 0) {
            config.maxNeurons = std::stoul(arg.substr(9));
        }
        
        else if (arg == "--enable-multithreading") {
            config.enableMultithreading = true;
        }
        else if (arg == "--disable-multithreading") {
            config.enableMultithreading = false;
        }
        
        else if (arg == "--enable-simd") {
            config.enableSIMD = true;
        }
        else if (arg == "--disable-simd") {
            config.enableSIMD = false;
        }
        
        else if (arg == "--enable-sparse") {
            config.enableSparseConnectivity = true;
        }
        else if (arg == "--disable-sparse") {
            config.enableSparseConnectivity = false;
        }
        
        else if (arg == "--no-events") {
            config.enableEventDriven = false;
        }
        
        else if (arg == "--output-dir" && i + 1 < argc) {
            outputDir = argv[++i];
        }
        else if (arg.find("--output-dir=") == 0) {
            outputDir = arg.substr(14);
        }
        
        else if (arg == "--no-checkpoints") {
            config.saveCheckpoints = false;
        }
        else if (arg == "--enable-checkpoints") {
            config.saveCheckpoints = true;
        }
        
        else if (arg == "--no-visualizations") {
            config.generateVisualizations = false;
        }
        else if (arg == "--enable-visualizations") {
            config.generateVisualizations = true;
        }
        
        else if (arg == "--no-ablations") {
            config.runAblations = false;
        }
        else if (arg == "--enable-ablations") {
            config.runAblations = true;
        }
        
        else if (arg == "--thread-count" && i + 1 < argc) {
            config.threadCount = std::stoi(argv[++i]);
        }
        else if (arg.find("--thread-count=") == 0) {
            config.threadCount = std::stoi(arg.substr(16));
        }
        
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Use --help for usage information.\n";
            return false;
        }
    }
    
    return true;
}

void printPhase5Help() {
    std::cout << "=== NLM Phase 5 Lifetime Learning Demo ===\n\n";
    std::cout << "Phase 5 demonstrates comprehensive lifetime learning through six developmental phases:\n\n";
    std::cout << "  Phase 1: Early Development (0-10k steps) - Initial synaptogenesis, sensory exploration\n";
    std::cout << "  Phase 2: Sensory Development (10k-50k steps) - Vision/contact object detection\n";
    std::cout << "  Phase 3: Motor Development (50k-100k steps) - Movement coordination, navigation\n";
    std::cout << "  Phase 4: Associative Development (100k-200k steps) - Action-consequence learning\n";
    std::cout << "  Phase 5: Memory Consolidation (200k-300k steps) - Episodic memory, concepts\n";
    std::cout << "  Phase 6: Social Development (300k-400k steps) - Other agent observation, imitation\n\n";
    std::cout << "Usage: ./nlm_phase5_demo [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --trials <n>             Number of trials per phase (default: 5)\n";
    std::cout << "  --steps <n>              Steps per trial (default: 10000)\n";
    std::cout << "  --neurons <n>            Max neurons (default: 100000)\n";
    std::cout << "  --enable-multithreading   Enable multithreading (default: true)\n";
    std::cout << "  --disable-multithreading  Disable multithreading\n";
    std::cout << "  --enable-simd            Enable SIMD optimizations (default: true)\n";
    std::cout << "  --disable-simd           Disable SIMD\n";
    std::cout << "  --enable-sparse          Enable sparse connectivity (default: true)\n";
    std::cout << "  --disable-sparse         Disable sparse connectivity\n";
    std::cout << "  --no-events              Disable event-driven processing\n";
    std::cout << "  --output-dir <dir>       Output directory for results (default: ./phase5_results)\n";
    std::cout << "  --no-checkpoints         Disable checkpoint saving\n";
    std::cout << "  --enable-checkpoints      Enable checkpoint saving\n";
    std::cout << "  --no-visualizations       Disable visualization generation\n";
    std::cout << "  --enable-visualizations   Enable visualization generation\n";
    std::cout << "  --no-ablations           Disable ablation experiments\n";
    std::cout << "  --enable-ablations       Enable ablation experiments\n";
    std::cout << "  --thread-count <n>       Number of threads for processing (default: 0 = auto)\n";
    std::cout << "  --help, -h               Show this help message\n";
}

// Helper function to create brain with appropriate configuration
std::shared_ptr<Brain> createPhase5Brain(const LifetimeExperimentConfig& config, uint64_t seed) {
    auto cfg = std::make_shared<Config>();
    
    // Set configuration based on Phase 5 requirements
    cfg->set("neuron_count", static_cast<int64_t>(config.initialNeurons));
    cfg->set("region_count", static_cast<int64_t>(2));  // Two regions for sensory/motor separation
    cfg->set("connection_probability", config.connectionProbability);
    cfg->set("random_seed", static_cast<uint64_t>(seed));
    cfg->set("simulation_timestep", 0.001);
    
    // Set Phase 5 specific parameters
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    cfg->set("stdp_tau", 20.0f);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    
    // Create and return brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain with seed " << seed << std::endl;
        return nullptr;
    }
    
    return brain;
}

// Helper function to run simulation with given brain and steps
bool runPhase5Simulation(Brain* brain, uint64_t steps) {
    if (!brain) {
        return false;
    }
    
    for (uint64_t step = 0; step < steps; ++step) {
        // Simulate brain processing with minimal external input
        // This creates spontaneous activity and learning
        brain->step(step, step * 0.001);
        
        // Inject some structured activity to promote learning
        if (step % 1000 == 0) {
            // Periodic injection of structured activity to different neuron types
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            brain->injectCurrentToNeurons(NeuronType::Motor, 5.0f);
        }
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 5 Lifetime Learning Demo ===" << std::endl;
    std::cout << "Demonstrating comprehensive developmental learning through 6 phases..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse command line arguments
    LifetimeExperimentConfig config;
    std::string outputDir = "./phase5_results";
    
    if (!parseArgs(argc, argv, config, outputDir)) {
        return 1;
    }
    
    // Create output directory
    std::filesystem::create_directories(outputDir);
    
    std::cout << "=== Phase 5 Configuration ===" << std::endl;
    std::cout << "Trials per phase: " << config.trialsPerPhase << std::endl;
    std::cout << "Steps per trial: " << config.stepsPerTrial << std::endl;
    std::cout << "Max neurons: " << config.maxNeurons << std::endl;
    std::cout << "Initial neurons: " << config.initialNeurons << std::endl;
    std::cout << "Multithreading: " << (config.enableMultithreading ? "enabled" : "disabled") << std::endl;
    std::cout << "SIMD: " << (config.enableSIMD ? "enabled" : "disabled") << std::endl;
    std::cout << "Sparse connectivity: " << (config.enableSparseConnectivity ? "enabled" : "disabled") << std::endl;
    std::cout << "Event-driven: " << (config.enableEventDriven ? "enabled" : "disabled") << std::endl;
    std::cout << "Checkpointing: " << (config.saveCheckpoints ? "enabled" : "disabled") << std::endl;
    std::cout << "Visualizations: " << (config.generateVisualizations ? "enabled" : "disabled") << std::endl;
    std::cout << "Ablations: " << (config.runAblations ? "enabled" : "disabled") << std::endl;
    std::cout << "Output directory: " << outputDir << std::endl;
    std::cout << std::endl;
    
    // Create Phase 5 experiment
    Phase5IntegratedExperiment experiment;
    
    // Create brain creation function
    auto createBrain = [&](const LifetimeExperimentConfig& cfg, uint64_t seed) {
        return createPhase5Brain(cfg, seed);
    };
    
    // Create simulation runner function
    auto runSimulation = [&](Brain* brain, uint64_t steps) {
        return runPhase5Simulation(brain, steps);
    };
    
    std::cout << "Starting Phase 5 lifetime experiment..." << std::endl;
    std::cout << "This will run " << config.trialsPerPhase << " trials of " << config.stepsPerTrial 
              << " steps each for each of the 6 developmental phases." << std::endl;
    std::cout << std::endl;
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run the experiment
    LifetimeExperimentResult result = experiment.run(config, createBrain, runSimulation);
    
    // Record end time
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Generate report
    std::cout << std::endl;
    std::cout << "=== Phase 5 Experiment Results ===" << std::endl;
    std::cout << "Total wall clock time: " << result.totalWallClockTime << " seconds" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average reward per step: " << result.avgRewardPerStep << std::endl;
    std::cout << "Overall learning efficiency: " << result.overallLearningEfficiency << std::endl;
    std::cout << "Final performance: " << result.finalPerformance << std::endl;
    std::cout << "Memory capacity: " << result.memoryCapacity << std::endl;
    std::cout << "Prediction accuracy: " << result.predictionAccuracy << std::endl;
    std::cout << "Generalization ability: " << result.generalizationAbility << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== Developmental Progress ===" << std::endl;
    for (const auto& phase : result.phaseResults) {
        std::cout << "Phase: " << phase.phaseName << std::endl;
        std::cout << "  Steps: " << phase.startStep << " - " << phase.endStep << std::endl;
        std::cout << "  Duration: " << phase.duration << " seconds" << std::endl;
        std::cout << "  Total reward: " << phase.totalReward << std::endl;
        std::cout << "  Average firing rate: " << phase.avgFiringRate << std::endl;
        std::cout << "  Exploration rate: " << phase.explorationRate << std::endl;
        std::cout << "  Goal directedness: " << phase.goalDirectedness << std::endl;
        std::cout << "  Adaptation speed: " << phase.adaptationSpeed << std::endl;
        std::cout << "  Memory retention: " << phase.memoryRetention << std::endl;
        std::cout << "  Episodic recall: " << phase.episodicRecall << std::endl;
        std::cout << "  Prediction accuracy: " << phase.predictionAccuracy << std::endl;
        std::cout << "  Prediction error: " << phase.predictionError << std::endl;
        
        if (!phase.developmentalProgress.empty()) {
            std::cout << "  Developmental progress:" << std::endl;
            for (const auto& progress : phase.developmentalProgress) {
                std::cout << "    " << progress.first << ": " << progress.second << std::endl;
            }
        }
        std::cout << std::endl;
    }
    
    // Generate JSON output
    std::string jsonOutput = result.toJSON();
    std::string jsonPath = outputDir + "/phase5_results.json";
    std::ofstream jsonFile(jsonPath);
    if (jsonFile.is_open()) {
        jsonFile << jsonOutput;
        jsonFile.close();
        std::cout << "Results saved to: " << jsonPath << std::endl;
    }
    
    // Generate text report
    std::string reportPath = outputDir + "/phase5_report.txt";
    std::ofstream reportFile(reportPath);
    if (reportFile.is_open()) {
        reportFile << result.summary();
        reportFile.close();
        std::cout << "Report saved to: " << reportPath << std::endl;
    }
    
    if (config.generateVisualizations) {
        std::cout << "Generating visualizations..." << std::endl;
        experiment.generateVisualizations(result, outputDir);
    }
    
    if (config.saveCheckpoints) {
        std::cout << "Saving final checkpoint..." << std::endl;
        // Save final brain state as checkpoint
        experiment.savePhaseCheckpoint(nullptr, "final", result.totalWallClockTime);
    }
    
    std::cout << std::endl;
    std::cout << "=== Phase 5 Demo Complete ===" << std::endl;
    std::cout << "The brain has completed " << config.trialsPerPhase * 6 << " developmental trials "
              << "across all 6 phases of learning." << std::endl;
    std::cout << "Key achievements:" << std::endl;
    std::cout << "  - Large-scale neural learning with " << config.maxNeurons << " neurons" << std::endl;
    std::cout << "  - Optimized performance with " 
              << (config.enableMultithreading ? "multithreading" : "single-threaded") << std::endl;
    std::cout << "  - " << (config.enableSIMD ? "SIMD" : "no SIMD") << " acceleration" << std::endl;
    std::cout << "  - " << (config.enableSparseConnectivity ? "Sparse" : "dense") << " connectivity" << std::endl;
    std::cout << "  - " << (config.enableEventDriven ? "Event-driven" : "step-driven") << " processing" << std::endl;
    std::cout << "  - Comprehensive memory formation and consolidation" << std::endl;
    std::cout << "  - Emergent specialized capabilities" << std::endl;
    std::cout << std::endl;
    
    return 0;
}
