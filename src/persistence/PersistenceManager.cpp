#include "PersistenceManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <zlib.h>
#include <cmath>

namespace nlm {

PersistenceManager::PersistenceManager() : lastOperationTime_() {
}

PersistenceManager::~PersistenceManager() {
}

bool PersistenceManager::initialize(const std::string& baseDirectory) {
    baseDirectory_ = baseDirectory;
    
    // Create directory if it doesn't exist
    if (!std::filesystem::exists(baseDirectory_)) {
        try {
            std::filesystem::create_directories(baseDirectory_);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to create persistence directory: " + std::string(e.what()));
            return false;
        }
    }
    
    return true;
}

void PersistenceManager::setBaseDirectory(const std::string& directory) {
    baseDirectory_ = directory;
    if (!std::filesystem::exists(baseDirectory_)) {
        std::filesystem::create_directories(baseDirectory_);
    }
}

bool PersistenceManager::saveBrainState(const std::string& filename,
                                       bool useBinary,
                                       CheckpointSystem::CompressionLevel compression,
                                       bool incremental) {
    try {
        // Create full path
        std::string fullPath = baseDirectory_ + "/" + filename;
        
        // Extract brain state from checkpoint manager
        if (!checkpointManager_) {
            NLM_LOG_ERROR("Checkpoint manager not available for saving brain state");
            return false;
        }
        
        PersistentBrainState state = extractBrainStateFromCheckpoint(*checkpointManager_);
        
        // Save in appropriate format
        bool success = useBinary ? saveBinary(fullPath, state, compression) : saveText(fullPath, state);
        
        if (success) {
            lastOperationTime_ = std::chrono::system_clock::now();
            NLM_LOG_INFO("Brain state saved successfully to " + fullPath + " (" + 
                        (useBinary ? "binary" : "text") + " format)");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        lastError_ = e.what();
        NLM_LOG_ERROR(std::string("Exception saving brain state: ") + e.what());
        return false;
    }
}

bool PersistenceManager::loadBrainState(const std::string& filename, bool useBinary) {
    try {
        std::string fullPath = baseDirectory_ + "/" + filename;
        
        if (!isPersistentStateAvailable(fullPath)) {
            NLM_LOG_ERROR("Persistent state file not available: " + fullPath);
            return false;
        }
        
        PersistentBrainState state;
        bool success = useBinary ? loadBinary(fullPath, state) : loadText(fullPath, state);
        
        if (success) {
            // Apply to checkpoint manager if available
            if (checkpointManager_) {
                success = applyBrainStateToCheckpoint(*checkpointManager_, state);
            }
            
            if (success) {
                NLM_LOG_INFO("Brain state loaded successfully from " + fullPath);
            }
        }
        
        return success;
        
    } catch (const std::exception& e) {
        lastError_ = e.what();
        NLM_LOG_ERROR(std::string("Exception loading brain state: ") + e.what());
        return false;
    }
}

bool PersistenceManager::isPersistentStateAvailable(const std::string& filename) const {
    if (!std::filesystem::exists(filename)) {
        return false;
    }
    
    // Try to validate the file
    bool isBinary = filename.find(".bml") != std::string::npos || filename.find(".bin") != std::string::npos;
    return validatePersistentState(filename, isBinary);
}

PersistentBrainStateInfo PersistenceManager::getPersistentStateInfo(const std::string& filename,
                                                                  bool useBinary) const {
    PersistentBrainStateInfo info;
    
    try {
        if (!std::filesystem::exists(filename)) {
            return info; // Empty info for non-existent file
        }
        
        // Get file stats
        auto fileTime = std::filesystem::last_write_time(filename);
        auto fileSize = std::filesystem::file_size(filename);
        
        info.timestamp = std::chrono::system_clock::to_time_t(fileTime);
        info.fileSize = static_cast<uint64_t>(fileSize);
        
        if (useBinary) {
            // For binary files, we could parse the header
            // For now, return basic info
            info.versionMajor = 1;
            info.versionMinor = 0;
            info.versionPatch = 0;
        } else {
            // For text files, parse the metadata
            std::ifstream file(filename);
            std::string line;
            
            while (std::getline(file, line)) {
                if (line.find("Version:") != std::string::npos) {
                    // Parse version (simplified)
                    size_t majorPos = line.find("1.0.");
                    if (majorPos != std::string::npos) {
                        info.versionMajor = 1;
                        info.versionMinor = 0;
                        info.versionPatch = 0;
                    }
                } else if (line.find("NeuronCount:") != std::string::npos) {
                    // Extract neuron count
                    size_t colonPos = line.find(":");
                    if (colonPos != std::string::npos) {
                        info.neuronCount = std::stoull(line.substr(colonPos + 1));
                    }
                } else if (line.find("SynapseCount:") != std::string::npos) {
                    size_t colonPos = line.find(":");
                    if (colonPos != std::string::npos) {
                        info.synapseCount = std::stoull(line.substr(colonPos + 1));
                    }
                } else if (line.find("Step:") != std::string::npos) {
                    size_t colonPos = line.find(":");
                    if (colonPos != std::string::npos) {
                        info.simulationStep = std::stoull(line.substr(colonPos + 1));
                    }
                } else if (line.find("Time:") != std::string::npos) {
                    size_t colonPos = line.find(":");
                    if (colonPos != std::string::npos) {
                        info.simulationTime = std::stod(line.substr(colonPos + 1));
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        lastError_ = e.what();
        NLM_LOG_ERROR(std::string("Exception getting persistent state info: ") + e.what());
    }
    
    return info;
}

bool PersistenceManager::createBackup(const std::string& prefix) {
    if (!checkpointManager_) {
        return false;
    }
    
    std::string backupName = prefix + "_" + 
                            std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + 
                            ".bml";
    
    return saveBrainState(backupName, true, CheckpointSystem::CompressionLevel::Balanced, false);
}

bool PersistenceManager::restoreFromBackup(const std::string& backupName) {
    if (backupName.empty()) {
        return false;
    }
    
    return loadBrainState(backupName, true);
}

std::vector<std::string> PersistenceManager::listBrainStates() const {
    std::vector<std::string> files;
    
    try {
        if (!std::filesystem::exists(baseDirectory_)) {
            return files;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(baseDirectory_)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find("brain_state_") != std::string::npos || 
                    filename.find("auto_backup_") != std::string::npos) {
                    files.push_back(entry.path().string());
                }
            }
        }
        
        // Sort by modification time (newest first)
        std::sort(files.begin(), files.end(),
                  [this](const std::string& a, const std::string& b) {
                      auto timeA = std::filesystem::last_write_time(a);
                      auto timeB = std::filesystem::last_write_time(b);
                      return timeA > timeB;
                  });
                  
    } catch (const std::exception& e) {
        lastError_ = e.what();
        NLM_LOG_ERROR(std::string("Exception listing brain states: ") + e.what());
    }
    
    return files;
}

bool PersistenceManager::deleteBrainState(const std::string& filename) {
    try {
        std::string fullPath = baseDirectory_ + "/" + filename;
        if (std::filesystem::exists(fullPath)) {
            std::filesystem::remove(fullPath);
            NLM_LOG_INFO("Deleted brain state: " + fullPath);
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        lastError_ = e.what();
        NLM_LOG_ERROR(std::string("Exception deleting brain state: ") + e.what());
        return false;
    }
}

void PersistenceManager::setCheckpointManager(std::unique_ptr<CheckpointManager> manager) {
    checkpointManager_ = std::move(manager);
}

PersistentBrainState PersistenceManager::extractBrainStateFromCheckpoint(CheckpointManager& manager) {
    PersistentBrainState state;
    
    // For now, extract basic state - this would be enhanced with full brain state extraction
    // This is a simplified version - real implementation would extract from all brain components
    
    if (checkpointManager_) {
        auto header = checkpointManager_->getHeader();
        state.simulationStep = header.step;
        state.simulationTime = header.simulationTime;
    }
    
    // In a full implementation, we would extract:
    // - From brain components (working memory, episodic memory, etc.)
    // - From neuromodulators (dopamine, curiosity, novelty, etc.)
    // - From cognitive systems (attention, planning, concepts)
    // - From prediction system
    
    return state;
}

bool PersistenceManager::applyBrainStateToCheckpoint(CheckpointManager& manager, const PersistentBrainState& state) {
    // In a full implementation, we would apply the state to:
    // - Simulation parameters (step, time)
    // - Developmental stage and age
    // - Neuromodulation levels
    // - Memory contents
    // - Prediction system state
    // - Cognitive system state
    
    return true; // Placeholder - would be implemented with actual state application
}

bool PersistenceManager::saveBinary(const std::string& filename, const PersistentBrainState& state,
                                   CheckpointSystem::CompressionLevel compression) {
    try {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for binary save: " + filename);
            return false;
        }
        
        // Write magic header
        const uint64_t magic = 0x4E4C4D42524D4154ULL; // "NLM BRAIN"
        file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        
        // Write version
        const uint32_t version = (1 << 24) | (0 << 16) | (0 << 8);
        file.write(reinterpret_cast<const char*>(&version), sizeof(version));
        
        // Write state structure
        file.write(reinterpret_cast<const char*>(&state), sizeof(state));
        
        // Write checksum (simplified)
        uint64_t checksum = 0;
        // In real implementation, calculate checksum of the state
        
        file.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in saveBinary: ") + e.what());
        return false;
    }
}

bool PersistenceManager::loadBinary(const std::string& filename, PersistentBrainState& state) {
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for binary load: " + filename);
            return false;
        }
        
        // Read magic header
        uint64_t magic;
        file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        if (magic != 0x4E4C4D42524D4154ULL) { // "NLM BRAIN"
            NLM_LOG_ERROR("Invalid binary file format: " + filename);
            return false;
        }
        
        // Read version
        uint32_t version;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        
        // Read state structure
        file.read(reinterpret_cast<char*>(&state), sizeof(state));
        
        // Read and validate checksum (simplified)
        uint64_t checksum;
        file.read(reinterpret_cast<char*>(&checksum), sizeof(checksum));
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in loadBinary: ") + e.what());
        return false;
    }
}

bool PersistenceManager::saveText(const std::string& filename, const PersistentBrainState& state) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for text save: " + filename);
            return false;
        }
        
