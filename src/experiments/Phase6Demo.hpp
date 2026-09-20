// Phase 6 Demo - Integration Test
// 
// This demo runs the Phase 6 integration experiment to verify
// that all brain systems are properly connected.

#pragma once

#include <memory>

namespace nlm {

class Phase6IntegratedExperiment;

class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    int run(int argc, char* argv[]);
    
private:
    void printHelp();
    
    std::unique_ptr<Phase6IntegratedExperiment> experiment_;
};

} // namespace nlm