// NLM Demo Framework Implementation

#include "core/DemoFramework.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <sys/stat.h> // For directory checking

namespace nlm {

// ProgressMonitor implementation
struct ProgressMonitor::Impl {
    std::string currentDescription;
    float currentProgress;
    float lastUpdateTime;
    float updateInterval;
    bool shouldContinueFlag;
    std::chrono::steady_clock::time_point startTime;
};

ProgressMonitor::ProgressMonitor() : pImpl(std::make_unique<Impl>()) {
    pImpl->currentProgress = 0.0f;
    pImpl->lastUpdateTime = 0.0f;
    pImpl->updateInterval = 0.1f; // Update every 100ms
    pImpl->shouldContinueFlag = true;
    pImpl->startTime = std::chrono::steady_clock::now();
}

void ProgressMonitor::start(const std::string& description) {
    pImpl->currentDescription = description;
    pImpl->currentProgress = 0.0f;
    pImpl->startTime = std::chrono::steady_clock::now();
    std::cout << "Starting: " << description << "..." << std::endl;
}

void ProgressMonitor::update(float progress, const std::string& message) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<float>(now - pImpl->startTime).count();
    
    pImpl->currentProgress = progress;
    
    if (elapsed - pImpl->lastUpdateTime >= pImpl->updateInterval || progress >= 1.0f) {
        pImpl->lastUpdateTime = elapsed;
        
        std::cout << "\rProgress: " << std::fixed << std::setprecision(1) << (progress * 100.0f) << "%";
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << std::flush;
        
        if (progress >= 1.0f) {
            std::cout << std::endl;
        }
    }
}

void ProgressMonitor::complete(const std::string& message) {
    update(1.0f, message);
    std::cout << std::endl << "Completed: " << (message.empty() ? "Success" : message) << std::endl;
}

bool ProgressMonitor::shouldContinue() const {
    return pImpl->shouldContinueFlag;
}

void ProgressMonitor::setUpdateInterval(float interval) {
    pImpl->updateInterval = interval;
}

// PerformanceProfiler implementation
struct PerformanceProfiler::Impl {
    std::chrono::steady_clock::time_point startTime;
    std::map<std::string, double> results;
    std::string currentLabel;
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}
PerformanceProfiler::~PerformanceProfiler() = default;

void PerformanceProfiler::start(const std::string& label) {
    pImpl->currentLabel = label;
    pImpl->startTime = std::chrono::steady_clock::now();
}

double PerformanceProfiler::getCurrentTime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(now - pImpl->startTime).count();
    return duration;
}

double PerformanceProfiler::getTotalTime() const {
    return results.empty() ? 0.0 : results.begin()->second;
}

void PerformanceProfiler::stop() {
    double elapsed = getCurrentTime();
    if (!pImpl->currentLabel.empty()) {
        pImpl->results[pImpl->currentLabel] = elapsed;
    }
    pImpl->currentLabel.clear();
}

void PerformanceProfiler::reset() {
    pImpl->results.clear();
    pImpl->currentLabel.clear();
}

const std::map<std::string, double>& PerformanceProfiler::getResults() const {
    return pImpl->results;
}

void PerformanceProfiler::printResults() const {
    if (pImpl->results.empty()) {
        std::cout << "No performance data available." << std::endl;
        return;
    }
    
    std::cout << "Performance Results:" << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    for (const auto& pair : pImpl->results) {
        std::cout << "  " << pair.first << ": " << pair.second << "s" << std::endl;
    }
}

void PerformanceProfiler::saveResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open performance results file: " << filename << std::endl;
        return;
    }
    
    file << "Performance Results" << std::endl;
    file << "===================" << std::endl;
    file << std::fixed << std::setprecision(3);
    
    for (const auto& pair : pImpl->results) {
        file << pair.first << ": " << pair.second << "s" << std::endl;
    }
    
    file.close();
    std::cout << "Performance results saved to: " << filename << std::endl;
}

// DemoRunner implementation
struct DemoRunner::Impl {
    std::string workingDirectory;
    std::string configFile;
    PerformanceProfiler profiler;
    ProgressMonitor progress;
    Logger logger;
};

