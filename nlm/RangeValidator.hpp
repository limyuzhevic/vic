/*
 * nlm/RangeValidator.hpp - Range checking utilities for the NLM system
 * Comprehensive range validation for numeric parameters and values
 */

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "nlm/Exceptions.hpp"

namespace nlm {

// Range validation utilities
class RangeValidator {
public:
    // Basic range checking
    template<typename T>
    static bool isInRange(T value, T min, T max, bool inclusive = true) {
        if (inclusive) {
            return value >= min && value <= max;
        } else {
            return value > min && value < max;
        }
    }
    
    // Float range validation with detailed error reporting
    static void validateFloatRange(float value, float min, float max, 
                                  const std::string& name,
                                  const std::string& file = "",
                                  int line = 0,
                                  const std::string& function = "");
    
    // Double range validation with detailed error reporting
    static void validateDoubleRange(double value, double min, double max,
                                    const std::string& name,
                                    const std::string& file = "",
                                    int line = 0,
                                    const std::string& function = "");
    
    // Integer range validation with detailed error reporting
    static void validateIntRange(int value, int min, int max,
                                 const std::string& name,
                                 const std::string& file = "",
                                 int line = 0,
                                 const std::string& function = "");
    
    // Vector range validation
    template<typename T>
    static void validateVectorRange(const std::vector<T>& values,
                                    T min, T max,
                                    const std::string& name,
                                    const std::string& file = "",
                                    int line = 0,
                                    const std::string& function = "");
    
    // Percentage validation (0-100%)
    static void validatePercentage(float value,
                                   const std::string& name,
                                   const std::string& file = "",
                                   int line = 0,
                                   const std::string& function = "");
    
    // Probability validation (0-1)
    static void validateProbability(float value,
                                    const std::string& name,
                                    const std::string& file = "",
                                    int line = 0,
                                    const std::string& function = "");
    
    // Angle validation (0-360 degrees)
    static void validateAngle(float value,
                              const std::string& name,
                              const std::string& file = "",
                              int line = 0,
                              const std::string& function = "");
    
    // Temperature validation (-273.15 to very high)
    static void validateTemperature(float value,
                                    const std::string& name,
                                    const std::string& file = "",
                                    int line = 0,
                                    const std::string& function = "");
    
    // Energy validation (positive values)
    static void validateEnergy(float value,
                               const std::string& name,
                               const std::string& file = "",
                               int line = 0,
                               const std::string& function = "");
    
    // Time validation (positive values)
    static void validateTime(float value,
                             const std::string& name,
                             const std::string& file = "",
                             int line = 0,
                             const std::string& function = "");
};

// Range validation implementation
inline void RangeValidator::validateFloatRange(float value, float min, float max,
                                               const std::string& name,
                                               const std::string& file,
                                               int line,
                                               const std::string& function) {
    if (value < min || value > max) {
        throw NeuralParameterException(name, value, file, line, function);
    }
}

inline void RangeValidator::validateDoubleRange(double value, double min, double max,
                                                 const std::string& name,
                                                 const std::string& file,
                                                 int line,
                                                 const std::string& function) {
    if (value < min || value > max) {
        throw NeuralParameterException(name, static_cast<float>(value), file, line, function);
    }
}

inline void RangeValidator::validateIntRange(int value, int min, int max,
                                             const std::string& name,
                                             const std::string& file,
                                             int line,
                                             const std::string& function) {
    if (value < min || value > max) {
        throw NeuralParameterException(name, static_cast<float>(value), file, line, function);
    }
}

inline void RangeValidator::validateVectorRange(const std::vector<float>& values,
                                                float min, float max,
                                                const std::string& name,
                                                const std::string& file,
                                                int line,
                                                const std::string& function) {
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] < min || values[i] > max) {
            throw NeuralParameterException(name + "[" + std::to_string(i) + "]", 
                                           values[i], file, line, function);
        }
    }
}

inline void RangeValidator::validatePercentage(float value,
                                               const std::string& name,
                                               const std::string& file,
                                               int line,
                                               const std::string& function) {
    if (value < 0.0f || value > 100.0f) {
        throw NeuralParameterException(name, value, file, line, function);
    }
}

inline void RangeValidator::validateProbability(float value,
                                                const std::string& name,
                                                const std::string& file,
                                                int line,
                                                const std::string& function) {
    if (value < 0.0f || value > 1.0f) {
        throw NeuralParameterException(name, value, file, line, function);
    }
}

inline void RangeValidator::validateAngle(float value,
                                          const std::string& name,
                                          const std::string& file,
                                          int line,
                                          const std::string& function) {
    if (value < 0.0f || value > 360.0f) {
        throw NeuralParameterException(name, value, file, line, function);
    }
}

inline void RangeValidator::validateTemperature(float value,
                                                const std::string& name,
                                                const std::string& file,
                                                int line,
                                                const std::string& function) {
    if (value < -273.15f) {
        throw NeuralParameterException(name, value, file, line, function);
    }
}

inline void RangeValidator::validateEnergy(float value,
                                           const std::string& name,
                                           const std::string& file,
                                           int line,
                                           const std::string& function) {
    if (value < 0.0f) {
        throw NeuralParameterException(name, value, file, line, function);
    }
}

inline void RangeValidator::validateTime(float value,
                                         const std::string& name,
                                         const std::string& file,
                                         int line,
                                         const std::string& function) {
    if (value < 0.0f) {
        throw NeuralParameterException(name, value, file, line, function);
    }
}

} // namespace nlm
