// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// Enhanced Version with Comprehensive CLI and Configuration Improvements
// Maintains backward compatibility with original functionality

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
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <map>
#include <cctype>
#include <cstdlib>

using namespace nlm;

// Forward declarations for helper classes
class CLIParser;
class UserFeedback;
class PerformanceMonitor;
class EnhancedExperimentRunner;

// Global references for easy access
static CLIParser* g_cliParser = nullptr;
static UserFeedback* g_userFeedback = nullptr;
static PerformanceMonitor* g_performanceMonitor = nullptr;

// Enhanced CLI argument parsing utilities
class CLIParser {
public:
    struct Arguments {
        std::string configFile = "configs/default.cfg";
        std::string experimentMode = "default";
        std::string outputFile = "";
        size_t demoEpisodes = 5;
        size_t demoStepsPerEpisode = 200;
        size_t benchmarkIterations = 100;
        double simulationTimestep = 0.001;
        size_t neuronCount = 500;
        size_t regionCount = 1;
        float connectionProbability = 0.15f;
        bool enableDebug = false;
        bool enablePerformanceMonitoring = false;
        bool enableValidation = false;
        bool exportToJSON = false;
        std::string logLevel = "info";
        std::vector<std::string> extraArgs;
        bool showHelp = false;
        bool version = false;
        std::string helpTopic = "";
    };

    static Arguments parse(int argc, char** argv) {
        Arguments args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle help and version flags
            if (arg == "--help" || arg == "-h") {
                args.showHelp = true;
            } else if (arg == "--version" || arg == "-v") {
                args.version = true;
            } else if (arg == "--help=" || arg.find("--help=") == 0) {
                if (arg.find("=") != std::string::npos) {
                    args.helpTopic = arg.substr(arg.find("=") + 1);
                }
            }
            // Handle --key=value format
            else if (arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setArgument(args, key, value);
            }
            // Handle --key value format
            else if (arg.substr(0, 2) == "--" && i + 1 < argc && 
                    argv[i + 1][0] != '-') {
                std::string key = arg.substr(2);
                std::string value = argv[++i];
                setArgument(args, key, value);
            }
            // Handle -k value format
            else if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-' &&
                    i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                setArgument(args, key, value);
            }
            // Handle boolean flags
            else if (arg == "--debug" || arg == "-d") {
                args.enableDebug = true;
            } else if (arg == "--enable-performance-monitoring" || arg == "--performance") {
                args.enablePerformanceMonitoring = true;
            } else if (arg == "--enable-validation" || arg == "--validate") {
                args.enableValidation = true;
            } else if (arg == "--export-json" || arg == "--export") {
                args.exportToJSON = true;
            }
            // Store extra arguments
            else if (arg.substr(0, 2) == "--") {
                args.extraArgs.push_back(arg);
            }
            else {
                args.extraArgs.push_back(arg);
            }
        }
        
        return args;
    }
    
private:
    static void setArgument(Arguments& args, const std::string& key, const std::string& value) {
        if (key == "config" || key == "c") {
            args.configFile = value;
        }
        else if (key == "mode" || key == "m") {
            args.experimentMode = value;
        }
        else if (key == "output" || key == "o") {
            args.outputFile = value;
        }
        else if (key == "demo-episodes" || key == "ep") {
            args.demoEpisodes = static_cast<size_t>(std::stoi(value));
        }
        else if (key == "demo-steps" || key == "steps") {
            args.demoStepsPerEpisode = static_cast<size_t>(std::stoi(value));
        }
        else if (key == "benchmark-iterations" || key == "iter") {
            args.benchmarkIterations = static_cast<size_t>(std::stoi(value));
        }
        else if (key == "simulation-timestep" || key == "dt") {
            args.simulationTimestep = std::stod(value);
        }
        else if (key == "neuron-count" || key == "n") {
            args.neuronCount = static_cast<size_t>(std::stoi(value));
        }
        else if (key == "region-count" || key == "r") {
            args.regionCount = static_cast<size_t>(std::stoi(value));
        }
        else if (key == "connection-probability" || key == "p") {
            args.connectionProbability = std::stof(value);
        }
        else if (key == "log-level" || key == "log") {
            args.logLevel = value;
        }
        else {
            // Store unknown arguments for backward compatibility
            args.extraArgs.push_back("--" + key + "=" + value);
        }
    }
};

