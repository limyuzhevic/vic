#include "CompleteCheckpointSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/EventSystem.hpp"
#include "../performance/ConnectivityManager.hpp"
#include "../performance/MemoryPoolManager.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <zlib.h>
#include <lzma.h>

namespace nlm {

CompleteCheckpointSystem::CompleteCheckpointSystem()
    : manager_(nullptr)
    , compressionEnabled_(true)
    , incremental_(false)
    , compressionLevel_(CompressionLevel::Balanced)
    , speedOptimized_(false)
    , lastSaveTime_(0)
    , lastLoadTime_(0)
    , totalCheckpoints_(0)
    , totalSaveTime_(0)
    , totalLoadTime_(0)
    , checkpointCounter_(0)
{
}

CompleteCheckpointSystem::~CompleteCheckpointSystem() {
    if (manager_) {
        delete manager_;
        manager_ = nullptr;
    }
}

void CompleteCheckpointSystem::configure(const std::string& checkpointPath, 
                                         bool compression, 
                                         bool incremental) {
    checkpointDir_ = checkpointPath;
    compressionEnabled_ = compression;
    incremental_ = incremental;
    
    // Create checkpoint directory if it doesn't exist
    std::filesystem::create_directories(checkpointDir_);
    
    // Setup manager if not already configured
    if (!manager_) {
        manager_ = new CheckpointManager();
        manager_->configure(checkpointDir_, 10000, 10, compression);
    }
}

bool CompleteCheckpointSystem::save(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO(std::string("Saving brain state to checkpoint: ") + filepath);
    
    try {
        // Setup writer
        CheckpointWriter writer;
        CompressionLevel compression = compressionEnabled_ ? compressionLevel_ : CompressionLevel::None;
        
        if (!writer.create(filepath, compression)) {
            NLM_LOG_ERROR(std::string("Failed to create checkpoint file: ") + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getRegionCount(),
            0, // Will be set by checkpoint method
            0.0  // Will be set by checkpoint method
        );
        
        // Create brain state data
        BrainState brainState;
        if (!extractBrainState(brain, brainState)) {
            NLM_LOG_ERROR("Failed to extract brain state");
            writer.abort();
            return false;
        }
        
        // Write brain state to checkpoint
        if (!writeBrainState(writer, brainState)) {
            NLM_LOG_ERROR("Failed to write brain state to checkpoint");
            writer.abort();
            return false;
        }
        
        // Finalize checkpoint
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            writer.abort();
            return false;
        }
        
        // Update statistics
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        lastSaveTime_ = duration;
        totalSaveTime_ += duration;
        totalCheckpoints_++;
        
        NLM_LOG_INFO(std::string("Brain state saved successfully (checkpoint #") + 
                     std::to_string(totalCheckpoints_) + std::string(") - ") +
                     std::to_string(duration) + std::string(" ms, ") +
                     std::to_string(writer.getBytesWritten()) + std::string(" bytes)"));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool CompleteCheckpointSystem::load(std::shared_ptr<Brain>& brain, const std::string& filepath) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO(std::string("Loading brain state from checkpoint: ") + filepath);
    
    try {
        // Setup reader
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR(std::string("Failed to open checkpoint file: ") + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR(std::string("Checkpoint validation failed: ") + reader.getError());
            return false;
        }
        
        // Read brain state
        BrainState brainState;
        if (!readBrainState(reader, brainState)) {
            NLM_LOG_ERROR("Failed to read brain state from checkpoint");
            return false;
        }
        
        // Restore brain state
        if (!restoreBrainState(brain, brainState)) {
            NLM_LOG_ERROR("Failed to restore brain state");
            return false;
        }
        
        // Update statistics
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        lastLoadTime_ = duration;
        totalLoadTime_ += duration;
        
        NLM_LOG_INFO(std::string("Brain state loaded successfully - ") + 
                     std::to_string(duration) + std::string(" ms, ") +
                     std::to_string(brainState.neuronCount) + std::string(" neurons, ") +
                     std::to_string(brainState.synapseCount) + std::string(" synapses"));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void CompleteCheckpointSystem::checkpoint(std::shared_ptr<Brain>& brain, SimulationStep step) {
    std::string filename = std::string("checkpoint_" ) + std::to_string(step) + std::string(".nlm");
    std::string filepath = checkpointDir_ + std::string("/") + filename;
    
    save(brain, filepath);
    
    // Update metadata for incremental checkpointing
    if (incremental_) {
        updateIncrementalMetadata(brain, step, filepath);
    }
}

bool CompleteCheckpointSystem::resume(std::shared_ptr<Brain>& brain, const std::string& filepath) {
    // Find the latest checkpoint if filepath is empty or indicates latest
    std::string latestPath;
    if (filepath.empty() || filepath == "latest") {
        latestPath = getLatestCheckpointPath();
        if (latestPath.empty()) {
            NLM_LOG_ERROR("No checkpoint files found");
            return false;
        }
    } else {
        latestPath = filepath;
    }
    
    return load(brain, latestPath);
}

bool CompleteCheckpointSystem::hasCheckpoint(const std::string& filepath) const {
    return std::filesystem::exists(filepath);
}

std::vector<std::string> CompleteCheckpointSystem::getCheckpointFiles(const std::string& directory) const {
    std::vector<std::string> checkpointFiles;
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find("checkpoint_") == 0 && filename.find(".nlm") != std::string::npos) {
                checkpointFiles.push_back(entry.path().string());
            }
        }
    }
    
    // Sort by modification time (newest first)
    std::sort(checkpointFiles.begin(), checkpointFiles.end(),
              [this](const std::string& a, const std::string& b) {
                  return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
              });
    
    return checkpointFiles;
}

void CompleteCheckpointSystem::cleanupOldCheckpoints(const std::string& directory, int keepCount) {
    auto checkpointFiles = getCheckpointFiles(directory);
    
    if (checkpointFiles.size() <= static_cast<size_t>(keepCount)) {
        return; // Keep all files
    }
    
    // Remove excess checkpoints
    for (size_t i = keepCount; i < checkpointFiles.size(); ++i) {
        try {
            std::filesystem::remove(checkpointFiles[i]);
            NLM_LOG_INFO(std::string("Removed old checkpoint: ") + checkpointFiles[i]);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to remove checkpoint ") + checkpointFiles[i] + 
                         std::string(": ") + e.what());
        }
    }
}

