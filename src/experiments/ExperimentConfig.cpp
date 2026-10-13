#include "ExperimentConfig.hpp"

namespace nlm {

struct ExperimentConfig::Impl {
    std::string name;
    std::map<std::string, ParameterValue> parameters;
    std::map<std::string, std::string> parameterTypes;
    std::map<std::string, ParameterConstraints> constraints;
    std::map<std::string, ExperimentTemplate> templates;
    
    Impl() {
        // Initialize with common experiment parameters
        initializeDefaultParameters();
        initializeDefaultTemplates();
    }
    
    void initializeDefaultParameters() {
        // Core simulation parameters
        constraints["neuron_count"] = {"neuron_count", 1000u, 1u, 1000000u, "Number of neurons", true, {}, "neurons"};
        constraints["simulation_steps"] = {"simulation_steps", 10000u, 1u, 1000000u, "Number of simulation steps", true, {}, "steps"};
        constraints["plasticity_rate"] = {"plasticity_rate", 0.01f, 0.0f, 1.0f, "Plasticity learning rate", false, {}, "rate"};
        constraints["learning_rate"] = {"learning_rate", 0.001f, 0.0f, 1.0f, "Learning rate", false, {}, "rate"};
        constraints["dt"] = {"dt", 0.01f, 0.0001f, 1.0f, "Time step size", false, {}, "ms"};
        
        // Neural properties
        constraints["excitation_inhibition_ratio"] = {"excitation_inhibition_ratio", 4.0f, 1.0f, 10.0f, "E/I balance ratio", false, {}, "ratio"};
        constraints["connection_probability"] = {"connection_probability", 0.1f, 0.0f, 1.0f, "Synaptic connection probability", false, {}, "probability"};
        constraints["delay_range"] = {"delay_range", std::vector<float>{1.0f, 10.0f}, std::vector<float>{0.0f}, std::vector<float>{1000.0f}, "Connection delay range", false, {}, "ms"};
        
        // Recording and output
        constraints["record_interval"] = {"record_interval", 100u, 1u, 10000u, "Recording interval", false, {}, "steps"};
        constraints["save_brain_state"] = {"save_brain_state", false, {}, {}, "Enable brain state saving", false, {}, "bool"};
        constraints["checkpoint_interval"] = {"checkpoint_interval", 1000u, 100u, 100000u, "Checkpoint saving interval", false, {}, "steps"};
    }
    
    void initializeDefaultTemplates() {
        // Small scale template
        ExperimentTemplate smallScale;
        smallScale.name = "small_scale";
        smallScale.description = "Small-scale neural network (1K neurons)";
        smallScale.parameters = {
            {"neuron_count", 1000u},
            {"simulation_steps", 10000u},
            {"plasticity_rate", 0.01f},
            {"learning_rate", 0.001f},
            {"dt", 0.01f},
            {"connection_probability", 0.1f}
        };
        smallScale.parameterTypes = {
            {"neuron_count", "size_t"},
            {"simulation_steps", "SimulationStep"},
            {"plasticity_rate", "float"},
            {"learning_rate", "float"},
            {"dt", "float"},
            {"connection_probability", "float"}
        };
        templates["small_scale"] = smallScale;
        
        // Medium scale template
        ExperimentTemplate mediumScale;
        mediumScale.name = "medium_scale";
        mediumScale.description = "Medium-scale neural network (10K neurons)";
        mediumScale.parameters = {
            {"neuron_count", 10000u},
            {"simulation_steps", 50000u},
            {"plasticity_rate", 0.05f},
            {"learning_rate", 0.005f},
            {"dt", 0.005f},
            {"connection_probability", 0.05f},
            {"record_interval", 1000u}
        };
        mediumScale.parameterTypes = {
            {"neuron_count", "size_t"},
            {"simulation_steps", "SimulationStep"},
            {"plasticity_rate", "float"},
            {"learning_rate", "float"},
            {"dt", "float"},
            {"connection_probability", "float"},
            {"record_interval", "size_t"}
        };
        templates["medium_scale"] = mediumScale;
        
        // Large scale template
        ExperimentTemplate largeScale;
        largeScale.name = "large_scale";
        largeScale.description = "Large-scale neural network (100K neurons)";
        largeScale.parameters = {
            {"neuron_count", 100000u},
            {"simulation_steps", 200000u},
            {"plasticity_rate", 0.1f},
            {"learning_rate", 0.01f},
            {"dt", 0.001f},
            {"connection_probability", 0.02f},
            {"save_brain_state", true},
            {"checkpoint_interval", 50000u}
        };
        largeScale.parameterTypes = {
            {"neuron_count", "size_t"},
            {"simulation_steps", "SimulationStep"},
            {"plasticity_rate", "float"},
            {"learning_rate", "float"},
            {"dt", "float"},
            {"connection_probability", "float"},
            {"save_brain_state", "bool"},
            {"checkpoint_interval", "size_t"}
        };
        templates["large_scale"] = largeScale;
    }
    
