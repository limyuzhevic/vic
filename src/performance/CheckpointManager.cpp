#include "CheckpointManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <filesystem>
#include <algorithm>
#include <ctime>
#include <fstream>

namespace nlm {

struct CheckpointManager::Impl {
    Brain* brain = nullptr;
    
    // Configuration
    std::string checkpointDir;
    uint64_t saveIntervalSteps = 10000;
    size_t maxCheckpoints = 10;
    bool compress = true;
    
    // State
    uint64_t currentStep = 0;
    double currentSimulationTime = 0.0;
    uint64_t lastSaveStep = 0;
    uint64_t totalCheckpointsCreated = 0;
    uint64_t totalBytesWritten = 0;
    uint64_t totalBytesRead = 0;
    
    bool checkpointInProgress = false;
    double checkpointProgress = 0.0;
    
    std::string lastCheckpointName;
    std::string lastCheckpointPath;
    
    // Providers
    std::function<bool(NeuronCheckpointData&)> neuronProvider;
    std::function<bool(SynapseCheckpointData&)> synapseProvider;
    
    // Cleanup
    bool cleanupOnSave = true;
    bool validateOnLoad = true;
    
    // Statistics
    struct CheckpointStats {
        uint64_t checkpointsCreated = 0;
        uint64_t checkpointsLoaded = 0;
        uint64_t checkpointsDeleted = 0;
        uint64_t neuronsSaved = 0;
        uint64_t synapsesSaved = 0;
        double totalSaveTime = 0.0;
    } stats;
};

CheckpointManager::CheckpointManager() : pImpl(std::make_unique<Impl>()) {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Create checkpoint directory if it doesn't exist
    if (!pImpl->checkpointDir.empty()) {
        std::filesystem::create_directories(pImpl->checkpointDir);
    }
    
    NLM_LOG_INFO("CheckpointManager initialized");
}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    pImpl->checkpointDir = checkpointDir;
    pImpl->saveIntervalSteps = saveIntervalSteps;
    pImpl->maxCheckpoints = maxCheckpoints;
    pImpl->compress = compress;
    
    // Create directory
    if (!pImpl->checkpointDir.empty()) {
        std::filesystem::create_directories(pImpl->checkpointDir);
    }
    
    NLM_LOG_INFO("CheckpointManager configured: dir=" + checkpointDir +
                ", interval=" + std::to_string(saveIntervalSteps) +
                ", maxCheckpoints=" + std::to_string(maxCheckpoints));
}

