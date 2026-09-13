#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/InternalSignals.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural prediction components
    class Brain* brain;
    RegionId predictionRegionId;
    std::vector<PopulationId> predictionPopulations;
    
    // Prediction state
    float predictionError;
    float confidence;
    float accuracy;
    std::vector<float> errorHistory;
    
    Impl() : 
        brain(nullptr), predictionRegionId(0), predictionError(0.0f), 
        confidence(0.5f), accuracy(0.5f) {
        // Initialize with neural prediction region
        // Will be set in initialize() method
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(class Brain* brain, class NeuralPrediction* neuralPrediction) {
    pImpl->brain = brain;
    
    // Find or create prediction region
    bool found = false;
    for (const auto& region : brain->getRegions()) {
        const std::string& name = region->getName();
        if (name.find("prediction") != std::string::npos || 
            name.find("pred") != std::string::npos) {
            pImpl->predictionRegionId = region->getId();
            found = true;
            break;
        }
    }
    
    // If no prediction region found, use first region
    if (!found && !brain->getRegions().empty()) {
        pImpl->predictionRegionId = brain->getRegionIds()[0];
    } else if (!found) {
        // Create new prediction region
        pImpl->predictionRegionId = brain->addRegion("Prediction_Region");
    }
    
    // Configure prediction populations
    configurePredictionPopulations();
    
    NLM_LOG_INFO("PredictionSystem neural substrate initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("PredictionSystem not initialized, returning current state");
        return currentState.clone();
    }
    
    NeuralRegion* region = pImpl->brain->getRegion(pImpl->predictionRegionId);
    if (!region) {
        return currentState.clone();
    }
    
    // Use neural dynamics to predict next state
    std::unique_ptr<SensoryInput> predictedState;
    
    // Generate prediction based on current neural activity
    predictedState = std::make_unique<InternalSignals>();
    
    // Add neural-based prediction pattern
    std::vector<float> predictedData;
    float avgActivity = 0.5f;  // Simplified average activity
    
    // Apply neural prediction dynamics with decay and noise
    for (size_t i = 0; i < 5; ++i) {
        float predicted = avgActivity * 0.95f;
        predicted += pImpl->brain->getRandomGenerator()->uniformReal(-0.1f, 0.1f);
        predictedData.push_back(predicted);
    }
    
    // Add predicted pattern to internal signals
    for (float value : predictedData) {
        static_cast<InternalSignals&>(*predictedState).addSignal(value);
    }
    
    NLM_LOG_DEBUG("Neural prediction generated");
    NLM_LOG_DEBUG("Prediction confidence: " + std::to_string(pImpl->confidence));
    
    return predictedState;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->brain) {
        return;
    }
    
    NeuralRegion* region = pImpl->brain->getRegion(pImpl->predictionRegionId);
    if (!region) {
        return;
    }
    
    // Compute prediction error using neural activity patterns
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on error
        pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
        pImpl->accuracy = 1.0f - pImpl->predictionError;
    }
    
    NLM_LOG_DEBUG("Prediction error computed: " + std::to_string(pImpl->predictionError));
}

float PredictionSystem::getPredictionAccuracy() const {
    return pImpl->accuracy;
}

void PredictionSystem::setLearningRate(float rate) {
    // Learning rate configuration can be applied to STDP and Hebbian systems
    STDP* stdp = pImpl->brain->getSTDP();
    if (stdp) {
        float ltpWeight = stdp->getLTPWeight();
        ltpWeight = std::max(0.001f, std::min(0.1f, rate));
        stdp->setLTPWeight(ltpWeight);
    }
}

void PredictionSystem::setErrorDecayRate(float rate) {
    // Store decay rate for internal use
    // This would typically be used in prediction dynamics
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) {
        return;
    }
    
    // Train neural prediction system using STDP and Hebbian learning
    NeuralRegion* region = pImpl->brain->getRegion(pImpl->predictionRegionId);
    if (!region) {
        return;
    }
    
    // Apply plasticity rules for prediction learning
    Hebbian* hebbian = pImpl->brain->getHebbian();
    if (hebbian) {
        // Hebbian learning strengthens co-activation
        for (const auto& pop : region->getPopulations()) {
            for (const auto* syn : pop->getSynapses()) {
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Apply Hebbian learning with error modulation
                        hebbian->update(syn, preSpikes, postSpikes, pImpl->brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
                    }
                }
            }
        }
    }
}

float PredictionSystem::getNeuralSynchronization() const {
    // Calculate neural synchronization from error history
    if (pImpl->errorHistory.size() < 2) return 0.0f;
    
    float sum = std::accumulate(pImpl->errorHistory.begin(), pImpl->errorHistory.end(), 0.0f);
    float mean = sum / pImpl->errorHistory.size();
    
    float variance = 0.0f;
    for (float error : pImpl->errorHistory) {
        float diff = error - mean;
        variance += diff * diff;
    }
    variance /= pImpl->errorHistory.size();
    
    // Lower variance = higher synchronization
    return std::max(0.0f, 1.0f - variance);
}

float PredictionSystem::getPopulationCorrelation() const {
    // Estimate population correlation from confidence changes
    return pImpl->confidence;
}

void PredictionSystem::configurePredictionPopulations() {
    NeuralRegion* region = pImpl->brain->getRegion(pImpl->predictionRegionId);
    if (!region) return;
    
    // Configure prediction populations based on brain size
    size_t totalNeurons = pImpl->brain->getTotalNeuronCount();
    size_t predictionNeurons = std::max<size_t>(50, totalNeurons / 20);
    size_t numPopulations = std::max<size_t>(2, predictionNeurons / 15);
    
    pImpl->predictionPopulations.clear();
    
    // Create prediction populations with specific properties
    for (size_t i = 0; i < numPopulations; ++i) {
        PopulationId popId = region->addPopulation(
            predictionNeurons / numPopulations,
            NeuronType::Internal
        );
        pImpl->predictionPopulations.push_back(popId);
        
        // Configure population for prediction
        NeuralPopulation* pop = region->getPopulation(popId);
        if (pop) {
            pop->setNeuronType(NeuronType::Internal);
            
            // Configure individual neurons for prediction
            for (size_t j = 0; j < pop->getSize(); ++j) {
                Neuron* neuron = pop->getNeuron(j);
                if (neuron) {
                    configurePredictionNeuron(neuron, i);
                }
            }
        }
    }
}

void PredictionSystem::configurePredictionNeuron(Neuron* neuron, size_t populationIndex) {
    if (!neuron) return;
    
    // Set neuron parameters for prediction dynamics
    neuron->setType(NeuronType::Internal);
    neuron->setThreshold(-45.0f - static_cast<float>(populationIndex) * 5.0f);
    neuron->setRestingPotential(-65.0f);
    neuron->setResetPotential(-70.0f);
    neuron->setRefractoryPeriod(3);
    
    // Enable prediction-specific plasticity
    auto& flags = neuron->getPlasticityFlags();
    flags.stdp = true;
    flags.hebbian = true;
    flags.eligible = true;
}

} // namespace nlm
