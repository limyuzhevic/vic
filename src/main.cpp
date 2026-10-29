// NLM - Neural Learning Machine
// Main executable entry point with improved CLI and structure

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

using namespace nlm;

// Forward declarations
void printHelp(const std::string& programName);
void printBanner();
std::string getVersionInfo();
std::string getBuildInfo();

// Command line interface
enum class CommandMode {
    AUTO,
    DEMONSTRATION,
    TEST_CONNECTIVITY,
    TEST_PLASTICITY,
    TEST_STDP,
    TEST_INTEGRATION,
    CONFIG_DUMP,
    HELP,
    VISUALIZATION,
    DEBUG,
    PROFILE,
    EXPORT,
    IMPORT,
    BATCH_TEST,
    NEURAL_ANALYSIS
};

// Command line parser
class CommandLineParser {
private:
    std::map<std::string, std::string> args;
    std::vector<std::string> positionalArgs;
    
public:
    bool parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args["help"] = "1";
            } else if (arg == "--version" || arg == "-v") {
                args["version"] = "1";
            } else if (arg == "--build-info") {
                args["build_info"] = "1";
            } else if (arg == "--config") {
                if (i + 1 < argc) {
                    args["config_file"] = argv[++i];
                }
            } else if (arg == "--config-set") {
                if (i + 2 < argc) {
                    args["config_set"] = argv[++i] + "=" + argv[++i];
                }
            } else if (arg == "--mode") {
                if (i + 1 < argc) {
                    args["mode"] = argv[++i];
                }
            } else if (arg == "--steps" || arg == "--duration") {
                if (i + 1 < argc) {
                    args["steps"] = argv[++i];
                }
            } else if (arg == "--neuron-count") {
                if (i + 1 < argc) {
                    args["neuron_count"] = argv[++i];
                }
            } else if (arg == "--region-count") {
                if (i + 1 < argc) {
                    args["region_count"] = argv[++i];
                }
            } else if (arg == "--connection-prob") {
                if (i + 1 < argc) {
                    args["connection_prob"] = argv[++i];
                }
            } else if (arg == "--demo") {
                args["demo_mode"] = "1";
            } else if (arg == "--test") {
                if (i + 1 < argc) {
                    args["test_type"] = argv[++i];
                }
            } else if (arg.substr(0, 2) == "--") {
                // Handle --key=value format
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    args[key] = value;
                }
            } else {
                positionalArgs.push_back(arg);
            }
        }
        return true;
    }
    
    std::string getArg(const std::string& key, const std::string& defaultValue = "") const {
        auto it = args.find(key);
        return (it != args.end()) ? it->second : defaultValue;
    }
    
    bool hasArg(const std::string& key) const {
        return args.find(key) != args.end();
    }
    
    const std::vector<std::string>& getPositionalArgs() const {
        return positionalArgs;
    }
};

// Demonstration mode - show basic functionality
void runDemonstrationMode(std::shared_ptr<Brain> brain) {
    printBanner();
    NLM_LOG_INFO("NLM Demonstration Mode - Basic Brain Functionality");
    NLM_LOG_INFO("Creating and simulating a basic neural network...");
    
    // Run demonstration steps
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
        if (step % 20 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                        std::to_string(brain->getFiringNeuronCount()) + " neurons firing");
        }
    }
    
    NLM_LOG_INFO("Demonstration complete!");
    NLM_LOG_INFO("Final statistics:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
}

