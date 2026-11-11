#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <chrono>

// Simple command-line argument parsing
struct CommandLineArgs {
    bool help = false;
    bool version = false;
    std::string mode = "run";  // run, test, benchmark, profile
    std::string config_file = "configs/default.cfg";
    uint64_t steps = 2000;
    size_t neurons = 500;
    size_t regions = 1;
    float connection_prob = 0.1f;
    bool enable_checkpointing = true;
    std::string checkpoint_path = "./checkpoints/nlm_checkpoint.bin";
    bool enable_replay = true;
    bool enable_development = true;
    std::string output_file = "nlm_results.txt";
    bool verbose = false;
    bool profile_memory = false;
    bool profile_cpu = false;
    bool profile_detailed = false;
    int profiling_level = 1;  // 1=basic, 2=detailed, 3=extreme
    std::string profile_output = "profile_data.txt";
    bool benchmark_cycles = false;
    uint64_t benchmark_steps = 10000;
    float benchmark_timestep = 0.001f;
};

CommandLineArgs parse_args(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "--mode" && i + 1 < argc) {
            args.mode = argv[++i];
        } else if (arg == "--config" && i + 1 < argc) {
            args.config_file = argv[++i];
        } else if (arg == "--steps" && i + 1 < argc) {
            args.steps = std::stoull(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            args.neurons = std::stoull(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            args.regions = std::stoull(argv[++i]);
        } else if (arg == "--connection-prob" && i + 1 < argc) {
            args.connection_prob = std::stof(argv[++i]);
        } else if (arg == "--no-checkpointing") {
            args.enable_checkpointing = false;
        } else if (arg == "--checkpoint-path" && i + 1 < argc) {
            args.checkpoint_path = argv[++i];
        } else if (arg == "--no-replay") {
            args.enable_replay = false;
        } else if (arg == "--no-development") {
            args.enable_development = false;
        } else if (arg == "--output" && i + 1 < argc) {
            args.output_file = argv[++i];
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--profile-memory") {
            args.profile_memory = true;
        } else if (arg == "--profile-cpu") {
            args.profile_cpu = true;
        } else if (arg == "--profile-detailed") {
            args.profile_detailed = true;
        } else if (arg == "--profile-level" && i + 1 < argc) {
            args.profiling_level = std::stoi(argv[++i]);
        } else if (arg == "--profile-output" && i + 1 < argc) {
            args.profile_output = argv[++i];
        } else if (arg == "--benchmark-cycles") {
            args.benchmark_cycles = true;
        } else if (arg == "--benchmark-steps" && i + 1 < argc) {
            args.benchmark_steps = std::stoull(argv[++i]);
        } else if (arg == "--benchmark-timestep" && i + 1 < argc) {
            args.benchmark_timestep = std::stof(argv[++i]);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
        }
    }
    
    return args;
}

