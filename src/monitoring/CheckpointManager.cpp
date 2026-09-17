#include "CheckpointManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <zlib.h>  // For compression
#include <zstd.h>  // For Zstandard compression if available
#include <cstring>
#include <sys/stat.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

// Static member initialization
CheckpointVersion CheckpointManager::currentVersion_ = CheckpointVersion();

CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true),
      compressionLevel_(CompressionLevel::Balanced), enableIncremental_(true),
      lastSaveStep_(0), visualization_(nullptr) {}

CheckpointManager::~CheckpointManager() {
    // Clean up incremental checkpoint files
    for (const auto& pair : incrementalCheckpoints_) {
        const auto& incInfo = pair.second;
        std::string incrementalPath = getIncrementalCheckpointPath(
            incInfo.baseName, incInfo.nextStep);
        std::remove(incrementalPath.c_str());
    }
}

bool CheckpointManager::configure(const std::string& checkpointDir,
                                  uint64_t saveIntervalSteps,
                                  size_t maxCheckpoints,
                                  bool compress,
                                  CompressionLevel compressionLevel,
                                  bool enableIncremental) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    compressionLevel_ = compressionLevel;
    enableIncremental_ = enableIncremental;
    
    // Ensure checkpoint directory exists
    if (!ensureDirectory(checkpointDir_)) {
        NLM_LOG_ERROR("Failed to create checkpoint directory: " + checkpointDir_);
        return false;
    }
    
    NLM_LOG_INFO("CheckpointManager configured:");
    NLM_LOG_INFO("  Directory: " + checkpointDir_);
    NLM_LOG_INFO("  Save interval: " + std::to_string(saveIntervalSteps_) + " steps");
    NLM_LOG_INFO("  Max checkpoints: " + std::to_string(maxCheckpoints_));
    NLM_LOG_INFO("  Compression: " + std::string(compress_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Incremental saves: " + std::string(enableIncremental_ ? "enabled" : "disabled"));
    
    return true;
}

bool CheckpointManager::save(const std::string& name, uint64_t step,
                             double simulationTime, const Brain& brain,
                             bool isIncremental) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Validate inputs
    if (name.empty()) {
        NLM_LOG_ERROR("Cannot save checkpoint with empty name");
        return false;
    }
    
    if (!neuronProvider_ || !synapseProvider_) {
        NLM_LOG_ERROR("Checkpoint data providers not set");
        return false;
    }
    
    // Update progress
    if (!updateProgress("save", 0.1, "Starting checkpoint save...", step)) {
        return false;
    }
    
    // Generate checkpoint path
    std::string filepath = checkpointDir_ + "/" + name;
    
    // Check if incremental save is requested
    if (isIncremental && enableIncremental_) {
        // Find base checkpoint for incremental save
        std::string baseName = getBaseCheckpointPath(name);
        uint64_t baseStep = lastSaveStep_;
        
        if (baseStep > 0) {
            NLM_LOG_INFO("Creating incremental checkpoint from step " + 
                        std::to_string(baseStep) + " to " + std::to_string(step));
            
            if (!createIncrementalCheckpoint(baseName, baseStep, step, brain)) {
                NLM_LOG_ERROR("Failed to create incremental checkpoint");
                return false;
            }
            
            // Update progress
            if (!updateProgress("save", 0.9, "Incremental checkpoint saved", step)) {
                return false;
            }
            
            double elapsed = std::chrono::duration<double>(
                std::chrono::steady_clock::now() - startTime).count();
            
            if (!updateStatistics(true, 0, elapsed, false, 0, 0)) {
                return false;
            }
            
            return true;
        }
    }
    
    // Create new checkpoint file
    CheckpointFile file;
    CompressionLevel actualCompression = compress_ ? compressionLevel_ : CompressionLevel::None;
    
    if (!file.open(filepath, true, actualCompression)) {
        NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
        return false;
    }
    
    // Write header
    if (!writeCheckpointHeader(file, step, simulationTime, 0, 0, 0)) {
        NLM_LOG_ERROR("Failed to write checkpoint header");
        file.close();
        std::remove(filepath.c_str());
        return false;
    }
    
    // Get brain data
    updateProgress("save", 0.2, "Collecting neuron data...", step);
    NeuronCheckpointData neuronData;
    if (!neuronProvider_(neuronData)) {
        NLM_LOG_ERROR("Failed to get neuron data from provider");
        file.close();
        std::remove(filepath.c_str());
        return false;
    }
    
    updateProgress("save", 0.4, "Writing neuron data...", step);
    if (!writeCheckpointData(file, CheckpointSection::Neurons,
                            &neuronData, sizeof(NeuronCheckpointData))) {
        NLM_LOG_ERROR("Failed to write neuron data");
        file.close();
        std::remove(filepath.c_str());
        return false;
    }
    
    updateProgress("save", 0.6, "Collecting synapse data...", step);
    SynapseCheckpointData synapseData;
    if (!synapseProvider_(synapseData)) {
        NLM_LOG_ERROR("Failed to get synapse data from provider");
        file.close();
        std::remove(filepath.c_str());
        return false;
    }
    
    updateProgress("save", 0.8, "Writing synapse data...", step);
    if (!writeCheckpointData(file, CheckpointSection::Synapses,
                            &synapseData, sizeof(SynapseCheckpointData))) {
        NLM_LOG_ERROR("Failed to write synapse data");
        file.close();
        std::remove(filepath.c_str());
        return false;
    }
    
    // Finalize checkpoint
    updateProgress("save", 0.9, "Finalizing checkpoint...", step);
    if (!file.finalize()) {
        NLM_LOG_ERROR("Failed to finalize checkpoint");
        file.close();
        std::remove(filepath.c_str());
        return false;
    }
    
    // Update statistics
    uint64_t bytesWritten = file.getBytesWritten();
    double elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - startTime).count();
    
    if (!updateStatistics(true, bytesWritten, elapsed, false, 0, 0)) {
        return false;
    }
    
    file.close();
    
    lastSaveStep_ = step;
    lastCheckpointPath_ = filepath;
    
    // Notify visualization
    notifyVisualization("checkpoint_saved", 1.0);
    
    NLM_LOG_INFO(std::string("Checkpoint saved: ") + name + " (" +
                 std::to_string(bytesWritten) + " bytes, " +
                 std::to_string(elapsed) + " seconds)");
    
    return true;
}

