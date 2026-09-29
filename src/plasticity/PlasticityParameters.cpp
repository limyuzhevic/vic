#include "PlasticityParameters.hpp"
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace nlm {

PlasticityParameters::PlasticityParameters(const std::unordered_map<std::string, std::any>& data)
    : data_(data) {}

void PlasticityParameters::set(const std::string& name, int value) {
    data_[name] = value;
}

void PlasticityParameters::set(const std::string& name, float value) {
    data_[name] = value;
}

void PlasticityParameters::set(const std::string& name, double value) {
    data_[name] = value;
}

void PlasticityParameters::set(const std::string& name, const std::string& value) {
    data_[name] = value;
}

void PlasticityParameters::set(const std::string& name, bool value) {
    data_[name] = value;
}

void PlasticityParameters::set(const std::string& name, const std::vector<float>& values) {
    data_[name] = values;
}

void PlasticityParameters::set(const std::string& name, const std::vector<std::string>& values) {
    data_[name] = values;
}

bool PlasticityParameters::hasParameter(const std::string& name) const {
    return data_.find(name) != data_.end();
}

template<typename T>
T PlasticityParameters::getParameter(const std::string& name, const std::any& defaultValue) const {
    auto it = data_.find(name);
    if (it == data_.end()) {
        if (!defaultValue.has_value()) {
            throw std::runtime_error("Parameter not found and no default value provided: " + name);
        }
        return std::any_cast<T>(defaultValue);
    }
    try {
        return std::any_cast<T>(it->second);
    } catch (const std::bad_any_cast&) {
        throw std::runtime_error("Type mismatch for parameter: " + name);
    }
}

int PlasticityParameters::getInt(const std::string& name, int defaultValue) const {
    try {
        return getParameter<int>(name, defaultValue);
    } catch (const std::runtime_error& e) {
        addError("getInt failed for parameter '" + name + "': " + e.what());
        return defaultValue;
    }
}

float PlasticityParameters::getFloat(const std::string& name, float defaultValue) const {
    try {
        return getParameter<float>(name, defaultValue);
    } catch (const std::runtime_error& e) {
        addError("getFloat failed for parameter '" + name + "': " + e.what());
        return defaultValue;
    }
}

double PlasticityParameters::getDouble(const std::string& name, double defaultValue) const {
    try {
        return getParameter<double>(name, defaultValue);
    } catch (const std::runtime_error& e) {
        addError("getDouble failed for parameter '" + name + "': " + e.what());
        return defaultValue;
    }
}

std::string PlasticityParameters::getString(const std::string& name, const std::string& defaultValue) const {
    try {
        return getParameter<std::string>(name, defaultValue);
    } catch (const std::runtime_error& e) {
        addError("getString failed for parameter '" + name + "': " + e.what());
        return defaultValue;
    }
}

bool PlasticityParameters::getBool(const std::string& name, bool defaultValue) const {
    try {
        return getParameter<bool>(name, defaultValue);
    } catch (const std::runtime_error& e) {
        addError("getBool failed for parameter '" + name + "': " + e.what());
        return defaultValue;
    }
}

std::vector<float> PlasticityParameters::getVectorFloat(const std::string& name,
                                                       const std::vector<float>& defaultValue) const {
    try {
        auto it = data_.find(name);
        if (it == data_.end()) {
            return defaultValue;
        }
        return std::any_cast<std::vector<float>>(it->second);
    } catch (const std::bad_any_cast&) {
        addError("Type mismatch for vector<float> parameter: " + name);
        return defaultValue;
    }
}

std::vector<std::string> PlasticityParameters::getVectorString(const std::string& name,
                                                               const std::vector<std::string>& defaultValue) const {
    try {
        auto it = data_.find(name);
        if (it == data_.end()) {
            return defaultValue;
        }
        return std::any_cast<std::vector<std::string>>(it->second);
    } catch (const std::bad_any_cast&) {
        addError("Type mismatch for vector<string> parameter: " + name);
        return defaultValue;
    }
}

PlasticityParameters::ParameterType PlasticityParameters::getParameterType(const std::string& name) const {
    auto it = data_.find(name);
    if (it == data_.end()) {
        return ParameterType::Int; // Default type
    }
    
    const std::any& value = it->second;
    if (std::any_cast<int>(&value) != nullptr) return ParameterType::Int;
    if (std::any_cast<float>(&value) != nullptr) return ParameterType::Float;
    if (std::any_cast<double>(&value) != nullptr) return ParameterType::Double;
    if (std::any_cast<std::string>(&value) != nullptr) return ParameterType::String;
    if (std::any_cast<bool>(&value) != nullptr) return ParameterType::Bool;
    if (std::any_cast<std::vector<float>>(&value) != nullptr) return ParameterType::VectorFloat;
    if (std::any_cast<std::vector<std::string>>(&value) != nullptr) return ParameterType::VectorString;
    
    return ParameterType::Int; // Unknown type
}

bool PlasticityParameters::isParameterValid(const std::string& name) const {
    auto it = data_.find(name);
    return it != data_.end();
}

void PlasticityParameters::removeParameter(const std::string& name) {
    data_.erase(name);
}

void PlasticityParameters::clear() {
    data_.clear();
    errors_.clear();
}

std::string PlasticityParameters::toString() const {
    std::stringstream ss;
    ss << "PlasticityParameters: {\n";
    for (const auto& pair : data_) {
        ss << "  " << pair.first << ": ";
        if (std::any_cast<int>(&pair.second) != nullptr) {
            ss << std::any_cast<int>(pair.second);
        } else if (std::any_cast<float>(&pair.second) != nullptr) {
            ss << std::any_cast<float>(pair.second);
        } else if (std::any_cast<double>(&pair.second) != nullptr) {
            ss << std::any_cast<double>(pair.second);
        } else if (std::any_cast<std::string>(&pair.second) != nullptr) {
            ss << "\"" << std::any_cast<std::string>(pair.second) << "\"";
        } else if (std::any_cast<bool>(&pair.second) != nullptr) {
            ss << (std::any_cast<bool>(pair.second) ? "true" : "false");
        } else if (std::any_cast<std::vector<float>>(&pair.second) != nullptr) {
            const auto& vec = std::any_cast<std::vector<float>>(pair.second);
            ss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
        } else if (std::any_cast<std::vector<std::string>>(&pair.second) != nullptr) {
            const auto& vec = std::any_cast<std::vector<std::string>>(pair.second);
            ss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << vec[i] << "\"";
            }
            ss << "]";
        }
        ss << "\n";
    }
    ss << "}";
    if (!errors_.empty()) {
        ss << "\nErrors: ";
        for (const auto& error : errors_) {
            ss << "\n  - " << error;
        }
    }
    return ss.str();
}

