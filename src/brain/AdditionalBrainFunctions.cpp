#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>
#include "Brain.hpp"

namespace nlm {

void Brain::logStatus() const {
    NLM_LOG_INFO("=== Brain Status ===");
    
    // Log region information
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto* region = pImpl->regions[i].get();
        if (region) {
            auto neurons = region->getAllNeurons();
            size_t activeNeurons = 0;
            for (Neuron* n : neurons) {
                if (n->isFiring()) ++activeNeurons;
            }
            NLM_LOG_INFO("Region " + std::to_string(i) + ": " + 
                         std::to_string(neurons.size()) + " neurons, " +
                         std::to_string(activeNeurons) + " firing");
        }
    }
    
    // Log statistics
    NLM_LOG_INFO("Total spikes: " + std::to_string(pImpl->totalSpikesTotal));
    NLM_LOG_INFO("Current step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("Current time: " + std::to_string(pImpl->currentTime) + "s");
    
    // Log learning metrics if available
    NLM_LOG_INFO("Learned preference: " + std::to_string(pImpl->learnedPreference));
}

bool Brain::saveState(const std::string& filename) {
    if (!pImpl->checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not initialized");
        return false;
    }
    
    std::string fullPath = filename;
    if (!filename.empty() && filename.find("/") == std::string::npos && 
        filename.find("\\") == std::string::npos) {
        fullPath = "../" + filename;
    }
    
    NLM_LOG_INFO("Saving brain state to: " + fullPath);
    
    std::ofstream file(fullPath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for writing: " + fullPath);
        return false;
    }
    
    try {
        // Write brain state in binary format
        file.write(reinterpret_cast<const char*>(&pImpl->totalSpikesTotal), sizeof(pImpl->totalSpikesTotal));
        file.write(reinterpret_cast<const char*>(&pImpl->learnedPreference), sizeof(pImpl->learnedPreference));
        file.write(reinterpret_cast<const char*>(&pImpl->currentStep), sizeof(pImpl->currentStep));
        file.write(reinterpret_cast<const char*>(&pImpl->currentTime), sizeof(pImpl->currentTime));
        
        // Save synaptic weights
        for (auto& region : pImpl->regions) {
            auto& synapses = region->getSynapses();
            size_t count = synapses.size();
            file.write(reinterpret_cast<const char*>(&count), sizeof(count));
            for (const auto& syn : synapses) {
                file.write(reinterpret_cast<const char*>(&syn->getWeight()), sizeof(float));
            }
        }
        
        file.flush();
        if (file.good()) {
            NLM_LOG_INFO("Successfully saved brain state");
            return true;
        } else {
            NLM_LOG_ERROR("Failed to write complete brain state");
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during save: " + std::string(e.what()));
        return false;
    }
}

bool Brain::loadState(const std::string& filename) {
    if (!pImpl->checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not initialized");
        return false;
    }
    
    std::string fullPath = filename;
    if (!filename.empty() && filename.find("/") == std::string::npos && 
        filename.find("\\") == std::string::npos) {
        fullPath = "../" + filename;
    }
    
    NLM_LOG_INFO("Loading brain state from: " + fullPath);
    
    std::ifstream file(fullPath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for reading: " + fullPath);
        return false;
    }
    
    try {
        // Clear current state first
        pImpl->regions.clear();
        pImpl->sensoryNeurons.clear();
        pImpl->motorNeurons.clear();
        pImpl->neuronById.clear();
        
        // Read brain state
        file.read(reinterpret_cast<char*>(&pImpl->totalSpikesTotal), sizeof(pImpl->totalSpikesTotal));
        file.read(reinterpret_cast<char*>(&pImpl->learnedPreference), sizeof(pImpl->learnedPreference));
        file.read(reinterpret_cast<char*>(&pImpl->currentStep), sizeof(pImpl->currentStep));
        file.read(reinterpret_cast<char*>(&pImpl->currentTime), sizeof(pImpl->currentTime));
        
        // Reload synaptic weights
        for (auto& region : pImpl->regions) {
            auto& synapses = region->getSynapses();
            size_t count = 0;
            file.read(reinterpret_cast<char*>(&count), sizeof(count));
            for (size_t i = 0; i < count; ++i) {
                float weight = 0.0f;
                file.read(reinterpret_cast<char*>(&weight), sizeof(float));
                if (i < synapses.size()) {
                    synapses[i]->setWeight(weight);
                }
            }
        }
        
        if (file.good() && !file.eof()) {
            NLM_LOG_ERROR("File truncated or incomplete");
            return false;
        }
        
        NLM_LOG_INFO("Successfully loaded brain state");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during load: " + std::string(e.what()));
        return false;
    }
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");

    // Re-initialising must rebuild, not append: a previous run left
    // reset()+initialize() reporting 1000 neurons for a 500-neuron brain.
    pImpl->regions.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    pImpl->nextRegionId = RegionId(1);
    pImpl->totalSpikesTotal = 0;
    
    return true;
}

} // namespace nlm
