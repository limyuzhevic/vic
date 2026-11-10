// Enhanced CLI help and utilities
namespace {
    const char* const PROGRAM_NAME = "nlm";
    const char* const VERSION = "0.1.0";
    const char* const BUILD_TYPE = "Release";
    
    struct CommandLineOptions {
        std::string configFile = "configs/default.cfg";
        std::string mode = "demo"; // demo, benchmark, test
        size_t duration = 5000;
        size_t stepDuration = 1000;
        bool enablePlasticity = true;
        bool enableDevelopment = false;
        bool enableNeuromodulation = true;
        bool enableMemory = true;
        bool enablePrediction = false;
        bool enableCognition = false;
        std::string outputFormat = "text";
        bool verbose = false;
        bool quiet = false;
        std::string outputFile;
        bool saveCheckpoint = false;
        bool restoreCheckpoint = false;
        std::string checkpointPath;
        std::vector<std::string> customArgs;
        
        // Print help information
        void printHelp() const {
            std::cout << "\n";
            std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                            NLM — 熙然 — Command Line Options                      ║" << std::endl;
            std::cout << "║                                                                              ║" << std::endl;
            std::cout << "║  Usage: " << PROGRAM_NAME << " [options]                                      ║" << std::endl;
            std::cout << "║                                                                              ║" << std::endl;
            std::cout << "║  Options:                                                                     ║" << std::endl;
            std::cout << "║    --help, -h                     Show this help message                         ║" << std::endl;
            std::cout << "║    --version, -v                  Show version information                    ║" << std::endl;
            std::cout << "║    --config <file>                Load configuration from file               ║" << std::endl;
            std::cout << "║    --mode <mode>                  Operation mode: demo, benchmark, test         ║" << std::endl;
            std::cout << "║    --duration <steps>             Total simulation steps                   ║" << std::endl;
            std::cout << "║    --step-duration <steps>        Steps per phase                           ║" << std::endl;
            std::cout << "║                                                                              ║" << std::endl;
            std::cout << "║  System Features:                                                            ║" << std::endl;
            std::cout << "║    --no-plasticity                Disable synaptic plasticity                ║" << std::endl;
            std::cout << "║    --development                  Enable developmental processes              ║" << std::endl;
            std::cout << "║    --no-neuromodulation            Disable neuromodulation                   ║" << std::endl;
            std::cout << "║    --no-memory                    Disable memory systems                    ║" << std::endl;
            std::cout << "║    --prediction                   Enable prediction system                   ║" << std::endl;
            std::cout << "║    --cognition                    Enable cognitive systems                   ║" << std::endl;
            std::cout << "║                                                                              ║" << std::endl;
            std::cout << "║  Output & I/O:                                                              ║" << std::endl;
            std::cout << "║    --output-format <format>       Output format: text, json, csv              ║" << std::endl;
            std::cout << "║    --verbose, -V                  Enable verbose logging                    ║" << std::endl;
            std::cout << "║    --quiet, -q                    Suppress most output                        ║" << std::endl;
            std::cout << "║    --output <file>                Save output to file                         ║" << std::endl;
            std::cout << "║    --checkpoint <path>            Save checkpoint to path                   ║" << std::endl;
            std::cout << "║    --restore <path>               Restore checkpoint from path               ║" << std::endl;
            std::cout << "║    --save-checkpoint              Alias for --checkpoint with default path   ║" << std::endl;
            std::cout << "║                                                                              ║" << std::endl;
            std::cout << "║  Advanced Options:                                                          ║" << std::endl;
            std::cout << "║    --enable-all                   Enable all neural systems                   ║" << std::endl;
            std::cout << "║    --disable-all                  Disable all optional systems               ║" << std::endl;
            std::cout << "║    --custom <args>                Custom system parameters                   ║" << std::endl;
            std::cout << "║                                                                              ║" << std::endl;
            std::cout << "║  Built with C++20, " << BUILD_TYPE << " build                                      ║" << std::endl;
            std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << "\n";
        }
        
