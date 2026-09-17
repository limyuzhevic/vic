// I. Motor Decoding Strategy Pattern - Separate motor decoding logic
#pragma once

namespace nlm {

class MotorDecodingStrategy {
public:
    virtual ~MotorDecodingStrategy() = default;
    virtual MotorCommand decode(const std::vector<Neuron*>& neurons) = 0;
    virtual std::string getName() const = 0;
};

class DefaultMotorDecodingStrategy : public MotorDecodingStrategy {
public:
    MotorCommand decode(const std::vector<Neuron*>& neurons) override;
    std::string getName() const override { return "Default"; }
};

class ThresholdMotorDecodingStrategy : public MotorDecodingStrategy {
public:
    ThresholdMotorDecodingStrategy(float threshold = 0.5f);
    MotorCommand decode(const std::vector<Neuron*>& neurons) override;
    std::string getName() const override { return "ThresholdBased"; }
    
private:
    float threshold_;
};

// II. Curiosity Handling Strategy Pattern - Separate curiosity logic  
#pragma once

namespace nlm {

class CuriosityHandler {
public:
    virtual ~CuriosityHandler() = default;
    virtual void update(float novelty, float predictionError, double timestep) = 0;
    virtual float getCuriosityLevel() const = 0;
    virtual std::string getType() const = 0;
};

class SimpleCuriosityHandler : public CuriosityHandler {
public:
    SimpleCuriosityHandler(float noveltyWeight = 2.0f, float errorWeight = 0.5f);
    void update(float novelty, float predictionError, double timestep) override;
    float getCuriosityLevel() const override { return curiosityLevel_; }
    std::string getType() const override { return "Simple"; }
    
private:
    float noveltyWeight_;
    float errorWeight_;
    float curiosityLevel_;
    float decayRate_;
};

// III. Configuration Validation System
#pragma once

namespace nlm {

class ParameterGroup {
public:
    ParameterGroup(const std::string& name, const std::string& description);
    
    template<typename T>
    void addParameter(const std::string& key, T defaultValue, 
                     const std::string& description = "",
                     std::function<bool(const T&)> validator = nullptr);
    
    template<typename T>
    bool validateParameter(const std::string& key, const T& value) const;
    
    std::string getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    size_t getParameterCount() const { return parameters_.size(); }
    
    const std::vector<std::string>& getParameterKeys() const { return parameterKeys_; }
    
    template<typename T>
    T getParameter(const std::string& key) const;
    
    template<typename T>
    void setParameter(const std::string& key, const T& value);
    
private:
    struct ParameterInfo {
        std::string key;
        std::any defaultValue;
        std::string description;
        std::function<bool(const std::any&)> validator;
        std::any currentValue;
        
        template<typename T>
        ParameterInfo(const std::string& k, T defaultVal, 
                     const std::string& desc = "",
                     std::function<bool(const T&)> val = nullptr)
            : key(k), defaultValue(defaultVal), description(desc),
              validator([val](const std::any& v) {
                  if (!val) return true;
                  try {
                      T valType = std::any_cast<T>(v);
                      return val(valType);
                  } catch (...) {
                      return false;
                  }
              }), currentValue(defaultVal) {}
    };
    
    std::string name_;
    std::string description_;
    std::vector<std::string> parameterKeys_;
    std::vector<ParameterInfo> parameters_;
};

class ConfigurationValidator {
public:
    static bool validateAll(const ParameterGroup& group) {
        for (const auto& param : group.getParameterKeys()) {
            // Implementation would validate all parameters
        }
        return true;
    }
    
    static std::vector<std::string> getValidationErrors(const ParameterGroup& group) {
        std::vector<std::string> errors;
        // Implementation would collect validation errors
        return errors;
    }
};

// IV. Dependency Injection Container for Subsystems
#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <typeindex>

namespace nlm {

class Subsystem {
public:
    virtual ~Subsystem() = default;
    virtual std::string getName() const = 0;
};

class SubsystemRegistry {
public:
    template<typename TSubsystem, typename... TArgs>
    void registerSubsystem(const std::string& name, TArgs&&... args) {
        auto subsystem = std::make_unique<TSubsystem>(std::forward<TArgs>(args)...);
        subsystems_[name] = std::move(subsystem);
    }
    
    template<typename TSubsystem>
    std::shared_ptr<TSubsystem> getSubsystem(const std::string& name) {
        auto it = subsystems_.find(name);
        if (it != subsystems_.end()) {
            return std::static_pointer_cast<TSubsystem>(it->second);
        }
        return nullptr;
    }
    
    bool hasSubsystem(const std::string& name) const {
        return subsystems_.find(name) != subsystems_.end();
    }
    
