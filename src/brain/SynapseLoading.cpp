// Complete SynapseLoading.cpp implementation
#include "SynapseLoading.hpp"
#include "Brain.hpp"
#include "NeuralRegion.hpp"
#include "NeuralPopulation.hpp"
#include <unordered_map>
#include <thread>
#include <mutex>
#include <algorithm>
#include <limits>
#include <chrono>

namespace nlm {

// Global synapse loading state
static SynapseLoadingState globalLoadingState;

// Forward declarations for helper functions
static bool applySynapseDataImpl(Brain::Impl* impl, const SynapseCheckpointData& synapseData);

// SynapseLookupTable implementation
struct SynapseLookupTable {
    std::unordered_map<std::pair<NeuronId, NeuronId>, Synapse*> synapseMap;
    std::unordered_map<NeuronId, std::vector<Synapse*>> outgoingSynapses;
    std::unordered_map<NeuronId, std::vector<Synapse*>> incomingSynapses;
    std::vector<Synapse*> allSynapses;
    mutable std::mutex mutex;
};

static SynapseLookupTable globalSynapseLookupTable;

// Performance optimization constants
static constexpr size_t DEFAULT_LOADING_TIMEOUT_MS = 5000;
static constexpr size_t MAX_VALIDATION_ERRORS = 10;

// Implementation of SynapseLoader methods

bool SynapseLoader::loadSynapses(const SynapseCheckpointData& data, Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot load synapses: brain is null");
        return false;
    }
    
    // Reset loading state
    globalLoadingState.synapsesLoaded = 0;
    globalLoadingState.synapsesCreated = 0;
    globalLoadingState.synapsesUpdated = 0;
    globalLoadingState.validationErrors = 0;
    globalLoadingState.validationPassed = false;
    
    // Validate input data
    if (!validateSynapseData(data)) {
        return false;
    }
    
    NLM_LOG_INFO("Synapse loading validation passed");
    
    // Determine loading strategy based on network size
    size_t synapseCount = data.sourceNeuron.size();
    bool useParallel = shouldUseParallelLoading(synapseCount);
    
    NLM_LOG_INFO(std::to_string(synapseCount) + " synapses to load, using " + 
                (useParallel ? "parallel" : "sequential") + " loading");
    
    if (useParallel) {
        return loadSynapsesParallel(data, brain);
    } else {
        return loadSynapsesSequential(data, brain);
    }
}

bool SynapseLoader::matchSynapseToExisting(const SynapseCheckpointData& synapseData, 
                                          const Brain* brain, 
                                          size_t synapseIndex) {
    if (!brain || synapseIndex >= synapseData.sourceNeuron.size()) {
        return false;
    }
    
    const NeuronId source = static_cast<NeuronId>(synapseData.sourceNeuron[synapseIndex]);
    const NeuronId dest = static_cast<NeuronId>(synapseData.destinationNeuron[synapseIndex]);
    
    // Find or create synapse
    Synapse* synapse = getOrCreateSynapse(brain, source, dest);
    if (!synapse) {
        ++globalLoadingState.validationErrors;
        return false;
    }
    
    // Apply all synapse properties from checkpoint
    bool updated = applySynapseProperties(synapse, synapseData, synapseIndex);
    if (updated) {
        ++globalLoadingState.synapsesUpdated;
    } else {
        ++globalLoadingState.synapsesCreated;
    }
    
    ++globalLoadingState.synapsesLoaded;
    return true;
}

Synapse* SynapseLoader::getOrCreateSynapse(const Brain* brain,
                                          const NeuronId& source,
                                          const NeuronId& dest) {
    // First try to find existing synapse in lookup table
    {
        std::lock_guard<std::mutex> lock(globalSynapseLookupTable.mutex);
        auto it = globalSynapseLookupTable.synapseMap.find({source, dest});
        if (it != globalSynapseLookupTable.synapseMap.end()) {
            return it->second;
        }
    }
    
    // If not found, search through all regions and populations
    for (const auto& region : brain->pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            // Find the source neuron
            Neuron* sourceNeuron = nullptr;
            for (auto* n : pop->getNeurons()) {
                if (n->getId() == source) {
                    sourceNeuron = n;
                    break;
                }
            }
            
            if (!sourceNeuron) {
                continue;
            }
            
            // Check outgoing synapses from this region
            for (const auto& region2 : brain->pImpl->regions) {
                for (const auto& pop2 : region2->getPopulations()) {
                    for (const auto* syn : pop2->getSynapses()) {
                        if (syn->getSourceNeuron() == source && syn->getDestinationNeuron() == dest) {
                            // Found existing synapse
                            std::lock_guard<std::mutex> lock(globalSynapseLookupTable.mutex);
                            globalSynapseLookupTable.synapseMap[{source, dest}] = const_cast<Synapse*>(syn);
                            globalSynapseLookupTable.outgoingSynapses[source].push_back(const_cast<Synapse*>(syn));
                            globalSynapseLookupTable.incomingSynapses[dest].push_back(const_cast<Synapse*>(syn));
                            globalSynapseLookupTable.allSynapses.push_back(const_cast<Synapse*>(syn));
                            return const_cast<Synapse*>(syn);
                        }
                    }
                }
            }
        }
    }
    
    return nullptr;
}

