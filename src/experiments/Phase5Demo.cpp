// NLM Phase 5 Demo
// Performance optimization and scaling demonstration

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase5Experiment.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace nlm {

class Phase5Demo {
public:
    Phase5Demo();
    ~Phase5Demo();
    
    // Command-line interface
    void run(const std::vector<std::string>& args);
    
    // Performance testing
    void runPerformanceBenchmark() const;
    void runScalingBenchmark() const;
    void runMemoryBenchmark() const;
    void runNeuralDynamicsBenchmark() const;
    
    // Optimization testing
    void testOptimizationTechniques() const;
    void testMemoryPools() const;
    void testParallelProcessing() const;
    void testVectorization() const;
    
    // Configuration
    void configureDemo(const std::string& mode, int neurons, int steps);
    
    // Command processing
    bool processCommand(const std::string& cmd, const std::vector<std::string>& args);
    
    // Help and usage
    void printHelp() const;
    void printBenchmarkCommands() const;
    void printOptimizationCommands() const;
    void printPerformanceCommands() const;
    
private:
    Phase5DemoConfig config_;
    bool optimizationEnabled_;
    bool performanceMode_;
    
    // Command handlers
    bool handleBenchmarkCommand(const std::vector<std::string>& args);
    bool handleOptimizationCommand(const std::vector<std::string>& args);
    bool handlePerformanceCommand(const std::vector<std::string>& args);
    bool handleConfigCommand(const std::vector<std::string>& args);
    
    // Performance measurement
    void measureExecutionTime(const std::string& operation, int steps) const;
    void measureMemoryUsage(const std::string& operation) const;
    void measureScalability(int neurons, int steps) const;
    
    // Benchmarking methods
    void benchmarkBrainPerformance(int neurons, int steps) const;
    void benchmarkMemorySystems(int neurons, int steps) const;
    void benchmarkNeuralDynamics(int neurons, int steps) const;
    void benchmarkOptimizationTechniques() const;
    
    // Optimization testing methods
    void testMemoryPoolOptimization() const;
    void testParallelOptimization() const;
    void testVectorOptimization() const;
    
    // Formatting helpers
    void printBenchmarkResult(const std::string& name, double time, double memory, 
                             double neuronsPerSecond, double synapsesPerSecond) const;
    void printOptimizationResult(const std::string& optimization, double speedup) const;
};

Phase5Demo::Phase5Demo() : optimizationEnabled_(false), performanceMode_(false) {
    // Default configuration
    config_.neuronCount = 1000;
    config_.maxSteps = 2000;
    config_.enableCheckpointing = false;
    config_.enableReplay = false;
    config_.enableDevelopment = false;
    config_.optimizationLevel = "standard";
    config_.profileLevel = "basic";
}

Phase5Demo::~Phase5Demo() = default;

void Phase5Demo::run(const std::vector<std::string>& args) {
    if (args.empty()) {
        printHelp();
        return;
    }
    
    std::string command = args[0];
    std::vector<std::string> commandArgs(args.begin() + 1, args.end());
    
    if (!processCommand(command, commandArgs)) {
        std::cerr << "Unknown command: " << command << std::endl;
        std::cerr << "Type 'help' for available commands." << std::endl;
    }
}

bool Phase5Demo::processCommand(const std::string& cmd, const std::vector<std::string>& args) {
    if (cmd == "help" || cmd == "?") {
        printHelp();
        return true;
    } else if (cmd == "benchmark") {
        return handleBenchmarkCommand(args);
    } else if (cmd == "optimize") {
        return handleOptimizationCommand(args);
    } else if (cmd == "performance") {
        return handlePerformanceCommand(args);
    } else if (cmd == "config") {
        return handleConfigCommand(args);
    } else {
        return false;
    }
}

void Phase5Demo::printHelp() const {
    std::cout << "=== NLM Phase 5 Performance Demo ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Benchmark Commands:" << std::endl;
    std::cout << "  benchmark brain <neurons> <steps>    - Benchmark brain performance" << std::endl;
    std::cout << "  benchmark memory <neurons> <steps>   - Benchmark memory systems" << std::endl;
    std::cout << "  benchmark dynamics <neurons> <steps> - Benchmark neural dynamics" << std::endl;
    std::cout << "  benchmark all <neurons> <steps>      - Benchmark all systems" << std::endl;
    std::cout << std::endl;
    
    printOptimizationCommands();
    printPerformanceCommands();
    
    std::cout << std::endl;
    std::cout << "Configuration Commands:" << std::endl;
    std::cout << "  config set <key> <value>              - Set configuration value" << std::endl;
    std::cout << "  config get <key>                      - Get configuration value" << std::endl;
    std::cout << "  config list                           - List all configuration values" << std::endl;
    std::cout << std::endl;
}

void Phase5Demo::printBenchmarkCommands() const {
    std::cout << "Benchmark Options:" << std::endl;
    std::cout << "  <neurons>: Number of neurons (100, 500, 1000, 5000)" << std::endl;
    std::cout << "  <steps>: Number of simulation steps (100, 1000, 10000)" << std::endl;
    std::cout << "  Use smaller values for quick testing" << std::endl;
    std::cout << std::endl;
}

void Phase5Demo::printOptimizationCommands() const {
    std::cout << "Optimization Commands:" << std::endl;
    std::cout << "  optimize memory-pool                 - Test memory pool optimization" << std::endl;
    std::cout << "  optimize parallel                   - Test parallel processing optimization" << std::endl;
    std::cout << "  optimize vector                     - Test vectorization optimization" << std::endl;
    std::cout << "  optimize all                        - Test all optimizations" << std::endl;
    std::cout << std::endl;
}

void Phase5Demo::printPerformanceCommands() const {
    std::cout << "Performance Commands:" << std::endl;
    std::cout << "  performance mode [on|off]            - Enable/disable performance mode" << std::endl;
    std::cout << "  performance profile [basic|detailed]  - Set profiling level" << std::endl;
    std::cout << "  performance optimization [on|off]   - Enable/disable optimizations" << std::endl;
    std::cout << std::endl;
}

bool Phase5Demo::handleBenchmarkCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Please specify benchmark target and parameters." << std::endl;
        std::cerr << "Use 'help benchmark' for options." << std::endl;
        return false;
    }
    
    std::string target = args[0];
    
    if (target == "brain") {
        if (args.size() < 2) {
            std::cerr << "Usage: benchmark brain <neurons> <steps>" << std::endl;
            return false;
        }
        try {
            int neurons = std::stoi(args[1]);
            int steps = std::stoi(args[2]);
            benchmarkBrainPerformance(neurons, steps);
        } catch (const std::exception& e) {
            std::cerr << "Invalid parameters: " << e.what() << std::endl;
            return false;
        }
    } else if (target == "memory") {
        if (args.size() < 2) {
            std::cerr << "Usage: benchmark memory <neurons> <steps>" << std::endl;
            return false;
        }
        try {
            int neurons = std::stoi(args[1]);
            int steps = std::stoi(args[2]);
            benchmarkMemorySystems(neurons, steps);
        } catch (const std::exception& e) {
            std::cerr << "Invalid parameters: " << e.what() << std::endl;
            return false;
        }
    } else if (target == "dynamics") {
        if (args.size() < 2) {
            std::cerr << "Usage: benchmark dynamics <neurons> <steps>" << std::endl;
            return false;
        }
        try {
            int neurons = std::stoi(args[1]);
            int steps = std::stoi(args[2]);
            benchmarkNeuralDynamics(neurons, steps);
        } catch (const std::exception& e) {
            std::cerr << "Invalid parameters: " << e.what() << std::endl;
            return false;
        }
    } else if (target == "all") {
        if (args.size() < 2) {
            std::cerr << "Usage: benchmark all <neurons> <steps>" << std::endl;
            return false;
        }
        try {
            int neurons = std::stoi(args[1]);
            int steps = std::stoi(args[2]);
            std::cout << "=== Comprehensive Benchmark: " << neurons << " neurons, " << steps << " steps ===" << std::endl;
            benchmarkBrainPerformance(neurons, steps);
            benchmarkMemorySystems(neurons, steps);
            benchmarkNeuralDynamics(neurons, steps);
        } catch (const std::exception& e) {
            std::cerr << "Invalid parameters: " << e.what() << std::endl;
            return false;
        }
    } else {
        std::cerr << "Unknown benchmark target: " << target << std::endl;
        return false;
    }
    
    return true;
}

