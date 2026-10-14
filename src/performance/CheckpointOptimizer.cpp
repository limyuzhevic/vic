#include "CheckpointOptimizer.hpp"

namespace nlm {

// DifferentialCalculator implementation

std::vector<DeltaRecord> DifferentialCalculator::calculateDelta(const std::vector<uint8_t>& oldData,
                                                              const std::vector<uint8_t>& newData) {
    std::vector<DeltaRecord> deltas;
    
    // Simple delta calculation for demonstration
    // In a real implementation, this would use more sophisticated algorithms
    if (oldData.size() != newData.size()) {
        return deltas; // Cannot calculate delta with different sizes
    }
    
    size_t deltaStart = 0;
    for (size_t i = 0; i < oldData.size(); ++i) {
        if (oldData[i] != newData[i]) {
            deltaStart = i;
            break;
        }
    }
    
    if (deltaStart == oldData.size()) {
        return deltas; // No changes
    }
    
    size_t deltaEnd = oldData.size();
    for (size_t i = oldData.size() - 1; i > deltaStart; --i) {
        if (oldData[i] != newData[i]) {
            deltaEnd = i + 1;
            break;
        }
    }
    
    DeltaRecord delta;
    delta.offset = deltaStart;
    delta.size = deltaEnd - deltaStart;
    delta.checksum = calculateChecksum(&newData[deltaStart], delta.size);
    delta.compression = CompressionAlgorithm::Zstd;
    
    // Compress the delta
    delta.data = compressor_.compress(&newData[deltaStart], delta.size, 
                                     CompressionAlgorithm::Zstd, 6);
    
    deltas.push_back(delta);
    return deltas;
}

std::vector<uint8_t> DifferentialCalculator::applyDelta(const std::vector<uint8_t>& baseData,
                                                         const std::vector<DeltaRecord>& deltaRecords) {
    std::vector<uint8_t> result = baseData;
    
    for (const auto& delta : deltaRecords) {
        if (delta.offset + delta.size > result.size()) {
            continue; // Invalid delta
        }
        
        // Decompress the delta
        std::vector<uint8_t> deltaData = compressor_.decompress(
            delta.data.data(), delta.data.size(), delta.compression);
        
        // Apply the delta
        for (size_t i = 0; i < delta.size; ++i) {
            result[delta.offset + i] = deltaData[i];
        }
    }
    
    return result;
}

uint64_t DifferentialCalculator::calculateChecksum(const void* data, size_t size) {
    return ChecksumCalculator::fletcher64(
        reinterpret_cast<const uint64_t*>(data), 
        size / sizeof(uint64_t));
}

// ChunkedCheckpointWriter implementation

ChunkedCheckpointWriter::ChunkedCheckpointWriter(const Config& config) : config_(config) {
}

ChunkedCheckpointWriter::~ChunkedCheckpointWriter() {
    // Clean up temp files
    if (!config_.tempDir.empty()) {
        // In a real implementation, this would clean up the temp directory
    }
}

bool ChunkedCheckpointWriter::writeChunks(const std::vector<uint8_t>& data,
                                         const std::string& basePath,
                                         const std::string& name) {
    if (data.empty()) return false;
    
    // Create output directory
    std::string outputDir = basePath + "/" + name;
    
    // Write data in chunks
    size_t offset = 0;
    size_t chunkId = 0;
    
    while (offset < data.size()) {
        size_t chunkSize = std::min(config_.chunkSize, data.size() - offset);
        
        ChunkInfo chunk;
        chunk.chunkId = chunkId;
        chunk.offset = offset;
        chunk.size = chunkSize;
        
        // Extract chunk data
        chunk.data.assign(data.begin() + offset, data.begin() + offset + chunkSize);
        
        // Compress chunk
        chunk.algorithm = CompressionAlgorithm::Zstd;
        chunk.compressedSize = chunk.data.size(); // Simplified
        
        // Store chunk
        // In a real implementation, this would write to disk or distributed storage
        
        offset += chunkSize;
        chunkId++;
    }
    
    return true;
}

std::vector<uint8_t> ChunkedCheckpointWriter::readChunks(const std::string& basePath,
                                                         const std::string& name,
                                                         uint64_t offset,
                                                         uint64_t size) {
    // In a real implementation, this would read from stored chunks
    return std::vector<uint8_t>();
}

std::vector<std::string> ChunkedCheckpointWriter::listCheckpoints(const std::string& basePath) const {
    // In a real implementation, this would list checkpoint directories
    return std::vector<std::string>();
}

void ChunkedCheckpointWriter::cleanup(const std::string& basePath, const std::string& name) {
    // In a real implementation, this would clean up checkpoint files
}

// DistributedCheckpointStorage implementation

DistributedCheckpointStorage::DistributedCheckpointStorage(const std::vector<NodeInfo>& nodes)
    : nodes_(nodes) {
}

DistributedCheckpointStorage::~DistributedCheckpointStorage() {
}

bool DistributedCheckpointStorage::store(const std::vector<uint8_t>& data,
                                        const std::string& name,
                                        const std::vector<std::string>& preferredNodes) {
    if (data.empty()) return false;
    
    // Select nodes for storage
    std::vector<std::string> targetNodes = preferredNodes.empty() ? 
        getAvailableNodes() : preferredNodes;
    
    if (targetNodes.empty()) {
        return false; // No available nodes
    }
    
    // Split data into chunks
    std::vector<ChunkInfo> chunks;
    size_t chunkSize = 1024 * 1024; // 1MB chunks
    
    for (size_t offset = 0; offset < data.size(); offset += chunkSize) {
        ChunkInfo chunk;
        chunk.chunkId = offset / chunkSize;
        chunk.offset = offset;
        chunk.size = std::min(chunkSize, data.size() - offset);
        chunk.data.assign(data.begin() + offset, 
                         data.begin() + offset + chunk.size);
        chunk.nodeId = targetNodes[chunk.chunkId % targetNodes.size()];
        chunk.replicationCount = 2;
        chunks.push_back(chunk);
    }
    
    // Store chunks
    for (const auto& chunk : chunks) {
        // In a real implementation, this would store to the specific node
    }
    
    // Update index
    std::lock_guard<std::mutex> lock(indexMutex_);
    if (checkpointIndex_.find(name) == checkpointIndex_.end()) {
        checkpointIndex_[name] = std::vector<ChunkInfo>();
    }
    checkpointIndex_[name] = chunks;
    
    return true;
}

std::optional<std::vector<uint8_t>> DistributedCheckpointStorage::retrieve(const std::string& name,
                                                                         const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(indexMutex_);
    
    auto it = checkpointIndex_.find(name);
    if (it == checkpointIndex_.end()) {
        return std::nullopt; // Checkpoint not found
    }
    
    const auto& chunks = it->second;
    if (chunks.empty()) {
        return std::nullopt;
    }
    
    // Reconstruct data from chunks
    std::vector<uint8_t> result;
    for (const auto& chunk : chunks) {
        if (nodeId.empty() || chunk.nodeId == nodeId) {
            result.insert(result.end(), chunk.data.begin(), chunk.data.end());
        }
    }
    
    return result;
}

std::vector<std::string> DistributedCheckpointStorage::getChunkLocations(const std::string& name,
                                                                        const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(indexMutex_);
    
    std::vector<std::string> locations;
    auto it = checkpointIndex_.find(name);
    if (it != checkpointIndex_.end()) {
        for (const auto& chunk : it->second) {
            if (nodeId.empty() || chunk.nodeId == nodeId) {
                locations.push_back(chunk.nodeId);
            }
        }
    }
    
    return locations;
}

std::vector<DistributedCheckpointStorage::NodeInfo> DistributedCheckpointStorage::monitorStorage() const {
    std::lock_guard<std::mutex> lock(nodesMutex_);
    return nodes_;
}

void DistributedCheckpointStorage::rebalance(const std::string& name) {
    // In a real implementation, this would rebalance data across nodes
}

std::vector<std::string> DistributedCheckpointStorage::getAvailableNodes() const {
    std::lock_guard<std::mutex> lock(nodesMutex_);
    std::vector<std::string> availableNodes;
    for (const auto& node : nodes_) {
        if (node.available) {
            availableNodes.push_back(node.nodeId);
        }
    }
    return availableNodes;
}

// CheckpointOptimizer implementation

CheckpointOptimizer::CheckpointOptimizer(const Config& config) 
    : config_(config)
    , lastIncrementalStep_(0)
    , lastDifferentialStep_(0) {
    
    // Initialize chunk writer
    ChunkedCheckpointWriter::Config chunkConfig;
    chunkConfig.chunkSize = config_.chunkSize * 1024 * 1024; // Convert MB to bytes
    chunkConfig.maxConcurrentWrites = config_.maxConcurrentOperations;
    chunkConfig.verifyWrites = true;
    chunkConfig.tempDir = config_.checkpointDir + "/tmp";
    
    chunkWriter_ = std::make_unique<ChunkedCheckpointWriter>(chunkConfig);
    
    // Initialize distributed storage if enabled
    if (config_.enableDistributed) {
        // In a real implementation, this would configure distributed storage
    }
}

CheckpointOptimizer::~CheckpointOptimizer() {
}

bool CheckpointOptimizer::createCheckpoint(const std::string& name, uint64_t sequence) {
    try {
        // Collect checkpoint data
        std::vector<uint8_t> checkpointData = collectCheckpointData();
        if (checkpointData.empty()) {
            return false;
        }
        
        // Determine checkpoint type
        CheckpointType checkpointType = config_.type;
        
        // For incremental checkpoints, check if needed
        if (config_.incrementalInterval > 0) {
            if (sequence % config_.incrementalInterval != 0) {
                checkpointType = CheckpointType::Incremental;
            }
        }
        
        // For differential checkpoints, check if needed
        if (config_.differentialInterval > 0) {
            if (sequence % config_.differentialInterval != 0) {
                checkpointType = CheckpointType::Differential;
            }
        }
        
        // Create metadata
        CheckpointMetadata metadata;
        metadata.version = CHECKPOINT_VERSION_MAJOR * 10000 + 
                          CHECKPOINT_VERSION_MINOR * 100 + 
                          CHECKPOINT_VERSION_PATCH;
        metadata.sequence = sequence;
        metadata.timestamp = std::time(nullptr);
        metadata.size = checkpointData.size();
        
        // Compress checkpoint data
        if (config_.enableCompression) {
            metadata.algorithm = "zstd";
            metadata.compressedSize = checkpointData.size(); // Simplified
            metadata.compressionRatio = 1.0;
        } else {
            metadata.algorithm = "none";
            metadata.compressedSize = checkpointData.size();
            metadata.compressionRatio = 1.0;
        }
        
        metadata.checksum = ChecksumCalculator::fletcher64(
            reinterpret_cast<const uint64_t*>(checkpointData.data()),
            checkpointData.size() / sizeof(uint64_t));
        
        metadata.parentSequence = sequence - 1; // Previous checkpoint
        metadata.description = "Performance-optimized checkpoint";
        metadata.tags = {"performance", "checkpoint"};
        
        // Write checkpoint
        std::string checkpointName = name.empty() ? 
            "checkpoint_" + std::to_string(sequence) : name;
        
        bool success = writeCheckpoint(checkpointData, checkpointName, metadata);
        if (success) {
            // Update metadata
            std::lock_guard<std::mutex> lock(metadataMutex_);
            checkpointMetadata_[checkpointName] = metadata;
            
            // Update last save step
            lastIncrementalStep_ = sequence;
            lastDifferentialStep_ = sequence;
            
            // Clean up old checkpoints
            if (config_.type == CheckpointType::Incremental) {
                pruneOldCheckpoints(10); // Keep last 10 checkpoints
            }
        }
        
        return success;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool CheckpointOptimizer::loadCheckpoint(const std::string& name) {
    // In a real implementation, this would load checkpoint data
    // and restore brain state
    return true;
}

CheckpointMetadata CheckpointOptimizer::getMetadata(const std::string& name) const {
    std::lock_guard<std::mutex> lock(metadataMutex_);
    auto it = checkpointMetadata_.find(name);
    if (it != checkpointMetadata_.end()) {
        return it->second;
    }
    
    // Return empty metadata
    CheckpointMetadata metadata;
    metadata.version = 0;
    return metadata;
}

std::vector<std::string> CheckpointOptimizer::listCheckpoints() const {
    std::lock_guard<std::mutex> lock(metadataMutex_);
    std::vector<std::string> names;
    for (const auto& pair : checkpointMetadata_) {
        names.push_back(pair.first);
    }
    return names;
}

bool CheckpointOptimizer::deleteCheckpoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(metadataMutex_);
    auto it = checkpointMetadata_.find(name);
    if (it != checkpointMetadata_.end()) {
        checkpointMetadata_.erase(it);
        // Also delete files from storage
        // In a real implementation, this would delete from disk
        return true;
    }
    return false;
}

void CheckpointOptimizer::pruneOldCheckpoints(size_t keepCount) {
    std::lock_guard<std::mutex> lock(metadataMutex_);
    
    if (checkpointMetadata_.size() <= keepCount) {
        return;
    }
    
    // Sort checkpoints by sequence number
    std::vector<std::pair<std::string, CheckpointMetadata>> sorted;
    for (const auto& pair : checkpointMetadata_) {
        sorted.push_back(pair);
    }
    
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) {
                  return a.second.sequence > b.second.sequence;
              });
    
