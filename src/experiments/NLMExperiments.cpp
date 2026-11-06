// NLMExperiments.cpp - Implementation of experimental framework utilities

#include "src/experiments/NLMExperiments.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <ctime>

namespace nlm {

// LearningExperiment implementations
void LearningExperiment::recordInitialState() {
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

void LearningExperiment::recordFinalState() {
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

void LearningExperiment::computeStatistics() {
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

bool LearningExperiment::hasLearningOccurred() const {
    if (initialWeights.empty() || finalWeights.empty()) return false;
    
    float initialMean = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f) / initialWeights.size();
    float finalMean = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f) / finalWeights.size();
    
    float weightChange = std::abs(finalMean - initialMean);
    
    // Check for significant weight changes
    for (size_t i = 0; i < std::min(initialWeights.size(), finalWeights.size()); ++i) {
        float delta = finalWeights[i] - initialWeights[i];
        if (delta > 0.01f || delta < -0.01f) return true;
    }
    
    return weightChange > 0.001f;
}

float LearningExperiment::getWeightChangePercentage() const {
    if (initialWeights.empty() || finalWeights.empty()) return 0.0f;
    
    float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
    float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
    
    return ((finalSum - initialSum) / initialSum) * 100.0f;
}

// Command line parsing implementations
NLMCommandLine::Options NLMCommandLine::parse(int argc, char* argv[]) {
    Options options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version" || arg == "-v") {
            options.version = true;
        } else if (arg == "--verbose" || arg == "-V") {
            options.verbose = true;
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                options.logLevel = argv[++i];
            }
        } else if (arg == "--log-file") {
            if (i + 1 < argc) {
                options.logFile = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg == "--seed" || arg == "-s") {
            if (i + 1 < argc) {
                options.randomSeed = std::stol(argv[++i]);
            }
        } else if (arg == "--timestep") {
            if (i + 1 < argc) {
                options.simulationTimestep = std::stod(argv[++i]);
            }
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                options.neuronCount = std::stol(argv[++i]);
            }
        } else if (arg == "--regions") {
            if (i + 1 < argc) {
                options.regionCount = std::stol(argv[++i]);
            }
        } else if (arg == "--conn-prob" || arg == "-c") {
            if (i + 1 < argc) {
                options.connectionProbability = std::stof(argv[++i]);
            }
        }
    }
    
    return options;
}

void NLMCommandLine::printHelp(const char* programName) {
    std::cout << "Usage: " << programName << " [options] [demo]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM - Neural Learning Machine - Demo Executer" << std::endl;
    std::cout << "Run neural demonstrations and experiments" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
    std::cout << "  -v, --version                 Show version information" << std::endl;
    std::cout << "  -V, --verbose                 Enable verbose output" << std::endl;
    std::cout << "  -o, --output <file>           Save output to file" << std::endl;
    std::cout << "  --log-level <level>           Set log level (DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cout << "  --log-file <file>             Write logs to file" << std::endl;
    std::cout << "  --config <file>               Load configuration file" << std::endl;
    std::cout << "  -s, --seed <seed>             Set random seed" << std::endl;
    std::cout << "  --timestep <steps>             Set simulation timestep" << std::endl;
    std::cout << "  -n, --neurons <count>         Set number of neurons" << std::endl;
    std::cout << "  --regions <count>             Set number ympäristöö" << std::endl;
    std::cout << "  -c, --conn-prob <prob>        Set connection probability" << std::endl;
    std::cout << std::endl;
    std::cout << "Available demos:" << std::endl;
    std::cout << "  phase2      Run Phase 2 demonstration (basic neural computation)" << std::endl;
    std::cout << "  phase3      Run Phase 3 demonstration (world interaction)" << std::endl;
    std::cout << "  phase4      Run Phase 4 demonstration (emerging cognition)" << std::endl;
    std::cout << "  phase6      Run Phase 6 demonstration (integration test)" << std::endl;
    std::cout << "  benchmark   Run performance benchmarks" << std::endl;
    std::cout << "  all         Run all demonstrations" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --neurons 1000 --timestep 0.001 phase3" << std::endl;
    std::cout << "  " << programName << " -s 12345 -n 2000 --conn-prob 0.2 all" << std::endl;
}

