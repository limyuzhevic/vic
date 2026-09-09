#include "MemorySystemManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// WorkingMemoryManager implementation
struct WorkingMemoryManager::Impl {
    Brain* brain;
};

WorkingMemoryManager::WorkingMemoryManager() {
    pImpl = std::make_unique<Impl>();
    workingMemory_ = std::make_unique<NeuralWorkingMemory>();
}

WorkingMemoryManager::~WorkingMemoryManager() = default;

void WorkingMemoryManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize WorkingMemoryManager: invalid brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    workingMemory_->initialize(pImpl->brain);
    
    NLM_LOG_INFO("Working memory system initialized");
}

void WorkingMemoryManager::update() {
    if (workingMemory_) {
        workingMemory_->update(0.001f);  // TODO: Get actual timestep from brain
    }
}

void WorkingMemoryManager::configureFromConfig(const Config& config) {
    // Configure working memory parameters
    // TODO: Add working memory specific configuration options
    
    NLM_LOG_INFO("Working memory system configured from config");
}

void WorkingMemoryManager::reset() {
    if (workingMemory_) {
        workingMemory_->clear();
    }
    NLM_LOG_INFO("Working memory system reset");
}

// EpisodicMemoryManager implementation
struct EpisodicMemoryManager::Impl {
    Brain* brain;
};

EpisodicMemoryManager::EpisodicMemoryManager() {
    pImpl = std::make_unique<Impl>();
    episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
}

EpisodicMemoryManager::~EpisodicMemoryManager() = default;

void EpisodicMemoryManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize EpisodicMemoryManager: invalid brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    episodicMemory_->initialize(pImpl->brain);
    
    NLM_LOG_INFO("Episodic memory system initialized");
}

void EpisodicMemoryManager::update() {
    // Episodic memory is updated during episodic events
    // This is called during brain steps
    if (episodicMemory_) {
        // TODO: Implement episodic memory update
    }
}

void EpisodicMemoryManager::configureFromConfig(const Config& config) {
    // Configure episodic memory parameters
    // TODO: Add episodic memory specific configuration options
    
    NLM_LOG_INFO("Episodic memory system configured from config");
}

void EpisodicMemoryManager::reset() {
    if (episodicMemory_) {
        episodicMemory_->clear();
    }
    NLM_LOG_INFO("Episodic memory system reset");
}

// AssociativeMemoryManager implementation
struct AssociativeMemoryManager::Impl {
    Brain* brain;
};

AssociativeMemoryManager::AssociativeMemoryManager() {
    pImpl = std::make_unique<Impl>();
    associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
}

AssociativeMemoryManager::~AssociativeMemoryManager() = default;

void AssociativeMemoryManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize AssociativeMemoryManager: invalid brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    associativeMemory_->initialize(pImpl->brain);
    
    NLM_LOG_INFO("Associative memory system initialized");
}

void AssociativeMemoryManager::update() {
    // Associative memory is updated during learning events
    // This is called during brain steps
    if (associativeMemory_) {
        // TODO: Implement associative memory update
    }
}

void AssociativeMemoryManager::configureFromConfig(const Config& config) {
    // Configure associative memory parameters
    // TODO: Add associative memory specific configuration options
    
    NLM_LOG_INFO("Associative memory system configured from config");
}

void AssociativeMemoryManager::reset() {
    if (associativeMemory_) {
        associativeMemory_->clear();
    }
    NLM_LOG_INFO("Associative memory system reset");
}

} // namespace nlm