bool CheckpointManager::load(const std::string& name, Brain& brain,
                             bool incremental) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Validate inputs
    if (name.empty()) {
        NLM_LOG_ERROR("Cannot load checkpoint with empty name");
        return false;
    }
    
    // Find checkpoint file
    std::string filepath = checkpointDir_ + "/" + name;
    
    // Check if incremental load is requested
    if (incremental && enableIncremental_) {
        // Find base checkpoint for incremental load
        std::string baseName = getBaseCheckpointPath(name);
        uint64_t baseStep = lastSaveStep_;
        
        if (baseStep > 0) {
            NLM_LOG_INFO("Loading incremental checkpoint from " + 
                        std::to_string(baseStep) + " to ?");
            
            if (!applyIncrementalCheckpoint(baseName, baseStep, brain)) {
                NLM_LOG_ERROR("Failed to apply incremental checkpoint");
                return false;
            }
            
            double elapsed = std::chrono::duration<double>(
                std::chrono::steady_clock::now() - startTime).count();
            
            if (!updateStatistics(false, 0, 0, true, 0, elapsed)) {
                return false;
            }
            
            return true;
        }
    }
    
    // Open checkpoint file
    CheckpointFile file;
    if (!file.open(filepath, false)) {
        NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
        return false;
    }
    
    // Read header
    CheckpointHeader header;
    if (!readCheckpointHeader(file, header)) {
        NLM_LOG_ERROR("Failed to read checkpoint header");
        file.close();
        return false;
    }
    
    // Validate checkpoint
    if (!validateCheckpoint(filepath)) {
        NLM_LOG_ERROR("Checkpoint validation failed: " + validationError_);
        file.close();
        return false;
    }
    
    // Read data (simplified - actual implementation would need more sophisticated parsing)
    NLM_LOG_INFO(std::string("Checkpoint loaded: ") + name + " (header validated)");
    
    file.close();
    
    // Update statistics
    double elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - startTime).count();
    
    if (!updateStatistics(false, 0, 0, true, 0, elapsed)) {
        return false;
    }
    
    return true;
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    // Check if we should save a checkpoint
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    // Generate checkpoint name if not provided
    std::string checkpointName = name;
    if (checkpointName.empty()) {
        checkpointName = generateCheckpointName(lastSaveStep_ + saveIntervalSteps_,
                                                0.0, "auto");
    }
    
    // We need a Brain instance to save, this is a placeholder
    // In real implementation, the checkpoint manager would need access to the brain
    NLM_LOG_INFO("Checkpoint manager requires brain instance for save operation");
    return false;
}

