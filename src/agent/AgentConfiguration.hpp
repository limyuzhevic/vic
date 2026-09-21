#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class AgentBrain;
class Brain;
class Config;

// Configuration file format structure
struct AgentConfigurationFile {
    nlohmann::json toJson() const {
        nlohmann::json j;
        
        // Neuron distribution
        j["neuron_distribution"]["motor_forward"] = config.distribution.motorForward;
        j["neuron_distribution"]["motor_backward"] = config.distribution.motorBackward;
        j["neuron_distribution"]["motor_turn_left"] = config.distribution.motorTurnLeft;
        j["neuron_distribution"]["motor_turn_right"] = config.distribution.motorTurnRight;
        j["neuron_distribution"]["motor_interact"] = config.distribution.motorInteract;
        j["neuron_distribution"]["motor_wait"] = config.distribution.motorWait;
        
        j["neuron_distribution"]["sensory_vision"] = config.distribution.sensoryVision;
        j["neuron_distribution"]["sensory_touch"] = config.distribution.sensoryTouch;
        j["neuron_distribution"]["sensory_internal"] = config.distribution.sensoryInternal;
        j["neuron_distribution"]["sensory_proprioception"] = config.distribution.sensoryProprioception;
        
        // Sensory configuration
        auto& sensory = j["sensory"];
        sensory["vision_width"] = config.sensory.visionWidth;
        sensory["vision_height"] = config.sensory.visionHeight;
        sensory["touch_points"] = config.sensory.touchPoints;
        sensory["internal_signals"] = config.sensory.internalSignals;
        sensory["proprioception_points"] = config.sensory.proprioceptionPoints;
        sensory["enable_audio"] = config.sensory.enableAudio;
        sensory["audio_channels"] = config.sensory.audioChannels;
        
        sensory["vision_scale"] = config.sensory.visionScale;
        sensory["touch_scale"] = config.sensory.touchScale;
        sensory["internal_scale"] = config.sensory.internalScale;
        sensory["proprioception_scale"] = config.sensory.proprioceptionScale;
        
        sensory["normalize_vision"] = config.sensory.normalizeVision;
        sensory["normalize_touch"] = config.sensory.normalizeTouch;
        sensory["normalize_internal"] = config.sensory.normalizeInternal;
        sensory["normalize_proprioception"] = config.sensory.normalizeProprioception;
        
        // Neuromodulation configuration
        auto& neuromod = j["neuromodulation"];
        neuromod["novelty_decay_rate"] = config.neuromodulation.noveltyDecayRate;
        neuromod["curiosity_base_level"] = config.neuromodulation.curiosityBaseLevel;
        neuromod["dopamine_clamp_min"] = config.neuromodulation.dopamineClampMin;
        neuromod["dopamine_clamp_max"] = config.neuromodulation.dopamineClampMax;
        neuromod["curiosity_threshold"] = config.neuromodulation.curiosityThreshold;
        neuromod["exploration_chance"] = config.neuromodulation.explorationChance;
        neuromod["curiosity_amplification"] = config.neuromodulation.curiosityAmplification;
        neuromod["error_amplification"] = config.neuromodulation.errorAmplification;
        
        // Development configuration
        auto& dev = j["development"];
        dev["initial_age"] = config.development.initialAge;
        dev["growth_rate"] = config.development.growthRate;
        dev["critical_period_end"] = config.development.criticalPeriodEnd;
        dev["maturation_end"] = config.development.maturationEnd;
        dev["plasticity_initial"] = config.development.plasticityInitial;
        dev["plasticity_critical"] = config.development.plasticityCritical;
        dev["plasticity_mature"] = config.development.plasticityMature;
        dev["plasticity_adult"] = config.development.plasticityAdult;
        
        // Sampling configuration
        auto& sampling = j["sampling"];
        sampling["enable_neural_sampling"] = config.sampling.enableNeuralSampling;
        sampling["sample_every_n_steps"] = config.sampling.sampleEveryNSteps;
        sampling["max_sample_history"] = config.sampling.maxSampleHistory;
        sampling["record_spikes"] = config.sampling.recordSpikes;
        sampling["record_weights"] = config.sampling.recordWeights;
        sampling["record_activity"] = config.sampling.recordActivity;
        
        sampling["enable_performance_monitoring"] = config.sampling.enablePerformanceMonitoring;
        sampling["monitor_interval"] = config.sampling.monitorInterval;
        sampling["max_performance_history"] = config.sampling.maxPerformanceHistory;
        
        // File configuration
        auto& file = j["file"];
        file["default_config_file"] = config.file.defaultConfigFile;
        file["checkpoint_file"] = config.file.checkpointFile;
        file["sample_output_file"] = config.file.sampleOutputFile;
        file["save_samples"] = config.file.saveSamples;
        file["save_checkpoints"] = config.file.saveCheckpoints;
        
        // Flags
        j["use_distribution"] = config.useDistribution;
        j["use_sensory_config"] = config.useSensoryConfig;
        j["use_neuromodulation_config"] = config.useNeuromodulationConfig;
        j["use_development_config"] = config.useDevelopmentConfig;
        j["use_sampling_config"] = config.useSamplingConfig;
        
        return j;
    }
    
