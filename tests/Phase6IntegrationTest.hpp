#pragma once

namespace nlm {

class Config;
class Brain;
class SimpleWorld;
class AgentBrain;
class NeuronType;
class DevelopmentalStage;

} // namespace nlm

namespace test_phase6_integration {

void testPhase6PythonBindings();
void testPhase6AdvancedAPIs();
void testPhase6IntegrationLoop();
void testPhase6ErrorHandling();
void testPhase6Performance();
void testPhase6MemorySystems();
void testPhase6Neuromodulation();
void testPhase6Prediction();
void testPhase6Development();

void runAllPhase6IntegrationTests();

} // namespace test_phase6_integration