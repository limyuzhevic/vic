#include "EnhancedDevelopmentSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../development/Maturation.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Pruning.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct EnhancedDevelopmentSystem::Impl {
    // Core development components
    std::unique_ptr<DevelopmentalStageManager> stageManager;
    std::unique_ptr<NeuralArchitectureEvolution> architectureEvolution;
    std::unique_ptr<CognitiveDevelopment> cognitiveDev;
    std::unique_ptr<BehavioralDevelopment> behavioralDev;
    std::unique_ptr<ExperienceDependentDevelopment> experienceDev;
    std::unique_ptr<DevelopmentalMilestones> milestones;
    
    // Sub-components
    std::unique_ptr<Maturation> maturation;
    std::unique_ptr<Synaptogenesis> synaptogenesis;
    std::unique_ptr<Pruning> pruning;
    
    // Brain reference
    Brain* brain;
    
    // Developmental metrics
    DevelopmentalMetrics metrics;
    CognitiveCapabilities capabilities;
    BehavioralProfile behaviorProfile;
    std::vector<NeuralRegionSpecialization> specializationProfile;
    
    // State tracking
    double developmentalAge;
    size_t stepsSinceLastStageAdvance;
    
    Impl() : brain(nullptr), developmentalAge(0.0), stepsSinceLastStageAdvance(0) {}
};

EnhancedDevelopmentSystem::EnhancedDevelopmentSystem() : pImpl(new Impl) {}

EnhancedDevelopmentSystem::~EnhancedDevelopmentSystem() = default;

void EnhancedDevelopmentSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize all development components
    pImpl->stageManager = std::make_unique<DevelopmentalStageManager>();
    pImpl->architectureEvolution = std::make_unique<NeuralArchitectureEvolution>();
    pImpl->cognitiveDev = std::make_unique<CognitiveDevelopment>();
    pImpl->behavioralDev = std::make_unique<BehavioralDevelopment>();
    pImpl->experienceDev = std::make_unique<ExperienceDependentDevelopment>();
    pImpl->milestones = std::make_unique<DevelopmentalMilestones>();
    
    pImpl->maturation = std::make_unique<Maturation>();
    pImpl->synaptogenesis = std::make_unique<Synaptogenesis>();
    pImpl->pruning = std::make_unique<Pruning>();
    
    // Initialize milestones
    pImpl->milestones->initializeMilestones(brain);
    
    NLM_LOG_INFO("Enhanced DevelopmentSystem initialized with all developmental components");
}

void EnhancedDevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt, SimulationStep currentStep) {
    pImpl->developmentalAge += dt;
    pImpl->stepsSinceLastStageAdvance++;
    
    // Update stage manager
    pImpl->stageManager->update(brain, pImpl->developmentalAge, currentStep);
    
    // Update architecture evolution
    pImpl->architectureEvolution->update(brain, rng, pImpl->developmentalAge, dt);
    
    // Update cognitive development
    pImpl->cognitiveDev->update(brain, pImpl->developmentalAge, dt);
    
    // Update behavioral development
    pImpl->behavioralDev->update(brain, rng, pImpl->developmentalAge, dt);
    
    // Update experience-dependent development
    pImpl->experienceDev->update(brain, rng, pImpl->developmentalAge, dt);
    
    // Update milestones
    pImpl->milestones->update(brain, pImpl->developmentalAge, dt);
    
    // Update maturation
    pImpl->maturation->update(brain, currentStep);
    
    // Update synaptogenesis (stage-specific rate)
    float synaptogenesisRate = getSynaptogenesisRate();
    pImpl->synaptogenesis->setFormationRate(synaptogenesisRate);
    pImpl->synaptogenesis->update(brain, rng);
    
    // Update pruning (stage-specific rate)
    float pruningRate = getPruningRate();
    pImpl->pruning->setPruningRate(pruningRate);
    pImpl->pruning->update(brain, rng);
    
    // Update metrics
    updateMetrics(brain);
}