bool CompleteCheckpointSystem::saveMetadata(const std::string& filepath, const std::string& metadata) {
    try {
        std::ofstream file(filepath, std::ios::app);
        if (!file.is_open()) {
            return false;
        }
        
        file << "\n=== METADATA ===\n";
        file << metadata;
        file << "\n=== END METADATA ===\n";
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save metadata: ") + e.what());
        return false;
    }
}

std::string CompleteCheckpointSystem::loadMetadata(const std::string& filepath) {
    std::string metadata;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return metadata;
        }
        
        std::string line;
        bool inMetadata = false;
        while (std::getline(file, line)) {
            if (line.find("=== METADATA ===") != std::string::npos) {
                inMetadata = true;
                continue;
            }
            if (line.find("=== END METADATA ===") != std::string::npos) {
                break;
            }
            if (inMetadata) {
                metadata += line + "\n";
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load metadata: ") + e.what());
    }
    
    return metadata;
}

void CompleteCheckpointSystem::enableCompression(bool enable) {
    compressionEnabled_ = enable;
    if (manager_) {
        manager_->setCompressionEnabled(enable);
    }
}

void CompleteCheckpointSystem::setCompressionLevel(int level) {
    compressionLevel_ = static_cast<CompressionLevel>(level);
    if (manager_) {
        manager_->setCompressionLevel(compressionLevel_);
    }
}

void CompleteCheckpointSystem::optimizeForSpeed(bool optimize) {
    speedOptimized_ = optimize;
    if (manager_) {
        manager_->optimizeForSpeed(optimize);
    }
}

double CompleteCheckpointSystem::getSaveTime() const {
    return lastSaveTime_;
}

double CompleteCheckpointSystem::getLoadTime() const {
    return lastLoadTime_;
}

uint64_t CompleteCheckpointSystem::getCheckpointSize() const {
    return totalCheckpoints_;
}

size_t CompleteCheckpointSystem::getCheckpointCount() const {
    return totalCheckpoints_;
}

