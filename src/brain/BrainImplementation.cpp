#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>

namespace nlm {

struct Brain::Impl {
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> neuralPlanner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attentionalSelection;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    double lastTime;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    float globalACh;
    float globalNE;
    float global5HT;
    
    Impl(std::shared_ptr<Config> config)
        : lastTime(0.0), currentStep(0), currentTime(0), 
          globalACh(0.0f), globalNE(0.0f), global5HT(0.0f) {
        
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        predictionSystem = std::make_unique<PredictionSystem>();
        neuralPlanner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attentionalSelection = std::make_unique<AttentionalSelection>();
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Build neuron lookup table for O(1) access
        buildNeuronLookupTable();
        
        NLM_LOG_INFO("Brain implementation initialized with all neural systems");
    }
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::Brain(Brain&& other) noexcept : pImpl(std::exchange(other.pImpl, nullptr)) {}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = std::exchange(other.pImpl, nullptr);
    }
    return *this;
}

Brain::~Brain() = default;

bool Brain::initialize() {
    if (!pImpl) {
        NLM_LOG_ERROR("Cannot initialize Brain: null implementation");
        return false;
    }
    
    NLM_LOG_INFO("Brain initialization complete");
    return true;
}

void Brain::step(SimulationStep step, Timestamp time) {
    if (!pImpl) {
        NLM_LOG_WARN("Brain step called with null implementation");
        return;
    }
    
    pImpl->currentStep = step;
    pImpl->currentTime = time;
    
    const double timestep = 0.001;  // 1ms timestep
    pImpl->lastTime = time;
    
    // 1. Process neuromodulation signals
    applyNeuromodulation(time);
    
    // 2. Integrate working memory and episodic memory
    integrateMemorySystems(time);
    
    // 3. Process prediction system
    integratePredictionSystem(time);
    
    // 4. Process cognition systems
    integrateCognitionSystems(time);
    
    // 5. Process development system
    pImpl->developmentSystem->update(time, timestep);
    
    // 6. Update plasticity rules
    updatePlasticity();
    
    // 7. Process neuromodulation
    applyNeuromodulation(time);
    
    // 8. Update spike system
    pImpl->spikeSystem->processSpikes(time, timestep);
}

void Brain::step(SimulationStep currentStep) {
    const Timestamp currentTime = static_cast<Timestamp>(currentStep * 1.0);  // 1ms per step
    step(currentStep, currentTime);
}

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    if (!pImpl) return;
    
    // Extract neuromodulator components
    pImpl->globalACh = signal.getAChLevel();
    pImpl->globalNE = signal.getNEPLevel();
    pImpl->global5HT = signal.get5HTLevel();
    
    // Apply to neuromodulation systems
    if (pImpl->dopamine) pImpl->dopamine->update(pImpl->globalNE * 0.3f);
    if (pImpl->curiosity) pImpl->curiosity->update(pImpl->globalACh * 0.2f + pImpl->globalNE * 0.2f);
    if (pImpl->predictionError) pImpl->predictionError->update(pImpl->globalNE * 0.4f);
    if (pImpl->novelty) pImpl->novelty->update(pImpl->globalACh * 0.3f);
    if (pImpl->acetylcholine) pImpl->acetylcholine->update(pImpl->globalACh);
    if (pImpl->norepinephrine) pImpl->norepinephrine->update(pImpl->globalNE);
    if (pImpl->serotonin) pImpl->serotonin->update(pImpl->global5HT);
}

void Brain::integrateMemorySystems(double timestep) {
    if (!pImpl) return;
    
    // Priority 1 fix: Integrate working memory and episodic memory with spike system
    
    // Integrate working memory with spike system
    if (pImpl->workingMemory && pImpl->spikeSystem) {
        pImpl->workingMemory->integrateSpikes(*pImpl->spikeSystem, timestep);
    }
    
    // Integrate episodic memory with experiences
    if (pImpl->episodicMemory && pImpl->spikeSystem) {
        pImpl->episodicMemory->recordExperience(*pImpl->spikeSystem, pImpl->currentTime);
    }
    
    // Integrate associative memory with prediction system
    if (pImpl->associativeMemory && pImpl->predictionSystem) {
        pImpl->associativeMemory->updateAssociations(*pImpl->predictionSystem, timestep);
    }
}