    template<typename T>
    std::string getTypeName(const T& value) const {
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, float>) return "float";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::vector<float>>) return "vector<float>";
        else if constexpr (std::is_same_v<T, size_t>) return "size_t";
        else if constexpr (std::is_same_v<T, SimulationStep>) return "SimulationStep";
        return "unknown";
    }
};

ExperimentConfig::ExperimentConfig() : pImpl(new Impl) {}

ExperimentConfig::ExperimentConfig(const std::string& name) : pImpl(new Impl) {
    pImpl->name = name;
}

const std::string& ExperimentConfig::getName() const {
    return pImpl->name;
}

void ExperimentConfig::setName(const std::string& name) {
    pImpl->name = name;
}

template<typename T>
void ExperimentConfig::setParameter(const std::string& key, const T& value) {
    if (pImpl->constraints.find(key) == pImpl->constraints.end()) {
        // Auto-register parameter if not in constraints
        pImpl->constraints[key] = {key, ParameterValue(value), {}, {}, "Auto-registered parameter"};
    }
    
    pImpl->parameters[key] = ParameterValue(value);
    pImpl->parameterTypes[key] = pImpl->getTypeName(value);
}

template<typename T>
T ExperimentConfig::getParameter(const std::string& key) const {
    auto it = pImpl->parameters.find(key);
    if (it == pImpl->parameters.end()) {
        throw std::runtime_error("Parameter not found: " + key);
    }
    return std::get<T>(it->second);
}

bool ExperimentConfig::hasParameter(const std::string& key) const {
    return pImpl->parameters.find(key) != pImpl->parameters.end();
}

void ExperimentConfig::setNeuronCount(size_t count) {
    setParameter<size_t>("neuron_count", count);
}

size_t ExperimentConfig::getNeuronCount() const {
    return getParameter<size_t>("neuron_count");
}

void ExperimentConfig::setSimulationSteps(SimulationStep steps) {
    setParameter<SimulationStep>("simulation_steps", steps);
}

SimulationStep ExperimentConfig::getSimulationSteps() const {
    return getParameter<SimulationStep>("simulation_steps");
}

void ExperimentConfig::setPlasticityRate(float rate) {
    setParameter<float>("plasticity_rate", rate);
}

float ExperimentConfig::getPlasticityRate() const {
    return getParameter<float>("plasticity_rate");
}

void ExperimentConfig::setLearningRate(float rate) {
    setParameter<float>("learning_rate", rate);
}

float ExperimentConfig::getLearningRate() const {
    return getParameter<float>("learning_rate");
}

void ExperimentConfig::setDt(float dt) {
    setParameter<float>("dt", dt);
}

float ExperimentConfig::getDt() const {
    return getParameter<float>("dt");
}

void ExperimentConfig::addTemplate(const ExperimentTemplate& template_) {
    pImpl->templates[template_.name] = template_;
}

ExperimentTemplate ExperimentConfig::getTemplate(const std::string& name) const {
    auto it = pImpl->templates.find(name);
    if (it == pImpl->templates.end()) {
        throw std::runtime_error("Template not found: " + name);
    }
    return it->second;
}

bool ExperimentConfig::hasTemplate(const std::string& name) const {
    return pImpl->templates.find(name) != pImpl->templates.end();
}