        // Write metadata
        file << "NLM Brain State (Text Format)\n";
        file << "Version: 1.0.0\n";
        file << "Created: " << std::chrono::system_clock::now() << "\n\n";
        
        // Write simulation state
        file << "Simulation State:\n";
        file << "  Step: " << state.simulationStep << "\n";
        file << "  Time: " << state.simulationTime << "\n";
        file << "  Timestep: " << state.timestep << "\n";
        file << "  Random Seed: " << state.randomSeed << "\n\n";
        
        // Write developmental state
        file << "Developmental State:\n";
        file << "  Stage: " << static_cast<int>(state.developmentalStage) << "\n";
        file << "  Age: " << state.developmentalAge << "\n\n";
        
        // Write neuromodulation state
        file << "Neuromodulation State:\n";
        file << "  Dopamine: " << state.dopamineLevel << "\n";
        file << "  Novelty: " << state.noveltyLevel << "\n";
        file << "  Curiosity: " << state.curiosityLevel << "\n";
        file << "  Prediction Error: " << state.predictionErrorLevel << "\n";
        file << "  Norepinephrine: " << state.norepinephrineLevel << "\n";
        file << "  Acetylcholine: " << state.acetylcholineLevel << "\n";
        file << "  Serotonin: " << state.serotoninLevel << "\n\n";
        
