#pragma once

namespace nlm {

class Brain;

namespace BrainInitialization {

void initialize(Brain& brain);
void initializeRandomGenerator(Brain& brain);
void initializePlasticitySystems(Brain& brain);
void initializeMemorySystems(Brain& brain);
void initializePredictionSystem(Brain& brain);
void initializeCognitionSystems(Brain& brain);
void initializeDevelopmentSystem(Brain& brain);
void initializeNeuromodulationSystems(Brain& brain);
void configurePlasticity(Brain& brain);
void configureSimulationParameters(Brain& brain);
void initializeCheckpointManager(Brain& brain);
void configureCheckpointManager(Brain& brain);

} // namespace BrainInitialization

} // namespace nlm