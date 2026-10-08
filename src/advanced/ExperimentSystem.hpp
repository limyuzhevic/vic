#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Experiment system for advanced commands
class Experiment {
public:
    virtual ~Experiment() = default;
    
    virtual bool run(std::shared_ptr<Brain> brain, const std::unordered_map<std::string, std::string>& params) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual size_t getDuration() const = 0;
    virtual std::unordered_map<std::string, float> getResults() const = 0;
    virtual bool validateParameters(const std::unordered_map<std::string, std::string>& params) const = 0;
};

class ConnectivityExperiment : public Experiment {
public:
    bool run(std::shared_ptr<Brain> brain, const std::unordered_map<std::string, std::string>& params) override;
    std::string getName() const override { return "ConnectivityExperiment"; }
    std::string getDescription() const override { return "Experiment to test network connectivity and information flow"; }
    size_t getDuration() const override { return 1000; }
    std::unordered_map<std::string, float> getResults() const override;
    bool validateParameters(const std::unordered_map<std::string, std::string>& params) const override;
};

class PlasticityExperiment : public Experiment {
public:
    bool run(std::shared_ptr<Brain> brain, const std::unordered_map<std::string, std::string>& params) override;
    std::string getName() const override { return "PlasticityExperiment"; }
    std::string getDescription() const override { return "Experiment to study synaptic plasticity and learning"; }
    size_t getDuration() const override { return 2000; }
    std::unordered_map<std::string, float> getResults() const override;
    bool validateParameters(const std::unordered_map<std::string, std::string>& params) const override;
};

class MemoryExperiment : public Experiment {
public:
    bool run(std::shared_ptr<Brain> brain, const std::unordered_map<std::string, std::string>& params) override;
    std::string getName() const override { return "MemoryExperiment"; }
    std::string getDescription() const override { return "Experiment to study memory formation and consolidation"; }
    size_t getDuration() const override { return 3000; }
    std::unordered_map<std::string, float> getResults() const override;
    bool validateParameters(const std::unordered_map<std::string, std::string>& params) const override;
};

class DevelopmentExperiment : public Experiment {
public:
    bool run(std::shared_ptr<Brain> brain, const std::unordered_map<std::string, std::string>& params) override;
    std::string getName() const override { return "DevelopmentExperiment"; }
    std::string getDescription() const override { return "Experiment to study developmental processes and maturation"; }
    size_t getDuration() const override { return 5000; }
    std::unordered_map<std::string, float> getResults() const override;
    bool validateParameters(const std::unordered_map<std::string, std::string>& params) const override;
};

// Memory state export/import structures
struct NeuronCheckpointData {
    std::vector<float> membranePotential;
    std::vector<float> restingPotential;
    std::vector<float> threshold;
    std::vector<float> resetPotential;
    std::vector<float> leakConductance;
    std::vector<uint8_t> firingState;
    std::vector<uint32_t> refractoryRemaining;
    std::vector<uint32_t> refractoryPeriod;
    std::vector<float> lastSpikeTime;
    std::vector<float> adaptationVariable;
};

struct SynapseCheckpointData {
    std::vector<uint32_t> sourceNeuron;
    std::vector<uint32_t> destinationNeuron;
    std::vector<float> weight;
    std::vector<uint32_t> delay;
    std::vector<uint8_t> synapseType;
    std::vector<float> eligibilityTrace;
    std::vector<std::vector<double>> preSpikeHistory;
    std::vector<std::vector<double>> postSpikeHistory;
    std::vector<PlasticityFlags> plasticityFlags;
};

struct MemoryTrace {
    double timestamp;
    NeuronId neuronId;
    float activationLevel;
    MemoryStrength strength;
};

struct ExperimentParameters {
    std::unordered_map<std::string, std::string> parameters;
    size_t duration = 1000;
    std::string name;
    std::string description;
    double startTime;
};

// Helper functions for advanced commands
inline std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

inline std::string generateExperimentId(const std::string& prefix = "exp") {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    return prefix + "_" + std::to_string(timestamp);
}

inline bool validateExperimentParameters(const ExperimentParameters& params) {
    if (params.name.empty()) return false;
    if (params.duration == 0) return false;
    return true;
}

inline std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

} // namespace nlm