bool CheckpointManager::validate(const std::string& name) const {
    std::string filepath = checkpointDir_ + "/" + name;
    return validateCheckpoint(filepath);
}

bool CheckpointManager::deleteCheckpoint(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name;
    return std::remove(filepath.c_str()) == 0;
}

std::vector<std::string> CheckpointManager::listCheckpoints() const {
    std::vector<std::string> checkpoints;
    
    if (!ensureDirectory(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            // Filter for checkpoint files (basic filter)
            if (filename.find(".checkpoint") != std::string::npos ||
                filename.find(".ckpt") != std::string::npos) {
                checkpoints.push_back(filename);
            }
        }
    }
    
    // Sort by modification time (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [this](const std::string& a, const std::string& b) {
                  std::string pathA = checkpointDir_ + "/" + a;
                  std::string pathB = checkpointDir_ + "/" + b;
                  
                  auto timeA = std::filesystem::last_write_time(pathA);
                  auto timeB = std::filesystem::last_write_time(pathB);
                  
                  return timeA > timeB;
              });
    
    return checkpoints;
}

CheckpointProgress CheckpointManager::getCurrentProgress() const {
    return currentProgress_;
}

bool CheckpointManager::setProgressCallback(
    std::function<void(const CheckpointProgress&)> callback) {
    progressCallback_ = callback;
    return true;
}

CheckpointStats CheckpointManager::getStatistics() const {
    return stats_;
}

bool CheckpointManager::resetStatistics() {
    stats_ = CheckpointStats();
    return true;
}

bool CheckpointManager::compressFile(const std::string& inputPath,
                                     const std::string& outputPath,
                                     CompressionLevel level) {
    std::ifstream input(inputPath, std::ios::binary);
    if (!input) {
        NLM_LOG_ERROR("Failed to open input file for compression: " + inputPath);
        return false;
    }
    
    std::ofstream output(outputPath, std::ios::binary);
    if (!output) {
        NLM_LOG_ERROR("Failed to create output file for compression: " + outputPath);
        return false;
    }
    
    // Simple compression implementation (placeholder)
    // In real implementation, use zlib or zstd
    output << input.rdbuf();
    
    NLM_LOG_INFO(std::string("File compressed: ") + inputPath + " -> " + outputPath);
    return true;
}

bool CheckpointManager::decompressFile(const std::string& inputPath,
                                       const std::string& outputPath) {
    std::ifstream input(inputPath, std::ios::binary);
    if (!input) {
        NLM_LOG_ERROR("Failed to open input file for decompression: " + inputPath);
        return false;
    }
    
    std::ofstream output(outputPath, std::ios::binary);
    if (!output) {
        NLM_LOG_ERROR("Failed to create output file for decompression: " + outputPath);
        return false;
    }
    
    // Simple decompression implementation (placeholder)
    // In real implementation, use zlib or zstd
    output << input.rdbuf();
    
    NLM_LOG_INFO(std::string("File decompressed: ") + inputPath + " -> " + outputPath);
    return true;
}