DemoRunner::DemoRunner() : pImpl(std::make_unique<Impl>()) {}
DemoRunner::~DemoRunner() = default;

bool DemoRunner::parseArguments(int argc, char** argv, DemoOptions& options) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                printUsage();
                return false;
            }
            else if (arg == "--version") {
                printVersion();
                return false;
            }
            else if (arg == "--list") {
                listAvailableDemos();
                return false;
            }
            else if (arg.substr(0, 10) == "--demo=") {
                std::string demoName = arg.substr(10);
                // Convert demo name to DemoMode
                static const std::map<std::string, DemoMode> demoMap = {
                    {"basic", DemoMode::BASIC_CONNECTIVITY},
                    {"plasticity", DemoMode::PLASTICITY_LEARNING},
                    {"stdp", DemoMode::STDP_VERIFICATION},
                    {"phase6", DemoMode::PHASE6_INTEGRATION},
                    {"text", DemoMode::TEXT_DEMO},
                    {"benchmark", DemoMode::BENCHMARK},
                    {"all", DemoMode::ALL_TESTS},
                    {"custom", DemoMode::CUSTOM_CONFIG}
                };
                
                auto it = demoMap.find(demoName);
                if (it != demoMap.end()) {
                    options.mode = it->second;
                } else {
                    std::cerr << "Unknown demo: " << demoName << std::endl;
                    return false;
                }
            }
            else if (arg.substr(0, 12) == "--config=") {
                options.configFile = arg.substr(12);
            }
            else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            }
            else if (arg == "--quiet" || arg == "-q") {
                options.quiet = true;
            }
            else if (arg == "--progress") {
                options.progress = true;
            }
            else if (arg.substr(0, 11) == "--output=") {
                options.outputFile = arg.substr(11);
            }
            else if (arg.substr(0, 15) == "--checkpoint=") {
                options.checkpointPath = arg.substr(15);
            }
            else if (arg.substr(0, 16) == "--log-level=") {
                options.logLevel = arg.substr(16);
            }
            else if (arg == "--no-colors") {
                options.enableColors = false;
            }
            else if (arg.substr(0, 16) == "--profile=") {
                options.profileOutput = arg.substr(16);
            }
            else if (arg == "--continuous") {
                options.runContinuous = true;
            }
            else if (arg.substr(0, 12) == "--runtime=") {
                options.runTime = std::stoul(arg.substr(12));
            }
            else if (arg.substr(0, 17) == "--neuron-count=") {
                options.neuronCount = static_cast<size_t>(std::stoul(arg.substr(17)));
            }
            else if (arg.substr(0, 17) == "--region-count=") {
                options.regionCount = static_cast<size_t>(std::stoul(arg.substr(17)));
            }
            else if (arg.substr(0, 23) == "--connection-prob=") {
                options.connectionProbability = std::stof(arg.substr(23));
            }
            else if (arg == "--advanced") {
                options.enableAdvanced = true;
            }
            else if (arg.substr(0, 12) == "--steps=") {
                options.steps = static_cast<uint64_t>(std::stoul(arg.substr(12)));
            }
            else {
                // Assume it's a custom argument
                options.customArgs.push_back(arg);
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing arguments: " << e.what() << std::endl;
        return false;
    }
}

bool DemoRunner::parseConfigFile(const std::string& filename, DemoOptions& options) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open config file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Parse based on key
        if (key == "demo_mode") {
            static const std::map<std::string, DemoMode> demoMap = {
                {"basic", DemoMode::BASIC_CONNECTIVITY},
                {"plasticity", DemoMode::PLASTICITY_LEARNING},
                {"stdp", DemoMode::STDP_VERIFICATION},
                {"phase6", DemoMode::PHASE6_INTEGRATION},
                {"text", DemoMode::TEXT_DEMO},
                {"benchmark", DemoMode::BENCHMARK},
                {"all", DemoMode::ALL_TESTS},
                {"custom", DemoMode::CUSTOM_CONFIG}
            };
            auto it = demoMap.find(value);
            if (it != demoMap.end()) {
                options.mode = it->second;
            }
        }
        else if (key == "steps") {
            options.steps = static_cast<uint64_t>(std::stoul(value));
        }
        else if (key == "neuron_count") {
            options.neuronCount = static_cast<size_t>(std::stoul(value));
        }
        else if (key == "region_count") {
            options.regionCount = static_cast<size_t>(std::stoul(value));
        }
        else if (key == "connection_probability") {
            options.connectionProbability = std::stof(value);
        }
        else if (key == "verbose") {
            options.verbose = (value == "true" || value == "1" || value == "yes");
        }
        else if (key == "quiet") {
            options.quiet = (value == "true" || value == "1" || value == "yes");
        }
        else if (key == "progress") {
            options.progress = (value == "true" || value == "1" || value == "yes");
        }
        else if (key == "enable_colors") {
            options.enableColors = (value == "true" || value == "1" || value == "yes");
        }
    }
    
    return true;
}