// Enhanced error handling and user feedback system
class UserFeedback {
public:
    static void showProgress(const std::string& message, int current, int total) {
        if (current % (total / 10) == 0 || current == total) {
            std::cout << "\rProgress: " << current << "/" << total << " " << message << std::flush;
            if (current == total) {
                std::cout << std::endl;
            }
        }
    }
    
    static void showError(const std::string& error, bool isFatal = false, bool isWarning = false) {
        if (isWarning) {
            std::cout << "\033[1;33mWARNING: " << error << "\033[0m" << std::endl;
        } else {
            std::cout << "\033[1;31mERROR: " << error << "\033[0m" << std::endl;
        }
        if (isFatal) {
            std::cout << "\033[1;31mFatal error occurred. Exiting.\033[0m" << std::endl;
            exit(1);
        }
    }
    
    static void showSuccess(const std::string& message) {
        std::cout << "\033[1;32m✓ " << message << "\033[0m" << std::endl;
    }
    
    static void showInfo(const std::string& message) {
        std::cout << "\033[1;36mINFO: " << message << "\033[0m" << std::endl;
    }
    
    static void showHeader(const std::string& title) {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << std::string(70, '=') << std::endl;
    }
    
    static void showSection(const std::string& title) {
        std::cout << "\n" << std::string(50, '-') << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << std::string(50, '-') << std::endl;
    }
    
    static void showFooter(const std::string& message) {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "  " << message << std::endl;
        std::cout << std::string(70, '=') << std::endl << std::endl;
    }
    
    static void showHelp() {
        showHeader("NLM Enhanced CLI Help");
        std::cout << "Enhanced Neural Learning Machine - Phase 2\n\n";
        std::cout << "USAGE:\n";
        std::cout << "  nlm [options]\n\n";
        std::cout << "EXPERIMENT MODES:\n";
        std::cout << "  default    - Run original Phase 2 tests (connectivity, plasticity, STDP)\n";
        std::cout << "  demo       - Run Phase 3 world interaction demo\n";
        std::cout << "  phase3     - Run Phase 3 world interaction demo\n";
        std::cout << "  phase4     - Run Phase 4 emerging cognition experiments\n";
        std::cout << "  benchmark  - Run performance benchmarks\n";
        std::cout << "  experiment - Run configurable experiment suite\n\n";
        std::cout << "CORE OPTIONS:\n";
        std::cout << "  --help, -h                    Show this help message\n";
        std::cout << "  --version, -v                Show version information\n";
        std::cout << "  --config, -c <file>           Configuration file path\n";
        std::cout << "  --mode, -m <mode>             Operation mode (default, demo, phase3, phase4, benchmark, experiment)\n";
        std::cout << "  --output, -o <file>           Output file for results\n\n";
        std::cout << "DEMO/BENCHMARK OPTIONS:\n";
        std::cout << "  --demo-episodes, -ep <n>      Number of demo episodes (default: 5)\n";
        std::cout << "  --demo-steps, -steps <n>      Steps per demo episode (default: 200)\n";
        std::cout << "  --benchmark-iterations, -iter <n>  Number of benchmark iterations (default: 100)\n\n";
        std::cout << "SIMULATION OPTIONS:\n";
        std::cout << "  --simulation-timestep, -dt <f>  Simulation timestep (default: 0.001)\n";
        std::cout << "  --neuron-count, -n <n>        Number of neurons (default: 500)\n";
        std::cout << "  --region-count, -r <n>        Number of regions (default: 1)\n";
        std::cout << "  --connection-probability, -p <f>  Connection probability (default: 0.15)\n\n";
        std::cout << "DEBUG AND VALIDATION OPTIONS:\n";
        std::cout << "  --debug, -d                   Enable debug mode\n";
        std::cout << "  --enable-performance-monitoring, --performance  Enable performance monitoring\n";
        std::cout << "  --enable-validation, --validate  Enable configuration validation\n";
        std::cout << "  --export-json, --export        Export configuration to JSON\n";
        std::cout << "  --log-level, -log <level>     Log level (debug, info, warning, error)\n\n";
        std::cout << "EXAMPLES:\n";
        std::cout << "  nlm --mode demo --demo-episodes 10\n";
        std::cout << "  nlm --mode benchmark --benchmark-iterations 1000\n";
        std::cout << "  nlm --mode phase4 --neuron-count 1000 --region-count 3\n";
        std::cout << "  nlm --config myconfig.cfg --mode experiment --enable-validation\n";
    }
    
