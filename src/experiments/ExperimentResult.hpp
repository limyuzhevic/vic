#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace nlm {

// Experiment metrics collected during execution
struct ExperimentMetrics {
    // Basic metrics from experiment execution
    double total_reward = 0.0;
    double avg_firing_rate = 0.0;
    size_t memory_episodes_stored = 0;
    
    // Additional metrics
    double novelty_level = 0.0;
    double curiosity_level = 0.0;
    double dopamine_level = 0.0;
    double avg_synaptic_weight = 0.0;
    
    // Performance metrics
    uint64_t start_step = 0;
    uint64_t end_step = 0;
    size_t neuron_count = 0;
    size_t synapse_count = 0;
    size_t firing_count = 0;
    
    // System integration status (from Phase6)
    bool memory_working_memory_integrated = false;
    bool memory_episodic_memory_integrated = false;
    bool neuromodulation_integrated = false;
    bool prediction_integrated = false;
    bool development_integrated = false;
    bool checkpointing_works = false;
    bool replay_works = false;
};

// Experiment configuration
struct ExperimentConfig {
    std::string name;
    std::string description;
    std::string version;
    uint64_t globalSeed = 42;
    std::unordered_map<std::string, std::string> parameters;
    
    // Phase6 specific config
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    uint64_t maxSteps = 10000;
};

// Full experiment result with metadata and results
struct ExperimentResult {
    // Metadata
    std::string experimentName;
    std::string version = "1.0.0";
    std::string timestamp;
    double wallClockTimeSeconds = 0.0;
    
    // Configuration
    ExperimentConfig config;
    
    // Results
    ExperimentMetrics metrics;
    
    // Step-by-step data
    struct StepData {
        uint64_t step;
        size_t neuronCount;
        size_t synapseCount;
        size_t firingCount;
        float avgFiringRate;
    };
    
    std::vector<StepData> steps;
    
    // Notes and metadata
    std::vector<std::string> notes;
    std::unordered_map<std::string, std::string> tags;
    
    // Serialization version
    static constexpr const char* CURRENT_VERSION = "1.0.0";
    