bool DemoRunner::parseDemoArguments(const std::vector<std::string>& args, DemoOptions& options) {
    // Parse demo-specific arguments
    for (const auto& arg : args) {
        if (arg.substr(0, 12) == "--steps=") {
            options.steps = static_cast<uint64_t>(std::stoul(arg.substr(12)));
        }
        else if (arg.substr(0, 17) == "--neuron-count=") {
            options.neuronCount = static_cast<size_t>(std::stoul(arg.substr(17)));
        }
        else if (arg.substr(0, 17) == "--region-count=") {
            options.regionCount = static_cast<size_t>(std::stoul(arg.substr(17)));
        }
        else if (arg.substr(0, 23) == "--connection-prob=") {
            options.connectionProbability = std::stof(arg.substr(23));
        }
    }
    return true;
}

void DemoRunner::setupLogging(const DemoOptions& options) {
    Logger::getGlobal().clearLoggers();
    
    // Set log level
    LogLevel logLevel;
    if (options.logLevel == "debug") logLevel = LogLevel::Debug;
    else if (options.logLevel == "warning") logLevel = LogLevel::Warning;
    else if (options.logLevel == "error") logLevel = LogLevel::Error;
    else if (options.logLevel == "critical") logLevel = LogLevel::Critical;
    else logLevel = LogLevel::Info;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    consoleLogger->setUseColors(options.enableColors);
    
    if (!options.outputFile.empty()) {
        consoleLogger->setOutputFile(options.outputFile);
    }
    
    Logger::getGlobal().addLogger(consoleLogger);
    Logger::getGlobal().setLevel(logLevel);
}

bool DemoRunner::runDemo(const DemoOptions& options) {
    setupLogging(options);
    
    NLM_LOG_INFO("Running demo: " + getDemoName(options.mode));
    NLM_LOG_INFO("Configuration loaded from: " + options.configFile);
    
    // Create brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return false;
    }
    
    // Run the specific demo
    switch (options.mode) {
        case DemoMode::BASIC_CONNECTIVITY:
            return runBasicConnectivityDemo(options);
        case DemoMode::PLASTICITY_LEARNING:
            return runPlasticityLearningDemo(options);
        case DemoMode::STDP_VERIFICATION:
            return runStdpVerificationDemo(options);
        case DemoMode::PHASE6_INTEGRATION:
            return runPhase6IntegrationDemo(options);
        case DemoMode::TEXT_DEMO:
            return runTextDemo(options);
        case DemoMode::BENCHMARK:
            return runBenchmarkDemo(options);
        case DemoMode::ALL_TESTS:
            return runAllDemos(options);
        case DemoMode::CUSTOM_CONFIG:
            return runCustomConfigDemo(options);
        default:
            NLM_LOG_ERROR("Unknown demo mode: " + std::to_string(static_cast<int>(options.mode)));
            return false;
    }
}

