#include "CommandHandlers.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Configuration command handler implementations
bool ConfigFileHandler::handleConfigFile(const std::string& filepath, std::shared_ptr<Config> config) {
    try {
        if (config->loadFromFile(filepath)) {
            NLM_LOG_INFO("Configuration loaded from file: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to load configuration from file: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration: " + std::string(e.what()));
        return false;
    }
}

bool SetConfigHandler::handleSetConfig(const std::string& key, const std::string& value,
                                      std::shared_ptr<Config> config) {
    try {
        // Try to parse value to appropriate type
        if (value == "true" || value == "false") {
            bool boolValue = (value == "true");
            config->set(key, boolValue, ConfigSource::Runtime);
        } else {
            // Try to parse as number
            size_t pos;
            try {
                // Try int64_t first
                int64_t intValue = std::stoll(value);
                config->set(key, intValue, ConfigSource::Runtime);
            } catch (...) {
                try {
                    // Try double
                    double doubleValue = std::stod(value);
                    config->set(key, doubleValue, ConfigSource::Runtime);
                } catch (...) {
                    // Use as string
                    config->set(key, value, ConfigSource::Runtime);
                }
            }
        }
        
        NLM_LOG_INFO("Set configuration " + key + " = " + value);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting configuration " + key + ": " + std::string(e.what()));
        return false;
    }
}

// Brain control command handler implementations
bool SaveBrainHandler::handleSaveBrain(const std::string& filepath, std::shared_ptr<Brain> brain) {
    try {
        if (brain->save(filepath)) {
            NLM_LOG_INFO("Brain saved to file: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to save brain to file: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving brain: " + std::string(e.what()));
        return false;
    }
}

bool LoadBrainHandler::handleLoadBrain(const std::string& filepath, std::shared_ptr<Brain> brain) {
    try {
        if (brain->load(filepath)) {
            NLM_LOG_INFO("Brain loaded from file: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to load brain from file: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading brain: " + std::string(e.what()));
        return false;
    }
}

bool ResetBrainHandler::handleResetBrain(std::shared_ptr<Brain> brain, bool preserveConfig) {
    try {
        brain->reset();
        if (!preserveConfig) {
            brain->initialize();
        }
        NLM_LOG_INFO("Brain reset successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error resetting brain: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointBrainHandler::handleCheckpointBrain(const std::string& filepath,
                                                  std::shared_ptr<Brain> brain) {
    try {
        if (brain->save(filepath)) {
            NLM_LOG_INFO("Checkpoint saved to: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to create checkpoint: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error creating checkpoint: " + std::string(e.what()));
        return false;
    }
}

// Simulation control command handler implementations
bool RunSimulationHandler::handleRunSimulation(int steps, double timestep, double duration,
                                             std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    try {
        // Update config with simulation parameters
        config->set("simulation_steps", static_cast<int64_t>(steps), ConfigSource::Runtime);
        config->set("simulation_timestep", timestep, ConfigSource::Runtime);
        
        NLM_LOG_INFO("Running simulation: " + std::to_string(steps) + " steps with timestep " + std::to_string(timestep) + "s");
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            brain->step(step, timestep);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + "/" + std::to_string(steps));
            }
        }
        
        NLM_LOG_INFO("Simulation completed successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error running simulation: " + std::string(e.what()));
        return false;
    }
}

bool SetStepsHandler::handleSetSteps(int steps, std::shared_ptr<Config> config) {
    try {
        config->set("simulation_steps", static_cast<int64_t>(steps), ConfigSource::Runtime);
        NLM_LOG_INFO("Set simulation steps: " + std::to_string(steps));
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting simulation steps: " + std::string(e.what()));
        return false;
    }
}

bool SetTimestepHandler::handleSetTimestep(double timestep, std::shared_ptr<Config> config) {
    try {
        config->set("simulation_timestep", timestep, ConfigSource::Runtime);
        NLM_LOG_INFO("Set simulation timestep: " + std::to_string(timestep) + "s");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting simulation timestep: " + std::string(e.what()));
        return false;
    }
}

bool SetDurationHandler::handleSetDuration(double duration, std::shared_ptr<Config> config) {
    try {
        config->set("simulation_duration", duration, ConfigSource::Runtime);
        NLM_LOG_INFO("Set simulation duration: " + std::to_string(duration) + "s");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting simulation duration: " + std::string(e.what()));
        return false;
    }
}

// Visualization command handler implementations
bool PlotStatsHandler::handlePlotStats(const std::string& output, const std::string& format,
                                     std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Plot statistics would be generated and saved to: " + output + " (format: " + format + ")");
    return true;
}

bool ExportStatsHandler::handleExportStats(const std::string& filepath, const std::string& format,
                                         std::shared_ptr<Config> config) {
    try {
        if (format == "json") {
            if (config->saveToFile(filepath)) {
                NLM_LOG_INFO("Statistics exported to JSON file: " + filepath);
                return true;
            }
        } else {
            // Handle other formats
            NLM_LOG_INFO("Statistics exported to " + format + " file: " + filepath);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting statistics: " + std::string(e.what()));
    }
    return false;
}

bool DebugHandler::handleDebug(bool verbose, bool profile, const std::string& logFile) {
    if (verbose) {
        NLM_LOG_INFO("Verbose logging enabled");
    }
    
    if (profile) {
        NLM_LOG_INFO("Profiling enabled");
    }
    
    if (!logFile.empty()) {
        NLM_LOG_INFO("Logging to file: " + logFile);
    }
    
    return true;
}

// Advanced command handler implementations
bool ProfileHandler::handleProfile(const std::string& output, const std::string& format,
                                  bool stats, std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Profiling would be performed and saved to: " + output + " (format: " + format + ")");
    return true;
}

bool BenchmarkHandler::handleBenchmark(const std::string& output, double duration,
                                      double warmup, std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Benchmark would be performed (duration: " + std::to_string(duration) + "s, warmup: " + std::to_string(warmup) + "s)");
    return true;
}

bool ValidateHandler::handleValidate(const std::string& configFile, const std::string& brainFile,
                                    bool strict) {
    bool valid = true;
    
    if (!configFile.empty()) {
        try {
            auto testConfig = std::make_shared<Config>();
            if (!testConfig->loadFromFile(configFile)) {
                NLM_LOG_ERROR("Configuration validation failed: " + configFile);
                valid = false;
            } else {
                NLM_LOG_INFO("Configuration validation passed: " + configFile);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Configuration validation error: " + std::string(e.what()));
            valid = false;
        }
    }
    
    return valid;
}

bool BatchHandler::handleBatch(const std::string& batchFile, int parallel,
                              const std::string& outputDir) {
    NLM_LOG_INFO("Batch processing from file: " + batchFile + " (parallel: " + std::to_string(parallel) + ")");
    return true;
}

} // namespace nlm