    bool fromJson(const nlohmann::json& j) {
        try {
            // Neuron distribution
            auto& dist = j["neuron_distribution"];
            config.distribution.motorForward = dist["motor_forward"];
            config.distribution.motorBackward = dist["motor_backward"];
            config.distribution.motorTurnLeft = dist["motor_turn_left"];
            config.distribution.motorTurnRight = dist["motor_turn_right"];
            config.distribution.motorInteract = dist["motor_interact"];
            config.distribution.motorWait = dist["motor_wait"];
            
            config.distribution.sensoryVision = dist["sensory_vision"];
            config.distribution.sensoryTouch = dist["sensory_touch"];
            config.distribution.sensoryInternal = dist["sensory_internal"];
            config.distribution.sensoryProprioception = dist["sensory_proprioception"];
            
            // Sensory configuration
            auto& sensory = j["sensory"];
            config.sensory.visionWidth = sensory["vision_width"];
            config.sensory.visionHeight = sensory["vision_height"];
            config.sensory.touchPoints = sensory["touch_points"];
            config.sensory.internalSignals = sensory["internal_signals"];
            config.sensory.proprioceptionPoints = sensory["proprioception_points"];
            config.sensory.enableAudio = sensory["enable_audio"];
            config.sensory.audioChannels = sensory["audio_channels"];
            
            config.sensory.visionScale = sensory["vision_scale"];
            config.sensory.touchScale = sensory["touch_scale"];
            config.sensory.internalScale = sensory["internal_scale"];
            config.sensory.proprioceptionScale = sensory["proprioception_scale"];
            
            config.sensory.normalizeVision = sensory["normalize_vision"];
            config.sensory.normalizeTouch = sensory["normalize_touch"];
            config.sensory.normalizeInternal = sensory["normalize_internal"];
            config.sensory.normalizeProprioception = sensory["normalize_proprioception"];
            
            // Neuromodulation configuration
            auto& neuromod = j["neuromodulation"];
            config.neuromodulation.noveltyDecayRate = neuromod["novelty_decay_rate"];
            config.neuromodulation.curiosityBaseLevel = neuromod["curiosity_base_level"];
            config.neuromodulation.dopamineClampMin = neuromod["dopamine_clamp_min"];
            config.neuromodulation.dopamineClampMax = neuromod["dopamine_clamp_max"];
            config.neuromodulation.curiosityThreshold = neuromod["curiosity_threshold"];
            config.neuromodulation.explorationChance = neuromod["exploration_chance"];
            config.neuromodulation.curiosityAmplification = neuromod["curiosity_amplification"];
            config.neuromodulation.errorAmplification = neuromod["error_amplification"];
            
            // Development configuration
            auto& dev = j["development"];
            config.development.initialAge = dev["initial_age"];
            config.development.growthRate = dev["growth_rate"];
            config.development.criticalPeriodEnd = dev["critical_period_end"];
            config.development.maturationEnd = dev["maturation_end"];
            config.development.plasticityInitial = dev["plasticity_initial"];
            config.development.plasticityCritical = dev["plasticity_critical"];
            config.development.plasticityMature = dev["plasticity_mature"];
            config.development.plasticityAdult = dev["plasticity_adult"];
            
            // Sampling configuration
            auto& sampling = j["sampling"];
            config.sampling.enableNeuralSampling = sampling["enable_neural_sampling"];
            config.sampling.sampleEveryNSteps = sampling["sample_every_n_steps"];
            config.sampling.maxSampleHistory = sampling["max_sample_history"];
            config.sampling.recordSpikes = sampling["record_spikes"];
            config.sampling.recordWeights = sampling["record_weights"];
            config.sampling.recordActivity = sampling["record_activity"];
            
            config.sampling.enablePerformanceMonitoring = sampling["enable_performance_monitoring"];
            config.sampling.monitorInterval = sampling["monitor_interval"];
            config.sampling.maxPerformanceHistory = sampling["max_performance_history"];
            
            // File configuration
            auto& file = j["file"];
            config.file.defaultConfigFile = file["default_config_file"];
            config.file.checkpointFile = file["checkpoint_file"];
            config.file.sampleOutputFile = file["sample_output_file"];
            config.file.saveSamples = file["save_samples"];
            config.file.saveCheckpoints = file["save_checkpoints"];
            
            // Flags
            config.useDistribution = j["use_distribution"];
            config.useSensoryConfig = j["use_sensory_config"];
            config.useNeuromodulationConfig = j["use_neuromodulation_config"];
            config.useDevelopmentConfig = j["use_development_config"];
            config.useSamplingConfig = j["use_sampling_config"];
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    AgentConfig config;
};

// Neural activity sampling system
class NeuralSampler {
public:
    struct Sample {
        double timestamp;
        std::string type;
        std::vector<float> data;
        std::map<std::string, float> metrics;
    };
    
    NeuralSampler(size_t maxHistory = 1000) : maxHistory_(maxHistory) {}
    
    void startSampling() { enabled_ = true; lastSampleTime_ = std::chrono::high_resolution_clock::now(); }
    void stopSampling() { enabled_ = false; }
    bool isEnabled() const { return enabled_; }
    
    void recordSample(const std::string& type, const std::vector<float>& data) {
        if (!enabled_) return;
        
        auto now = std::chrono::high_resolution_clock::now();
        double timestamp = std::chrono::duration<double>(now.time_since_epoch()).count();
        
        Sample sample;
        sample.timestamp = timestamp;
        sample.type = type;
        sample.data = data;
        
        if (samples_.size() >= maxHistory_) {
            samples_.erase(samples_.begin());
        }
        
        samples_.push_back(sample);
    }
    
    const std::vector<Sample>& getSamples() const { return samples_; }
    std::vector<Sample> getSamplesByType(const std::string& type) const {
        std::vector<Sample> filtered;
        for (const auto& sample : samples_) {
            if (sample.type == type) {
                filtered.push_back(sample);
            }
        }
        return filtered;
    }
    
    void clear() { samples_.clear(); }
    size_t size() const { return samples_.size(); }
    
    // Analysis methods
    double getAverageForType(const std::string& type) const {
        auto samples = getSamplesByType(type);
        if (samples.empty()) return 0.0;
        
        double sum = 0.0;
        for (const auto& sample : samples) {
            for (float val : sample.data) {
                sum += val;
            }
        }
        
        return sum / (samples.size() * samples[0].data.size());
    }
    
    double getMinForType(const std::string& type) const {
        auto samples = getSamplesByType(type);
        if (samples.empty()) return 0.0;
        
        double min = samples[0].data[0];
        for (const auto& sample : samples) {
            for (float val : sample.data) {
                if (val < min) min = val;
            }
        }
        return min;
    }
    
    double getMaxForType(const std::string& type) const {
        auto samples = getSamplesByType(type);
        if (samples.empty()) return 0.0;
        
        double max = samples[0].data[0];
        for (const auto& sample : samples) {
            for (float val : sample.data) {
                if (val > max) max = val;
            }
        }
        return max;
    }
    
    // Export methods
    bool exportToCSV(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        
        file << "timestamp,type,value\n";
        
        for (const auto& sample : samples_) {
            for (float val : sample.data) {
                file << sample.timestamp << "," << sample.type << "," << val << "\n";
            }
        }
        
        file.close();
        return true;
    }
    
    bool exportToJSON(const std::string& filename) const {
        nlohmann::json j = nlohmann::json::array();
        
        for (const auto& sample : samples_) {
            nlohmann::json sampleJson;
            sampleJson["timestamp"] = sample.timestamp;
            sampleJson["type"] = sample.type;
            sampleJson["data"] = sample.data;
            sampleJson["metrics"] = sample.metrics;
            j.push_back(sampleJson);
        }
        
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        
        file << j.dump(2);
        file.close();
        return true;
    }
    
private:
    bool enabled_ = false;
    std::vector<Sample> samples_;
    size_t maxHistory_;
    std::chrono::high_resolution_clock::time_point lastSampleTime_;
};

// Performance monitoring system
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double timestamp;
        double cpuUsage;        // Percentage 0.0-100.0
        double memoryUsage;     // Percentage 0.0-100.0
        size_t spikeCount;
        double averageActivity; // Normalized 0.0-1.0
        double simulationSpeed; // Steps per second
        double memoryFootprint; // MB
    };
    