bool DemoRunner::runAllDemos(const DemoOptions& options) {
    DemoOptions demoOptions = options;
    bool allSuccess = true;
    
    // Run demos in logical order
    const std::vector<DemoMode> demoOrder = {
        DemoMode::BASIC_CONNECTIVITY,
        DemoMode::STDP_VERIFICATION,
        DemoMode::PLASTICITY_LEARNING,
        DemoMode::PHASE6_INTEGRATION,
        DemoMode::TEXT_DEMO,
        DemoMode::BENCHMARK
    };
    
    for (DemoMode mode : demoOrder) {
        NLM_LOG_INFO("=== Running " + getDemoName(mode) + " ===");
        
        demoOptions.mode = mode;
        bool success = runDemo(demoOptions);
        
        if (!success) {
            allSuccess = false;
            NLM_LOG_WARNING("Demo " + getDemoName(mode) + " failed, continuing with next demo");
        }
        
        // Brief pause between demos
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    return allSuccess;
}

bool DemoRunner::runBasicConnectivityDemo(const DemoOptions& options) {
    NLM_LOG_INFO("=== Basic Connectivity Demo ===");
    
    // Create brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return false;
    }
    
    // Inject current into neurons and check for spikes
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found");
        return false;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("No neurons found");
        return false;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into first few neurons
    for (size_t i = 0; i < std::min<size_t>(10, neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < options.steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (options.progress && step % 100 == 0) {
            progress.update(static_cast<float>(step) / options.steps,
                          "Steps completed: " + std::to_string(step));
        }
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("✓ Basic connectivity test PASSED");
        return true;
    } else {
        NLM_LOG_WARNING("No spikes detected - may indicate connectivity issues");
        return false; // Not fatal, just informational
    }
}

bool DemoRunner::runPlasticityLearningDemo(const DemoOptions& options) {
    NLM_LOG_INFO("=== Plasticity Learning Demo ===");
    
    // Create brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return false;
    }
    
    // Record initial state
    std::vector<float> initialWeights;
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Enable plasticity
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Run learning simulation
    std::vector<NeuronId> patternNeurons;
    if (region) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    for (SimulationStep step = 0; step < options.steps; ++step) {
        // Apply input pattern
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (options.progress && step % 200 == 0) {
            progress.update(static_cast<float>(step) / options.steps,
                          "Learning steps: " + std::to_string(step));
        }
    }
    
    // Record final state
    std::vector<float> finalWeights;
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    // Calculate weight changes
    size_t strengthened = 0, weakened = 0, unchanged = 0;
    size_t minSize = std::min(initialWeights.size(), finalWeights.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights[i] - initialWeights[i];
        if (delta > 0.01f) ++strengthened;
        else if (delta < -0.01f) ++weakened;
        else ++unchanged;
    }
    
    float initialMean = 0.0f, finalMean = 0.0f;
    if (!initialWeights.empty()) {
        float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        initialMean = sum / initialWeights.size();
    }
    if (!finalWeights.empty()) {
        float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        finalMean = sum / finalWeights.size();
    }
    
    NLM_LOG_INFO("Weight Statistics:");
    NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
    NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
    NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
    NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
    NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
    NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
    
    bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                           (strengthened > 0 || weakened > 0);
    
    if (learningOccurred) {
        NLM_LOG_INFO("✓ Plasticity learning test PASSED - measurable synaptic changes detected");
        return true;
    } else {
        NLM_LOG_WARNING("No significant learning detected");
        return false; // Not fatal, just informational
    }
}

bool DemoRunner::runStdpVerificationDemo(const DemoOptions& options) {
    NLM_LOG_INFO("=== STDP Verification Demo ===");
    
    // Create brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return false;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region || region->getSynapses().empty()) {
        NLM_LOG_ERROR("No synapses available for STDP test");
        return false;
    }
    
    // Test a single synapse
    auto& synapses = region->getSynapses();
    Synapse* syn = synapses[0].get();
    
    syn->enablePlasticity(false, true, false); // STDP only
    float beforeWeight = syn->getWeight();
    
    NLM_LOG_INFO("Testing STDP on synapse: pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) +
                 ", weight=" + std::to_string(beforeWeight));
    
    // Apply correlated pre->post activity
    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
    
    float afterWeight = syn->getWeight();
    float delta = afterWeight - beforeWeight;
    
    NLM_LOG_INFO("After correlated activity: weight=" + std::to_string(afterWeight) +
                 ", delta=" + std::to_string(delta));
    
    if (delta > 0.001f) {
        NLM_LOG_INFO("✓ STDP potentiation detected - test PASSED");
        return true;
    } else if (delta < -0.001f) {
        NLM_LOG_INFO("! STDP depression detected - check parameters");
        return false;
    } else {
        NLM_LOG_WARNING("No STDP change detected");
        return false; // Not fatal, just informational
    }
}

