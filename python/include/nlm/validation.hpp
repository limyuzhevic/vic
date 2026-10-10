// NLM Python Bindings Validation
// Runtime type checking and validation functions

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace py = pybind11;
namespace nlm {

// Validation functions for Python bindings
namespace validation {

    // Check if a value is within bounds
    template<typename T>
    void check_bounds(const std::string& name, T value, T min, T max) {
        if (value < min || value > max) {
            throw ValidationError("Value for " + name + " must be between " +
                                 std::to_string(min) + " and " + std::to_string(max) +
                                 ", got " + std::to_string(value));
        }
    }

    // Check if a pointer is null
    inline void check_not_null(const std::string& name, void* ptr) {
        if (!ptr) {
            throw ValidationError("Pointer for " + name + " cannot be null");
        }
    }

    // Check if a container is empty when it shouldn't be
    template<typename Container>
    void check_not_empty(const std::string& name, const Container& container) {
        if (container.empty()) {
            throw ValidationError("Container " + name + " cannot be empty");
        }
    }

    // Check if a container has expected size
    template<typename Container>
    void check_size(const std::string& name, const Container& container, size_t expected_size) {
        if (container.size() != expected_size) {
            throw ValidationError("Container " + name + " must have size " +
                                 std::to_string(expected_size) + ", got " +
                                 std::to_string(container.size()));
        }
    }

    // Check if a value matches one of allowed values
    template<typename T>
    void check_allowed(const std::string& name, T value, std::initializer_list<T> allowed) {
        if (std::find(allowed.begin(), allowed.end(), value) == allowed.end()) {
            std::string allowed_str = "";
            for (auto it = allowed.begin(); it != allowed.end(); ++it) {
                if (it != allowed.begin()) allowed_str += ", ";
                allowed_str += std::to_string(*it);
            }
            throw ValidationError("Value for " + name + " must be one of: " + allowed_str +
                                 ", got " + std::to_string(value));
        }
    }

    // Check vector bounds for array operations
    inline void check_vector_bounds(const std::string& name, const std::vector<float>& vec, 
                                   size_t min_size, size_t max_size = SIZE_MAX) {
        size_t size = vec.size();
        if (size < min_size) {
            throw ValidationError("Vector " + name + " must have at least " +
                                 std::to_string(min_size) + " elements, got " +
                                 std::to_string(size));
        }
        if (size > max_size) {
            throw ValidationError("Vector " + name + " must have at most " +
                                 std::to_string(max_size) + " elements, got " +
                                 std::to_string(size));
        }
    }

    // Check for numeric overflow
    template<typename T>
    void check_overflow(T value, T min, T max) {
        if (value < min || value > max) {
            throw ValidationError("Value " + std::to_string(value) + " is out of range");
        }
    }

    // Validate configuration key
    inline void validate_config_key(const std::string& key) {
        if (key.empty()) {
            throw ConfigError("Configuration key cannot be empty");
        }
        // Check for valid key characters (alphanumeric and underscores)
        if (!std::all_of(key.begin(), key.end(), [](char c) {
            return std::isalnum(c) || c == '_';
        })) {
            throw ConfigError("Configuration key must contain only alphanumeric characters and underscores");
        }
    }

    // Validate file path
    inline void validate_filepath(const std::string& path) {
        if (path.empty()) {
            throw ConfigError("File path cannot be empty");
        }
    }

    // Validate simulation parameters
    inline void validate_simulation_params(double timestep, size_t max_steps) {
        if (timestep <= 0.0) {
            throw ValidationError("Timestep must be positive");
        }
        if (max_steps == 0) {
            throw ValidationError("Max steps must be positive");
        }
        if (max_steps > 1000000) {
            throw ValidationError("Max steps too large (max: 1000000)");
        }
    }

    // Validate neuron parameters
    inline void validate_neuron_params(float max_voltage, float min_voltage, float threshold) {
        if (max_voltage <= min_voltage) {
            throw ValidationError("Max voltage must be greater than min voltage");
        }
        if (threshold >= max_voltage) {
            throw ValidationError("Threshold must be less than max voltage");
        }
    }

    // Validate synaptic weight
    inline void validate_synaptic_weight(float weight) {
        if (std::isnan(weight)) {
            throw ValidationError("Synaptic weight cannot be NaN");
        }
        if (std::isinf(weight)) {
            throw ValidationError("Synaptic weight cannot be infinite");
        }
    }

} // namespace validation

} // namespace nlm
