#include "BrainPersistenceManager.hpp"
#include "../Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

BrainPersistenceManager::BrainPersistenceManager(std::shared_ptr<Config> config)
    : maxCheckpoints(10)
    , checkpointInterval(1000)
    , compressionEnabled(true)
    , incrementalBackupEnabled(false)
    , diffStorageEnabled(false)
{
}

bool BrainPersistenceManager::saveBrainState(Brain* brain, const std::string& filepath) const {
    return brain->save(filepath);
}

bool BrainPersistenceManager::loadBrainState(Brain* brain, const std::string& filepath) {
    return brain->load(filepath);
}

void BrainPersistenceManager::configureCheckpoint(Brain* brain, const std::string& directory, 
                                                 size_t maxCheckpoints, size_t checkpointInterval,
                                                 bool compressionEnabled) {
    this->checkpointDirectory = directory;
    this->maxCheckpoints = maxCheckpoints;
    this->checkpointInterval = checkpointInterval;
    this->compressionEnabled = compressionEnabled;
    
    if (checkpointManager) {
        checkpointManager->configure(directory, maxCheckpoints, 5, compressionEnabled);
    }
}

bool BrainPersistenceManager::hasCheckpoint(Brain* brain, const std::string& filepath) const {
    // Check if checkpoint exists
    std::ifstream file(filepath);
    return file.good();
}

bool BrainPersistenceManager::validateCheckpoint(Brain* brain, const std::string& filepath) const {
    // Validate checkpoint file
    if (!hasCheckpoint(brain, filepath)) {
        checkpointError = "Checkpoint file not found: " + filepath;
        return false;
    }
    
    // Additional validation logic would go here
    // For now, just return true if file exists
    return true;
}

void BrainPersistenceManager::cleanupOldCheckpoints(Brain* brain) {
    if (checkpointManager) {
        checkpointManager->cleanupOldCheckpoints(maxCheckpoints);
    }
}

void BrainPersistenceManager::removeCheckpoint(Brain* brain, const std::string& filepath) {
    // Remove checkpoint file
    std::remove(filepath.c_str());
}

BrainPersistenceManager::BrainStatistics BrainPersistenceManager::collectBrainStatistics(Brain* brain) const {
    BrainStatistics stats;
    stats.totalNeuronCount = brain->getTotalNeuronCount();
    stats.totalSynapseCount = brain->getTotalSynapseCount();
    stats.regionCount = brain->getRegionCount();
    stats.currentStep = 0;  // Should come from brain state
    stats.currentTime = 0.0;  // Should come from brain state
    stats.totalSpikes = brain->getTotalSpikeCount();
    stats.averageFiringRate = brain->getAverageFiringRate();
    stats.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    return stats;
}

void BrainPersistenceManager::enableIncrementalBackup(Brain* brain, bool enabled) {
    incrementalBackupEnabled = enabled;
}

void BrainPersistenceManager::enableCheckpointCompression(Brain* brain, bool enabled) {
    compressionEnabled = enabled;
    if (checkpointManager) {
        checkpointManager->setCompressionEnabled(enabled);
    }
}

void BrainPersistenceManager::enableDiffStorage(Brain* brain, bool enabled) {
    diffStorageEnabled = enabled;
}

void BrainPersistenceManager::resetPersistenceManager(Brain* brain) {
    checkpointError.clear();
}

void BrainPersistenceManager::updateCheckpointInterval(Brain* brain, size_t newInterval) {
    checkpointInterval = newInterval;
}

void BrainPersistenceManager::setCheckpointDirectory(Brain* brain, const std::string& directory) {
    checkpointDirectory = directory;
}

void BrainPersistenceManager::createCheckpointDirectory(const std::string& directory) {
    // Create checkpoint directory if it doesn't exist
    std::filesystem::create_directories(directory);
}

void BrainPersistenceManager::writeBrainMetadata(Brain* brain, const BrainStatistics& stats) const {
    // Write brain statistics to metadata file
    std::string metadataFile = checkpointDirectory + "/metadata.txt";
    std::ofstream file(metadataFile);
    if (file.is_open()) {
        file << "Total neurons: " << stats.totalNeuronCount << "\n";
        file << "Total synapses: " << stats.totalSynapseCount << "\n";
        file << "Region count: " << stats.regionCount << "\n";
        file << "Current step: " << stats.currentStep << "\n";
        file << "Current time: " << stats.currentTime << "\n";
        file << "Total spikes: " << stats.totalSpikes << "\n";
        file << "Average firing rate: " << stats.averageFiringRate << "\n";
        file << "E/I ratio: " << stats.excitationInhibitionRatio << "\n";
    }
}

void BrainPersistenceManager::readBrainMetadata(Brain* brain, BrainStatistics& stats) const {
    // Read brain statistics from metadata file
    std::string metadataFile = checkpointDirectory + "/metadata.txt";
    std::ifstream file(metadataFile);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            // Parse metadata lines (simplified)
        }
    }
}

void BrainPersistenceManager::validateCheckpointData(Brain* brain, const std::string& filepath) const {
    // Validate checkpoint data integrity
    // This would include checking checksums, data consistency, etc.
}

void BrainPersistenceManager::restoreBrainStateFromCheckpoint(Brain* brain, const std::string& filepath) {
    // Restore brain state from checkpoint
    brain->load(filepath);
}

void BrainPersistenceManager::enableIncrementalBackup(Brain* brain) {
    incrementalBackupEnabled = true;
}

void BrainPersistenceManager::enableCheckpointCompression(Brain* brain) {
    compressionEnabled = true;
    if (checkpointManager) {
        checkpointManager->setCompressionEnabled(true);
    }
}

void BrainPersistenceManager::enableDiffStorage(Brain* brain) {
    diffStorageEnabled = true;
}

void BrainPersistenceManager::compressCheckpoint(const std::string& filepath) const {
    // Compress checkpoint file
    // Implementation would depend on compression library
}

void BrainPersistenceManager::storeCheckpointDiff(const std::string& oldFile, const std::string& newFile) const {
    // Store difference between two checkpoint files for efficient storage
}

void BrainPersistenceManager::createIncrementalBackup(const std::string& filepath) const {
    // Create incremental backup of checkpoint
    // Implementation would create backup of specific changes
}