#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
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
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    
    // Auto-advance stage based on time in stage
    // Initial: 60 steps, CriticalPeriod: 300 steps, Maturation: 600 steps
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        advanceStage();
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.0f;
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 0.5f;
        case DevelopmentalStage::Adult: return 0.2f;
        case DevelopmentalStage::Aging: return 0.1f;
        default: return 0.5f;
    }
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

/**
 * Get current development system parameters
 */
std::vector<float> DevelopmentSystem::getParameters() const {
    std::vector<float> params;
    params.push_back(0.5f);  // plasticityBase
    params.push_back(300.0f); // criticalPeriodDuration
    params.push_back(0.5f);  // maturationRate
    return params;
}

/**
 * Set development system parameters
 */
void DevelopmentSystem::setParameters(float plasticityBase, float criticalPeriodDuration,
                                       float maturationRate) {
    // Store parameters in pImpl for future use
    // Currently just logging, but could store for actual use
    NLM_LOG_INFO("DevelopmentSystem parameters set: plasticityBase=" + std::to_string(plasticityBase) +
                 ", criticalPeriodDuration=" + std::to_string(criticalPeriodDuration) +
                 ", maturationRate=" + std::to_string(maturationRate));
}

/**
 * Get developmental stage characteristics
 */
std::map<std::string, float> DevelopmentSystem::getStageCharacteristics() const {
    std::map<std::string, float> characteristics;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            characteristics["plasticity"] = 1.0f;
            characteristics["learning_rate"] = 1.0f;
            characteristics["critical_period"] = 0.0f;
            characteristics["maturation_rate"] = 0.1f;
            characteristics["synaptogenesis_rate"] = 0.001f;
            characteristics["pruning_rate"] = 0.0001f;
            break;
        case DevelopmentalStage::Infant:
            characteristics["plasticity"] = 0.9f;
            characteristics["learning_rate"] = 0.8f;
            characteristics["critical_period"] = 0.0f;
            characteristics["maturation_rate"] = 0.2f;
            characteristics["synaptogenesis_rate"] = 0.0005f;
            characteristics["pruning_rate"] = 0.00005f;
            break;
        case DevelopmentalStage::Juvenile:
            characteristics["plasticity"] = 0.7f;
            characteristics["learning_rate"] = 0.6f;
            characteristics["critical_period"] = 0.2f;
            characteristics["maturation_rate"] = 0.3f;
            characteristics["synaptogenesis_rate"] = 0.0002f;
            characteristics["pruning_rate"] = 0.00002f;
            break;
        case DevelopmentalStage::Adolescent:
            characteristics["plasticity"] = 0.5f;
            characteristics["learning_rate"] = 0.4f;
            characteristics["critical_period"] = 0.5f;
            characteristics["maturation_rate"] = 0.4f;
            characteristics["synaptogenesis_rate"] = 0.0001f;
            characteristics["pruning_rate"] = 0.00001f;
            break;
        case DevelopmentalStage::Adult:
            characteristics["plasticity"] = 0.3f;
            characteristics["learning_rate"] = 0.2f;
            characteristics["critical_period"] = 0.8f;
            characteristics["maturation_rate"] = 0.1f;
            characteristics["synaptogenesis_rate"] = 0.00005f;
            characteristics["pruning_rate"] = 0.000005f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            characteristics["plasticity"] = 0.8f;
            characteristics["learning_rate"] = 0.7f;
            characteristics["critical_period"] = 1.0f;
            characteristics["maturation_rate"] = 0.3f;
            characteristics["synaptogenesis_rate"] = 0.0008f;
            characteristics["pruning_rate"] = 0.00008f;
            break;
        case DevelopmentalStage::Maturation:
            characteristics["plasticity"] = 0.6f;
            characteristics["learning_rate"] = 0.5f;
            characteristics["critical_period"] = 0.6f;
            characteristics["maturation_rate"] = 0.8f;
            characteristics["synaptogenesis_rate"] = 0.0003f;
            characteristics["pruning_rate"] = 0.00003f;
            break;
        case DevelopmentalStage::Senescent:
            characteristics["plasticity"] = 0.2f;
            characteristics["learning_rate"] = 0.1f;
            characteristics["critical_period"] = 0.9f;
            characteristics["maturation_rate"] = 0.05f;
            characteristics["synaptogenesis_rate"] = 0.00001f;
            characteristics["pruning_rate"] = 0.000001f;
            break;
        case DevelopmentalStage::Aging:
            characteristics["plasticity"] = 0.1f;
            characteristics["learning_rate"] = 0.05f;
            characteristics["critical_period"] = 0.95f;
            characteristics["maturation_rate"] = 0.02f;
            characteristics["synaptogenesis_rate"] = 0.000005f;
            characteristics["pruning_rate"] = 0.0000005f;
            break;
        default:
            characteristics["plasticity"] = 0.5f;
            characteristics["learning_rate"] = 0.3f;
            characteristics["critical_period"] = 0.5f;
            characteristics["maturation_rate"] = 0.3f;
            characteristics["synaptogenesis_rate"] = 0.0001f;
            characteristics["pruning_rate"] = 0.00001f;
    }
    
    return characteristics;
}