    static void showVersion() {
        showHeader("NLM Version Information");
        std::cout << "Neural Learning Machine (NLM)\n";
        std::cout << "Phase 2: Real Neural Computation\n";
        std::cout << "Version: 0.1.0\n";
        std::cout << "Build Date: " << __DATE__ << " " << __TIME__ << "\n";
        std::cout << "C++ Standard: C++20\n";
        std::cout << "\nEnhanced Configuration System:\n";
        std::cout << "  Type-safe API with validation\n";
        std::cout << "  JSON configuration support\n";
        std::cout << "  Schema constraints and dependencies\n";
        std::cout << "  Comprehensive error handling\n";
        std::cout << "\nEnhanced CLI Features:\n";
        std::cout << "  Comprehensive argument parsing\n";
        std::cout << "  Multiple operation modes\n";
        std::cout << "  Performance monitoring\n";
        std::cout << "  Enhanced user feedback\n";
    }
    
    static void showProgressBar(int current, int total, const std::string& message) {
        int barWidth = 50;
        float progress = static_cast<float>(current) / total;
        int pos = static_cast<int>(barWidth * progress);
        
        std::cout << "\r[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100) << "% " << message;
        
        if (current == total) {
            std::cout << std::endl;
        }
    }
    
    static std::string formatTime(double seconds) {
        if (seconds < 60) {
            return std::to_string(seconds) + "s";
        } else if (seconds < 3600) {
            int minutes = static_cast<int>(seconds / 60);
            double remainingSeconds = seconds - (minutes * 60);
            return std::to_string(minutes) + "m " + std::to_string(remainingSeconds) + "s";
        } else {
            int hours = static_cast<int>(seconds / 3600);
            int minutes = static_cast<int>((seconds - (hours * 3600)) / 60);
            double remainingSeconds = seconds - (hours * 3600) - (minutes * 60);
            return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(remainingSeconds) + "s";
        }
    }
    
    static std::string formatMemory(double bytes) {
        if (bytes < 1024) {
            return std::to_string(bytes) + " B";
        } else if (bytes < 1024 * 1024) {
            return std::to_string(bytes / 1024) + " KB";
        } else if (bytes < 1024 * 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024)) + " MB";
        } else {
            return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
        }
    }
};

// Performance monitoring and metrics collection
class PerformanceMonitor {
public:
    struct Metrics {
        double cpuTime = 0.0;
        double wallTime = 0.0;
        size_t neuronsProcessed = 0;
        size_t synapsesCreated = 0;
        size_t spikesGenerated = 0;
        double memoryUsage = 0.0;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::vector<double> memorySnapshots;
    };
    
    static void startMonitoring() {
        instance().startTime = std::chrono::high_resolution_clock::now();
        instance().memorySnapshots.clear();
    }
    
    static void endMonitoring() {
        instance().endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(instance().endTime - instance().startTime);
        instance().wallTime = duration.count() / 1000.0;
        
        instance().memorySnapshots.push_back(instance().memoryUsage);
    }
    
    static void incrementNeurons(size_t count) {
        instance().neuronsProcessed += count;
    }
    
    static void incrementSynapses(size_t count) {
        instance().synapsesCreated += count;
    }
    