void NLMCommandLine::printVersion() {
    std::cout << "NLM - Neural Learning Machine " << "version 1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "Built with support for all neural systems and memory integration" << std::endl;
}

bool NLMCommandLine::saveConfiguration(const Options& options, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM command line tool" << std::endl;
    file << std::endl;
    
    file << "[general]" << std::endl;
    file << "random_seed = " << options.randomSeed << std::endl;
    file << "simulation_timestep = " << options.simulationTimestep << std::endl;
    file << "verbose = " << (options.verbose ? "true" : "false") << std::endl;
    file << std::endl;
    
    file << "[brain]" << std::endl;
    file << "neuron_count = " << options.neuronCount << std::endl;
    file << "region_count = " << options.regionCount << std::endl;
    file << "connection_probability = " << options.connectionProbability << std::endl;
    file << std::endl;
    
    file << "[logging]" << std::endl;
    file << "level = " << options.logLevel << std::endl;
    if (!options.logFile.empty()) {
        file << "file = " << options.logFile << std::endl;
    }
    
    file.close();
    return true;
}

bool NLMCommandLine::loadConfiguration(const Options& baseOptions, Options& result) {
    std::ifstream file(baseOptions.configFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Simple configuration parser - skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim whitespace
        size_t firstNonSpace = line.find_first_not_of(" \t\r\n");
        if (firstNonSpace == std::string::npos) continue;
        line = line.substr(firstNonSpace);
        
        size_t lastNonSpace = line.find_last_not_of(" \t\r\n");
        if (lastNonSpace != std::string::npos) {
            line = line.substr(0, lastNonSpace + 1);
        }
        
        if (line.empty()) continue;
        
        // Parse key = value pairs
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Trim key and value
            size_t keyFirst = key.find_first_not_of(" \t");
            if (keyFirst != std::string::npos) {
                key = key.substr(keyFirst);
                size_t keyLast = key.find_last_not_of(" \t");
                if (keyLast != std::string::npos) {
                    key = key.substr(0, keyLast + 1);
                }
            }
            
            size_t valFirst = value.find_first_not_of(" \t");
            if (valFirst != std::string::npos) {
                value = value.substr(valFirst);
                size_t valLast = value.find_last_not_of(" \t");
                if (valLast != std::string::npos) {
                    value = value.substr(0, valLast + 1);
                }
            }
            
            // Apply configuration based on key
            if (key == "random_seed") {
                result.randomSeed = std::stol(value);
            } else if (key == "simulation_timestep") {
                result.simulationTimestep = std::stod(value);
            } else if (key == "neuron_count") {
                result.neuronCount = std::stol(value);
            } else if (key == "region_count") {
                result.regionCount = std::stol(value);
            } else if (key == "connection_probability") {
                result.connectionProbability = std::stof(value);
            } else if (key == "verbose") {
                result.verbose = (value == "true" || value == "1" || value == "yes");
            } else if (key == "log_level") {
                result.logLevel = value;
            }
        }
    }
    
    file.close();
    return true;
}

// NeuralDemo base class implementations
std::shared_ptr<Config> NeuralDemo::createDefaultConfig() const {
    auto config = std::make_shared<Config>();
    
    // Set default values that are common across demos
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    
    return config;
}

void NeuralDemo::logStep(int step, float reward, float firingRate, size_t traces) const {
    // Helper method for logging simulation steps
    if (step % 100 == 0) {
        NLM_LOG_INFO("Step " + std::to_string(step) + 
                    " | Reward: " + std::to_string(reward) +
                    " | Firing Rate: " + std::to_string(firingRate) +
                    " | Working Mem: " + std::to_string(traces));
    }
}

