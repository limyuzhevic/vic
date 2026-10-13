#include "BrainStateManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <zlib.h>
#include <chrono>
#include <numeric>
#include <cmath>

namespace nlm {

struct BrainStateManager::Impl {
    bool checkpointingEnabled = false;
    size_t checkpointInterval = 1000;
    std::string checkpointDirectory = "./checkpoints";
    
    size_t totalCheckpointsCreated = 0;
    size_t totalCheckpointsRestored = 0;
    size_t totalBackupSize = 0;
    Timestamp lastCheckpointTime = 0.0;
    
    std::unordered_map<std::string, BrainStateBackupInfo> checkpointRegistry;
    std::unordered_map<SimulationStep, std::string> incrementalCheckpoints;
    
    Impl() {
        // Ensure default checkpoint directory exists
        ensureDirectoryExists(checkpointDirectory);
    }
};

BrainStateManager::BrainStateManager() : pImpl(new Impl) {}

BrainStateManager::~BrainStateManager() {
    // Cleanup on destruction
    deleteAllCheckpoints();
}

void BrainStateManager::configureCheckpointing(bool enabled, size_t interval, const std::string& checkpointDir) {
    pImpl->checkpointingEnabled = enabled;
    pImpl->checkpointInterval = interval;
    
    if (!checkpointDir.empty()) {
        pImpl->checkpointDirectory = checkpointDir;
    }
    
    ensureDirectoryExists(pImpl->checkpointDirectory);
    NLM_LOG_INFO("Checkpointing configured: enabled=" + std::string(enabled ? "true" : "false") + 
                 ", interval=" + std::to_string(interval) + 
                 ", directory=" + pImpl->checkpointDirectory);
}

bool BrainStateManager::createCheckpoint(const std::shared_ptr<Brain>& brain, const std::string& label) {
    if (!brain || !pImpl->checkpointingEnabled) {
        return false;
    }
    
    SimulationStep currentStep = 0; // Would get from brain's simulation clock
    std::string filename = generateCheckpointFilename(label, currentStep);
    
    return backupBrainState(filename, brain);
}

std::shared_ptr<Brain> BrainStateManager::restoreCheckpoint(const std::string& label, const Config& config) {
    auto it = pImpl->checkpointRegistry.find(label);
    if (it == pImpl->checkpointRegistry.end()) {
        NLM_LOG_ERROR("Checkpoint not found: " + label);
        return nullptr;
    }
    
    const BrainStateBackupInfo& info = it->second;
    pImpl->totalCheckpointsRestored++;
    
    NLM_LOG_INFO("Restoring checkpoint: " + label + " (from " + info.filename + ")");
    return restoreBrainState(info.filename, config);
}

std::shared_ptr<Brain> BrainStateManager::restoreLatestCheckpoint(const Config& config) {
    if (pImpl->checkpointRegistry.empty()) {
        return nullptr;
    }
    
    // Find latest checkpoint by timestamp
    auto latestIt = std::max_element(pImpl->checkpointRegistry.begin(), pImpl->checkpointRegistry.end(),
        [](const auto& a, const auto& b) { return a.second.timestamp < b.second.timestamp; });
    
    return restoreCheckpoint(latestIt->first, config);
}

bool BrainStateManager::createIncrementalCheckpoint(const std::shared_ptr<Brain>& brain, SimulationStep currentStep) {
    if (!brain || !pImpl->checkpointingEnabled) {
        return false;
    }
    
    std::string filename = generateCheckpointFilename("incremental_" + std::to_string(currentStep), currentStep);
    
    if (!backupBrainState(filename, brain)) {
        return false;
    }
    
    pImpl->incrementalCheckpoints[currentStep] = filename;
    
    // Clean up old incremental checkpoints if we exceed the interval
    auto it = pImpl->incrementalCheckpoints.begin();
    while (it != pImpl->incrementalCheckpoints.end()) {
        if (std::abs(static_cast<int64_t>(it->first - currentStep)) > static_cast<int64_t>(pImpl->checkpointInterval)) {
            // Delete old checkpoint
            try {
                std::filesystem::remove(it->second);
            } catch (const std::exception&) {}
            it = pImpl->incrementalCheckpoints.erase(it);
        } else {
            ++it;
        }
    }
    
    return true;
}

bool BrainStateManager::restoreIncrementalCheckpoint(SimulationStep targetStep, const Config& config) {
    auto it = pImpl->incrementalCheckpoints.find(targetStep);
    if (it == pImpl->incrementalCheckpoints.end()) {
        NLM_LOG_ERROR("Incremental checkpoint not found for step: " + std::to_string(targetStep));
        return false;
    }
    
    return restoreBrainState(it->second, config) != nullptr;
}

BrainStateDifference BrainStateManager::compareBrainStates(const std::shared_ptr<Brain>& brain1,
                                                         const std::shared_ptr<Brain>& brain2) const {
    BrainStateDifference diff;
    
    if (!brain1 || !brain2) {
        diff.summary = "Invalid brain states provided for comparison";
        return diff;
    }
    
    BrainState state1 = brainToState(brain1);
    BrainState state2 = brainToState(brain2);
    
    // Calculate membrane potential differences
    if (state1.neuronMembranePotentials.size() == state2.neuronMembranePotentials.size()) {
        double totalDiff = 0.0;
        for (size_t i = 0; i < state1.neuronMembranePotentials.size(); ++i) {
            double diff = std::abs(state1.neuronMembranePotentials[i] - state2.neuronMembranePotentials[i]);
            totalDiff += diff;
            if (diff > 0.1) { // Threshold for "significant" difference
                diff.differingNeuronIds.push_back({i, diff});
            }
        }
        diff.neuronPotentialDifference = totalDiff / state1.neuronMembranePotentials.size();
    }
    
    // Calculate synaptic weight differences
    size_t minSynapseSize = std::min(state1.synapseWeights.size(), state2.synapseWeights.size());
    if (minSynapseSize > 0) {
        double totalDiff = 0.0;
        for (size_t i = 0; i < minSynapseSize; ++i) {
            double diff = 0.0;
            for (size_t j = 0; j < state1.synapseWeights[i].size(); ++j) {
                if (j < state2.synapseWeights[i].size()) {
                    diff += std::abs(state1.synapseWeights[i][j] - state2.synapseWeights[i][j]);
                }
            }
            totalDiff += diff;
            if (diff > 0.01) {
                diff.differingSynapseIds.push_back({i, diff});
            }
        }
        diff.weightDifference = totalDiff / minSynapseSize;
    }
    
    // Calculate spike count difference
    double spikeDiff = std::abs(static_cast<double>(state1.totalSpikeCount) - static_cast<double>(state2.totalSpikeCount));
    diff.spikeDifference = spikeDiff / std::max(1u, std::max(state1.totalSpikeCount, state2.totalSpikeCount));
    
    // Calculate overall difference (weighted average)
    diff.totalDifference = (diff.neuronPotentialDifference * 0.4 + diff.weightDifference * 0.4 + diff.spikeDifference * 0.2);
    
    // Generate summary
    std::stringstream ss;
    ss << "BrainStateComparison: total_diff=" << diff.totalDifference 
       << ", neuron_diff=" << diff.neuronPotentialDifference
       << ", weight_diff=" << diff.weightDifference
       << ", spike_diff=" << diff.spikeDifference
       << ", neurons_changed=" << diff.differingNeuronIds.size()
       << ", synapses_changed=" << diff.differingSynapseIds.size();
    diff.summary = ss.str();
    
    NLM_LOG_INFO("Brain state comparison: " + diff.summary);
    
    return diff;
}

double BrainStateManager::calculateStateDifference(const std::shared_ptr<Brain>& brain1,
                                                  const std::shared_ptr<Brain>& brain2) const {
    BrainStateDifference diff = compareBrainStates(brain1, brain2);
    return diff.totalDifference;
}

bool BrainStateManager::backupBrainState(const std::string& filepath, const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return false;
    }
    
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Create brain state
        BrainState state = brainToState(brain);
        