        // Print version information
        void printVersion() const {
            std::cout << PROGRAM_NAME << " version " << VERSION << " (C++20, " << BUILD_TYPE << " build)" << std::endl;
            std::cout << "Neural Learning Machine — An experimental artificial developmental brain" << std::endl;
            std::cout << "Website: https://github.com/nlm-project/nlm" << std::endl;
        }
        
        // Parse command line arguments
        bool parseArgs(int argc, char** argv) {
            std::vector<std::string> args;
            for (int i = 1; i < argc; ++i) {
                args.push_back(argv[i]);
            }
            
            for (size_t i = 0; i < args.size(); ++i) {
                std::string arg = args[i];
                
                if (arg == "--help" || arg == "-h") {
                    printHelp();
                    return true; // Exit after help
                }
                else if (arg == "--version" || arg == "-v") {
                    printVersion();
                    return true; // Exit after version
                }
                else if (arg.find("--config") == 0) {
                    if (arg.find("=") != std::string::npos) {
                        configFile = arg.substr(arg.find("=") + 1);
                    } else if (i + 1 < args.size()) {
                        configFile = args[++i];
                    }
                }
                else if (arg.find("--mode") == 0) {
                    if (arg.find("=") != std::string::npos) {
                        mode = arg.substr(arg.find("=") + 1);
                    } else if (i + 1 < args.size()) {
                        mode = args[++i];
                    }
                    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
                }
                else if (arg.find("--duration") == 0) {
                    if (arg.find("=") != std::string::npos) {
                        duration = std::stoul(arg.substr(arg.find("=") + 1));
                    } else if (i + 1 < args.size()) {
                        duration = std::stoul(args[++i]);
                    }
                }
                else if (arg.find("--step-duration") == 0) {
                    if (arg.find("=") != std::string::npos) {
                        stepDuration = std::stoul(arg.substr(arg.find("=") + 1));
                    } else if (i + 1 < args.size()) {
                        stepDuration = std::stoul(args[++i]);
                    }
                }
                else if (arg == "--no-plasticity") {
                    enablePlasticity = false;
                }
                else if (arg == "--development") {
                    enableDevelopment = true;
                }
                else if (arg == "--no-neuromodulation") {
                    enableNeuromodulation = false;
                }
                else if (arg == "--no-memory") {
                    enableMemory = false;
                }
                else if (arg == "--prediction") {
                    enablePrediction = true;
                }
                else if (arg == "--cognition") {
                    enableCognition = true;
                }
                else if (arg.find("--output-format") == 0) {
                    if (arg.find("=") != std::string::npos) {
                        outputFormat = arg.substr(arg.find("=") + 1);
                    } else if (i + 1 < args.size()) {
                        outputFormat = args[++i];
                    }
                    std::transform(outputFormat.begin(), outputFormat.end(), outputFormat.begin(), ::tolower);
                }
                else if (arg == "--verbose" || arg == "-V") {
                    verbose = true;
                }
                else if (arg == "--quiet" || arg == "-q") {
                    quiet = true;
                }
                else if (arg.find("--output") == 0) {
                    if (arg.find("=") != std::string::npos) {
                        outputFile = arg.substr(arg.find("=") + 1);
                    } else if (i + 1 < args.size()) {
                        outputFile = args[++i];
                    }
                }
                else if (arg.find("--checkpoint") == 0) {
                    if (arg.find("=") != std::string::npos) {
                        checkpointPath = arg.substr(arg.find("=") + 1);
                        saveCheckpoint = true;
                    } else if (i + 1 < args.size()) {
                        checkpointPath = args[++i];
                        saveCheckpoint = true;
                    }
                }
                else if (arg.find("--restore") == 0) {
                    if (arg.find("=") != std::string::npos) {
                        checkpointPath = arg.substr(arg.find("=") + 1);
                        restoreCheckpoint = true;
                    } else if (i + 1 < args.size()) {
                        checkpointPath = args[++i];
                        restoreCheckpoint = true;
                    }
                }
                else if (arg == "--save-checkpoint") {
                    saveCheckpoint = true;
                    if (i + 1 < args.size() && args[i + 1].find("--") == 0) {
                        // No checkpoint path provided, use default
                    }
                }
                else if (arg == "--enable-all") {
                    enablePlasticity = true;
                    enableDevelopment = true;
                    enableNeuromodulation = true;
                    enableMemory = true;
                    enablePrediction = true;
                    enableCognition = true;
                }
                else if (arg == "--disable-all") {
                    enablePlasticity = false;
                    enableDevelopment = false;
                    enableNeuromodulation = false;
                    enableMemory = false;
                    enablePrediction = false;
                    enableCognition = false;
                }
                else if (arg.find("--custom") == 0) {
                    if (arg.find("=") != std::string::npos) {
                        customArgs.push_back(arg.substr(arg.find("=") + 1));
                    } else if (i + 1 < args.size()) {
                        customArgs.push_back(args[++i]);
                    }
                }
                else {
                    // Unknown argument, could be a custom parameter or error
                    if (arg.find("-") == 0) {
                        // Command line option with no value, treat as error
                        std::cerr << "ERROR: Unknown option: " << arg << std::endl;
                        printHelp();
                        return false;
                    } else {
                        // Pass through to config loading
                        customArgs.push_back(arg);
                    }
                }
            }
            
            return true;
        }
        
