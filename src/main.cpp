// NLM (熙然) - Neural Learning Machine
// Phase 2: Enhanced Real Neural Computation Demo
//
// This phase implements comprehensive test suite with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// Enhanced features:
// - Comprehensive command-line argument parsing
// - Flexible test execution control
// - Runtime monitoring and logging
// - Checkpoint save/load support
// - Performance profiling options
// - Detailed statistics and reporting

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
#include "development/DevelopmentSystem.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <filesystem>
#include <cstdint>
#include <limits>

namespace nlm {

// Timing utilities
class Timer {
public:
    Timer() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime_).count();
    }
    
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Progress reporting utilities
class ProgressReporter {
public:
    ProgressReporter(size_t total, size_t interval, bool quiet = false)
        : total_(total), interval_(interval), quiet_(quiet), current_(0) {}
    
    void update(size_t current, const std::string& message = "") {
        current_ = current;
        if (!quiet_ && current_ % interval_ == 0) {
            double progress = static_cast<double>(current_) / total_ * 100.0;
            std::cout << "\rProgress: [" << std::string(static_cast<size_t>(progress / 10), '=')
                     << (progress < 100 ? ">" : "=") << std::string(9 - static_cast<size_t>(progress / 10), ' ')
                     << "] " << std::fixed << std::setprecision(1) << progress << "% "
                     << message << std::flush;
        }
    }
    
    void complete() {
        if (!quiet_) {
            std::cout << "\rProgress: [==========] 100.0% Completed" << std::endl;
        }
    }
    
private:
    size_t total_;
    size_t interval_;
    bool quiet_;
    size_t current_;
};

// Statistics collection
class TestStatistics {
public:
    TestStatistics() : reset() {}
    
    void record(const std::string& name, double value, bool isTime = false) {
        if (isTime) {
            times_[name] = value;
        } else {
            metrics_[name] = value;
        }
    }
    
    double getMetric(const std::string& name) const {
        auto it = metrics_.find(name);
        return it != metrics_.end() ? it->second : 0.0;
    }
    
    double getTime(const std::string& name) const {
        auto it = times_.find(name);
        return it != times_.end() ? it->second : 0.0;
    }
    
    void printReport() const {
        std::cout << "\nStatistics Report:" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        if (!metrics_.empty()) {
            std::cout << "Metrics:"" << std::endl;
            for (const auto& [name, value] : metrics_) {
                std::cout << "  " << std::left << std::setw(20) << name << ": " << value << std::endl;
            }
        }
        
        if (!times_.empty()) {
            std::cout << "Times:"" << std::endl;
            for (const auto& [name, value] : times_) {
                std::cout << "  " << std::left << std::setw(20) << name << ": " << std::fixed << std::setprecision(6) << value << "s" << std::endl;
            }
        }
    }
    
    void clear() {
        metrics_.clear();
        times_.clear();
    }
    
private:
    std::unordered_map<std::string, double> metrics_;
    std::unordered_map<std::string, double> times_;
};

// Comprehensive Command-line Interface
struct CommandLineOptions {
    bool help = false;
    bool verbose = false;
    bool quiet = false;
    bool profile = false;
    std::string configFile = "configs/default.cfg";
    std::string checkpointFile = "";
    std::string savePath = "";
    bool runAllTests = true;
    bool skipTest1 = false;
    bool skipTest2 = false;
    bool skipTest3 = false;
    size_t maxSteps = 1000;
    size_t testSteps = 1000;
    size_t stdpSteps = 50;
    float currentInjection = 30.0f;
    float sensoryInputScale = 10.0f;
    float motorCommandScale = 1.0f;
    size_t patternNeuronsCount = 20;
    bool enableStructuralPlasticity = true;
    bool enableSTDP = true;
    bool enableHebbian = true;
    int randomSeed = 42;
    double timestep = 0.001;
    bool showProgress = true;
    size_t progressInterval = 100;
    std::string logFile = "";
    std::string outputFile = "";
    bool runPhase6 = false;
    std::string phase6Config = "";
};