    // Default constructor
    ExperimentResult() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        timestamp = std::ctime(&time);
        // Remove trailing newline from ctime
        if (!timestamp.empty() && timestamp.back() == '\n') {
            timestamp.pop_back();
        }
    }
    
    // Convert to JSON
    nlohmann::json toJson() const {
        nlohmann::json j;
        
        // Metadata
        j["experiment_name"] = experimentName;
        j["version"] = version;
        j["timestamp"] = timestamp;
        j["wall_clock_time_seconds"] = wallClockTimeSeconds;
        
        // Configuration
        j["config"]["name"] = config.name;
        j["config"]["description"] = config.description;
        j["config"]["version"] = config.version;
        j["config"]["globalSeed"] = config.globalSeed;
        j["config"]["parameters"] = config.parameters;
        j["config"]["neuronCount"] = config.neuronCount;
        j["config"]["regionCount"] = config.regionCount;
        j["config"]["connectionProbability"] = config.connectionProbability;
        j["config"]["enableCheckpointing"] = config.enableCheckpointing;
        j["config"]["enableReplay"] = config.enableReplay;
        j["config"]["enableDevelopment"] = config.enableDevelopment;
        j["config"]["checkpointPath"] = config.checkpointPath;
        j["config"]["maxSteps"] = config.maxSteps;
        
        // Metrics
        auto& m = j["metrics"];
        m["total_reward"] = metrics.total_reward;
        m["avg_firing_rate"] = metrics.avg_firing_rate;
        m["memory_episodes_stored"] = metrics.memory_episodes_stored;
        m["novelty_level"] = metrics.novelty_level;
        m["curiosity_level"] = metrics.curiosity_level;
        m["dopamine_level"] = metrics.dopamine_level;
        m["avg_synaptic_weight"] = metrics.avg_synaptic_weight;
        m["start_step"] = metrics.start_step;
        m["end_step"] = metrics.end_step;
        m["neuron_count"] = metrics.neuron_count;
        m["synapse_count"] = metrics.synapse_count;
        m["firing_count"] = metrics.firing_count;
        
        // Integration status
        auto& integration = j["integration_status"];
        integration["memory_working_memory_integrated"] = metrics.memory_working_memory_integrated;
        integration["memory_episodic_memory_integrated"] = metrics.memory_episodic_memory_integrated;
        integration["neuromodulation_integrated"] = metrics.neuromodulation_integrated;
        integration["prediction_integrated"] = metrics.prediction_integrated;
        integration["development_integrated"] = metrics.development_integrated;
        integration["checkpointing_works"] = metrics.checkpointing_works;
        integration["replay_works"] = metrics.replay_works;
        
        // Step data
        j["steps"] = std::vector<nlohmann::json>(steps.size());
        for (size_t i = 0; i < steps.size(); ++i) {
            auto& step = j["steps"][i];
            step["step"] = steps[i].step;
            step["neuron_count"] = steps[i].neuronCount;
            step["synapse_count"] = steps[i].synapseCount;
            step["firing_count"] = steps[i].firingCount;
            step["avg_firing_rate"] = steps[i].avgFiringRate;
        }
        
        // Notes
        j["notes"] = notes;
        j["tags"] = tags;
        
        return j;
    }
    
    // Convert from JSON
    static ExperimentResult fromJson(const nlohmann::json& j) {
        ExperimentResult result;
        
        // Metadata
        result.experimentName = j.value("experiment_name", "");
        result.version = j.value("version", "1.0.0");
        result.timestamp = j.value("timestamp", "");
        result.wallClockTimeSeconds = j.value("wall_clock_time_seconds", 0.0);
        
        // Configuration
        result.config.name = j["config"].value("name", "");
        result.config.description = j["config"].value("description", "");
        result.config.version = j["config"].value("version", "1.0.0");
        result.config.globalSeed = j["config"].value("globalSeed", 42);
        result.config.parameters = j["config"].value("parameters", std::unordered_map<std::string, std::string>{});
        result.config.neuronCount = j["config"].value("neuronCount", 1000);
        result.config.regionCount = j["config"].value("regionCount", 1);
        result.config.connectionProbability = j["config"].value("connectionProbability", 0.1f);
        result.config.enableCheckpointing = j["config"].value("enableCheckpointing", true);
        result.config.enableReplay = j["config"].value("enableReplay", true);
        result.config.enableDevelopment = j["config"].value("enableDevelopment", true);
        result.config.checkpointPath = j["config"].value("checkpointPath", "./checkpoint_test.bin");
        result.config.maxSteps = j["config"].value("maxSteps", 10000);
        
        // Metrics
        auto& m = j["metrics"];
        result.metrics.total_reward = m.value("total_reward", 0.0);
        result.metrics.avg_firing_rate = m.value("avg_firing_rate", 0.0);
        result.metrics.memory_episodes_stored = static_cast<size_t>(m.value("memory_episodes_stored", 0));
        result.metrics.novelty_level = m.value("novelty_level", 0.0);
        result.metrics.curiosity_level = m.value("curiosity_level", 0.0);
        result.metrics.dopamine_level = m.value("dopamine_level", 0.0);
        result.metrics.avg_synaptic_weight = m.value("avg_synaptic_weight", 0.0);
        result.metrics.start_step = m.value("start_step", 0);
        result.metrics.end_step = m.value("end_step", 0);
        result.metrics.neuron_count = static_cast<size_t>(m.value("neuron_count", 0));
        result.metrics.synapse_count = static_cast<size_t>(m.value("synapse_count", 0));
        result.metrics.firing_count = static_cast<size_t>(m.value("firing_count", 0));
        
        // Integration status
        if (j.contains("integration_status")) {
            auto& integration = j["integration_status"];
            result.metrics.memory_working_memory_integrated = integration.value("memory_working_memory_integrated", false);
            result.metrics.memory_episodic_memory_integrated = integration.value("memory_episodic_memory_integrated", false);
            result.metrics.neuromodulation_integrated = integration.value("neuromodulation_integrated", false);
            result.metrics.prediction_integrated = integration.value("prediction_integrated", false);
            result.metrics.development_integrated = integration.value("development_integrated", false);
            result.metrics.checkpointing_works = integration.value("checkpointing_works", false);
            result.metrics.replay_works = integration.value("replay_works", false);
        }
        
        // Step data
        if (j.contains("steps")) {
            result.steps.reserve(j["steps"].size());
            for (const auto& stepJson : j["steps"]) {
                StepData step;
                step.step = stepJson.value("step", 0);
                step.neuronCount = static_cast<size_t>(stepJson.value("neuron_count", 0));
                step.synapseCount = static_cast<size_t>(stepJson.value("synapse_count", 0));
                step.firingCount = static_cast<size_t>(stepJson.value("firing_count", 0));
                step.avgFiringRate = stepJson.value("avg_firing_rate", 0.0f);
                result.steps.push_back(step);
            }
        }
        
        // Notes
        if (j.contains("notes")) {
            result.notes = j["notes"].get<std::vector<std::string>>();
        }
        
        // Tags
        if (j.contains("tags")) {
            result.tags = j["tags"].get<std::unordered_map<std::string, std::string>>();
        }
        
        return result;
    }
    
    // Helper methods for analysis
    double calculateAverageRewardPerStep() const {
        if (steps.empty()) return 0.0;
        return metrics.total_reward / steps.size();
    }
    
    double calculateTotalFiring() const {
        return metrics.avg_firing_rate * steps.size();
    }
    
    std::string generateSummaryReport() const {
        std::string report = "Experiment: " + experimentName + "\n";
        report += "Version: " + version + "\n";
        report += "Timestamp: " + timestamp + "\n";
        report += "Wall Clock Time: " + std::to_string(wallClockTimeSeconds) + " seconds\n";
        report += "Total Reward: " + std::to_string(metrics.total_reward) + "\n";
        report += "Average Reward per Step: " + std::to_string(calculateAverageRewardPerStep()) + "\n";
        report += "Average Firing Rate: " + std::to_string(metrics.avg_firing_rate) + "\n";
        report += "Total Firing Activity: " + std::to_string(calculateTotalFiring()) + "\n";
        report += "Memory Episodes Stored: " + std::to_string(metrics.memory_episodes_stored) + "\n";
        report += "Novelty Level: " + std::to_string(metrics.novelty_level) + "\n";
        report += "Curiosity Level: " + std::to_string(metrics.curiosity_level) + "\n";
        report += "Dopamine Level: " + std::to_string(metrics.dopamine_level) + "\n";
        report += "Avg Synaptic Weight: " + std::to_string(metrics.avg_synaptic_weight) + "\n";
        return report;
    }
};