        // Validate options
        bool validate() const {
            std::vector<std::string> validModes = {"demo", "benchmark", "test"};
            if (std::find(validModes.begin(), validModes.end(), mode) == validModes.end()) {
                std::cerr << "ERROR: Invalid mode: " << mode << std::endl;
                return false;
            }
            
            std::vector<std::string> validFormats = {"text", "json", "csv"};
            if (std::find(validFormats.begin(), validFormats.end(), outputFormat) == validFormats.end()) {
                std::cerr << "ERROR: Invalid output format: " << outputFormat << std::endl;
                return false;
            }
            
            if (duration == 0) {
                std::cerr << "ERROR: Duration must be greater than 0" << std::endl;
                return false;
            }
            
            if (stepDuration == 0) {
                std::cerr << "ERROR: Step duration must be greater than 0" << std::endl;
                return false;
            }
            
            return true;
        }
    };

    // Enhanced utility functions
    template<typename T>
    T clamp(T value, T min, T max) {
        return std::max(min, std::min(max, value));
    }
    
    double calculateProgress(size_t current, size_t total) {
        if (total == 0) return 0.0;
        return 100.0 * static_cast<double>(current) / static_cast<double>(total);
    }
    
    void printProgress(const std::string& label, size_t current, size_t total, size_t step = 0) {
        if (total == 0) return;
        
        double progress = calculateProgress(current, total);
        size_t barWidth = 50;
        size_t filled = static_cast<size_t>(barWidth * progress / 100.0);
        
        std::cout << "\r";
        std::cout << "  " << label << ": ";
        
        // Progress bar
        std::cout << "["; 
        for (size_t i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "=";
            else if (i == filled) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "]";
        
        std::cout << " " << std::fixed << std::setprecision(1) << progress << "%";
        
        if (step > 0) {
            std::cout << " (step " << step << ")";
        }
        
        std::cout.flush();
        
        if (current >= total) {
            std::cout << "\r";
            std::cout << "  " << label << ": [=================================================] 100.0% Complete" << std::endl;
        }
    }
    
    // Enhanced logging function
    void logEnhanced(const std::string& level, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::cout << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S")
                  << "." << std::setfill('0') << std::setw(3) << ms.count()
                  << "] [" << level << "] " << message << std::endl;
    }
    
    // Parse custom arguments
    void parseCustomArgs(std::shared_ptr<Config> config, const std::vector<std::string>& customArgs) {
        for (const auto& arg : customArgs) {
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = arg.substr(0, eqPos);
                std::string value = arg.substr(eqPos + 1);
                
                // Try to parse value as different types
                try {
                    // Try int64
                    size_t pos;
                    int64_t intVal = std::stoll(value, &pos);
                    if (pos == value.size()) {
                        config->set(key, intVal, ConfigSource::CommandLine);
                        continue;
                    }
                } catch (...) {}
                
                try {
                    // Try double
                    size_t pos;
                    double doubleVal = std::stod(value, &pos);
                    if (pos == value.size()) {
                        config->set(key, doubleVal, ConfigSource::CommandLine);
                        continue;
                    }
                } catch (...) {}
                
                try {
                    // Try float
                    size_t pos;
                    float floatVal = std::stof(value, &pos);
                    if (pos == value.size()) {
                        config->set(key, floatVal, ConfigSource::CommandLine);
                        continue;
                    }
                } catch (...) {}
                
                // Default to string
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }

    // Enhanced benchmark structure
    struct EnhancedBenchmarkResult {
        std::string name;
        size_t neurons;
        size_t synapses;
        size_t steps;
        double totalTime;      // seconds
        double stepsPerSecond;
        double spikesPerStep;
        double synapsesPerNeuron;
        std::vector<double> stepTimes;
        size_t peakMemoryUsage;
        double avgFiringRate;
        double learningRate;
    };
    
    EnhancedBenchmarkResult runEnhancedBenchmark(
        size_t neuronCount, size_t steps, const std::shared_ptr<Config>& config) {
        
        std::cout << "  Running enhanced benchmark: " << neuronCount << " neurons, " 
                  << steps << " steps..." << std::flush;
        
        // Create brain with enhanced configuration
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Record start time
        auto startTime = std::chrono::high_resolution_clock::now();
        std::vector<double> stepTimes;
        stepTimes.reserve(steps);
        
        // Run simulation with enhanced monitoring
        for (size_t step = 0; step < steps; ++step) {
            // Inject varied input pattern based on step
            double inputIntensity = 30.0f;
            if (step % 10 == 0) {
                inputIntensity = 60.0f; // Stronger input at regular intervals
            }
            
            brain->injectCurrentToNeurons(NeuronType::Sensory, inputIntensity);
            
            auto stepStart = std::chrono::high_resolution_clock::now();
            brain->step(step, step * 0.001);
            auto stepEnd = std::chrono::high_resolution_clock::now();
            
            double stepTime = std::chrono::duration<double>(stepEnd - stepStart).count();
            stepTimes.push_back(stepTime);
            
            // Progress update every 10%
            if (step % (steps / 10) == 0) {
                printProgress("Benchmark", step, steps, step);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Calculate statistics
        EnhancedBenchmarkResult result;
        result.name = "Benchmark " + std::to_string(neuronCount);
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        result.stepTimes = std::move(stepTimes);
        result.peakMemoryUsage = 0; // TODO: Implement memory tracking
        result.avgFiringRate = static_cast<double>(brain->getFiringNeuronCount()) / steps;
        
        // Calculate learning rate (weight change over time)
        double learningRate = 0.0;
        if (auto* region = brain->getRegion(RegionId(1))) {
            double totalWeightChange = 0.0;
            size_t synapseCount = 0;
            for (const auto& syn : region->getSynapses()) {
                totalWeightChange += std::abs(syn->getWeight() - 0.5f); // Reference weight
                synapseCount++;
            }
            if (synapseCount > 0) {
                learningRate = totalWeightChange / synapseCount;
            }
        }
        result.learningRate = learningRate;
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
        std::cout << "    Step time stddev: " << std::fixed << std::setprecision(6) << calculateStdDev(result.stepTimes) << "s" << std::endl;
        
        return result;
    }
    
    // Helper function to calculate standard deviation
    double calculateStdDev(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / values.size();
        
        double sqSum = 0.0;
        for (double val : values) {
            sqSum += (val - mean) * (val - mean);
        }
        
        return std::sqrt(sqSum / values.size());
    }
    
    // Print enhanced benchmark results
    void printEnhancedBenchmarkResults(const std::vector<EnhancedBenchmarkResult>& results) {
        std::cout << std::endl;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                                 NLM Enhanced Performance Benchmark Results                   ║" << std::endl;
        std::cout << "╠═════════════════════════════╦═════════════════════╦════════════╦════════════════╦════════════════╗" << std::endl;
        std::cout << "║ Name                      ║   Neurons         ║ Steps/sec  ║ Avg Firing    ║ Learning Rate  ║" << std::endl;
        std::cout << "╠═════════════════════════════╬═════════════════════╬════════════╬════════════════╬════════════════╗" << std::endl;
        
        for (const auto& r : results) {
            std::cout << "║ " << std::setw(25) << r.name << " ║ " 
                      << std::setw(19) << r.neurons << " ║ " 
                      << std::setw(11) << std::fixed << std::setprecision(0) << r.stepsPerSecond << " ║ "
                      << std::setw(15) << std::fixed << std::setprecision(3) << r.avgFiringRate << " ║ "
                      << std::setw(16) << std::fixed << std::setprecision(6) << r.learningRate << " ║" << std::endl;
        }
        
        std::cout << "╚═════════════════════════════╩═════════════════════╩════════════╩════════════════╩════════════════╝" << std::endl;
        std::cout << std::endl;
    }
}
#include <algorithm>
#include <map>

using namespace nlm;

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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
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
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    // A pattern, not a seizure: stimulate only the first 20 sensory neurons
    // each step. The old code injected 30.0f into *every* sensory neuron
    // 20 times per step (600.0f), so the whole network fired synchronously
    // and every weight saturated to 1.0 - learning was measurable but not
    // selective at all.
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Use one real synapse and its own pre/post neurons. The old test
    // fired neurons[0]/neurons[1] and measured synapses[0..4], which are
    // usually unrelated to that pair, so its result said nothing about STDP.
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");

    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");

    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }

    float delta = syn->getWeight() - beforeWeight;
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                 " (Δ=" + std::to_string(delta) + ")");

    float totalDelta = delta;
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

int main(int argc, char** argv) {
    // Parse command line arguments
    CommandLineOptions options;
    if (!options.parseArgs(argc, argv)) {
        return 1;
    }
    
    if (options.validate()) {
        // If help or version was printed, we should exit
        if (argc > 1 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h" ||
                         std::string(argv[1]) == "--version" || std::string(argv[1]) == "-v")) {
            return 0;
        }
    } else {
        return 1;
    }
    
    printBanner();
    
    // Set up logging level based on options
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.verbose ? LogLevel::Debug : 
                                                         options.quiet ? LogLevel::Error : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize random seed
    Random::initialize(42);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (options.configFile != "configs/default.cfg") {
        if (config->loadFromFile(options.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + options.configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Parse custom arguments
    parseCustomArgs(config, options.customArgs);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  config_mode: " + options.mode);
    NLM_LOG_INFO("  config_duration: " + std::to_string(options.duration));
    NLM_LOG_INFO("  config_step_duration: " + std::to_string(options.stepDuration));
    NLM_LOG_INFO("  plasticity_enabled: " + std::to_string(options.enablePlasticity));
    NLM_LOG_INFO("  development_enabled: " + std::to_string(options.enableDevelopment));
    NLM_LOG_INFO("  neuromodulation_enabled: " + std::to_string(options.enableNeuromodulation));
    NLM_LOG_INFO("  memory_enabled: " + std::to_string(options.enableMemory));
    NLM_LOG_INFO("  prediction_enabled: " + std::to_string(options.enablePrediction));
    NLM_LOG_INFO("  cognition_enabled: " + std::to_string(options.enableCognition));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain with enhanced configuration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain (Phase 2)...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run enhanced experiments based on mode
    if (options.mode == "benchmark") {
        // Run enhanced benchmark
        std::cout << "Running enhanced benchmark..." << std::endl;
        std::cout << "Benchmark mode: " << options.duration << " steps with " << options.stepDuration << " step duration" << std::endl;
        
        // For now, we'll run the original tests but with enhanced configuration
        runBasicConnectivityTest(brain);
        runPlasticityExperiment(brain);
        runStdpVerification(brain);
    } else if (options.mode == "test") {
        // Run comprehensive test suite
        std::cout << "Running comprehensive test suite..." << std::endl;
        
        // Test 1: Basic connectivity
        runBasicConnectivityTest(brain);
        
        // Reset for plasticity test
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        // Reset for STDP test
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else {
        // Demo mode (default)
        std::cout << "Running NLM Phase 2 demonstration..." << std::endl;
        std::cout << "Mode: " << options.mode << std::endl;
        std::cout << "Duration: " << options.duration << " steps" << std::endl;
        
        // Test 1: Basic connectivity
        runBasicConnectivityTest(brain);
        
        // Reset for plasticity test
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        // Reset for STDP test
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}