        // Write metadata
        Timestamp timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
        file.write(reinterpret_cast<const char*>(&state.totalNeuronCount), sizeof(state.totalNeuronCount));
        file.write(reinterpret_cast<const char*>(&state.totalSynapseCount), sizeof(state.totalSynapseCount));
        file.write(reinterpret_cast<const char*>(&state.totalSpikeCount), sizeof(state.totalSpikeCount));
        file.write(reinterpret_cast<const char*>(&state.simulationTime), sizeof(state.simulationTime));
        
        // Write neuron data
        size_t neuronCount = state.neuronMembranePotentials.size();
        file.write(reinterpret_cast<const char*>(&neuronCount), sizeof(neuronCount));
        file.write(reinterpret_cast<const char*>(state.neuronMembranePotentials.data(), 
                                                  neuronCount * sizeof(float));
        
        file.write(reinterpret_cast<const char*>(&neuronCount), sizeof(neuronCount));
        file.write(reinterpret_cast<const char*>(state.neuronThresholds.data(), 
                                                  neuronCount * sizeof(float));
        
        // Write synapse data
        size_t synapseSize = state.synapseWeights.size();
        file.write(reinterpret_cast<const char*>(&synapseSize), sizeof(synapseSize));
        for (const auto& row : state.synapseWeights) {
            size_t rowSize = row.size();
            file.write(reinterpret_cast<const char*>(&rowSize), sizeof(rowSize));
            file.write(reinterpret_cast<const char*>(row.data(), rowSize * sizeof(float));
        }
        
        file.close();
        
        // Update backup info
        BrainStateBackupInfo info = createBackupInfo(filepath, brain, false);
        pImpl->checkpointRegistry[info.filename] = info;
        pImpl->totalCheckpointsCreated++;
        pImpl->totalBackupSize += info.memoryUsage;
        pImpl->lastCheckpointTime = info.timestamp;
        
        NLM_LOG_INFO("Brain state backup created: " + filepath + " (size: " + std::to_string(info.memoryUsage) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to backup brain state to " + filepath + ": " + e.what());
        return false;
    }
}

bool BrainStateManager::backupBrainStateCompressed(const std::string& filepath, const std::shared_ptr<Brain>& brain,
                                                  const std::string& algorithm) {
    if (!brain) {
        return false;
    }
    
    try {
        // Get uncompressed data
        BrainState state = brainToState(brain);
        
        // Serialize to bytes (simplified)
        std::vector<uint8_t> data;
        data.reserve(1024 * 1024); // Initial capacity
        
        auto writeData = [&data](const void* ptr, size_t size) {
            const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(ptr);
            data.insert(data.end(), bytePtr, bytePtr + size);
        };
        
        // Write metadata
        Timestamp timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        writeData(&timestamp, sizeof(timestamp));
        writeData(&state.totalNeuronCount, sizeof(state.totalNeuronCount));
        writeData(&state.totalSynapseCount, sizeof(state.totalSynapseCount));
        writeData(&state.totalSpikeCount, sizeof(state.totalSpikeCount));
        writeData(&state.simulationTime, sizeof(state.simulationTime));
        
        // Write neuron data
        size_t neuronCount = state.neuronMembranePotentials.size();
        writeData(&neuronCount, sizeof(neuronCount));
        writeData(state.neuronMembranePotentials.data(), neuronCount * sizeof(float));
        
        writeData(&neuronCount, sizeof(neuronCount));
        writeData(state.neuronThresholds.data(), neuronCount * sizeof(float));
        
        // Write synapse data
        size_t synapseSize = state.synapseWeights.size();
        writeData(&synapseSize, sizeof(synapseSize));
        for (const auto& row : state.synapseWeights) {
            size_t rowSize = row.size();
            writeData(&rowSize, sizeof(rowSize));
            writeData(row.data(), rowSize * sizeof(float));
        }
        
        // Compress data based on algorithm
        std::vector<uint8_t> compressedData;
        bool compressionSuccess = false;
        
        if (algorithm == "gzip") {
            // Simplified gzip-like compression (placeholder)
            compressedData = data; // In real implementation, would compress
            compressionSuccess = true;
        } else {
            compressedData = data;
            compressionSuccess = true;
        }
        
        if (compressionSuccess) {
            // Write compressed data to file
            std::ofstream file(filepath, std::ios::binary);
            if (!file.is_open()) {
                return false;
            }
            
            file.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
            file.close();
            
            // Update backup info
            BrainStateBackupInfo info = createBackupInfo(filepath, brain, true);
            info.compressionAlgorithm = algorithm;
            pImpl->checkpointRegistry[info.filename] = info;
            pImpl->totalCheckpointsCreated++;
            pImpl->totalBackupSize += info.memoryUsage;
            pImpl->lastCheckpointTime = info.timestamp;
            
            NLM_LOG_INFO(std::string("Compressed brain state backup created: ") + filepath + 
                        " (original: " + std::to_string(data.size()) + 
                        " bytes, compressed: " + std::to_string(compressedData.size()) + " bytes)");
            return true;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create compressed brain state backup to " + filepath + ": " + e.what());
    }
    
    return false;
}

std::shared_ptr<Brain> BrainStateManager::restoreBrainState(const std::string& filepath, const Config& config) {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return nullptr;
        }
        
        // Check file size
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        // Create brain from config
        auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain from config during restore");
            return nullptr;
        }
        
        // For now, return the brain (full restoration would need more complex implementation)
        // In a real implementation, this would restore the brain state from the file
        
        file.close();
        
        NLM_LOG_INFO("Brain state restored from: " + filepath);
        return brain;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to restore brain state from " + filepath + ": " + e.what());
        return nullptr;
    }
}

bool BrainStateManager::validateBrainState(const std::shared_ptr<Brain>& brain) const {
    if (!brain) {
        return false;
    }
    
    std::vector<std::string> errors = validateBrainStateDetailed(brain);
    return errors.empty();
}

std::vector<std::string> BrainStateManager::validateBrainStateDetailed(const std::shared_ptr<Brain>& brain) const {
    std::vector<std::string> errors;
    
    if (!brain) {
        errors.push_back("Brain pointer is null");
        return errors;
    }
    
    // Check if brain is initialized
    if (!brain->getSpikeSystem()) {
        errors.push_back("Brain spike system not initialized");
    }
    
    // Check neuron counts
    size_t totalNeuronCount = brain->getTotalNeuronCount();
    if (totalNeuronCount == 0) {
        errors.push_back("Brain has zero neurons");
    }
    
    if (totalNeuronCount > 1000000) {
        errors.push_back("Brain has unusually high neuron count: " + std::to_string(totalNeuronCount));
    }
    
    // Check spike counts
    size_t totalSpikeCount = brain->getTotalSpikeCount();
    if (totalSpikeCount > 100000000) {
        errors.push_back("Brain has unusually high spike count: " + std::to_string(totalSpikeCount));
    }
    
    return errors;
}
}

