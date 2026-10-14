#pragma once

/**
 * @file PlasticityRuleFactory.hpp
 * @brief Factory system for creating and managing plasticity rules
 * @author NLM Project
 * @date Phase 4
 * 
 * This file defines the plasticity rule factory system, providing a flexible
 * and extensible architecture for creating, configuring, and managing
 * synaptic plasticity rules in the NLM platform.
 * 
 * The factory system includes:
 * - Abstract factory for creating plasticity rules
 * - Concrete factories for built-in rule types
 * - Configuration-based rule creation
 * - Rule registry and plugin architecture
 * - Rule parameter validation and setting
 * - Performance-optimized rule lookup
 * 
 * This system addresses the repetitive code patterns in individual
 * plasticity rule implementations by providing common functionality
 * and a consistent interface for all rule types.
 * 
 * @note This factory system uses the Factory Method pattern to enable
 *       easy extension of new plasticity rules without modifying
 *       core simulation code.
 */

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <any>
#include <typeindex>
#include <cassert>
#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @brief Abstract base factory for creating plasticity rules
 */
class IPlasticityRuleFactory {
public:
    virtual ~IPlasticityRuleFactory() = default;
    
    /**
     * @brief Create a plasticity rule instance
     * 
     * @tparam RuleType The type of rule to create
     * @tparam Args Constructor argument types
     * @param args Constructor arguments
     * @return Unique pointer to created rule instance
     */
    template<typename RuleType, typename... Args>
    static std::unique_ptr<RuleType> create(Args&&... args) {
        return std::unique_ptr<RuleType>(new RuleType(std::forward<Args>(args)...));
    }
    
    /**
     * @brief Create a plasticity rule from configuration
     * 
     * @param ruleType Type of rule to create
     * @param config Configuration parameters
     * @return Unique pointer to created rule
     */
    virtual std::unique_ptr<PlasticityRule> createFromConfig(
        const std::string& ruleType,
        const std::unordered_map<std::string, std::any>& config) = 0;
    
    /**
     * @brief Get the name of this factory
     * 
     * @return Factory name
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if factory can create the specified rule type
     * 
     * @param ruleType Rule type to check
     * @return true if factory supports this rule type
     */
    virtual bool canCreate(const std::string& ruleType) const = 0;
};

/**
 * @brief Factory for built-in plasticity rule types
 */
class BuiltInPlasticityRuleFactory : public IPlasticityRuleFactory {
public:
    BuiltInPlasticityRuleFactory();
    ~BuiltInPlasticityRuleFactory() override;
    
    std::unique_ptr<PlasticityRule> createFromConfig(
        const std::string& ruleType,
        const std::unordered_map<std::string, std::any>& config) override;
    
    const char* getName() const override;
    
    bool canCreate(const std::string& ruleType) const override;
    
    /**
     * @brief Create STDP rule with parameters
     * 
     * @param ltpWeight Long-term potentiation weight
     * @param ltdWeight Long-term depression weight
     * @param timeConstant STDP time constant (ms)
     * @return Unique pointer to STDP rule
     */
    std::unique_ptr<STDP> createSTDP(float ltpWeight = 0.01f, float ltdWeight = 0.012f,
                                      float timeConstant = 20.0f);
    
    /**
     * @brief Create Hebbian rule with parameters
     * 
     * @param learningRate Learning rate
     * @param maxWeight Maximum synaptic weight
     * @param minWeight Minimum synaptic weight
     * @return Unique pointer to Hebbian rule
     */
    std::unique_ptr<Hebbian> createHebbian(float learningRate = 0.01f,
                                            float maxWeight = 1.0f,
                                            float minWeight = -1.0f);
    
    /**
     * @brief Create Anti-Hebbian rule
     * 
     * @return Unique pointer to Anti-Hebbian rule
     */
    std::unique_ptr<AntiHebbianRule> createAntiHebbian();
    
    /**
     * @brief Create BCM rule with threshold parameter
     * 
     * @param bcmThreshold BCM threshold parameter
     * @return Unique pointer to BCM rule
     */
    std::unique_ptr<BCMRule> createBCM(float bcmThreshold = 0.0f);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Rule configuration template
 * 
 * Provides a type-safe way to configure plasticity rules with validation.
 */
template<typename RuleType>
class RuleConfiguration {
public:
    RuleConfiguration() = default;
    
