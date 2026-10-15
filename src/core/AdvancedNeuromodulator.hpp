#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <functional>
#include "../core/Config/Config.hpp"
#include "../brain/Neuron.hpp"

namespace nlm {

class AdvancedNeuromodulator {
public:
    AdvancedNeuromodulator();
    virtual ~AdvancedNeuromodulator();
    
    // Disable copying, enable moving
    AdvancedNeuromodulator(const AdvancedNeuromodulator&) = delete;
    AdvancedNeuromodulator& operator=(const AdvancedNeuromodulator&) = delete;
    AdvancedNeuromodulator(AdvancedNeuromodulator&&) noexcept;
    AdvancedNeuromodulator& operator=(AdvancedNeuromodulator&&) noexcept;
    
    // Initialization and lifecycle
    virtual bool initialize(const Config& config);
    virtual void update(double timestep);
    virtual void shutdown();
    
    // Neuromodulator interface
    virtual void applyToNeuron(Neuron& neuron, double timestep);
    virtual double getSignal(const std::string& target) const;
    virtual void addModulationTarget(const std::string& target, double weight = 1.0);
    
    // Advanced neuromodulation methods
    virtual void addCustomModulator(std::unique_ptr<class CustomModulator> modulator);
    virtual class CustomModulator* getModulator(const std::string& name) const;
    virtual void setLearningRate(double rate);
    virtual void setDecayRate(double rate);
    virtual void setIntegrationMethod(const std::string& method);
    
    // Prediction error integration
    virtual void setPredictionErrorSource(const std::string& source);
    virtual double getPredictionError() const;
    virtual void setPredictionError(double error);
    
    // Novelty detection
    virtual void setNoveltyDetector(std::unique_ptr<class NoveltyDetector> detector);
    virtual class NoveltyDetector* getNoveltyDetector() const;
    virtual double getNoveltyScore() const;
    
    // Custom implementation support
    virtual void addCustomSignalGenerator(const std::string& name, 
                                          std::function<double(double)> generator);
    virtual double generateCustomSignal(const std::string& name, double timestep) const;
    
    // Statistics and profiling
    virtual std::string getNeuromodulationStats() const;
    virtual void enableProfiling(bool enable);
    virtual bool isProfilingEnabled() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Custom modulator interface
class CustomModulator {
public:
    virtual ~CustomModulator() = default;
    virtual std::string getName() const = 0;
    virtual std::string getType() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual double computeSignal(double timestep, const NeuromodulatorContext& context) = 0;
    virtual void updateParameters(const Config& config) = 0;
};

// Novelty detector interface  
class NoveltyDetector {
public:
    virtual ~NoveltyDetector() = default;
    virtual std::string getName() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual double computeNovelty(const std::vector<float>& features) = 0;
    virtual void update(double timestep) = 0;
};

// Neuromodulator context for custom implementations
struct NeuromodulatorContext {
    double timestep;
    NeuronId neuronId;
    NeuronState neuronState;
    double predictionError;
    double noveltyScore;
    std::map<std::string, double> externalSignals;
    NeuromodulatorContext()
        : timestep(0.0), neuronId(0), predictionError(0.0), noveltyScore(0.0) {}
};

} // namespace nlm
