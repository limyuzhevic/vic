#include "NeuralRegion.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct NeuralRegion::Impl {
    RegionId id;
    std::string name;
    std::vector<std::unique_ptr<NeuralPopulation>> populations;
    std::vector<std::unique_ptr<Synapse>> synapses;
    std::unordered_map<NeuronId, std::vector<SynapseId>> outgoingSynapses;  // source -> synapse ids
    std::unordered_map<NeuronId, std::vector<SynapseId>> incomingSynapses;  // dest -> synapse ids
    SynapseId nextSynapseId;
    
    explicit Impl(RegionId id) : id(id), nextSynapseId(1) {}
};

NeuralRegion::NeuralRegion(RegionId id) : pImpl(new Impl(id)) {}

NeuralRegion::NeuralRegion(RegionId id, const std::string& name) : pImpl(new Impl(id)) {
    pImpl->name = name;
}

NeuralRegion::~NeuralRegion() = default;

NeuralRegion::NeuralRegion(NeuralRegion&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

NeuralRegion& NeuralRegion::operator=(NeuralRegion&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

RegionId NeuralRegion::getId() const {
    return pImpl->id;
}

const std::string& NeuralRegion::getName() const {
    return pImpl->name;
}

void NeuralRegion::setName(const std::string& name) {
    pImpl->name = name;
}

PopulationId NeuralRegion::addPopulation(size_t size, NeuronType type) {
    PopulationId popId(pImpl->populations.size() + 1);
    auto population = std::make_unique<NeuralPopulation>(popId, size);
    population->setNeuronType(type);
    pImpl->populations.push_back(std::move(population));
    return popId;
}

NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) {
    if (id.index() == 0 || id.index() > pImpl->populations.size()) {
        return nullptr;
    }
    return pImpl->populations[id.index() - 1].get();
}

const NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) const {
    if (id.index() == 0 || id.index() > pImpl->populations.size()) {
        return nullptr;
    }
    return pImpl->populations[id.index() - 1].get();
}

size_t NeuralRegion::getPopulationCount() const {
    return pImpl->populations.size();
}

const std::vector<std::unique_ptr<NeuralPopulation>>& NeuralRegion::getPopulations() const {
    return pImpl->populations;
}

std::vector<NeuralPopulation*> NeuralRegion::getAllPopulations() {
    std::vector<NeuralPopulation*> result;
    result.reserve(pImpl->populations.size());
    for (auto& pop : pImpl->populations) {
        result.push_back(pop.get());
    }
    return result;
}

SynapseId NeuralRegion::addSynapse(NeuronId source, NeuronId destination,
                                   SynapticWeight weight, Delay delay) {
    SynapseId synId(pImpl->nextSynapseId++);
    auto synapse = std::make_unique<Synapse>(synId, source, destination);
    synapse->setWeight(weight);
    synapse->setDelay(delay);
    
    pImpl->outgoingSynapses[source].push_back(synId);
    pImpl->incomingSynapses[destination].push_back(synId);
    
    pImpl->synapses.push_back(std::move(synapse));
    return synId;
}

Synapse* NeuralRegion::getSynapse(SynapseId id) {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

const Synapse* NeuralRegion::getSynapse(SynapseId id) const {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

size_t NeuralRegion::getSynapseCount() const {
    return pImpl->synapses.size();
}

const std::vector<std::unique_ptr<Synapse>>& NeuralRegion::getSynapses() const {
    return pImpl->synapses;
}

std::vector<Synapse*> NeuralRegion::getSynapsesFrom(NeuronId neuron) {
    std::vector<Synapse*> result;
    auto it = pImpl->outgoingSynapses.find(neuron);
    if (it != pImpl->outgoingSynapses.end()) {
        for (SynapseId synId : it->second) {
            if (auto* syn = getSynapse(synId)) {
                result.push_back(syn);
            }
        }
    }
    return result;
}

std::vector<Synapse*> NeuralRegion::getSynapsesTo(NeuronId neuron) {
    std::vector<Synapse*> result;
    auto it = pImpl->incomingSynapses.find(neuron);
    if (it != pImpl->incomingSynapses.end()) {
        for (SynapseId synId : it->second) {
            if (auto* syn = getSynapse(synId)) {
                result.push_back(syn);
            }
        }
    }
    return result;
}

size_t NeuralRegion::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& pop : pImpl->populations) {
        total += pop->getSize();
    }
    return total;
}

size_t NeuralRegion::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& pop : pImpl->populations) {
        total += pop->getActiveCount();
    }
    return total;
}

size_t NeuralRegion::getFiringNeuronCount() const {
    size_t total = 0;
    for (const auto& pop : pImpl->populations) {
        total += pop->getFiringCount();
    }
    return total;
}