// DemoFactory implementations
NeuralDemo* DemoFactory::createDemo(DemoType type) {
    switch (type) {
        case DemoType::PHASE2:
            // Return phase 2 demo instance
            return nullptr;
        case DemoType::PHASE3:
            // Return phase 3 demo instance
            return nullptr;
        case DemoType::PHASE4:
            // Return phase 4 demo instance
            return nullptr;
        case DemoType::PHASE6:
            // Return phase 6 demo instance
            return nullptr;
        case DemoType::BENCHMARK:
            // Return benchmark demo instance
            return nullptr;
        case DemoType::ALL:
            // Return multi-demo controller
            return nullptr;
        default:
            return nullptr;
    }
}

std::vector<DemoFactory::DemoType> DemoFactory::parseDemoTypes(const std::string& demoList) {
    std::vector<DemoType> types;
    
    // Simple parsing of demo list (comma or space separated)
    std::stringstream ss(demoList);
    std::string token;
    
    while (ss >> token) {
        if (token == "phase2" || token == "p2") {
            types.push_back(DemoType::PHASE2);
        } else if (token == "phase3" || token == "p3") {
            types.push_back(DemoType::PHASE3);
        } else if (token == "phase4" || token == "p4") {
            types.push_back(DemoType::PHASE4);
        } else if (token == "phase6" || token == "p6") {
            types.push_back(DemoType::PHASE6);
        } else if (token == "benchmark" || token == "b") {
            types.push_back(DemoType::BENCHMARK);
        } else if (token == "all") {
            types.push_back(DemoType::ALL);
        }
    }
    
    return types;
}

// NeuralUtils implementations
std::vector<float> NeuralUtils::normalizeInput(const std::vector<float>& input, float min, float max) {
    std::vector<float> normalized(input.size());
    float range = max - min;
    
    if (range > 0.0f) {
        for (size_t i = 0; i < input.size(); ++i) {
            normalized[i] = (input[i] - min) / range;
        }
    }
    
    return normalized;
}

float NeuralUtils::calculateLearningRate(int step, int totalSteps, float initialRate) {
    // Calculate learning rate with decay over time
    float decayFactor = 1.0f - (static_cast<float>(step) / static_cast<float>(totalSteps));
    return initialRate * decayFactor;
}

std::vector<NeuronId> NeuralUtils::getTopActiveNeurons(const std::shared_ptr<Brain>& brain, size_t count) {
    // Get the most active neurons from the brain
    if (!brain || !brain->getSpikeSystem()) {
        return {};
    }
    
    return brain->getSpikeSystem()->getMostActiveNeurons(count);
}

float NeuralUtils::calculateWeightStats(const std::vector<float>& weights) {
    if (weights.empty()) return 0.0f;
    
    float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
    float mean = sum / weights.size();
    
    float sqSum = 0.0f;
    for (float w : weights) {
        sqSum += (w - mean) * (w - mean);
    }
    
    float variance = sqSum / weights.size();
    return std::sqrt(variance);
}

bool NeuralUtils::validateConfiguration(const std::shared_ptr<Config>& config) {
    if (!config) return false;
    
    // Check for required configuration parameters
    auto neuronCount = config->get<int64_t>("neuron_count");
    auto regionCount = config->get<int64_t>("region_count");
    auto connectionProb = config->get<float>("connection_probability");
    
    if (!neuronCount || !regionCount || !connectionProb) {
        return false;
    }
    
    // Validate ranges
    if (*neuronCount <= 0 || *regionCount <= 0) return false;
    if (*connectionProb < 0.0f || *connectionProb > 1.0f) return false;
    
    return true;
}

std::string NeuralUtils::formatDuration(double seconds) {
    // Format duration in human-readable format
    int days = static_cast<int>(seconds / 86400);
    int hours = static_cast<int>((seconds - days * 86400) / 3600);
    int minutes = static_cast<int>((seconds - days * 86400 - hours * 3600) / 60);
    int secs = static_cast<int>(seconds % 60);
    
    std::stringstream ss;
    if (days > 0) ss << days << "d ";
    if (hours > 0) ss << hours << "h ";
    if (minutes > 0) ss << minutes << "m ";
    ss << secs << "s";
    
    return ss.str();
}

} // namespace nlm