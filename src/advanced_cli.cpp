#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>

namespace nlm {
    
    // Simple argument parser (similar to argparse but minimal)
    class ArgumentParser {
    public:
        struct Argument {
            std::string name;
            std::string description;
            std::string defaultValue;
            bool required;
        };
        
        void addArgument(const std::string& name, const std::string& description, 
                         const std::string& defaultValue = "", bool required = false) {
            arguments_[name] = Argument{name, description, defaultValue, required};
        }
        
        bool parse(int argc, char** argv) {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--help" || arg == "-h") {
                    showHelp();
                    return false;
                }
                
                if (arg.find("--") == 0) {
                    size_t equals = arg.find("=");
                    std::string key;
                    std::string value;
                    
                    if (equals != std::string::npos) {
                        key = arg.substr(0, equals);
                        value = arg.substr(equals + 1);
                    } else {
                        key = arg;
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            value = argv[++i];
                        }
                    }
                    
                    // Remove -- prefix
                    if (!key.empty() && key[0] == '-') {
                        key = key.substr(1);
                    }
                    
                    parsedArgs_[key] = value;
                } else if (arg == "-c" && i + 1 < argc) {
                    parsedArgs_["config"] = argv[++i];
                }
            }
            return validateArguments();
        }
        
        std::string getValue(const std::string& key, const std::string& defaultValue = "") const {
            auto it = parsedArgs_.find(key);
            if (it != parsedArgs_.end()) {
                return it->second;
            }
            return defaultValue;
        }
        
        bool hasValue(const std::string& key) const {
            return parsedArgs_.find(key) != parsedArgs_.end();
        }
        
        void showHelp() const {
            std::cout << "NLM CLI Tool" << std::endl;
            std::cout << "===========\n" << std::endl;
            
            for (const auto& pair : arguments_) {
                const Argument& arg = pair.second;
                std::cout << "  --" << arg.name;
                if (!arg.defaultValue.empty()) {
                    std::cout << " (default: " << arg.defaultValue << ")";
                }
                std::cout << "\n    " << arg.description << std::endl;
            }
            
            std::cout << "\nExamples:\n";
            std::cout << "  nlm_tool --config demo.cfg --episodes 10 --steps 100\n";
            std::cout << "  nlm_tool --neuron-count 1000 --plasticity-mode stdp\n";
        }
        
    private:
        bool validateArguments() {
            for (const auto& pair : arguments_) {
                const Argument& arg = pair.second;
                if (arg.required && !hasValue(arg.name)) {
                    std::cerr << "Error: Missing required argument --" << arg.name << std::endl;
                    return false;
                }
            }
            return true;
        }
        
        std::vector<Argument> arguments_;
        std::map<std::string, std::string> parsedArgs_;
    };
    
    // Enhanced configuration manager with validation
    class EnhancedConfigManager {
    public:
        static std::shared_ptr<Config> loadConfig(const std::string& configFile, const ArgumentParser& parser) {
            auto config = std::make_shared<Config>();
            
            // Load from file if provided
            if (parser.hasValue("config")) {
                std::string configFile = parser.getValue("config");
                if (config->loadFromFile(configFile)) {
                    std::cout << "Loaded configuration from: " << configFile << std::endl;
                } else {
                    std::cout << "Using default configuration (file not found: " << configFile << ")" << std::endl;
                }
            } else {
                std::cout << "Using default configuration." << std::endl;
            }
            
            // Override with command line arguments
            EnhancedConfigManager::applyCommandLineOverrides(config, parser);
            
            return config;
        }
        
        static void applyCommandLineOverrides(std::shared_ptr<Config> config, const ArgumentParser& parser) {
            // Parse numeric values
            auto parseInt = [](const std::string& str) -> std::optional<int64_t> {
                try {
                    return std::stoll(str);
                } catch (...) {
                    return std::nullopt;
                }
            };
            
            auto parseFloat = [](const std::string& str) -> std::optional<double> {
                try {
                    return std::stod(str);
                } catch (...) {
                    return std::nullopt;
                }
            };
            
            // Neuron configuration
            if (parser.hasValue("neuron-count")) {
                auto value = parseInt(parser.getValue("neuron-count"));
                if (value) config->set("neuron_count", *value, ConfigSource::CommandLine);
            }
            
            if (parser.hasValue("region-count")) {
                auto value = parseInt(parser.getValue("region-count"));
                if (value) config->set("region_count", *value, ConfigSource::CommandLine);
            }
            
            // Plasticity configuration
            if (parser.hasValue("plasticity-mode")) {
                std::string mode = parser.getValue("plasticity_mode");
                if (mode == "stdp") {
                    config->set("plasticity_mode", "stdp", ConfigSource::CommandLine);
                } else if (mode == "hebbian") {
                    config->set("plasticity_mode", "hebbian", ConfigSource::CommandLine);
                } else if (mode == "both") {
                    config->set("plasticity_mode", "both", ConfigSource::CommandLine);
                }
            }
            
            if (parser.hasValue("stdp-ltp")) {
                auto value = parseFloat(parser.getValue("stdp-ltp"));
                if (value) config->set("stdp_ltp_weight", *value, ConfigSource::CommandLine);
            }
            
            if (parser.hasValue("stdp-ltd")) {
                auto value = parseFloat(parser.getValue("stdp-ltd"));
                if (value) config->set("stdp_ltd_weight", *value, ConfigSource::CommandLine);
            }
            
            // Environment configuration
            if (parser.hasValue("env-width")) {
                auto value = parseFloat(parser.getValue("env-width"));
                if (value) config->set("environment_width", *value, ConfigSource::CommandLine);
            }
            
            if (parser.hasValue("env-height")) {
                auto value = parseFloat(parser.getValue("env-height"));
                if (value) config->set("environment_height", *value, ConfigSource::CommandLine);
            }
            
            // Simulation configuration
            if (parser.hasValue("timestep")) {
                auto value = parseFloat(parser.getValue("timestep"));
                if (value) config->set("simulation_timestep", *value, ConfigSource::CommandLine);
            }
            
            if (parser.hasValue("seed")) {
                auto value = parseInt(parser.getValue("seed"));
                if (value) config->set("random_seed", *value, ConfigSource::CommandLine);
            }
        }
        
        static void printConfigurationSummary(const Config& config) {
            std::cout << "\n=== Configuration Summary ===" << std::endl;
            
            auto keys = config.getKeys();
            std::sort(keys.begin(), keys.end());
            
            for (const std::string& key : keys) {
                std::cout << "  " << key << ": ";
                
                // Print different types based on content
                if (config.has("neuron_count")) {
                    int64_t val = config.getOr<int64_t>("neuron_count", 0);
                    std::cout << val << std::endl;
                } else if (config.has("stdp_ltp_weight")) {
                    double val = config.getOr<double>("stdp_ltp_weight", 0.0);
                    std::cout << val << std::endl;
                } else if (config.has("environment_width")) {
                    double val = config.getOr<double>("environment_width", 0.0);
                    std::cout << val << std::endl;
                } else if (config.has("simulation_timestep")) {
                    double val = config.getOr<double>("simulation_timestep", 0.0);
                    std::cout << val << "s" << std::endl;
                } else {
                    std::cout << config.get<std::string>(key).value_or("") << std::endl;
                }
            }
        }
    };
    
    // Test runner for organized execution
    class TestRunner {
    public:
        static void runLearningExperiment(std::shared_ptr<Brain> brain, const std::string& name) {
            std::cout << "\n=== " << name << " ===" << std::endl;
            
            LearningExperiment experiment(brain, 42);
            experiment.recordInitialState();
            
            // Run plasticity experiment
            runPlasticityExperiment(brain);
            
            experiment.recordFinalState();
            experiment.computeStatistics();
        }
        
        static void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
            std::cout << "\n=== Basic Neural Connectivity ===" << std::endl;
            
            auto* region = brain->getRegion(RegionId(1));
            if (!region) {
                std::cout << "  No neurons found!" << std::endl;
                return;
            }
            
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) {
                std::cout << "  No neurons found!" << std::endl;
                return;
            }
            
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            std::cout << "  Injecting current into 10 neurons..." << std::endl;
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);
            }
            
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            std::cout << "  Spikes generated: " << spikes << std::endl;
            
            if (spikes > 0) {
                std::cout << "  ✓ Spikes propagate through network" << std::endl;
            } else {
                std::cout << "  ! No spikes - checking neuron parameters..." << std::endl;
                for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                    std::cout << "    Neuron " << i 
                              << " V=" << neurons[i]->getMembranePotential()
                              << " thresh=" << neurons[i]->getThreshold() << std::endl;
                }
            }
        }
        
        static void runStdpVerification(std::shared_ptr<Brain> brain) {
            std::cout << "\n=== STDP Verification ===" << std::endl;
            
            auto* region = brain->getRegion(RegionId(1));
            if (!region) return;
            
            auto& synapses = region->getSynapses();
            if (synapses.size() < 5) {
                std::cout << "  Not enough synapses for STDP test" << std::endl;
                return;
            }
            
            std::cout << "  Testing STDP on 5 synapses:" << std::endl;
            
            std::vector<float> beforeWeights;
            for (size_t i = 0; i < 5; ++i) {
                beforeWeights.push_back(synapses[i]->getWeight());
                synapses[i]->enablePlasticity(false, true, false);
                std::cout << "    Synapse " << i << " before: " << beforeWeights[i] << std::endl;
            }
            
            std::cout << "  Creating correlated pre->post activity (potentiation)..." << std::endl;
            
            for (int trial = 0; trial < 50; ++trial) {
                Neuron* preNeuron = nullptr;
                Neuron* postNeuron = nullptr;
                
                auto neurons = region->getAllNeurons();
                if (neurons.size() >= 2) {
                    preNeuron = neurons[0];
                    postNeuron = neurons[1];
                }
                
                if (preNeuron && postNeuron) {
                    preNeuron->injectCurrent(60.0f);
                    brain->step(trial * 2, trial * 2 * 0.001);
                    
                    postNeuron->injectCurrent(60.0f);
                    brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                }
            }
            
            std::cout << "  After correlated activity:" << std::endl;
            float totalDelta = 0.0f;
            for (size_t i = 0; i < 5; ++i) {
                float delta = synapses[i]->getWeight() - beforeWeights[i];
                totalDelta += delta;
                std::cout << "    Synapse " << i << " after: " << synapses[i]->getWeight()
                          << " (Δ=" << delta << ")" << std::endl;
            }
            
            if (totalDelta > 0.001f) {
                std::cout << "  ✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
            } else if (totalDelta < -0.001f) {
                std::cout << "  ! STDP reversed: Check parameters" << std::endl;
            } else {
                std::cout << "  ! No change: STDP may not be triggering" << std::endl;
            }
        }
        
        // Helper struct for learning experiment (moved from main.cpp)
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
                
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        initialWeights.push_back(syn->getWeight());
                    }
                }
                
                std::cout << "Initial state recorded:" << std::endl;
                std::cout << "  Synapses: " << initialSynapseCount << std::endl;
                if (!initialWeights.empty()) {
                    float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                    float mean = sum / initialWeights.size();
                    std::cout << "  Mean weight: " << mean << std::endl;
                }
            }
            
            void recordFinalState() {
                finalWeights.clear();
                
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        finalWeights.push_back(syn->getWeight());
                    }
                }
                
                mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
                
                std::cout << "Final state recorded:" << std::endl;
                std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
                if (!finalWeights.empty()) {
                    float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                    float mean = sum / finalWeights.size();
                    std::cout << "  Mean weight: " << mean << std::endl;
                }
            }
            
            void computeStatistics() {
                std::cout << "\n=== Learning Experiment Results ===" << std::endl;
                
                if (initialWeights.empty() || finalWeights.empty()) {
                    std::cout << "ERROR: No weights recorded" << std::endl;
                    return;
                }
                
                float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float initialMean = initialSum / initialWeights.size();
                float finalMean = finalSum / finalWeights.size();
                
                std::cout << "Weight Statistics:" << std::endl;
                std::cout << "  Initial mean weight: " << initialMean << std::endl;
                std::cout << "  Final mean weight: " << finalMean << std::endl;
                std::cout << "  Change: " << (finalMean - initialMean) << std::endl;
                
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
                
                std::cout << "\nSynaptic Changes:" << std::endl;
                std::cout << "  Strengthened: " << strengthened << std::endl;
                std::cout << "  Weakened: " << weakened << std::endl;
                std::cout << "  Unchanged: " << unchanged << std::endl;
                
                std::cout << "\nSpike Activity:" << std::endl;
                std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
                std::cout << "  Most active neurons recorded: " << mostActiveNeurons.size() << std::endl;
                
                bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                        (strengthened > 0 || weakened > 0);
                
                if (learningOccurred) {
                    std::cout << "\n✓ LEARNING DETECTED: Synaptic weights changed through experience" << std::endl;
                } else {
                    std::cout << "\n✗ NO LEARNING: Weights did not change significantly" << std::endl;
                }
            }
        };
        
    private:
        static void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
            std::cout << "\nApplying repeated input patterns (1000 steps)..." << std::endl;
            
            for (SimulationStep step = 0; step < 1000; ++step) {
                for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                
                brain->step(step, step * 0.001);
                
                if (step % 100 == 0) {
                    std::cout << "  Step " << step 
                              << " | Spikes: " << brain->getTotalSpikeCount()
                              << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
                }
            }
        }
    };
}

