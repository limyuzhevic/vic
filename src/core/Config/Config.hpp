#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <map>
#include <typeindex>
#include <functional>
#include <unordered_map>

namespace nlm {

// Neuromodulator configuration structure
struct NeuromodulatorConfig {
    std::string name;
    std::string type;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    bool enabled;
    std::unordered_map<std::string, float> parameters;
    std::vector<std::string> interactions;
    
    NeuromodulatorConfig() : baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
                             adaptationRate(0.2f), enabled(true) {}
};

// Neuromodulator interaction configuration
struct NeuromodulatorInteractionConfig {
    std::string source;
    std::string target;
    std::string interactionType; // "excitatory", "inhibitory", "modulatory", "feedback", "feedforward"
    float strength;
    float threshold;
    
    NeuromodulatorInteractionConfig() : strength(1.0f), threshold(0.5f) {}
};

// Advanced neuromodulation configuration
struct AdvancedNeuromodulationConfig {
    // Neuromodulator-specific configurations
    NeuromodulatorConfig dopamineConfig;
    NeuromodulatorConfig acetylcholineConfig;
    NeuromodulatorConfig serotoninConfig;
    NeuromodulatorConfig endorphinsConfig;
    NeuromodulatorConfig norepinephrineConfig;
    NeuromodulatorConfig curiosityConfig;
    NeuromodulatorConfig noveltyConfig;
    NeuromodulatorConfig predictionErrorConfig;
    
    // Global neuromodulation parameters
    float globalAdaptationRate;
    float globalNoveltyThreshold;
    float globalLearningProgressThreshold;
    float globalAttentionModulation;
    float globalMotivationModulation;
    
    // Cross-modulator interaction configurations
    std::vector<NeuromodulatorInteractionConfig> interactions;
    
    // Plasticity neuromodulation parameters
    float plasticityBaseFactor;
    float plasticityNeuromodulatorWeight;
    float eligibilityTraceDecay;
    float temporalDifferenceDecay;
    
    // Learning rule parameters
    float hebbianLearningRate;
    float stdpLTPWeight;
    float stdpLTDWeight;
    float stdpTimeConstant;
    float bcmTheta;
    float bcmSigma;
    
    // Advanced learning parameters
    float rewardPredictionErrorGain;
    float curiosityExplorationBalance;
    float noveltyPatternMemorySize;
    float predictionErrorConfidenceThreshold;
    