        // Write memory state
        file << "Memory State:\n";
        file << "  Working Memory Size: " << state.workingMemorySize << "\n";
        file << "  Episodic Memory Size: " << state.episodicMemorySize << "\n";
        file << "  Associative Memory Size: " << state.associativeMemorySize << "\n\n";
        
        // Write prediction system state
        file << "Prediction System State:\n";
        file << "  Confidence: " << state.predictionConfidence << "\n";
        file << "  Active: " << (state.predictionActive ? "true" : "false") << "\n\n";
        
        // Write cognitive system state
        file << "Cognitive System State:\n";
        file << "  Attention Inhibition: " << state.attentionInhibition << "\n";
        file << "  Attention Excitation: " << state.attentionExcitation << "\n";
        file << "  Planning Depth: " << state.planningDepth << "\n";
        file << "  Concept Count: " << state.conceptCount << "\n\n";
        
        // Write simulation parameters
        file << "Simulation Parameters:\n";
        file << "  Replay Interval: " << state.replayInterval << "\n";
        file << "  Consolidation Interval: " << state.consolidationInterval << "\n";
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in saveText: ") + e.what());
        return false;
    }
}

bool PersistenceManager::loadText(const std::string& filename, PersistentBrainState& state) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for text load: " + filename);
            return false;
        }
        
        std::string line;
        bool inSection = false;
        
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') continue;
            
            // Check for section headers
            if (line.find(":") != std::string::npos && line.find("State:") != std::string::npos) {
                inSection = true;
                continue;
            }
            
            if (!inSection) continue;
            
            // Parse key-value pairs
            size_t colonPos = line.find(":");
            if (colonPos == std::string::npos) continue;
            
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            // Parse values
            if (key == "Step") {
                state.simulationStep = std::stoull(value);
            } else if (key == "Time") {
                state.simulationTime = std::stod(value);
            } else if (key == "Timestep") {
                state.timestep = std::stod(value);
            } else if (key == "Random Seed") {
                state.randomSeed = std::stoull(value);
            } else if (key == "Stage") {
                state.developmentalStage = static_cast<DevelopmentalStage>(std::stoi(value));
            } else if (key == "Age") {
                state.developmentalAge = std::stof(value);
            } else if (key == "Dopamine") {
                state.dopamineLevel = std::stof(value);
            } else if (key == "Novelty") {
                state.noveltyLevel = std::stof(value);
            } else if (key == "Curiosity") {
                state.curiosityLevel = std::stof(value);
            } else if (key == "Prediction Error") {
                state.predictionErrorLevel = std::stof(value);
            } else if (key == "Norepinephrine") {
                state.norepinephrineLevel = std::stof(value);
            } else if (key == "Acetylcholine") {
                state.acetylcholineLevel = std::stof(value);
            } else if (key == "Serotonin") {
                state.serotoninLevel = std::stof(value);
            } else if (key == "Working Memory Size") {
                state.workingMemorySize = std::stoi(value);
            } else if (key == "Episodic Memory Size") {
                state.episodicMemorySize = std::stoi(value);
            } else if (key == "Associative Memory Size") {
                state.associativeMemorySize = std::stoi(value);
            } else if (key == "Confidence") {
                state.predictionConfidence = std::stof(value);
            } else if (key == "Active") {
                state.predictionActive = (value == "true" || value == "true");
            } else if (key == "Attention Inhibition") {
                state.attentionInhibition = std::stof(value);
            } else if (key == "Attention Excitation") {
                state.attentionExcitation = std::stof(value);
            } else if (key == "Planning Depth") {
                state.planningDepth = std::stoi(value);
            } else if (key == "Concept Count") {
                state.conceptCount = std::stoi(value);
            } else if (key == "Replay Interval") {
                state.replayInterval = std::stoull(value);
            } else if (key == "Consolidation Interval") {
                state.consolidationInterval = std::stoull(value);
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in loadText: ") + e.what());
        return false;
    }
}

bool PersistenceManager::validatePersistentState(const std::string& filename, bool useBinary) const {
    // Basic validation - check if file exists and has content
    if (!std::filesystem::exists(filename) || std::filesystem::file_size(filename) == 0) {
        return false;
    }
    
    if (useBinary) {
        // For binary files, check magic header
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;
        
        uint64_t magic;
        file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        
        return magic == 0x4E4C4D42524D4154ULL; // "NLM BRAIN"
    } else {
        // For text files, check if it starts with expected header
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        
        std::string firstLine;
        std::getline(file, firstLine);
        
        return firstLine.find("NLM Brain State") != std::string::npos;
    }
}

std::string PersistenceManager::getFileExtension(bool useBinary) const {
    return useBinary ? ".bml" : ".tml";
}

} // namespace nlm
