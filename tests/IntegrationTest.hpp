#pragma once

#include <memory>
#include <string>
#include <vector>

namespace nlm {

class Config;
class Brain;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class AttentionalSelection;
class ConceptFormation;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionErrorSignal;
class DevelopmentalStage;
class Action;
class ActionType;
class NeuronId;
class NeuronType;
class EpisodicMemoryItem;

class IntegrationTest {
public:
    IntegrationTest();
    ~IntegrationTest();
    
    bool runAllTests();
    void printTestResults();
    
private:
    bool testMemorySystems();
    bool testPredictionSystem();
    bool testNeuralPlanner();
    bool testNeuromodulationMemoryConnections();
    bool testDevelopmentCognitionConnections();
    bool testCompleteBrainLoop();
    bool testCheckpointPersistence();
    
    std::shared_ptr<Logger> logger_;
};

} // namespace nlm
