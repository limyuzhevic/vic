// Common utilities and helper functions for NLM demos
// This file contains shared functionality to reduce code duplication
// across all demo executables

#ifndef NLM_DEMO_UTILS_H
#define NLM_DEMO_UTILS_H

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>

namespace nlm {

// Forward declarations
class AgentBrain;
class SimpleWorld;

// === Common Configuration Helpers ===

// Create and configure a standard NLM configuration
std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    return config;
}

// Configure environment-specific settings
void configureEnvironment(std::shared_ptr<Config> config) {
    config->set("environment_width", 20, ConfigSource::Default);
    config->set("environment_height", 20, ConfigSource::Default);
    config->set("vision_width", 16, ConfigSource::Default);
    config->set("vision_height", 16, ConfigSource::Default);
}

// Configure brain-specific settings  
void configureBrain(std::shared_ptr<Config> config) {
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

// Configure performance-specific settings
void configurePerformance(std::shared_ptr<Config> config) {
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
}

// Configure Phase 6 integration test settings
void configurePhase6(std::shared_ptr<Config> config) {
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

// === Command Line Parsing Utilities ===

class DemoCommandLineParser {
public:
    struct CommonOptions {
        std::string configFile = "configs/default.cfg";
        bool verbose = false;
        bool quiet = false;
        size_t numTrials = 50;
        size_t maxSteps = 2000;
        std::string outputFile;
        bool enableProgress = true;
        bool enableProfiling = false;
        std::string logLevel = "info";
    };

    static CommonOptions parseCommon(int argc, char** argv) {
        CommonOptions options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    options.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    options.configFile = argv[++i];
                }
            } else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            } else if (arg == "--quiet" || arg == "-q") {
                options.quiet = true;
            } else if (arg == "--num-trials" && i + 1 < argc) {
                options.numTrials = std::stoull(argv[++i]);
            } else if (arg == "--max-steps" && i + 1 < argc) {
                options.maxSteps = std::stoull(argv[++i]);
            } else if (arg == "--output" && i + 1 < argc) {
                options.outputFile = argv[++i];
            } else if (arg == "--no-progress") {
                options.enableProgress = false;
            } else if (arg == "--profile") {
                options.enableProfiling = true;
            } else if (arg == "--log-level" && i + 1 < argc) {
                options.logLevel = argv[++i];
            }
        }
        
        return options;
    }
};

// === Logging and Output Utilities ===

class DemoOutput {
public:
    static void printHeader(const std::string& title) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
    
    static void printSection(const std::string& title) {
        std::cout << "\n--- " << title << " ---" << std::endl;
    }
    
    static void printResult(const std::string& name, float value, float max = 1.0f) {
        std::cout << "  " << std::left << std::setw(30) << name << ": ";
        std::cout << std::fixed << std::setprecision(3) << value;
        if (max > 0) {
            std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
        }
        std::cout << std::endl;
    }
    
    static void printProgress(size_t current, size_t total, const std::string& label = "Progress") {
        if (current % 1000 == 0 || current == total) {
            double percentage = 100.0 * current / total;
            size_t barWidth = 40;
            size_t pos = (size_t)(barWidth * percentage / 100.0);
            
            std::cout << "\r" << label << ": [" << std::string(pos, '=') << std::string(barWidth - pos, ' ') << "] "
                      << std::fixed << std::setprecision(1) << percentage << "% " << current << " / " << total;
            std::cout << std::flush;
        }
    }
    
    static void printCompletion(const std::chrono::steady_clock::time_point& start, const std::string& operation = "Operation") {
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        
        std::cout << std::endl << operation << " completed in " << duration.count() << " seconds." << std::endl;
    }
    
    static void saveResults(const std::string& filename, const std::vector<std::pair<std::string, float>>& results) {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
            return;
        }
        
        file << "NLM Demo Results" << std::endl;
        file << "=================" << std::endl;
        file << std::endl;
        
        for (const auto& result : results) {
            file << result.first << ": " << result.second << std::endl;
        }
        
        file.close();
        std::cout << "Results saved to: " << filename << std::endl;
    }
};

// === Simulation Utilities ===

class SimulationController {
public:
    struct EpisodeMetrics {
        int episode;
        int steps;
        float totalReward;
        float finalEnergy;
        float finalNovelty;
        float finalCuriosity;
        size_t totalSynapses;
        float averageFiringRate;
        int forwardCount;
        int interactCount;
        std::set<std::pair<int, int>> visitedCells;
    };
    