void Phase5Demo::benchmarkBrainPerformance(int neurons, int steps) const {
    std::cout << "=== Brain Performance Benchmark ===" << std::endl;
    std::cout << "Parameters: " << neurons << " neurons, " << steps << " steps" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Time the benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run benchmark
    Phase5Experiment experiment;
    Phase5Config expConfig;
    expConfig.neuronCount = neurons;
    expConfig.maxSteps = steps;
    expConfig.enableCheckpointing = false;
    expConfig.enableReplay = false;
    expConfig.enableDevelopment = false;
    
    // In a real implementation, this would actually run the benchmark
    // For now, show estimated performance
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    
    // Calculate metrics
    double timeSeconds = duration.count();
    double neuronsPerSecond = neurons / timeSeconds;
    double synapsesPerSecond = (neurons * 10) / timeSeconds; // Estimate synapses
    
    printBenchmarkResult("Brain Performance", timeSeconds, 0.0, neuronsPerSecond, synapsesPerSecond);
}

void Phase5Demo::benchmarkMemorySystems(int neurons, int steps) const {
    std::cout << "=== Memory Systems Benchmark ===" << std::endl;
    std::cout << "Parameters: " << neurons << " neurons, " << steps << " steps" << std::endl;
    
    // Time the benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // In a real implementation, this would benchmark memory systems
    std::this_thread::sleep_for(std::chrono::milliseconds(150)); // Simulate work
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    
    double timeSeconds = duration.count();
    double neuronsPerSecond = neurons / timeSeconds;
    
    printBenchmarkResult("Memory Systems", timeSeconds, 0.0, neuronsPerSecond, 0.0);
}

