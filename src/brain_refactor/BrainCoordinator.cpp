#include "BrainCoordinator.hpp"
#include "Brain.hpp"
#include "NeuralComputationSystem.hpp"
#include "PlasticityManager.hpp"
#include "MemorySystems.hpp"
#include "CognitionSystem.hpp"
#include "DevelopmentSystem.hpp"
#include "NeuromodulationSystem.hpp"
#include "EnvironmentInterface.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>

namespace nlm {

struct BrainCoordinator::Impl {
    std::shared_ptr<Config> config;
    Brain* brain;
    
    // Subsystem instances
    std::unique_ptr<NeuralComputationSystem> neuralComputation;
    std::unique_ptr<PlasticityManager> plasticity;
    std::unique_ptr<MemorySystems> memory;
    std::unique_ptr<CognitionSystem> cognition;
    std::unique_ptr<DevelopmentSystem> development;
    std::unique_ptr<NeuromodulationSystem> neuromodulation;
    std::unique_ptr<EnvironmentInterface> environment;
    
    // Progress tracking
    float progress;
    ProgressCallback progressCallback;
    
    // Timing
    SimulationStep currentStep;
    Timestamp currentTime;
    TimestepDuration timestep;
    
    // Statistics
    size_t initializationSteps;
    size_t totalSteps;
};

BrainCoordinator::BrainCoordinator(std::shared_ptr<Config> config)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->progress = 0.0f;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0f;
    pImpl->timestep = 0.001f;
    pImpl->initializationSteps = 0;
    pImpl->totalSteps = 0;
}

BrainCoordinator::~BrainCoordinator() = default;

bool BrainCoordinator::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->initializationSteps = 0;
    
    // Create and initialize all subsystems
    pImpl->neuralComputation = std::make_unique<NeuralComputationSystem>();
    pImpl->plasticity = std::make_unique<PlasticityManager>();
    pImpl->memory = std::make_unique<MemorySystems>();
    pImpl->cognition = std::make_unique<CognitionSystem>();
    pImpl->development = std::make_unique<DevelopmentSystem>();
    pImpl->neuromodulation = std::make_unique<NeuromodulationSystem>();
    pImpl->environment = std::make_unique<EnvironmentInterface>();
    
    const size_t totalSubsystems = 7;
    
    // Initialize each subsystem
    if (pImpl->neuralComputation && pImpl->neuralComputation->initialize(brain)) {
        pImpl->initializationSteps++;
    }
    if (pImpl->plasticity && pImpl->plasticity->initialize(brain)) {
        pImpl->initializationSteps++;
    }
    if (pImpl->memory && pImpl->memory->initialize(brain)) {
        pImpl->initializationSteps++;
    }
    if (pImpl->cognition && pImpl->cognition->initialize(brain)) {
        pImpl->initializationSteps++;
    }
    if (pImpl->development && pImpl->development->initialize(brain)) {
        pImpl->initializationSteps++;
    }
    if (pImpl->neuromodulation && pImpl->neuromodulation->initialize(brain)) {
        pImpl->initializationSteps++;
    }
    if (pImpl->environment && pImpl->environment->initialize(brain)) {
        pImpl->initializationSteps++;
    }
    
    // Update progress
    pImpl->progress = static_cast<float>(pImpl->initializationSteps) / totalSubsystems;
    if (pImpl->progressCallback) {
        pImpl->progressCallback(pImpl->progress);
    }
    
    return pImpl->initializationSteps == totalSubsystems;
}

void BrainCoordinator::step(SimulationStep step, Timestamp time) {
    pImpl->currentStep = step;
    pImpl->currentTime = time;
    pImpl->totalSteps++;
    
    // Update progress (0.0 to 0.7 for initialization phase)
    pImpl->progress = static_cast<float>(pImpl->initializationSteps) / 7.0f;
    if (pImpl->progressCallback) {
        pImpl->progressCallback(pImpl->progress);
    }
    
    // Process each subsystem in order
    // 1. Neural computation (core processing)
    if (pImpl->neuralComputation) {
        pImpl->neuralComputation->update(pImpl->timestep);
    }
    
    // 2. Plasticity updates (learning from spikes)
    if (pImpl->plasticity) {
        pImpl->plasticity->update(pImpl->timestep);
    }
    
    // 3. Memory systems (storing and maintaining memories)
    if (pImpl->memory) {
        pImpl->memory->update(pImpl->timestep);
    }
    
    // 4. Cognition (planning, prediction, attention)
    if (pImpl->cognition) {
        pImpl->cognition->update(pImpl->timestep);
    }
    
    // 5. Development (maturation, structural changes)
    if (pImpl->development) {
        pImpl->development->update(pImpl->timestep);
    }
    
    // 6. Neuromodulation (dopamine, curiosity, prediction error)
    if (pImpl->neuromodulation) {
        pImpl->neuromodulation->update(pImpl->timestep);
    }
    
    // 7. Environment interaction (sensory input, motor output)
    if (pImpl->environment) {
        pImpl->environment->update(pImpl->timestep);
    }
    
    // Update progress (0.7 to 1.0 for subsystem interaction phase)
    pImpl->progress = 0.7f + static_cast<float>(pImpl->totalSteps % 10) / 30.0f;
    if (pImpl->progressCallback) {
        pImpl->progressCallback(pImpl->progress);
    }
}

