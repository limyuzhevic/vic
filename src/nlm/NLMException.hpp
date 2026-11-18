#pragma once

#include <string>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <sstream>
#include <memory>

namespace nlm {

// Custom exception hierarchy for NLM

class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, const std::string& error_code = "NLM_ERROR", 
             const std::string& details = "") 
        : std::runtime_error(format_message(message, error_code, details)),
          error_code_(error_code), details_(details) {
        stack_trace_ = get_stack_trace();
    }
    
    virtual const char* what() const noexcept override {
        return formatted_message_.c_str();
    }
    
    const std::string& getErrorCode() const { return error_code_; }
    const std::string& getDetails() const { return details_; }
    const std::string& getStackTrace() const { return stack_trace_; }
    const std::string& getFormattedMessage() const { return formatted_message_; }
    
    virtual ~NLMError() = default;
    
protected:
    static std::string format_message(const std::string& message, 
                                     const std::string& error_code, 
                                     const std::string& details) {
        std::stringstream ss;
        ss << "[" << error_code << "] " << message;
        if (!details.empty()) {
            ss << "\nDetails: " << details;
        }
        ss << "\nTimestamp: " << get_timestamp();
        return ss.str();
    }
    
    static std::string get_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    static std::string get_stack_trace() {
        // Simplified stack trace - in production would use platform-specific methods
        return "Stack trace unavailable in simplified build";
    }
    
private:
    std::string error_code_;
    std::string details_;
    std::string stack_trace_;
    std::string formatted_message_;
};

class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, 
                       const std::string& filepath = "",
                       const std::string& missing_keys = "",
                       const std::string& invalid_values = "")
        : NLMError(message, "CONFIG_ERROR", format_details(filepath, missing_keys, invalid_values)) {
    }
    
private:
    static std::string format_details(const std::string& filepath, 
                                     const std::string& missing_keys,
                                     const std::string& invalid_values) {
        std::stringstream ss;
        if (!filepath.empty()) {
            ss << "File: " << filepath << "\n";
        }
        if (!missing_keys.empty()) {
            ss << "Missing keys: " << missing_keys << "\n";
        }
        if (!invalid_values.empty()) {
            ss << "Invalid values: " << invalid_values << "\n";
        }
        return ss.str();
    }
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message,
                const std::string& neuron_id = "",
                const std::string& memory_type = "",
                size_t allocated_size = 0,
                size_t max_capacity = 0)
        : NLMError(message, "MEMORY_ERROR", format_details(neuron_id, memory_type, allocated_size, max_capacity)) {
    }
    
private:
    static std::string format_details(const std::string& neuron_id,
                                     const std::string& memory_type,
                                     size_t allocated_size,
                                     size_t max_capacity) {
        std::stringstream ss;
        if (!neuron_id.empty()) {
            ss << "Neuron ID: " << neuron_id << "\n";
        }
        if (!memory_type.empty()) {
            ss << "Memory type: " << memory_type << "\n";
        }
        if (allocated_size > 0) {
            ss << "Allocated size: " << allocated_size << " bytes\n";
        }
        if (max_capacity > 0) {
            ss << "Max capacity: " << max_capacity << " bytes\n";
        }
        return ss.str();
    }
};

class PlasticityError : public NLMError {
public:
    PlasticityError(const std::string& message,
                   const std::string& plasticity_rule = "",
                   const std::string& neuron_type = "",
                   const std::string& parameters = "")
        : NLMError(message, "PLASTICITY_ERROR", format_details(plasticity_rule, neuron_type, parameters)) {
    }
    
private:
    static std::string format_details(const std::string& plasticity_rule,
                                     const std::string& neuron_type,
                                     const std::string& parameters) {
        std::stringstream ss;
        if (!plasticity_rule.empty()) {
            ss << "Plasticity rule: " << plasticity_rule << "\n";
        }
        if (!neuron_type.empty()) {
            ss << "Neuron type: " << neuron_type << "\n";
        }
        if (!parameters.empty()) {
            ss << "Parameters: " << parameters << "\n";
        }
        return ss.str();
    }
};

class SimulationError : public NLMError {
public:
    SimulationError(const std::string& message,
                    int step_number = -1,
                    const std::string& component = "",
                    double simulation_time = 0.0)
        : NLMError(message, "SIMULATION_ERROR", format_details(step_number, component, simulation_time)) {
    }
    
private:
    static std::string format_details(int step_number,
                                     const std::string& component,
                                     double simulation_time) {
        std::stringstream ss;
        if (step_number >= 0) {
            ss << "Step number: " << step_number << "\n";
        }
        if (!component.empty()) {
            ss << "Component: " << component << "\n";
        }
        if (simulation_time > 0.0) {
            ss << "Simulation time: " << simulation_time << "\n";
        }
        return ss.str();
    }
};

class DevelopmentError : public NLMError {
public:
    DevelopmentError(const std::string& message,
                     const std::string& developmental_stage = "",
                     const std::string& current_stage = "",
                     const std::string& requirements = "")
        : NLMError(message, "DEVELOPMENT_ERROR", format_details(developmental_stage, current_stage, requirements)) {
    }
    
private:
    static std::string format_details(const std::string& developmental_stage,
                                     const std::string& current_stage,
                                     const std::string& requirements) {
        std::stringstream ss;
        if (!developmental_stage.empty()) {
            ss << "Developmental stage: " << developmental_stage << "\n";
        }
        if (!current_stage.empty()) {
            ss << "Current stage: " << current_stage << "\n";
        }
        if (!requirements.empty()) {
            ss << "Requirements: " << requirements << "\n";
        }
        return ss.str();
    }
};

class NeuromodulationError : public NLMError {
public:
    NeuromodulationError(const std::string& message,
                         const std::string& neuromodulator = "",
                         const std::string& target_neuron = "",
                         double signal_strength = 0.0)
        : NLMError(message, "NEUROMODULATION_ERROR", format_details(neuromodulator, target_neuron, signal_strength)) {
    }
    
private:
    static std::string format_details(const std::string& neuromodulator,
                                     const std::string& target_neuron,
                                     double signal_strength) {
        std::stringstream ss;
        if (!neuromodulator.empty()) {
            ss << "Neuromodulator: " << neuromodulator << "\n";
        }
        if (!target_neuron.empty()) {
            ss << "Target neuron: " << target_neuron << "\n";
        }
        if (signal_strength > 0.0) {
            ss << "Signal strength: " << signal_strength << "\n";
        }
        return ss.str();
    }
};

} // namespace nlm