// NLM Brain State Management
// Advanced save/load functionality with compression and validation

#ifndef NLM_BRAIN_STATE_HPP
#define NLM_BRAIN_STATE_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

class BrainStateManager {
public:
    struct BrainMetadata {
        std::string version;
        uint64_t timestamp;
        uint64_t checksum;
        std::map<std::string, std::string> configuration;
        size_t neuronCount;
        size_t synapseCount;
        float simulationTimestep;
        std::string description;
    };

    BrainStateManager();
    ~BrainStateManager();

    // Save brain state with multiple format options
    bool saveBrainState(const std::string& filename, 
                       const Brain& brain,
                       const std::string& format = "binary",
                       bool compress = true,
                       const std::string& description = "");

    // Load brain state from file
    bool loadBrainState(const std::string& filename,
                       Brain& brain,
                       const std::string& format = "auto",
                       bool validate = true);

    // Checkpoint management for large simulations
    bool createCheckpoint(const std::string& filename,
                         const Brain& brain,
                         size_t checkpointInterval,
                         size_t maxCheckpoints = 100);

    // Brain snapshot for reproduction
    bool exportBrainSnapshot(const std::string& filename,
                            const Brain& brain,
                            const std::map<std::string, std::string>& options = {});

    // Import brain from external sources
    bool importBrain(const std::string& filename,
                    Brain& brain,
                    const std::string& sourceFormat = "auto");

    // Brain comparison and validation
    static bool compareBrainStates(const std::string& file1,
                                  const std::string& file2,
                                  std::vector<std::string>& differences);

    // Metadata extraction without full load
    static bool extractMetadata(const std::string& filename,
                               BrainMetadata& metadata);

private:
    // File format handlers
    bool saveAsBinary(const std::string& filename, const Brain& brain, bool compress);
    bool saveAsText(const std::string& filename, const Brain& brain);
    bool saveAsJSON(const std::string& filename, const Brain& brain);

    bool loadFromBinary(const std::string& filename, Brain& brain, bool validate);
    bool loadFromText(const std::string& filename, Brain& brain);
    bool loadFromJSON(const std::string& filename, Brain& brain);

    // Compression utilities
    std::string compressData(const std::string& data) const;
    std::string decompressData(const std::string& compressed) const;
    bool isCompressionAvailable() const;

    // Validation and integrity
    uint64_t calculateChecksum(const std::string& data) const;
    bool validateIntegrity(const std::string& data, uint64_t expectedChecksum) const;
    bool validateConfiguration(const std::map<std::string, std::string>& config) const;
    bool validateBrainConnectivity(const Brain& brain) const;

    // Metadata management
    BrainMetadata createMetadata(const Brain& brain, const std::string& description) const;
    std::string metadataToString(const BrainMetadata& metadata) const;
    BrainMetadata parseMetadata(const std::string& metadataString) const;

    // Serialization utilities
    std::string serializeNeuron(const Neuron* neuron) const;
    std::string serializeSynapse(const Synapse* synapse) const;
    std::string serializeRegion(const NeuralRegion* region) const;

    Neuron* deserializeNeuron(const std::string& data) const;
    Synapse* deserializeSynapse(const std::string& data, std::map<NeuronId, Neuron*>& neuronMap) const;
    NeuralRegion* deserializeRegion(const std::string& data, std::map<NeuronId, Neuron*>& neuronMap) const;

    // Backup and recovery
    bool createBackup(const std::string& source, const std::string& backupDir);
    bool restoreFromBackup(const std::string& backupFile, Brain& brain);
    std::vector<std::string> getBackupList(const std::string& backupDir) const;
};

// Advanced brain state operations
class AdvancedBrainOperations {
public:
    AdvancedBrainOperations(std::shared_ptr<Brain> brain);
    ~AdvancedBrainOperations();

    // Brain cloning and copying
    std::shared_ptr<Brain> cloneBrain() const;
    bool copyBrainTo(const std::string& filename) const;
    bool restoreBrainFrom(const std::string& filename);

    // Brain manipulation and modification
    bool injectSynapses(const std::vector<std::pair<NeuronId, NeuronId>>& connections,
                       const std::vector<float>& weights);
    bool removeSynapses(const std::vector<std::pair<NeuronId, NeuronId>>& connections);
    bool modifySynapseWeights(const std::map<SynapseId, float>& weightChanges);

    // State transformation
    bool resetToState(const std::string& filename);
    bool mergeBrains(const std::vector<std::string>& brainFiles);
    bool pruneBrain(float threshold, bool preserveTopology = false);

    // Analysis and modification
    float calculateBrainComplexity() const;
    size_t countConnectedComponents() const;
    bool balanceExcitatoryInhibitoryRatio(float targetRatio);
    std::vector<NeuronId> findMostActiveNeurons(size_t count = 10) const;

private:
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm

#endif // NLM_BRAIN_STATE_HPP