    AdvancedNeuromodulationConfig() {
        // Default configurations for each neuromodulator
        dopamineConfig.name = "Dopamine";
        dopamineConfig.type = "Reward";
        dopamineConfig.baseline = 0.1f;
        dopamineConfig.peak = 1.0f;
        dopamineConfig.decayRate = 0.1f;
        dopamineConfig.releaseRate = 1.0f;
        dopamineConfig.adaptationRate = 0.2f;
        
        acetylcholineConfig.name = "Acetylcholine";
        acetylcholineConfig.type = "Attention";
        acetylcholineConfig.baseline = 0.1f;
        acetylcholineConfig.peak = 1.0f;
        acetylcholineConfig.decayRate = 0.1f;
        acetylcholineConfig.releaseRate = 0.8f;
        acetylcholineConfig.adaptationRate = 0.15f;
        
        serotoninConfig.name = "Serotonin";
        serotoninConfig.type = "LearningRate";
        serotoninConfig.baseline = 0.1f;
        serotoninConfig.peak = 1.0f;
        serotoninConfig.decayRate = 0.1f;
        serotoninConfig.releaseRate = 0.9f;
        serotoninConfig.adaptationRate = 0.1f;
        
        endorphinsConfig.name = "Endorphins";
        endorphinsConfig.type = "Motivation";
        endorphinsConfig.baseline = 0.1f;
        endorphinsConfig.peak = 1.0f;
        endorphinsConfig.decayRate = 0.1f;
        endorphinsConfig.releaseRate = 0.8f;
        endorphinsConfig.adaptationRate = 0.15f;
        
        norepinephrineConfig.name = "Norepinephrine";
        norepinephrineConfig.type = "Arousal";
        norepinephrineConfig.baseline = 0.1f;
        norepinephrineConfig.peak = 1.0f;
        norepinephrineConfig.decayRate = 0.1f;
        norepinephrineConfig.releaseRate = 0.9f;
        norepinephrineConfig.adaptationRate = 0.1f;
        
        curiosityConfig.name = "Curiosity";
        curiosityConfig.type = "Exploration";
        curiosityConfig.baseline = 0.0f;
        curiosityConfig.peak = 1.0f;
        curiosityConfig.decayRate = 0.05f;
        curiosityConfig.releaseRate = 1.2f;
        curiosityConfig.adaptationRate = 0.2f;
        
        noveltyConfig.name = "Novelty";
        noveltyConfig.type = "NoveltyDetection";
        noveltyConfig.baseline = 0.0f;
        noveltyConfig.peak = 1.0f;
        noveltyConfig.decayRate = 0.1f;
        noveltyConfig.releaseRate = 1.0f;
        noveltyConfig.adaptationRate = 0.15f;
        
        predictionErrorConfig.name = "PredictionError";
        predictionErrorConfig.type = "ErrorSignal";
        predictionErrorConfig.baseline = 0.0f;
        predictionErrorConfig.peak = 1.0f;
        predictionErrorConfig.decayRate = 0.1f;
        predictionErrorConfig.releaseRate = 1.0f;
        predictionErrorConfig.adaptationRate = 0.1f;
        
        // Global parameters
        globalAdaptationRate = 0.2f;
        globalNoveltyThreshold = 0.3f;
        globalLearningProgressThreshold = 0.7f;
        globalAttentionModulation = 1.0f;
        globalMotivationModulation = 1.0f;
        
        // Plasticity parameters
        plasticityBaseFactor = 1.0f;
        plasticityNeuromodulatorWeight = 0.8f;
        eligibilityTraceDecay = 0.95f;
        temporalDifferenceDecay = 0.9f;
        
        // Learning rule parameters
        hebbianLearningRate = 0.01f;
        stdpLTPWeight = 0.01f;
        stdpLTDWeight = 0.012f;
        stdpTimeConstant = 20.0f;
        bcmTheta = 0.5f;
        bcmSigma = 0.1f;
        
        // Advanced parameters
        rewardPredictionErrorGain = 1.0f;
        curiosityExplorationBalance = 0.7f;
        noveltyPatternMemorySize = 50.0f;
        predictionErrorConfidenceThreshold = 0.8f;
    }
};

// Custom exception class for configuration errors
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

// Forward declarations
class Config;

// Abstract base class for component factories
class ComponentFactory {
public:
    virtual ~ComponentFactory() = default;
    
    // Create component instance
    virtual std::unique_ptr<void> create(const std::vector<float>& parameters) = 0;
    
    // Get component type name
    virtual const std::string& getComponentType() const = 0;
    
    // Check if factory can create this type
    virtual bool canCreate(const std::string& type) const = 0;
    
    // Get default parameters for this type
    virtual std::vector<float> getDefaultParameters() const = 0;
};

// Abstract base class for dependency injection container
class DependencyInjector {
public:
    virtual ~DependencyInjector() = default;
    
    // Register a component factory
    virtual void registerFactory(std::unique_ptr<ComponentFactory> factory) = 0;
    
    // Get factory for component type
    virtual ComponentFactory* getFactory(const std::string& type) = 0;
    
    // Create component instance
    virtual std::unique_ptr<void> createComponent(const std::string& type,
                                                 const std::vector<float>& parameters = {}) = 0;
    
    // Get all registered component types
    virtual std::vector<std::string> getRegisteredTypes() const = 0;
    
    // Clear all registrations
    virtual void clear() = 0;
};

// Abstract base class for observers (Observer pattern)
class Observer {
public:
    virtual ~Observer() = default;
    
    // Handle state change notification
    virtual void onStateChange(const std::string& eventType,
                              const std::string& key,
                              const ConfigValue& value) = 0;
    
    // Get observer name
    virtual const std::string& getName() const = 0;
};

// Abstract base class for event handlers (Event handling pattern)
class EventHandler {
public:
    virtual ~EventHandler() = default;
    
    // Handle event
    virtual void handleEvent(const std::string& eventType,
                            const std::map<std::string, ConfigValue>& data) = 0;
    
    // Check if handler can process this event
    virtual bool canHandle(const std::string& eventType) const = 0;
    