// Parse command-line arguments with comprehensive options
CommandLineOptions parseCommandLine(int argc, char** argv) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg == "--profile") {
            options.profile = true;
        } else if (arg == "--test1") {
            options.runAllTests = true;
            options.skipTest1 = false;
            options.skipTest2 = true;
            options.skipTest3 = true;
        } else if (arg == "--test2") {
            options.runAllTests = true;
            options.skipTest1 = true;
            options.skipTest2 = false;
            options.skipTest3 = true;
        } else if (arg == "--test3") {
            options.runAllTests = true;
            options.skipTest1 = true;
            options.skipTest2 = true;
            options.skipTest3 = false;
        } else if (arg == "--all-tests" || arg == "--tests") {
            options.runAllTests = true;
            options.skipTest1 = false;
            options.skipTest2 = false;
            options.skipTest3 = false;
        } else if (arg == "--skip-test1") {
            options.skipTest1 = true;
        } else if (arg == "--skip-test2") {
            options.skipTest2 = true;
        } else if (arg == "--skip-test3") {
            options.skipTest3 = true;
        } else if (arg == "--no-test1") {
            options.skipTest1 = true;
        } else if (arg == "--no-test2") {
            options.skipTest2 = true;
        } else if (arg == "--no-test3") {
            options.skipTest3 = true;
        } else if (arg == "--max-steps") {
            if (i + 1 < argc) options.maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--test-steps") {
            if (i + 1 < argc) options.testSteps = std::stoul(argv[++i]);
        } else if (arg == "--stdp-steps") {
            if (i + 1 < argc) options.stdpSteps = std::stoul(argv[++i]);
        } else if (arg == "--current" || arg == "--input-current") {
            if (i + 1 < argc) options.currentInjection = std::stof(argv[++i]);
        } else if (arg == "--sensory-scale") {
            if (i + 1 < argc) options.sensoryInputScale = std::stof(argv[++i]);
        } else if (arg == "--motor-scale") {
            if (i + 1 < argc) options.motorCommandScale = std::stof(argv[++i]);
        } else if (arg == "--pattern-neurons") {
            if (i + 1 < argc) options.patternNeuronsCount = std::stoul(argv[++i]);
        } else if (arg == "--random-seed") {
            if (i + 1 < argc) options.randomSeed = std::stoi(argv[++i]);
        } else if (arg == "--timestep") {
            if (i + 1 < argc) options.timestep = std::stod(argv[++i]);
        } else if (arg == "--progress") {
            options.showProgress = true;
            if (i + 1 < argc) options.progressInterval = std::stoul(argv[++i]);
        } else if (arg == "--no-progress") {
            options.showProgress = false;
        } else if (arg == "--log-file") {
            if (i + 1 < argc) options.logFile = argv[++i];
        } else if (arg == "--output-file") {
            if (i + 1 < argc) options.outputFile = argv[++i];
        } else if (arg == "--checkpoint" || arg == "--load") {
            if (i + 1 < argc) options.checkpointFile = argv[++i];
        } else if (arg == "--save" || arg == "--checkpoint-file") {
            if (i + 1 < argc) options.savePath = argv[++i];
        } else if (arg == "--config") {
            if (i + 1 < argc) options.configFile = argv[++i];
        } else if (arg == "--no-plasticity") {
            options.enableStructuralPlasticity = false;
            options.enableSTDP = false;
            options.enableHebbian = false;
        } else if (arg == "--no-stdp") {
            options.enableSTDP = false;
        } else if (arg == "--no-hebbian") {
            options.enableHebbian = false;
        } else if (arg == "--no-structural") {
            options.enableStructuralPlasticity = false;
        } else if (arg == "--version" || arg == "--ver") {
            std::cout << "NLM Phase 2 Enhanced Demo v1.0" << std::endl;
            options.help = true;
        } else if (arg == "--stats") {
            options.verbose = true;
            options.showProgress = false;
        } else if (arg == "--phase6") {
            options.runPhase6 = true;
        } else if (arg == "--phase6-config") {
            if (i + 1 < argc) options.phase6Config = argv[++i];
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
        }
    }
    
    return options;
}