bool DemoRunner::runPhase6IntegrationDemo(const DemoOptions& options) {
    NLM_LOG_INFO("=== Phase 6 Integration Demo ===");
    
    // This would typically create a Phase6IntegratedExperiment instance
    // For now, we'll just log that the integration demo is available
    NLM_LOG_INFO("Phase 6 integration demo is available as a separate executable");
    NLM_LOG_INFO("Run './nlm_phase6_demo' for the full integration test");
    return true; // Always succeeds for this simple case
}

bool DemoRunner::runTextDemo(const DemoOptions& options) {
    NLM_LOG_INFO("=== Text Teaching Demo ===");
    
    // This would typically run the Phase6TextDemo
    // For now, we'll just log that the text demo is available
    NLM_LOG_INFO("Text teaching demo is available as a separate executable");
    NLM_LOG_INFO("Run './nlm_phase6_text_demo' for the text teaching demo");
    return true; // Always succeeds for this simple case
}

bool DemoRunner::runBenchmarkDemo(const DemoOptions& options) {
    NLM_LOG_INFO("=== Benchmark Demo ===");
    
    // This would typically run the benchmark
    // For now, we'll just log that the benchmark is available
    NLM_LOG_INFO("Benchmark demo is available as a separate executable");
    NLM_LOG_INFO("Run './nlm_benchmark' for performance measurements");
    return true; // Always succeeds for this simple case
}

bool DemoRunner::runCustomConfigDemo(const DemoOptions& options) {
    NLM_LOG_INFO("=== Custom Configuration Demo ===");
    
    // Create configuration from options
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(options.neuronCount));
    cfg->set("region_count", static_cast<int64_t>(options.regionCount));
    cfg->set("connection_probability", options.connectionProbability);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return false;
    }
    
    // Run custom simulation
    NLM_LOG_INFO("Running custom simulation with " + std::to_string(options.neuronCount) +
                 " neurons, " + std::to_string(options.steps) + " steps");
    
    for (SimulationStep step = 0; step < options.steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (options.progress && step % 100 == 0) {
            progress.update(static_cast<float>(step) / options.steps,
                          "Custom steps: " + std::to_string(step));
        }
    }
    
    NLM_LOG_INFO("Custom simulation completed");
    NLM_LOG_INFO("Final metrics: ");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    
    return true;
}

void DemoRunner::printUsage() const {
    std::cout << "Usage: nlm_demo [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --demo=<name>              Demo to run (basic, plasticity, stdp, phase6, text, benchmark, all, custom)" << std::endl;
    std::cout << "  --config=<file>            Configuration file to load" << std::endl;
    std::cout << "  --output=<file>            Output file for results" << std::endl;
    std::cout << "  --checkpoint=<path>        Checkpoint path for save/load" << std::endl;
    std::cout << "  --verbose, -v              Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                Suppress non-essential output" << std::endl;
    std::cout << "  --progress                 Show progress indicators" << std::endl;
    std::cout << "  --log-level=<level>        Log level (debug, info, warning, error, critical)" << std::endl;
    std::cout << "  --no-colors                Disable colored output" << std::endl;
    std::cout << "  --profile=<file>           Save performance profiling data" << std::endl;
    std::cout << "  --continuous               Run continuously until interrupted" << std::endl;
    std::cout << "  --runtime=<seconds>        Run time in seconds (for continuous mode)" << std::endl;
    std::cout << "  --steps=<count>            Number of simulation steps" << std::endl;
    std::cout << "  --neuron-count=<count>     Number of neurons" << std::endl;
    std::cout << "  --region-count=<count>     Number of regions" << std::endl;
    std::cout << "  --connection-prob=<prob>   Connection probability (0.0-1.0)" << std::endl;
    std::cout << "  --advanced                 Enable advanced features and commands" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_demo --demo=basic --steps=1000" << std::endl;
    std::cout << "  nlm_demo --demo=plasticity --config=myconfig.cfg" << std::endl;
    std::cout << "  nlm_demo --demo=all --progress --output=results.txt" << std::endl;
    std::cout << std::endl;
    std::cout << "Available demos:" << std::endl;
    listAvailableDemos();
}

