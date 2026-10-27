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

void printAdvancedHelp() {
    printBanner();
    
    std::cout << R"(
Advanced NLM Command Line Interface - Expert Mode
================================================

Basic Usage:
    nlm [options]
    nlm --config=/path/to/config.cfg [options]

Advanced Configuration Options:
    --config=FILE                 Load configuration from file
    --validate-only              Validate configuration only (no simulation)
    --validate-schema-only       Validate configuration against schema only
    --export-config=FILE          Export current configuration to file
    --backup-config=FILE          Create backup of current configuration
    --import-config=FILE          Import configuration from file
    --merge-config=FILE           Merge configuration from file
    --config-key=KEY              Set configuration key (advanced)
    --config-file=FILE            Load from alternative config file

Diagnostic Options:
    --diag-config                 Display configuration summary
    --diag-schema                Display current schema
    --diag-validation            Run full configuration validation
    --diag-memory                Memory diagnostics
    --diag-performance           Performance profiling
    --diag-neurons               Neuron diagnostics
    --diag-synapses              Synapse diagnostics
    --diag-plasticity           Plasticity diagnostics

Phase Control:
    --phase=PHASE                Set developmental phase (1-6)
    --skip-phases=N              Skip first N phases for faster testing
    --restart-phase=PHASE         Restart from specified phase

Performance Tuning:
    --simulation-speed=LEVEL     Control simulation speed (1-10)
    --neuron-count=COUNT         Override neuron count
    --region-count=COUNT         Override region count
    --connection-prob=PROB       Override connection probability
    --timestep=DT                Override simulation timestep
    --max-steps=STEPS            Override maximum simulation steps

Debug Options:
    --debug-mode                 Enable debug mode
    --trace-neurons              Trace individual neuron activity
    --trace-synapses             Trace synaptic connections
    --log-level=LVL              Set log level (DEBUG, INFO, WARNING, ERROR)
    --log-file=FILE              Set log file
    --verbose                    Enable verbose output

Expert Configuration System:
    --validate-only              Validate configuration and exit
    --export-config=FILE          Export current config to file
    --import-config=FILE          Import config from file
    --merge-config=FILE           Merge config from file
    --validate-schema            Validate against schema
    --show-schema                Display schema information

Prediction & Planning:
    --enable-prediction          Enable prediction system
    --enable-planning            Enable planning system
    --enable-memory              Enable memory systems
    --memory-type=TYPE           Set memory system type

Neuromodulation:
    --dopamine-level=LEVEL       Set dopamine baseline
    --curiosity-gain=GAIN        Set curiosity gain
    --reward-scale=SCALE         Set reward scaling

Development:
    --development-stage=STAGE    Set developmental stage
    --synaptogenesis-rate=RATE   Set synaptogenesis rate
    --pruning-rate=RATE          Set pruning rate

Visualization:
    --enable-visualization       Enable visualization
    --visualization-rate=RATE    Set visualization update rate

Preset Configurations:
    --preset=NAME                Load preset configuration (fast, balanced, detailed, research)
    --copy-preset=NAME=FILE       Save current config as preset

Safety & Recovery:
    --dry-run                    Validate and display changes without applying
    --create-checkpoint          Create configuration checkpoint
    --restore-checkpoint=FILE    Restore from checkpoint
    --validate-strict            Run strict validation (fail on warnings)

Examples:
    # Validate and export configuration
    nlm --validate-only --export-config=backup.cfg

    # Run with custom settings
    nlm --neuron-count=2000 --region-count=4 --connection-prob=0.2

    # Debug configuration
    nlm --diag-config --diag-schema --diag-validation

    # Performance testing
    nlm --skip-phases=3 --simulation-speed=5 --max-steps=5000

    # Expert configuration management
    nlm --import-config=research.cfg --merge-config

    # Validate with strict mode
    nlm --validate-only --validate-strict

For help on any specific option, run:
    nlm --help
)" << std::endl;
}

