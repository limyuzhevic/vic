#pragma once

namespace nlm {

class Brain;

namespace BrainAccessors {

void registerSensoryInputHandler(Brain& brain);
void registerDelayedInputHandler(Brain& brain);
void processMotorNeuronGroup(Brain& brain, float& firingMotor);
void updateAttentionForWorkingMemory(Brain& brain);
void configureDopaminePlasticity(Brain& brain, float plasticityMod);

} // namespace BrainAccessors

} // namespace nlm