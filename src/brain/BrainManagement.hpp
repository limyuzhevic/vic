#pragma once

namespace nlm {

class Brain;

namespace BrainManagement {

void createRegions(Brain& brain, size_t regionCount);
void createNeurons(Brain& brain, size_t neuronCount, size_t regionCount);
void initializeConnectivity(Brain& brain, size_t regionCount, float connectionProbability);
void updateStatistics(Brain& brain);
void collectSensoryMotorNeurons(Brain& brain);

} // namespace BrainManagement

} // namespace nlm