    /**
     * @brief Add a parameter to the configuration
     * 
     * @tparam T Parameter type
     * @param name Parameter name
     * @param value Parameter value
     * @return Reference to this configuration
     */
    template<typename T>
    RuleConfiguration& add(const std::string& name, const T& value) {
        config_[name] = value;
        return *this;
    }
    
    /**
     * @brief Get a parameter from configuration
     * 
     * @tparam T Parameter type
     * @param name Parameter name
     * @return Parameter value
     * @throws std::invalid_argument if parameter not found
     */
    template<typename T>
    T get(const std::string& name) const {
        auto it = config_.find(name);
        if (it == config_.end()) {
            throw std::invalid_argument("Parameter not found in configuration");
        }
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            throw std::invalid_argument("Parameter type mismatch");
        }
    }
    
    /**
     * @brief Get a parameter with default value
     * 
     * @tparam T Parameter type
     * @param name Parameter name
     * @param defaultValue Default value if parameter not found
     * @return Parameter value
     */
    template<typename T>
    T get(const std::string& name, const T& defaultValue) const {
        auto it = config_.find(name);
        if (it == config_.end()) {
            return defaultValue;
        }
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            return defaultValue;
        }
    }
    
    /**
     * @brief Get all configuration parameters
     * 
     * @return Constant reference to parameter map
     */
    const std::unordered_map<std::string, std::any>& getAll() const {
        return config_;
    }
    
    /**
     * @brief Check if parameter exists
     * 
     * @param name Parameter name
     * @return true if parameter exists
     */
    bool has(const std::string& name) const {
        return config_.find(name) != config_.end();
    }
    
    /**
     * @brief Validate configuration against rule requirements
     * 
     * @param rule Rule instance to validate against
     * @throws ValidationError if configuration is invalid
     */
    void validate(const PlasticityRule& rule) const {
        // Base validation - can be overridden by specific rule types
        for (const auto& [name, value] : config_) {
            // Validate parameter ranges and types
            validateParameter(name, value, rule);
        }
    }
    
protected:
    virtual void validateParameter(const std::string& name, const std::any& value,
                                   const PlasticityRule& rule) const {
        // Default validation - check basic types
        if (name == "learningRate") {
            float lr = std::any_cast<float>(value);
            if (lr < 0.0f || lr > 1.0f) {
                throw ValidationError("Learning rate must be in range [0.0, 1.0]",
                                     "learningRate", "[0.0, 1.0]", std::to_string(lr));
            }
        } else if (name == "maxWeight") {
            float mw = std::any_cast<float>(value);
            if (mw < -10.0f || mw > 10.0f) {
                throw ValidationError("Max weight must be in reasonable range",
                                     "maxWeight", "[-10.0, 10.0]", std::to_string(mw));
            }
        }
        // Add more parameter-specific validation as needed
    }

private:
    std::unordered_map<std::string, std::any> config_;
};

/**
 * @brief Plasticity rule registry for plugin architecture
 * 
 * Manages registered plasticity rule types and their factories,
 * enabling runtime loading of new rule types.
 */
class PlasticityRuleRegistry {
public:
    static PlasticityRuleRegistry& getInstance();
    
    /**
     * @brief Register a plasticity rule type with a factory
     * 
     * @param ruleType Rule type name
     * @param factory Factory instance
     */
    void registerRuleType(const std::string& ruleType,
                         std::unique_ptr<IPlasticityRuleFactory> factory);
    
    /**
     * @brief Check if rule type is registered
     * 
     * @param ruleType Rule type name
     * @return true if rule type is registered
     */
    bool isRegistered(const std::string& ruleType) const;
    
    /**
     * @brief Get factory for rule type
     * 
     * @param ruleType Rule type name
     * @return Pointer to factory (nullptr if not found)
     */
    IPlasticityRuleFactory* getFactory(const std::string& ruleType) const;
    
    /**
     * @brief Get all registered rule types
     * 
     * @return Vector of rule type names
     */
    std::vector<std::string> getRegisteredRuleTypes() const;
    
