// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

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

// Shared configuration management and utility functions
namespace ConfigUtils {
    
    // Validate configuration key name
    bool isValidKeyName(const std::string& key) {
        if (key.empty() || key.size() > 256) {
            return false;
        }
        // Check for invalid characters
        for (char c : key) {
            if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
                return false;
            }
        }
        return true;
    }
    
    // Sanitize configuration key
    std::string sanitizeKey(const std::string& key) {
        std::string result;
        for (char c : key) {
            if (std::isalnum(c) || c == '_' || c == '-' || c == '.') {
                result += c;
            }
        }
        return result;
    }
    
    // Get configuration value as specific type with fallback
    template<typename T>
    T getConfigValue(const Config& config, const std::string& key, const T& defaultValue) {
        auto value = config.get<T>(key);
        return value.has_value() ? value.value() : defaultValue;
    }
    
    // Load configuration from string (JSON, YAML, or key=value format)
    bool loadConfigFromString(Config& config, const std::string& content) {
        try {
            // Try JSON format first
            auto jsonData = nlohmann::json::parse(content);
            if (jsonData.is_object()) {
                for (const auto& [key, value] : jsonData.items()) {
                    config.set(key, value.get<std::string>(), ConfigSource::File);
                }
                return true;
            }
        } catch (...) {}
        
        // Fall back to key=value format
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            line = Config::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, pos));
                std::string value = Config::trim(line.substr(pos + 1));
                config.set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    }
    
    // Merge two configurations with precedence
    void mergeConfigs(Config& target, const Config& source) {
        for (const auto& key : source.getKeys()) {
            if (!target.has(key)) {
                auto value = source.get<std::string>(key);
                if (value.has_value()) {
                    target.set(key, value.value(), ConfigSource::File);
                }
            }
        }
    }
    
    // Get configuration as JSON object
    nlohmann::json configToJson(const Config& config) {
        nlohmann::json jsonData = nlohmann::json::object();
        for (const auto& key : config.getKeys()) {
            auto value = config.get<std::string>(key);
            if (value.has_value()) {
                jsonData[key] = value.value();
            }
        }
        return jsonData;
    }
    
    // Validate configuration values
    bool validateConfig(const Config& config, const std::vector<std::string>& requiredKeys) {
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                NLM_LOG_WARNING(std::string("Missing required config key: ") + key);
                return false;
            }
        }
        return true;
    }
    
} // namespace ConfigUtils

ExperimentResult runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    ExperimentResult result;
    result.name = "Basic Neural Connectivity";
    result.success = false;
    result.details = "Basic connectivity test failed";
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        result.details = "Region not found";
        return result;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        result.details = "No neurons found";
        return result;
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
        result.success = true;
        result.details = "Successfully generated " + std::to_string(spikes) + " spikes through network";
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        result.details = "No spikes generated - neuron parameters may need adjustment";
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
    
    return result;
}

ExperimentResult runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    ExperimentResult result;
    result.name = "Plasticity Learning Experiment";
    result.success = false;
    result.details = "Plasticity experiment failed";
    
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
    
    // Determine success based on learning
    float initialMean = 0.0f, finalMean = 0.0f;
    if (!experiment.initialWeights.empty()) {
        initialMean = std::accumulate(experiment.initialWeights.begin(), experiment.initialWeights.end(), 0.0f) / experiment.initialWeights.size();
    }
    if (!experiment.finalWeights.empty()) {
        finalMean = std::accumulate(experiment.finalWeights.begin(), experiment.finalWeights.end(), 0.0f) / experiment.finalWeights.size();
    }
    
    bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                            (experiment.finalWeights.size() < experiment.initialWeights.size() ? 
                            true : false);
    
    result.success = learningOccurred;
    result.details = "Plasticity experiment completed - learning " + 
                    (learningOccurred ? "detected" : "not detected");
    
    return result;
}

ExperimentResult runStdpVerification(std::shared_ptr<Brain> brain) {
    ExperimentResult result;
    result.name = "STDP Verification";
    result.success = false;
    result.details = "STDP verification failed";
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        result.details = "Region not found";
        return result;
    }
    
    // Use one real synapse and its own pre/post neurons. The old test
    // fired neurons[0]/neurons[1] and measured synapses[0..4], which are
    // usually unrelated to that pair, so its result said nothing about STDP.
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        result.details = "No synapses available for STDP test";
        return result;
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
        result.success = true;
        result.details = "STDP working: Pre-before-post produced potentiation (Δ=" + std::to_string(totalDelta) + ")";
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        result.details = "STDP reversed: Check parameters (Δ=" + std::to_string(totalDelta) + ")";
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        result.details = "STDP may not be triggering: No change (Δ=" + std::to_string(totalDelta) + ")";
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
    
    return result;
}

