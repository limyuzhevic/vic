#pragma once

#include <string>
#include <memory>

namespace nlm {

class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    void runIntegrationTest();
    void runPerformanceTest();
    void runMemoryTest();
    void runNeuromodulationTest();
    void runCheckpointTest();
    void runReplayTest();
    
    std::string getName() const;
    float getPerformanceScore() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

}
