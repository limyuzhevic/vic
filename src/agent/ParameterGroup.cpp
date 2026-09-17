// ParameterGroup implementation
#include "../agent/ModularityLayer.hpp"
#include <iostream>
#include <stdexcept>

namespace nlm {

ParameterGroup::ParameterGroup(const std::string& name, const std::string& description)
    : name_(name), description_(description) {}

template<typename T>
void ParameterGroup::addParameter(const std::string& key, T defaultValue,
                                 const std::string& description,
                                 std::function<bool(const T&)> validator) {
    parameterKeys_.push_back(key);
    parameters_.emplace_back(key, defaultValue, description, validator);
}

template<typename T>
bool ParameterGroup::validateParameter(const std::string& key, const T& value) const {
    for (const auto& param : parameters_) {
        if (param.key == key) {
            if (param.validator) {
                return param.validator(value);
            }
            return true; // No validator, assume valid
        }
    }
    throw std::runtime_error("Parameter not found: " + key);
}

template<typename T>
T ParameterGroup::getParameter(const std::string& key) const {
    for (const auto& param : parameters_) {
        if (param.key == key) {
            try {
                return std::any_cast<T>(param.currentValue);
            } catch (const std::bad_any_cast&) {
                throw std::runtime_error("Type mismatch for parameter: " + key);
            }
        }
    }
    throw std::runtime_error("Parameter not found: " + key);
}

template<typename T>
void ParameterGroup::setParameter(const std::string& key, const T& value) {
    for (auto& param : parameters_) {
        if (param.key == key) {
            if (param.validator && !param.validator(value)) {
                throw std::runtime_error("Validation failed for parameter: " + key);
            }
            param.currentValue = value;
            return;
        }
    }
    throw std::runtime_error("Parameter not found: " + key);
}

// Specializations for common types
void ParameterGroup::addParameter(const std::string& key, bool defaultValue,
                                 const std::string& description,
                                 std::function<bool(bool)> validator) {
    parameterKeys_.push_back(key);
    parameters_.emplace_back(key, defaultValue, description, 
                           [validator](const std::any& v) {
                               if (!validator) return true;
                               try {
                                   return validator(std::any_cast<bool>(v));
                               } catch (...) {
                                   return false;
                               }
                           });
    parameters_.back().currentValue = defaultValue;
}

void ParameterGroup::addParameter(const std::string& key, int defaultValue,
                                 const std::string& description,
                                 std::function<bool(int)> validator) {
    parameterKeys_.push_back(key);
    parameters_.emplace_back(key, defaultValue, description,
                           [validator](const std::any& v) {
                               if (!validator) return true;
                               try {
                                   return validator(std::any_cast<int>(v));
                               } catch (...) {
                                   return false;
                               }
                           });
    parameters_.back().currentValue = defaultValue;
}

void ParameterGroup::addParameter(const std::string& key, float defaultValue,
                                 const std::string& description,
                                 std::function<bool(float)> validator) {
    parameterKeys_.push_back(key);
    parameters_.emplace_back(key, defaultValue, description,
                           [validator](const std::any& v) {
                               if (!validator) return true;
                               try {
                                   return validator(std::any_cast<float>(v));
                               } catch (...) {
                                   return false;
                               }
                           });
    parameters_.back().currentValue = defaultValue;
}

void ParameterGroup::addParameter(const std::string& key, double defaultValue,
                                 const std::string& description,
                                 std::function<bool(double)> validator) {
    parameterKeys_.push_back(key);
    parameters_.emplace_back(key, defaultValue, description,
                           [validator](const std::any& v) {
                               if (!validator) return true;
                               try {
                                   return validator(std::any_cast<double>(v));
                               } catch (...) {
                                   return false;
                               }
                           });
    parameters_.back().currentValue = defaultValue;
}

void ParameterGroup::addParameter(const std::string& key, std::string defaultValue,
                                 const std::string& description,
                                 std::function<bool(const std::string&)> validator) {
    parameterKeys_.push_back(key);
    parameters_.emplace_back(key, defaultValue, description,
                           [validator](const std::any& v) {
                               if (!validator) return true;
                               try {
                                   return validator(std::any_cast<std::string>(v));
                               } catch (...) {
                                   return false;
                               }
                           });
    parameters_.back().currentValue = defaultValue;
}