    // Get handler name
    virtual const std::string& getName() const = 0;
};

// Abstract base class for strategy implementations (Strategy pattern)
class Strategy {
public:
    virtual ~Strategy() = default;
    
    // Execute strategy
    virtual ConfigValue execute(const std::vector<ConfigValue>& inputs) = 0;
    
    // Get strategy name
    virtual const std::string& getName() const = 0;
    
    // Set strategy parameters
    virtual void setParameters(const std::map<std::string, ConfigValue>& params) = 0;
    
    // Get strategy parameters
    virtual std::map<std::string, ConfigValue> getParameters() const = 0;
};

// Abstract base class for neural model implementations
class NeuralModel {
public:
    virtual ~NeuralModel() = default;
    
    // Process input and produce output
    virtual std::vector<float> process(const std::vector<float>& input) = 0;
    
    // Get model name
    virtual const std::string& getName() const = 0;
    
    // Get model type
    virtual const std::string& getType() const = 0;
    
    // Set model parameters
    virtual void setParameters(const std::map<std::string, float>& params) = 0;
    
    // Get model parameters
    virtual std::map<std::string, float> getParameters() const = 0;
};

// Abstract base class for neuromodulation implementations
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Apply neuromodulatory signal
    virtual void apply(NeuronId neuronId, float signal) = 0;
    
    // Get neuromodulator name
    virtual const std::string& getName() const = 0;
    
    // Get neuromodulator type
    virtual const std::string& getType() const = 0;
    
    // Set neuromodulator strength
    virtual void setStrength(float strength) = 0;
    
    // Get neuromodulator strength
    virtual float getStrength() const = 0;
    
    // Check if neuromodulator is active
    virtual bool isActive() const = 0;
    
    // Activate neuromodulator
    virtual void activate() = 0;
    
    // Deactivate neuromodulator
    virtual void deactivate() = 0;
};

// Factory interface for component creation
class ComponentFactory {
public:
    virtual ~ComponentFactory() = default;
    
    // Register a factory for a component type
    virtual void registerFactory(const std::string& type, 
                                std::function<std::unique_ptr<void>()> factory) = 0;
    
    // Create component instance
    virtual std::unique_ptr<void> create(const std::string& type) = 0;
    