void DemoRunner::printVersion() const {
    std::cout << "NLM Demo Framework v1.0.0" << std::endl;
    std::cout << "Built with C++20" << std::endl;
    std::cout << "Phase 6 Integration Ready" << std::endl;
}

std::string DemoRunner::getDemoName(DemoMode mode) {
    static const std::map<DemoMode, std::string> demoNames = {
        {DemoMode::BASIC_CONNECTIVITY, "Basic Connectivity"},
        {DemoMode::PLASTICITY_LEARNING, "Plasticity Learning"},
        {DemoMode::STDP_VERIFICATION, "STDP Verification"},
        {DemoMode::PHASE6_INTEGRATION, "Phase 6 Integration"},
        {DemoMode::TEXT_DEMO, "Text Teaching Demo"},
        {DemoMode::BENCHMARK, "Performance Benchmark"},
        {DemoMode::ALL_TESTS, "Run All Tests"},
        {DemoMode::CUSTOM_CONFIG, "Custom Configuration"}
    };
    
    auto it = demoNames.find(mode);
    return (it != demoNames.end()) ? it->second : "Unknown";
}

std::string DemoRunner::getDemoDescription(DemoMode mode) {
    static const std::map<DemoMode, std::string> demoDescriptions = {
        {DemoMode::BASIC_CONNECTIVITY, "Test basic neural connectivity and spike propagation"},
        {DemoMode::PLASTICITY_LEARNING, "Demonstrate synaptic plasticity through learning"},
        {DemoMode::STDP_VERIFICATION, "Verify spike-timing dependent plasticity mechanisms"},
        {DemoMode::PHASE6_INTEGRATION, "Run the complete Phase 6 integration test"},
        {DemoMode::TEXT_DEMO, "Text teaching and concept formation demo"},
        {DemoMode::BENCHMARK, "Performance benchmark for different network sizes"},
        {DemoMode::ALL_TESTS, "Run all available demos sequentially"},
        {DemoMode::CUSTOM_CONFIG, "Run custom simulation with specified parameters"}
    };
    
    auto it = demoDescriptions.find(mode);
    return (it != demoDescriptions.end()) ? it->second : "No description available";
}

void DemoRunner::listAvailableDemos() {
    std::cout << "Available demos:" << std::endl;
    std::cout << std::endl;
    
    // Get all demo modes
    const std::vector<DemoMode> demoModes = {
        DemoMode::BASIC_CONNECTIVITY,
        DemoMode::PLASTICITY_LEARNING,
        DemoMode::STDP_VERIFICATION,
        DemoMode::PHASE6_INTEGRATION,
        DemoMode::TEXT_DEMO,
        DemoMode::BENCHMARK,
        DemoMode::ALL_TESTS,
        DemoMode::CUSTOM_CONFIG
    };
    
    for (DemoMode mode : demoModes) {
        std::cout << "  " << getDemoName(mode) << std::endl;
        std::cout << "    " << getDemoDescription(mode) << std::endl;
        std::cout << std::endl;
    }
}

std::string DemoRunner::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).milliseconds() % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    ss << "_" << std::setfill('0') << std::setw(3) << ms;
    
    return ss.str();
}

bool DemoRunner::validateOptions(const DemoOptions& options) {
    if (options.connectionProbability < 0.0f || options.connectionProbability > 1.0f) {
        std::cerr << "Error: connection_probability must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    
    if (options.neuronCount == 0) {
        std::cerr << "Error: neuron_count must be greater than 0" << std::endl;
        return false;
    }
    
    if (options.regionCount == 0) {
        std::cerr << "Error: region_count must be greater than 0" << std::endl;
        return false;
    }
    
    if (options.steps == 0) {
        std::cerr << "Error: steps must be greater than 0" << std::endl;
        return false;
    }
    
    if (options.runContinuous && options.runTime == 0) {
        std::cerr << "Error: run_time must be greater than 0 for continuous mode" << std::endl;
        return false;
    }
    
    return true;
}

void DemoRunner::setWorkingDirectory(const std::string& directory) {
    pImpl->workingDirectory = directory;
    // Change working directory
    if (chdir(directory.c_str()) != 0) {
        std::cerr << "Warning: Could not change to directory: " << directory << std::endl;
    }
}

std::string DemoRunner::getDefaultConfigPath() {
    return "configs/default.cfg";
}

std::vector<std::string> DemoRunner::getAvailableConfigs() {
    std::vector<std::string> configs;
    
    // Check for config directory
    struct stat info;
    if (stat("configs", &info) != 0 || !(info.st_mode & S_IFDIR)) {
        return configs;
    }
    
    // Open directory
    DIR* dir = opendir("configs");
    if (!dir) {
        return configs;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name.length() > 4 && name.substr(name.length() - 4) == ".cfg") {
            configs.push_back("configs/" + name);
        }
    }
    
    closedir(dir);
    return configs;
}