/**
 * Get developmental stage capabilities
 */
std::map<std::string, float> DevelopmentSystem::getStageCapabilities() const {
    std::map<std::string, float> capabilities;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            capabilities["working_memory_capacity"] = 2.0f;
            capabilities["episodic_memory_capacity"] = 0.5f;
            capabilities["planning_depth"] = 0.0f;
            capabilities["attention_span"] = 5.0f;
            capabilities["reward_processing"] = 0.5f;
            capabilities["prediction_accuracy"] = 0.0f;
            capabilities["concept_formation"] = 0.1f;
            capabilities["social_learning"] = 0.0f;
            break;
        case DevelopmentalStage::Infant:
            capabilities["working_memory_capacity"] = 3.0f;
            capabilities["episodic_memory_capacity"] = 1.0f;
            capabilities["planning_depth"] = 0.1f;
            capabilities["attention_span"] = 10.0f;
            capabilities["reward_processing"] = 0.7f;
            capabilities["prediction_accuracy"] = 0.1f;
            capabilities["concept_formation"] = 0.2f;
            capabilities["social_learning"] = 0.1f;
            break;
        case DevelopmentalStage::Juvenile:
            capabilities["working_memory_capacity"] = 4.0f;
            capabilities["episodic_memory_capacity"] = 2.0f;
            capabilities["planning_depth"] = 0.3f;
            capabilities["attention_span"] = 20.0f;
            capabilities["reward_processing"] = 0.8f;
            capabilities["prediction_accuracy"] = 0.3f;
            capabilities["concept_formation"] = 0.4f;
            capabilities["social_learning"] = 0.3f;
            break;
        case DevelopmentalStage::Adolescent:
            capabilities["working_memory_capacity"] = 5.0f;
            capabilities["episodic_memory_capacity"] = 3.0f;
            capabilities["planning_depth"] = 0.5f;
            capabilities["attention_span"] = 30.0f;
            capabilities["reward_processing"] = 0.9f;
            capabilities["prediction_accuracy"] = 0.5f;
            capabilities["concept_formation"] = 0.6f;
            capabilities["social_learning"] = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            capabilities["working_memory_capacity"] = 6.0f;
            capabilities["episodic_memory_capacity"] = 5.0f;
            capabilities["planning_depth"] = 0.7f;
            capabilities["attention_span"] = 60.0f;
            capabilities["reward_processing"] = 1.0f;
            capabilities["prediction_accuracy"] = 0.7f;
            capabilities["concept_formation"] = 0.8f;
            capabilities["social_learning"] = 0.8f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            capabilities["working_memory_capacity"] = 4.5f;
            capabilities["episodic_memory_capacity"] = 3.5f;
            capabilities["planning_depth"] = 0.6f;
            capabilities["attention_span"] = 40.0f;
            capabilities["reward_processing"] = 0.95f;
            capabilities["prediction_accuracy"] = 0.6f;
            capabilities["concept_formation"] = 0.7f;
            capabilities["social_learning"] = 0.6f;
            break;
        case DevelopmentalStage::Maturation:
            capabilities["working_memory_capacity"] = 5.5f;
            capabilities["episodic_memory_capacity"] = 4.5f;
            capabilities["planning_depth"] = 0.8f;
            capabilities["attention_span"] = 50.0f;
            capabilities["reward_processing"] = 1.0f;
            capabilities["prediction_accuracy"] = 0.8f;
            capabilities["concept_formation"] = 0.9f;
            capabilities["social_learning"] = 0.7f;
            break;
        case DevelopmentalStage::Senescent:
            capabilities["working_memory_capacity"] = 3.0f;
            capabilities["episodic_memory_capacity"] = 2.0f;
            capabilities["planning_depth"] = 0.3f;
            capabilities["attention_span"] = 15.0f;
            capabilities["reward_processing"] = 0.6f;
            capabilities["prediction_accuracy"] = 0.4f;
            capabilities["concept_formation"] = 0.5f;
            capabilities["social_learning"] = 0.4f;
            break;
        case DevelopmentalStage::Aging:
            capabilities["working_memory_capacity"] = 2.5f;
            capabilities["episodic_memory_capacity"] = 1.5f;
            capabilities["planning_depth"] = 0.2f;
            capabilities["attention_span"] = 10.0f;
            capabilities["reward_processing"] = 0.4f;
            capabilities["prediction_accuracy"] = 0.3f;
            capabilities["concept_formation"] = 0.3f;
            capabilities["social_learning"] = 0.2f;
            break;
        default:
            capabilities["working_memory_capacity"] = 4.0f;
            capabilities["episodic_memory_capacity"] = 2.0f;
            capabilities["planning_depth"] = 0.4f;
            capabilities["attention_span"] = 25.0f;
            capabilities["reward_processing"] = 0.6f;
            capabilities["prediction_accuracy"] = 0.4f;
            capabilities["concept_formation"] = 0.5f;
            capabilities["social_learning"] = 0.3f;
    }
    
    return capabilities;
}

