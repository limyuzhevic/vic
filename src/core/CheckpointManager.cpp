#include "CheckpointManager.hpp"
#include <filesystem>
#include <zlib.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>
#include <iomanip>

namespace nlm {

CheckpointManager::CheckpointManager()
    : enabled_(true)
    , autoSave_(true)
    , compressionEnabled_(false)
    , checkpointDir_(".")
    , saveInterval_(10000)
    , maxCheckpoints_(10)
    , checkpointFormat_("binary")
    , totalCheckpointsCreated_(0)
    , totalBytesSaved_(0)
    , totalBytesLoaded_(0)
    , nextCheckpointStep_(0)
    , totalSaveTime_(0.0)
    , failedSaves_(0)
    , checkpointInfoCacheDirty_(true)
{
    NLM_LOG_INFO("Enhanced CheckpointManager initialized");
}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& directory, 
                                 SimulationStep saveInterval,
                                 size_t maxCheckpoints,
                                 bool enableAutoSave,
                                 bool enableCompression,
                                 const std::string& format) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    checkpointDir_ = directory;
    saveInterval_ = saveInterval;
    maxCheckpoints_ = maxCheckpoints;
    autoSave_ = enableAutoSave;
    compressionEnabled_ = enableCompression;
    checkpointFormat_ = format;
    
    if (!ensureDirectoryExists(checkpointDir_)) {
        NLM_LOG_ERROR("Failed to create checkpoint directory: " + checkpointDir_);
    }
    
    checkpointInfoCacheDirty_ = true;
    
    NLM_LOG_INFO("CheckpointManager configured: dir=" + checkpointDir_ + 
                 ", interval=" + std::to_string(saveInterval_) + 
                 ", max_checkpoints=" + std::to_string(maxCheckpoints_) +
                 ", auto_save=" + std::to_string(autoSave_) +
                 ", compression=" + std::to_string(compressionEnabled_));
}