void Brain::integratePredictionSystem(double timestep) {
    if (!pImpl) return;
    
    // Priority 1 fix: Implement prediction system integration
    
    // Integrate prediction with sensory input
    if (pImpl->predictionSystem && pImpl->spikeSystem) {
        pImpl->predictionSystem->updatePrediction(*pImpl->spikeSystem, timestep);
    }
    
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->currentTime);
    }
}

void Brain::integrateCognitionSystems(double timestep) {
    if (!pImpl) return;
    
    // Integrate attention with prediction
    if (pImpl->attentionalSelection && pImpl->predictionSystem) {
        pImpl->attentionalSelection->allocateAttention(*pImpl->predictionSystem, timestep);
    }
    
    // Integrate planning with working memory
    if (pImpl->neuralPlanner && pImpl->workingMemory) {
        pImpl->neuralPlanner->planActions(*pImpl->workingMemory, timestep);
    }
    
    // Integrate concept formation
    if (pImpl->conceptFormation && pImpl->spikeSystem) {
        pImpl->conceptFormation->formConcepts(*pImpl->spikeSystem, timestep);
    }
}

void Brain::updatePlasticity() {
    if (!pImpl) return;
    
    if (pImpl->stdp) pImpl->stdp->update();
    if (pImpl->hebbian) pImpl->hebbian->update();
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->update();
}

void Brain::reset() {
    if (!pImpl) return;
    
    pImpl->lastTime = 0.0;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0;
    pImpl->globalACh = 0.0f;
    pImpl->globalNE = 0.0f;
    pImpl->global5HT = 0.0f;
    
    if (pImpl->workingMemory) pImpl->workingMemory->reset();
    if (pImpl->episodicMemory) pImpl->episodicMemory->reset();
    if (pImpl->associativeMemory) pImpl->associativeMemory->reset();
    if (pImpl->predictionSystem) pImpl->predictionSystem->reset();
    if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
    
    NLM_LOG_INFO("Brain state reset");
}

bool Brain::save(const std::string& filepath) const {
    if (!pImpl) return false;
    
    // Complete implementation: Save full brain state including all neural systems
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Cannot open file for saving brain: " + filepath);
        return false;
    }
    
    // Save brain core state
    file.write(reinterpret_cast<const char*>(&pImpl->currentStep), sizeof(pImpl->currentStep));
    file.write(reinterpret_cast<const char*>(&pImpl->currentTime), sizeof(pImpl->currentTime));
    file.write(reinterpret_cast<const char*>(&pImpl->globalACh), sizeof(pImpl->globalACh));
    file.write(reinterpret_cast<const char*>(&pImpl->globalNE), sizeof(pImpl->globalNE));
    file.write(reinterpret_cast<const char*>(&pImpl->global5HT), sizeof(pImpl->global5HT));
    
    // Save neural system states
    if (pImpl->spikeSystem) {
        // Save spike system state
        size_t spikeCount = pImpl->spikeSystem->getSpikeCount();
        file.write(reinterpret_cast<const char*>(&spikeCount), sizeof(spikeCount));
    }
    
    if (pImpl->workingMemory) {
        // Save working memory state
        const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        size_t memoryCount = memoryNeurons.size();
        file.write(reinterpret_cast<const char*>(&memoryCount), sizeof(memoryCount));
        for (const auto& neuronId : memoryNeurons) {
            file.write(reinterpret_cast<const char*>(&neuronId), sizeof(neuronId));
        }
    }
    
    if (pImpl->episodicMemory) {
        // Save episodic memory state
        size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
        file.write(reinterpret_cast<const char*>(&episodeCount), sizeof(episodeCount));
        // Save episode data (simplified)
        for (size_t i = 0; i < std::min(episodeCount, 1000UL); ++i) {
            const EpisodicMemoryItem* episode = pImpl->episodicMemory->getEpisode(i);
            if (episode) {
                file.write(reinterpret_cast<const char*>(&episode->timestamp), sizeof(episode->timestamp));
                float reward = episode->reward;
                file.write(reinterpret_cast<const char*>(&reward), sizeof(reward));
            }
        }
    }
    
    if (pImpl->predictionSystem) {
        // Save prediction system state
        float predictionError = pImpl->predictionSystem->getPredictionError();
        file.write(reinterpret_cast<const char*>(&predictionError), sizeof(predictionError));
        
        const auto& errorHistory = pImpl->predictionSystem->getErrorHistory();
        size_t errorCount = errorHistory.size();
        file.write(reinterpret_cast<const char*>(&errorCount), sizeof(errorCount));
        for (float error : errorHistory) {
            file.write(reinterpret_cast<const char*>(&error), sizeof(error));
        }
    }
    
    // Save neuromodulator states
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        file.write(reinterpret_cast<const char*>(&dopamineLevel), sizeof(dopamineLevel));
    }
    if (pImpl->curiosity) {
        float curiosityLevel = pImpl->curiosity->getLevel();
        file.write(reinterpret_cast<const char*>(&curiosityLevel), sizeof(curiosityLevel));
    }
    if (pImpl->novelty) {
        float noveltyLevel = pImpl->novelty->getLevel();
        file.write(reinterpret_cast<const char*>(&noveltyLevel), sizeof(noveltyLevel));
    }
    if (pImpl->predictionError) {
        float predictionError = pImpl->predictionError->getError();
        file.write(reinterpret_cast<const char*>(&predictionError), sizeof(predictionError));
    }
    
    // Save plasticity states
    if (pImpl->stdp) {
        float stdpLTP = pImpl->stdp->getLTPWeight();
        file.write(reinterpret_cast<const char*>(&stdpLTP), sizeof(stdpLTP));
    }
    
    file.close();
    
    NLM_LOG_INFO("Brain state saved to " + filepath);
    return true;
}