    static void incrementSpikes(size_t count) {
        instance().spikesGenerated += count;
    }
    
    static void recordMemoryUsage(double usage) {
        instance().memoryUsage = usage;
    }
    
    static void report(const Config& config) {
        UserFeedback::showSection("Performance Metrics");
        
        double totalTime = instance().wallTime;
        if (totalTime > 0) {
            double neuronsPerSecond = instance().neuronsProcessed / totalTime;
            double synapsesPerSecond = instance().synapsesCreated / totalTime;
            double spikesPerSecond = instance().spikesGenerated / totalTime;
            
            std::cout << "  Processing Speed:\n";
            std::cout << "    Neurons processed: " << instance().neuronsProcessed << " (" << std::fixed << std::setprecision(2) << neuronsPerSecond << " neurons/sec)\n";
            std::cout << "    Synapses created: " << instance().synapsesCreated << " (" << std::fixed << std::setprecision(2) << synapsesPerSecond << " synapses/sec)\n";
            std::cout << "    Spikes generated: " << instance().spikesGenerated << " (" << std::fixed << std::setprecision(2) << spikesPerSecond << " spikes/sec)\n";
        }
        
        if (config.has("neuron_count")) {
            size_t neuronCount = config.get<size_t>("neuron_count").value();
            double simulationTime = config.get<double>("simulation_timestep").value();
            
            std::cout << "  Configuration:\n";
            std::cout << "    Neurons: " << neuronCount << "\n";
            std::cout << "    Simulation timestep: " << simulationTime << "s\n";
            std::cout << "    Memory usage: " << UserFeedback::formatMemory(instance().memoryUsage) << "\n";
            std::cout << "    Total simulation time: " << UserFeedback::formatTime(totalTime) << "\n";
        }
        
        // Memory trend
        if (instance().memorySnapshots.size() > 1) {
            double initialMem = instance().memorySnapshots.front();
            double finalMem = instance().memorySnapshots.back();
            double memChange = finalMem - initialMem;
            std::cout << "    Memory change: " << UserFeedback::formatMemory(memChange) << "\n";
        }
    }
    
    static void snapshotMemoryUsage(const Config& config) {
        double usage = config.getMemoryUsage();
        instance().memorySnapshots.push_back(usage);
    }
    
    static void reset() {
        instance().wallTime = 0.0;
        instance().neuronsProcessed = 0;
        instance().synapsesCreated = 0;
        instance().spikesGenerated = 0;
        instance().memoryUsage = 0.0;
        instance().memorySnapshots.clear();
    }
    
private:
    static PerformanceMonitor& instance() {
        static PerformanceMonitor monitor;
        return monitor;
    }
    
    PerformanceMonitor() {}
};

// Enhanced experiment runner with support for multiple modes
class EnhancedExperimentRunner {
public:
    static void runDefaultExperiments(std::shared_ptr<Brain> brain, const Config& config, 
                                      const CLIParser::Arguments& args) {
        UserFeedback::showSection("Enhanced Default Experiments (Backward Compatible)");
        
        // Run all three original tests for backward compatibility
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain, args.demoEpisodes);
        
        brain->reset();
        brain->initialize();
        
