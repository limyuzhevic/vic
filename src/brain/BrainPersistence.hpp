#pragma once

namespace nlm {

class Brain;

namespace BrainPersistence {

void setupNeuronCheckpointData(Brain& brain, NeuronCheckpointData& data);
void saveNeurons(Brain& brain, CheckpointWriter& writer);
void saveSynapses(Brain& brain, CheckpointWriter& writer);
void loadNeurons(Brain& brain, CheckpointReader& reader);

} // namespace BrainPersistence

} // namespace nlm