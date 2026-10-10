#pragma once

#include "plugin/PluginSystem.hpp"
#include "brain/Neuron.hpp"
#include <memory>
#include <vector>
#include <cmath>

namespace nlm {

// Custom neural model plugin
class CustomNeuralModelPlugin : public PluginBase<CustomNeuralModelPlugin> {
public:
    CustomNeuralModelPlugin() : PluginBase(
        PluginCapability("CustomNeuralModel", PluginType::NeuralModel, "1.0.0", 
                       "A custom neural model with enhanced dynamics", true),
        "1.0.0"
    ) {}
    
    bool initializeImpl(const std::map<std::string, std::string>& config) override {
        // Parse configuration
        if (config.find("alpha") != config.end()) {
            alpha_ = std::stod(config.at("alpha"));
        }
        if (config.find("beta") != config.end()) {
            beta_ = std::stod(config.at("beta"));
        }
        if (config.find("gamma") != config.end()) {
            gamma_ = std::stod(config.at("gamma"));
        }
        
        // Validate parameters
        if (alpha_ <= 0 || beta_ <= 0 || gamma_ <= 0) {
            std::cerr << "Invalid parameters for CustomNeuralModelPlugin" << std::endl;
            return false;
        }
        
        std::cout << "CustomNeuralModelPlugin initialized with alpha=" << alpha_ 
                  << ", beta=" << beta_ << ", gamma=" << gamma_ << std::endl;
        return true;
    }
    
    void cleanupImpl() override {
        neurons_.clear();
        std::cout << "CustomNeuralModelPlugin cleaned up" << std::endl;
    }
    
    // Custom neuron implementation
    class CustomNeuron : public Neuron {
    public:
        CustomNeuron(NeuronId id) : Neuron(id), alpha_(alpha_), beta_(beta_), gamma_(gamma_) {}
        
        bool stepLIF(Timestamp currentTime, TimestepDuration dt) override {
            // Enhanced LIF dynamics with custom parameters
            MembranePotential& V = getState().membranePotential;
            MembranePotential V_rest = getState().restingPotential;
            float tau = 20.0f;
            
            // Custom dynamics with nonlinear components
            float input = getTotalCurrent();
            float leak = (V_rest - V) / tau;
            float alpha = alpha_;
            float beta = beta_;
            float gamma = gamma_;
            
            // Enhanced integration with custom parameters
            V = V + static_cast<float>(dt) * 1000.0f * (leak + input * alpha - V * beta / gamma);
            
            // Clamp to reasonable range
            V = std::clamp(V, -100.0f, 50.0f);
            
            // Custom spike detection with threshold modulation
            float threshold = getState().threshold * (1.0f + gamma * sin(currentTime * 0.01f));
            
            if (V >= threshold) {
                return Neuron::stepLIF(currentTime, dt);
            }
            
            return false;
        }
        
        void setCustomParameters(float alpha, float beta, float gamma) {
            alpha_ = alpha;
            beta_ = beta;
            gamma_ = gamma;
        }
        
    private:
        float alpha_ = 1.0f;
        float beta_ = 1.0f;
        float gamma_ = 1.0f;
    };
    
    // Create a custom neuron with enhanced dynamics
    std::shared_ptr<Neuron> createEnhancedNeuron(NeuronId id) {
        auto neuron = std::make_shared<CustomNeuron>(id);
        neurons_.push_back(neuron);
        return neuron;
    }
    
    // Get all enhanced neurons
    std::vector<std::shared_ptr<Neuron>> getEnhancedNeurons() const {
        return neurons_;
    }
    
    // Get model parameters
    double getAlpha() const { return alpha_; }
    double getBeta() const { return beta_; }
    double getGamma() const { return gamma_; }
    
    void setAlpha(double alpha) { alpha_ = alpha; }
    void setBeta(double beta) { beta_ = beta; }
    void setGamma(double gamma) { gamma_ = gamma; }
    
    // Get plugin capabilities
    static std::string getPluginName() {
        return "CustomNeuralModelPlugin";
    }
    
    static PluginCapability getPluginCapability() {
        return PluginCapability("CustomNeuralModel", PluginType::NeuralModel, "1.0.0",
                               "A custom neural model with enhanced dynamics", true);
    }
    
    // Global function to create plugin instance
    static std::shared_ptr<IPlugin> createPlugin() {
        return std::make_shared<CustomNeuralModelPlugin>();
    }
    
private:
    double alpha_ = 1.0;
    double beta_ = 1.0;
    double gamma_ = 1.0;
    std::vector<std::shared_ptr<Neuron>> neurons_;
};

// Memory system plugin
class CustomMemorySystemPlugin : public PluginBase<CustomMemorySystemPlugin> {
public:
    CustomMemorySystemPlugin() : PluginBase(
        PluginCapability("CustomMemorySystem", PluginType::MemorySystem, "1.0.0",
                       "An enhanced memory system with custom algorithms", true),
        "1.0.0"
    ) {}
    
    bool initializeImpl(const std::map<std::string, std::string>& config) override {
        // Parse configuration
        if (config.find("capacity_multiplier") != config.end()) {
            capacityMultiplier_ = std::stod(config.at("capacity_multiplier"));
        }
        if (config.find(" consolidation_strength") != config.end()) {
            consolidationStrength_ = std::stod(config.at(" consolidation_strength"));
        }
        
        std::cout << "CustomMemorySystemPlugin initialized" << std::endl;
        return true;
    }
    