float NeuralRegion::getAverageFiringRate() const {
    size_t popCount = pImpl->populations.size();
    if (popCount == 0) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& pop : pImpl->populations) {
        sum += pop->getAverageFiringRate();
    }
    return sum / static_cast<float>(popCount);
}

float NeuralRegion::getAverageSynapticWeight() const {
    if (pImpl->synapses.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (const auto& syn : pImpl->synapses) {
        sum += syn->getWeight();
    }
    return sum / static_cast<float>(pImpl->synapses.size());
}

float NeuralRegion::getSynapticDensity() const {
    size_t neuronCount = getTotalNeuronCount();
    if (neuronCount == 0) return 0.0f;
    
    // Maximum possible connections: N * (N-1) for directed graph
    size_t maxConnections = neuronCount * (neuronCount - 1);
    if (maxConnections == 0) return 0.0f;
    
    return static_cast<float>(pImpl->synapses.size()) / static_cast<float>(maxConnections);
}

void NeuralRegion::step(Timestamp currentTime) {
    // Step all populations
    for (auto& pop : pImpl->populations) {
        pop->step(currentTime);
    }
    
    // Step all synapses
    for (auto& syn : pImpl->synapses) {
        syn->step(currentTime);
    }
}

void NeuralRegion::reset() {
    for (auto& pop : pImpl->populations) {
        pop->reset();
    }
    for (auto& syn : pImpl->synapses) {
        syn->reset();
    }
}

void NeuralRegion::initializeRandomConnectivity(RandomGenerator& rng,
                                               float connectionProbability,
                                               float meanWeight,
                                               float weightVariance) {
    // Efficient random connectivity initialization
    // Creates synapses based on neuron types:
    // - Excitatory neurons -> all neurons (excitatory synapses)
    // - Inhibitory neurons -> all neurons (inhibitory synapses)
    
    // Get all neurons
    auto neurons = getAllNeurons();
    size_t neuronCount = neurons.size();
    
    // Create random connections
    for (size_t i = 0; i < neuronCount; ++i) {
        Neuron* preNeuron = neurons[i];
        NeuronType preType = preNeuron->getType();
        
        // Skip if not a proper source neuron
        if (preType == NeuronType::Modulatory) continue;
        
        for (size_t j = 0; j < neuronCount; ++j) {
            if (i == j) continue;  // No self-connections
            
            Neuron* postNeuron = neurons[j];
            
            // Probabilistic connection
            if (!rng.bernoulli(connectionProbability)) continue;
            
            // Determine synapse type based on pre-synaptic neuron type
            SynapseType synType;
            float weight;
            
            if (preType == NeuronType::Excitatory || 
                preType == NeuronType::Sensory ||
                preType == NeuronType::Motor ||
                preType == NeuronType::Internal) {
                // excitatory synapse
                synType = SynapseType::Excitatory;
                weight = meanWeight + rng.normal(0.0f, weightVariance);
                weight = std::max(0.01f, weight);  // Ensure positive
            } else if (preType == NeuronType::Inhibitory) {
                // inhibitory synapse
                synType = SynapseType::Inhibitory;
                weight = -(meanWeight + rng.normal(0.0f, weightVariance));
                weight = std::min(-0.01f, weight);  // Ensure negative
            } else {
                synType = SynapseType::Excitatory;
                weight = meanWeight + rng.normal(0.0f, weightVariance);
            }
            
            // Add synapse
            SynapseId synId = addSynapse(preNeuron->getId(), postNeuron->getId(), weight, 1);
            
            // Initialize random properties
            if (Synapse* syn = getSynapse(synId)) {
                syn->setType(synType);
                syn->initializeRandom(rng);
            }
        }
    }
}

}

// Phase 2: Region-level dynamics implementation

void NeuralRegion::updateRegionDynamics(float timestep) {
    // Update region-wide dynamics based on neural activity
    // This represents phase 2 implementation that was in the header
    
    // Calculate global region activation from all neurons
    float globalActivation = 0.0f;
    size_t neuronCount = 0;
    
    for (auto& pop : pImpl->populations) {
        for (Neuron* neuron : pop->getNeurons()) {
            const auto& state = neuron->getState();
            if (state.firingState == FiringState::Refractory || 
                (state.firingState == FiringState::Active && 
                 std::abs(state.membranePotential - state.restingPotential) > 5.0f)) {
                globalActivation += std::abs(state.membranePotential - state.restingPotential) / 20.0f;
            }
            neuronCount++;
        }
    }
    
    if (neuronCount > 0) {
        globalActivation /= static_cast<float>(neuronCount);
    }
    
    // Update region synaptic dynamics based on activation
    for (auto& syn : pImpl->synapses) {
        // Adaptive synaptic modification based on regional activity
        float activityFactor = globalActivation * 0.1f;
        float weightChange = activityFactor * (1.0f - std::abs(syn->getWeight())) * 0.001f;
        syn->addToWeight(weightChange);
    }
}

