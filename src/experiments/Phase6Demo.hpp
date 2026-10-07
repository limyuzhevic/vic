#ifndef NLM_PHASE6_DEMO_HPP
#define NLM_PHASE6_DEMO_HPP

#include <memory>
#include <string>

namespace nlm {

class Brain;
class Phase6Demo {
public:
    explicit Phase6Demo(std::shared_ptr<Brain> brain);
    
    bool run(int steps = 1000);
    
    // Run verification tests for each subsystem
    void runIntegrationTests();
    void runMemoryTests();
    void runNeuromodulationTests();
    void runPredictionTests();
    void runDevelopmentTests();
    
    // Get statistics
    double getTotalReward() const { return totalReward; }
    double getAverageFiringRate() const { return avgFiringRate; }
    int getStepsCompleted() const { return stepsCompleted; }
    
private:
    std::shared_ptr<Brain> brain;
    int stepsCompleted;
    double totalReward;
    double avgFiringRate;
    
    void logProgress(int step, double currentReward);
};

} // namespace nlm

#endif // NLM_PHASE6_DEMO_HPP