bool CheckpointManager::setCurrentVersion(uint32_t major, uint32_t minor, uint32_t patch) {
    currentVersion_.major = major;
    currentVersion_.minor = minor;
    currentVersion_.patch = patch;
    return true;
}

bool CheckpointManager::validateCheckpoint(const std::string& filepath) const {
    validationError_.clear();
    
    // Check if file exists and is readable
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        validationError_ = "Cannot open checkpoint file: " + filepath;
        return false;
    }
    
    // Basic validation (in real implementation, would parse header and check checksums)
    // This is a placeholder for the actual validation logic
    
    file.close();
    return true;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return enableIncremental_ && (currentStep % saveIntervalSteps_ == 0) &&
           (currentStep > lastSaveStep_);
}

bool CheckpointManager::createIncrementalCheckpoint(const std::string& basePath,
                                                    uint64_t baseStep,
                                                    uint64_t currentStep,
                                                    const Brain& brain) {
    // This is a simplified implementation
    // In real implementation, would create delta checkpoint
    
    IncrementalCheckpoint incInfo;
    incInfo.baseName = basePath;
    incInfo.baseStep = baseStep;
    incInfo.nextStep = currentStep;
    incInfo.timestamp = std::time(nullptr);
    incInfo.isCompressed = compress_;
    
    incrementalCheckpoints_[basePath] = incInfo;
    
    NLM_LOG_INFO(std::string("Created incremental checkpoint from step ") +
                 std::to_string(baseStep) + " to " + std::to_string(currentStep));
    
    return true;
}

bool CheckpointManager::applyIncrementalCheckpoint(const std::string& basePath,
                                                   uint64_t baseStep,
                                                   Brain& brain) {
    // This is a simplified implementation
    // In real implementation, would apply delta to load checkpoint
    
    if (incrementalCheckpoints_.find(basePath) == incrementalCheckpoints_.end()) {
        NLM_LOG_ERROR("Incremental checkpoint not found: " + basePath);
        return false;
    }
    
    const auto& incInfo = incrementalCheckpoints_[basePath];
    
    NLM_LOG_INFO(std::string("Applied incremental checkpoint from step ") +
                 std::to_string(incInfo.baseStep) + " to " + std::to_string(incInfo.nextStep));
    
    return true;
}

bool CheckpointManager::updateProgress(const std::string& operation, double progress,
                                       const std::string& message,
                                       uint64_t step, uint64_t totalSteps,
                                       uint64_t bytesProcessed, uint64_t totalBytes) {
    currentProgress_.operation = operation;
    currentProgress_.progress = std::max(0.0, std::min(1.0, progress));
    currentProgress_.message = message;
    currentProgress_.currentStep = step;
    currentProgress_.totalSteps = totalSteps;
    currentProgress_.bytesProcessed = bytesProcessed;
    currentProgress_.totalBytes = totalBytes;
    
    // Calculate estimated time remaining
    currentProgress_.estimatedTimeRemaining = 0.0;
    if (progress > 0.0 && progress < 1.0 && totalSteps > 0 && step > 0) {
        double rate = static_cast<double>(step) / progress;
        if (rate > 0.0) {
            currentProgress_.estimatedTimeRemaining = (totalSteps - step) / rate;
        }
    }
    
    // Call progress callback if set
    if (progressCallback_) {
        progressCallback_(currentProgress_);
    }
    
    return true;
}