void NeuralRegion::applyNeuromodulation(float dopamine, float serotonin, float acetylcholine) {
    // Apply neuromodulation effects on all neurons in region
    for (auto& pop : pImpl->populations) {
        for (Neuron* neuron : pop->getNeurons()) {
            // Dopamine increases excitability
            if (dopamine > 0.0f) {
                neuron->injectCurrent(dopamine * 0.5f);
            }
            
            // Serotonin modulates firing thresholds
            if (serotonin != 0.0f) {
                float thresholdAdjustment = serotonin * 0.2f;  // Increase threshold
                neuron->setThreshold(neuron->getThreshold() + thresholdAdjustment);
            }
            
            // Acetylcholine enhances synaptic plasticity
            if (acetylcholine > 0.0f) {
                auto& flags = neuron->getPlasticityFlags();
                flags.stdp = true;  // Enhance STDP
                flags.hebbian = true;  // Enhance Hebbian
            }
        }
    }
}

float NeuralRegion::getRegionActivation() const {
    // Calculate region activation as average membrane potential deviation from rest
    float totalActivation = 0.0f;
    size_t activeCount = 0;
    
    for (const auto& pop : pImpl->populations) {
        for (const Neuron* neuron : pop->getNeurons()) {
            const auto& state = neuron->getState();
            if (state.firingState == FiringState::Refractory || 
                (state.firingState == FiringState::Active && 
                 std::abs(state.membranePotential - state.restingPotential) > 5.0f)) {
                totalActivation += std::abs(state.membranePotential - state.restingPotential) / 20.0f;
                activeCount++;
            }
        }
    }
    
    if (activeCount == 0) return 0.0f;
    return totalActivation / static_cast<float>(activeCount);
}

void NeuralRegion::setRegionActivation(float activation) {
    // Set target activation and adjust global neuronal parameters
    // This could be used for targeted region activation in experiments
    activation = std::clamp(activation, 0.0f, 1.0f);
    
    // If activation is high, increase excitability across the region
    if (activation > 0.5f) {
        for (auto& pop : pImpl->populations) {
            for (Neuron* neuron : pop->getNeurons()) {
                // Increase leak conductance to make neurons more excitable
                neuron->setLeakConductance(neuron->getLeakConductance() + activation * 5.0f);
                // Decrease threshold to promote firing
                neuron->setThreshold(neuron->getThreshold() - activation * 10.0f);
            }
        }
    }
}

void NeuralRegion::integrateWithGlobalPrediction(const std::vector<float>& prediction) {
    // Integrate with global prediction system (Phase 2 feature)
    // This allows regions to participate in global predictive coding
    
    if (prediction.empty()) return;
    
    // Use prediction to modulate regional connectivity
    size_t predictionIndex = 0;
    for (auto& syn : pImpl->synapses) {
        if (predictionIndex < prediction.size()) {
            // Prediction-driven synaptic modification
            float predEffect = prediction[predictionIndex] * 0.01f;
            
            // Apply prediction to synaptic weight with temporal decay
            float weight = syn->getWeight();
            weight += predEffect * (1.0f - std::abs(weight)) * 0.005f;
            syn->setWeight(weight);
            
            predictionIndex++;
        }
    }
}

void NeuralRegion::applyDevelopment(float plasticityModifier, DevelopmentalStage stage) {
    // Apply developmental effects to the region
    // This represents phase 2 developmental implementation
    
    for (auto& pop : pImpl->populations) {
        for (Neuron* neuron : pop->getNeurons()) {
            switch (stage) {
                case DevelopmentalStage::Initial:
                    // High plasticity, rapid synaptogenesis
                    neuron->setRefractoryPeriod(static_cast<uint32_t>(5.0f / plasticityModifier));
                    break;
                    
                case DevelopmentalStage::CriticalPeriod:
                    // Moderate plasticity, specialization begins
                    neuron->setRefractoryPeriod(static_cast<uint32_t>(10.0f * plasticityModifier));
                    break;
                    
                case DevelopmentalStage::Maturation:
                    // Lower plasticity, stabilization
                    neuron->setRefractoryPeriod(static_cast<uint32_t>(15.0f * plasticityModifier));
                    break;
                    
                case DevelopmentalStage::Adult:
                    // Low plasticity, homeostatic maintenance
                    neuron->setRefractoryPeriod(static_cast<uint32_t>(20.0f * plasticityModifier));
                    break;
                    
                case DevelopmentalStage::Aging:
                    // Further reduction in plasticity
                    neuron->setRefractoryPeriod(static_cast<uint32_t>(25.0f * plasticityModifier));
                    break;
            }
        }
    }
}
