#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "Experiment.hpp"

namespace nlm {

// Parameter types for experiment configuration
using ParameterValue = std::variant<int, float, double, std::string, bool, std::vector<float>>;

// Parameter constraints for validation
struct ParameterConstraints {
    std::string name;
    ParameterValue defaultValue;
    std::optional<ParameterValue> minValue;
    std::optional<ParameterValue> maxValue;
    std::string description;
    bool required = false;
    std::vector<ParameterValue> allowedValues;
    std::string unit;
};

// Experiment template for common use cases
struct ExperimentTemplate {
    std::string name;
    std::string description;
    std::map<std::string, ParameterValue> parameters;
    std::map<std::string, std::string> parameterTypes;
};

// Enhanced ExperimentConfig class with advanced features
class ExperimentConfig {
public:
    ExperimentConfig();
    ExperimentConfig(const std::string& name);
    
    // Basic configuration
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Experiment parameters with type safety
    template<typename T>
    void setParameter(const std::string& key, const T& value);
    
    template<typename T>
    T getParameter(const std::string& key) const;
    
    bool hasParameter(const std::string& key) const;
    
    // Common experiment parameters
    void setNeuronCount(size_t count);
    size_t getNeuronCount() const;
    
    void setSimulationSteps(SimulationStep steps);
    SimulationStep getSimulationSteps() const;
    
    void setPlasticityRate(float rate);
    float getPlasticityRate() const;
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setDt(float dt);
    float getDt() const;
    
    // Template/preset support
    void addTemplate(const ExperimentTemplate& template_);
    ExperimentTemplate getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;
    std::vector<std::string> getAvailableTemplates() const;
    
    // Parameter validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Export/import support
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    std::string toString() const;
    
    // Preset configurations
    static ExperimentConfig createSmallScale();
    static ExperimentConfig createMediumScale();
    static ExperimentConfig createLargeScale();
    static ExperimentConfig createConnectome(RegionId region);
    static ExperimentConfig createPlasticity();
    static ExperimentConfig createLearning();
    static ExperimentConfig createTiming();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Template parameter type converters
namespace ParameterType {
    template<typename T>
    std::string toTypeName();
    
    template<typename T>
    ParameterValue convertToVariant(const T& value);
    
    template<typename T>
    T convertFromVariant(const ParameterValue& variant);
}

} // namespace nlm