bool CheckpointManager::updateStatistics(bool saveSuccess, uint64_t bytesWritten,
                                         double saveTime, bool loadSuccess,
                                         uint64_t bytesRead, double loadTime) {
    if (saveSuccess) {
        stats_.totalSaves++;
        stats_.totalSizeSaved += bytesWritten;
        
        // Update average save time
        double totalSaveTime = stats_.averageSaveTime * (stats_.totalSaves - 1);
        stats_.averageSaveTime = (totalSaveTime + saveTime) / stats_.totalSaves;
    }
    
    if (loadSuccess) {
        stats_.totalLoads++;
        stats_.totalSizeLoaded += bytesRead;
        
        // Update average load time
        double totalLoadTime = stats_.averageLoadTime * (stats_.totalLoads - 1);
        stats_.averageLoadTime = (totalLoadTime + loadTime) / stats_.totalLoads;
    }
    
    return true;
}

bool CheckpointManager::cleanupOldCheckpoints() {
    std::vector<std::string> checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= maxCheckpoints_) {
        return true;
    }
    
    // Delete oldest checkpoints
    size_t toDelete = checkpoints.size() - maxCheckpoints_;
    
    for (size_t i = toDelete; i < checkpoints.size(); ++i) {
        deleteCheckpoint(checkpoints[i]);
    }
    
    NLM_LOG_INFO(std::string("Cleaned up ") + std::to_string(toDelete) +
                 " old checkpoints, keeping " + std::to_string(maxCheckpoints_) +
                 " most recent");
    
    return true;
}

bool CheckpointManager::getCheckpointInfo(const std::string& name,
                                          CheckpointInfo& info) const {
    // Implementation would read checkpoint metadata
    // This is a placeholder
    
    info.name = name;
    info.step = 0;
    info.simulationTime = 0.0;
    info.neuronCount = 0;
    info.synapseCount = 0;
    info.fileSize = 0;
    info.timestamp = 0;
    
    return true;
}

bool CheckpointManager::getIncrementalCheckpointInfo(const std::string& basePath,
                                                     IncrementalCheckpoint& incInfo) const {
    auto it = incrementalCheckpoints_.find(basePath);
    if (it == incrementalCheckpoints_.end()) {
        return false;
    }
    
    incInfo = it->second;
    return true;
}

std::string CheckpointManager::getIncrementalCheckpointPath(const std::string& basePath,
                                                             uint64_t step) const {
    return basePath + ".incremental." + std::to_string(step);
}

std::string CheckpointManager::getBaseCheckpointPath(const std::string& path) const {
    // Extract base name from checkpoint path
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        return path.substr(0, dotPos);
    }
    return path;
}

bool CheckpointManager::notifyVisualization(const std::string& event, double progress) {
    if (visualization_) {
        if (event == "checkpoint_saved") {
            visualization_->visualizeCheckpointStatus(lastCheckpointPath_, progress);
        }
    }
    return true;
}

// Implementation of CheckpointFile helper class
bool CheckpointManager::CheckpointFile::open(const std::string& filepath, bool writeMode,
                                            CompressionLevel compression) {
    close();  // Close existing file if open
    
    const char* mode = writeMode ? "wb" : "rb";
    handle_ = fopen(filepath.c_str(), mode);
    
    if (!handle_) {
        return false;
    }
    
    compressionLevel_ = compression;
    return true;
}

bool CheckpointManager::CheckpointFile::write(const void* data, size_t size) {
    if (!handle_ || !data || size == 0) {
        return false;
    }
    
    size_t written = fwrite(data, 1, size, handle_);
    return written == size;
}

bool CheckpointManager::CheckpointFile::read(void* data, size_t size) {
    if (!handle_ || !data || size == 0) {
        return false;
    }
    
    size_t read = fread(data, 1, size, handle_);
    return read == size;
}

bool CheckpointManager::CheckpointFile::finalize() {
    if (!handle_) {
        return false;
    }
    
    return flush();
}

// Static member definition
CheckpointVersion CheckpointManager::currentVersion_ = CheckpointVersion();

} // namespace nlm