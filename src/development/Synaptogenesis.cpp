#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    std::unordered_map<SynapseId, std::chrono::steady_clock::time_point> synapseFormations;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f) {}
    
    ~Impl() {
        synapseFormations.clear();
    }
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    if (rate < 0.0f) throw std::invalid_argument("Formation rate cannot be negative");
    pImpl->formationRate = rate;
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    if (density < 0.0f || density > 1.0f) throw std::invalid_argument("Density must be between 0 and 1");
    pImpl->targetDensity = density;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    // Get current number of synapses and neurons
    size_t currentSynapseCount = 0;
    size_t currentNeuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            currentSynapseCount += pop->getSynapseCount();
            currentNeuronCount += pop->getNeuronCount();
        }
    }
    
    // Calculate current density
    float currentDensity = 0.0f;
    if (currentNeuronCount > 0) {
        currentDensity = static_cast<float>(currentSynapseCount) / (currentNeuronCount * currentNeuronCount);
    }
    
    // If we're below target density, create new synapses
    if (currentDensity < pImpl->targetDensity) {
        size_t neuronsPerRegion = 0;
        std::vector<NeuralRegion*> regions;
        
        for (const auto& region : brain->getRegions()) {
            regions.push_back(region);
            for (auto& pop : region->getPopulations()) {
                neuronsPerRegion += pop->getNeuronCount();
            }
        }
        
        if (neuronsPerRegion > 0 && !regions.empty()) {
            // Determine how many new synapses to create
            size_t targetSynapseCount = static_cast<size_t>(
                neuronsPerRegion * neuronsPerRegion * pImpl->targetDensity
            );
            
            // Each region gets proportional share
            size_t synapsesToCreate = targetSynapseCount - currentSynapseCount;
            
            if (synapsesToCreate > 0) {
                // Create new synapses between randomly selected neuron pairs
                for (size_t i = 0; i < synapsesToCreate; ++i) {
                    // Pick random region
                    size_t regionIndex = rng.uniformInt(0, regions.size() - 1);
                    NeuralRegion* region = regions[regionIndex];
                    
                    // Get all neurons in region
                    std::vector<Neuron*> neurons;
                    for (auto& pop : region->getPopulations()) {
                        for (Neuron* n : pop->getNeurons()) {
                            neurons.push_back(n);
                        }
                    }
                    
                    if (neurons.size() >= 2) {
                        // Select two distinct neurons for new synapse
                        size_t preIdx = rng.uniformInt(0, neurons.size() - 1);
                        size_t postIdx;
                        do {
                            postIdx = rng.uniformInt(0, neurons.size() - 1);
                        } while (postIdx == preIdx);
                        
                        Neuron* preNeuron = neurons[preIdx];
                        Neuron* postNeuron = neurons[postIdx];
                        
                        // Ensure no duplicate synapse already exists
                        bool synapseExists = false;
                        for (const auto& syn : region->getSynapses()) {
                            if ((syn->getPreNeuron() == preNeuron && syn->getPostNeuron() == postNeuron) ||
                                (syn->getPreNeuron() == postNeuron && syn->getPostNeuron() == preNeuron)) {
                                synapseExists = true;
                                break;
                            }
                        }
                        
                        if (!synapseExists) {
                            // Create new synapse with initial weight
                            auto newSynapse = std::make_unique<Synapse>(preNeuron, postNeuron);
                            newSynapse->setWeight(0.5f); // Initial weight
                            newSynapse->enablePlasticity(true, true, false); // Enable Hebbian and STDP
                            
                            // Add to region
                            region->addSynapse(std::move(newSynapse));
                            
                            // Record formation time for potential elimination
                            SynapseId newId = region->getSynapseId(preNeuron, postNeuron);
                            pImpl->synapseFormations[newId] = std::chrono::steady_clock::now();
                        }
                    }
                }
            }
        }
    }
    
    // Clean up old formation records
    auto now = std::chrono::steady_clock::now();
    for (auto it = pImpl->synapseFormations.begin(); it != pImpl->synapseFormations.end(); ) {
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second);
        if (duration.count() > 60) {  // Remove records older than 60 seconds
            it = pImpl->synapseFormations.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace nlm
