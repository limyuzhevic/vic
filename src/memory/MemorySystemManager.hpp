#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

class WorkingMemoryManager {
public:
    WorkingMemoryManager();
    ~WorkingMemoryManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update working memory
    void update();
    
    // Get working memory
    NeuralWorkingMemory* get() { return workingMemory_.get(); }
    const NeuralWorkingMemory* get() const { return workingMemory_.get(); }
    
    // Configure from config
    void configureFromConfig(const Config& config);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
};

class EpisodicMemoryManager {
public:
    EpisodicMemoryManager();
    ~EpisodicMemoryManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update episodic memory
    void update();
    
    // Get episodic memory
    NeuralEpisodicMemory* get() { return episodicMemory_.get(); }
    const NeuralEpisodicMemory* get() const { return episodicMemory_.get(); }
    
    // Configure from config
    void configureFromConfig(const Config& config);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
};

class AssociativeMemoryManager {
public:
    AssociativeMemoryManager();
    ~AssociativeMemoryManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update associative memory
    void update();
    
    // Get associative memory
    NeuralAssociativeMemory* get() { return associativeMemory_.get(); }
    const NeuralAssociativeMemory* get() const { return associativeMemory_.get(); }
    
    // Configure from config
    void configureFromConfig(const Config& config);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
};

} // namespace nlm