// Results analyzer for comparing experiments
class ResultsAnalyzer {
public:
    // Calculate statistical summaries for an experiment result
    static std::string generateStatisticalSummary(const ExperimentResult& result) {
        std::string summary;
        summary += "Statistical Summary for Experiment: " + result.experimentName + "\n";
        summary += "===========================================\n";
        
        // Basic metrics
        summary += "Basic Metrics:\n";
        summary += "  - Total Reward: " + std::to_string(result.metrics.total_reward) + "\n";
        summary += "  - Average Firing Rate: " + std::to_string(result.metrics.avg_firing_rate) + "\n";
        summary += "  - Memory Episodes: " + std::to_string(result.metrics.memory_episodes_stored) + "\n";
        
        // System integration
        summary += "\nSystem Integration:\n";
        summary += "  - Memory Working Memory: " + std::string(result.metrics.memory_working_memory_integrated ? "✓" : "✗") + "\n";
        summary += "  - Memory Episodic Memory: " + std::string(result.metrics.memory_episodic_memory_integrated ? "✓" : "✗") + "\n";
        summary += "  - Neuromodulation: " + std::string(result.metrics.neuromodulation_integrated ? "✓" : "✗") + "\n";
        summary += "  - Prediction System: " + std::string(result.metrics.prediction_integrated ? "✓" : "✗") + "\n";
        summary += "  - Development System: " + std::string(result.metrics.development_integrated ? "✓" : "✗") + "\n";
        summary += "  - Checkpointing: " + std::string(result.metrics.checkpointing_works ? "✓" : "✗") + "\n";
        summary += "  - Replay System: " + std::string(result.metrics.replay_works ? "✓" : "✗") + "\n";
        
        // Performance metrics
        summary += "\nPerformance Metrics:\n";
        summary += "  - Start Step: " + std::to_string(result.metrics.start_step) + "\n";
        summary += "  - End Step: " + std::to_string(result.metrics.end_step) + "\n";
        summary += "  - Neuron Count: " + std::to_string(result.metrics.neuron_count) + "\n";
        summary += "  - Synapse Count: " + std::to_string(result.metrics.synapse_count) + "\n";
        summary += "  - Firing Count: " + std::to_string(result.metrics.firing_count) + "\n";
        
        return summary;
    }
    
