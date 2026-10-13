#pragma once

#include "../core/Types/Types.hpp"
#include "Synapse.hpp"
#include <unordered_map>
#include <thread>
#include <mutex>
#include <chrono>

namespace nlm {

// Synapse checkpoint data structure (reference to existing structure)
struct SynapseCheckpointData;

// Synapse lookup table for efficient synapse access during loading
struct SynapseLookupTable {
    std::unordered_map<std::pair<NeuronId, NeuronId>, Synapse*> synapseMap;
    std::unordered_map<NeuronId, std::vector<Synapse*>> outgoingSynapses;
    std::unordered_map<NeuronId, std::vector<Synapse*>> incomingSynapses;
    std::vector<Synapse*> allSynapses;
    mutable std::mutex mutex;
};

// Synapse loading state tracking
struct SynapseLoadingState {
    size_t synapsesLoaded = 0;
    size_t synapsesCreated = 0;
    size_t synapsesUpdated = 0;
    size_t validationErrors = 0;
    std::chrono::steady_clock::time_point startTime;
    bool validationPassed = false;
    
    SynapseLoadingState() : startTime(std::chrono::steady_clock::now()) {}
};

// Main SynapseLoader class for checkpoint loading
class SynapseLoader {
public:
    static bool loadSynapses(const SynapseCheckpointData& data, Brain* brain);
    static bool matchSynapseToExisting(const SynapseCheckpointData& synapseData, 
                                      const Brain* brain, 
                                      size_t synapseIndex);
    static Synapse* getOrCreateSynapse(const Brain* brain,
                                      const NeuronId& source,
                                      const NeuronId& dest);
    static void setupSynapseLookupTables(const Brain* brain);
    static SynapseLoadingState getLoadingState() { return globalLoadingState; }
    static void resetLoadingState() {
        globalLoadingState.synapsesLoaded = 0;
        globalLoadingState.synapsesCreated = 0;
        globalLoadingState.synapsesUpdated = 0;
        globalLoadingState.validationErrors = 0;
        globalLoadingState.validationPassed = false;
    }

private:
    static bool validateSynapseData(const SynapseCheckpointData& data);
    static bool checkDataConsistency(const SynapseCheckpointData& data, size_t count);
    static bool validateIndividualSynapses(const SynapseCheckpointData& data, size_t count);
    static bool validateSingleSynapse(const SynapseCheckpointData& data, size_t index);
    static bool shouldUseParallelLoading(size_t synapseCount);
    static bool loadSynapsesSequential(const SynapseCheckpointData& data, Brain* brain);
    static bool loadSynapsesParallel(const SynapseCheckpointData& data, Brain* brain);
    static bool loadSingleSynapse(const SynapseCheckpointData& data, size_t index, Brain* brain);
    static Synapse* findExistingSynapse(Brain* brain, const NeuronId& source, const NeuronId& dest);
    static bool updateExistingSynapse(Synapse* synapse, const SynapseCheckpointData& data, size_t index);
    static bool createNewSynapse(Brain* brain, const NeuronId& source, const NeuronId& dest,
                                 const SynapseCheckpointData& data, size_t index);
    static bool isLoadingTimedOut();

private:
    static SynapseLoadingState globalLoadingState;
    static SynapseLookupTable globalSynapseLookupTable;
    static std::mutex globalMutex;
};

// Global loading functions for Brain.cpp integration
bool validateSynapseCheckpointData(const SynapseCheckpointData& data);
bool validateSingleSynapseData(const SynapseCheckpointData& data, size_t index);
bool loadSynapsesSequential(Brain::Impl* impl, const SynapseCheckpointData& data);
bool loadSynapsesParallel(Brain::Impl* impl, const SynapseCheckpointData& data);
void setupSynapseLookupTables(Brain::Impl* impl);
bool applySynapseDataImpl(Brain::Impl* impl, const SynapseCheckpointData& synapseData);

} // namespace nlm