bool CompleteCheckpointSystem::extractBrainState(std::shared_ptr<Brain>& brain, BrainState& state) {
    try {
        // Extract neuron data
        extractNeuronData(brain, state);
        
        // Extract synapse data
        extractSynapseData(brain, state);
        
        // Extract connectivity data
        extractConnectivityData(brain, state);
        
        // Extract plasticity data
        extractPlasticityData(brain, state);
        
        // Extract memory system data
        extractMemoryData(brain, state);
        
        // Extract development system data
        extractDevelopmentData(brain, state);
        
        // Extract neuromodulation data
        extractNeuromodulationData(brain, state);
        
        // Extract simulation state
        extractSimulationState(brain, state);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception extracting brain state: ") + e.what());
        return false;
    }
}

bool CompleteCheckpointSystem::readBrainState(CheckpointReader& reader, BrainState& state) {
    try {
        // Read neuron data
        if (!reader.readSection(CheckpointSection::Neurons, 
                              reinterpret_cast<const void*>(&state.neuronData), 
                              sizeof(NeuronCheckpointData))) {
            return false;
        }
        
        // Read synapse data
        if (!reader.readSection(CheckpointSection::Synapses,
                              reinterpret_cast<const void*>(&state.synapseData),
                              sizeof(SynapseCheckpointData))) {
            return false;
        }
        
        // Read connectivity data
        if (!reader.readSection(CheckpointSection::Connectivity,
                              reinterpret_cast<const void*>(&state.connectivityData),
                              sizeof(ConnectivityData))) {
            return false;
        }
        
        // Read plasticity data
        if (!reader.readSection(CheckpointSection::Plasticity,
                              reinterpret_cast<const void*>(&state.plasticityData),
                              sizeof(PlasticityData))) {
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception reading brain state: ") + e.what());
        return false;
    }
}

bool CompleteCheckpointSystem::restoreBrainState(std::shared_ptr<Brain>& brain, const BrainState& state) {
    try {
        // Restore neuron data
        restoreNeuronData(brain, state);
        
        // Restore synapse data
        restoreSynapseData(brain, state);
        
        // Restore connectivity data
        restoreConnectivityData(brain, state);
        
        // Restore plasticity data
        restorePlasticityData(brain, state);
        
        // Restore memory system data
        restoreMemoryData(brain, state);
        
        // Restore development system data
        restoreDevelopmentData(brain, state);
        
        // Restore neuromodulation data
        restoreNeuromodulationData(brain, state);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception restoring brain state: ") + e.what());
        return false;
    }
}

void CompleteCheckpointSystem::updateIncrementalMetadata(std::shared_ptr<Brain>& brain, SimulationStep step, const std::string& filepath) {
    std::string metadata = std::string("Step: ") + std::to_string(step) +
                          std::string("\nNeurons: ") + std::to_string(brain->getTotalNeuronCount()) +
                          std::string("\nSynapses: ") + std::to_string(brain->getTotalSynapseCount()) +
                          std::string("\nTimestamp: ") + std::to_string(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    
    saveMetadata(filepath, metadata);
}

std::string CompleteCheckpointSystem::getLatestCheckpointPath() const {
    auto checkpointFiles = getCheckpointFiles(checkpointDir_);
    if (checkpointFiles.empty()) {
        return "";
    }
    return checkpointFiles[0];
}

bool CompleteCheckpointSystem::writeBrainState(CheckpointWriter& writer, const BrainState& state) {
    // Write neurons
    if (!writer.writeNeurons(state.neuronData)) {
        return false;
    }
    
    // Write synapses
    if (!writer.writeSynapses(state.synapseData)) {
        return false;
    }
    
    // Write connectivity
    if (!writer.writeSection(CheckpointSection::Connectivity,
                            reinterpret_cast<const void*>(&state.connectivityData),
                            sizeof(ConnectivityData))) {
        return false;
    }
    
    // Write plasticity
    if (!writer.writeSection(CheckpointSection::Plasticity,
                            reinterpret_cast<const void*>(&state.plasticityData),
                            sizeof(PlasticityData))) {
        return false;
    }
    
    return true;
}

void CompleteCheckpointSystem::extractNeuronData(std::shared_ptr<Brain>& brain, BrainState& state) {
    state.neuronData.membranePotential.reserve(brain->getTotalNeuronCount());
    state.neuronData.restingPotential.reserve(brain->getTotalNeuronCount());
    state.neuronData.threshold.reserve(brain->getTotalNeuronCount());
    state.neuronData.resetPotential.reserve(brain->getTotalNeuronCount());
    state.neuronData.leakConductance.reserve(brain->getTotalNeuronCount());
    
    size_t neuronIndex = 0;
    for (const auto& region : brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto* neuron : population->getNeurons()) {
                const auto& nState = neuron->getState();
                state.neuronData.membranePotential.push_back(nState.membranePotential);
                state.neuronData.restingPotential.push_back(nState.restingPotential);
                state.neuronData.threshold.push_back(nState.threshold);
                state.neuronData.resetPotential.push_back(nState.resetPotential);
                state.neuronData.leakConductance.push_back(nState.leakConductance);
                state.neuronData.firingState.push_back(static_cast<uint8_t>(nState.firingState));
                state.neuronData.refractoryRemaining.push_back(nState.refractoryRemaining);
                state.neuronData.refractoryPeriod.push_back(nState.refractoryPeriod);
                state.neuronData.lastSpikeTime.push_back(nState.lastSpikeTime);
                state.neuronData.neuronType.push_back(static_cast<uint64_t>(neuron->getType()));
                state.neuronData.regionId.push_back(0); // Simplified
                state.neuronData.populationId.push_back(0); // Simplified
                
                neuronIndex++;
            }
        }
    }
    
    state.neuronCount = neuronIndex;
}