        runStdpVerification(brain);
    }
    
    static void runPhase3Demo(std::shared_ptr<Brain> brain, const Config& config, 
                            size_t episodes, size_t stepsPerEpisode) {
        UserFeedback::showSection("Phase 3: World Interaction Demo");
        
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        auto configCopy = std::make_shared<Config>(config);
        
        // Configure environment
        configCopy->set("environment_width", 20);
        configCopy->set("environment_height", 20);
        configCopy->set("vision_width", 16);
        configCopy->set("vision_height", 16);
        
        // Configure brain
        configCopy->set("neuron_count", 500);
        configCopy->set("region_count", 2);
        configCopy->set("connection_probability", 0.1f);
        configCopy->set("random_seed", 42u);
        configCopy->set("simulation_timestep", 0.001);
        
        // Create brain
        auto demoBrain = std::make_shared<Brain>(configCopy);
        demoBrain->initialize();
        
        // Create simple world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->setRandomSeed(42);
        
        // Create agent brain interface
        auto agentBrain = std::make_shared<AgentBrain>(demoBrain);
        agentBrain->initialize(world);
        
        // Configure agent brain
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << demoBrain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << demoBrain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain->getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain->getMotorOutputSize() << "\n\n";
        
        // Run episodes
        for (size_t episode = 0; episode < episodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            world->reset();
            agentBrain->reset();
            demoBrain->reset();
            
            float episodeReward = 0.0f;
            
            for (size_t step = 0; step < stepsPerEpisode; ++step) {
                const SensoryPercept& percept = world->getSensoryPercept();
                agentBrain->processSensoryInput(percept);
                
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    demoBrain->step(neuralStep);
                }
                
                MotorCommand cmd = agentBrain->decodeMotorCommand();
                ActionResult result = world->applyMotorCommand(cmd, world->getSimulationTime());
                
                agentBrain->applyRewardModulation(result.reward, 0.0f);
                world->update(0.01);
                agentBrain->updateDevelopment(0.01);
                
                episodeReward += result.reward;
                PerformanceMonitor::incrementSpikes(demoBrain->getTotalSpikeCount());
            }
            
            std::cout << "--- Episode " << episode << " ended ---\n";
            std::cout << "  Total reward: " << episodeReward << "\n";
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
    }
    
    static void runPhase4Demo(std::shared_ptr<Brain> brain, const Config& config, 
                            size_t trials) {
        UserFeedback::showSection("Phase 4: Emerging Cognition");
        
        std::cout << "NLM Phase 4: Emerging Cognition\n";
        std::cout << "================================\n\n";
        
        auto configCopy = std::make_shared<Config>(config);
        
        configCopy->set("neuron_count", size_t(1000));
        configCopy->set("region_count", size_t(2));
        configCopy->set("connection_probability", 0.1f);
        configCopy->set("random_seed", uint64_t(42));
        configCopy->set("simulation_timestep", 0.001);
        
        // Create brain
        std::cout << "Initializing brain...\n";
        auto demoBrain = std::make_shared<Brain>(configCopy);
        if (!demoBrain->initialize()) {
            UserFeedback::showError("Failed to initialize brain!");
            return;
        }
        
        Phase4IntegratedExperiment experiment;
        Phase4Results results = experiment.run(demoBrain.get(), trials, 50);
        
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "Phase 4 Results:\n";
        std::cout << "  Total Reward: " << results.totalReward << "\n";
        std::cout << "  Episodes Stored: " << results.episodesStored << "\n";
        std::cout << "  Concepts Formed: " << results.conceptsFormed << "\n";
        std::cout << "  Planning Confidence: " << results.planningConfidence << "\n";
        std::cout << "  Body Awareness: " << results.bodyAwareness << "\n";
        std::cout << std::string(60, '=') << "\n";
    }
    
    static void runBenchmark(std::shared_ptr<Brain> brain, const Config& config, 
                           size_t iterations) {
        UserFeedback::showSection("Benchmark Mode");
        
        std::cout << "Running benchmark with " << iterations << " iterations...\n";
        
        auto configCopy = std::make_shared<Config>(config);
        configCopy->set("benchmark_iterations", static_cast<int64_t>(iterations));
        
        PerformanceMonitor::startMonitoring();
        
        for (size_t i = 0; i < iterations; ++i) {
            auto testBrain = std::make_shared<Brain>(configCopy);
            if (testBrain->initialize()) {
                PerformanceMonitor::incrementNeurons(testBrain->getTotalNeuronCount());
                PerformanceMonitor::incrementSynapses(testBrain->getTotalSynapseCount());
                PerformanceMonitor::incrementSpikes(testBrain->getTotalSpikeCount());
            }
            UserFeedback::showProgressBar(i + 1, iterations, "Benchmark iteration");
        }
        
        PerformanceMonitor::endMonitoring();
        PerformanceMonitor::report(config);
    }
    
    static void runExperimentSuite(std::shared_ptr<Brain> brain, const Config& config, 
                                  const CLIParser::Arguments& args) {
        UserFeedback::showSection("Experiment Suite Mode");
        
        std::cout << "Running comprehensive experiment suite...\n";
        std::cout << "Parameters:\n";
        std::cout << "  Neurons: " << args.neuronCount << "\n";
        std::cout << "  Regions: " << args.regionCount << "\n";
        std::cout << "  Connection probability: " << args.connectionProbability << "\n";
        std::cout << "  Simulation timestep: " << args.simulationTimestep << "s\n";
        std::cout << "  Episodes: " << args.demoEpisodes << "\n";
        
        // Run a series of experiments
        PerformanceMonitor::startMonitoring();
        
        // Experiment 1: Basic connectivity
        UserFeedback::showSection("Experiment 1: Basic Connectivity Test");
        runBasicConnectivityTest(brain);
        
        // Experiment 2: Plasticity learning
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain, args.demoEpisodes);
        
        // Experiment 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        // Experiment 4: Phase 3 demo
        brain->reset();
        brain->initialize();
        runPhase3Demo(brain, config, 2, 100);
        
        PerformanceMonitor::endMonitoring();
        PerformanceMonitor::report(config);
        
        UserFeedback::showSuccess("Experiment suite completed successfully!");
    }
    
    static void showExperimentMenu() {
        UserFeedback::showSection("Experiment Selection Menu");
        std::cout << "Select experiment mode:\n";
        std::cout << "  1. Default (backward compatible) - Original Phase 2 tests\n";
        std::cout << "  2. Phase 3 - World interaction demo\n";
        std::cout << "  3. Phase 4 - Emerging cognition experiments\n";
        std::cout << "  4. Benchmark - Performance testing\n";
        std::cout << "  5. Experiment Suite - Comprehensive testing\n";
        std::cout << "\n  Enter choice (1-5): ";
    }
}; 

