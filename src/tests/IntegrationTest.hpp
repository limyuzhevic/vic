#pragma once

#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <fstream>

namespace nlm {

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