    // Generate comparison report between two experiment results
    static std::string generateComparisonReport(const ExperimentResult& result1, const ExperimentResult& result2) {
        std::string report = "Comparison Report\n";
        report += "==================\n\n";
        
        report += "Experiments:\n";
        report += "  - " + result1.experimentName + "\n";
        report += "  - " + result2.experimentName + "\n\n";
        
        report += "Key Metrics Comparison:\n";
        report += "  Metric               " + result1.experimentName + "     " + result2.experimentName + "\n";
        report += "  -------------------------------  ----------------  ----------------\n";
        
        // Reward comparison
        double reward1 = result1.metrics.total_reward;
        double reward2 = result2.metrics.total_reward;
        report += std::format("  Total Reward         {:>12.2f}     {:>12.2f}\n", reward1, reward2);
        
        // Firing rate comparison
        double firing1 = result1.metrics.avg_firing_rate;
        double firing2 = result2.metrics.avg_firing_rate;
        report += std::format("  Avg Firing Rate      {:>12.2f}     {:>12.2f}\n", firing1, firing2);
        
        // Episodes comparison
        size_t ep1 = result1.metrics.memory_episodes_stored;
        size_t ep2 = result2.metrics.memory_episodes_stored;
        report += std::format("  Memory Episodes      {:>12d}     {:>12d}\n", ep1, ep2);
        
        // Novelty comparison
        double novel1 = result1.metrics.novelty_level;
        double novel2 = result2.metrics.novelty_level;
        report += std::format("  Novelty Level        {:>12.2f}     {:>12.2f}\n", novel1, novel2);
        
        // Curiosity comparison
        double curious1 = result1.metrics.curiosity_level;
        double curious2 = result2.metrics.curiosity_level;
        report += std::format("  Curiosity Level      {:>12.2f}     {:>12.2f}\n", curious1, curious2);
        
        // Wall clock time comparison
        double time1 = result1.wallClockTimeSeconds;
        double time2 = result2.wallClockTimeSeconds;
        report += std::format("  Wall Clock Time      {:>12.2f}s    {:>12.2f}s\n", time1, time2);
        
        return report;
    }
    
    // Export visualization data
    static std::string exportVisualizationData(const ExperimentResult& result) {
        std::string jsonData = "{\n";
        jsonData += "  \"experiment_name\": \"" + result.experimentName + "\"," + "\n";
        jsonData += "  \"timestamp\": \"" + result.timestamp + "\"," + "\n";
        jsonData += "  \"steps\": [" + "\n";
        
        for (size_t i = 0; i < result.steps.size(); ++i) {
            const auto& step = result.steps[i];
            jsonData += "    {\"step\": " + std::to_string(step.step) + ", \"reward\": " + 
                       std::to_string(step.avgFiringRate) + ", \"firing\": " + 
                       std::to_string(step.firingCount) + "}" + (i < result.steps.size() - 1 ? "," : "") + "\n";
        }
        
        jsonData += "  ],\n";
        jsonData += "  \"metrics\": {\n";
        jsonData += "    \"total_reward\": " + std::to_string(result.metrics.total_reward) + ",\n";
        jsonData += "    \"avg_firing_rate\": " + std::to_string(result.metrics.avg_firing_rate) + ",\n";
        jsonData += "    \"memory_episodes_stored\": " + std::to_string(result.metrics.memory_episodes_stored) + "\n";
        jsonData += "  }\n";
        jsonData += "}\n";
        
        return jsonData;
    }
};

} // namespace nlm