    void cleanupImpl() override {
        std::cout << "CustomMemorySystemPlugin cleaned up" << std::endl;
    }
    
    // Custom memory management with enhanced algorithms
    class EnhancedMemoryManager {
    public:
        EnhancedMemoryManager(double capacityMultiplier, double consolidationStrength)
            : capacityMultiplier_(capacityMultiplier), consolidationStrength_(consolidationStrength) {}
        
        double getEffectiveCapacity(size_t baseCapacity) const {
            return static_cast<double>(baseCapacity) * capacityMultiplier_;
        }
        
        double getConsolidationFactor() const {
            return consolidationStrength_;
        }
        
        // Enhanced consolidation algorithm
        double calculateConsolidationScore(double activation, double age) const {
            return activation * std::exp(-age * consolidationStrength_);
        }
        
    private:
        double capacityMultiplier_;
        double consolidationStrength_;
    };
    
    EnhancedMemoryManager getMemoryManager() const {
        return EnhancedMemoryManager(capacityMultiplier_, consolidationStrength_);
    }
    
    static std::string getPluginName() {
        return "CustomMemorySystemPlugin";
    }
    
    static PluginCapability getPluginCapability() {
        return PluginCapability("CustomMemorySystem", PluginType::MemorySystem, "1.0.0",
                               "An enhanced memory system with custom algorithms", true);
    }
    
    static std::shared_ptr<IPlugin> createPlugin() {
        return std::make_shared<CustomMemorySystemPlugin>();
    }
    
private:
    double capacityMultiplier_ = 1.0;
    double consolidationStrength_ = 0.5;
};

// Neuromodulation plugin
class CustomNeuromodulationPlugin : public PluginBase<CustomNeuromodulationPlugin> {
public:
    CustomNeuromodulationPlugin() : PluginBase(
        PluginCapability("CustomNeuromodulation", PluginType::Neuromodulator, "1.0.0",
                       "Custom neuromodulation with dopamine and curiosity", false),
        "1.0.0"
    ) {}
    
    bool initializeImpl(const std::map<std::string, std::string>& config) override {
        // Parse neuromodulation parameters
        if (config.find("dopamine_sensitivity") != config.end()) {
            dopamineSensitivity_ = std::stod(config.at("dopamine_sensitivity"));
        }
        if (config.find("curiosity_strength") != config.end()) {
            curiosityStrength_ = std::stod(config.at("curiosity_strength"));
        }
        
        std::cout << "CustomNeuromodulationPlugin initialized" << std::endl;
        return true;
    }
    
    void cleanupImpl() override {
        std::cout << "CustomNeuromodulationPlugin cleaned up" << std::endl;
    }
    
    // Enhanced neuromodulation with custom dynamics
    class EnhancedNeuromodulation {
    public:
        EnhancedNeuromodulation(double dopamineSensitivity, double curiosityStrength)
            : dopamineSensitivity_(dopamineSensitivity), curiosityStrength_(curiosityStrength) {}
        
        double calculateDopamineModulation(double predictionError) const {
            return dopamineSensitivity_ * std::tanh(predictionError);
        }
        
        double calculateCuriosityModulation(double novelty, double potentialReward) const {
            return curiosityStrength_ * novelty * std::log(1.0 + potentialReward);
        }
        
        double calculateCombinedModulation(double predictionError, double novelty, 
                                           double potentialReward) const {
            double dopamine = calculateDopamineModulation(predictionError);
            double curiosity = calculateCuriosityModulation(novelty, potentialReward);
            return dopamine * curiosity;
        }
        
    private:
        double dopamineSensitivity_;
        double curiosityStrength_;
    };
    
    EnhancedNeuromodulation getNeuromodulation() const {
        return EnhancedNeuromodulation(dopamineSensitivity_, curiosityStrength_);
    }
    
    static std::string getPluginName() {
        return "CustomNeuromodulationPlugin";
    }
    
    static PluginCapability getPluginCapability() {
        return PluginCapability("CustomNeuromodulation", PluginType::Neuromodulator, "1.0.0",
                               "Custom neuromodulation with dopamine and curiosity", false);
    }
    
    static std::shared_ptr<IPlugin> createPlugin() {
        return std::make_shared<CustomNeuromodulationPlugin>();
    }
    
private:
    double dopamineSensitivity_ = 1.0;
    double curiosityStrength_ = 0.8;
};

extern "C" {
    // Custom neural model plugin
    std::shared_ptr<IPlugin> createCustomNeuralModelPlugin() {
        return std::make_shared<CustomNeuralModelPlugin>();
    }
    
    // Custom memory system plugin
    std::shared_ptr<IPlugin> createCustomMemorySystemPlugin() {
        return std::make_shared<CustomMemorySystemPlugin>();
    }
    
    // Custom neuromodulation plugin
    std::shared_ptr<IPlugin> createCustomNeuromodulationPlugin() {
        return std::make_shared<CustomNeuromodulationPlugin>();
    }
}

} // namespace nlm