NeuralComputationSystem* BrainCoordinator::getNeuralComputation() const {
    return pImpl->neuralComputation.get();
}

PlasticityManager* BrainCoordinator::getPlasticity() const {
    return pImpl->plasticity.get();
}

MemorySystems* BrainCoordinator::getMemory() const {
    return pImpl->memory.get();
}

CognitionSystem* BrainCoordinator::getCognition() const {
    return pImpl->cognition.get();
}

DevelopmentSystem* BrainCoordinator::getDevelopment() const {
    return pImpl->development.get();
}

NeuromodulationSystem* BrainCoordinator::getNeuromodulation() const {
    return pImpl->neuromodulation.get();
}

EnvironmentInterface* BrainCoordinator::getEnvironment() const {
    return pImpl->environment.get();
}

void BrainCoordinator::setProgressCallback(ProgressCallback callback) {
    pImpl->progressCallback = std::move(callback);
}

void BrainCoordinator::reset() {
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0f;
    pImpl->progress = 0.0f;
    
    if (pImpl->neuralComputation) pImpl->neuralComputation->reset();
    if (pImpl->plasticity) pImpl->plasticity->reset();
    if (pImpl->memory) pImpl->memory->reset();
    if (pImpl->cognition) pImpl->cognition->reset();
    if (pImpl->development) pImpl->development->reset();
    if (pImpl->neuromodulation) pImpl->neuromodulation->reset();
    if (pImpl->environment) pImpl->environment->reset();
}

void BrainCoordinator::logStatus() const {
    NLM_LOG_INFO("=== BrainCoordinator Status ===");
    NLM_LOG_INFO("Current step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("Current time: " + std::to_string(pImpl->currentTime) + " s");
    NLM_LOG_INFO("Progress: " + std::to_string(pImpl->progress * 100.0f) + "%");
    NLM_LOG_INFO("Total steps executed: " + std::to_string(pImpl->totalSteps));
    
    if (pImpl->neuralComputation) pImpl->neuralComputation->logStatus();
    if (pImpl->plasticity) pImpl->plasticity->logStatus();
    if (pImpl->memory) pImpl->memory->logStatus();
    if (pImpl->cognition) pImpl->cognition->logStatus();
    if (pImpl->development) pImpl->development->logStatus();
    if (pImpl->neuromodulation) pImpl->neuromodulation->logStatus();
    if (pImpl->environment) pImpl->environment->logStatus();
}

bool BrainCoordinator::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving BrainCoordinator state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Save metadata
        writer.setMetadata(
            "brain_coordinator",
            pImpl->totalSteps,
            pImpl->currentStep,
            pImpl->currentTime,
            pImpl->progress
        );
        
        // Save progress
        writer.writeFloat("progress", pImpl->progress);
        writer.writeUint32("total_steps", pImpl->totalSteps);
        
        // Save subsystem states (simplified - in a real implementation,
        // each subsystem would have its own checkpoint format)
        writer.writeString("neural_computation", "initialized");
        writer.writeString("plasticity", "initialized");
        writer.writeString("memory", "initialized");
        writer.writeString("cognition", "initialized");
        writer.writeString("development", "initialized");
        writer.writeString("neuromodulation", "initialized");
        writer.writeString("environment", "initialized");
        
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("BrainCoordinator state saved successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving BrainCoordinator: ") + e.what());
        return false;
    }
}

bool BrainCoordinator::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading BrainCoordinator state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read basic metadata
        pImpl->totalSteps = reader.readUint32("total_steps");
        pImpl->progress = reader.readFloat("progress");
        pImpl->currentStep = reader.readUint64("current_step");
        pImpl->currentTime = reader.readFloat("current_time");
        
        // Load subsystem states (simplified - in a real implementation,
        // each subsystem would restore its own state)
        std::string neuralStatus = reader.readString("neural_computation");
        std::string plasticityStatus = reader.readString("plasticity");
        std::string memoryStatus = reader.readString("memory");
        std::string cognitionStatus = reader.readString("cognition");
        std::string developmentStatus = reader.readString("development");
        std::string neuromodulationStatus = reader.readString("neuromodulation");
        std::string environmentStatus = reader.readString("environment");
        
        // Reset subsystems with loaded state
        reset();
        
        NLM_LOG_INFO("BrainCoordinator state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading BrainCoordinator: ") + e.what());
        return false;
    }
}

} // namespace nlm
