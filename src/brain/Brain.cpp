#include "Brain.hpp"

namespace nlm {

Brain::Brain(std::shared_ptr<Config> config)
    : brainCore_(std::make_unique<BrainCore>(config))
    , brainMemory_(std::make_unique<BrainMemory>())
    , brainNeuromodulation_(std::make_unique<BrainNeuromodulation>())
    , brainPlasticity_(std::make_unique<BrainPlasticity>())
    , brainDevelopment_(std::make_unique<BrainDevelopment>())
    , brainCognition_(std::make_unique<BrainCognition>())
    , brainSimulation_(std::make_unique<BrainSimulation>())
    , brainCheckpoint_(std::make_unique<BrainCheckpoint>()) {
    // Initialize subsystems reference to this brain instance
    brainMemory_->setBrainReference(this);
    brainNeuromodulation_->setBrainReference(this);
    brainPlasticity_->setBrainReference(this);
    brainNeuromodulation_->setBrainReference(this);
    brainDevelopment_->setBrainReference(this);
    brainCognition_->setBrainReference(this);
    brainSimulation_->setBrainReference(this);
    brainCheckpoint_->setBrainReference(this);
    
    // Set cross-subsystem references
    brainMemory_->setNeuromodulationReference(brainNeuromodulation_.get());
    brainNeuromodulation_->setMemoryReference(brainMemory_.get());
    brainPlasticity_->setNeuromodulationReference(brainNeuromodulation_.get());
    brainDevelopment_->setPlasticityReference(brainPlasticity_.get());
    brainCognition_->setMemoryReference(brainMemory_.get());
    brainCognition_->setNeuromodulationReference(brainNeuromodulation_.get());
    brainSimulation_->setMemoryReference(brainMemory_.get());
    brainSimulation_->setNeuromodulationReference(brainNeuromodulation_.get());
    brainSimulation_->setPlasticityReference(brainPlasticity_.get());
    brainSimulation_->setDevelopmentReference(brainDevelopment_.get());
    brainSimulation_->setCognitionReference(brainCognition_.get());
    brainCheckpoint_->setBrainReference(this);
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept
    : brainCore_(std::move(other.brainCore_))
    , brainMemory_(std::move(other.brainMemory_))
    , brainNeuromodulation_(std::move(other.brainNeuromodulation_))
    , brainPlasticity_(std::move(other.brainPlasticity_))
    , brainDevelopment_(std::move(other.brainDevelopment_))
    , brainCognition_(std::move(other.brainCognition_))
    , brainSimulation_(std::move(other.brainSimulation_))
    , brainCheckpoint_(std::move(other.brainCheckpoint_)) {
    // Subsystems reference this brain instance through their setBrainReference methods
    brainMemory_->setBrainReference(this);
    brainNeuromodulation_->setBrainReference(this);
    brainPlasticity_->setBrainReference(this);
    brainDevelopment_->setBrainReference(this);
    brainCognition_->setBrainReference(this);
    brainSimulation_->setBrainReference(this);
    brainCheckpoint_->setBrainReference(this);
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        brainCore_ = std::move(other.brainCore_);
        brainMemory_ = std::move(other.brainMemory_);
        brainNeuromodulation_ = std::move(other.brainNeuromodulation_);
        brainPlasticity_ = std::move(other.brainPlasticity_);
        brainDevelopment_ = std::move(other.brainDevelopment_);
        brainCognition_ = std::move(other.brainCognition_);
        brainSimulation_ = std::move(other.brainSimulation_);
        brainCheckpoint_ = std::move(other.brainCheckpoint_);
        
        // Update subsystem references
        brainMemory_->setBrainReference(this);
        brainNeuromodulation_->setBrainReference(this);
        brainPlasticity_->setBrainReference(this);
        brainDevelopment_->setBrainReference(this);
        brainCognition_->setBrainReference(this);
        brainSimulation_->setBrainReference(this);
        brainCheckpoint_->setBrainReference(this);
    }
    return *this;
}

bool Brain::initialize() {
    // Initialize core brain first
    if (!brainCore_->initialize()) {
        return false;
    }
    
    // Initialize all subsystems
    brainMemory_->initialize(this);
    brainNeuromodulation_->initialize(this);
    brainPlasticity_->initialize(this);
    brainDevelopment_->initialize(this);
    brainCognition_->initialize(this);
    brainSimulation_->initialize(this);
    brainCheckpoint_->initialize(this);
    
    return true;
}

void Brain::reset() {
    // Reset all subsystems
    brainCore_->reset();
    brainMemory_->clearEpisodicMemory();
    brainNeuromodulation_->update(0.0);
    brainPlasticity_->update(0, 0.0);
    brainDevelopment_->update(0, 0.0);
    brainCognition_->update(0.0);
    brainSimulation_->reset();
    // Checkpoints are automatically handled by the checkpoint system
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * 0.001);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    brainSimulation_->updateStep(currentStep, 0.001);
}

// Forward all method calls to the appropriate subsystem
// (This is a simplified version - in practice, you might want to use delegation patterns)

// Basic accessors

// Forward method implementations
// Brain::getTotalNeuronCount() const { return brainCore_->getTotalNeuronCount(); }
// (Continue with all other forward declarations)

} // namespace nlm
