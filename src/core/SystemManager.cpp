// System manager implementations
#include "SystemManager.hpp"
#include "IBrainInterface.hpp"
#include "../core/EventSystem.hpp"
#include <iostream>

namespace nlm {

MemorySystemManager::MemorySystemManager() : active_(false) {}
MemorySystemManager::~MemorySystemManager() = default;

void MemorySystemManager::initialize(IBrainInterface* brain) {
    if (!brain) {
        std::cerr << "ERROR: Cannot initialize MemorySystemManager - null brain interface" << std::endl;
        return;
    }
    
    std::cout << "MemorySystemManager: Initializing memory systems..." << std::endl;
    
    // Initialize working memory
    workingMemory_ = std::make_unique<NeuralWorkingMemory>();
    workingMemory_->initialize(brain);
    
    // Initialize episodic memory
    episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
    episodicMemory_->initialize(brain);
    
    // Initialize associative memory
    associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
    associativeMemory_->initialize(brain);
    
    active_ = true;
    std::cout << "MemorySystemManager: Memory systems initialized successfully" << std::endl;
}

void MemorySystemManager::update(double timestep) {
    if (!active_) return;
    
    // Update working memory
    if (workingMemory_) {
        workingMemory_->update(timestep);
    }
    
    // Update episodic memory (store episodes periodically)
    if (episodicMemory_) {
        episodicMemory_->update(timestep);
    }
    
    // Update associative memory
    if (associativeMemory_) {
        associativeMemory_->update(timestep);
    }
}

void MemorySystemManager::reset() {
    if (workingMemory_) workingMemory_->clear();
    if (episodicMemory_) episodicMemory_->clear();
    if (associativeMemory_) associativeMemory_->clear();
    active_ = false;
}

bool MemorySystemManager::isActive() const {
    return active_ && workingMemory_ && episodicMemory_ && associativeMemory_;
}

NeuromodulationSystemManager::NeuromodulationSystemManager() : active_(false) {}
NeuromodulationSystemManager::~NeuromodulationSystemManager() = default;

void NeuromodulationSystemManager::initialize(IBrainInterface* brain) {
    if (!brain) {
        std::cerr << "ERROR: Cannot initialize NeuromodulationSystemManager - null brain interface" << std::endl;
        return;
    }
    
    std::cout << "NeuromodulationSystemManager: Initializing neuromodulation systems..." << std::endl;
    
    // Initialize neuromodulation systems
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    predictionError_ = std::make_unique<PredictionError>();
    
    // Initialize each neuromodulator with brain interface
    if (dopamine_) dopamine_->initialize(brain);
    if (curiosity_) curiosity_->initialize(brain);
    if (novelty_) novelty_->initialize(brain);
    if (predictionError_) predictionError_->initialize(brain);
    
    active_ = true;
    std::cout << "NeuromodulationSystemManager: Neuromodulation systems initialized successfully" << std::endl;
}

void NeuromodulationSystemManager::update(double timestep) {
    if (!active_) return;
    
    // Update each neuromodulator
    if (dopamine_) dopamine_->update(timestep);
    if (curiosity_) curiosity_->update(timestep);
    if (novelty_) novelty_->update(timestep);
    if (predictionError_) predictionError_->update(timestep);
}

void NeuromodulationSystemManager::reset() {
    if (dopamine_) dopamine_->reset();
    if (curiosity_) curiosity_->reset();
    if (novelty_) novelty_->reset();
    if (predictionError_) predictionError_->reset();
    active_ = false;
}

bool NeuromodulationSystemManager::isActive() const {
    return active_ && dopamine_ && curiosity_ && novelty_ && predictionError_;
}

CognitionSystemManager::CognitionSystemManager() : active_(false) {}
CognitionSystemManager::~CognitionSystemManager() = default;

void CognitionSystemManager::initialize(IBrainInterface* brain) {
    if (!brain) {
        std::cerr << "ERROR: Cannot initialize CognitionSystemManager - null brain interface" << std::endl;
        return;
    }
    
    std::cout << "CognitionSystemManager: Initializing cognition systems..." << std::endl;
    
    // Initialize cognition systems
    planner_ = std::make_unique<NeuralPlanner>();
    conceptFormation_ = std::make_unique<ConceptFormation>();
    attention_ = std::make_unique<AttentionalSelection>();
    
    // Initialize each cognition system with brain interface
    if (planner_) planner_->initialize(brain);
    if (conceptFormation_) conceptFormation_->initialize(brain);
    if (attention_) attention_->initialize(brain);
    
    active_ = true;
    std::cout << "CognitionSystemManager: Cognition systems initialized successfully" << std::endl;
}

void CognitionSystemManager::update(double timestep) {
    if (!active_) return;
    
    // Update cognition systems
    if (planner_) planner_->update(timestep);
    if (conceptFormation_) conceptFormation_->update(timestep);
    if (attention_) attention_->update(timestep);
}

void CognitionSystemManager::reset() {
    if (planner_) planner_->reset();
    if (conceptFormation_) conceptFormation_->reset();
    if (attention_) attention_->reset();
    active_ = false;
}

bool CognitionSystemManager::isActive() const {
    return active_ && planner_ && conceptFormation_ && attention_;
}

PlasticitySystemManager::PlasticitySystemManager() : active_(false) {}
PlasticitySystemManager::~PlasticitySystemManager() = default;

void PlasticitySystemManager::initialize(IBrainInterface* brain) {
    if (!brain) {
        std::cerr << "ERROR: Cannot initialize PlasticitySystemManager - null brain interface" << std::endl;
        return;
    }
    
    std::cout << "PlasticitySystemManager: Initializing plasticity systems..." << std::endl;
    
    // Initialize plasticity systems
    stdp_ = std::make_unique<STDP>();
    hebbian_ = std::make_unique<Hebbian>();
    structuralPlasticity_ = std::make_unique<StructuralPlasticity>();
    
    // Initialize each plasticity system
    if (stdp_) stdp_->initialize(brain);
    if (hebbian_) hebbian_->initialize(brain);
    if (structuralPlasticity_) structuralPlasticity_->initialize(brain);
    
    active_ = true;
    std::cout << "PlasticitySystemManager: Plasticity systems initialized successfully" << std::endl;
}

void PlasticitySystemManager::update(double timestep) {
    if (!active_) return;
    
    // Update plasticity systems
    if (stdp_) stdp_->update(timestep);
    if (hebbian_) hebbian_->update(timestep);
    if (structuralPlasticity_) structuralPlasticity_->update(timestep);
}

void PlasticitySystemManager::reset() {
    if (stdp_) stdp_->reset();
    if (hebbian_) hebbian_->reset();
    if (structuralPlasticity_) structuralPlasticity_->reset();
    active_ = false;
}

bool PlasticitySystemManager::isActive() const {
    return active_ && stdp_ && hebbian_ && structuralPlasticity_;
}

DevelopmentSystemManager::DevelopmentSystemManager() : active_(false) {}
DevelopmentSystemManager::~DevelopmentSystemManager() = default;

void DevelopmentSystemManager::initialize(IBrainInterface* brain) {
    if (!brain) {
        std::cerr << "ERROR: Cannot initialize DevelopmentSystemManager - null brain interface" << std::endl;
        return;
    }
    
    std::cout << "DevelopmentSystemManager: Initializing development system..." << std::endl;
    
    // Initialize development system
    developmentSystem_ = std::make_unique<DevelopmentSystem>();
    developmentSystem_->initialize(brain);
    
    active_ = true;
    std::cout << "DevelopmentSystemManager: Development system initialized successfully" << std::endl;
}

void DevelopmentSystemManager::update(double timestep) {
    if (!active_) return;
    
    if (developmentSystem_) {
        developmentSystem_->update(timestep);
    }
}

void DevelopmentSystemManager::reset() {
    if (developmentSystem_) developmentSystem_->reset();
    active_ = false;
}

bool DevelopmentSystemManager::isActive() const {
    return active_ && developmentSystem_;
}

} // namespace nlm