    /**
     * @brief Create a rule of specified type
     * 
     * @param ruleType Rule type name
     * @param config Configuration parameters
     * @return Unique pointer to created rule
     */
    std::unique_ptr<PlasticityRule> createRule(const std::string& ruleType,
                                              const RuleConfiguration<PlasticityRule>& config);
    
    /**
     * @brief Create a rule with string configuration
     * 
     * @param ruleType Rule type name
     * @param config Configuration parameters as string map
     * @return Unique pointer to created rule
     */
    std::unique_ptr<PlasticityRule> createRule(
        const std::string& ruleType,
        const std::unordered_map<std::string, std::string>& config);
    
    /**
     * @brief Remove all registered rule types
     */
    void clear();

private:
    PlasticityRuleRegistry() = default;
    ~PlasticityRuleRegistry() = default;
    
    // Disable copying
    PlasticityRuleRegistry(const PlasticityRuleRegistry&) = delete;
    PlasticityRuleRegistry& operator=(const PlasticityRuleRegistry&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Convenience functions for common plasticity rule operations
 */
namespace PlasticityRuleFactory {

/**
 * @brief Create a plasticity rule of specified type
 * 
 * @tparam RuleType The type of rule to create
 * @tparam Args Constructor argument types
 * @param args Constructor arguments
 * @return Unique pointer to created rule
 */
template<typename RuleType, typename... Args>
inline std::unique_ptr<RuleType> create(Args&&... args) {
    return BuiltInPlasticityRuleFactory::create<RuleType>(std::forward<Args>(args)...);
}

/**
 * @brief Create a plasticity rule from configuration
 * 
 * @param ruleType Rule type name
 * @param config Configuration parameters
 * @return Unique pointer to created rule
 */
inline std::unique_ptr<PlasticityRule> createRule(
    const std::string& ruleType,
    const RuleConfiguration<PlasticityRule>& config) {
    return PlasticityRuleRegistry::getInstance().createRule(ruleType, config);
}

/**
 * @brief Create a plasticity rule from string configuration
 * 
 * @param ruleType Rule type name
 * @param config Configuration parameters
 * @return Unique pointer to created rule
 */
inline std::unique_ptr<PlasticityRule> createRule(
    const std::string& ruleType,
    const std::unordered_map<std::string, std::string>& config) {
    return PlasticityRuleRegistry::getInstance().createRule(ruleType, config);
}

/**
 * @brief Get list of available rule types
 * 
 * @return Vector of rule type names
 */
inline std::vector<std::string> getAvailableRuleTypes() {
    return PlasticityRuleRegistry::getInstance().getRegisteredRuleTypes();
}

/**
 * @brief Register a custom rule type
 * 
 * @param ruleType Rule type name
 * @param factory Factory for this rule type
 */
inline void registerRuleType(const std::string& ruleType,
                            std::unique_ptr<IPlasticityRuleFactory> factory) {
    PlasticityRuleRegistry::getInstance().registerRuleType(ruleType, std::move(factory));
}

/**
 * @brief Initialize built-in rule types
 * 
 * @note This function should be called during application startup
 */
inline void initializeBuiltInRuleTypes() {
    static bool initialized = false;
    if (!initialized) {
        BuiltInPlasticityRuleFactory factory;
        // Register built-in rule types
        PlasticityRuleRegistry::getInstance().registerRuleType("STDP", std::make_unique<BuiltInPlasticityRuleFactory>(factory));
        PlasticityRuleRegistry::getInstance().registerRuleType("Hebbian", std::make_unique<BuiltInPlasticityRuleFactory>(factory));
        PlasticityRuleRegistry::getInstance().registerRuleType("AntiHebbian", std::make_unique<BuiltInPlasticityRuleFactory>(factory));
        PlasticityRuleRegistry::getInstance().registerRuleType("BCM", std::make_unique<BuiltInPlasticityRuleFactory>(factory));
        initialized = true;
    }
}

} // namespace PlasticityRuleFactory

} // namespace nlm

// Explicit instantiations
namespace nlm {
    template class RuleConfiguration<PlasticityRule>;
}