    // Run a simulated world interaction episode
    static EpisodeMetrics runEpisode(AgentBrain& agent, SimpleWorld& world, 
                                     int stepsPerEpisode, bool verbose = false) {
        EpisodeMetrics metrics;
        metrics.episode = 0;  // Will be set by caller
        metrics.steps = 0;
        metrics.totalReward = 0.0f;
        
        world.reset();
        agent.reset();
        
        for (int step = 0; step < stepsPerEpisode; ++step) {
            // Get sensory percept from world
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Simulate brain
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                (void)neuralStep;
                // Would need brain step here
            }
            
            // Decode motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            
            // Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0f);
            
            // Update world
            world.update(0.1);
            
            // Update development
            agent.updateDevelopment(0.1);
            
            // Track metrics
            metrics.totalReward += result.reward;
            metrics.steps++;
            
            if (cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward) {
                const auto& body = world.getAgentBody();
                metrics.visitedCells.insert({static_cast<int>(std::floor(body.x)),
                                           static_cast<int>(std::floor(body.y))});
            }
            
            if (verbose && step % 50 == 0) {
                std::cout << "  Step " << step << ": reward=" << result.reward
                          << ", action=" << static_cast<int>(cmd)
                          << ", neuromod=" << agent.getNeuromodulationLevel()
                          << ", novelty=" << agent.getNoveltyLevel() << std::endl;
            }
        }
        
        return metrics;
    }
    
    // Run multiple episodes with collection of metrics
    static std::vector<EpisodeMetrics> runEpisodes(AgentBrain& agent, SimpleWorld& world,
                                                   int numEpisodes, int stepsPerEpisode,
                                                   bool verbose = false) {
        std::vector<EpisodeMetrics> allMetrics;
        
        for (int episode = 0; episode < numEpisodes; ++episode) {
            if (verbose) {
                std::cout << "--- Episode " << episode << " started ---" << std::endl;
            }
            
            EpisodeMetrics metrics = runEpisode(agent, world, stepsPerEpisode, verbose);
            metrics.episode = episode;
            
            if (verbose) {
                std::cout << "--- Episode " << episode << " ended ---" << std::endl;
                std::cout << "  Steps: " << metrics.steps << std::endl;
                std::cout << "  Total Reward: " << metrics.totalReward << std::endl;
                std::cout << "  Visited Cells: " << metrics.visitedCells.size() << std::endl;
            }
            
            allMetrics.push_back(metrics);
        }
        
        return allMetrics;
    }
};

// === Demonstration-Specific Utilities ===

namespace DemoSpecific {
    
    // Motor command string conversion
    const char* motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            default: return "Wait";
        }
    }
    
    // Developmental stage string conversion
    const char* stageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    // Create a test percept with specified vision and touch
    SensoryPercept createPercept(const std::vector<float>& vision,
                                const std::vector<float>& touch,
                                const std::string& text = "") {
        SensoryPercept p;
        p.setVision(vision);
        p.setTouch(touch);
        p.setInternal({1.0f, 1.0f, 0.0f, 0.0f});
        p.setProprioception({0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f});
        p.setAudio({});
        p.setText(text);
        return p;
    }
    
    // Calculate cosine similarity between two vectors
    float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        size_t n = std::min(a.size(), b.size());
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        
        for (size_t i = 0; i < n; ++i) {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        if (normA < 1e-9f || normB < 1e-9f) return 0.0f;
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }
    
    // Calculate difference between two vectors
    std::vector<float> subtract(const std::vector<float>& a, const std::vector<float>& b) {
        size_t n = std::min(a.size(), b.size());
        std::vector<float> result;
        result.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            result.push_back(a[i] - b[i]);
        }
        return result;
    }
}

// === Error Handling Utilities ===

class ErrorHandler {
public:
    static bool validateConfig(const std::shared_ptr<Config>& config) {
        if (!config) {
            std::cerr << "ERROR: Configuration is null" << std::endl;
            return false;
        }
        
        // Check for required values
        if (!config->hasValue("neuron_count")) {
            std::cerr << "WARNING: neuron_count not set, using default (500)" << std::endl;
        }
        
        if (!config->hasValue("region_count")) {
            std::cerr << "WARNING: region_count not set, using default (2)" << std::endl;
        }
        
        return true;
    }
    
    static bool fileExists(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }
    
    static void safeFileWrite(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "ERROR: Cannot write to file: " << filename << std::endl;
            return;
        }
        file << content;
    }
};

// === Progress and Profiling Utilities ===

class PerformanceProfiler {
public:
    struct ProfilerData {
        std::chrono::steady_clock::time_point startTime;
        std::string operationName;
        std::chrono::duration<double> totalTime;
        size_t operations;
        double timePerOperation;
    };
    
    static ProfilerData startProfile(const std::string& name) {
        ProfilerData data;
        data.operationName = name;
        data.startTime = std::chrono::steady_clock::now();
        data.operations = 0;
        return data;
    }
    
    static void incrementOperation(ProfilerData& data) {
        data.operations++;
    }
    
    static void endProfile(ProfilerData& data) {
        auto end = std::chrono::steady_clock::now();
        data.totalTime = std::chrono::duration_cast<std::chrono::duration<double>>(end - data.startTime);
        data.timePerOperation = data.operations > 0 ? data.totalTime.count() / data.operations : 0.0;
    }
    
    static void printProfileReport(const ProfilerData& data) {
        std::cout << std::endl;
        std::cout << "Performance Profile: " << data.operationName << std::endl;
        std::cout << "  Total time: " << data.totalTime.count() << " seconds" << std::endl;
        std::cout << "  Operations: " << data.operations << std::endl;
        std::cout << "  Time per operation: " << data.timePerOperation << " seconds" << std::endl;
    }
};

} // namespace nlm

#endif // NLM_DEMO_UTILS_H
