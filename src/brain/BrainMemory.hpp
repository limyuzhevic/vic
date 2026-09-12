// Brain memory implementation - handles all memory system components
#include "Brain.hpp"
#include "MemorySystem.hpp"
#include <memory>

namespace nlm {

class BrainMemory : public Brain {
public:
    BrainMemory(std::shared_ptr<Config> config);
    ~BrainMemory() override;
    
    // Memory system integration
    bool initializeMemorySystems();
    void updateMemorySystems(TimestepDuration dt);
    
    // Override brain methods
    bool initialize() override;
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    // Memory system accessors
    NeuralWorkingMemory* getWorkingMemory() override;
    NeuralEpisodicMemory* getEpisodicMemory() override;
    NeuralAssociativeMemory* getAssociativeMemory() override;
    
private:
    std::unique_ptr<MemorySystem> memorySystem;
};

} // namespace nlm