    PerformanceMonitor(size_t maxHistory = 100) : maxHistory_(maxHistory) {}
    
    void recordMetrics(const PerformanceMetrics& metrics) {
        if (metrics_.size() >= maxHistory_) {
            metrics_.erase(metrics_.begin());
        }
        metrics_.push_back(metrics);
    }
    
    const std::vector<PerformanceMetrics>& getMetrics() const { return metrics_; }
    
    double getAverageCPU() const {
        if (metrics_.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& m : metrics_) sum += m.cpuUsage;
        return sum / metrics_.size();
    }
    
    double getAverageMemory() const {
        if (metrics_.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& m : metrics_) sum += m.memoryUsage;
        return sum / metrics_.size();
    }
    
    double getAverageActivity() const {
        if (metrics_.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& m : metrics_) sum += m.averageActivity;
        return sum / metrics_.size();
    }
    
    double getMinCPU() const {
        if (metrics_.empty()) return 0.0;
        double min = metrics_[0].cpuUsage;
        for (const auto& m : metrics_) {
            if (m.cpuUsage < min) min = m.cpuUsage;
        }
        return min;
    }
    
    double getMaxCPU() const {
        if (metrics_.empty()) return 0.0;
        double max = metrics_[0].cpuUsage;
        for (const auto& m : metrics_) {
            if (m.cpuUsage > max) max = m.cpuUsage;
        }
        return max;
    }
    
