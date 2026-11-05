// BrainCheckpoint.hpp - Checkpoint saving/loading
#pragma once

#include "../performance/CheckpointSystem.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <string>

namespace nlm {

class Brain;
class Neuron;
class Synapse;

class BrainCheckpoint {
public:
    static bool save(Brain& brain, const std::string& filepath);
    static bool load(Brain& brain, const std::string& filepath);
    static void saveSimulationState(Brain& brain, CheckpointWriter& writer);
    static void loadSimulationState(Brain& brain, CheckpointReader& reader);
    static void saveRandomState(Brain& brain, CheckpointWriter& writer);
    static void loadRandomState(Brain& brain, CheckpointReader& reader);
    static void savePlasticityState(Brain& brain, CheckpointWriter& writer);
    static void loadPlasticityState(Brain& brain, CheckpointReader& reader);
    static void saveMemoryState(Brain& brain, CheckpointWriter& writer);
    static void loadMemoryState(Brain& brain, CheckpointReader& reader);
    static void setupCheckpointManager(Brain& brain);
    static bool validateCheckpoint(Brain& brain, const std::string& filepath);
    static bool verifyCheckpointIntegrity(Brain& brain, const std::string& filepath);
    static void cleanupCheckpoint(Brain& brain, const std::string& filepath);
    static std::vector<std::string> getCheckpointFiles(Brain& brain);
    static size_t getCheckpointCount(Brain& brain);
    static void setCheckpointRetention(Brain& brain, size_t maxCheckpoints);
    static void setCheckpointCompression(Brain& brain, CompressionLevel level);
    static void setCheckpointEncryption(Brain& brain, bool enabled);
    static std::string getCheckpointMetadata(Brain& brain, const std::string& filepath);
    static bool updateCheckpoint(Brain& brain, const std::string& filepath);
    static bool createCheckpointDirectory(Brain& brain, const std::string& directory);

private:
    static void writeNeuronState(Brain& brain, CheckpointWriter& writer);
    static void writeSynapseState(Brain& brain, CheckpointWriter& writer);
    static void writeDelayedSpikeEvents(Brain& brain, CheckpointWriter& writer);
    static void readNeuronState(Brain& brain, CheckpointReader& reader);
    static void readSynapseState(Brain& brain, CheckpointReader& reader);
    static void readDelayedSpikeEvents(Brain& brain, CheckpointReader& reader);
    static void writePhase5ExtraSections(Brain& brain, CheckpointWriter& writer);
    static void readPhase5ExtraSections(Brain& brain, CheckpointReader& reader);
    static bool p5AppendU64(std::vector<uint8_t>& out, uint64_t v);
    static bool p5AppendFloat(std::vector<uint8_t>& out, float v);
    static bool p5AppendDouble(std::vector<uint8_t>& out, double v);
    static template <typename T> bool p5AppendVec(std::vector<uint8_t>& out, const std::vector<T>& v);
};