void SynapseLoader::setupSynapseLookupTables(const Brain* brain) {
    if (!brain) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(globalSynapseLookupTable.mutex);
    
    // Clear existing tables
    globalSynapseLookupTable.synapseMap.clear();
    globalSynapseLookupTable.outgoingSynapses.clear();
    globalSynapseLookupTable.incomingSynapses.clear();
    globalSynapseLookupTable.allSynapses.clear();
    
    // Rebuild tables from existing synapses
    for (const auto& region : brain->pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* syn : pop->getSynapses()) {
                const NeuronId source = syn->getSourceNeuron();
                const NeuronId dest = syn->getDestinationNeuron();
                
                // Add to lookup tables
                globalSynapseLookupTable.synapseMap[{source, dest}] = const_cast<Synapse*>(syn);
                globalSynapseLookupTable.outgoingSynapses[source].push_back(const_cast<Synapse*>(syn));
                globalSynapseLookupTable.incomingSynapses[dest].push_back(const_cast<Synapse*>(syn));
                globalSynapseLookupTable.allSynapses.push_back(const_cast<Synapse*>(syn));
            }
        }
    }
}

// Helper functions for Brain.cpp integration

bool validateSynapseCheckpointData(const SynapseCheckpointData& data) {
    size_t count = data.sourceNeuron.size();
    
    if (count == 0) {
        return true;  // No synapses to load is valid
    }
    
    // Check data consistency
    if (data.destinationNeuron.size() != count ||
        data.weight.size() != count ||
        data.delay.size() != count ||
        data.synapseType.size() != count ||
        data.plasticityFlags.size() != count ||
        data.eligibilityTrace.size() != count) {
        return false;
    }
    
    // Validate synapse properties
    for (size_t i = 0; i < count; ++i) {
        if (!validateSingleSynapseData(data, i)) {
            return false;
        }
    }
    
    return true;
}

bool validateSingleSynapseData(const SynapseCheckpointData& data, size_t index) {
    // Check for self-connections
    if (data.sourceNeuron[index] == data.destinationNeuron[index]) {
        return false;  // Self-connections not allowed
    }
    
    // Validate weight bounds
    if (data.weight[index] < -1.0f || data.weight[index] > 1.0f) {
        return false;
    }
    
    // Validate delay (reasonable range)
    if (data.delay[index] > 10000) {
        return false;
    }
    
    // Validate synapse type
    if (static_cast<uint8_t>(data.synapseType[index]) > 4) {
        return false;
    }
    
    return true;
}

bool loadSynapsesSequential(Brain::Impl* impl, const SynapseCheckpointData& data) {
    size_t count = data.sourceNeuron.size();
    
    for (size_t i = 0; i < count; ++i) {
        // Build neuron ID to region/pops mapping for lookup
        std::unordered_map<NeuronId, std::pair<RegionId, PopulationId>> neuronToRegionPop;
        for (const auto& region : impl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (Neuron* neuron : pop->getNeurons()) {
                    neuronToRegionPop[neuron->getId()] = std::make_pair(region->getId(), pop->getId());
                }
            }
        }
        
        // Apply synapse data using the mapping for O(1) lookup
        NeuronId source = static_cast<NeuronId>(data.sourceNeuron[i]);
        NeuronId dest = static_cast<NeuronId>(data.destinationNeuron[i]);
        
        auto sourceIt = neuronToRegionPop.find(source);
        auto destIt = neuronToRegionPop.find(dest);
        
        if (sourceIt != neuronToRegionPop.end() && destIt != neuronToRegionPop.end()) {
            // Find the source population
            auto* sourceRegion = impl->getRegion(sourceIt->second.first);
            if (!sourceRegion) continue;
            
            auto* sourcePop = sourceRegion->getPopulation(sourceIt->second.second);
            if (!sourcePop) continue;
            
            // Create or update synapse with all properties
            SynapseId synId = sourcePop->addSynapse(source, dest, data.weight[i], data.delay[i]);
            if (synId.value == INVALID_SYNAPSE_ID.value) {
                continue;
            }
            
            // Get the synapse and set all properties
            auto* syn = sourcePop->getSynapse(synId);
            if (!syn) continue;
            
            // Apply all synapse properties from checkpoint
            syn->setType(static_cast<SynapseType>(data.synapseType[i]));
            syn->setEligibilityTrace(data.eligibilityTrace[i]);
            
            // Set plasticity flags
            PlasticityFlags& flags = syn->getPlasticityFlags();
            uint8_t pf = data.plasticityFlags[i];
            flags.hebbian = (pf & 0x01) != 0;
            flags.stdp = (pf & 0x02) != 0;
            flags.reward_modulated = (pf & 0x04) != 0;
            
            // Set additional properties if available
            if (data.efficacy.size() > i) {
                syn->setEfficacy(data.efficacy[i]);
            }
        }
    }
    
    return true;
}