void logExperimentResults(const std::vector<ExperimentResult>& results) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXPERIMENT SUMMARY ===");
    
    int successCount = 0;
    for (const auto& result : results) {
        std::string status = result.success ? "✓ PASS" : "✗ FAIL";
        NLM_LOG_INFO("  " + status + " - " + result.name + ": " + result.details);
        if (result.success) successCount++;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Results: " + std::to_string(successCount) + "/" + std::to_string(results.size()) + " experiments passed");
    
    if (successCount == results.size()) {
        NLM_LOG_INFO("✓ ALL EXPERIMENTS PASSED");
    } else {
        NLM_LOG_INFO("✗ SOME EXPERIMENTS FAILED");
    }
}

void logConfiguration(const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config.getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config.getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config.getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config.getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config.getOr<float>("connection_probability", 0.1f)));
    NLM_LOG_INFO("");
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

// Command line interface and configuration parsing
struct CLIOptions {
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string logFile = "";
    std::string logLevel = "INFO";
    bool quiet = false;
    bool verbose = false;
    bool version = false;
    bool help = false;
    std::vector<std::string> customArgs;
};

CLIOptions parseCommandLine(int argc, char** argv) {
    CLIOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
            options.verbose = false;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
            options.quiet = false;
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        } else if (arg == "--logfile" || arg == "--log") {
            if (i + 1 < argc) {
                options.logFile = argv[++i];
            }
        } else if (arg == "--loglevel") {
            if (i + 1 < argc) {
                options.logLevel = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg.find("=") != std::string::npos && arg.substr(0, 2) == "--") {
            options.customArgs.push_back(arg);
        } else if (arg.find("=") != std::string::npos && arg[0] == '-') {
            options.customArgs.push_back(arg);
        } else if (arg[0] == '-') {
            options.customArgs.push_back(arg);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                options.customArgs.push_back(argv[++i]);
            }
        } else {
            // Unknown argument, treat as custom arg
            options.customArgs.push_back(arg);
        }
    }
    
    return options;
}

void printHelp(const char* programName) {
    printBanner();
    std::cout << R"(
Usage: "" << programName << "" [OPTIONS]

NLM (Neural Learning Machine) - Phase 2 Real Neural Computation

Options:
  -h, --help              Show this help message and exit
  -v, --verbose           Enable verbose output
  -q, --quiet             Suppress informational messages
  -o, --output FILE       Save results to FILE
  --logfile FILE          Write logs to FILE
  --loglevel LEVEL        Set log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
  --config FILE           Load configuration from FILE
      
Experiment Options:
  --no-tests              Skip all tests and run only initialization
  --test1-only           Run only Test 1: Basic Connectivity
  --test2-only           Run only Test 2: Plasticity Learning
  --test3-only           Run only Test 3: STDP Verification
  --test1-only           Run only Test 1: Basic Connectivity
  --test2-only           Run only Test 2: Plasticity Learning
  --test3-only           Run only Test 3: STDP Verification

Output Options:
  --json-output          Save results in JSON format
  --summary-only        Show only experiment summary, no detailed output

Debug Options:
  --version              Show version information and exit
      )" << std::endl;
}

void printVersion() {
    std::cout << R"(
NLM (熙然) - Neural Learning Machine
Version: 0.1.0 (Phase 2)

Build: "" << __DATE__ << " " << __TIME__ << """

Real spiking neural computation with:
  - Leaky Integrate-and-Fire (LIF) neurons
  - Event-driven spike propagation with delays
  - STDP and Hebbian plasticity rules
  - Structural plasticity (synaptogenesis/pruning)
""" << std::endl;
}