bool DemoRunner::saveResults(const std::string& filename, const std::string& results) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open results file: " << filename << std::endl;
        return false;
    }
    
    file << results;
    file.close();
    
    std::cout << "Results saved to: " << filename << std::endl;
    return true;
}

bool DemoRunner::loadResults(const std::string& filename, std::string& results) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open results file: " << filename << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    results = buffer.str();
    
    file.close();
    return true;
}

void DemoRunner::reportProgress(const std::string& message, float progress) {
    if (progress >= 0.0f) {
        progress.update(progress, message);
    } else {
        NLM_LOG_INFO(message);
    }
}

void DemoRunner::handleError(const std::string& error, bool fatal) {
    std::cerr << "Error: " << error << std::endl;
    
    if (fatal) {
        std::cerr << "Fatal error - exiting." << std::endl;
        exit(1);
    }
}

// Global demo runner instance
DemoRunner& getDemoRunner() {
    static DemoRunner runner;
    return runner;
}

// Convenience functions
bool runDemoByName(const std::string& demoName) {
    DemoOptions options;
    options.mode = DemoMode::CUSTOM_CONFIG; // Default to custom for now
    
    // Convert demo name to mode (simplified)
    if (demoName == "basic") options.mode = DemoMode::BASIC_CONNECTIVITY;
    else if (demoName == "plasticity") options.mode = DemoMode::PLASTICITY_LEARNING;
    else if (demoName == "stdp") options.mode = DemoMode::STDP_VERIFICATION;
    else if (demoName == "phase6") options.mode = DemoMode::PHASE6_INTEGRATION;
    else if (demoName == "text") options.mode = DemoMode::TEXT_DEMO;
    else if (demoName == "benchmark") options.mode = DemoMode::BENCHMARK;
    else if (demoName == "all") options.mode = DemoMode::ALL_TESTS;
    
    return getDemoRunner().runDemo(options);
}

bool runDemoWithConfig(const std::string& configFile, const std::string& demoName) {
    DemoOptions options;
    options.configFile = configFile;
    
    // Convert demo name to mode (simplified)
    if (demoName == "basic") options.mode = DemoMode::BASIC_CONNECTIVITY;
    else if (demoName == "plasticity") options.mode = DemoMode::PLASTICITY_LEARNING;
    else if (demoName == "stdp") options.mode = DemoMode::STDP_VERIFICATION;
    else if (demoName == "phase6") options.mode = DemoMode::PHASE6_INTEGRATION;
    else if (demoName == "text") options.mode = DemoMode::TEXT_DEMO;
    else if (demoName == "benchmark") options.mode = DemoMode::BENCHMARK;
    else if (demoName == "all") options.mode = DemoMode::ALL_TESTS;
    
    return getDemoRunner().parseConfigFile(configFile, options) && getDemoRunner().runDemo(options);
}

bool runBenchmark(const std::map<std::string, std::pair<size_t, size_t>>& benchmarks) {
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    for (const auto& pair : benchmarks) {
        const std::string& name = pair.first;
        const auto& params = pair.second;
        
        size_t neurons = params.first;
        size_t steps = params.second;
        
        std::cout << "  Running benchmark: " << name << " (" << neurons << " neurons, " 
                  << steps << " steps)..." << std::flush;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject some random input
            for (size_t i = 0; i < std::min(size_t(10), neurons / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Collect results
        BenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        std::cout << " Done!" << std::endl;
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print results
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     NLM Performance Benchmark Results                           ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    return true;
}

} // namespace nlm