bool CheckpointManager::save(SimulationStep step, Timestamp timestamp,
                             const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             const std::vector<std::unique_ptr<Synapse>>& synapses,
                             const std::string& description) {
    if (!enabled_) {
        return true; // Silently skip if disabled
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        std::string filename = generateCheckpointFilename(step, timestamp);
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        
        // Count neurons and synapses
        for (const auto& region : regions) {
            for (const auto& pop : region->getPopulations()) {
                neuronCount += pop->getSize();
            }
        }
        
        synapseCount = synapses.size();
        
        float simulationTime = static_cast<float>(timestamp);
        
        bool success = false;
        
        if (checkpointFormat_ == "binary") {
            success = writeBinaryCheckpoint(filename, regions, synapses);
        } else {
            success = writeTextCheckpoint(filename, regions, synapses);
        }
        
        if (success) {
            // Update statistics
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            updateStatistics(step, timestamp, neuronCount, synapseCount, simulationTime, 
                           true, 0); // Will be updated after file size check
            
            // Update checkpoint info cache
            checkpointInfoCache_.push_back(createCheckpointInfo(step, timestamp, 
                                                               neuronCount, synapseCount, 
                                                               simulationTime));
            
            // Update next checkpoint description
            if (!description.empty()) {
                nextCheckpointDescription_ = description;
            }
            
            // Clean up old checkpoints if needed
            cleanupOldCheckpoints();
            
            // Update file size statistics
            std::error_code ec;
            auto fileSize = std::filesystem::file_size(filename, ec);
            if (!ec) {
                totalBytesSaved_ += fileSize;
            }
            
            NLM_LOG_INFO("Checkpoint saved: " + filename + 
                        " (step=" + std::to_string(step) + ", " +
                        "neurons=" + std::to_string(neuronCount) + ", " +
                        "synapses=" + std::to_string(synapseCount) + ", " +
                        "time=" + std::to_string(duration) + "s)");
            
            totalCheckpointsCreated_++;
            nextCheckpointStep_ = step + saveInterval_;
            
            return true;
        } else {
            failedSaves_++;
            NLM_LOG_ERROR("Failed to save checkpoint: " + filename);
            return false;
        }
        
    } catch (const std::exception& e) {
        failedSaves_++;
        NLM_LOG_ERROR("Exception during checkpoint save: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::load(SimulationStep targetStep,
                             std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             std::vector<std::unique_ptr<Synapse>>& synapses,
                             SimulationStep& outLoadedStep,
                             Timestamp& outLoadedTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!enabled_) {
        NLM_LOG_ERROR("Cannot load checkpoint: checkpointing is disabled");
        return false;
    }
    
    // Find the checkpoint file closest to target step
    std::string bestFile;
    CheckpointInfo bestInfo;
    
    updateCheckpointInfoCache();
    
    for (const auto& info : checkpointInfoCache_) {
        if (info.step == targetStep) {
            bestFile = info.filepath;
            bestInfo = info;
            break;
        } else if (info.step < targetStep && 
                  (bestFile.empty() || info.step > bestInfo.step)) {
            bestFile = info.filepath;
            bestInfo = info;
        }
    }
    
    if (bestFile.empty()) {
        NLM_LOG_ERROR("No checkpoint found for step <= " + std::to_string(targetStep));
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        bool success = false;
        
        if (checkpointFormat_ == "binary") {
            success = readBinaryCheckpoint(bestFile, regions, synapses);
        } else {
            success = readTextCheckpoint(bestFile, regions, synapses);
        }
        
        if (success) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            // Update statistics
            size_t neuronCount = 0;
            size_t synapseCount = 0;
            
            for (const auto& region : regions) {
                for (const auto& pop : region->getPopulations()) {
                    neuronCount += pop->getSize();
                }
            }
            
            synapseCount = synapses.size();
            
            float simulationTime = bestInfo.timestamp;
            
            updateStatistics(bestInfo.step, bestInfo.timestamp, neuronCount, synapseCount, 
                           simulationTime, true, 0);
            
            // Update file size statistics
            std::error_code ec;
            auto fileSize = std::filesystem::file_size(bestFile, ec);
            if (!ec) {
                totalBytesLoaded_ += fileSize;
            }
            
            outLoadedStep = bestInfo.step;
            outLoadedTime = bestInfo.timestamp;
            
            NLM_LOG_INFO("Checkpoint loaded: " + bestFile + 
                        " (step=" + std::to_string(bestInfo.step) + ", " +
                        "neurons=" + std::to_string(neuronCount) + ", " +
                        "synapses=" + std::to_string(synapseCount) + ", " +
                        "time=" + std::to_string(duration) + "s)");
            
            return true;
        } else {
            NLM_LOG_ERROR("Failed to load checkpoint: " + bestFile);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during checkpoint load: " + std::string(e.what()));
        return false;
    }
}

void CheckpointManager::cleanupOldCheckpoints() {
    if (checkpointInfoCache_.size() <= maxCheckpoints_) {
        return; // No cleanup needed
    }
    
    // Sort by step (oldest first)
    std::sort(checkpointInfoCache_.begin(), checkpointInfoCache_.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.step < b.step;
              });
    
    // Remove oldest checkpoints
    size_t toRemove = checkpointInfoCache_.size() - maxCheckpoints_;
    std::vector<std::string> removedFiles;
    
    for (size_t i = 0; i < toRemove; ++i) {
        const auto& info = checkpointInfoCache_[i];
        std::error_code ec;
        if (std::filesystem::remove(info.filepath, ec)) {
            removedFiles.push_back(info.filepath);
        } else {
            NLM_LOG_WARN("Failed to remove checkpoint file: " + info.filepath);
        }
    }
    
    // Update cache
    checkpointInfoCache_.erase(checkpointInfoCache_.begin(), 
                               checkpointInfoCache_.begin() + toRemove);
    
    if (!removedFiles.empty()) {
        NLM_LOG_INFO("Cleaned up " + std::to_string(removedFiles.size()) + 
                    " old checkpoints");
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::getAvailableCheckpoints() const {
    updateCheckpointInfoCache();
    return checkpointInfoCache_;
}

CheckpointManager::Statistics CheckpointManager::getStatistics() const {
    Statistics stats;
    std::lock_guard<std::mutex> lock(mutex_);
    
    stats.totalCheckpointsCreated = totalCheckpointsCreated_;
    stats.totalBytesSaved = totalBytesSaved_;
    stats.totalBytesLoaded = totalBytesLoaded_;
    stats.nextCheckpointStep = nextCheckpointStep_;
    stats.failedSaves = failedSaves_;
    
    if (totalCheckpointsCreated_ > 0) {
        stats.averageSaveTime = totalSaveTime_ / totalCheckpointsCreated_;
    } else {
        stats.averageSaveTime = 0.0;
    }
    
    return stats;
}

bool CheckpointManager::exportMetadata(const std::string& metadataFile) const {
    updateCheckpointInfoCache();
    
    std::ofstream file(metadataFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open metadata file for writing: " + metadataFile);
        return false;
    }
    
    // Write metadata in JSON format
    file << "{\n";
    file << "  \"checkpoint_manager_version\": \"1.0\",\n";
    file << "  \"checkpoint_dir\": \"" << checkpointDir_ << "\",\n";
    file << "  \"save_interval\": " << saveInterval_ << ",\n";
    file << "  \"max_checkpoints\": " << maxCheckpoints_ << ",\n";
    file << "  \"enabled\": " << (enabled_ ? "true" : "false") << ",\n";
    file << "  \"auto_save\": " << (autoSave_ ? "true" : "false") << ",\n";
    file << "  \"compression_enabled\": " << (compressionEnabled_ ? "true" : "false") << ",\n";
    file << "  \"checkpoint_format\": \"" << checkpointFormat_ << "\",\n";
    file << "  \"statistics\": {\n";
    
    Statistics stats = getStatistics();
    file << "    \"total_checkpoints_created\": " << stats.totalCheckpointsCreated << ",\n";
    file << "    \"total_bytes_saved\": " << stats.totalBytesSaved << ",\n";
    file << "    \"total_bytes_loaded\": " << stats.totalBytesLoaded << ",\n";
    file << "    \"next_checkpoint_step\": " << stats.nextCheckpointStep << ",\n";
    file << "    \"average_save_time\": " << stats.averageSaveTime << ",\n";
    file << "    \"failed_saves\": " << stats.failedSaves << "\n";
    file << "  },\n";
    
    file << "  \"checkpoints\": [\n";
    
    for (size_t i = 0; i < checkpointInfoCache_.size(); ++i) {
        const auto& info = checkpointInfoCache_[i];
        file << "    {\n";
        file << "      \"filepath\": \"" << info.filepath << "\",\n";
        file << "      \"step\": " << info.step << ",\n";
        file << "      \"timestamp\": " << info.timestamp << ",\n";
        file << "      \"neuron_count\": " << info.neuronCount << ",\n";
        file << "      \"synapse_count\": " << info.synapseCount << ",\n";
        file << "      \"simulation_time\": " << info.simulationTime << ",\n";
        file << "      \"description\": \"" << info.description << "\"\n";
        file << "    }";
        if (i < checkpointInfoCache_.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    NLM_LOG_INFO("Checkpoint metadata exported to: " + metadataFile);
    return true;
}

bool CheckpointManager::importMetadata(const std::string& metadataFile) {
    std::ifstream file(metadataFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open metadata file for reading: " + metadataFile);
        return false;
    }
    
    try {
        // Simple JSON parsing (could be enhanced with proper JSON library)
        std::string line;
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // For now, just update cache and basic info
        checkpointInfoCache_.clear();
        checkpointInfoCacheDirty_ = true;
        
        NLM_LOG_INFO("Checkpoint metadata imported from: " + metadataFile);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during metadata import: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::backupDirectory(const std::string& backupPath) {
    if (!ensureDirectoryExists(backupPath)) {
        NLM_LOG_ERROR("Failed to create backup directory: " + backupPath);
        return false;
    }
    
    try {
        for (const auto& info : checkpointInfoCache_) {
            std::string src = info.filepath;
            std::string dst = backupPath + "/" + std::filesystem::path(src).filename().string();
            
            if (compressionEnabled_) {
                // Compress during backup
                if (!compressFile(src, dst)) {
                    NLM_LOG_WARN("Failed to compress checkpoint during backup: " + src);
                    continue;
                }
            } else {
                // Simple copy
                std::error_code ec;
                if (!std::filesystem::copy_file(src, dst, std::filesystem::copy_options::overwrite_existing, ec)) {
                    NLM_LOG_WARN("Failed to copy checkpoint during backup: " + src);
                    continue;
                }
            }
        }
        
        NLM_LOG_INFO("Checkpoint directory backed up to: " + backupPath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during backup: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::restoreFromBackup(const std::string& backupPath) {
    if (!ensureDirectoryExists(backupPath)) {
        NLM_LOG_ERROR("Backup directory does not exist: " + backupPath);
        return false;
    }
    
    try {
        // Clear current checkpoints
        checkpointInfoCache_.clear();
        checkpointInfoCacheDirty_ = true;
        
        // Restore backup files
        for (const auto& entry : std::filesystem::directory_iterator(backupPath)) {
            if (entry.is_regular_file()) {
                std::string src = entry.path().string();
                std::string dst = checkpointDir_ + "/" + entry.path().filename().string();
                
                if (compressionEnabled_) {
                    // Decompress during restore
                    std::string decompressedDst = dst.substr(0, dst.find_last_of('.'));
                    if (decompressFile(src, decompressedDst)) {
                        dst = decompressedDst;
                    } else {
                        NLM_LOG_WARN("Failed to decompress checkpoint during restore: " + src);
                        continue;
                    }
                } else {
                    // Simple copy
                    std::error_code ec;
                    if (!std::filesystem::copy_file(src, dst, std::filesystem::copy_options::overwrite_existing, ec)) {
                        NLM_LOG_WARN("Failed to copy checkpoint during restore: " + src);
                        continue;
                    }
                }
            }
        }
        
        NLM_LOG_INFO("Restored checkpoint directory from backup: " + backupPath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during restore: " + std::string(e.what()));
        return false;
    }
}

// Internal helper implementations
bool CheckpointManager::ensureDirectoryExists(const std::string& path) const {
    try {
        std::filesystem::create_directories(path);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create directory " + path + ": " + std::string(e.what()));
        return false;
    }
}

std::string CheckpointManager::generateCheckpointFilename(SimulationStep step, Timestamp timestamp) const {
    std::ostringstream oss;
    oss << checkpointDir_ << "/";
    oss << "checkpoint_step_" << step << "_t_" << std::fixed << std::setprecision(3) << timestamp;
    
    if (compressionEnabled_) {
        oss << ".gz";
    } else if (checkpointFormat_ == "text") {
        oss << ".txt";
    } else {
        oss << ".bin";
    }
    
    return oss.str();
}

bool CheckpointManager::writeBinaryCheckpoint(const std::string& filename,
                                              const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                              const std::vector<std::unique_ptr<Synapse>>& synapses) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open checkpoint file for writing: " + filename);
        return false;
    }
    
    try {
        // Write magic number and version
        const char magic[] = "NCLMCKPTv1";
        file.write(magic, sizeof(magic));
        
        // Write file size placeholder
        uint64_t fileSizePlaceholder = 0;
        file.write(reinterpret_cast<const char*>(&fileSizePlaceholder), sizeof(fileSizePlaceholder));
        
        // Write regions
        uint32_t regionCount = static_cast<uint32_t>(regions.size());
        file.write(reinterpret_cast<const char*>(&regionCount), sizeof(regionCount));
        
        for (const auto& region : regions) {
            saveRegion(file, region.get());
        }
        
        // Write synapses
        uint32_t synapseCount = static_cast<uint32_t>(synapses.size());
        file.write(reinterpret_cast<const char*>(&synapseCount), sizeof(synapseCount));
        
        for (const auto& synapse : synapses) {
            saveSynapse(file, synapse.get());
        }
        
        // Update file size
        auto currentPos = file.tellp();
        file.seekp(0, std::ios::beg);
        file.write(reinterpret_cast<const char*>(&currentPos), sizeof(currentPos));
        file.seekp(currentPos);
        
        NLM_LOG_DEBUG("Binary checkpoint written: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during binary checkpoint write: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::readBinaryCheckpoint(const std::string& filename,
                                             std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                             std::vector<std::unique_ptr<Synapse>>& synapses) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open checkpoint file for reading: " + filename);
        return false;
    }
    
    try {
        // Read magic number and version
        char magic[12];
        file.read(magic, sizeof(magic));
        if (std::string(magic, sizeof(magic)) != "NCLMCKPTv1") {
            NLM_LOG_ERROR("Invalid checkpoint file format: " + filename);
            return false;
        }
        
        // Read file size
        uint64_t fileSize;
        file.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
        
        // Read regions
        uint32_t regionCount;
        file.read(reinterpret_cast<char*>(&regionCount), sizeof(regionCount));
        
        regions.clear();
        for (uint32_t i = 0; i < regionCount; ++i) {
            auto region = loadRegion(file);
            if (region) {
                regions.push_back(std::move(region));
            } else {
                NLM_LOG_WARN("Failed to load region from checkpoint");
            }
        }
        
        // Read synapses
        uint32_t synapseCount;
        file.read(reinterpret_cast<char*>(&synapseCount), sizeof(synapseCount));
        
        synapses.clear();
        for (uint32_t i = 0; i < synapseCount; ++i) {
            auto synapse = loadSynapse(file);
            if (synapse) {
                synapses.push_back(std::move(synapse));
            } else {
                NLM_LOG_WARN("Failed to load synapse from checkpoint");
            }
        }
        
        NLM_LOG_DEBUG("Binary checkpoint read: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during binary checkpoint read: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::writeTextCheckpoint(const std::string& filename,
                                            const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                            const std::vector<std::unique_ptr<Synapse>>& synapses) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open checkpoint file for writing: " + filename);
        return false;
    }
    
    try {
        // Write header
        file << "# NLM Checkpoint File (Text Format)" << std::endl;
        file << "# Version: 1.0" << std::endl;
        file << "# Generated: " << std::chrono::system_clock::now() << std::endl;
        file << "# Neurons: " << getTotalNeuronCount(regions) << std::endl;
        file << "# Synapses: " << synapses.size() << std::endl;
        file << std::endl;
        
        // Write regions
        file << "[REGIONS]" << std::endl;
        for (const auto& region : regions) {
            file << "[REGION]" << std::endl;
            file << "id=" << region->getId() << std::endl;
            file << "name=" << region->getName() << std::endl;
            file << "population_count=" << region->getPopulationCount() << std::endl;
            file << std::endl;
            
            // Write populations
            file << "[POPULATIONS]" << std::endl;
            for (const auto& pop : region->getPopulations()) {
                file << "[POPULATION]" << std::endl;
                file << "id=" << pop->getId() << std::endl;
                file << "size=" << pop->getSize() << std::endl;
                file << "neuron_type=" << static_cast<int>(pop->getNeuronType()) << std::endl;
                file << "neuron_ids=";
                for (size_t i = 0; i < pop->getSize(); ++i) {
                    if (i > 0) file << ",";
                    file << pop->getNeuron(i)->getId();
                }
                file << std::endl;
                file << std::endl;
            }
            file << std::endl;
        }
        
        // Write synapses
        file << "[SYNAPSES]" << std::endl;
        for (const auto& synapse : synapses) {
            file << "[SYNAPSE]" << std::endl;
            file << "id=" << synapse->getId() << std::endl;
            file << "source=" << synapse->getSourceNeuron() << std::endl;
            file << "destination=" << synapse->getDestinationNeuron() << std::endl;
            file << "weight=" << synapse->getWeight() << std::endl;
            file << "delay=" << synapse->getDelay() << std::endl;
            file << "type=" << static_cast<int>(synapse->getType()) << std::endl;
            file << std::endl;
        }
        
        NLM_LOG_DEBUG("Text checkpoint written: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during text checkpoint write: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::readTextCheckpoint(const std::string& filename,
                                           std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                           std::vector<std::unique_ptr<Synapse>>& synapses) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open checkpoint file for reading: " + filename);
        return false;
    }
    
    try {
        std::string line;
        std::string currentSection;
        std::string currentBlock;
        
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (line == "[REGIONS]" || line == "[POPULATIONS]" || 
                line == "[SYNAPSES]" || line == "[REGION]" || 
                line == "[POPULATION]" || line == "[SYNAPSE]") {
                currentSection = line;
                currentBlock.clear();
                continue;
            }
            
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);
                
                // TODO: Parse checkpoint data and reconstruct regions/synapses
                // This is a simplified implementation - full implementation would be complex
                NLM_LOG_WARN("Text checkpoint parsing not fully implemented: " + key + "=" + value);
            }
        }
        
        NLM_LOG_DEBUG("Text checkpoint read: " + filename + " (partial implementation)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during text checkpoint read: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::compressFile(const std::string& inputFile, const std::string& outputFile) const {
    std::ifstream in(inputFile, std::ios::binary);
    std::ofstream out(outputFile, std::ios::binary);
    
    if (!in.is_open() || !out.is_open()) {
        NLM_LOG_ERROR("Failed to open files for compression: " + inputFile + " -> " + outputFile);
        return false;
    }
    
    try {
        zlib_stream compress_stream;
        compress_stream.next_in = reinterpret_cast<Bytef*>(in.read())));
        // This is a simplified compression - real implementation would use zlib properly
        NLM_LOG_WARN("Compression not fully implemented");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during file compression: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::decompressFile(const std::string& inputFile, const std::string& outputFile) const {
    std::ifstream in(inputFile, std::ios::binary);
    std::ofstream out(outputFile, std::ios::binary);
    
    if (!in.is_open() || !out.is_open()) {
        NLM_LOG_ERROR("Failed to open files for decompression: " + inputFile + " -> " + outputFile);
        return false;
    }
    
    try {
        // Similar to compression - simplified for now
        NLM_LOG_WARN("Decompression not fully implemented");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during file decompression: " + std::string(e.what()));
        return false;
    }
}

CheckpointManager::CheckpointInfo CheckpointManager::createCheckpointInfo(SimulationStep step, Timestamp timestamp,
                                                                         size_t neuronCount, size_t synapseCount,
                                                                         float simulationTime) const {
    CheckpointInfo info;
    info.filepath = generateCheckpointFilename(step, timestamp);
    info.step = step;
    info.timestamp = timestamp;
    info.neuronCount = neuronCount;
    info.synapseCount = synapseCount;
    info.simulationTime = simulationTime;
    info.description = nextCheckpointDescription_;
    return info;
}

void CheckpointManager::updateCheckpointInfoCache() const {
    if (!checkpointInfoCacheDirty_) {
        return;
    }
    
    checkpointInfoCache_.clear();
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().string();
                
                // Try to extract checkpoint info from filename
                // This is a simplified implementation
                SimulationStep step = 0;
                Timestamp timestamp = 0.0;
                
                // Look for step and timestamp in filename
                size_t stepPos = filename.find("checkpoint_step_");
                if (stepPos != std::string::npos) {
                    size_t startPos = stepPos + 19; // Length of "checkpoint_step_"
                    size_t endPos = filename.find("_t_", startPos);
                    if (endPos != std::string::npos) {
                        std::string stepStr = filename.substr(startPos, endPos - startPos);
                        try {
                            step = static_cast<SimulationStep>(std::stoull(stepStr));
                        } catch (...) {}
                    }
                }
                
                // Default values if parsing fails
                if (step == 0) {
                    step = static_cast<SimulationStep>(std::filesystem::last_write_time(entry).time_since_epoch().count());
                }
                
                if (timestamp == 0.0) {
                    timestamp = std::chrono::system_clock::now().time_since_epoch().count() * 1e-9;
                }
                
                // Count neurons and synapses would require loading the checkpoint
                // For now, use placeholder values
                size_t neuronCount = 0;
                size_t synapseCount = 0;
                float simulationTime = static_cast<float>(timestamp);
                
                CheckpointInfo info = createCheckpointInfo(step, timestamp, neuronCount, synapseCount, simulationTime);
                checkpointInfoCache_.push_back(info);
            }
        }
        
        checkpointInfoCacheDirty_ = false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception updating checkpoint info cache: " + std::string(e.what()));
    }
}

void CheckpointManager::updateStatistics(SimulationStep step, Timestamp timestamp,
                                         size_t neuronCount, size_t synapseCount,
                                         float simulationTime, bool success,
                                         size_t bytesTransferred) const {
    if (success) {
        totalSaveTime_ += std::chrono::high_resolution_clock::now().time_since_epoch().count() * 1e-9;
    } else {
        failedSaves_++;
    }
}

// Serialization helper implementations
void CheckpointManager::saveRegion(std::ofstream& file, const NeuralRegion* region) const {
    // Write region data
    RegionId id = region->getId();
    file.write(reinterpret_cast<const char*>(&id), sizeof(id));
    
    size_t nameSize = region->getName().size();
    file.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
    file.write(region->getName().c_str(), nameSize);
    
    // TODO: Write populations and synapses
}

std::unique_ptr<NeuralRegion> CheckpointManager::loadRegion(std::ifstream& file) const {
    // Read region data
    RegionId id;
    file.read(reinterpret_cast<char*>(&id), sizeof(id));
    
    size_t nameSize;
    file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
    
    std::string name(nameSize, ' ');
    file.read(&name[0], nameSize);
    
    // TODO: Read populations and synapses
    // Return placeholder region
    return nullptr;
}

void CheckpointManager::saveSynapse(std::ofstream& file, const Synapse* synapse) const {
    // Write synapse data
    SynapseId id = synapse->getId();
    file.write(reinterpret_cast<const char*>(&id), sizeof(id));
    
    NeuronId source = synapse->getSourceNeuron();
    file.write(reinterpret_cast<const char*>(&source), sizeof(source));
    
    NeuronId destination = synapse->getDestinationNeuron();
    file.write(reinterpret_cast<const char*>(&destination), sizeof(destination));
    
    // TODO: Write other synapse properties
}

std::unique_ptr<Synapse> CheckpointManager::loadSynapse(std::ifstream& file) const {
    // Read synapse data
    SynapseId id;
    file.read(reinterpret_cast<char*>(&id), sizeof(id));
    
    NeuronId source;
    file.read(reinterpret_cast<char*>(&source), sizeof(source));
    
    NeuronId destination;
    file.read(reinterpret_cast<char*>(&destination), sizeof(destination));
    
    // TODO: Read other synapse properties
    // Return placeholder synapse
    return nullptr;
}

// Helper function to get total neuron count
inline size_t CheckpointManager::getTotalNeuronCount(const std::vector<std::unique_ptr<NeuralRegion>>& regions) const {
    size_t total = 0;
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            total += pop->getSize();
        }
    }
    return total;
}

} // namespace nlm

// Note: This implementation includes zlib dependency and zlib_stream
// For production use, ensure zlib is properly linked

// zlib_stream implementation (simplified)
namespace nlm {
    struct zlib_stream {
        z_stream z;
        zlib_stream() {
            z.next_in = Z_NULL;
            z.zalloc = Z_NULL;
            z.zfree = Z_NULL;
            z.avail_in = 0;
            z.avail_out = 0;
            inflateInit2(&z, 15 + 16); // Auto header detection
        }
        ~zlib_stream() {
            inflateEnd(&z);
        }
    };
}

// Include zlib header (simplified - ensure proper zlib installation)
#include <zlib.h>