#include "Pruning.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/Neuron.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../core/Random/Random.hpp"
#include "../../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <queue>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    float activityDependency;
    float experienceExpectation;
    float criticalPeriodModulation;
    float homeostaticSetpoint;
    
    // Statistics
    size_t totalSynapsesPruned;
    size_t totalExperienceDrivenPruned;
    size_t totalHomeostaticPruned;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f),
             activityDependency(0.7f), experienceExpectation(0.5f),
             criticalPeriodModulation(1.0f), homeostaticSetpoint(0.1f),
             totalSynapsesPruned(0), totalExperienceDrivenPruned(0), 
             totalHomeostaticPruned(0) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = std::clamp(threshold, 0.0f, 0.1f);
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
}