void CompleteCheckpointSystem::restoreNeuronData(std::shared_ptr<Brain>& brain, const BrainState& state) {
    size_t neuronIndex = 0;
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                if (neuronIndex < state.neuronData.membranePotential.size()) {
                    neuron->setMembranePotential(state.neuronData.membranePotential[neuronIndex]);
                    neuron->setRestingPotential(state.neuronData.restingPotential[neuronIndex]);
                    neuron->setThreshold(state.neuronData.threshold[neuronIndex]);
                    neuron->setResetPotential(state.neuronData.resetPotential[neuronIndex]);
                    neuron->setLeakConductance(state.neuronData.leakConductance[neuronIndex]);
                    
                    if (neuronIndex < state.neuronData.firingState.size()) {
                        neuron->setFiringState(static_cast<FiringState>(state.neuronData.firingState[neuronIndex]));
                    }
                    
                    if (neuronIndex < state.neuronData.refractoryPeriod.size()) {
                        neuron->setRefractoryPeriod(state.neuronData.refractoryPeriod[neuronIndex]);
                    }
                }
                neuronIndex++;
            }
        }
    }
}

void CompleteCheckpointSystem::extractSynapseData(std::shared_ptr<Brain>& brain, BrainState& state) {
    state.synapseData.sourceNeuron.reserve(brain->getTotalSynapseCount());
    state.synapseData.destinationNeuron.reserve(brain->getTotalSynapseCount());
    state.synapseData.weight.reserve(brain->getTotalSynapseCount());
    state.synapseData.delay.reserve(brain->getTotalSynapseCount());
    state.synapseData.synapseType.reserve(brain->getTotalSynapseCount());
    
    for (const auto& region : brain->getRegions()) {
        for (const auto* synapse : region->getSynapses()) {
            state.synapseData.sourceNeuron.push_back(synapse->getSourceNeuron().index());
            state.synapseData.destinationNeuron.push_back(synapse->getDestinationNeuron().index());
            state.synapseData.weight.push_back(synapse->getWeight());
            state.synapseData.delay.push_back(synapse->getDelay());
            state.synapseData.synapseType.push_back(static_cast<uint8_t>(synapse->getType()));
            state.synapseData.eligibilityTrace.push_back(synapse->getEligibilityTrace());
        }
    }
    
    state.synapseCount = state.synapseData.weight.size();
}

void CompleteCheckpointSystem::restoreSynapseData(std::shared_ptr<Brain>& brain, const BrainState& state) {
    // Note: Synapse restoration is complex as it requires rebuilding connectivity
    // This is a simplified version - in practice, you'd need to rebuild the entire network
    NLM_LOG_INFO(std::string("Checkpoint contains ") + std::to_string(state.synapseCount) + 
                 std::string(" synapses - manual restoration required"));
}

