// Memory system component - handles working memory, episodic memory, and associative memory
#include "Brain.hpp"
#include <memory>

namespace nlm {

class MemorySystem {
public:
    MemorySystem(Brain& brain);
    ~MemorySystem();
    
    // Initialize all memory systems
    bool initialize();
    
    // Update memory systems
    void update(TimestepDuration dt);
    
    // Get memory system status
    size_t getWorkingMemorySize() const;
    size_t getEpisodicMemorySize() const;
    size_t getAssociativeMemorySize() const;
    
    // Store new memories
    void storeObservation(const class Observation& observation);
    void storeAction(const class Action& action);
    
private:
    Brain& brain;
    std::unique_ptr<class NeuralWorkingMemory> workingMemory;
    std::unique_ptr<class NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<class NeuralAssociativeMemory> associativeMemory;
};

} // namespace nlm