std::vector<std::string> ExperimentConfig::getAvailableTemplates() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->templates) {
        names.push_back(pair.first);
    }
    return names;
}

bool ExperimentConfig::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> ExperimentConfig::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& pair : pImpl->constraints) {
        const std::string& key = pair.first;
        const ParameterConstraints& constraints = pair.second;
        
        // Check if required parameter is present
        if (constraints.required && pImpl->parameters.find(key) == pImpl->parameters.end()) {
            errors.push_back("Required parameter missing: " + key);
            continue;
        }
        
        // Check if parameter is present (non-required)
        if (pImpl->parameters.find(key) != pImpl->parameters.end()) {
            const ParameterValue& value = pImpl->parameters.at(key);
            
            // Type checking (basic)
            const std::string& expectedType = pImpl->parameterTypes.at(key);
            std::string actualType;
            
            if (std::holds_alternative<int>(value)) actualType = "int";
            else if (std::holds_alternative<float>(value)) actualType = "float";
            else if (std::holds_alternative<double>(value)) actualType = "double";
            else if (std::holds_alternative<std::string>(value)) actualType = "string";
            else if (std::holds_alternative<bool>(value)) actualType = "bool";
            else if (std::holds_alternative<std::vector<float>>(value)) actualType = "vector<float>";
            else if (std::holds_alternative<size_t>(value)) actualType = "size_t";
            else if (std::holds_alternative<SimulationStep>(value)) actualType = "SimulationStep";
            
            if (actualType != expectedType && expectedType != "unknown") {
                errors.push_back("Parameter type mismatch for " + key + ": expected " + expectedType + ", got " + actualType);
            }
            
            // Value constraints
            if (constraints.minValue.has_value()) {
                // Basic constraint checking (would need more sophisticated implementation for full type support)
            }
            if (constraints.maxValue.has_value()) {
                // Basic constraint checking
            }
            if (!constraints.allowedValues.empty()) {
                // Check if value is in allowed values
            }
        }
    }
    
    return errors;
}

bool ExperimentConfig::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "ExperimentConfig: " << name << "\n";
        file << "Parameters:\n";
        for (const auto& pair : parameters) {
            file << "  " << key << ": " << value << "\n";
        }
        
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ExperimentConfig::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("ExperimentConfig:") == 0) {
                name = line.substr(16);
            } else if (line.find("  ") == 0 && line.size() > 2) {
                size_t colon = line.find(":");
                if (colon != std::string::npos) {
                    std::string key = line.substr(2, colon - 2);
                    std::string value = line.substr(colon + 1);
                    parameters[key] = value;
                }
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string ExperimentConfig::toString() const {
    std::stringstream ss;
    ss << "ExperimentConfig: " << name << "\n";
    ss << "Parameters:\n";
    for (const auto& pair : parameters) {
        ss << "  " << pair.first << ": " << pair.second << "\n";
    }
    return ss.str();
}

// Template instantiations and type converters
namespace ParameterType {
    template<typename T>
    std::string ExperimentConfig::Impl::getTypeName(const T& value) const {
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, float>) return "float";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::vector<float>>) return "vector<float>";
        else if constexpr (std::is_same_v<T, size_t>) return "size_t";
        else if constexpr (std::is_same_v<T, SimulationStep>) return "SimulationStep";
        return "unknown";
    }
    
    template<typename T>
    ParameterValue ParameterType::convertToVariant(const T& value) {
        if constexpr (std::is_same_v<T, int>) return int(value);
        else if constexpr (std::is_same_v<T, float>) return float(value);
        else if constexpr (std::is_same_v<T, double>) return double(value);
        else if constexpr (std::is_same_v<T, std::string>) return std::string(value);
        else if constexpr (std::is_same_v<T, bool>) return bool(value);
        else if constexpr (std::is_same_v<T, std::vector<float>>) return value;
        else if constexpr (std::is_same_v<T, size_t>) return size_t(value);
        else if constexpr (std::is_same_v<T, SimulationStep>) return SimulationStep(value);
        throw std::runtime_error("Unsupported type for ParameterValue conversion");
    }
    
    template<typename T>
    T ParameterType::convertFromVariant(const ParameterValue& variant) {
        return std::get<T>(variant);
    }
}