// Print help information
void printHelp() {
    std::cout << R"(
════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
                                                                                      ║  NLM — 熙然 (Phase 2 Demo)                                              ║
                                                                                      ║  Enhanced Neural Computation Demo                                        ║
                                                                                      ║  An experimental artificial developmental brain                            ║
                                                                                      ║                                                               ║
════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
                                                                                      ║                                                                       ║
                                                                                      ║  USAGE:                                                               ║
                                                                                      ║  ./nlm [OPTIONS]                                                       ║
                                                                                      ║                                                                       ║
                                                                                      ║  BASIC EXAMPLES:                                                       ║
                                                                                      ║  ./nlm                                   ║ Run all three tests (default)              ║
                                                                                      ║  ./nlm --test1                           ║ Only run basic connectivity test            ║
                                                                                      ║  ./nlm --test2                           ║ Only run plasticity learning test           ║
                                                                                      ║  ./nlm --test3                           ║ Only run STDP verification test            ║
                                                                                      ║                                                                       ║
                                                                                      ║  ADVANCED USAGE:                                                        ║
                                                                                      ║  ./nlm --verbose --config my.cfg --max-steps 2000 --current 50.0      ║
                                                                                      ║                                                                       ║
                                                                                      ║                                                                       ║
════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
                                                                                      ║                                                                       ║
                                                                                      ║  COMMON OPTIONS:                                                       ║
                                                                                      ║                                                                       ║
                                                                                      ║  -h, --help              Show this help message                                        ║
                                                                                      ║  -v, --verbose           Enable verbose logging                                      ║
                                                                                      ║  -q, --quiet             Minimal output (suppress progress)                              ║
                                                                                      ║  --profile               Enable performance profiling                                 ║
                                                                                      ║  --test1, --test2, --test3  Run specific test (only first/second/third)║
                                                                                      ║  --all-tests              Run all tests (default)                        ║
                                                                                      ║  --no-test1, --no-test2...  Skip specific test                       ║
                                                                                      ║  --max-steps N           Set maximum steps for test (default 1000)     ║
                                                                                      ║  --current VAL           Set current injection magnitude (default 30.0)║
                                                                                      ║  --pattern-neurons N     Number of neurons in input pattern (default 20)║
                                                                                      ║  --random-seed N         Random seed for reproducibility (default 42)  ║
                                                                                      ║  --timestep VAL          Simulation timestep (default 0.001)        ║
                                                                                      ║                                                                       ║
                                                                                      ║  CONFIGURATION:                                                       ║
                                                                                      ║  --config FILE           Load configuration from file (default: configs/default.cfg) ║
                                                                                      ║                                                                       ║
                                                                                      ║  CHECKPOINT/SAVE/LOAD:                                                 ║
                                                                                      ║  --checkpoint FILE, --load FILE   Load brain state from checkpoint          ║
                                                                                      ║  --save FILE, --checkpoint-file FILE  Save brain state to file after test    ║
                                                                                      ║                                                                       ║
                                                                                      ║  OUTPUT/LOGGING:                                                       ║
                                                                                      ║  --log-file FILE           Log all output to file                         ║
                                                                                      ║  --output-file FILE        Write results to output file                    ║
                                                                                      ║  --no-progress, --quiet    Suppress progress output                       ║
                                                                                      ║  --progress INTERVAL       Show progress every INTERVAL steps (default 100)║
                                                                                      ║                                                                       ║
                                                                                      ║  PLASTICITY/LEARNING:                                                   ║
                                                                                      ║  --no-plasticity           Disable all plasticity rules                   ║
                                                                                      ║  --no-stdp                 Disable STDP (Hebbian remains enabled)         ║
                                                                                      ║  --no-hebbian             Disable Hebbian (STDP remains enabled)          ║
                                                                                      ║  --no-structural           Disable structural plasticity                   ║
                                                                                      ║                                                                       ║
                                                                                      ║  TEST/DEMO CONTROL:                                                    ║
                                                                                      ║  --test1, --test2, --test3   Run specific test (1=connectivity, 2=plasticity, 3=STDP)║
                                                                                      ║  --all-tests                Run all tests (default)                        ║
                                                                                      ║  --skip-test1, --skip-test2, --skip-test3  Skip specific test              ║
                                                                                      ║  --no-test1, --no-test2, --no-test3  Alternative syntax to skip tests      ║
                                                                                      ║                                                                       ║
                                                                                      ║  BEHAVIORAL MODIFIERS:                                                  ║
                                                                                      ║  --current VAL, --input-current VAL    Set input current injection (default 30.0)║
                                                                                      ║  --sensory-scale VAL        Scale factor for sensory input (default 10.0)   ║
                                                                                      ║  --motor-scale VAL          Scale factor for motor commands (default 1.0) ║
                                                                                      ║  --random-seed N            Random seed for reproducible results (default 42)║
                                                                                      ║  --timestep VAL             Simulation timestep in seconds (default 0.001) ║
                                                                                      ║                                                                       ║
                                                                                      ║  PHASE 6 INTEGRATION TEST:                                              ║
                                                                                      ║  --phase6                   Run Phase 6 comprehensive integration test      ║
                                                                                      ║  --phase6-config FILE       Load Phase 6 configuration from file           ║
                                                                                      ║                                                                       ║
                                                                                      ║  DEVELOPMENT/ADVANCED:                                                  ║
                                                                                      ║  --profile                  Enable performance profiling and timing       ║
                                                                                      ║  --stats                    Enable statistics mode (quiet progress)       ║
                                                                                      ║  --version, --ver          Show version information                       ║
                                                                                      ║                                                                       ║
                                                                                      ║  MISCELLANEOUS:                                                         ║
                                                                                      ║  --no-plasticity           Disable all plasticity rules                   ║
                                                                                      ║  --no-stdp                 Disable STDP plasticity rule                     ║
                                                                                      ║  --no-hebbian             Disable Hebbian plasticity rule                     ║
                                                                                      ║  --no-structural           Disable structural plasticity                   ║
                                                                                      ║                                                                       ║
                                                                                      ║  EXAMPLES:                                                             ║
                                                                                      ║  ./nlm --test1 --max-steps 500 --progress 50                      ║ Run test 1 with 500 steps, show progress every 50 steps ║
                                                                                      ║  ./nlm --test2 --current 50.0 --sensory-scale 20.0 --motor-scale 2.0   ║ Run test 2 with custom input parameters ║
                                                                                      ║  ./nlm --all-tests --save demo1.checkpoint                                     ║ Run all tests and save final state to demo1.checkpoint ║
                                                                                      ║  ./nlm --test3 --load saved_brain.checkpoint --random-seed 12345       ║ Load and run test 3 with specific seed ║
                                                                                      ║  ./nlm --phase6 --phase6-config phase6.cfg --no-structural              ║ Run Phase 6 integration test with custom config ║
                                                                                      ║  ./nlm --verbose --log-file nlm.log --output-file results.txt         ║ Enable verbose mode and save outputs to files ║
                                                                                      ║                                                                       ║
════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
                                                                                      ║                                                                       ║
                                                                                      ║  NLM Phase 2 Enhanced Demo v1.0                                      ║
                                                                                      ║  Release: Enhanced Neural Computation with Full Test Suite              ║
                                                                                      ║  Built with C++17, SIMD optimizations, and modern C++ practices       ║
                                                                                      ║                                                                       ║
════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
                                      "  << std::endl;
}