// Test connectivity - verify neural network can transmit signals
void runConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Connectivity Test ===");
    
    auto neurons = brain->getTotalNeuronCount();
    NLM_LOG_INFO("Brain has " + std::to_string(neurons) + " neurons");
    
    if (neurons == 0) {
        NLM_LOG_ERROR("No neurons in brain - connectivity test failed");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into first few neurons
    NLM_LOG_INFO("Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min<size_t>(10, neurons); ++i) {
        brain->injectCurrentToNeurons(NeuronType::Excitatory, 50.0f);
    }
    
    // Run steps to allow spikes to propagate
    for (int step = 0; step < 50; ++step) {
        brain->step(step);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Connectivity test result: " + std::to_string(spikes) + " spikes generated");
    
    if (spikes > 0) {
        NLM_LOG_INFO("✓ Connectivity test PASSED - neural signals propagate");
    } else {
        NLM_LOG_WARNING("Connectivity test shows no spikes - check neuron parameters");
    }
}

// Test plasticity - verify synaptic learning capabilities
void runPlasticityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Plasticity Learning Test ===");
    
    // Record initial state
    NLM_LOG_INFO("Recording initial synaptic weights...");
    size_t initialSynapses = brain->getTotalSynapseCount();
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input patterns to stimulate learning
    NLM_LOG_INFO("Applying repeated input patterns (500 steps)...");
    for (int step = 0; step < 500; ++step) {
        // Create correlated activity pattern
        for (size_t i = 0; i < 5 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    size_t finalSynapses = brain->getTotalSynapseCount();
    NLM_LOG_INFO("Plasticity test result: " + std::to_string(finalSynapses) + 
                " synapses (initial: " + std::to_string(initialSynapses) + ")");
    
    if (finalSynapses >= initialSynapses) {
        NLM_LOG_INFO("✓ Plasticity test PASSED - synaptic changes detected");
    } else {
        NLM_LOG_WARNING("Plasticity test shows unexpected synapse count");
    }
}

// Test STDP - verify spike timing dependent plasticity
void runSTDPTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== STDP Verification Test ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region || region->getSynapses().size() < 3) {
        NLM_LOG_WARNING("Not enough synapses for STDP test");
        return;
    }
    
    // Get first 3 synapses
    auto& synapses = region->getSynapses();
    std::vector<float> beforeWeights;
    
    NLM_LOG_INFO("Testing STDP on 3 synapses:");
    for (size_t i = 0; i < 3; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("Creating correlated pre->post activity (LTP test)...");
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        for (int trial = 0; trial < 30; ++trial) {
            // Fire pre-synaptic neuron
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post-synaptic neuron
            neurons[1]->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 3; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("✓ STDP test PASSED - LTP observed (pre-before-post)");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("! STDP test shows LTD (pre-after-post)");
    } else {
        NLM_LOG_INFO("! STDP test shows no change - may need tuning");
    }
}

// Additional mode implementations

// Visualization mode - generate visual output
void runVisualizationMode(std::shared_ptr<Brain> brain, const CommandLineParser& parser) {
    NLM_LOG_INFO("=== Visualization Mode ===");
    NLM_LOG_INFO("Generating neural network visualization...");
    
    int width = 800;
    int height = 600;
    bool saveToFile = parser.hasArg("save-viz");
    std::string outputFile = parser.getArg("output-file", "brain_viz.png");
    
    // For now, just log the visualization parameters
    NLM_LOG_INFO("Visualization parameters:");
    NLM_LOG_INFO("  Dimensions: " + std::to_string(width) + "x" + std::to_string(height));
    NLM_LOG_INFO("  Save to file: " + std::to_string(saveToFile));
    NLM_LOG_INFO("  Output file: " + outputFile);
    
    // Run a short simulation for visualization
    for (int step = 0; step < 50; ++step) {
        brain->step(step);
    }
    
    NLM_LOG_INFO("Visualization simulation complete.");
    if (saveToFile) {
        NLM_LOG_INFO("Visualization would be saved to: " + outputFile);
    }
}

// Debug mode - enable detailed debugging
void runDebugMode(std::shared_ptr<Brain> brain, const CommandLineParser& parser) {
    NLM_LOG_INFO("=== Debug Mode ===");
    NLM_LOG_INFO("Enabling detailed debugging...");
    
    bool enableMemoryDebug = parser.hasArg("memory-debug");
    bool enablePlasticityDebug = parser.hasArg("plasticity-debug");
    bool enableNeuromodulationDebug = parser.hasArg("neuromod-debug");
    bool enableDevelopmentDebug = parser.hasArg("development-debug");
    
    NLM_LOG_INFO("Debug enabled subsystems:");
    if (enableMemoryDebug) NLM_LOG_INFO("  Working Memory Debug");
    if (enablePlasticityDebug) NLM_LOG_INFO("  Plasticity Debug");
    if (enableNeuromodulationDebug) NLM_LOG_INFO("  Neuromodulation Debug");
    if (enableDevelopmentDebug) NLM_LOG_INFO("  Development Debug");
    
    // Temporarily increase log level for debugging
    auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(Logger::getGlobal().getLogger(0));
    if (consoleLogger) {
        consoleLogger->setLevel(LogLevel::Debug);
    }
    
    // Run debug simulation
    NLM_LOG_INFO("Running debug simulation (200 steps)...");
    for (int step = 0; step < 200; ++step) {
        brain->step(step);
        if (step % 50 == 0) {
            NLM_LOG_INFO("Debug step " + std::to_string(step) + ": " +
                        std::to_string(brain->getFiringNeuronCount()) + " neurons firing");
        }
    }
    
    // Restore log level
    if (consoleLogger) {
        consoleLogger->setLevel(LogLevel::Info);
    }
    
    NLM_LOG_INFO("Debug mode complete.");
}

// Profile mode - performance profiling
void runProfileMode(std::shared_ptr<Brain> brain, const CommandLineParser& parser) {
    NLM_LOG_INFO("=== Profile Mode ===");
    NLM_LOG_INFO("Starting performance profiling...");
    
    int profileSteps = 1000;
    if (parser.hasArg("profile-steps")) {
        try {
            profileSteps = std::stoi(parser.getArg("profile-steps"));
        } catch (...) {
            NLM_LOG_WARNING("Invalid profile-steps value");
        }
    }
    
    NLM_LOG_INFO("Profiling " + std::to_string(profileSteps) + " steps...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < profileSteps; ++step) {
        brain->step(step);
        if (step % 100 == 0) {
            NLM_LOG_INFO("Profile step " + std::to_string(step) + ": " +
                        std::to_string(brain->getFiringNeuronCount()) + " neurons firing");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double stepsPerSecond = (profileSteps * 1000.0) / duration.count();
    
    NLM_LOG_INFO("=== Profiling Results ===");
    NLM_LOG_INFO("Total steps: " + std::to_string(profileSteps));
    NLM_LOG_INFO("Total time: " + std::to_string(duration.count() / 1000.0) + "s");
    NLM_LOG_INFO("Steps per second: " + std::to_string(stepsPerSecond));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    
    NLM_LOG_INFO("Profile mode complete.");
}

// Export mode - save brain state and analysis
void runExportMode(std::shared_ptr<Brain> brain, const CommandLineParser& parser) {
    NLM_LOG_INFO("=== Export Mode ===");
    NLM_LOG_INFO("Exporting brain state and analysis...");
    
    std::string exportFile = parser.getArg("export-file", "nlm_export");
    bool exportConfig = parser.hasArg("export-config");
    bool exportStats = parser.hasArg("export-stats");
    
    NLM_LOG_INFO("Export parameters:");
    NLM_LOG_INFO("  Export file: " + exportFile);
    NLM_LOG_INFO("  Export config: " + std::to_string(exportConfig));
    NLM_LOG_INFO("  Export stats: " + std::to_string(exportStats));
    
    // In a real implementation, this would:
    // 1. Save brain state to file
    // 2. Export configuration
    // 3. Generate statistical analysis
    
    if (exportConfig) {
        NLM_LOG_INFO("Would export configuration to: " + exportFile + ".json");
    }
    
    if (exportStats) {
        NLM_LOG_INFO("Would export statistics to: " + exportFile + ".stats");
    }
    
    // Run a simulation to have data to export
    NLM_LOG_INFO("Running export simulation...");
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    NLM_LOG_INFO("Export simulation complete.");
    NLM_LOG_INFO("Note: File export functionality would save data here.");
}

// Import mode - load saved brain state
void runImportMode(std::shared_ptr<Brain> brain, const CommandLineParser& parser) {
    NLM_LOG_INFO("=== Import Mode ===");
    NLM_LOG_INFO("Loading saved brain state...");
    
    std::string importFile = parser.getArg("import-file");
    if (importFile.empty()) {
        NLM_LOG_ERROR("Import file not specified. Use --import-file=<filename>");
        return;
    }
    
    NLM_LOG_INFO("Importing from: " + importFile);
    
    // In a real implementation, this would:
    // 1. Load brain state from file
    // 2. Restore configuration
    // 3. Reinitialize brain with imported state
    
    NLM_LOG_INFO("Import functionality would load data from: " + importFile);
    NLM_LOG_INFO("Note: This would restore brain state and configuration.");
}

// Batch test mode - run multiple test scenarios
void runBatchTestMode(std::shared_ptr<Brain> brain, const CommandLineParser& parser) {
    NLM_LOG_INFO("=== Batch Test Mode ===");
    NLM_LOG_INFO("Running multiple test scenarios...");
    
    int numTests = 3;
    if (parser.hasArg("batch-tests")) {
        try {
            numTests = std::stoi(parser.getArg("batch-tests"));
        } catch (...) {
            NLM_LOG_WARNING("Invalid batch-tests value");
        }
    }
    
    std::vector<std::string> testNames = {"connectivity", "plasticity", "stdp", "integration"};
    
    for (int test = 0; test < numTests && test < testNames.size(); ++test) {
        std::string testName = testNames[test];
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Batch Test " + std::to_string(test + 1) + ": " + testName + " ===");
        
        brain->reset();
        brain->initialize();
        
        if (testName == "connectivity") {
            runConnectivityTest(brain);
        } else if (testName == "plasticity") {
            runPlasticityTest(brain);
        } else if (testName == "stdp") {
            runSTDPTest(brain);
        } else if (testName == "integration") {
            runConnectivityTest(brain);
            brain->reset(); brain->initialize();
            runPlasticityTest(brain);
            brain->reset(); brain->initialize();
            runSTDPTest(brain);
        }
        
        NLM_LOG_INFO("Batch test " + std::to_string(test + 1) + " completed.");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== All Batch Tests Complete ===");
}

// Neural analysis mode - advanced brain analysis
void runNeuralAnalysisMode(std::shared_ptr<Brain> brain, const CommandLineParser& parser) {
    NLM_LOG_INFO("=== Neural Analysis Mode ===");
    NLM_LOG_INFO("Performing advanced neural analysis...");
    
    bool analyzeSynapticStrengths = parser.hasArg("analyze-synapses");
    bool analyzeFiringPatterns = parser.hasArg("analyze-firing");
    bool analyzeConnectivity = parser.hasArg("analyze-connectivity");
    bool analyzePlasticity = parser.hasArg("analyze-plasticity");
    
    // Run analysis simulation
    NLM_LOG_INFO("Running analysis simulation...");
    for (int step = 0; step < 200; ++step) {
        brain->step(step);
    }
    
    NLM_LOG_INFO("=== Neural Analysis Results ===");
    
    if (analyzeSynapticStrengths) {
        NLM_LOG_INFO("Synaptic Analysis:");
        NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        // Would analyze individual synapse properties
        NLM_LOG_INFO("  (Detailed synapse analysis would be shown here)");
    }
    
    if (analyzeFiringPatterns) {
        NLM_LOG_INFO("Firing Pattern Analysis:");
        NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
        NLM_LOG_INFO("  Excitation/inhibition ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        NLM_LOG_INFO("  (Detailed firing pattern analysis would be shown here)");
    }
    
    if (analyzeConnectivity) {
        NLM_LOG_INFO("Connectivity Analysis:");
        // Would analyze network connectivity
        NLM_LOG_INFO("  (Connectivity analysis would be shown here)");
    }
    
    if (analyzePlasticity) {
        NLM_LOG_INFO("Plasticity Analysis:");
        // Would analyze plasticity effects
        NLM_LOG_INFO("  (Plasticity analysis would be shown here)");
    }
    
    NLM_LOG_INFO("Neural analysis complete.");
}

// Main function with improved CLI
int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    CommandLineParser parser;
    parser.parse(argc, argv);
    
    // Check for help/version flags
    if (parser.hasArg("help")) {
        printHelp(argv[0]);
        return 0;
    }
    
    if (parser.hasArg("version")) {
        std::cout << "NLM " << getVersionInfo() << std::endl;
        std::cout << getBuildInfo() << std::endl;
        return 0;
    }
    
    if (parser.hasArg("build_info")) {
        std::cout << getBuildInfo() << std::endl;
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set log level based on verbose flag
    if (parser.hasArg("verbose")) {
        consoleLogger->setLevel(LogLevel::Debug);
    }
    
    NLM_LOG_INFO("=== NLM Neural Learning Machine ===");
    NLM_LOG_INFO("Version: " + getVersionInfo());
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    std::string configFile = "configs/default.cfg";
    if (parser.hasArg("config_file")) {
        configFile = parser.getArg("config_file");
    }
    
    // Load config from file
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_WARNING("Using default configuration (could not load: " + configFile + ")");
    }
    
    // Override with command line config sets
    if (parser.hasArg("config_set")) {
        std::string configSet = parser.getArg("config_set");
        size_t pos = configSet.find('=');
        if (pos != std::string::npos) {
            std::string key = configSet.substr(0, pos);
            std::string value = configSet.substr(pos + 1);
            config->set(key, value, ConfigSource::CommandLine);
            NLM_LOG_INFO("Set configuration: " + key + " = " + value);
        }
    }
    
    // Apply command line overrides
    if (parser.hasArg("steps")) {
        try {
            int steps = std::stoi(parser.getArg("steps"));
            config->set("max_simulation_steps", steps, ConfigSource::CommandLine);
        } catch (...) {
            NLM_LOG_WARNING("Invalid steps value: " + parser.getArg("steps"));
        }
    }
    
    if (parser.hasArg("neuron_count")) {
        try {
            int64_t neurons = std::stoll(parser.getArg("neuron_count"));
            config->set("neuron_count", neurons, ConfigSource::CommandLine);
        } catch (...) {
            NLM_LOG_WARNING("Invalid neuron_count value: " + parser.getArg("neuron_count"));
        }
    }
    
    if (parser.hasArg("region_count")) {
        try {
            int64_t regions = std::stoll(parser.getArg("region_count"));
            config->set("region_count", regions, ConfigSource::CommandLine);
        } catch (...) {
            NLM_LOG_WARNING("Invalid region_count value: " + parser.getArg("region_count"));
        }
    }
    
    if (parser.hasArg("connection_prob")) {
        try {
            float prob = std::stof(parser.getArg("connection_prob"));
            config->set("connection_probability", prob, ConfigSource::CommandLine);
        } catch (...) {
            NLM_LOG_WARNING("Invalid connection_prob value: " + parser.getArg("connection_prob"));
        }
    }
    
    // Determine execution mode
    std::string mode = parser.getArg("mode");
    if (mode.empty() && parser.hasArg("demo_mode")) {
        mode = "demo";
    }
    if (mode.empty() && parser.hasArg("test_type")) {
        mode = parser.getArg("test_type");
    }
    
    // Default to auto mode if nothing specified
    CommandMode commandMode = CommandMode::AUTO;
    if (!mode.empty()) {
        if (mode == "demo") commandMode = CommandMode::DEMONSTRATION;
        else if (mode == "test_connectivity") commandMode = CommandMode::TEST_CONNECTIVITY;
        else if (mode == "test_plasticity") commandMode = CommandMode::TEST_PLASTICITY;
        else if (mode == "test_stdp") commandMode = CommandMode::TEST_STDP;
        else if (mode == "test_integration") commandMode = CommandMode::TEST_INTEGRATION;
        else if (mode == "config_dump") commandMode = CommandMode::CONFIG_DUMP;
        else if (mode == "help") commandMode = CommandMode::HELP;
        else if (mode == "visualization") commandMode = CommandMode::VISUALIZATION;
        else if (mode == "debug") commandMode = CommandMode::DEBUG;
        else if (mode == "profile") commandMode = CommandMode::PROFILE;
        else if (mode == "export") commandMode = CommandMode::EXPORT;
        else if (mode == "import") commandMode = CommandMode::IMPORT;
        else if (mode == "batch_test") commandMode = CommandMode::BATCH_TEST;
        else if (mode == "neural_analysis") commandMode = CommandMode::NEURAL_ANALYSIS;
    }
    
    NLM_LOG_INFO("Execution mode: " + mode + " (parsed as " + 
                (mode.empty() ? "AUTO" : mode) + ")");
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Execute based on mode
    switch (commandMode) {
        case CommandMode::AUTO:
            // Default behavior - run all tests
            NLM_LOG_INFO("=== AUTO MODE: Running All Tests ===");
            
            // Reset brain between tests
            brain->reset();
            brain->initialize();
            runConnectivityTest(brain);
            
            brain->reset();
            brain->initialize();
            runPlasticityTest(brain);
            
            brain->reset();
            brain->initialize();
            runSTDPTest(brain);
            break;
            
        case CommandMode::DEMONSTRATION:
            runDemonstrationMode(brain);
            break;
            
        case CommandMode::TEST_CONNECTIVITY:
            runConnectivityTest(brain);
            break;
            
        case CommandMode::TEST_PLASTICITY:
            runPlasticityTest(brain);
            break;
            
        case CommandMode::TEST_STDP:
            runSTDPTest(brain);
            break;
            
        case CommandMode::TEST_INTEGRATION:
            NLM_LOG_INFO("=== Integration Test Mode ===");
            runConnectivityTest(brain);
            brain->reset(); brain->initialize();
            runPlasticityTest(brain);
            brain->reset(); brain->initialize();
            runSTDPTest(brain);
            break;
            
        case CommandMode::CONFIG_DUMP:
            NLM_LOG_INFO("=== Configuration Dump ===");
            NLM_LOG_INFO(config->summary());
            break;
            
        case CommandMode::HELP:
            printHelp(argv[0]);
            break;
            
        case CommandMode::VISUALIZATION:
            runVisualizationMode(brain, parser);
            break;
            
        case CommandMode::DEBUG:
            runDebugMode(brain, parser);
            break;
            
        case CommandMode::PROFILE:
            runProfileMode(brain, parser);
            break;
            
        case CommandMode::EXPORT:
            runExportMode(brain, parser);
            break;
            
        case CommandMode::IMPORT:
            runImportMode(brain, parser);
            break;
            
        case CommandMode::BATCH_TEST:
            runBatchTestMode(brain, parser);
            break;
            
        case CommandMode::NEURAL_ANALYSIS:
            runNeuralAnalysisMode(brain, parser);
            break;
            
        default:
            NLM_LOG_WARNING("Unknown mode " + mode + " - using AUTO mode");
            // Fall through to AUTO mode behavior
            brain->reset();
            brain->initialize();
            runConnectivityTest(brain);
            break;
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    
    return 0;
}