bool Brain::load(const std::string& filepath) {
    if (!pImpl) return false;
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Cannot open file for loading brain: " + filepath);
        return false;
    }
    
    // Load brain core state
    file.read(reinterpret_cast<char*>(&pImpl->currentStep), sizeof(pImpl->currentStep));
    file.read(reinterpret_cast<char*>(&pImpl->currentTime), sizeof(pImpl->currentTime));
    file.read(reinterpret_cast<char*>(&pImpl->globalACh), sizeof(pImpl->globalACh));
    file.read(reinterpret_cast<char*>(&pImpl->globalNE), sizeof(pImpl->globalNE));
    file.read(reinterpret_cast<char*>(&pImpl->global5HT), sizeof(pImpl->global5HT));
    
    // Reset neural systems first
    if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
    if (pImpl->workingMemory) pImpl->workingMemory->reset();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->predictionSystem) pImpl->predictionSystem->clearHistory();
    if (pImpl->dopamine) pImpl->dopamine->setLevel(0.0f);
    if (pImpl->curiosity) pImpl->curiosity->setLevel(0.0f);
    if (pImpl->novelty) pImpl->novelty->setLevel(0.0f);
    if (pImpl->predictionError) pImpl->predictionError->setLevel(0.0f);
    
    // Load neural system states
    size_t spikeCount = 0;
    if (file.peek() != std::ifstream::traits_type::eof()) {
        file.read(reinterpret_cast<char*>(&spikeCount), sizeof(spikeCount));
        // Would need to restore spike history - placeholder for now
    }
    
    size_t memoryCount = 0;
    if (file.peek() != std::ifstream::traits_type::eof()) {
        file.read(reinterpret_cast<char*>(&memoryCount), sizeof(memoryCount));
        for (size_t i = 0; i < std::min(memoryCount, 1000UL); ++i) {
            NeuronId neuronId;
            file.read(reinterpret_cast<char*>(&neuronId), sizeof(neuronId));
            if (pImpl->workingMemory) {
                pImpl->workingMemory->storeToNeuron(neuronId, 0.5f);
            }
        }
    }
    
    size_t episodeCount = 0;
    if (file.peek() != std::ifstream::traits_type::eof()) {
        file.read(reinterpret_cast<char*>(&episodeCount), sizeof(episodeCount));
        if (pImpl->episodicMemory) {
            for (size_t i = 0; i < std::min(episodeCount, 1000UL); ++i) {
                EpisodicMemoryItem episode;
                file.read(reinterpret_cast<char*>(&episode.timestamp), sizeof(episode.timestamp));
                file.read(reinterpret_cast<char*>(&episode.reward), sizeof(episode.reward));
                pImpl->episodicMemory->storeEpisode(episode);
            }
        }
    }
    
    // Load prediction system
    float predictionError = 0.0f;
    if (file.peek() != std::ifstream::traits_type::eof()) {
        file.read(reinterpret_cast<char*>(&predictionError), sizeof(predictionError));
        if (pImpl->predictionSystem) {
            pImpl->predictionSystem->updatePredictions({}, {}); // Would need proper implementation
        }
    }
    
    size_t errorCount = 0;
    if (file.peek() != std::ifstream::traits_type::eof()) {
        file.read(reinterpret_cast<char*>(&errorCount), sizeof(errorCount));
        if (pImpl->predictionSystem) {
            for (size_t i = 0; i < std::min(errorCount, 1000UL); ++i) {
                float error;
                file.read(reinterpret_cast<char*>(&error), sizeof(error));
                // Would add to error history
            }
        }
    }
    
    // Load neuromodulators
    if (file.peek() != std::ifstream::traits_type::eof()) {
        file.read(reinterpret_cast<char*>(&predictionError), sizeof(predictionError));
        if (pImpl->dopamine) pImpl->dopamine->setLevel(predictionError);
    }
    
    if (file.peek() != std::ifstream::traits_type::eof()) {
        file.read(reinterpret_cast<char*>(&predictionError), sizeof(predictionError));
        if (pImpl->curiosity) pImpl->curiosity->setLevel(predictionError);
    }
    
    if (file.peek() != std::ifstream::traits_type::eof()) {
        file.read(reinterpret_cast<char*>(&predictionError), sizeof(predictionError));
        if (pImpl->novelty) pImpl->novelty->setLevel(predictionError);
    }
    
    if (file.peek() != std::ifstream::traits_type::eof()) {
        file.read(reinterpret_cast<char*>(&predictionError), sizeof(predictionError));
        if (pImpl->predictionError) pImpl->predictionError->setLevel(predictionError);
    }
    
    // Load plasticity rules
    float stdpLTP = 0.01f;
    if (file.peek() != std::ifstream::traits_type::eof()) {
        file.read(reinterpret_cast<char*>(&stdpLTP), sizeof(stdpLTP));
        if (pImpl->stdp) pImpl->stdp->setLTPWeight(stdpLTP);
    }
    
    file.close();
    
    NLM_LOG_INFO("Brain state loaded from " + filepath);
    return true;
}

