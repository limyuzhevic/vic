#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>

namespace nlm {

class BrainCheckpoint {
public:
    BrainCheckpoint() = default;
    ~BrainCheckpoint() = default;

    // Save brain state to file (checkpointing)
    bool save(Brain* brain, const std::string& filepath) const;

    // Load brain state from file
    bool load(Brain* brain, const std::string& filepath);

    // Get checkpoint statistics
    struct CheckpointStats {
        size_t neuronsSaved = 0;
        size_t synapsesSaved = 0;
        size_t spikesSaved = 0;
        size_t memoryItemsSaved = 0;
        double saveTime = 0.0;
        double loadTime = 0.0;
        bool saveSuccess = false;
        bool loadSuccess = false;
    };

    CheckpointStats getStats() const { return stats; }
    void resetStats() { stats = CheckpointStats(); }

private:
    // Helper: write neuron data to checkpoint
    bool writeNeurons(Brain* brain, CheckpointWriter& writer) const;

    // Helper: write synapse data to checkpoint
    bool writeSynapses(Brain* brain, CheckpointWriter& writer) const;

    // Helper: write delayed spike events to checkpoint
    bool writeDelayedSpikes(Brain* brain, CheckpointWriter& writer) const;

    // Helper: write Phase 5 extra sections to checkpoint
    bool writePhase5ExtraSections(Brain* brain, CheckpointWriter& writer) const;

    // Helper: read neuron data from checkpoint
    bool readNeurons(Brain* brain, CheckpointReader& reader);

    // Helper: read synapse data from checkpoint
    bool readSynapses(Brain* brain, CheckpointReader& reader);

    // Helper: read delayed spike events from checkpoint
    bool readDelayedSpikes(Brain* brain, CheckpointReader& reader);

    // Helper: read Phase 5 extra sections from checkpoint
    bool readPhase5ExtraSections(Brain* brain, CheckpointReader& reader);

    // Helper: match synapses by (source, destination) pair
    static uint64_t makePairKey(uint64_t src, uint64_t dst);

    CheckpointStats stats;

    // Phase 5 checkpoint payload helpers (legacy format compatibility)
    namespace {
        void p5AppendU64(std::vector<uint8_t>& out, uint64_t v);
        void p5AppendFloat(std::vector<uint8_t>& out, float v);
        void p5AppendDouble(std::vector<uint8_t>& out, double v);
        template <typename T>
        void p5AppendVec(std::vector<uint8_t>& out, const std::vector<T>& v);

        struct P5Reader {
            const uint8_t* p;
            const uint8_t* end;
            bool ok = true;
            bool u64(uint64_t& v);
            bool f32(float& v);
            bool f64(double& v);
            template <typename T>
            bool vec(std::vector<T>& v);
        };
    }
};