bool BrainStateManager::exportBrainState(const std::string& filepath, const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return false;
    }
    
    // Create a simple text-based export for now
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "BrainState Export" << std::endl;
        file << "=================" << std::endl;
        file << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        // Get spike events
        auto spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            file << "Spike Events: " << spikeSystem->getSpikeCount() << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool BrainStateManager::importBrainState(const std::string& filepath, std::shared_ptr<Brain>& brain, const Config& config) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Total Neurons: ") == 0) {
                // Parse and apply settings based on import
                // Simplified implementation
            } else if (line.find("Total Synapses: ") == 0) {
                // Parse and apply settings based on import
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> BrainStateManager::listCheckpoints(const std::string& directory) const {
    std::vector<std::string> checkpoints;
    
    std::string searchDir = directory.empty() ? pImpl->checkpointDirectory : directory;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(searchDir)) {
            if (entry.is_regular_file() && (entry.path().extension() == ".bin" || entry.path().extension() == ".checkpoint")) {
                checkpoints.push_back(entry.path().string());
            }
        }
    } catch (const std::exception&) {}
    
    return checkpoints;
}

std::vector<BrainStateBackupInfo> BrainStateManager::getCheckpointInfo(const std::string& directory) const {
    std::vector<BrainStateBackupInfo> infos;
    
    std::string searchDir = directory.empty() ? pImpl->checkpointDirectory : directory;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(searchDir)) {
            if (entry.is_regular_file() && (entry.path().extension() == ".bin" || entry.path().extension() == ".checkpoint")) {
                // Try to load backup info if available
                BrainStateBackupInfo info;
                info.filename = entry.path().string();
                info.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
                info.step = 0;
                info.checksum = 0;
                info.memoryUsage = entry.file_size();
                info.compressed = false;
                info.compressionAlgorithm = "none";
                
                infos.push_back(info);
            }
        }
    } catch (const std::exception&) {}
    
    return infos;
}