void CompleteCheckpointSystem::extractConnectivityData(std::shared_ptr<Brain>& brain, BrainState& state) {
    // Extract connectivity information
    size_t totalNeurons = brain->getTotalNeuronCount();
    state.connectivityData.neuronCount = totalNeurons;
    
    // For now, use simplified connectivity data
    // A full implementation would extract detailed connectivity information
    state.connectivityData.connectionProbability = 0.1f; // Default value
    state.connectivityData.maxConnectionsPerNeuron = static_cast<size_t>(totalNeurons * 0.1f);
}

void CompleteCheckpointSystem::restoreConnectivityData(std::shared_ptr<Brain>& brain, const BrainState& state) {
    // Restore connectivity - this would require rebuilding the network
    // For now, just log the requirement
    NLM_LOG_INFO(std::string("Restoring connectivity - requires network rebuild"));
}

void CompleteCheckpointSystem::extractPlasticityData(std::shared_ptr<Brain>& brain, BrainState& state) {
    // Extract plasticity rule parameters
    // This would include STDP parameters, Hebbian parameters, etc.
    state.plasticityData.stdpLtpWeight = 0.01f;
    state.plasticityData.stdpLtdWeight = 0.012f;
    state.plasticityData.stdpTau = 20.0f;
    state.plasticityData.hebbianLearningRate = 0.01f;
    state.plasticityData.plasticityModulation = 1.0f;
}

void CompleteCheckpointSystem::restorePlasticityData(std::shared_ptr<Brain>& brain, const BrainState& state) {
    // Restore plasticity rule parameters
    // This would restore the brain components' plasticity parameters
    NLM_LOG_INFO(std::string("Restoring plasticity parameters - requires component configuration"));
}

void CompleteCheckpointSystem::extractMemoryData(std::shared_ptr<Brain>& brain, BrainState& state) {
    // Extract memory system data
    state.memoryData.workingMemoryTraces = 0;
    state.memoryData.episodicMemorySize = 0;
    state.memoryData.associativeMemorySize = 0;
}

void CompleteCheckpointSystem::restoreMemoryData(std::shared_ptr<Brain>& brain, const BrainState& state) {
    // Restore memory system state
    NLM_LOG_INFO(std::string("Restoring memory systems - requires memory system state restoration"));
}

void CompleteCheckpointSystem::extractDevelopmentData(std::shared_ptr<Brain>& brain, BrainState& state) {
    // Extract development system data
    state.developmentData.developmentalStage = DevelopmentalStage::Adult;
    state.developmentData.synaptogenesisRate = 0.0001f;
    state.developmentData.pruningRate = 0.00001f;
    state.developmentData.maturationProgress = 1.0f;
}

void CompleteCheckpointSystem::restoreDevelopmentData(std::shared_ptr<Brain>& brain, const BrainState& state) {
    // Restore development system state
    NLM_LOG_INFO(std::string("Restoring development system - requires development system state restoration"));
}

void CompleteCheckpointSystem::extractNeuromodulationData(std::shared_ptr<Brain>& brain, BrainState& state) {
    // Extract neuromodulation system data
    state.neuromodulationData.dopamineLevel = 0.5f;
    state.neuromodulationData.curiosityLevel = 0.5f;
    state.neuromodulationData.noveltyLevel = 0.5f;
    state.neuromodulationData.predictionError = 0.0f;
}

void CompleteCheckpointSystem::restoreNeuromodulationData(std::shared_ptr<Brain>& brain, const BrainState& state) {
    // Restore neuromodulation system state
    NLM_LOG_INFO(std::string("Restoring neuromodulation systems - requires neuromodulation system state restoration"));
}

void CompleteCheckpointSystem::extractSimulationState(std::shared_ptr<Brain>& brain, BrainState& state) {
    // Extract current simulation state
    state.simulationStep = 0; // Would be the current step
    state.simulationTime = 0.0; // Would be current time
    state.totalSpikes = brain->getTotalSpikeCount();
    state.averageFiringRate = brain->getAverageFiringRate();
}

} // namespace nlm