    void clear() { metrics_.clear(); }
    size_t size() const { return metrics_.size(); }
    
    // Generate performance report
    std::string generateReport() const {
        std::stringstream ss;
        ss << "=== Performance Report ===\n";
        ss << "Total samples: " << metrics_.size() << "\n";
        ss << "Time range: ";
        if (!metrics_.empty()) {
            ss << metrics_.front().timestamp << " to " << metrics_.back().timestamp << "\n";
        }
        
        ss << "\nCPU Usage:\n";
        ss << "  Average: " << getAverageCPU() << "%\n";
        ss << "  Min: " << getMinCPU() << "%\n";
        ss << "  Max: " << getMaxCPU() << "%\n";
        
        ss << "\nMemory Usage:\n";
        ss << "  Average: " << getAverageMemory() << "%\n";
        
        ss << "\nNeural Activity:\n";
        ss << "  Average: " << getAverageActivity() << "\n";
        
        ss << "\nSpike Statistics:\n";
        size_t totalSpikes = 0;
        for (const auto& m : metrics_) totalSpikes += m.spikeCount;
        ss << "  Total spikes: " << totalSpikes << "\n";
        ss << "  Average per sample: " << (metrics_.empty() ? 0 : totalSpikes / metrics_.size()) << "\n";
        
        return ss.str();
    }
    
private:
    std::vector<PerformanceMetrics> metrics_;
    size_t maxHistory_;
};

// Checkpoint system for agent state persistence
class CheckpointSystem {
public:
    struct Checkpoint {
        double timestamp;
        std::string agentState;
        std::string neuralState;
        std::string configState;
        std::string performanceState;
    };
    
    CheckpointSystem(const std::string& defaultFile = "checkpoints/agent_checkpoint.bin") 
        : defaultFile_(defaultFile) {}
    
    void saveCheckpoint(const Checkpoint& checkpoint, const std::string& filename = "") {
        std::string filepath = filename.empty() ? defaultFile_ : filename;
        
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) return;
        
        // Serialize checkpoint data
        // Simplified implementation - would use proper serialization
        file << checkpoint.timestamp << "\n";
        file << checkpoint.agentState << "\n";
        file << checkpoint.neuralState << "\n";
        file << checkpoint.configState << "\n";
        file << checkpoint.performanceState << "\n";
        
        file.close();
    }
    
    Checkpoint loadCheckpoint(const std::string& filename = "") {
        std::string filepath = filename.empty() ? defaultFile_ : filename;
        
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return Checkpoint{0.0, "", "", "", ""};
        }
        
        Checkpoint checkpoint;
        file >> checkpoint.timestamp;
        file.ignore(); // Skip newline
        std::getline(file, checkpoint.agentState);
        std::getline(file, checkpoint.neuralState);
        std::getline(file, checkpoint.configState);
        std::getline(file, checkpoint.performanceState);
        
        file.close();
        return checkpoint;
    }
    
    std::vector<Checkpoint> listCheckpoints(const std::string& directory = "checkpoints") {
        std::vector<Checkpoint> checkpoints;
        
        // Simplified implementation - would scan directory for checkpoint files
        // and load them
        
        return checkpoints;
    }
    
    bool deleteCheckpoint(const std::string& filename) {
        std::string filepath = (filename.empty() ? defaultFile_ : filename);
        return std::remove(filepath.c_str()) == 0;
    }
    
private:
    std::string defaultFile_;
};

} // namespace nlm