DevelopmentalStage EnhancedDevelopmentSystem::getStage() const {
    return pImpl->stageManager->getCurrentStage();
}

const char* EnhancedDevelopmentSystem::getStageName() const {
    return pImpl->stageManager->getStageName();
}

const char* EnhancedDevelopmentSystem::getStageDescription() const {
    return pImpl->stageManager->getStageDescription();
}

double EnhancedDevelopmentSystem::getDevelopmentalAge() const {
    return pImpl->developmentalAge;
}

DevelopmentalMetrics EnhancedDevelopmentSystem::getMetrics() const {
    return pImpl->metrics;
}

CognitiveCapabilities EnhancedDevelopmentSystem::getCognitiveCapabilities() const {
    return pImpl->capabilities;
}

BehavioralProfile EnhancedDevelopmentSystem::getBehavioralProfile() const {
    return pImpl->behaviorProfile;
}

std::vector<NeuralRegionSpecialization> EnhancedDevelopmentSystem::getSpecializationProfile() const {
    return pImpl->specializationProfile;
}

bool EnhancedDevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stageManager->isCriticalPeriod();
}

float EnhancedDevelopmentSystem::getCriticalPeriodProgress() const {
    return pImpl->stageManager->getCriticalPeriodProgress();
}

float EnhancedDevelopmentSystem::getPlasticityModifier() const {
    return pImpl->stageManager->getStagePlasticity();
}

float EnhancedDevelopmentSystem::getLearningRate() const {
    return pImpl->stageManager->getStageLearningRate();
}

float EnhancedDevelopmentSystem::getSynaptogenesisRate() const {
    return pImpl->stageManager->getStageSynapticDensity();
}

float EnhancedDevelopmentSystem::getPruningRate() const {
    return pImpl->stageManager->getStageCriticalPeriod();
}

void EnhancedDevelopmentSystem::advanceStage() {
    pImpl->stageManager->advanceStage();
}

void EnhancedDevelopmentSystem::recordExperience(Brain* brain, float reward, double timestamp) {
    pImpl->experienceDev->recordLearningExperience(brain, reward, timestamp);
}

bool EnhancedDevelopmentSystem::isMilestoneAchieved(const char* milestoneName) const {
    return pImpl->milestones->isMilestoneAchieved(milestoneName);
}

std::vector<const char*> EnhancedDevelopmentSystem::getNextMilestones() const {
    return pImpl->milestones->getNextMilestones();
}

void EnhancedDevelopmentSystem::updateMetrics(Brain* brain) {
    // Calculate comprehensive developmental metrics
    pImpl->metrics.synapticDensity = 0.0f;
    pImpl->metrics.connectivityDepth = 0.0f;
    pImpl->metrics.regionalSpecialization = 0.0f;
    
    // Update cognitive capabilities
    pImpl->capabilities = pImpl->cognitiveDev->getCapabilities();
    
    // Update behavioral profile
    pImpl->behaviorProfile = pImpl->behavioralDev->getProfile();
    
    // Update specialization profile
    pImpl->specializationProfile = pImpl->architectureEvolution->getSpecializationProfile();
    
    // Calculate integration metrics based on system performance
    if (brain) {
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t activeNeurons = brain->getActiveNeuronCount();
        
        if (totalNeurons > 0) {
            pImpl->metrics.synapticDensity = static_cast<float>(activeNeurons) / totalNeurons;
        }
        
        // Update working memory capacity based on cognitive development
        pImpl->metrics.workingMemoryCapacity = pImpl->capabilities.workingMemoryCapacity;
        
        // Update planning complexity
        pImpl->metrics.planningComplexity = pImpl->capabilities.planningDepth;
        
        // Update exploration behavior
        pImpl->metrics.exploratoryBehavior = pImpl->behaviorProfile.curiosityDrive;
    }
}

} // namespace nlm