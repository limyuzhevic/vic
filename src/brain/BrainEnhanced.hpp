#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include "../core/Types/Types.hpp"

namespace nlm {

// Monitoring system types

struct MonitoringStats {
    std::string name;
    double value;
    std::string unit;
    std::map<std::string, double> tags;
    
    MonitoringStats(const std::string& n, double v, const std::string& u = "")
        : name(n), value(v), unit(u) {}
};

struct MonitoringThresholds {
    std::map<std::string, double> alertThresholds;
    std::map<std::string, double> warningThresholds;
    bool enableAlerts;
    bool enableWarnings;
    double samplingInterval;
    
    MonitoringThresholds() : enableAlerts(true), enableWarnings(true), samplingInterval(1.0) {}
};

struct EventLogEntry {
    Timestamp timestamp;
    EventType type;
    std::string message;
    SeverityLevel severity;
    std::map<std::string, std::string> data;
    
    EventLogEntry() : timestamp(0.0), type(EventType::Simulation), severity(SeverityLevel::Info) {}
    EventLogEntry(Timestamp ts, EventType t, const std::string& msg, SeverityLevel sev)
        : timestamp(ts), type(t), message(msg), severity(sev) {}
};

enum class EventType : uint8_t {
    Simulation,
    Spike,
    Plasticity,
    Memory,
    Development,
    Neuromodulation,
    Error,
    Warning,
    Info,
    Custom
};

enum class SeverityLevel : uint8_t {
    Critical,
    Error,
    Warning,
    Info,
    Debug
};

// Advanced Analysis types

struct SpikingPatternAnalysis {
    std::string name;
    double value;
    std::string description;
    std::map<std::string, double> metrics;
    
    SpikingPatternAnalysis(const std::string& n, double v, const std::string& desc = "")
        : name(n), value(v), description(desc) {}
};

struct ConnectivityMetrics {
    std::string name;
    double value;
    std::string unit;
    std::map<std::string, double> statistics;
    
    ConnectivityMetrics(const std::string& n, double v, const std::string& u = "")
        : name(n), value(v), unit(u) {}
};

struct ReplayEvent {
    SimulationStep step;
    std::vector<NeuronId> neuronIds;
    double strength;
    std::string type;
    Timestamp timestamp;
    std::map<std::string, double> metadata;
    
    ReplayEvent() : step(0), strength(0.0), timestamp(0.0) {}
    ReplayEvent(SimulationStep s, const std::vector<NeuronId>& ids, double strength)
        : step(s), neuronIds(ids), strength(strength), timestamp(static_cast<double>(s)) {}
};

struct PlasticityEffect {
    std::string name;
    double value;
    std::string description;
    double impactOnLearning;
    std::map<std::string, double> parameters;
    
    PlasticityEffect(const std::string& n, double v, const std::string& desc = "", double impact = 0.0)
        : name(n), value(v), description(desc), impactOnLearning(impact) {}
};

struct ExportFormat {
    enum Type : uint8_t {
        Binary,
        Text,
        JSON,
        CSV,
        XML,
        Auto
    };
};

struct ImportFormat {
    enum Type : uint8_t {
        Binary,
        Text,
        JSON,
        CSV,
        XML,
        Auto
    };
};

// Debug and Diagnostics types

struct NeuronDiagnostics {
    NeuronId neuronId;
    NeuronType neuronType;
    float membranePotential;
    FiringState firingState;
    float threshold;
    float restingPotential;
    float totalSpikes;
    float firingRate;
    float refractoryRemaining;
    float lastSpikeTime;
    std::map<std::string, float> internalState;
    
    NeuronDiagnostics() : neuronId(INVALID_NEURON_ID), neuronType(NeuronType::Internal),
                         membranePotential(0.0f), firingState(FiringState::Resting),
                         threshold(0.0f), restingPotential(0.0f), totalSpikes(0.0f),
                         firingRate(0.0f), refractoryRemaining(0.0f), lastSpikeTime(0.0f) {}
};

struct RegionDiagnostics {
    RegionId regionId;
    std::string regionName;
    size_t neuronCount;
    size_t synapseCount;
    size_t populationCount;
    float averageSynapticWeight;
    float averageFiringRate;
    size_t activeNeuronCount;
    std::map<std::string, float> statistics;
    
    RegionDiagnostics() : regionId(INVALID_REGION_ID), neuronCount(0), synapseCount(0),
                          populationCount(0), averageSynapticWeight(0.0f),
                          averageFiringRate(0.0f), activeNeuronCount(0) {}
};

enum class PlasticityEventType : uint8_t {
    STDP,
    Hebbian,
    Structural,
    RewardModulated,
    All
};

// Configuration and Validation types

struct ConfigValidationResult {
    bool isValid;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::map<std::string, bool> validationStatus;
    
    ConfigValidationResult() : isValid(true) {}
};

struct ConfigComparison {
    std::string metric;
    double currentValue;
    double baselineValue;
    double difference;
    std::string status;
    
    ConfigComparison(const std::string& m, double current, double baseline)
        : metric(m), currentValue(current), baselineValue(baseline),
          difference(current - baseline), status("") {}
};

struct ConfigOptimization {
    std::string suggestion;
    std::string expectedImpact;
    PriorityLevel priority;
    double estimatedBenefit;
    std::map<std::string, double> parameters;
    
    ConfigOptimization(const std::string& sug, const std::string& impact, PriorityLevel prio)
        : suggestion(sug), expectedImpact(impact), priority(prio), estimatedBenefit(0.0) {}
};

enum class PriorityLevel : uint8_t {
    Low,
    Medium,
    High,
    Critical
};

// Additional types for enhanced APIs

struct ActionId {
    uint64_t value;
    
    constexpr ActionId() : value(0) {}
    explicit constexpr ActionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const ActionId& other) const = default;
    constexpr bool operator!=(const ActionId& other) const = default;
    
    constexpr uint64_t index() const { return value; }
};

} // namespace nlm