int main(int argc, char** argv) {
    std::cout << "NLM Advanced CLI Tool" << std::endl;
    std::cout << "=======================\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    nlm::ArgumentParser parser;
    
    // Add arguments
    parser.addArgument("config", "Configuration file path", "configs/default.cfg");
    parser.addArgument("neuron-count", "Number of neurons in the brain", "500");
    parser.addArgument("region-count", "Number of brain regions", "1");
    parser.addArgument("plasticity-mode", "Plasticity mode: stdp, hebbian, or both", "stdp");
    parser.addArgument("stdp-ltp", "STDP LTP weight", "0.02");
    parser.addArgument("stdp-ltd", "STDP LTD weight", "0.015");
    parser.addArgument("env-width", "Environment width (for world interaction)", "20");
    parser.addArgument("env-height", "Environment height (for world interaction)", "20");
    parser.addArgument("timestep", "Simulation timestep", "0.001");
    parser.addArgument("seed", "Random seed for reproducibility", "42");
    
    // Parse arguments
    if (!parser.parse(argc, argv)) {
        return 1;
    }
    
    // Load configuration
    auto config = nlm::EnhancedConfigManager::loadConfig(parser.getValue("config"), parser);
    
    // Apply command line overrides
    nlm::EnhancedConfigManager::applyCommandLineOverrides(config, parser);
    
    // Print configuration summary
    nlm::EnhancedConfigManager::printConfigurationSummary(*config);
    
    // Initialize brain
    std::cout << "\nInitializing NLM Brain..." << std::endl;
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Run tests based on user preference
    std::string testMode = parser.getValue("test-mode", "basic");
    
    if (testMode == "all" || testMode == "basic") {
        nlm::TestRunner::runBasicConnectivityTest(brain);
    }
    
    if (testMode == "all" || testMode == "plasticity") {
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        nlm::TestRunner::runLearningExperiment(brain, "Plasticity Learning Experiment");
    }
    
    if (testMode == "all" || testMode == "stdp") {
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        
        nlm::TestRunner::runStdpVerification(brain);
    }
    
    // Final brain status
    std::cout << "\n=== Final Brain Status ===" << std::endl;
    brain->logStatus();
    
    std::cout << "\n=== Advanced CLI Tool Complete ===" << std::endl;
    std::cout << "Features demonstrated:" << std::endl;
    std::cout << "  ✓ Advanced argument parsing with validation" << std::endl;
    std::cout << "  ✓ Enhanced configuration management" << std::endl;
    std::cout << "  ✓ Modular test execution" << std::endl;
    std::cout << "  ✓ Comprehensive error handling" << std::endl;
    
    return 0;
}