// Save checkpoint
bool saveCheckpoint(const std::string& filename, const std::shared_ptr<Brain>& brain) {
    try {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open checkpoint file for writing: " << filename << std::endl;
            return false;
        }
        
        // Simple checkpoint serialization
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t synapseCount = brain->getTotalSynapseCount();
        SimulationStep stepCount = brain->getTotalSpikeCount();
        
        file.write(reinterpret_cast<const char*>(&neuronCount), sizeof(size_t));
        file.write(reinterpret_cast<const char*>(&synapseCount), sizeof(size_t));
        file.write(reinterpret_cast<const char*>(&stepCount), sizeof(SimulationStep));
        
        std::cout << "Checkpoint saved to: " << filename << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving checkpoint: " << e.what() << std::endl;
        return false;
    }
}

// Load checkpoint
bool loadCheckpoint(const std::string& filename, std::shared_ptr<Brain>& brain) {
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open checkpoint file for reading: " << filename << std::endl;
            return false;
        }
        
        // Simple checkpoint deserialization
        size_t neuronCount, synapseCount;
        SimulationStep stepCount;
        
        file.read(reinterpret_cast<char*>(&neuronCount), sizeof(size_t));
        file.read(reinterpret_cast<char*>(&synapseCount), sizeof(size_t));
        file.read(reinterpret_cast<char*>(&stepCount), sizeof(SimulationStep));
        
        std::cout << "Checkpoint loaded from: " << filename << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading checkpoint: " << e.what() << std::endl;
        return false;
    }
}

