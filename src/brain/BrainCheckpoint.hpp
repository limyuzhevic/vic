// Brain Checkpoint Header
// Refactored checkpoint persistence for Brain.cpp

#ifndef BRAIN_CHECKPOINT_HPP
#define BRAIN_CHECKPOINT_HPP

#include "Brain.hpp"
#include "../brain/BrainHelpers.hpp"

namespace nlm {

class BrainCheckpoint {
public:
    // Phase 5 checkpoint payload helpers
    static void p5AppendU64(std::vector<uint8_t>& out, uint64_t v);
    static void p5AppendFloat(std::vector<uint8_t>& out, float v);
    static void p5AppendDouble(std::vector<uint8_t>& out, double v);
    template <typename T>
    static void p5AppendVec(std::vector<uint8_t>& out, const std::vector<T>& v);
    
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
    
    // Data preparation methods
    static void prepareNeuronCheckpointData(const Brain::Impl& pImpl, NeuronCheckpointData& neuronData);
    static void prepareSynapseCheckpointData(const Brain::Impl& pImpl, SynapseCheckpointData& synapseData);
    static void prepareSimulationStateData(const Brain::Impl& pImpl, std::vector<uint8_t>& payload);
    static void prepareRandomStateData(const Brain::Impl& pImpl, std::vector<uint8_t>& payload);
    static void preparePlasticityData(const Brain::Impl& pImpl, std::vector<uint8_t>& payload);
    static void prepareMemoryData(const Brain::Impl& pImpl, std::vector<uint8_t>& payload);
    
    // Save/load operations
    static bool saveBrain(Brain::Impl& pImpl, const std::string& filepath);
    static bool loadBrain(Brain::Impl& pImpl, const std::string& filepath);
    
    // Helper methods
    static void applyRestoredNeuronStates(Brain::Impl& pImpl, const NeuronCheckpointData& neuronData);
    static void applyRestoredSynapseStates(Brain::Impl& pImpl, const SynapseCheckpointData& synapseData);
    static void restoreExtraSections(Brain::Impl& pImpl);
};

} // namespace nlm

#endif // BRAIN_CHECKPOINT_HPP