bool loadSynapsesParallel(Brain::Impl* impl, const SynapseCheckpointData& data) {
    size_t count = data.sourceNeuron.size();
    std::vector<std::future<bool>> futures;
    
    // Determine optimal thread count
    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 2;
    
    size_t synapsesPerThread = count / threadCount;
    
    // Create threads for parallel processing
    for (unsigned int t = 0; t < threadCount; ++t) {
        size_t startIdx = t * synapsesPerThread;
        size_t endIdx = (t == threadCount - 1) ? count : (t + 1) * synapsesPerThread;
        
        futures.push_back(std::async(std::launch::async, 
            [impl, &data, startIdx, endIdx]() {
                bool result = true;
                
                // Each thread builds its own neuron-to-region mapping
                std::unordered_map<NeuronId, std::pair<RegionId, PopulationId>> neuronToRegionPop;
                for (const auto& region : impl->regions) {
                    for (const auto& pop : region->getPopulations()) {
                        for (Neuron* neuron : pop->getNeurons()) {
                            neuronToRegionPop[neuron->getId()] = std::make_pair(region->getId(), pop->getId());
                        }
                    }
                }
                
                for (size_t i = startIdx; i < endIdx; ++i) {
                    NeuronId source = static_cast<NeuronId>(data.sourceNeuron[i]);
                    NeuronId dest = static_cast<NeuronId>(data.destinationNeuron[i]);
                    
                    auto sourceIt = neuronToRegionPop.find(source);
                    auto destIt = neuronToRegionPop.find(dest);
                    
                    if (sourceIt != neuronToRegionPop.end() && destIt != neuronToRegionPop.end()) {
                        // Find the source population
                        auto* sourceRegion = impl->getRegion(sourceIt->second.first);
                        if (!sourceRegion) continue;
                        
                        auto* sourcePop = sourceRegion->getPopulation(sourceIt->second.second);
                        if (!sourcePop) continue;
                        
                        // Create or update synapse
                        SynapseId synId = sourcePop->addSynapse(source, dest, data.weight[i], data.delay[i]);
                        if (synId.value == INVALID_SYNAPSE_ID.value) {
                            continue;
                        }
                        
                        // Get the synapse and set all properties
                        auto* syn = sourcePop->getSynapse(synId);
                        if (!syn) continue;
                        
                        // Apply all synapse properties
                        syn->setType(static_cast<SynapseType>(data.synapseType[i]));
                        syn->setEligibilityTrace(data.eligibilityTrace[i]);
                        
                        // Set plasticity flags
                        PlasticityFlags& flags = syn->getPlasticityFlags();
                        uint8_t pf = data.plasticityFlags[i];
                        flags.hebbian = (pf & 0x01) != 0;
                        flags.stdp = (pf & 0x02) != 0;
                        flags.reward_modulated = (pf & 0x04) != 0;
                        
                        // Set additional properties if available
                        if (data.efficacy.size() > i) {
                            syn->setEfficacy(data.efficacy[i]);
                        }
                    }
                }
                
                return result;
            }
        ));
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        if (!future.get()) {
            return false;
        }
    }
    
    return true;
}

void setupSynapseLookupTables(Brain::Impl* impl) {
    if (!impl) return;
    
    std::lock_guard<std::mutex> lock(globalSynapseLookupTable.mutex);
    
    // Clear existing tables
    globalSynapseLookupTable.synapseMap.clear();
    globalSynapseLookupTable.outgoingSynapses.clear();
    globalSynapseLookupTable.incomingSynapses.clear();
    globalSynapseLookupTable.allSynapses.clear();
    
    // Rebuild tables from existing synapses
    for (const auto& region : impl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* syn : pop->getSynapses()) {
                const NeuronId source = syn->getSourceNeuron();
                const NeuronId dest = syn->getDestinationNeuron();
                
                // Add to lookup tables
                globalSynapseLookupTable.synapseMap[{source, dest}] = const_cast<Synapse*>(syn);
                globalSynapseLookupTable.outgoingSynapses[source].push_back(const_cast<Synapse*>(syn));
                globalSynapseLookupTable.incomingSynapses[dest].push_back(const_cast<Synapse*>(syn));
                globalSynapseLookupTable.allSynapses.push_back(const_cast<Synapse*>(syn));
            }
        }
    }
}

bool applySynapseDataImpl(Brain::Impl* impl, const SynapseCheckpointData& synapseData) {
    if (!impl) return false;
    
    // Setup synapse lookup tables for efficient loading
    setupSynapseLookupTables(impl);
    
    // Validate checkpoint data
    if (!validateSynapseCheckpointData(synapseData)) {
        return false;
    }
    
    size_t synapseCount = synapseData.sourceNeuron.size();
    
    // Determine loading strategy based on network size
    bool useParallel = (synapseCount > 1000);
    
    if (useParallel) {
        return loadSynapsesParallel(impl, synapseData);
    } else {
        return loadSynapsesSequential(impl, synapseData);
    }
}

} // namespace nlm