void CheckpointManager::requestImmediateCheckpoint(const std::string& reason) {
    pImpl->lastSaveStep = pImpl->currentStep - pImpl->saveIntervalSteps;
    NLM_LOG_INFO("CheckpointManager requested immediate checkpoint: " + reason);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentSimulationTime = currentTime;
    
    // Check if checkpoint should be saved
    if (shouldSave(currentStep)) {
        std::string checkpointName = "checkpoint_" + std::to_string(currentStep);
        
        NLM_LOG_INFO("CheckpointManager creating checkpoint at step " + 
                    std::to_string(currentStep) + ": " + checkpointName);
        
        bool success = saveImmediately(checkpointName);
        
        if (success) {
            pImpl->lastSaveStep = currentStep;
            pImpl->stats.checkpointsCreated++;
            pImpl->totalCheckpointsCreated++;
        }
        
        return success;
    }
    
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!pImpl->brain) return false;
    
    pImpl->checkpointInProgress = true;
    pImpl->checkpointProgress = 0.0;
    pImpl->lastCheckpointName = name;
    
    try {
        std::string filepath = pImpl->checkpointDir;
        if (!filepath.empty() && filepath.back() != '/') {
            filepath += "/";
        }
        filepath += name + ".ckp";
        
        NLM_LOG_INFO("CheckpointManager saving to: " + filepath);
        
        // Create checkpoint writer
        CheckpointWriter writer;
        
        if (!writer.create(filepath, pImpl->compress ? CheckpointSystem::CompressionLevel::Balanced : CheckpointSystem::CompressionLevel::None)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            pImpl->checkpointInProgress = false;
            return false;
        }
        
        // Set metadata
        writer.setMetadata(pImpl->brain->getTotalNeuronCount(),
                          pImpl->brain->getTotalSynapseCount(),
                          pImpl->brain->getRegionCount(),
                          pImpl->currentStep,
                          pImpl->currentSimulationTime);
        
        // Write neurons
        pImpl->checkpointProgress = 0.2;
        
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.restingPotential.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.threshold.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.resetPotential.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.leakConductance.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.firingState.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.refractoryRemaining.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.refractoryPeriod.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.lastSpikeTime.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.neuronType.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.regionId.reserve(pImpl->brain->getTotalNeuronCount());
        neuronData.populationId.reserve(pImpl->brain->getTotalNeuronCount());
        
        // Use provider if available, otherwise collect from brain directly
        if (pImpl->neuronProvider) {
            if (!pImpl->neuronProvider(neuronData)) {
                NLM_LOG_ERROR("Neuron provider failed");
                writer.abort();
                pImpl->checkpointInProgress = false;
                return false;
            }
        } else {
            // Direct collection from brain (simplified)
            for (const auto& region : pImpl->brain->getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        neuronData.membranePotential.push_back(state.membranePotential);
                        neuronData.restingPotential.push_back(state.restingPotential);
                        neuronData.threshold.push_back(state.threshold);
                        neuronData.resetPotential.push_back(state.resetPotential);
                        neuronData.leakConductance.push_back(state.leakConductance);
                        neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                        neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                        neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                        neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                        neuronData.neuronType.push_back(static_cast<uint64_t>(neuron->getType()));
                        neuronData.regionId.push_back(region->getId().index());
                        neuronData.populationId.push_back(pop->getId().index());
                    }
                }
            }
        }
        
        pImpl->stats.neuronsSaved += neuronData.membranePotential.size();
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            writer.abort();
            pImpl->checkpointInProgress = false;
            return false;
        }
        
        // Write synapses
        pImpl->checkpointProgress = 0.5;
        
        SynapseCheckpointData synapseData;
        if (pImpl->synapseProvider) {
            if (!pImpl->synapseProvider(synapseData)) {
                NLM_LOG_ERROR("Synapse provider failed");
                writer.abort();
                pImpl->checkpointInProgress = false;
                return false;
            }
        } else {
            // Collect from brain regions
            for (const auto& region : pImpl->brain->getRegions()) {
                for (const auto* syn : region->getSynapses()) {
                    synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                    synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                    synapseData.weight.push_back(syn->getWeight());
                    synapseData.delay.push_back(syn->getDelay());
                    synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                    synapseData.plasticityFlags.push_back(syn->getPlasticityFlags());
                    synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                    synapseData.efficacy.push_back(syn->getEfficacy());
                    synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                    synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
                }
            }
        }
        
        pImpl->stats.synapsesSaved += synapseData.weight.size();
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            writer.abort();
            pImpl->checkpointInProgress = false;
            return false;
        }
        
        // Finalize
        pImpl->checkpointProgress = 0.8;
        
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            writer.abort();
            pImpl->checkpointInProgress = false;
            return false;
        }
        
        pImpl->checkpointProgress = 1.0;
        pImpl->totalBytesWritten += writer.getBytesWritten();
        pImpl->lastCheckpointPath = filepath;
        
        NLM_LOG_INFO("CheckpointManager checkpoint saved: " + std::to_string(writer.getBytesWritten()) + " bytes");
        
        // Prune old checkpoints if configured
        if (pImpl->cleanupOnSave) {
            pruneOldCheckpoints(pImpl->maxCheckpoints);
        }
        
        pImpl->checkpointInProgress = false;
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during checkpoint: " + std::string(e.what()));
        pImpl->checkpointInProgress = false;
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    if (!pImpl->brain) return false;
    
    try {
        std::string filepath = pImpl->checkpointDir;
        if (!filepath.empty() && filepath.back() != '/') {
            filepath += "/";
        }
        filepath += name + ".ckp";
        
        NLM_LOG_INFO("CheckpointManager loading from: " + filepath);
        
        CheckpointReader reader;
        
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            reader.close();
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            reader.close();
            return false;
        }
        
        pImpl->totalBytesRead += neuronData.membranePotential.size() * sizeof(float) * 10; // Approximate
        
        // Apply neuron states (simplified - would need region mapping)
        size_t idx = 0;
        for (const auto& region : pImpl->brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            reader.close();
            return false;
        }
        
        NLM_LOG_INFO("CheckpointManager loaded: " + std::to_string(synapseData.weight.size()) + " synapses");
        
        reader.close();
        pImpl->stats.checkpointsLoaded++;
        pImpl->lastCheckpointName = name;
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during checkpoint loading: " + std::string(e.what()));
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointManager::CheckpointInfo> checkpoints;
    
    if (pImpl->checkpointDir.empty()) {
        return checkpoints;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(pImpl->checkpointDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // Check if it's a checkpoint file (ends with .ckp)
                if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".ckp") {
                    CheckpointInfo info;
                    info.name = filename.substr(0, filename.size() - 4);
                    
                    // Extract step from filename if possible
                    // Format: checkpoint_<step>.ckp
                    size_t underscore = info.name.find('_');
                    size_t dot = info.name.find('.');
                    if (underscore != std::string::npos && dot != std::string::npos) {
                        std::string stepStr = info.name.substr(underscore + 1, dot - underscore - 1);
                        try {
                            info.step = std::stoull(stepStr);
                        } catch (...) {
                            info.step = 0;
                        }
                    } else {
                        info.step = 0;
                    }
                    
                    info.fileSize = entry.file_size();
                    info.timestamp = std::chrono::system_clock::to_time_t(
                        std::chrono::file_time_clock::to_sys(entry.last_write_time()));
                    
                    // Try to get simulation time from metadata if possible
                    // For now, set to step * 0.1
                    info.simulationTime = info.step * 0.1;
                    
                    checkpoints.push_back(info);
                }
            }
        }
        
        // Sort by step number (descending - most recent first)
        std::sort(checkpoints.begin(), checkpoints.end(),
                 [](const CheckpointInfo& a, const CheckpointInfo& b) {
                     return a.step > b.step;
                 });
                 
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to list checkpoints: " + std::string(e.what()));
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Sort by step number (ascending - oldest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
             [](const CheckpointInfo& a, const CheckpointInfo& b) {
                 return a.step < b.step;
             });
    
    // Delete oldest checkpoints
    size_t toDelete = checkpoints.size() - keepCount;
    for (size_t i = 0; i < toDelete; ++i) {
        std::string filepath = pImpl->checkpointDir;
        if (!filepath.empty() && filepath.back() != '/') {
            filepath += "/";
        }
        filepath += checkpoints[i].name + ".ckp";
        
        try {
            std::filesystem::remove(filepath);
            pImpl->stats.checkpointsDeleted++;
            NLM_LOG_INFO("CheckpointManager deleted old checkpoint: " + checkpoints[i].name);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to delete checkpoint " + checkpoints[i].name + ": " + std::string(e.what()));
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return pImpl->lastCheckpointPath;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep <= pImpl->lastSaveStep) {
        return false;
    }
    
    return (currentStep - pImpl->lastSaveStep) >= pImpl->saveIntervalSteps;
}

void CheckpointManager::reset() {
    pImpl->currentStep = 0;
    pImpl->currentSimulationTime = 0.0;
    pImpl->lastSaveStep = 0;
    pImpl->lastCheckpointName.clear();
    pImpl->lastCheckpointPath.clear();
    
    pImpl->checkpointInProgress = false;
    pImpl->checkpointProgress = 0.0;
    
    pImpl->neuronProvider = nullptr;
    pImpl->synapseProvider = nullptr;
    
    NLM_LOG_INFO("CheckpointManager reset");
}

} // namespace nlm
