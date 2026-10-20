#pragma once

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Checkpoint/CheckpointManager.hpp"
#include <string>

namespace nlm {

class BrainPersistence {
public:
    static bool saveBrainState(const Brain* brain, const std::string& filepath);
    static bool loadBrainState(Brain* brain, const std::string& filepath);
    
private:
    static bool writeNeuronsToCheckpoint(CheckpointWriter& writer, const Brain& brain);
    static bool readNeuronsFromCheckpoint(CheckpointReader& reader, Brain& brain);
    static bool writeSynapsesToCheckpoint(CheckpointWriter& writer, const Brain& brain);
    static bool readSynapsesFromCheckpoint(CheckpointReader& reader, Brain& brain);
};

} // namespace nlm