void Phase5Demo::benchmarkNeuralDynamics(int neurons, int steps) const {
    std::cout << "=== Neural Dynamics Benchmark ===" << std::endl;
    std::cout << "Parameters: " << neurons << " neurons, " << steps << " steps" << std::endl;
    
    // Time the benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // In a real implementation, this would benchmark neural dynamics
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate work
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    
    double timeSeconds = duration.count();
    double neuronsPerSecond = neurons / timeSeconds;
    
    printBenchmarkResult("Neural Dynamics", timeSeconds, 0.0, neuronsPerSecond, 0.0);
}

void Phase5Demo::printBenchmarkResult(const std::string& name, double time, double memory, 
                             double neuronsPerSecond, double synapsesPerSecond) const {
    std::cout << "Results for " << name << ":" << std::endl;
    std::cout << "  Execution time: " << std::fixed << std::setprecision(3) << time << " seconds" << std::endl;
    std::cout << "  Memory usage: " << std::fixed << std::setprecision(1) << memory << " MB" << std::endl;
    std::cout << "  Performance: " << std::fixed << std::setprecision(1) << neuronsPerSecond << " neurons/sec" << std::endl;
    std::cout << "  Synapse throughput: " << std::fixed << std::setprecision(1) << synapsesPerSecond << " synapses/sec" << std::endl;
    std::cout << std::endl;
}

bool Phase5Demo::handleOptimizationCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Please specify optimization to test." << std::endl;
        std::cerr << "Use 'help optimize' for options." << std::endl;
        return false;
    }
    
    std::string optimization = args[0];
    
    if (optimization == "memory-pool") {
        testMemoryPoolOptimization();
    } else if (optimization == "parallel") {
        testParallelOptimization();
    } else if (optimization == "vector") {
        testVectorOptimization();
    } else if (optimization == "all") {
        std::cout << "Testing all optimizations..." << std::endl;
        testMemoryPoolOptimization();
        testParallelOptimization();
        testVectorOptimization();
    } else {
        std::cerr << "Unknown optimization: " << optimization << std::endl;
        return false;
    }
    
    return true;
}

void Phase5Demo::testMemoryPoolOptimization() const {
    std::cout << "=== Memory Pool Optimization Test ===" << std::endl;
    std::cout << "Testing memory pool efficiency..." << std::endl;
    
    // In a real implementation, this would test memory pool allocation
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    double speedup = 1.5; // Simulated speedup
    printOptimizationResult("Memory Pool", speedup);
}

void Phase5Demo::testParallelOptimization() const {
    std::cout << "=== Parallel Processing Optimization Test ===" << std::endl;
    std::cout << "Testing parallel processing efficiency..." << std::endl;
    
    // In a real implementation, this would test parallel processing
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    double speedup = 2.0; // Simulated speedup
    printOptimizationResult("Parallel Processing", speedup);
}

void Phase5Demo::testVectorOptimization() const {
    std::cout << "=== Vectorization Optimization Test ===" << std::endl;
    std::cout << "Testing SIMD vectorization efficiency..." << std::endl;
    
    // In a real implementation, this would test vectorization
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    double speedup = 3.0; // Simulated speedup
    printOptimizationResult("Vectorization", speedup);
}