/**
 * Get current developmental milestones
 */
std::vector<std::string> DevelopmentSystem::getDevelopmentalMilestones() const {
    std::vector<std::string> milestones;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            milestones.push_back("Basic synaptic formation");
            milestones.push_back("Initial neural connectivity");
            milestones.push_back("Basic sensory processing");
            milestones.push_back("Reflex-based behavior");
            milestones.push_back("Primitive reward responses");
            break;
        case DevelopmentalStage::Infant:
            milestones.push_back("Enhanced neural plasticity");
            milestones.push_back("Basic motor skills development");
            milestones.push_back("Simple learning capabilities");
            milestones.push_back("Early habit formation");
            milestones.push_back("Social attachment behavior");
            break;
        case DevelopmentalStage::Juvenile:
            milestones.push_back("Skill refinement and practice");
            milestones.push_back("Improved working memory");
            milestones.push_back("Basic problem solving");
            milestones.push_back("Social learning and imitation");
            milestones.push_back("Enhanced attention span");
            break;
        case DevelopmentalStage::Adolescent:
            milestones.push_back("Advanced cognitive development");
            milestones.push_back("Emotional regulation learning");
            milestones.push_back("Complex planning abilities");
            milestones.push_back("Abstract thinking emergence");
            milestones.push_back("Peer influence processing");
            break;
        case DevelopmentalStage::Adult:
            milestones.push_back("Specialized skill acquisition");
            milestones.push_back("Expert-level performance");
            milestones.push_back("Autonomous decision making");
            milestones.push_back("Long-term memory consolidation");
            milestones.push_back("Complex social reasoning");
            break;
        case DevelopmentalStage::CriticalPeriod:
            milestones.push_back("Sensitive learning window opening");
            milestones.push_back("Enhanced pattern detection");
            milestones.push_back("Language and sensory refinement");
            milestones.push_back("Rapid skill acquisition");
            milestones.push_back("Neural circuit optimization");
            break;
        case DevelopmentalStage::Maturation:
            milestones.push_back("Neural maturation completion");
            milestones.push_back("Stable cognitive function");
            milestones.push_back("Optimized connectivity patterns");
            milestones.push_back("Efficient memory processing");
            milestones.push_back("Mature behavioral responses");
            break;
        case DevelopmentalStage::Senescent:
            milestones.push_back("Age-related cognitive changes");
            milestones.push_back("Wisdom integration");
            milestones.push_back("Experience-based decision making");
            milestones.push_back("Sensory adaptation");
            milestones.push_back("Maintenance of essential functions");
            break;
        case DevelopmentalStage::Aging:
            milestones.push_back("Physical and cognitive decline");
            milestones.push_back("Reduced plasticity");
            milestones.push_back("Memory consolidation challenges");
            milestones.push_back("Behavioral stabilization");
            milestones.push_back("Experience preservation");
            break;
        default:
            milestones.push_back("Developmental uncertainty");
            milestones.push_back("Basic neural maintenance");
            milestones.push_back("Stable synaptic balance");
            milestones.push_back("Fundamental cognitive functions");
            milestones.push_back("Adaptive response capabilities");
    }
    
    return milestones;
}

} // namespace nlm