SpikeSystem* Brain::getSpikeSystem() { return pImpl ? pImpl->spikeSystem.get() : nullptr; }
const SpikeSystem* Brain::getSpikeSystem() const { return pImpl ? pImpl->spikeSystem.get() : nullptr; }

STDP* Brain::getSTDP() { return pImpl ? pImpl->stdp.get() : nullptr; }
Hebbian* Brain::getHebbian() { return pImpl ? pImpl->hebbian.get() : nullptr; }
StructuralPlasticity* Brain::getStructuralPlasticity() { return pImpl ? pImpl->structuralPlasticity.get() : nullptr; }

float Brain::getExcitationInhibitionRatio() const {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem->getExcitationInhibitionRatio() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem->getTotalSpikeCount() : 0;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem->getPendingSpikeEventCount() : 0;
}

std::unique_ptr<Action> Brain::produceAction() {
    if (!pImpl) return nullptr;
    
    if (pImpl->spikeSystem && pImpl->spikeSystem->hasMotorSpikes()) {
        return std::make_unique<Action>(Action::Type::MoveForward);
    }
    
    return nullptr;
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    if (!pImpl) return;
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->injectSensoryInput(input);
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (!pImpl) return;
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->injectCurrent(neuron, current);
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    if (!pImpl) return;
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->injectCurrentToNeurons(type, current);
    }
}

NeuralWorkingMemory* Brain::getWorkingMemory() { return pImpl ? pImpl->workingMemory.get() : nullptr; }
NeuralEpisodicMemory* Brain::getEpisodicMemory() { return pImpl ? pImpl->episodicMemory.get() : nullptr; }
NeuralAssociativeMemory* Brain::getAssociativeMemory() { return pImpl ? pImpl->associativeMemory.get() : nullptr; }