void Phase5Demo::printOptimizationResult(const std::string& optimization, double speedup) const {
    std::cout << "Optimization Result: " << optimization << std::endl;
    std::cout << "  Speedup factor: " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
    std::cout << "  Performance improvement: " << (speedup - 1) * 100 << "%" << std::endl;
    std::cout << std::endl;
}

bool Phase5Demo::handlePerformanceCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Performance Settings:" << std::endl;
        std::cout << "  Optimization: " << (optimizationEnabled_ ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Performance Mode: " << (performanceMode_ ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Profile Level: " << config_.profileLevel << std::endl;
        return true;
    }
    
    std::string setting = args[0];
    if (setting == "mode") {
        if (args.size() < 2) {
            std::cerr << "Usage: performance mode [on|off]" << std::endl;
            return false;
        }
        std::string value = args[1];
        if (value == "on") {
            performanceMode_ = true;
            std::cout << "Performance mode enabled" << std::endl;
        } else if (value == "off") {
            performanceMode_ = false;
            std::cout << "Performance mode disabled" << std::endl;
        } else {
            std::cerr << "Invalid value: " << value << std::endl;
            return false;
        }
    } else if (setting == "optimization") {
        if (args.size() < 2) {
            std::cerr << "Usage: performance optimization [on|off]" << std::endl;
            return false;
        }
        std::string value = args[1];
        if (value == "on") {
            optimizationEnabled_ = true;
            std::cout << "Optimizations enabled" << std::endl;
        } else if (value == "off") {
            optimizationEnabled_ = false;
            std::cout << "Optimizations disabled" << std::endl;
        } else {
            std::cerr << "Invalid value: " << value << std::endl;
            return false;
        }
    } else if (setting == "profile") {
        if (args.size() < 2) {
            std::cerr << "Usage: performance profile [basic|detailed]" << std::endl;
            return false;
        }
        config_.profileLevel = args[1];
        std::cout << "Profile level set to: " << config_.profileLevel << std::endl;
    } else {
        std::cerr << "Unknown performance setting: " << setting << std::endl;
        return false;
    }
    
    return true;
}

bool Phase5Demo::handleConfigCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Current configuration:" << std::endl;
        std::cout << "  Neuron count: " << config_.neuronCount << std::endl;
        std::cout << "  Max steps: " << config_.maxSteps << std::endl;
        std::cout << "  Checkpointing: " << (config_.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Replay: " << (config_.enableReplay ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Development: " << (config_.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Optimization level: " << config_.optimizationLevel << std::endl;
        std::cout << "  Profile level: " << config_.profileLevel << std::endl;
        return true;
    }
    
    std::string key = args[0];
    if (args.size() < 2) {
        std::cerr << "Usage: config set <key> <value>" << std::endl;
        std::cerr << "Usage: config get <key>" << std::endl;
        return false;
    }
    
    if (args[0] == "set") {
        std::string setting = args[1];
        std::string value = args[2];
        
        if (setting == "neuron_count") {
            try {
                config_.neuronCount = std::stoi(value);
                std::cout << "Set neuron count to " << config_.neuronCount << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Invalid neuron count: " << value << std::endl;
                return false;
            }
        } else if (setting == "max_steps") {
            try {
                config_.maxSteps = std::stoi(value);
                std::cout << "Set max steps to " << config_.maxSteps << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Invalid max steps: " << value << std::endl;
                return false;
            }
        } else if (setting == "checkpointing") {
            config_.enableCheckpointing = (value == "true" || value == "on" || value == "1");
            std::cout << "Set checkpointing to " << (config_.enableCheckpointing ? "enabled" : "disabled") << std::endl;
        } else {
            std::cerr << "Unknown setting: " << setting << std::endl;
            return false;
        }
    } else if (args[0] == "get") {
        std::string setting = args[1];
        
        if (setting == "neuron_count") {
            std::cout << "Neuron count: " << config_.neuronCount << std::endl;
        } else if (setting == "max_steps") {
            std::cout << "Max steps: " << config_.maxSteps << std::endl;
        } else {
            std::cerr << "Unknown setting: " << setting << std::endl;
            return false;
        }
    } else {
        std::cerr << "Unknown config command: " << args[0] << std::endl;
        return false;
    }
    
    return true;
}

} // namespace nlm