// IParameterResolver.hpp
// Abstract interface for parameter resolution and interpolation
// Resolves parameter values and interpolates between configurations
// Supports parameter evolution and dynamic parameter management

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>
#include <chrono>

namespace nlm {

// Parameter interpolation information
struct ParameterInterpolation {
    std::string paramName;
    float startValue;
    float targetValue;
    SimulationStep startStep;
    SimulationStep endStep;
    bool isComplete;
    std::string interpolationType;  // "linear", "ease_in", "ease_out", "spline"
    
    ParameterInterpolation() : startValue(0), targetValue(0), startStep(0), endStep(0), isComplete(false) {}
    ParameterInterpolation(const std::string& name, float start, float target,
                           SimulationStep startStep, SimulationStep endStep,
                           const std::string& type = "linear")
        : paramName(name), startValue(start), targetValue(target),
          startStep(startStep), endStep(endStep), isComplete(false), interpolationType(type) {}
};

// Parameter resolution result
struct ResolutionResult {
    std::map<std::string, std::string> resolvedValues;
    std::map<std::string, float> resolvedFloats;
    std::map<std::string, int> resolvedInts;
    std::map<std::string, bool> resolvedBools;
    std::string errorMessage;
    bool success;
    
    ResolutionResult() : success(true), errorMessage("") {}
    ResolutionResult(bool s, const std::string& msg) : success(s), errorMessage(msg) {}
};

// Dynamic parameter value
struct DynamicParameter {
    std::string name;
    std::string type;
    float currentValue;
    float targetValue;
    float rateOfChange;
    SimulationStep lastUpdated;
    std::function<float(float, TimestepDuration)> updateFunction;  // For time-based evolution
    
    DynamicParameter() : currentValue(0), targetValue(0), rateOfChange(0), lastUpdated(0) {}
};

class IParameterResolver {
public:
    virtual ~IParameterResolver() = default;

    // Resolve parameter values from configuration
    virtual ResolutionResult resolve(const std::map<std::string, std::string>& config) = 0;

    // Get resolved parameter value
    virtual std::string getResolvedString(const std::string& paramName) const = 0;
    virtual float getResolvedFloat(const std::string& paramName) const = 0;
    virtual int getResolvedInt(const std::string& paramName) const = 0;
    virtual bool getResolvedBool(const std::string& paramName) const = 0;

    // Start parameter interpolation
    virtual void interpolateParameter(const std::string& paramName,
                                      float startValue,
                                      float targetValue,
                                      SimulationStep duration,
                                      const std::string& interpolationType = "linear") = 0;

    // Update interpolations
    virtual void updateInterpolations(TimestepDuration dt) = 0;

    // Get current interpolation status
    virtual ParameterInterpolation getInterpolation(const std::string& paramName) const = 0;
    virtual std::vector<ParameterInterpolation> getActiveInterpolations() const = 0;

    // Add dynamic parameter that evolves over time
    virtual void addDynamicParameter(const DynamicParameter& param) = 0;
    virtual void removeDynamicParameter(const std::string& paramName) = 0;
    virtual DynamicParameter getDynamicParameter(const std::string& paramName) const = 0;

    // Update dynamic parameters
    virtual void updateDynamicParameters(TimestepDuration dt) = 0;

    // Get all parameter values (current + interpolated)
    virtual std::map<std::string, float> getAllParameterValues() const = 0;

    // Update resolver configuration
    virtual void update(const std::map<std::string, std::string>& config) = 0;

    // Reset parameter resolver
    virtual void reset() = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IParameterResolver"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for parameter resolution
    using ParameterResolvedCallback = std::function<void(const std::string& paramName, float value)>;
    virtual void setParameterResolvedCallback(const ParameterResolvedCallback& callback) = 0;

    using InterpolationCompletedCallback = std::function<void(const std::string& paramName, float finalValue)>;
    virtual void setInterpolationCompletedCallback(const InterpolationCompletedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Parameter resolver parameters
    virtual void setInterpolationAccuracy(float accuracy) = 0;  // Steps per interpolation
    virtual float getInterpolationAccuracy() const = 0;
    virtual void setDynamicParameterUpdateRate(float rate) = 0;  // Updates per second
    virtual float getDynamicParameterUpdateRate() const = 0;

    // Performance metrics
    virtual uint64_t getTotalResolutions() const = 0;
    virtual float getResolutionSpeed() const = 0;  // Resolutions per second
    virtual uint64_t getActiveInterpolationsCount() const = 0;
    virtual uint64_t getDynamicParametersCount() const = 0;

protected:
    IParameterResolver() = default;
};

} // namespace nlm