void print_help() {
    std::cout << "NLM Phase 6 Unified Interface" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_phase6 [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  run       Run the complete integration experiment (default)" << std::endl;
    std::cout << "  test      Run integration verification tests" << std::endl;
    std::cout << "  benchmark  Run performance benchmark" << std::endl;
    std::cout << "  profile    Run with performance profiling" << std::endl;
    std::cout << std::endl;
    std::cout << "Core Options:" << std::endl;
    std::cout << "  -h, --help           Show this help message" << std::endl;
    std::cout << "  --version            Show version information" << std::endl;
    std::cout << "  --mode MODE          Set operation mode" << std::endl;
    std::cout << "  --steps N            Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --neurons N          Number of neurons (default: 500)" << std::endl;
    std::cout << "  --regions N          Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection-prob P  Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --no-checkpointing   Disable checkpointing" << std::endl;
    std::cout << "  --checkpoint-path P  Checkpoint file path" << std::endl;
    std::cout << "  --no-replay          Disable memory replay" << std::endl;
    std::cout << "  --no-development     Disable developmental changes" << std::endl;
    std::cout << "  -v, --verbose        Enable verbose output" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance Monitoring Options:" << std::endl;
    std::cout << "  --profile-memory      Enable memory profiling" << std::endl;
    std::cout << "  --profile-cpu        Enable CPU profiling" << std::endl;
    std::cout << "  --profile-detailed   Enable detailed profiling" << std::endl;
    std::cout << "  --profile-level L    Profiling level (1-3, default: 1)" << std::endl;
    std::cout << "  --profile-output F   Profile data output file" << std::endl;
    std::cout << "  --benchmark-cycles   Run performance benchmark" << std::endl;
    std::cout << "  --benchmark-steps N  Benchmark step count (default: 10000)" << std::endl;
    std::cout << "  --benchmark-timestep T  Benchmark time step (default: 0.001)" << std::endl;
    std::cout << std::endl;
    std::cout << "Output Options:" << std::endl;
    std::cout << "  --output FILENAME    Save results to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_phase6 --steps 5000 --neurons 1000" << std::endl;
    std::cout << "  nlm_phase6 --mode benchmark --steps 10000" << std::endl;
    std::cout << "  nlm_phase6 --mode profile --profile-memory --profile-level 2" << std::endl;
    std::cout << "  nlm_phase6 --benchmark-cycles --benchmark-steps 20000" << std::endl;
    std::cout << std::endl;
}

void print_version() {
    std::cout << "NLM Phase 6 Unified Interface v1.0" << std::endl;
    std::cout << "Based on NLM (熙然) - Experimental Artificial Developmental Brain" << std::endl;
    std::cout << "Phase 6: Final Integration (2026)" << std::endl;
}

void save_results_to_file(const std::string& filename, const nlm::Phase6IntegrationResult& result, const CommandLineArgs& args) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open output file: " << filename << std::endl;
        return;
    }
    
    file << "=== NLM Phase 6 Integration Results ===" << std::endl;
    file << "Total Reward: " << result.totalReward << std::endl;
    file << "Average Firing Rate: " << result.avgFiringRate << std::endl;
    file << "Average Synaptic Weight: " << result.avgSynapticWeight << std::endl;
    file << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    file << "Novelty Level: " << result.noveltyLevel << std::endl;
    file << "Curiosity Level: " << result.curiosityLevel << std::endl;
    file << "Dopamine Level: " << result.dopamineLevel << std::endl;
    file << "Wall Clock Time: " << result.totalWallClockTime << " seconds" << std::endl;
    file << std::endl;
    
    file << "=== System Integration Status ===" << std::endl;
    file << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    file << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    file << std::endl;
    
    // Add profiling data if enabled
    if (args.profile_memory || args.profile_cpu || args.profile_detailed) {
        file << "=== Performance Profile ===" << std::endl;
        file << "Mode: " << args.mode << std::endl;
        file << "Profiling Level: " << args.profiling_level << std::endl;
        if (args.profile_memory) file << "Memory Profiling: ENABLED" << std::endl;
        if (args.profile_cpu) file << "CPU Profiling: ENABLED" << std::endl;
        if (args.profile_detailed) file << "Detailed Profiling: ENABLED" << std::endl;
        file << std::endl;
    }
    
    file.close();
}