bool PlasticityParameters::validate() const {
    errors_.clear();
    // Basic validation - in a real implementation, would validate against rule-specific constraints
    return !hasErrors();
}

std::unordered_map<std::string, std::any> PlasticityParameters::toMap() const {
    return data_;
}

PlasticityParameters PlasticityParameters::fromMap(const std::unordered_map<std::string, std::any>& data) {
    return PlasticityParameters(data);
}

PlasticityParameters PlasticityParameters::getDefaultSTDPParameters() {
    PlasticityParameters params;
    params.set("ltp_weight", 0.01f);
    params.set("ltd_weight", 0.012f);
    params.set("time_constant", 20.0f);
    return params;
}

PlasticityParameters PlasticityParameters::getDefaultHebbianParameters() {
    PlasticityParameters params;
    params.set("learning_rate", 0.01f);
    params.set("max_weight", 1.0f);
    params.set("min_weight", -1.0f);
    params.set("covariance_threshold", 0.0f);
    return params;
}

PlasticityParameters PlasticityParameters::getDefaultR_STDPParameters() {
    PlasticityParameters params;
    params.set("ltp_weight", 0.01f);
    params.set("ltd_weight", 0.012f);
    params.set("time_constant", 20.0f);
    params.set("dopamine_scale", 1.0f);
    return params;
}

PlasticityParameters PlasticityParameters::getDefaultTripletSTDPParameters() {
    PlasticityParameters params;
    params.set("a_plus", 0.025f);
    params.set("a_minus", 0.0275f);
    params.set("a_u", 0.005f);
    params.set("tau_plus", 20.0f);
    params.set("tau_minus", 20.0f);
    params.set("tau_u", 200.0f);
    return params;
}

bool PlasticityParameters::validateForRule(const std::string& ruleName, 
                                         const PlasticityParameters& params) {
    // Basic validation - in a real implementation, would validate each parameter
    // against the specific rule's constraints and ranges
    if (ruleName.empty()) {
        return false;
    }
    
    // Check for essential parameters
    if (ruleName == "STDP") {
        return params.hasParameter("ltp_weight") && params.hasParameter("ltd_weight") && 
               params.hasParameter("time_constant");
    } else if (ruleName == "Hebbian") {
        return params.hasParameter("learning_rate") && params.hasParameter("max_weight") &&
               params.hasParameter("min_weight");
    } else if (ruleName == "R-STDP") {
        return params.hasParameter("ltp_weight") && params.hasParameter("ltd_weight") &&
               params.hasParameter("time_constant") && params.hasParameter("dopamine_scale");
    } else if (ruleName == "TripletSTDP") {
        return params.hasParameter("a_plus") && params.hasParameter("a_minus") &&
               params.hasParameter("a_u") && params.hasParameter("tau_plus") &&
               params.hasParameter("tau_minus") && params.hasParameter("tau_u");
    }
    
    return true; // Unknown rule type - assume valid
}

// Static member definition
const std::unordered_map<std::string, PlasticityParameters::RuleParameters>& 
PlasticityParameters::getRuleParameterDefinitions() {
    static const std::unordered_map<std::string, RuleParameters> definitions = {
        {"STDP", RuleParameters("STDP", 
                               "Spike-Timing-Dependent Plasticity based on pre-post spike timing")},
        {"Hebbian", RuleParameters("Hebbian", 
                                  "Hebbian learning: neurons that fire together wire together")},
        {"R-STDP", RuleParameters("R-STDP", 
                                 "Reward-modulated STDP with dopamine signals")},
        {"TripletSTDP", RuleParameters("TripletSTDP", 
                                       "STDP with triplet interactions and calcium dynamics")},
    };
    return definitions;
}

} // namespace nlm