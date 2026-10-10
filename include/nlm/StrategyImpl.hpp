#pragma once

#include "Strategy.hpp"
#include <stdexcept>
#include <cmath>

namespace nlm {

void STDPAlgorithm::apply(std::shared_ptr<NLMObject> presynaptic,
                         std::shared_ptr<NLMObject> postsynaptic,
                         double weight,
                         double dt) {
    // Implement STDP rule
    double dt_pre = std::max(0.0, dt); // Assuming presynaptic spike
    double dt_post = std::max(0.0, -dt); // Assuming postsynaptic spike
    
    double weightChange = learningRate * std::exp(-std::abs(dt_pre) / timeConstant) * weight;
    if (dt < 0) {
        weightChange = -maxWeightChange * std::exp(-std::abs(dt_post) / timeConstant) * weight;
    }
    
    // Clamp weight change
    weightChange = std::max(-maxWeightChange, std::min(maxWeightChange, weightChange));
    
    // Update weight
    double newWeight = weight + weightChange;
    
    // Store result (simplified - would need actual weight access)
    // In a real implementation, we would call setter methods
}

std::unordered_map<std::string, double> STDPAlgorithm::getParameters() const {
    return {
        {"learningRate", learningRate},
        {"timeConstant", timeConstant},
        {"maxWeightChange", maxWeightChange}
    };
}

void STDPAlgorithm::setParameters(const std::unordered_map<std::string, double>& params) {
    if (params.find("learningRate") != params.end()) {
        learningRate = std::max(0.0, params.at("learningRate"));
    }
    if (params.find("timeConstant") != params.end()) {
        timeConstant = std::max(0.1, params.at("timeConstant"));
    }
    if (params.find("maxWeightChange") != params.end()) {
        maxWeightChange = std::max(0.0, params.at("maxWeightChange"));
    }
}

void HebbianLearning::apply(std::shared_ptr<NLMObject> presynaptic,
                           std::shared_ptr<NLMObject> postsynaptic,
                           double weight,
                           double dt) {
    // Implement Hebbian learning rule
    double weightChange = learningRate * (1.0 - weight);
    double newWeight = weight + weightChange;
    
    // Clamp to max weight
    newWeight = std::min(maxWeight, newWeight);
    
    // Store result (simplified - would need actual weight access)
    // In a real implementation, we would call setter methods
}

std::unordered_map<std::string, double> HebbianLearning::getParameters() const {
    return {
        {"learningRate", learningRate},
        {"maxWeight", maxWeight}
    };
}

void HebbianLearning::setParameters(const std::unordered_map<std::string, double>& params) {
    if (params.find("learningRate") != params.end()) {
        learningRate = std::max(0.0, params.at("learningRate"));
    }
    if (params.find("maxWeight") != params.end()) {
        maxWeight = std::max(0.0, params.at("maxWeight"));
    }
}

void DopamineNeuromodulation::update(double dt) {
    // Update neuromodulation factors
    baseline = std::max(0.0, baseline);
    predictionError = predictionError; // Would update based on prediction error
    // Scale prediction error
    predictionError = predictionError * (1.0 - dt * learningRate);
}

void DopamineNeuromodulation::setModulationFactor(const std::string& neuromodulator, double factor) {
    if (neuromodulator == "dopamine") {
        predictionError = factor;
    }
}

double DopamineNeuromodulation::getModulationFactor(const std::string& neuromodulator) const {
    if (neuromodulator == "dopamine") {
        return predictionError;
    }
    return baseline;
}
}