#pragma once

#include "CommonBaseClasses.hpp"
#include <string>
#include <unordered_map>
#include <memory>

namespace nlm {

/**
 * Strategy pattern base class for plasticity rules
 * Defines the interface for different plasticity algorithms
 */
class PlasticityStrategy {
public:
    virtual ~PlasticityStrategy() = default;
    
    // Apply the plasticity rule to the specified presynaptic and postsynaptic neurons
    virtual void apply(std::shared_ptr<NLMObject> presynaptic,
                      std::shared_ptr<NLMObject> postsynaptic,
                      double weight,
                      double dt) = 0;
    
    // Get the name of the strategy
    virtual const std::string& getName() const = 0;
    
    // Get the parameters of the strategy
    virtual std::unordered_map<std::string, double> getParameters() const = 0;
    
    // Set the parameters of the strategy
    virtual void setParameters(const std::unordered_map<std::string, double>& params) = 0;
};

/**
 * STDP (Spike-Timing-Dependent Plasticity) strategy
 */
class STDPAlgorithm : public PlasticityStrategy {
public:
    std::string getName() const override { return "STDP"; }
    void apply(std::shared_ptr<NLMObject> presynaptic,
              std::shared_ptr<NLMObject> postsynaptic,
              double weight,
              double dt) override;
    std::unordered_map<std::string, double> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, double>& params) override;

private:
    double learningRate = 0.01;
    double timeConstant = 20.0;
    double maxWeightChange = 0.1;
};

/**
 * Hebbian learning strategy
 */
class HebbianLearning : public PlasticityStrategy {
public:
    std::string getName() const override { return "Hebbian"; }
    void apply(std::shared_ptr<NLMObject> presynaptic,
              std::shared_ptr<NLMObject> postsynaptic,
              double weight,
              double dt) override;
    std::unordered_map<std::string, double> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, double>& params) override;

private:
    double learningRate = 0.001;
    double maxWeight = 1.0;
};

/**
 * Neuromodulation strategy for plasticity rules
 * Integrates neuromodulator signals into plasticity
 */
class NeuromodulationStrategy {
public:
    virtual ~NeuromodulationStrategy() = default;
    
    // Get the neuromodulation factor
    virtual double getModulationFactor(const std::string& neuromodulator) const = 0;
    
    // Set the neuromodulation factor
    virtual void setModulationFactor(const std::string& neuromodulator, double factor) = 0;
    
    // Update neuromodulation factors
    virtual void update(double dt) = 0;
};

/**
 * Dopamine neuromodulation strategy
 */
class DopamineNeuromodulation : public NeuromodulationStrategy {
public:
    double getModulationFactor(const std::string& neuromodulator) const override;
    void setModulationFactor(const std::string& neuromodulator, double factor) override;
    void update(double dt) override;

private:
    double baseline = 0.1;
    double predictionError = 0.0;
    double learningRate = 0.01;
};
}