    void listSubsystems() const {
        for (const auto& pair : subsystems_) {
            std::cout << "  - " << pair.first << std::endl;
        }
    }
    
private:
    std::unordered_map<std::string, std::unique_ptr<Subsystem>> subsystems_;
};

// V. Null-Safe Wrapper Utilities
#pragma once

namespace nlm {

template<typename T>
class NullSafePtr {
public:
    explicit NullSafePtr(T* ptr = nullptr) : ptr_(ptr) {}
    
    T& operator*() {
        if (!ptr_) {
            throw std::runtime_error("Null pointer dereference attempted");
        }
        return *ptr_;
    }
    
    const T& operator*() const {
        if (!ptr_) {
            throw std::runtime_error("Null pointer dereference attempted");
        }
        return *ptr_;
    }
    
    T* operator->() {
        if (!ptr_) {
            throw std::runtime_error("Null pointer dereference attempted");
        }
        return ptr_;
    }
    
    const T* operator->() const {
        if (!ptr_) {
            throw std::runtime_error("Null pointer dereference attempted");
        }
        return ptr_;
    }
    
    bool isNull() const { return ptr_ == nullptr; }
    bool isNotNull() const { return ptr_ != nullptr; }
    
    explicit operator bool() const { return ptr_ != nullptr; }
    
    T* get() { return ptr_; }
    const T* get() const { return ptr_; }
    
    void reset(T* ptr = nullptr) { ptr_ = ptr; }
    
private:
    T* ptr_;
};

// VI. Abstract Brain Interface - Decouple core logic
#pragma once

namespace nlm {

class IMemorySystem {
public:
    virtual ~IMemorySystem() = default;
    virtual void store(const std::vector<float>& data) = 0;
    virtual std::vector<float> recall() const = 0;
    virtual void clear() = 0;
    virtual size_t capacity() const = 0;
};

class IBrainInterface {
public:
    virtual ~IBrainInterface() = default;
    
    // Core neural computation
    virtual void receiveSensoryInput(const SensoryInput& input) = 0;
    virtual std::unique_ptr<Action> produceAction() = 0;
    virtual void step() = 0;
    
    // System access
    virtual IMemorySystem* getMemorySystem() = 0;
    virtual const IMemorySystem* getMemorySystem() const = 0;
    
    // Statistics
    virtual size_t getActiveNeuronCount() const = 0;
    virtual size_t getTotalSpikeCount() const = 0;
    
    // State
    virtual void reset() = 0;
};

class AbstractBrain : public IBrainInterface {
protected:
    std::unique_ptr<IMemorySystem> memorySystem_;
    
    void validateNeuronState(const std::vector<Neuron*>& neurons) {
        for (size_t i = 0; i < neurons.size(); ++i) {
            if (!neurons[i]) {
                throw std::runtime_error("Null neuron at index " + std::to_string(i));
            }
        }
    }
    
    float calculateNormalizedActivity(const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) return 0.0f;
        
        float totalActivity = 0.0f;
        for (Neuron* neuron : neurons) {
            if (neuron) {
                totalActivity += std::abs(neuron->getState().membranePotential - 
                                         neuron->getState().restingPotential);
            }
        }
        
        return totalActivity / neurons.size();
    }
};

// VII. Improved AgentBrain with Dependency Injection
#pragma once

namespace nlm {

class AgentBrain {
public:
    AgentBrain(std::shared_ptr<IBrainInterface> brainInterface,
               std::unique_ptr<MotorDecodingStrategy> motorStrategy,
               std::unique_ptr<CuriosityHandler> curiosityHandler,
               std::shared_ptr<Config> config = nullptr);
    
    ~AgentBrain();
    
    // Configuration-driven sizing
    void configureFromConfig(const Config& config);
    size_t getSensoryInputSize() const { return sensoryInputSize_; }
    size_t getMotorOutputSize() const { return motorOutputSize_; }
    
    // Core operations
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    
    // State management
    void reset();
    bool isValid() const;
    
    // Getters
    float getCuriosityLevel() const { return curiosityHandler_ ? curiosityHandler_->getCuriosityLevel() : 0.0f; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    
private:
    // Null-safe helper methods
    template<typename T>
    bool safeAccess(const std::vector<T*>& container, size_t index, T*& out) {
        out = nullptr;
        if (index < container.size()) {
            out = container[index];
        }
        return out != nullptr;
    }
    
    void initializeNeuronGroups();
    void validatePercept(const SensoryPercept& percept) const;
    
    // Dependency injection fields
    std::shared_ptr<IBrainInterface> brainInterface_;
    std::unique_ptr<MotorDecodingStrategy> motorStrategy_;
    std::unique_ptr<CuriosityHandler> curiosityHandler_;
    std::shared_ptr<Config> config_;
    
    // Neuron group management
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // State
    std::vector<float> previousVision_;
    float noveltyLevel_;
    float predictionError_;
    float expectedReward_;
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    float sensoryNoveltyDecay_;
    size_t sensoryInputSize_;
    size_t motorOutputSize_;
};