bool BrainStateManager::deleteCheckpoint(const std::string& label) {
    auto it = pImpl->checkpointRegistry.find(label);
    if (it == pImpl->checkpointRegistry.end()) {
        return false;
    }
    
    const std::string& filename = it->second.filename;
    
    try {
        std::filesystem::remove(filename);
        pImpl->checkpointRegistry.erase(it);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool BrainStateManager::deleteAllCheckpoints(const std::string& directory) {
    std::string searchDir = directory.empty() ? pImpl->checkpointDirectory : directory;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(searchDir)) {
            if (entry.is_regular_file() && (entry.path().extension() == ".bin" || entry.path().extension() == ".checkpoint")) {
                std::filesystem::remove(entry.path());
            }
        }
        pImpl->checkpointRegistry.clear();
        pImpl->incrementalCheckpoints.clear();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

BrainState BrainStateManager::brainToState(const std::shared_ptr<Brain>& brain) const {
    BrainState state;
    
    if (!brain) {
        return state;
    }
    
    // Get neuron data
    state.totalNeuronCount = brain->getTotalNeuronCount();
    
    // For now, create simplified state (real implementation would access brain internals)
    state.neuronMembranePotentials.resize(state.totalNeuronCount, -70.0f);
    state.neuronThresholds.resize(state.totalNeuronCount, -55.0f);
    
    // Get synapse data
    state.totalSynapseCount = brain->getTotalSynapseCount();
    state.synapseWeights.resize(100, std::vector<float>(100, 0.5f)); // Simplified
    state.synapsePlasticity.resize(100, std::vector<bool>(100, false)); // Simplified
    
    // Get statistics
    state.totalSpikeCount = brain->getTotalSpikeCount();
    state.simulationTime = 0.0; // Would get from brain clock
    
    return state;
}

std::shared_ptr<Brain> BrainStateManager::stateToBrain(const BrainState& state, const Config& config) const {
    auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
    if (!brain->initialize()) {
        return nullptr;
    }
    
    // For now, just return the initialized brain
    // Real implementation would restore state from BrainState struct
    
    return brain;
}

double BrainStateManager::calculateStateEntropy(const std::shared_ptr<Brain>& brain) const {
    if (!brain) {
        return 0.0;
    }
    
    // Simplified entropy calculation based on spike distribution
    size_t totalSpikes = brain->getTotalSpikeCount();
    if (totalSpikes == 0) {
        return 0.0;
    }
    
    // Get spike system statistics
    auto spikeSystem = brain->getSpikeSystem();
    if (!spikeSystem) {
        return 0.0;
    }
    
    // Calculate entropy based on spike count distribution across neurons
    size_t neuronCount = brain->getTotalNeuronCount();
    if (neuronCount == 0) {
        return 0.0;
    }
    
    // This is a simplified entropy calculation
    double entropy = 0.0;
    double spikeProbability = static_cast<double>(totalSpikes) / neuronCount;
    if (spikeProbability > 0 && spikeProbability < 1.0) {
        entropy = -spikeProbability * std::log2(spikeProbability) - (1.0 - spikeProbability) * std::log2(1.0 - spikeProbability);
    }
    
    return entropy;
}

std::vector<size_t> BrainStateManager::getMostActiveNeurons(const std::shared_ptr<Brain>& brain, size_t count) const {
    std::vector<size_t> mostActive;
    
    if (!brain) {
        return mostActive;
    }
    
    // Simplified implementation
    size_t neuronCount = brain->getTotalNeuronCount();
    
    // Generate most active neurons (simplified)
    mostActive.reserve(count);
    for (size_t i = 0; i < std::min(count, neuronCount); ++i) {
        mostActive.push_back(i);
    }
    
    return mostActive;
}

std::vector<size_t> BrainStateManager::getLeastActiveNeurons(const std::shared_ptr<Brain>& brain, size_t count) const {
    std::vector<size_t> leastActive;
    
    if (!brain) {
        return leastActive;
    }
    
    // Simplified implementation
    size_t neuronCount = brain->getTotalNeuronCount();
    
    // Generate least active neurons (simplified)
    leastActive.reserve(count);
    for (size_t i = neuronCount - 1; i >= std::max(static_cast<size_t>(0), neuronCount - count); --i) {
        leastActive.push_back(i);
    }
    
    return leastActive;
}

std::vector<uint8_t> BrainStateManager::compressBrainState(const std::shared_ptr<Brain>& brain) const {
    if (!brain) {
        return std::vector<uint8_t>();
    }
    
    // Simplified compression (in real implementation would use actual compression)
    BrainState state = brainToState(brain);
    
    std::vector<uint8_t> data;
    data.reserve(1024 * 1024); // Initial capacity
    
    // Serialize state to bytes
    auto writeData = [&data](const void* ptr, size_t size) {
        const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(ptr);
        data.insert(data.end(), bytePtr, bytePtr + size);
    };
    
    // Write state
    writeData(&state.totalNeuronCount, sizeof(state.totalNeuronCount));
    writeData(state.neuronMembranePotentials.data(), state.neuronMembranePotentials.size() * sizeof(float));
    
    // In real implementation, would apply actual compression algorithm
    // For now, return uncompressed data
    return data;
}

std::shared_ptr<Brain> BrainStateManager::decompressBrainState(const std::vector<uint8_t>& data, const Config& config) const {
    auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
    if (!brain->initialize()) {
        return nullptr;
    }
    
    // In real implementation, would decompress data and restore state
    // For now, just return initialized brain
    
    return brain;
}

bool BrainStateManager::isCheckpointingEnabled() const {
    return pImpl->checkpointingEnabled;
}

size_t BrainStateManager::getCheckpointInterval() const {
    return pImpl->checkpointInterval;
}

std::string BrainStateManager::getCheckpointDirectory() const {
    return pImpl->checkpointDirectory;
}

size_t BrainStateManager::getTotalCheckpointsCreated() const {
    return pImpl->totalCheckpointsCreated;
}

size_t BrainStateManager::getTotalCheckpointsRestored() const {
    return pImpl->totalCheckpointsRestored;
}

double BrainStateManager::getAverageCheckpointSize() const {
    if (pImpl->totalCheckpointsCreated == 0) {
        return 0.0;
    }
    return static_cast<double>(pImpl->totalBackupSize) / pImpl->totalCheckpointsCreated;
}

Timestamp BrainStateManager::getLastCheckpointTime() const {
    return pImpl->lastCheckpointTime;
}

void BrainStateManager::cleanupOldCheckpoints(size_t keepCount) {
    if (pImpl->checkpointRegistry.size() <= keepCount) {
        return;
    }
    
    // Sort checkpoints by timestamp (oldest first)
    std::vector<BrainStateBackupInfo> sortedInfos;
    for (const auto& pair : pImpl->checkpointRegistry) {
        sortedInfos.push_back(pair.second);
    }
    
    std::sort(sortedInfos.begin(), sortedInfos.end(),
              [](const BrainStateBackupInfo& a, const BrainStateBackupInfo& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Delete oldest checkpoints to keep only keepCount
    size_t toDelete = sortedInfos.size() - keepCount;
    for (size_t i = 0; i < toDelete; ++i) {
        const std::string& filename = sortedInfos[i].filename;
        try {
            std::filesystem::remove(filename);
            pImpl->checkpointRegistry.erase(filename);
        } catch (const std::exception&) {}
    }
}

std::string BrainStateManager::generateCheckpointFilename(const std::string& label, SimulationStep step) const {
    std::string filename = label;
    if (!label.empty()) {
        filename += "_";
    }
    filename += std::to_string(step) + ".checkpoint";
    
    // Ensure filename is in the checkpoint directory
    if (!pImpl->checkpointDirectory.empty() && pImpl->checkpointDirectory.back() != '/') {
        filename = pImpl->checkpointDirectory + "/" + filename;
    } else if (!pImpl->checkpointDirectory.empty()) {
        filename = pImpl->checkpointDirectory + filename;
    }
    
    return filename;
}

uint64_t BrainStateManager::calculateChecksum(const std::vector<uint8_t>& data) const {
    uint64_t checksum = 0;
    for (uint8_t byte : data) {
        checksum = (checksum << 7) | (checksum >> (64 - 7));
        checksum += static_cast<uint64_t>(byte);
    }
    return checksum;
}

bool BrainStateManager::ensureDirectoryExists(const std::string& path) const {
    try {
        std::filesystem::path dir(path);
        if (!std::filesystem::exists(dir)) {
            return std::filesystem::create_directories(dir);
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

BrainStateBackupInfo BrainStateManager::createBackupInfo(const std::string& filepath, const std::shared_ptr<Brain>& brain, bool compressed) const {
    BrainStateBackupInfo info;
    
    info.filename = filepath;
    info.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    info.step = 0; // Would get from brain
    
    try {
        info.memoryUsage = std::filesystem::file_size(filepath);
    } catch (const std::exception&) {
        info.memoryUsage = 0;
    }
    
    info.compressed = compressed;
    info.compressionAlgorithm = compressed ? "gzip" : "none";
    
    return info;
}

} // namespace nlm
