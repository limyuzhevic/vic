// Command-line interface utilities for NLM
// Provides enhanced command-line parsing, configuration management,
// brain checkpoint utilities, and experiment session management.

#include "CommandLineUtils.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct CommandLineUtils::Impl {
    std::string checkpointDir;
    std::string sessionDir;
    std::unordered_map<std::string, std::string> lastSessionState;
    
    Impl() : checkpointDir("."), sessionDir("./sessions") {
        // Ensure directories exist
        std::filesystem::create_directories(checkpointDir);
        std::filesystem::create_directories(sessionDir);
    }
};

CommandLineUtils::CommandLineUtils() : pImpl(std::make_unique<Impl>()) {}

CommandLineUtils::~CommandLineUtils() = default;

CommandLineUtils::CommandLineUtils(CommandLineUtils&&) noexcept = default;

CommandLineUtils& CommandLineUtils::operator=(CommandLineUtils&&) noexcept = default;

std::unordered_map<std::string, std::string> CommandLineUtils::parseArguments(int argc, char** argv) {
    std::unordered_map<std::string, std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            // --key=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args[key] = value;
            } else {
                // --key format (flag)
                args[arg.substr(2)] = "true";
            }
        } else if (arg[0] == '-') {
            // -key value format
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args[key] = argv[++i];
            } else {
                args[key] = "true";
            }
        } else {
            // Positional argument
            args[std::to_string(i - 1)] = arg;
        }
    }
    
    return args;
}

std::shared_ptr<ConfigManager> CommandLineUtils::loadConfig(const std::string& filepath, bool validate) {
    auto config = std::make_shared<ConfigManager>();
    
    // Try to detect file format
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = config->loadFromJSONFile(filepath, validate);
    } else {
        success = config->loadFromFile(filepath);
    }
    
    return success ? config : nullptr;
}

bool CommandLineUtils::saveConfig(const std::shared_ptr<ConfigManager>& config, 
                                 const std::string& filepath, bool pretty) {
    if (!config) return false;
    
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string jsonData = config->exportToJSON(pretty);
    file << jsonData;
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

bool CommandLineUtils::createCheckpoint(const std::string& filepath, int compression,
                                         const std::shared_ptr<ConfigManager>& config) {
    try {
        // Create checkpoint using CheckpointSystem
        CheckpointWriter writer;
        
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Write configuration if provided
        if (config) {
            std::string jsonData = config->exportToJSON(true);
            writer.writeSection(CheckpointSection::Config, 
                               jsonData.c_str(), jsonData.size());
        }
        
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            writer.abort();
            return false;
        }
        
        NLM_LOG_INFO("Checkpoint created: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error creating checkpoint: " + std::string(e.what()));
        return false;
    }
}

bool CommandLineUtils::loadCheckpoint(const std::string& filepath) {
    try {
        CheckpointReader reader;
        
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint: " + filepath);
            return false;
        }
        
        // Validate checkpoint
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Checkpoint loaded: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading checkpoint: " + std::string(e.what()));
        return false;
    }
}