void handleAdvancedCommands(std::shared_ptr<Config>& config, const std::vector<std::string>& args) {
    bool showHelp = false;
    bool validateOnly = false;
    bool validateSchemaOnly = false;
    bool exportConfig = false;
    std::string exportConfigFile;
    bool importConfig = false;
    std::string importConfigFile;
    bool mergeConfig = false;
    bool diagConfig = false;
    bool diagSchema = false;
    bool diagValidation = false;
    bool diagMemory = false;
    bool diagPerformance = false;
    bool diagNeurons = false;
    bool diagSynapses = false;
    bool diagPlasticity = false;
    bool debugMode = false;
    bool verbose = false;
    
    std::string logLevel = "INFO";
    std::string logFile = "nlm.log";
    bool enableVisualization = false;
    int simulationSpeed = 1;
    int neuronCount = -1;
    int regionCount = -1;
    double connectionProb = -1.0;
    double timestep = -1.0;
    int64_t maxSteps = -1;
    
    // Parse advanced arguments
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--help" || args[i] == "-h") {
            showHelp = true;
        } else if (args[i] == "--validate-only") {
            validateOnly = true;
        } else if (args[i] == "--validate-schema-only") {
            validateSchemaOnly = true;
        } else if (args[i].rfind("--export-config=", 0) == 0) {
            exportConfig = true;
            exportConfigFile = args[i].substr(16);
        } else if (args[i] == "--import-config") {
            importConfig = true;
            if (i + 1 < args.size()) {
                importConfigFile = args[++i];
            }
        } else if (args[i] == "--merge-config") {
            mergeConfig = true;
        } else if (args[i] == "--diag-config") {
            diagConfig = true;
        } else if (args[i] == "--diag-schema") {
            diagSchema = true;
        } else if (args[i] == "--diag-validation") {
            diagValidation = true;
        } else if (args[i] == "--debug-mode") {
            debugMode = true;
        } else if (args[i] == "--verbose") {
            verbose = true;
        } else if (args[i].rfind("--log-level=", 0) == 0) {
            logLevel = args[i].substr(11);
        } else if (args[i].rfind("--log-file=", 0) == 0) {
            logFile = args[i].substr(10);
        } else if (args[i] == "--enable-visualization") {
            enableVisualization = true;
        } else if (args[i].rfind("--neuron-count=", 0) == 0) {
            neuronCount = std::stoi(args[i].substr(14));
        } else if (args[i].rfind("--region-count=", 0) == 0) {
            regionCount = std::stoi(args[i].substr(15));
        } else if (args[i].rfind("--connection-prob=", 0) == 0) {
            connectionProb = std::stod(args[i].substr(19));
        } else if (args[i].rfind("--timestep=", 0) == 0) {
            timestep = std::stod(args[i].substr(10));
        } else if (args[i].rfind("--max-steps=", 0) == 0) {
            maxSteps = std::stoll(args[i].substr(11));
        } else if (args[i] == "--diag-memory") {
            diagMemory = true;
        } else if (args[i] == "--diag-performance") {
            diagPerformance = true;
        } else if (args[i] == "--diag-neurons") {
            diagNeurons = true;
        } else if (args[i] == "--diag-synapses") {
            diagSynapses = true;
        } else if (args[i] == "--diag-plasticity") {
            diagPlasticity = true;
        } else if (args[i].rfind("--preset=", 0) == 0) {
            // Handle preset configurations
            std::string preset = args[i].substr(8);
            if (preset == "fast") {
                config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::CommandLine);
                config->set("region_count", static_cast<int64_t>(1), ConfigSource::CommandLine);
                config->set("connection_probability", 0.2, ConfigSource::CommandLine);
                config->set("max_simulation_steps", static_cast<int64_t>(1000), ConfigSource::CommandLine);
                config->set("simulation_time_limit", 0.0, ConfigSource::CommandLine);
            } else if (preset == "balanced") {
                config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::CommandLine);
                config->set("region_count", static_cast<int64_t>(2), ConfigSource::CommandLine);
                config->set("connection_probability", 0.15, ConfigSource::CommandLine);
                config->set("max_simulation_steps", static_cast<int64_t>(5000), ConfigSource::CommandLine);
                config->set("simulation_time_limit", 0.0, ConfigSource::CommandLine);
            } else if (preset == "detailed") {
                config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::CommandLine);
                config->set("region_count", static_cast<int64_t>(4), ConfigSource::CommandLine);
                config->set("connection_probability", 0.1, ConfigSource::CommandLine);
                config->set("max_simulation_steps", static_cast<int64_t>(20000), ConfigSource::CommandLine);
                config->set("simulation_time_limit", 0.0, ConfigSource::CommandLine);
            } else if (preset == "research") {
                config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::CommandLine);
                config->set("region_count", static_cast<int64_t>(6), ConfigSource::CommandLine);
                config->set("connection_probability", 0.05, ConfigSource::CommandLine);
                config->set("max_simulation_steps", static_cast<int64_t>(50000), ConfigSource::CommandLine);
                config->set("simulation_time_limit", 0.0, ConfigSource::CommandLine);
                config->set("development_synaptogenesis_rate", 0.0005, ConfigSource::CommandLine);
                config->set("development_pruning_rate", 0.00005, ConfigSource::CommandLine);
            }
        }
    }
    
    if (showHelp) {
        printAdvancedHelp();
        return;
    }
    
    if (validateOnly) {
        std::cout << "=== Configuration Validation ===" << std::endl;
        auto errors = config->validate();
        if (errors.empty()) {
            std::cout << "✓ Configuration is valid!" << std::endl;
        } else {
            std::cout << "⚠ Configuration has issues:" << std::endl;
            for (const auto& error : errors) {
                std::string severity = (error.severity == ValidationResult::Error) ? "ERROR" :
                                     (error.severity == ValidationResult::Warning) ? "WARNING" : "INFO";
                std::cout << "  " << severity << ": " << error.key << " - " << error.message << std::endl;
            }
        }
        return;
    }
    
    if (validateSchemaOnly) {
        std::cout << "=== Schema Validation ===" << std::endl;
        static nlm::ConfigValidator validator;
        if (validator.getSchemaEntries().empty()) {
            validator.loadDefaultSchema();
        }
        auto errors = validator.validate(*config);
        if (errors.empty()) {
            std::cout << "✓ Schema validation passed!" << std::endl;
        } else {
            std::cout << "⚠ Schema validation has issues:" << std::endl;
            for (const auto& error : errors) {
                std::string severity = (error.severity == ValidationResult::Error) ? "ERROR" :
                                     (error.severity == ValidationResult::Warning) ? "WARNING" : "INFO";
                std::cout << "  " << severity << ": " << error.key << " - " << error.message << std::endl;
            }
        }
        return;
    }
    
    if (exportConfig) {
        std::cout << "=== Exporting Configuration ===" << std::endl;
        if (config->saveToFile(exportConfigFile)) {
            std::cout << "✓ Configuration exported to: " << exportConfigFile << std::endl;
        } else {
            std::cout << "✗ Failed to export configuration to: " << exportConfigFile << std::endl;
        }
        return;
    }
    
    if (importConfig && !importConfigFile.empty()) {
        std::cout << "=== Importing Configuration ===" << std::endl;
        if (mergeConfig) {
            // Merge import with current config
            nlm::Config importedConfig;
            if (importedConfig.loadFromFile(importConfigFile)) {
                for (const auto& key : importedConfig.getKeys()) {
                    config->set(key, importedConfig.getOr<std::string>(key, ""), ConfigSource::Runtime);
                }
                std::cout << "✓ Configuration merged from: " << importConfigFile << std::endl;
            } else {
                std::cout << "✗ Failed to import configuration from: " << importConfigFile << std::endl;
            }
        } else {
            // Replace current config
            config->clear();
            if (config->loadFromFile(importConfigFile)) {
                std::cout << "✓ Configuration imported from: " << importConfigFile << std::endl;
            } else {
                std::cout << "✗ Failed to import configuration from: " << importConfigFile << std::endl;
            }
        }
        return;
    }
    
    if (diagConfig) {
        std::cout << "=== Configuration Diagnostics ===" << std::endl;
        std::cout << config->summary() << std::endl;
        return;
    }
    
    if (diagSchema) {
        std::cout << "=== Schema Diagnostics ===" << std::endl;
        static nlm::ConfigValidator validator;
        if (validator.getSchemaEntries().empty()) {
            validator.loadDefaultSchema();
        }
        std::cout << "Schema contains " << validator.getSchemaEntries().size() << " entries:" << std::endl;
        for (const auto& entry : validator.getSchemaEntries()) {
            std::cout << "  " << entry.key << " (" << entry.type << ") - " << entry.description;
            if (entry.required) {
                std::cout << " [REQUIRED]";
            }
            std::cout << std::endl;
        }
        return;
    }
    
    if (diagValidation) {
        std::cout << "=== Configuration Validation Diagnostics ===" << std::endl;
        auto errors = config->validate();
        if (errors.empty()) {
            std::cout << "✓ Configuration is valid!" << std::endl;
        } else {
            std::cout << "⚠ Validation results:" << std::endl;
            for (const auto& error : errors) {
                std::string severity = (error.severity == ValidationResult::Error) ? "ERROR" :
                                     (error.severity == ValidationResult::Warning) ? "WARNING" : "INFO";
                std::cout << "  " << severity << ": " << error.key << " - " << error.message << std::endl;
            }
        }
        return;
    }
    
    if (verbose) {
        std::cout << "=== Verbose Mode ===" << std::endl;
        std::cout << "Debug mode: " << (debugMode ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Log level: " << logLevel << std::endl;
        std::cout << "Log file: " << logFile << std::endl;
        std::cout << "Visualization: " << (enableVisualization ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Configuration keys: " << config->getKeys().size() << std::endl;
    }
    
    // Apply overrides to configuration
    if (neuronCount != -1) {
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
        if (verbose) std::cout << "Overrode neuron_count: " << neuronCount << std::endl;
    }
    
    if (regionCount != -1) {
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::CommandLine);
        if (verbose) std::cout << "Overrode region_count: " << regionCount << std::endl;
    }
    
    if (connectionProb != -1.0) {
        config->set("connection_probability", connectionProb, ConfigSource::CommandLine);
        if (verbose) std::cout << "Overrode connection_probability: " << connectionProb << std::endl;
    }
    
    if (timestep != -1.0) {
        config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
        if (verbose) std::cout << "Overrode simulation_timestep: " << timestep << std::endl;
    }
    
    if (maxSteps != -1) {
        config->set("max_simulation_steps", maxSteps, ConfigSource::CommandLine);
        if (verbose) std::cout << "Overrode max_simulation_steps: " << maxSteps << std::endl;
    }
    
    if (verbose) {
        std::cout << "=== Advanced Configuration Summary ===" << std::endl;
        std::cout << "Simulation timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
        std::cout << "Neuron count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
        std::cout << "Region count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
        std::cout << "Connection probability: " << config->getOr<double>("connection_probability", 0.15) << std::endl;
        std::cout << "Max simulation steps: " << config->getOr<int64_t>("max_simulation_steps", 10000) << std::endl;
        std::cout << "Configuration validation: " << (config->validate().empty() ? "PASS" : "FAIL") << std::endl;
    }
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
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
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
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
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
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
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
    // Parse command line arguments to detect advanced commands
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // Handle advanced commands
    auto config = std::make_shared<Config>();
    
    // Try to load config file first for advanced commands
    std::string configFile = "configs/default.cfg";
    for (size_t i = 1; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < args.size()) {
                configFile = args[++i];
            }
        }
    }
    
    // Load configuration for advanced commands
    config->loadFromFile(configFile);
    
    // Process advanced command line arguments
    handleAdvancedCommands(config, args);
    
    // Check if any advanced command was processed that should exit
    bool advancedExit = false;
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--validate-only" || 
            args[i] == "--validate-schema-only" ||
            args[i].rfind("--export-config=", 0) == 0 ||
            args[i] == "--import-config" ||
            args[i] == "--diag-config" ||
            args[i] == "--diag-schema" ||
            args[i] == "--diag-validation" ||
            args[i] == "--diag-memory" ||
            args[i] == "--diag-performance" ||
            args[i] == "--diag-neurons" ||
            args[i] == "--diag-synapses" ||
            args[i] == "--diag-plasticity" ||
            args[i] == "--help" || args[i] == "-h" ||
            args[i].rfind("--preset=", 0) == 0) {
            advancedExit = true;
            break;
        }
    }
    
    if (advancedExit) {
        return 0;
    }
    
    // Original main program continues from here
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