    // Keep only the most recent checkpoints
    auto newEnd = sorted.begin() + keepCount;
    std::vector<std::string> toDelete;
    for (auto it = newEnd; it != sorted.end(); ++it) {
        toDelete.push_back(it->first);
    }
    
    for (const auto& name : toDelete) {
        checkpointMetadata_.erase(name);
        // Delete from storage
        // In a real implementation, this would delete from disk
    }
}

uint64_t CheckpointOptimizer::calculateStorageUsage(const std::string& basePath) const {
    // In a real implementation, this would calculate actual storage usage
    return 0;
}

bool CheckpointOptimizer::compactCheckpoints(const std::string& basePath,
                                             const std::string& outputName,
                                             double compactionThreshold) {
    // In a real implementation, this would compact checkpoints
    return true;
}

std::vector<uint8_t> CheckpointOptimizer::collectCheckpointData() const {
    // In a real implementation, this would collect all checkpoint data
    // from the brain, neurons, synapses, etc.
    return std::vector<uint8_t>();
}

bool CheckpointOptimizer::writeCheckpoint(const std::vector<uint8_t>& data,
                                          const std::string& name,
                                          const CheckpointMetadata& metadata) {
    // In a real implementation, this would write checkpoint data to storage
    return true;
}

bool CheckpointOptimizer::validateCheckpoint(const std::string& name) const {
    // In a real implementation, this would validate checkpoint integrity
    return true;
}

} // namespace nlm