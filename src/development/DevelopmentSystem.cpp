#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Enhanced developmental tracking
    double overallAge;
    double learningRate;
    double memoryConsolidationRate;
    double attentionSpan;
    double processingSpeed;
    
    // Stage-specific parameters
    float criticalPeriodEnd;
    float maturationEnd;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0)
        , overallAge(0.0), learningRate(1.0), memoryConsolidationRate(0.5), attentionSpan(10.0)
        , processingSpeed(1.0), criticalPeriodEnd(300.0), maturationEnd(960.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    pImpl->stepsInCurrentStage = 0;
    NLM_LOG_INFO("Development stage advanced to: " + std::string(getStageName()));
    
    // Update stage-specific parameters when stage changes
    if (stage == DevelopmentalStage::CriticalPeriod) {
        pImpl->criticalPeriodEnd = pImpl->stageAge + 300.0;
    } else if (stage == DevelopmentalStage::Maturation) {
        pImpl->maturationEnd = pImpl->stageAge + 600.0;
    }
}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            pImpl->criticalPeriodEnd = pImpl->stageAge + 300.0;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            pImpl->maturationEnd = pImpl->stageAge + 600.0;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            break;
        case DevelopmentalStage::Aging:
            // Already at final stage
            break;
        default:
            break;
    }
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
}

const char* DevelopmentSystem::getStageName() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    pImpl->stageAge += 0.001;  // Approximate timestep
    pImpl->overallAge += 0.001;
    ++pImpl->stepsInCurrentStage;
    age_ = pImpl->overallAge;
    
    // Update stage-specific parameters
    updateStageParameters(brain, 0.001);
    
    // Auto-advance stage based on time in stage
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    pImpl->overallAge += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    age_ = pImpl->overallAge;
    
    // Update stage-specific parameters
    updateStageParameters(brain, dt);
    
    // Auto-advance stage based on developmental age
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        advanceStage();
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Enhanced plasticity factor that varies by stage and other factors
    float basePlasticity = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: 
            basePlasticity = 1.2f; // Early development is highly plastic
            break;
        case DevelopmentalStage::CriticalPeriod: 
            basePlasticity = 0.9f;
            break;
        case DevelopmentalStage::Maturation: 
            basePlasticity = 0.6f;
            break;
        case DevelopmentalStage::Adult: 
            basePlasticity = 0.3f;
            break;
        case DevelopmentalStage::Aging: 
            basePlasticity = 0.15f; // Lower in old age
            break;
        default: 
            basePlasticity = 0.5f;
    }
    
    // Modulate by overall developmental age
    float ageFactor = 1.0f / (1.0f + age_ * 0.01f); // Older = less plastic
    return basePlasticity * ageFactor;
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
}

void DevelopmentSystem::updateStageParameters(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Update developmental parameters based on stage
    double age = pImpl->overallAge;
    
    // Learning rate decreases with development (becomes more selective)
    pImpl->learningRate = 1.0 / (1.0 + age * 0.005);
    
    // Memory consolidation increases with development
    pImpl->memoryConsolidationRate = std::min(1.0, 0.3 + age * 0.002);
    
    // Attention span develops over time
    pImpl->attentionSpan = 5.0 + age * 0.1;
    
    // Processing speed increases with development
    pImpl->processingSpeed = 0.8 + age * 0.002;
    
    // Stage-specific effects
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High plasticity, broad learning, limited attention span
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period for certain types of learning
            pImpl->learningRate *= 1.5f; // Enhanced learning during critical period
            pImpl->memoryConsolidationRate *= 1.2f;
            break;
            
        case DevelopmentalStage::Maturation:
            // Specialization and refinement
            pImpl->learningRate *= 0.8f;
            pImpl->memoryConsolidationRate *= 1.1f;
            break;
            
        case DevelopmentalStage::Adult:
            // Adult learning with high consolidation
            pImpl->memoryConsolidationRate *= 1.3f;
            break;
            
        case DevelopmentalStage::Aging:
            // Reduced learning, different memory profile
            pImpl->learningRate *= 0.6f;
            pImpl->memoryConsolidationRate *= 0.8f;
            break;
    }
    
    // Connect development to other brain systems
    
    // 1. Neuromodulation - development affects neuromodulator levels and sensitivity
    if (brain->getDopamine()) {
        // Dopamine response matures with development
        float dopamineMaturation = std::min(1.0, age / 300.0);
        // Would apply maturation effects to dopamine system
    }
    
    // 2. Memory systems - development affects consolidation and retrieval
    if (brain->getWorkingMemory()) {
        // Working memory capacity develops with age
        size_t capacity = 100 + static_cast<size_t>(age * 10);
        brain->getWorkingMemory()->setCapacity(std::min(capacity, size_t(1000)));
    }
    
    if (brain->getEpisodicMemory()) {
        // Episodic memory consolidation mechanisms develop
        brain->getEpisodicMemory()->setMaxEpisodes(100 + static_cast<size_t>(age * 5));
    }
    
    // 3. Attention - development improves attentional control
    if (brain->getAttention()) {
        // Attention parameters could be tuned based on developmental stage
        // Stronger inhibition in adult stages, more flexible in early stages
    }
    
    // 4. Prediction system - development refines predictive models
    if (brain->getPredictionSystem()) {
        // Prediction accuracy and confidence increase with development
        // Would update prediction system parameters
    }
    
    // 5. Planning and cognition - development shapes cognitive strategies
    if (brain->getPlanner()) {
        // Planning depth and complexity develops
        int planningDepth = 3 + static_cast<int>(age / 200);
        // Would update planner parameters
    }
    
    // Log developmental progress periodically
    if (static_cast<int>(age_) % 60 == 0 && age_ > 0) {
        NLM_LOG_INFO("Developmental status: Stage=" + std::string(getStageName()) +
                    ", Age=" + std::to_string(age_) +
                    ", LearningRate=" + std::to_string(pImpl->learningRate) +
                    ", MemoryConsolidation=" + std::to_string(pImpl->memoryConsolidationRate));
    }
}

} // namespace nlm
