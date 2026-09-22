#pragma once

namespace nlm {

// Forward declarations for all brain phase methods
void processDelayedSpikeEvents(Brain& brain);
void updateNeuronDynamics(Brain& brain);
void detectSpikesAndScheduleEvents(Brain& brain);
void updateWorkingMemory(Brain& brain);
void applyNeuromodulationEffects(Brain& brain);
void applyPlasticityRules(Brain& brain);
void updateEpisodicMemory(Brain& brain);
void updatePredictionSystem(Brain& brain);
void updateAttentionSystem(Brain& brain);
void updateConceptFormation(Brain& brain);
void applyStructuralPlasticity(Brain& brain);
void replayImportantMemories(Brain& brain);
void applyDevelopmentEffects(Brain& brain);
void performMemoryConsolidation(Brain& brain);
void manageCheckpoints(Brain& brain);

} // namespace nlm