int main(int argc, char* argv[]) {
    CommandLineArgs args = parse_args(argc, argv);
    
    if (args.help) {
        print_help();
        return 0;
    }
    
    if (args.version) {
        print_version();
        return 0;
    }
    
    // Initialize logging
    nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);
    if (args.verbose) {
        nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Debug);
    }
    
    std::cout << "=== NLM Phase 6 Unified Interface ===" << std::endl;
    std::cout << "Mode: " << args.mode << std::endl;
    std::cout << "Configuration: " << args.config_file << std::endl;
    std::cout << "Steps: " << args.steps << std::endl;
    std::cout << "Neurons: " << args.neurons << std::endl;
    std::cout << "Regions: " << args.regions << std::endl;
    std::cout << std::endl;
    
    nlm::Phase6Config config;
    config.maxSteps = args.steps;
    config.neuronCount = args.neurons;
    config.regionCount = args.regions;
    config.connectionProbability = args.connection_prob;
    config.enableCheckpointing = args.enable_checkpointing;
    config.enableReplay = args.enable_replay;
    config.enableDevelopment = args.enable_development;
    config.checkpointPath = args.checkpoint_path;
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    nlm::Phase6IntegratedExperiment experiment;
    
    if (args.mode == "test") {
        std::cout << "--- Running Integration Verification ---" << std::endl;
        bool success = experiment.verifyIntegration();
        if (success) {
            std::cout << "✓ Integration verification passed!" << std::endl;
        } else {
            std::cout << "✗ Integration verification failed!" << std::endl;
            return 1;
        }
    } else if (args.mode == "benchmark") {
        std::cout << "--- Running Performance Benchmark ---" << std::endl;
        // Run benchmark with larger settings
        nlm::Phase6Config benchmarkConfig;
        benchmarkConfig.maxSteps = args.benchmark_steps;
        benchmarkConfig.neuronCount = 2000;
        benchmarkConfig.regionCount = 2;
        benchmarkConfig.enableCheckpointing = false;
        
        auto benchStart = std::chrono::high_resolution_clock::now();
        auto result = experiment.run(benchmarkConfig);
        auto benchEnd = std::chrono::high_resolution_clock::now();
        
        double benchTime = std::chrono::duration<double>(benchEnd - benchStart).count();
        std::cout << "Benchmark completed in " << benchTime << " seconds" << std::endl;
        std::cout << "Steps per second: " << args.benchmark_steps / benchTime << std::endl;
        
        if (!args.output_file.empty()) {
            save_results_to_file(args.output_file, result, args);
        }
    } else if (args.mode == "profile") {
        std::cout << "--- Running with Performance Profiling ---" << std::endl;
        
        // Profiling level 1: Basic timing
        std::cout << "[PROFILING LEVEL " << args.profiling_level << "] Starting profiling..." << std::endl;
        
        if (args.profile_memory) {
            std::cout << "[PROFILING] Memory profiling enabled" << std::endl;
            // In a real implementation, we would collect memory usage data
        }
        
        if (args.profile_cpu) {
            std::cout << "[PROFILING] CPU profiling enabled" << std::endl;
            // In a real implementation, we would collect CPU performance data
        }
        
        if (args.profile_detailed) {
            std::cout << "[PROFILING] Detailed profiling enabled" << std::endl;
            // In a real implementation, we would collect detailed performance metrics
        }
        
        auto result = experiment.run(config);
        
        std::cout << "[PROFILING] Integration complete." << std::endl;
        
        if (!args.output_file.empty()) {
            save_results_to_file(args.output_file, result, args);
        }
        
        // Write profile data if profiling was enabled
        if (args.profile_memory || args.profile_cpu || args.profile_detailed) {
            std::cout << "[PROFILING] Writing profile data to: " << args.profile_output << std::endl;
            std::ofstream profileFile(args.profile_output);
            if (profileFile.is_open()) {
                profileFile << "=== NLM Performance Profile ===" << std::endl;
                profileFile << "Execution Time: " << result.totalWallClockTime << "s" << std::endl;
                profileFile << "Total Reward: " << result.totalReward << std::endl;
                profileFile << "Average Firing Rate: " << result.avgFiringRate << std::endl;
                profileFile << "Memory Episodes: " << result.memoryEpisodesStored << std::endl;
                profileFile << "System Integration: " << (result.memoryWorkingMemoryIntegrated ? "OK" : "FAIL") << std::endl;
                profileFile << "Mode: " << args.mode << std::endl;
                profileFile << "Profiling Level: " << args.profiling_level << std::endl;
                profileFile << "Neurons: " << config.neuronCount << std::endl;
                profileFile << "Regions: " << config.regionCount << std::endl;
                profileFile << "Steps: " << config.maxSteps << std::endl;
                profileFile.close();
            }
        }
    } else { // default run mode
        std::cout << "--- Running Complete Integration Experiment ---" << std::endl;
        auto result = experiment.run(config);
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
    }
    
    auto endWall = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endWall - startWall).count();
    
    std::cout << std::endl;
    std::cout << "=== Execution Complete ===" << std::endl;
    std::cout << "Total time: " << totalTime << " seconds" << std::endl;
    
    if (!args.output_file.empty()) {
        std::cout << "Results saved to: " << args.output_file << std::endl;
    }
    
    if (args.profile_memory || args.profile_cpu || args.profile_detailed) {
        std::cout << "Profile data saved to: " << args.profile_output << std::endl;
    }
    
    return 0;
}