// Original test functions for backward compatibility
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    UserFeedback::showSection("Test 1: Basic Neural Connectivity");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        UserFeedback::showError("No regions found in brain");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        UserFeedback::showError("No neurons found in brain");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    UserFeedback::showInfo("Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    UserFeedback::showInfo("Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        UserFeedback::showSuccess("Spikes propagate through network");
    } else {
        UserFeedback::showWarning("No spikes generated - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            UserFeedback::showInfo("  Neuron " + std::to_string(i) + 
                               " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                               " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain, size_t demoEpisodes) {
    UserFeedback::showSection("Test 2: Plasticity Learning Experiment");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    UserFeedback::showInfo("Applying repeated input patterns...");
    
    for (size_t episode = 0; episode < demoEpisodes; ++episode) {
        UserFeedback::showInfo("  Episode " + std::to_string(episode) + ": Applying input patterns (100 steps)...");
        
        for (SimulationStep step = 0; step < 100; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
            
            // Log progress every 50 steps
            if (step % 50 == 0) {
                UserFeedback::showInfo("    Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        PerformanceMonitor::incrementSpikes(brain->getTotalSpikeCount());
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    UserFeedback::showSection("Test 3: STDP Verification");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        UserFeedback::showWarning("Not enough synapses for STDP test");
        return;
    }
    
    UserFeedback::showInfo("Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        UserFeedback::showInfo("    Synapse " + std::to_string(i) + 
                           " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    UserFeedback::showInfo("Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    UserFeedback::showInfo("After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        UserFeedback::showInfo("    Synapse " + std::to_string(i) + 
                           " after: " + std::to_string(synapses[i]->getWeight()) +
                           " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    if (totalDelta > 0.001f) {
        UserFeedback::showSuccess("STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        UserFeedback::showWarning("STDP reversed: Check parameters");
    } else {
        UserFeedback::showWarning("No change: STDP may not be triggering");
    }
}

// Enhanced configuration setup with validation
void setupConfiguration(Config& config, const CLIParser::Arguments& args) {
    UserFeedback::showInfo("Setting up enhanced configuration system...");
    
    // Load configuration from file
    if (config.loadFromFile(args.configFile)) {
        UserFeedback::showSuccess("Loaded configuration from: " + args.configFile);
    } else {
        UserFeedback::showWarning("Using default configuration (file not found: " + args.configFile + ")");
    }
    
    // Override with command line arguments
    if (!args.extraArgs.empty()) {
        // Convert extra args to argc/argv format for config->loadFromArgs
        int argc = args.extraArgs.size() + 1;
        char** argv = new char*[argc];
        argv[0] = new char[6];
        std::strcpy(argv[0], "nlm");
        
        for (size_t i = 0; i < args.extraArgs.size(); ++i) {
            std::string arg = args.extraArgs[i];
            argv[i + 1] = new char[arg.length() + 1];
            std::strcpy(argv[i + 1], arg.c_str());
        }
        
        config.loadFromArgs(argc, argv);
        
        // Clean up
        delete[] argv[0];
        for (size_t i = 1; i < static_cast<size_t>(argc); ++i) {
            delete[] argv[i];
        }
        delete[] argv;
    }
    
    // Set defaults based on operation mode and command line overrides
    if (args.experimentMode == "demo") {
        config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config.set("connection_probability", 0.15f, ConfigSource::Default);
        config.set("simulation_timestep", args.simulationTimestep, ConfigSource::Default);
    }
    else if (args.experimentMode == "benchmark") {
        config.set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(args.regionCount), ConfigSource::Default);
        config.set("connection_probability", args.connectionProbability, ConfigSource::Default);
        config.set("simulation_timestep", args.simulationTimestep, ConfigSource::Default);
        config.set("benchmark_iterations", static_cast<int64_t>(args.benchmarkIterations), ConfigSource::Default);
    }
    else if (args.experimentMode == "experiment") {
        config.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config.set("connection_probability", 0.1f, ConfigSource::Default);
        config.set("simulation_timestep", args.simulationTimestep, ConfigSource::Default);
    }
    
    // STDP parameters
    config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config.set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Enable/disable features based on configuration
    if (args.experimentMode == "demo") {
        config.set("enable_stdp", true, ConfigSource::Default);
        config.set("enable_hebbian", true, ConfigSource::Default);
        config.set("enable_structural_plasticity", true, ConfigSource::Default);
    }
    else if (args.experimentMode == "benchmark") {
        config.set("enable_stdp", true, ConfigSource::Default);
        config.set("enable_hebbian", true, ConfigSource::Default);
        config.set("enable_structural_plasticity", false, ConfigSource::Default);
    }
    else if (args.experimentMode == "experiment") {
        config.set("enable_stdp", true, ConfigSource::Default);
        config.set("enable_hebbian", true, ConfigSource::Default);
        config.set("enable_structural_plasticity", true, ConfigSource::Default);
    }
    
    // Set log level
    if (args.logLevel == "debug") {
        config.set("log_level", "debug", ConfigSource::Default);
    }
    else if (args.logLevel == "warning") {
        config.set("log_level", "warning", ConfigSource::Default);
    }
    else if (args.logLevel == "error") {
        config.set("log_level", "error", ConfigSource::Default);
    }
    
    // Validate configuration if requested
    if (args.enableValidation) {
        UserFeedback::showInfo("Validating configuration...");
        if (config.validate()) {
            UserFeedback::showSuccess("Configuration validation passed.");
        } else {
            UserFeedback::showError("Configuration validation failed.");
            auto errors = config.validateAll();
            for (const auto& error : errors) {
                UserFeedback::showError("  " + error);
            }
            UserFeedback::showError("Configuration validation errors found. Use --enable-validation for details.", true);
        }
    }
    
    // Show configuration summary
    UserFeedback::showInfo("Configuration Summary:");
    UserFeedback::showInfo("  " + config.summary());
    
    // Export to JSON if requested
    if (args.exportToJSON) {
        std::string jsonStr = config.exportToJSON();
        if (!args.outputFile.empty()) {
            std::ofstream outfile(args.outputFile);
            outfile << jsonStr;
            UserFeedback::showSuccess("Configuration exported to JSON file: " + args.outputFile);
        } else {
            UserFeedback::showInfo("Configuration as JSON:\n" + jsonStr);
        }
    }
}

int main(int argc, char** argv) {
    try {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Initialize global references
        g_cliParser = nullptr; // CLIParser object is created on stack
        g_userFeedback = nullptr; // UserFeedback object is created on stack
        g_performanceMonitor = nullptr; // PerformanceMonitor object is created on stack
        
        // Parse command line arguments
        CLIParser::Arguments args = CLIParser::parse(argc, argv);
        
        // Initialize global references
        g_cliParser = nullptr;
        g_userFeedback = nullptr;
        g_performanceMonitor = nullptr;
        
        // Handle help and version
        if (args.showHelp) {
            UserFeedback::showHelp();
            return 0;
        }
        
        if (args.version) {
            UserFeedback::showVersion();
            return 0;
        }
        
        // Show banner for backward compatibility
        if (args.experimentMode == "default" || args.experimentMode == "demo" || 
            args.experimentMode == "benchmark" || args.experimentMode == "experiment") {
            printBanner();
        }
        
        UserFeedback::showHeader("NLM - Enhanced Neural Learning Machine");
        UserFeedback::showInfo("Starting NLM with enhanced CLI and configuration system...");
        
        // Setup configuration
        auto config = std::make_shared<Config>();
        setupConfiguration(*config, args);
        
        // Initialize brain
        UserFeedback::showInfo("Initializing NLM Brain...");
        PerformanceMonitor::startMonitoring();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            UserFeedback::showError("Failed to initialize brain!", true);
        }
        
        brain->logStatus();
        
        // Run based on experiment mode
        if (args.experimentMode == "demo") {
            UserFeedback::showInfo("Running in DEMO mode with " + std::to_string(args.demoEpisodes) + " episodes...");
            
            brain->reset();
            brain->initialize();
            runBasicConnectivityTest(brain);
            
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain, args.demoEpisodes);
            
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
        }
        else if (args.experimentMode == "phase3") {
            UserFeedback::showInfo("Running Phase 3 World Interaction Demo...");
            brain->reset();
            brain->initialize();
            EnhancedExperimentRunner::runPhase3Demo(brain, *config, args.demoEpisodes, args.demoStepsPerEpisode);
        }
        else if (args.experimentMode == "phase4") {
            UserFeedback::showInfo("Running Phase 4 Emerging Cognition Demo...");
            EnhancedExperimentRunner::runPhase4Demo(brain, *config, args.demoEpisodes);
        }
        else if (args.experimentMode == "benchmark") {
            UserFeedback::showInfo("Running benchmark with " + std::to_string(args.benchmarkIterations) + " iterations...");
            EnhancedExperimentRunner::runBenchmark(brain, *config, args.benchmarkIterations);
        }
        else if (args.experimentMode == "experiment") {
            UserFeedback::showInfo("Running comprehensive experiment suite...");
            EnhancedExperimentRunner::runExperimentSuite(brain, *config, args);
        }
        else {
            // Default behavior for backward compatibility
            UserFeedback::showInfo("Running default experiment mode (backward compatible)...");
            EnhancedExperimentRunner::runDefaultExperiments(brain, *config, args);
        }
        
        // Final brain status
        UserFeedback::showSection("Final Brain Status");
        brain->logStatus();
        
        UserFeedback::showFooter("NLM Phase 2 Complete! Enhanced CLI and configuration system working successfully.");
        
        PerformanceMonitor::report(*config);
        
        return 0;
        
    } catch (const std::exception& e) {
        UserFeedback::showError("Unexpected error: " + std::string(e.what()), true);
    } catch (...) {
        UserFeedback::showError("Unknown error occurred.", true);
    }
    
    return 1;
}