bool CommandLineUtils::saveSession(const std::string& sessionName,
                                   const std::shared_ptr<ConfigManager>& config,
                                   uint64_t step, double simTime) {
    if (!config) return false;
    
    try {
        // Create session directory
        std::string sessionDir = pImpl->sessionDir + "/" + sessionName;
        std::filesystem::create_directories(sessionDir);
        
        // Save configuration
        std::string configFile = sessionDir + "/config.json";
        if (!saveConfig(config, configFile, true)) {
            return false;
        }
        
        // Save metadata
        std::string metadataFile = sessionDir + "/metadata.json";
        json metadata;
        metadata["session_name"] = sessionName;
        metadata["step"] = step;
        metadata["simulation_time"] = simTime;
        metadata["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        std::ofstream metaFile(metadataFile);
        if (metaFile.is_open()) {
            metaFile << metadata.dump(2);
        }
        
        // Store in last session state
        pImpl->lastSessionState[sessionName] = metadataFile;
        
        NLM_LOG_INFO("Session saved: " + sessionName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving session: " + std::string(e.what()));
        return false;
    }
}

bool CommandLineUtils::loadSession(const std::string& sessionName) {
    try {
        std::string sessionDir = pImpl->sessionDir + "/" + sessionName;
        
        // Check if session directory exists
        if (!std::filesystem::exists(sessionDir)) {
            NLM_LOG_ERROR("Session not found: " + sessionName);
            return false;
        }
        
        // Load configuration
        std::string configFile = sessionDir + "/config.json";
        auto config = loadConfig(configFile, true);
        
        if (!config) {
            NLM_LOG_ERROR("Failed to load configuration for session: " + sessionName);
            return false;
        }
        
        // Load metadata
        std::string metadataFile = sessionDir + "/metadata.json";
        json metadata;
        std::ifstream metaInFile(metadataFile);
        if (metaInFile.is_open()) {
            metaInFile >> metadata;
            pImpl->lastSessionState[sessionName] = metadataFile;
        }
        
        NLM_LOG_INFO("Session loaded: " + sessionName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading session: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> CommandLineUtils::listCheckpoints(const std::string& pattern) {
    std::vector<std::string> checkpoints;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(pImpl->checkpointDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // Simple wildcard matching
                bool matches = true;
                size_t patternPos = 0;
                size_t filePos = 0;
                
                while (patternPos < pattern.size() && filePos < filename.size()) {
                    if (pattern[patternPos] == '*') {
                        while (patternPos + 1 < pattern.size() && pattern[patternPos + 1] == '*') {
                            patternPos++;
                        }
                        if (patternPos + 1 >= pattern.size()) {
                            break; // '*' matches anything
                        }
                        while (filePos < filename.size()) {
                            if (matchesPattern(pattern.substr(patternPos + 1), filename.substr(filePos))) {
                                matches = true;
                                break;
                            }
                            filePos++;
                        }
                        if (!matches) matches = false;
                        break;
                    } else if (pattern[patternPos] == '?') {
                        if (filePos < filename.size()) {
                            filePos++;
                        } else {
                            matches = false;
                            break;
                        }
                    } else if (pattern[patternPos] != filename[filePos]) {
                        matches = false;
                        break;
                    }
                    
                    patternPos++;
                    filePos++;
                }
                
                if (matches && patternPos == pattern.size() && filePos == filename.size()) {
                    checkpoints.push_back(filename);
                }
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error listing checkpoints: " + std::string(e.what()));
    }
    
    return checkpoints;
}

void CommandLineUtils::configureLogging(const std::string& level,
                                        const std::string& file,
                                        const std::string& format,
                                        size_t maxSize) {
    try {
        // Set log level
        LogLevel logLevel;
        if (level == "Debug") logLevel = LogLevel::Debug;
        else if (level == "Info") logLevel = LogLevel::Info;
        else if (level == "Warning") logLevel = LogLevel::Warning;
        else if (level == "Error") logLevel = LogLevel::Error;
        else if (level == "Critical") logLevel = LogLevel::Critical;
        else logLevel = LogLevel::Info;
        
        Logger::getInstance().setLevel(logLevel);
        
        // TODO: Add file logging if needed
        // For now, just configure console logging
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
        Logger::getInstance().addLogger(consoleLogger);
        
        NLM_LOG_INFO("Logging configured: level=" + level + ", file=" + (file.empty() ? "console" : file));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error configuring logging: " + std::string(e.what()));
    }
}

std::string CommandLineUtils::getConfigSummary(const std::shared_ptr<ConfigManager>& config) {
    if (!config) return "No configuration provided.";
    
    std::ostringstream oss;
    oss << "Configuration Summary:";
    oss << "\n  Total keys: " << config->getKeys().size();
    
    auto stats = config->getStats();
    oss << "\n  Load count: " << stats["load_count"];
    oss << "\n  Save count: " << stats["save_count"];
    oss << "\n  Validation count: " << stats["validation_count"];
    
    oss << "\n\nConfiguration Entries:";
    for (const auto& key : config->getKeys()) {
        auto valueOpt = config->get<ConfigValue>(key);
        if (!valueOpt) continue;
        
        const ConfigValue& value = valueOpt.value();
        oss << "\n  " << key << " = ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, value);
    }
    
    return oss.str();
}

std::string CommandLineUtils::runSimulation(const std::shared_ptr<ConfigManager>& config, 
                                            uint64_t steps) {
    if (!config) return "No configuration provided.";
    
    std::ostringstream oss;
    oss << "Running simulation with " << steps << " steps...";
    oss << "\nConfiguration: " << getConfigSummary(config);
    
    // TODO: Integrate with actual simulation code
    oss << "\n\nSimulation completed (placeholder implementation).";
    
    return oss.str();
}

bool CommandLineUtils::exportBrainState(const std::vector<uint8_t>& brainState, 
                                       const std::string& filepath) {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) return false;
        
        // Write header
        uint64_t size = brainState.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        
        // Write data
        file.write(reinterpret_cast<const char*>(brainState.data()), size);
        
        NLM_LOG_INFO("Brain state exported: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting brain state: " + std::string(e.what()));
        return false;
    }
}

std::vector<uint8_t> CommandLineUtils::importBrainState(const std::string& filepath) {
    std::vector<uint8_t> brainState;
    
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open brain state file: " + filepath);
            return brainState;
        }
        
        // Read header
        uint64_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        
        // Read data
        brainState.resize(size);
        file.read(reinterpret_cast<char*>(brainState.data()), size);
        
        NLM_LOG_INFO("Brain state imported: " + filepath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error importing brain state: " + std::string(e.what()));
    }
    
    return brainState;
}

std::string CommandLineUtils::getSessionInfo(const std::string& sessionName) {
    std::ostringstream oss;
    
    try {
        std::string sessionDir = pImpl->sessionDir + "/" + sessionName;
        
        if (!std::filesystem::exists(sessionDir)) {
            return "Session not found: " + sessionName;
        }
        
        // Try to load metadata
        std::string metadataFile = sessionDir + "/metadata.json";
        if (std::filesystem::exists(metadataFile)) {
            json metadata;
            std::ifstream metaFile(metadataFile);
            if (metaFile.is_open()) {
                metaFile >> metadata;
                oss << "Session: " << metadata.value("session_name", sessionName);
                oss << "\nStep: " << metadata.value("step", 0);
                oss << "\nSimulation time: " << metadata.value("simulation_time", 0.0);
                oss << "\nTimestamp: " << metadata.value("timestamp", 0);
            }
        }
        
    } catch (const std::exception& e) {
        return "Error getting session info: " + std::string(e.what());
    }
    
    return oss.str();
}

bool CommandLineUtils::deleteSession(const std::string& sessionName) {
    try {
        std::string sessionDir = pImpl->sessionDir + "/" + sessionName;
        
        if (!std::filesystem::exists(sessionDir)) {
            NLM_LOG_ERROR("Session not found for deletion: " + sessionName);
            return false;
        }
        
        // Remove session directory
        std::filesystem::remove_all(sessionDir);
        
        // Remove from last session state
        pImpl->lastSessionState.erase(sessionName);
        
        NLM_LOG_INFO("Session deleted: " + sessionName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error deleting session: " + std::string(e.what()));
        return false;
    }
}

std::string CommandLineUtils::validateConfig(const std::shared_ptr<ConfigManager>& config) {
    if (!config) return "No configuration provided.";
    
    auto result = config->validate();
    
    std::ostringstream oss;
    oss << "Validation Result:";
    oss << "\n  Valid: " << (result.valid ? "YES" : "NO");
    
    if (!result.errors.empty()) {
        oss << "\n  Errors:";
        for (const auto& error : result.errors) {
            oss << "\n    - " << error;
        }
    }
    
    if (!result.warnings.empty()) {
        oss << "\n  Warnings:";
        for (const auto& warning : result.warnings) {
            oss << "\n    - " << warning;
        }
    }
    
    return oss.str();
}

std::string CommandLineUtils::getHelp(const std::string& programName) {
    std::ostringstream oss;
    oss << "=== NLM Command Line Utilities ===\n";
    oss << "\nUsage:\n";
    oss << "  " << programName << " [options]\n";
    oss << "\nOptions:\n";
    oss << "  --help                     Show this help message\n";
    oss << "  --version                  Show version information\n";
    oss << "\nConfiguration:\n";
    oss << "  --config FILE              Load configuration from file\n";
    oss << "  --save-config FILE         Save current configuration to file\n";
    oss << "  --validate                 Validate configuration\n";
    oss << "\nCheckpointing:\n";
    oss << "  --checkpoint FILE          Create brain checkpoint\n";
    oss << "  --load-checkpoint FILE     Load brain checkpoint\n";
    oss << "  --list-checkpoints         List available checkpoints\n";
    oss << "\nSession Management:\n";
    oss << "  --save-session NAME        Save experiment session\n";
    oss << "  --load-session NAME        Load experiment session\n";
    oss << "  --session-info NAME        Get session information\n";
    oss << "  --delete-session NAME      Delete experiment session\n";
    oss << "\nLogging:\n";
    oss << "  --log-level LEVEL          Set logging level (Debug, Info, Warning, Error, Critical)\n";
    oss << "  --log-file FILE            Set log output file\n";
    oss << "\nSimulation:\n";
    oss << "  --run STEPS                Run simulation with specified steps\n";
    oss << "\nBrain State:\n";
    oss << "  --export-brain STATE_FILE  Export brain state to file\n";
    oss << "  --import-brain STATE_FILE  Import brain state from file\n";
    oss << "\nExamples:\n";
    oss << "  " << programName << " --config myconfig.json --run 1000\n";
    oss << "  " << programName << " --save-session session1 --log-level Debug\n";
    oss << "  " << programName << " --checkpoint brain.bin --list-checkpoints\n";
    
    return oss.str();
}

std::string CommandLineUtils::processCommandLine(int argc, char** argv,
                                                  const std::shared_ptr<ConfigManager>& defaultConfig) {
    std::ostringstream oss;
    
    // Check for help
    std::unordered_map<std::string, std::string> args = parseArguments(argc, argv);
    
    if (args.find("help") != args.end() || args.find("?") != args.end()) {
        return getHelp("nlm");
    }
    
    // Check for version
    if (args.find("version") != args.end()) {
        oss << "NLM (熙然) Neural Learning Machine v0.1.0\n";
        oss << "Phase 6: Final Integration\n";
        return oss.str();
    }
    
    // Load configuration if specified
    std::shared_ptr<ConfigManager> config = defaultConfig;
    if (args.find("config") != args.end()) {
        config = loadConfig(args["config"], true);
        if (!config) {
            oss << "Error: Failed to load configuration from: " << args["config"] << std::endl;
            return oss.str();
        }
    }
    
    // Handle other commands
    if (args.find("save-config") != args.end()) {
        if (config && saveConfig(config, args["save-config"], true)) {
            oss << "Configuration saved to: " << args["save-config"] << std::endl;
        }
    } else if (args.find("validate") != args.end()) {
        if (config) {
            oss << validateConfig(config) << std::endl;
        }
    } else if (args.find("checkpoint") != args.end()) {
        createCheckpoint(args["checkpoint"], 6, config);
    } else if (args.find("load-checkpoint") != args.end()) {
        loadCheckpoint(args["load-checkpoint"]);
    } else if (args.find("list-checkpoints") != args.end()) {
        auto checkpoints = listCheckpoints();
        oss << "Available checkpoints:" << std::endl;
        for (const auto& checkpoint : checkpoints) {
            oss << "  " << checkpoint << std::endl;
        }
    } else if (args.find("save-session") != args.end()) {
        saveSession(args["save-session"], config);
    } else if (args.find("load-session") != args.end()) {
        loadSession(args["load-session"]);
    } else if (args.find("session-info") != args.end()) {
        oss << getSessionInfo(args["session-info"]) << std::endl;
    } else if (args.find("delete-session") != args.end()) {
        deleteSession(args["delete-session"]);
    } else if (args.find("log-level") != args.end()) {
        configureLogging(args["log-level"],
                         args.find("log-file") != args.end() ? args["log-file"] : "",
                         "default");
    } else if (args.find("run") != args.end()) {
        uint64_t steps = 0;
        try {
            steps = std::stoull(args["run"]);
        } catch (...) {
            steps = 1000; // Default
        }
        oss << runSimulation(config, steps) << std::endl;
    } else if (args.find("export-brain") != args.end()) {
        std::vector<uint8_t> brainState; // TODO: Get actual brain state
        exportBrainState(brainState, args["export-brain"]);
    } else if (args.find("import-brain") != args.end()) {
        importBrainState(args["import-brain"]);
    } else {
        // Default action: show config summary
        if (config) {
            oss << getConfigSummary(config) << std::endl;
            oss << "\nUse --help for more options.\n";
        } else {
            oss << "NLM Command Line Utilities\n";
            oss << "Use --help for more options.\n";
        }
    }
    
    return oss.str();
}

bool CommandLineUtils::matchesPattern(const std::string& pattern, const std::string& str) {
    // Simple wildcard matching implementation
    size_t patternPos = 0;
    size_t strPos = 0;
    
    while (patternPos < pattern.size() && strPos < str.size()) {
        if (pattern[patternPos] == '*') {
            while (patternPos + 1 < pattern.size() && pattern[patternPos + 1] == '*') {
                patternPos++;
            }
            if (patternPos + 1 >= pattern.size()) {
                return true; // '*' matches anything
            }
            while (strPos < str.size()) {
                if (matchesPattern(pattern.substr(patternPos + 1), str.substr(strPos))) {
                    return true;
                }
                strPos++;
            }
            return false;
        } else if (pattern[patternPos] == '?') {
            if (strPos < str.size()) {
                strPos++;
            } else {
                return false;
            }
        } else if (pattern[patternPos] != str[strPos]) {
            return false;
        }
        
        patternPos++;
        strPos++;
    }
    
    return patternPos == pattern.size() && strPos == str.size();
}

} // namespace nlm