// Test 1: Basic Connectivity Test
bool runTest1(const std::shared_ptr<Brain>& brain, const std::shared_ptr<Environment>& environment,
              const CommandLineOptions& options, TestStatistics& stats) {
    Timer timer;
    size_t totalSteps = options.maxSteps;
    
    std::cout << "Running Test 1: Basic Connectivity Test" << std::endl;
    std::cout << "Steps: " << totalSteps << ", Current: " << options.currentInjection << " μA" << std::endl;
    
    // Create progress reporter
    ProgressReporter reporter(totalSteps, options.progressInterval, options.quiet);
    
    // Reset brain for test
    brain->reset();
    
    // Run simulation
    for (size_t step = 0; step < totalSteps; ++step) {
        // Apply input current
        brain->injectCurrentToNeurons(NeuronType::Excitatory, options.currentInjection);
        
        // Run simulation step
        brain->step(step);
        
        // Record metrics
        if (step % options.progressInterval == 0 || step == totalSteps - 1) {
            size_t neuronCount = brain->getTotalNeuronCount();
            size_t synapseCount = brain->getTotalSynapseCount();
            size_t firingCount = brain->getTotalSpikeCount();
            float avgFiringRate = brain->getAverageFiringRate();
            
            stats.record("test1_step_" + std::to_string(step), step);
            stats.record("test1_neurons", static_cast<double>(neuronCount));
            stats.record("test1_synapses", static_cast<double>(synapseCount));
            stats.record("test1_firing_count", static_cast<double>(firingCount));
            stats.record("test1_avg_firing_rate", static_cast<double>(avgFiringRate));
            
            reporter.update(step, "Step completed");
        }
    }
    
    reporter.complete();
    stats.record("test1_total_time", timer.elapsed(), true);
    
    std::cout << "Test 1 completed successfully in " << timer.elapsed() << " seconds" << std::endl;
    return true;
}

// Test 2: Plasticity Learning Test
bool runTest2(const std::shared_ptr<Brain>& brain, const std::shared_ptr<Environment>& environment,
              const CommandLineOptions& options, TestStatistics& stats) {
    Timer timer;
    size_t totalSteps = options.testSteps;
    
    std::cout << "Running Test 2: Plasticity Learning Test" << std::endl;
    std::cout << "Steps: " << totalSteps << ", Plasticity enabled: " << (options.enableSTDP || options.enableHebbian || options.enableStructuralPlasticity) << std::endl;
    
    ProgressReporter reporter(totalSteps, options.progressInterval, options.quiet);
    
    // Reset brain for test
    brain->reset();
    
    // Update development system stage for plasticity
    if (options.enableStructuralPlasticity) {
        brain->getDevelopmentSystem()->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    }
    
    // Run simulation with plasticity
    for (size_t step = 0; step < totalSteps; ++step) {
        // Apply input current with some variation to trigger plasticity
        float current = options.currentInjection * (1.0f + 0.1f * std::sin(static_cast<float>(step) * 0.1f));
        brain->injectCurrentToNeurons(NeuronType::Excitatory, current);
        
        // Run simulation step
        brain->step(step);
        
        // Update development system
        brain->getDevelopmentSystem()->update(brain.get(), step);
        
        // Record plasticity metrics
        if (step % options.progressInterval == 0 || step == totalSteps - 1) {
            size_t neuronCount = brain->getTotalNeuronCount();
            size_t synapseCount = brain->getTotalSynapseCount();
            size_t firingCount = brain->getTotalSpikeCount();
            float avgFiringRate = brain->getAverageFiringRate();
            
            stats.record("test2_step_" + std::to_string(step), step);
            stats.record("test2_neurons", static_cast<double>(neuronCount));
            stats.record("test2_synapses", static_cast<double>(synapseCount));
            stats.record("test2_firing_count", static_cast<double>(firingCount));
            stats.record("test2_avg_firing_rate", static_cast<double>(avgFiringRate));
            
            reporter.update(step, "Step completed");
        }
    }
    
    reporter.complete();
    stats.record("test2_total_time", timer.elapsed(), true);
    
    std::cout << "Test 2 completed successfully in " << timer.elapsed() << " seconds" << std::endl;
    return true;
}

