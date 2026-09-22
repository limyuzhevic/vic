#pragma once

namespace nlm {

class Brain;

namespace BrainIntegration {

void registerSpikeHandlers(Brain& brain);
void integrateMemorySystems(Brain& brain, size_t neuronCount);
void integrateCognitionSystems(Brain& brain);
void integrateNeuromodulationSystems(Brain& brain);
void configureCheckpointSystem(Brain& brain);

} // namespace BrainIntegration

} // namespace nlm