    // Get list of registered types
    virtual std::vector<std::string> getRegisteredTypes() const = 0;
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration validation helpers
    template<typename T>
    bool validateConfigValue(const std::string& key, const ConfigValue& value) const {
        if (key.empty()) {
            return false;
        }
        
        try {
            std::get<T>(value);
            return true;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
    
    template<typename T>
    bool validateConfigRange(const std::string& key, const ConfigValue& value, T minValue, T maxValue) const {
        if (key.empty()) {
            return false;
        }
        
        if (!validateConfigValue<T>(key, value)) {
            return false;
        }
        
        try {
            T val = std::get<T>(value);
            return val >= minValue && val <= maxValue;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
    
    // Get validated config value
    template<typename T>
    T getValidatedConfig(const std::string& key, const T& defaultValue, 
                        bool validateRange = false, T minValue = T(), T maxValue = T()) const {
        auto val = get<T>(key);
        if (!val.has_value()) {
            return defaultValue;
        }
        
        if (validateRange && !validateConfigRange<T>(key, *val, minValue, maxValue)) {
            return defaultValue;
        }
        
        return *val;
    }
    
    // Observer management (for Observer pattern)
    void attachObserver(std::unique_ptr<Observer> observer);
    void detachObserver(const std::string& observerName);
    void notifyStateChange(const std::string& eventType, 
                          const std::string& key, 
                          const ConfigValue& value);
    
    // Event handler management (for Event handling pattern)
    void registerEventHandler(std::unique_ptr<EventHandler> handler);
    void unregisterEventHandler(const std::string& handlerName);
    void dispatchEvent(const std::string& eventType,
                      const std::map<std::string, ConfigValue>& data);
    
    // Strategy management (for Strategy pattern)
    void setStrategy(std::unique_ptr<Strategy> strategy);
    Strategy* getStrategy() const;
    ConfigValue executeStrategy(const std::vector<ConfigValue>& inputs);
    
    // Neural model management
    void setNeuralModel(std::unique_ptr<NeuralModel> model);
    NeuralModel* getNeuralModel() const;
    std::vector<float> processNeuralModel(const std::vector<float>& input);
    
    // Neuromodulator management
    void addNeuromodulator(std::unique_ptr<Neuromodulator> modulator);
    void removeNeuromodulator(const std::string& name);
    void applyNeuromodulation(NeuronId neuronId, const std::string& modulatorName, float signal);
    
    // Advanced neuromodulation configuration
    void setAdvancedNeuromodulationConfig(const AdvancedNeuromodulationConfig& config);
    AdvancedNeuromodulationConfig getAdvancedNeuromodulationConfig() const;
    
    // Get individual neuromodulator config
    NeuromodulatorConfig getDopamineConfig() const;
    NeuromodulatorConfig getAcetylcholineConfig() const;
    NeuromodulatorConfig getSerotoninConfig() const;
    NeuromodulatorConfig getEndorphinsConfig() const;
    NeuromodulatorConfig getNorepinephrineConfig() const;
    NeuromodulatorConfig getCuriosityConfig() const;
    NeuromodulatorConfig getNoveltyConfig() const;
    NeuromodulatorConfig getPredictionErrorConfig() const;
    
    // Set individual neuromodulator config
    void setDopamineConfig(const NeuromodulatorConfig& config);
    void setAcetylcholineConfig(const NeuromodulatorConfig& config);
    void setSerotoninConfig(const NeuromodulatorConfig& config);
    void setEndorphinsConfig(const NeuromodulatorConfig& config);
    void setNorepinephrineConfig(const NeuromodulatorConfig& config);
    void setCuriosityConfig(const NeuromodulatorConfig& config);
    void setNoveltyConfig(const NeuromodulatorConfig& config);
    void setPredictionErrorConfig(const NeuromodulatorConfig& config);
    
    // Get advanced neuromodulation parameters
    float getGlobalAdaptationRate() const;
    float getGlobalNoveltyThreshold() const;
    float getGlobalLearningProgressThreshold() const;
    float getGlobalAttentionModulation() const;
    float getGlobalMotivationModulation() const;
    
    // Set advanced neuromodulation parameters
    void setGlobalAdaptationRate(float rate);
    void setGlobalNoveltyThreshold(float threshold);
    void setGlobalLearningProgressThreshold(float threshold);
    void setGlobalAttentionModulation(float modulation);
    void setGlobalMotivationModulation(float modulation);
    
    // Get plasticity parameters
    float getPlasticityBaseFactor() const;
    float getPlasticityNeuromodulatorWeight() const;
    float getEligibilityTraceDecay() const;
    float getTemporalDifferenceDecay() const;
    
    // Set plasticity parameters
    void setPlasticityBaseFactor(float factor);
    void setPlasticityNeuromodulatorWeight(float weight);
    void setEligibilityTraceDecay(float decay);
    void setTemporalDifferenceDecay(float decay);
    
    // Get learning rule parameters
    float getHebbianLearningRate() const;
    float getSTDP LTPWeight() const;
    float getSTDP LTDWeight() const;
    float getSTDPTimeConstant() const;
    float getBCMTheta() const;
    float getBCMSigma() const;
    
    // Set learning rule parameters
    void setHebbianLearningRate(float rate);
    void setSTDP LTPWeight(float weight);
    void setSTDP LTDWeight(float weight);
    void setSTDPTimeConstant(float tau);
    void setBCMTheta(float theta);
    void setBCMSigma(float sigma);
    
    // Get advanced learning parameters
    float getRewardPredictionErrorGain() const;
    float getCuriosityExplorationBalance() const;
    float getNoveltyPatternMemorySize() const;
    float getPredictionErrorConfidenceThreshold() const;
    
    // Set advanced learning parameters
    void setRewardPredictionErrorGain(float gain);
    void setCuriosityExplorationBalance(float balance);
    void setNoveltyPatternMemorySize(float size);
    void setPredictionErrorConfidenceThreshold(float threshold);
    
    // Dependency injection container
    void setDependencyInjector(std::unique_ptr<DependencyInjector> injector);
    DependencyInjector* getDependencyInjector() const;
    
    // Component factory management
    void setComponentFactory(std::unique_ptr<ComponentFactory> factory);
    ComponentFactory* getComponentFactory() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