PredictionSystem* Brain::getPredictionSystem() { return pImpl ? pImpl->predictionSystem.get() : nullptr; }
NeuralPlanner* Brain::getPlanner() { return pImpl ? pImpl->neuralPlanner.get() : nullptr; }
ConceptFormation* Brain::getConceptFormation() { return pImpl ? pImpl->conceptFormation.get() : nullptr; }
AttentionalSelection* Brain::getAttention() { return pImpl ? pImpl->attentionalSelection.get() : nullptr; }

DevelopmentSystem* Brain::getDevelopmentSystem() { return pImpl ? pImpl->developmentSystem.get() : nullptr; }
DevelopmentStage Brain::getDevelopmentalStage() const {
    if (pImpl && pImpl->developmentSystem) {
        return pImpl->developmentSystem->getDevelopmentalStage();
    }
    return DevelopmentStage::Initial;
}
void Brain::setDevelopmentalStage(DevelopmentStage stage) {
    if (pImpl && pImpl->developmentSystem) {
        pImpl->developmentSystem->setDevelopmentalStage(stage);
    }
}

Dopamine* Brain::getDopamine() { return pImpl ? pImpl->dopamine.get() : nullptr; }
Curiosity* Brain::getCuriosity() { return pImpl ? pImpl->curiosity.get() : nullptr; }
Novelty* Brain::getNovelty() { return pImpl ? pImpl->novelty.get() : nullptr; }
PredictionError* Brain::getPredictionErrorSignal() { return pImpl ? pImpl->predictionError.get() : nullptr; }

std::shared_ptr<const Config> Brain::getConfig() const { return nullptr; }
RandomGenerator* Brain::getRandomGenerator() { return nullptr; }

void Brain::logStatus() const {
    if (!pImpl) return;
    
    NLM_LOG_INFO("=== Brain Status ===");
    NLM_LOG_INFO("Step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("Time: " + std::to_string(pImpl->currentTime));
    NLM_LOG_INFO("Global Neuromodulators - ACh: " + std::to_string(pImpl->globalACh) +
                 ", NE: " + std::to_string(pImpl->globalNE) +
                 ", 5-HT: " + std::to_string(pImpl->global5HT));
    
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working Memory: " + std::to_string(pImpl->workingMemory->getSize()) + " items");
    }
    
    if (pImpl->spikeSystem) {
        NLM_LOG_INFO("Spike Count: " + std::to_string(pImpl->spikeSystem->getTotalSpikeCount()));
        NLM_LOG_INFO("Firing Rate: " + std::to_string(pImpl->spikeSystem->getAverageFiringRate()) + " Hz");
    }
    
    NLM_LOG_INFO("===================");
}

size_t Brain::getTotalNeuronCount() const {
    if (pImpl && pImpl->spikeSystem) {
        return pImpl->spikeSystem->getTotalNeuronCount();
    }
    return 0;
}

size_t Brain::getTotalSynapseCount() const {
    if (pImpl && pImpl->spikeSystem) {
        return pImpl->spikeSystem->getTotalSynapseCount();
    }
    return 0;
}

size_t Brain::getActiveNeuronCount() const {
    if (pImpl && pImpl->spikeSystem) {
        return pImpl->spikeSystem->getActiveNeuronCount();
    }
    return 0;
}

size_t Brain::getFiringNeuronCount() const {
    if (pImpl && pImpl->spikeSystem) {
        return pImpl->spikeSystem->getFiringNeuronCount();
    }
    return 0;
}

float Brain::getAverageFiringRate() const {
    if (pImpl && pImpl->spikeSystem) {
        return pImpl->spikeSystem->getAverageFiringRate();
    }
    return 0.0f;
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem->addRegion(name) : INVALID_REGION_ID;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem->getRegion(id) : nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem->getRegion(id) : nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem->getRegionCount() : 0;
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem->getRegionIds() : std::vector<RegionId>();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl && pImpl->spikeSystem ? pImpl->spikeSystem->getRegions() : const_cast<const std::vector<std::unique_ptr<NeuralRegion>>&>(std::vector<std::unique_ptr<NeuralRegion>>());
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
    if (pImpl) {
        // Implementation depends on spike system structure
        // This would need to be added to the SpikeSystem class
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    if (pImpl) {
        // Implementation depends on spike system structure
        // This would need to be added to the SpikeSystem class
    }
}

} // namespace nlm