bool setupLogging(const std::string& logFile, const std::string& logLevel, bool quiet, bool verbose) {
    try {
        // Create logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>();
        
        // Configure log level
        LogLevel level = LogLevel::Info;
        if (logLevel == "DEBUG") level = LogLevel::Debug;
        else if (logLevel == "WARNING") level = LogLevel::Warning;
        else if (logLevel == "ERROR") level = LogLevel::Error;
        else if (logLevel == "CRITICAL") level = LogLevel::Critical;
        
        consoleLogger->setLevel(level);
        logger->addLogger(consoleLogger);
        
        if (!logFile.empty()) {
            consoleLogger->setOutputFile(logFile);
        }
        
        // Set global logger
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Logging system initialized");
        NLM_LOG_INFO("Log level: " + logLevel);
        if (!logFile.empty()) {
            NLM_LOG_INFO("Log file: " + logFile);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize logging system: " << e.what() << std::endl;
        return false;
    }
}

bool runAllTests(std::shared_ptr<Brain> brain, bool verbose = false) {
    std::vector<ExperimentResult> results;
    
    // Run Test 1: Basic connectivity
    ExperimentResult test1Result = runBasicConnectivityTest(brain);
    results.push_back(test1Result);
    
    // Reset brain for next test
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning
    ExperimentResult test2Result = runPlasticityExperiment(brain);
    results.push_back(test2Result);
    
    // Reset brain for next test
    brain->reset();
    brain->initialize();
    
    // Run Test 3: STDP verification
    ExperimentResult test3Result = runStdpVerification(brain);
    results.push_back(test3Result);
    
    // Display results
    logExperimentResults(results);
    
    return std::all_of(results.begin(), results.end(),
        [](const ExperimentResult& r) { return r.success; });
}

bool runSelectedTests(std::shared_ptr<Brain> brain, int testMask, bool verbose = false) {
    std::vector<ExperimentResult> results;
    
    if (testMask & 1) {
        ExperimentResult test1Result = runBasicConnectivityTest(brain);
        results.push_back(test1Result);
        if (testMask & 2) {  // Reset for next test if continuing
            brain->reset();
            brain->initialize();
        }
    }
    
    if (testMask & 2) {
        ExperimentResult test2Result = runPlasticityExperiment(brain);
        results.push_back(test2Result);
        if (testMask & 4) {  // Reset for next test if continuing
            brain->reset();
            brain->initialize();
        }
    }
    
    if (testMask & 4) {
        ExperimentResult test3Result = runStdpVerification(brain);
        results.push_back(test3Result);
    }
    
    // Display results
    logExperimentResults(results);
    
    return std::all_of(results.begin(), results.end(),
        [](const ExperimentResult& r) { return r.success; });
}

int main(int argc, char** argv) {
    // Parse command line options
    CLIOptions options = parseCommandLine(argc, argv);
    
    // Handle help and version
    if (options.help) {
        printHelp(argv[0]);
        return 0;
    }
    
    if (options.version) {
        printVersion();
        return 0;
    }
    
    printBanner();
    
    // Setup logging system
    if (!setupLogging(options.logFile, options.logLevel, options.quiet, options.verbose)) {
        std::cerr << "Warning: Failed to initialize logging system, using defaults" << std::endl;
    }
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Starting neural simulation with configurable options");
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Try to load configuration from file
    if (!options.configFile.empty()) {
        NLM_LOG_INFO("Loading configuration from: " + options.configFile);
        if (!config->loadFromFile(options.configFile)) {
            NLM_LOG_ERROR("Failed to load configuration from " + options.configFile + ", using defaults");
        } else {
            NLM_LOG_INFO("Configuration loaded successfully");
        }
    }
    
    // Override with command line arguments
    if (!options.customArgs.empty()) {
        NLM_LOG_INFO("Applying command line overrides: " + std::to_string(options.customArgs.size()) + " arguments");
        // Note: Config::loadFromArgs would parse --key=value format
        // For now, just log that we would process custom args
    }
    
    // Apply default values for Phase 2 (if not already set)
    applyDefaultConfiguration(config);
    
    // Log configuration summary
    logConfiguration(*config);
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Determine which tests to run based on command line options
    int testMask = 7; // All tests (1 | 2 | 4)
    bool runAll = true;
    
    if (options.customArgs.size() > 0) {
        // Process custom test options
        for (const auto& arg : options.customArgs) {
            if (arg == "--test1-only") testMask = 1;
            else if (arg == "--test2-only") testMask = 2;
            else if (arg == "--test3-only") testMask = 4;
            else if (arg == "--no-tests") testMask = 0;
            else if (arg == "--json-output") config->set("output_format", "json", ConfigSource::Runtime);
        }
        runAll = (testMask == 7);
    }
    
    // Run tests
    bool success = false;
    if (runAll) {
        success = runAllTests(brain, options.verbose);
    } else if (testMask > 0) {
        success = runSelectedTests(brain, testMask, options.verbose);
    } else {
        NLM_LOG_INFO("No tests selected to run");
        success = true; // Nothing to do is not a failure
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Log overall status
    if (success) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== ALL TESTS PASSED ===");
        NLM_LOG_INFO("Phase 2 objectives completed successfully.");
        NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    } else {
        NLM_LOG_ERROR("");
        NLM_LOG_ERROR("=== TESTS FAILED ===");
        NLM_LOG_ERROR("Some tests did not complete successfully.");
        NLM_LOG_ERROR("Check the output above for details on failures.");
    }
    
    return success ? 0 : 1;
}

void logExperimentResults(const std::vector<ExperimentResult>& results) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXPERIMENT SUMMARY ===");
    
    int successCount = 0;
    for (const auto& result : results) {
        std::string status = result.success ? "✓ PASS" : "✗ FAIL";
        NLM_LOG_INFO("  " + status + " - " + result.name + ": " + result.details);
        if (result.success) successCount++;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Results: " + std::to_string(successCount) + "/" + std::to_string(results.size()) + " experiments passed");
    
    if (successCount == results.size()) {
        NLM_LOG_INFO("✓ ALL EXPERIMENTS PASSED");
    } else {
        NLM_LOG_INFO("✗ SOME EXPERIMENTS FAILED");
    }
}

void logConfiguration(const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config.getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config.getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config.getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config.getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config.getOr<float>("connection_probability", 0.1f)));
    NLM_LOG_INFO("");
}