// Static preset creation methods
ExperimentConfig ExperimentConfig::createSmallScale() {
    ExperimentConfig config("small_scale_experiment");
    config.setParameter<size_t>("neuron_count", 1000);
    config.setParameter<SimulationStep>("simulation_steps", 10000);
    config.setParameter<float>("plasticity_rate", 0.01f);
    config.setParameter<float>("learning_rate", 0.001f);
    config.setParameter<float>("dt", 0.01f);
    config.setParameter<float>("connection_probability", 0.1f);
    return config;
}

ExperimentConfig ExperimentConfig::createMediumScale() {
    ExperimentConfig config("medium_scale_experiment");
    config.setParameter<size_t>("neuron_count", 10000);
    config.setParameter<SimulationStep>("simulation_steps", 50000);
    config.setParameter<float>("plasticity_rate", 0.05f);
    config.setParameter<float>("learning_rate", 0.005f);
    config.setParameter<float>("dt", 0.005f);
    config.setParameter<float>("connection_probability", 0.05f);
    config.setParameter<size_t>("record_interval", 1000);
    return config;
}

ExperimentConfig ExperimentConfig::createLargeScale() {
    ExperimentConfig config("large_scale_experiment");
    config.setParameter<size_t>("neuron_count", 100000);
    config.setParameter<SimulationStep>("simulation_steps", 200000);
    config.setParameter<float>("plasticity_rate", 0.1f);
    config.setParameter<float>("learning_rate", 0.01f);
    config.setParameter<float>("dt", 0.001f);
    config.setParameter<float>("connection_probability", 0.02f);
    config.setParameter<bool>("save_brain_state", true);
    config.setParameter<size_t>("checkpoint_interval", 50000);
    return config;
}

ExperimentConfig ExperimentConfig::createConnectome(RegionId region) {
    ExperimentConfig config("connectome_experiment");
    config.setParameter<size_t>("neuron_count", 50000);
    config.setParameter<SimulationStep>("simulation_steps", 100000);
    config.setParameter<float>("plasticity_rate", 0.2f);
    config.setParameter<float>("learning_rate", 0.02f);
    config.setParameter<float>("dt", 0.01f);
    config.setParameter<float>("excitation_inhibition_ratio", 4.0f);
    config.setParameter<std::string>("target_region", std::to_string(region.value));
    return config;
}

ExperimentConfig ExperimentConfig::createPlasticity() {
    ExperimentConfig config("plasticity_experiment");
    config.setParameter<size_t>("neuron_count", 5000);
    config.setParameter<SimulationStep>("simulation_steps", 50000);
    config.setParameter<float>("plasticity_rate", 0.3f);
    config.setParameter<float>("learning_rate", 0.03f);
    config.setParameter<float>("dt", 0.02f);
    config.setParameter<std::string>("plasticity_type", "stdp");
    return config;
}

ExperimentConfig ExperimentConfig::createLearning() {
    ExperimentConfig config("learning_experiment");
    config.setParameter<size_t>("neuron_count", 2000);
    config.setParameter<SimulationStep>("simulation_steps", 20000);
    config.setParameter<float>("plasticity_rate", 0.05f);
    config.setParameter<float>("learning_rate", 0.005f);
    config.setParameter<float>("dt", 0.01f);
    config.setParameter<std::string>("task_type", "reinforcement");
    return config;
}

ExperimentConfig ExperimentConfig::createTiming() {
    ExperimentConfig config("timing_experiment");
    config.setParameter<size_t>("neuron_count", 3000);
    config.setParameter<SimulationStep>("simulation_steps", 30000);
    config.setParameter<float>("plasticity_rate", 0.02f);
    config.setParameter<float>("learning_rate", 0.002f);
    config.setParameter<float>("dt", 0.005f);
    config.setParameter<std::vector<float>>("delay_range", std::vector<float>{1.0f, 5.0f});
    return config;
}

} // namespace nlm
