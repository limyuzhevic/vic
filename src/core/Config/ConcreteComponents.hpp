#pragma once

#include "../core/Config/Config.hpp"
#include <map>
#include <memory>
#include <vector>

namespace nlm {

// Concrete implementation of component factory
class DefaultComponentFactory : public ComponentFactory {
public:
    DefaultComponentFactory() = default;
    ~DefaultComponentFactory() override = default;
    
    void registerFactory(const std::string& type, 
                        std::function<std::unique_ptr<void>()> factory) override {
        factories_[type] = factory;
    }
    
    std::unique_ptr<void> create(const std::string& type) override {
        auto it = factories_.find(type);
        if (it != factories_.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    std::vector<std::string> getRegisteredTypes() const override {
        std::vector<std::string> types;
        for (const auto& pair : factories_) {
            types.push_back(pair.first);
        }
        return types;
    }
    
private:
    std::map<std::string, std::function<std::unique_ptr<void>()>> factories_;
};

// Concrete implementation of dependency injector
class SimpleDependencyInjector : public DependencyInjector {
public:
    SimpleDependencyInjector() = default;
    ~SimpleDependencyInjector() override = default;
    
    void registerFactory(std::unique_ptr<ComponentFactory> factory) override {
        factories_[factory->getComponentType()] = std::move(factory);
    }
    
    ComponentFactory* getFactory(const std::string& type) override {
        auto it = factories_.find(type);
        return (it != factories_.end()) ? it->second.get() : nullptr;
    }
    
    std::unique_ptr<void> createComponent(const std::string& type,
                                         const std::vector<float>& parameters) override {
        ComponentFactory* factory = getFactory(type);
        if (factory) {
            // Parameters are not currently used in this simple implementation
            return factory->create(parameters);
        }
        return nullptr;
    }
    
    std::vector<std::string> getRegisteredTypes() const override {
        std::vector<std::string> types;
        for (const auto& pair : factories_) {
            types.push_back(pair.first);
        }
        return types;
    }
    
    void clear() override {
        factories_.clear();
    }
    
private:
    std::map<std::string, std::unique_ptr<ComponentFactory>> factories_;
};

// Concrete implementation of observer
class SimpleObserver : public Observer {
public:
    SimpleObserver(const std::string& name,
                  std::function<void(const std::string&, const std::string&, const ConfigValue&)> callback)
        : name_(name), callback_(callback) {}
    
    ~SimpleObserver() override = default;
    
    void onStateChange(const std::string& eventType,
                      const std::string& key,
                      const ConfigValue& value) override {
        if (callback_) {
            callback_(eventType, key, value);
        }
    }
    
    const std::string& getName() const override {
        return name_;
    }
    
private:
    std::string name_;
    std::function<void(const std::string&, const std::string&, const ConfigValue&)> callback_;
};

// Concrete implementation of event handler
class SimpleEventHandler : public EventHandler {
public:
    SimpleEventHandler(const std::string& name,
                      std::function<bool(const std::string&)> canHandle,
                      std::function<void(const std::string&, const std::map<std::string, ConfigValue>&)> handler)
        : name_(name), canHandle_(canHandle), handler_(handler) {}
    
    ~SimpleEventHandler() override = default;
    
    void handleEvent(const std::string& eventType,
                   const std::map<std::string, ConfigValue>& data) override {
        if (handler_) {
            handler_(eventType, data);
        }
    }
    
    bool canHandle(const std::string& eventType) const override {
        return canHandle_ ? canHandle_(eventType) : false;
    }
    
    const std::string& getName() const override {
        return name_;
    }
    
private:
    std::string name_;
    std::function<bool(const std::string&)> canHandle_;
    std::function<void(const std::string&, const std::map<std::string, ConfigValue>&)> handler_;
};

// Concrete implementation of strategy (e.g., learning strategy)
class SimpleStrategy : public Strategy {
public:
    SimpleStrategy(const std::string& name,
                  std::function<ConfigValue(const std::vector<ConfigValue>&)> executor)
        : name_(name), executor_(executor) {}
    
    ~SimpleStrategy() override = default;
    
    ConfigValue execute(const std::vector<ConfigValue>& inputs) override {
        return executor_ ? executor_(inputs) : ConfigValue();
    }
    
    const std::string& getName() const override {
        return name_;
    }
    
    void setParameters(const std::map<std::string, ConfigValue>& params) override {
        parameters_ = params;
    }
    
    std::map<std::string, ConfigValue> getParameters() const override {
        return parameters_;
    }
    
private:
    std::string name_;
    std::function<ConfigValue(const std::vector<ConfigValue>&)> executor_;
    std::map<std::string, ConfigValue> parameters_;
};

// Concrete implementation of neural model
class SimpleNeuralModel : public NeuralModel {
public:
    SimpleNeuralModel(const std::string& name, const std::string& type)
        : name_(name), type_(type) {}
    
    ~SimpleNeuralModel() override = default;
    
    std::vector<float> process(const std::vector<float>& input) override {
        // Simple processing: apply weights and activation
        std::vector<float> output(input.size(), 0.0f);
        for (size_t i = 0; i < input.size(); ++i) {
            output[i] = input[i] * weights_[i];
            // Simple activation function
            output[i] = 1.0f / (1.0f + std::exp(-output[i]));
        }
        return output;
    }
    
    const std::string& getName() const override {
        return name_;
    }
    
    const std::string& getType() const override {
        return type_;
    }
    
    void setParameters(const std::map<std::string, float>& params) override {
        parameters_ = params;
        // Update weights from parameters
        for (const auto& param : parameters_) {
            if (param.first.find("weight_") == 0) {
                size_t index = std::stoi(param.first.substr(7));
                if (index < weights_.size()) {
                    weights_[index] = param.second;
                }
            }
        }
    }
    
    std::map<std::string, float> getParameters() const override {
        return parameters_;
    }
    
private:
    std::string name_;
    std::string type_;
    std::vector<float> weights_ = {1.0f, 1.0f, 1.0f};  // Default weights
    std::map<std::string, float> parameters_;
};

// Concrete implementation of neuromodulator
class SimpleNeuromodulator : public Neuromodulator {
public:
    SimpleNeuromodulator(const std::string& name, const std::string& type)
        : name_(name), type_(type) {}
    
    ~SimpleNeuromodulator() override = default;
    
    void apply(NeuronId neuronId, float signal) override {
        // Store neuromodulatory signal
        signals_[neuronId] = signal;
    }
    
    const std::string& getName() const override {
        return name_;
    }
    
    const std::string& getType() const override {
        return type_;
    }
    
    void setStrength(float strength) override {
        strength_ = strength;
    }
    
    float getStrength() const override {
        return strength_;
    }
    
    bool isActive() const override {
        return active_;
    }
    
    void activate() override {
        active_ = true;
    }
    
    void deactivate() override {
        active_ = false;
    }
    
    float getSignal(NeuronId neuronId) const {
        auto it = signals_.find(neuronId);
        return (it != signals_.end()) ? it->second : 0.0f;
    }
    
private:
    std::string name_;
    std::string type_;
    float strength_ = 1.0f;
    bool active_ = false;
    std::map<NeuronId, float> signals_;
};

} // namespace nlm