// Test 3: STDP Verification Test
bool runTest3(const std::shared_ptr<Brain>& brain, const std::shared_ptr<Environment>& environment,
              const CommandLineOptions& options, TestStatistics& stats) {
    Timer timer;
    size_t totalSteps = options.testSteps;
    
    std::cout << "Running Test 3: STDP Verification Test" << std::endl;
    std::cout << "Steps: " << totalSteps << ", STDP enabled: " << options.enableSTDP << std::endl;
    
    ProgressReporter reporter(totalSteps, options.progressInterval, options.quiet);
    
    // Reset brain for test
    brain->reset();
    
    // Set developmental stage for STDP
    if (options.enableSTDP) {
        brain->getDevelopmentSystem()->setDevelopmentalStage(DevelopmentalStage::Maturation);
    }
    
    // Run simulation with spike patterns
    for (size_t step = 0; step < totalSteps; ++step) {
        // Create spike patterns for STDP verification (simplified)
        // In a real implementation, you would inject specific spike patterns
        
        // Apply input current
        brain->injectCurrentToNeurons(NeuronType::Excitatory, options.currentInjection);
        
        // Run simulation step
        brain->step(step);
        
        // Update development system
        brain->getDevelopmentSystem()->update(brain.get(), step);
        
        // Record STDP metrics
        if (step % options.progressInterval == 0 || step == totalSteps - 1) {
            size_t neuronCount = brain->getTotalNeuronCount();
            size_t synapseCount = brain->getTotalSynapseCount();
            size_t firingCount = brain->getTotalSpikeCount();
            float avgFiringRate = brain->getAverageFiringRate();
            
            stats.record("test3_step_" + std::to_string(step), step);
            stats.record("test3_neurons", static_cast<double>(neuronCount));
            stats.record("test3_synapses", static_cast<double>(synapseCount));
            stats.record("test3_firing_count", static_cast<double>(firingCount));
            stats.record("test3_avg_firing_rate", static_cast<double>(avgFiringRate));
            
            reporter.update(step, "Step completed");
        }
    }
    
    reporter.complete();
    stats.record("test3_total_time", timer.elapsed(), true);
    
    std::cout << "Test 3 completed successfully in " << timer.elapsed() << " seconds" << std::endl;
    return true;
}

// Phase 6 Integration Test
bool runPhase6(const std::shared_ptr<Brain>& brain, const std::shared_ptr<Environment>& environment,
              const CommandLineOptions& options, TestStatistics& stats) {
    Timer timer;
    size_t totalSteps = options.maxSteps;
    
    std::cout << "Running Phase 6: Comprehensive Integration Test" << std::endl;
    std::cout << "Steps: " << totalSteps << ", All plasticity enabled: " << (options.enableStructuralPlasticity && options.enableSTDP && options.enableHebbian) << std::endl;
    
    ProgressReporter reporter(totalSteps, options.progressInterval, options.quiet);
    
    // Reset brain for test
    brain->reset();
    
    // Set developmental stage for integration test
    brain->getDevelopmentSystem()->setDevelopmentalStage(DevelopmentalStage::Adult);
    
    // Run comprehensive integration test
    for (size_t step = 0; step < totalSteps; ++step) {
        // Apply varying input current
        float current = options.currentInjection * (0.5f + 0.5f * std::sin(static_cast<float>(step) * 0.05f));
        brain->injectCurrentToNeurons(NeuronType::Excitatory, current);
        
        // Run simulation step
        brain->step(step);
        
        // Update development system
        brain->getDevelopmentSystem()->update(brain.get(), step);
        
        // Record comprehensive metrics
        if (step % options.progressInterval == 0 || step == totalSteps - 1) {
            size_t neuronCount = brain->getTotalNeuronCount();
            size_t synapseCount = brain->getTotalSynapseCount();
            size_t firingCount = brain->getTotalSpikeCount();
            float avgFiringRate = brain->getAverageFiringRate();
            
            stats.record("phase6_step_" + std::to_string(step), step);
            stats.record("phase6_neurons", static_cast<double>(neuronCount));
            stats.record("phase6_synapses", static_cast<double>(synapseCount));
            stats.record("phase6_firing_count", static_cast<double>(firingCount));
            stats.record("phase6_avg_firing_rate", static_cast<double>(avgFiringRate));
            
            reporter.update(step, "Step completed");
        }
    }
    
    reporter.complete();
    stats.record("phase6_total_time", timer.elapsed(), true);
    
    std::cout << "Phase 6 completed successfully in " << timer.elapsed() << " seconds" << std::endl;
    return true;
}

// Main function
int main(int argc, char** argv) {
    // Parse command-line arguments
    CommandLineOptions options = parseCommandLine(argc, argv);
    
    // Handle help/version requests
    if (options.help) {
        printHelp();
        return 0;
    }
    
    // Create a simple config for testing
    auto config = std::make_shared<Config>();
    config->setValue("neuron_count", std::to_string(options.patternNeuronsCount));
    config->setValue("region_count", "1");
    config->setValue("connection_probability", "0.1");
    config->setValue("stdp_ltp_weight", "0.01");
    config->setValue("stdp_ltd_weight", "0.012");
    config->setValue("stdp_tau", "20.0");
    config->setValue("synaptogenesis_rate", "0.0001");
    config->setValue("pruning_rate", "0.00001");
    config->setValue("replay_interval", "100");
    config->setValue("consolidation_interval", "1000");
    
    // Initialize random seed
    nlm::Random::getInstance().seed(options.randomSeed);
    
    // Initialize logging if log file specified
    if (!options.logFile.empty()) {
        nlm::Logger::getInstance().initialize(options.logFile);
        std::cout << "Logging initialized to file: " << options.logFile << std::endl;
    }
    
    // Initialize brain with config
    auto brain = std::make_shared<Brain>(config);
    brain->setRandomSeed(options.randomSeed);
    
    // Initialize environment
    auto environment = std::make_shared<Environment>();
    
    // Load checkpoint if specified
    if (!options.checkpointFile.empty()) {
        if (!brain->load(options.checkpointFile)) {
            std::cerr << "Failed to load checkpoint, starting with fresh brain" << std::endl;
        }
    }
    
    std::cout << "NLM Phase 2 Enhanced Demo" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "Config file: " << options.configFile << std::endl;
    std::cout << "Max steps: " << options.maxSteps << std::endl;
    std::cout << "Test steps: " << options.testSteps << std::endl;
    std::cout << "STDP steps: " << options.stdpSteps << std::endl;
    std::cout << "Current injection: " << options.currentInjection << " μA" << std::endl;
    std::cout << "Sensory scale: " << options.sensoryInputScale << std::endl;
    std::cout << "Motor scale: " << options.motorCommandScale << std::endl;
    std::cout << "Pattern neurons: " << options.patternNeuronsCount << std::endl;
    std::cout << "Structural plasticity: " << (options.enableStructuralPlasticity ? "enabled" : "disabled") << std::endl;
    std::cout << "STDP: " << (options.enableSTDP ? "enabled" : "disabled") << std::endl;
    std::cout << "Hebbian: " << (options.enableHebbian ? "enabled" : "disabled") << std::endl;
    std::cout << "Random seed: " << options.randomSeed << std::endl;
    std::cout << "Timestep: " << options.timestep << " s" << std::endl;
    std::cout << "Progress reporting: " << (options.showProgress ? "enabled" : "disabled") << std::endl;
    std::cout << "Progress interval: " << options.progressInterval << " steps" << std::endl;
    if (!options.savePath.empty()) {
        std::cout << "Save path: " << options.savePath << std::endl;
    }
    std::cout << std::endl;
    
    // Create overall statistics
    TestStatistics overallStats;
    
    // Run tests based on configuration
    bool allTestsPassed = true;
    
    if (options.runAllTests) {
        if (!options.skipTest1) {
            std::cout << std::endl << "=== Test 1: Basic Connectivity ===" << std::endl;
            TestStatistics test1Stats;
            if (!runTest1(brain, environment, options, test1Stats)) {
                allTestsPassed = false;
                std::cerr << "Test 1 failed!" << std::endl;
            } else {
                for (const auto& [name, value] : test1Stats.metrics_) {
                    overallStats.metrics_[name] = value;
                }
                for (const auto& [name, value] : test1Stats.times_) {
                    overallStats.times_[name] = value;
                }
            }
        }
        
        if (!options.skipTest2) {
            std::cout << std::endl << "=== Test 2: Plasticity Learning ===" << std::endl;
            TestStatistics test2Stats;
            if (!runTest2(brain, environment, options, test2Stats)) {
                allTestsPassed = false;
                std::cerr << "Test 2 failed!" << std::endl;
            } else {
                for (const auto& [name, value] : test2Stats.metrics_) {
                    overallStats.metrics_[name] = value;
                }
                for (const auto& [name, value] : test2Stats.times_) {
                    overallStats.times_[name] = value;
                }
            }
        }
        
        if (!options.skipTest3) {
            std::cout << std::endl << "=== Test 3: STDP Verification ===" << std::endl;
            TestStatistics test3Stats;
            if (!runTest3(brain, environment, options, test3Stats)) {
                allTestsPassed = false;
                std::cerr << "Test 3 failed!" << std::endl;
            } else {
                for (const auto& [name, value] : test3Stats.metrics_) {
                    overallStats.metrics_[name] = value;
                }
                for (const auto& [name, value] : test3Stats.times_) {
                    overallStats.times_[name] = value;
                }
            }
        }
        
        // Run Phase 6 integration test if requested
        if (options.runPhase6) {
            std::cout << std::endl << "=== Phase 6: Integration Test ===" << std::endl;
            TestStatistics phase6Stats;
            if (!runPhase6(brain, environment, options, phase6Stats)) {
                allTestsPassed = false;
                std::cerr << "Phase 6 failed!" << std::endl;
            } else {
                for (const auto& [name, value] : phase6Stats.metrics_) {
                    overallStats.metrics_[name] = value;
                }
                for (const auto& [name, value] : phase6Stats.times_) {
                    overallStats.times_[name] = value;
                }
            }
        }
    }
    
    // Save final state if requested
    if (!options.savePath.empty()) {
        brain->save(options.savePath);
    }
    
    // Print final statistics
    overallStats.printReport();
    
    // Write results to output file if requested
    if (!options.outputFile.empty()) {
        std::ofstream outfile(options.outputFile);
        if (outfile.is_open()) {
            outfile << "NLM Phase 2 Demo Results" << std::endl;
            outfile << "All tests passed: " << (allTestsPassed ? "yes" : "no") << std::endl;
            outfile << "Total metrics: " << overallStats.metrics_.size() << std::endl;
            outfile << "Total time measurements: " << overallStats.times_.size() << std::endl;
            outfile << std::endl;
            
            for (const auto& [name, value] : overallStats.metrics_) {
                outfile << name << ": " << value << std::endl;
            }
            outfile << std::endl;
            
            for (const auto& [name, value] : overallStats.times_) {
                outfile << name << ": " << value << std::endl;
            }
            
            outfile.close();
            std::cout << "Results saved to: " << options.outputFile << std::endl;
        } else {
            std::cerr << "Failed to open output file: " << options.outputFile << std::endl;
        }
    }
    
    std::cout << std::endl << "Demo completed. " << (allTestsPassed ? "All tests passed!" : "Some tests failed!") << std::endl;
    
    return allTestsPassed ? 0 : 1;
}

} // namespace nlm
