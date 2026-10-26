// Include standard headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>
#include <chrono>
#include <thread>
#include <numeric>
#include <cmath>
#include <filesystem>

// Include NLM headers
#include "AdvancedFeatures.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "development/DevelopmentSystem.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"

namespace nlm {

// =============================================================================
// Plugin Implementation
// =============================================================================

class ExamplePlugin : public NLMPlugin {
public:
    std::string getName() const override {
        return "example_plugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Example plugin demonstrating plugin architecture";
    }
    
    void initialize(std::shared_ptr<Brain> brain) override {
        this->brain = brain;
        NLM_LOG_INFO("ExamplePlugin initialized");
    }
    
    void cleanup() override {
        NLM_LOG_INFO("ExamplePlugin cleaned up");
    }
    
    bool execute(const std::string& command, const std::vector<std::string>& args) override {
        if (command == "stats") {
            return showStats(args);
        } else if (command == "reset") {
            return resetBrain();
        } else if (command == "analyze") {
            return analyzeBrain(args);
        }
        return false;
    }
    
    std::vector<std::string> getCommands() const override {
        return {"stats", "reset", "analyze"};
    }
    
private:
    std::shared_ptr<Brain> brain;
    
    bool showStats(const std::vector<std::string>& args) {
        if (!brain) return false;
        
        NLM_LOG_INFO("=== Brain Statistics ===");
        NLM_LOG_INFO("Neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
        
        if (auto* workingMemory = brain->getWorkingMemory()) {
            NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
        }
        
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
        }
        
        if (auto* dopamine = brain->getDopamine()) {
            NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
        }
        
        return true;
    }
    
    bool resetBrain() {
        if (!brain) return false;
        brain->reset();
        NLM_LOG_INFO("Brain reset completed");
        return true;
    }
    
    bool analyzeBrain(const std::vector<std::string>& args) {
        if (!brain) return false;
        
        NLM_LOG_INFO("=== Brain Analysis ===");
        
        // Analyze connectivity
        NLM_LOG_INFO("Analyzing connectivity...");
        float eiRatio = brain->getExcitationInhibitionRatio();
        NLM_LOG_INFO("E/I ratio: " + std::to_string(eiRatio));
        
        // Analyze spike activity
        NLM_LOG_INFO("Analyzing spike activity...");
        size_t firingNeurons = brain->getFiringNeuronCount();
        NLM_LOG_INFO("Currently firing neurons: " + std::to_string(firingNeurons));
        
        // Analyze memory
        NLM_LOG_INFO("Analyzing memory systems...");
        if (auto* workingMemory = brain->getWorkingMemory()) {
            NLM_LOG_INFO("Working memory capacity: " + std::to_string(workingMemory->getCapacity()));
        }
        
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            NLM_LOG_INFO("Episodic memory max episodes: " + std::to_string(episodicMemory->getMaxEpisodes()));
        }
        
        // Analyze development
        NLM_LOG_INFO("Analyzing development...");
        DevelopmentalStage stage = brain->getDevelopmentalStage();
        NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(stage)));
        
        return true;
    }
};

// =============================================================================
// ExperimentRunner Implementation
// =============================================================================

ExperimentRunner::ExperimentRunner() {
    NLM_LOG_INFO("ExperimentRunner initialized");
}

ExperimentRunner::~ExperimentRunner() {
    NLM_LOG_INFO("ExperimentRunner destroyed");
}

bool ExperimentRunner::loadScript(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open script file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    scriptContent = buffer.str();
    
    NLM_LOG_INFO("Script loaded from: " + filepath);
    return true;
}

bool ExperimentRunner::executeScript(std::shared_ptr<Brain> brain, const std::vector<std::string>& variables) {
    // Simple script execution - for now, just log the script
    NLM_LOG_INFO("=== Executing Experiment Script ===");
    NLM_LOG_INFO(scriptContent);
    
    // Set variables (placeholder implementation)
    for (const auto& var : variables) {
        // Parse variable assignment (simplified)
        size_t equalsPos = var.find('=');
        if (equalsPos != std::string::npos) {
            std::string name = var.substr(0, equalsPos);
            std::string value = var.substr(equalsPos + 1);
            NLM_LOG_INFO("Variable: " + name + " = " + value);
        }
    }
    
    // Run the experiment (placeholder - would use script logic)
    NLM_LOG_INFO("Running experiment...");
    
    // For now, just run a basic connectivity test
    if (brain) {
        runBasicConnectivityTest(brain);
    }
    
    return true;
}

std::vector<std::string> ExperimentRunner::getAvailableExperiments() const {
    return {"connectivity_test", "plasticity_experiment", "stdp_verification", "learning_experiment"};
}

std::vector<std::string> ExperimentRunner::getExperimentParams(const std::string& experimentName) const {
    if (experimentName == "connectivity_test") {
        return {"duration", "neuron_count", "connection_probability"};
    } else if (experimentName == "plasticity_experiment") {
        return {"steps", "learning_rate", "plasticity_enabled"};
    } else if (experimentName == "stdp_verification") {
        return {"trials", "pre_neuron", "post_neuron"};
    } else if (experimentName == "learning_experiment") {
        return {"duration", "reward_function", "exploration_rate"};
    }
    return {};
}

std::string ExperimentRunner::getExperimentDescription(const std::string& experimentName) const {
    if (experimentName == "connectivity_test") {
        return "Test basic neural connectivity and spike propagation";
    } else if (experimentName == "plasticity_experiment") {
        return "Demonstrate synaptic plasticity and learning";
    } else if (experimentName == "stdp_verification") {
        return "Verify STDP (Spike-Timing-Dependent Plasticity) mechanism";
    } else if (experimentName == "learning_experiment") {
        return "Comprehensive learning experiment with memory integration";
    }
    return "Unknown experiment";
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Brain> brain, const std::string& experimentName,
                                     const std::map<std::string, std::string>& params) {
    NLM_LOG_INFO("Running experiment: " + experimentName);
    
    for (const auto& param : params) {
        NLM_LOG_INFO("  Parameter: " + param.first + " = " + param.second);
    }
    
    if (experimentName == "connectivity_test") {
        if (brain) runBasicConnectivityTest(brain);
    } else if (experimentName == "plasticity_experiment") {
        // Would run plasticity experiment
        NLM_LOG_INFO("Plasticity experiment would run here");
    } else if (experimentName == "stdp_verification") {
        // Would run STDP verification
        NLM_LOG_INFO("STDP verification would run here");
    } else if (experimentName == "learning_experiment") {
        // Would run learning experiment
        NLM_LOG_INFO("Learning experiment would run here");
    }
    
    return true;
}

bool ExperimentRunner::saveResults(const std::string& filepath, const std::string& experimentName) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open results file: " + filepath);
        return false;
    }
    
    file << "Experiment: " << experimentName << std::endl;
    file << "Timestamp: " << std::chrono::system_clock::now() << std::endl;
    file << "Results saved successfully" << std::endl;
    
    file.close();
    NLM_LOG_INFO("Results saved to: " + filepath);
    return true;
}

bool ExperimentRunner::loadResults(const std::string& filepath, std::string& experimentName,
                                  std::map<std::string, std::string>& results) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open results file: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Experiment: ") == 0) {
            experimentName = line.substr(13);
        } else if (line.find("Results:") != std::string::npos) {
            results["status"] = "completed";
        }
    }
    
    file.close();
    NLM_LOG_INFO("Results loaded from: " + filepath);
    return true;
}

// =============================================================================
// DebugTools Implementation
// =============================================================================

DebugTools::DebugTools() {
    NLM_LOG_INFO("DebugTools initialized");
}

DebugTools::~DebugTools() {
    NLM_LOG_INFO("DebugTools destroyed");
}

void DebugTools::startProfiling(const std::string& sessionName) {
    NLM_LOG_INFO("Starting profiling session: " + sessionName);
    profileSessionName = sessionName;
    profileStartTime = std::chrono::high_resolution_clock::now();
}

bool DebugTools::stopProfiling(std::string& outputFile) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - profileStartTime);
    
    NLM_LOG_INFO("Profiling session stopped. Duration: " + std::to_string(duration.count()) + "ms");
    
    // Create simple profiling report
    std::ostringstream report;
    report << "Profiling Report" << std::endl;
    report << "Session: " << profileSessionName << std::endl;
    report << "Duration: " << duration.count() << "ms" << std::endl;
    report << "Generated: " << std::chrono::system_clock::now() << std::endl;
    
    outputFile = "profile_" + profileSessionName + ".txt";
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create profiling report: " + outputFile);
        return false;
    }
    
    file << report.str();
    file.close();
    
    NLM_LOG_INFO("Profiling report saved to: " + outputFile);
    return true;
}

std::map<std::string, double> DebugTools::getProfilingStats(const std::string& sessionName) const {
    return {};
}

bool DebugTools::analyzeMemoryUsage(std::string& report) {
    // Simple memory analysis
    std::ostringstream stream;
    stream << "Memory Usage Analysis" << std::endl;
    stream << "Generated: " << std::chrono::system_clock::now() << std::endl;
    stream << "Note: This is a placeholder implementation. Real memory analysis would require system-specific tools." << std::endl;
    
    report = stream.str();
    return true;
}

bool DebugTools::analyzeSpikePatterns(std::shared_ptr<Brain> brain, std::string& report) {
    if (!brain) return false;
    
    std::ostringstream stream;
    stream << "Spike Pattern Analysis" << std::endl;
    stream << "Generated: " << std::chrono::system_clock::now() << std::endl;
    stream << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    stream << "Currently firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    stream << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    
    report = stream.str();
    return true;
}

bool DebugTools::detectBottlenecks(std::string& report) {
    std::ostringstream stream;
    stream << "Bottleneck Detection" << std::endl;
    stream << "Generated: " << std::chrono::system_clock::now() << std::endl;
    stream << "Note: This is a placeholder implementation. Real bottleneck detection would require performance profiling." << std::endl;
    
    report = stream.str();
    return true;
}

void DebugTools::enableDebugMode(bool enable) {
    debugMode = enable;
    if (enable) {
        NLM_LOG_INFO("Debug mode enabled");
    } else {
        NLM_LOG_INFO("Debug mode disabled");
    }
}

bool DebugTools::isDebugModeEnabled() const {
    return debugMode;
}

void DebugTools::setDebugLevel(LogLevel level) {
    debugLevel = level;
}

LogLevel DebugTools::getDebugLevel() const {
    return debugLevel;
}

bool DebugTools::exportDebugInfo(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create debug file: " + filepath);
        return false;
    }
    
    file << "Debug Information" << std::endl;
    file << "Generated: " << std::chrono::system_clock::now() << std::endl;
    file << "Debug mode: " << (debugMode ? "enabled" : "disabled") << std::endl;
    file << "Debug level: " << static_cast<int>(debugLevel) << std::endl;
    
    file.close();
    NLM_LOG_INFO("Debug information saved to: " + filepath);
    return true;
}

// =============================================================================
// StateAnalyzer Implementation
// =============================================================================

StateAnalyzer::StateAnalyzer() {
    NLM_LOG_INFO("StateAnalyzer initialized");
}

StateAnalyzer::~StateAnalyzer() {
    NLM_LOG_INFO("StateAnalyzer destroyed");
}

bool StateAnalyzer::analyzeState(std::shared_ptr<Brain> brain, std::string& analysis) {
    if (!brain) return false;
    
    std::ostringstream stream;
    stream << "=== Brain State Analysis ===" << std::endl;
    stream << "Generated: " << std::chrono::system_clock::now() << std::endl;
    stream << std::endl;
    stream << "Neural Components:" << std::endl;
    stream << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    stream << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    stream << "  Regions: " << brain->getRegionCount() << std::endl;
    stream << std::endl;
    
    stream << "Activity Metrics:" << std::endl;
    stream << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    stream << "  Firing neurons (this step): " << brain->getFiringNeuronCount() << std::endl;
    stream << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    stream << "  Pending spike events: " << brain->getPendingSpikeEventCount() << std::endl;
    stream << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    stream << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    stream << std::endl;
    
    stream << "Memory Systems:" << std::endl;
    if (auto* workingMemory = brain->getWorkingMemory()) {
        stream << "  Working memory: " << workingMemory->getActiveTraces() << " traces (capacity: " << workingMemory->getCapacity() << ")" << std::endl;
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        stream << "  Episodic memory: " << episodicMemory->getEpisodeCount() << " episodes (max: " << episodicMemory->getMaxEpisodes() << ")" << std::endl;
    }
    
    stream << std::endl;
    stream << "Neuromodulation:" << std::endl;
    if (auto* dopamine = brain->getDopamine()) {
        stream << "  Dopamine: " << dopamine->getLevel() << std::endl;
    }
    
    stream << "  Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    
    analysis = stream.str();
    return true;
}

bool StateAnalyzer::generateStateStats(std::shared_ptr<Brain> brain, std::string& stats) {
    return analyzeState(brain, stats);
}

bool StateAnalyzer::compareStates(const std::string& state1File, const std::string& state2File,
                                 std::string& comparison) {
    std::ifstream file1(state1File);
    std::ifstream file2(state2File);
    
    if (!file1.is_open() || !file2.is_open()) {
        NLM_LOG_ERROR("Failed to open state files for comparison");
        return false;
    }
    
    std::ostringstream stream;
    stream << "=== Brain State Comparison ===" << std::endl;
    stream << "State files compared: " << state1File << " vs " << state2File << std::endl;
    stream << "Note: This is a placeholder implementation. Real comparison would require state parsing." << std::endl;
    
    comparison = stream.str();
    return true;
}

double StateAnalyzer::calculateStateSimilarity(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
    if (!brain1 || !brain2) return 0.0;
    
    // Simple similarity calculation based on key metrics
    double totalScore = 0.0;
    double maxScore = 100.0;
    
    // Neuron count similarity
    double neuronCountRatio = std::min(brain1->getTotalNeuronCount(), brain2->getTotalNeuronCount()) / 
                             std::max(static_cast<size_t>(1), std::max(brain1->getTotalNeuronCount(), brain2->getTotalNeuronCount()));
    totalScore += neuronCountRatio * 20.0;
    
    // Synapse count similarity
    double synapseCountRatio = std::min(brain1->getTotalSynapseCount(), brain2->getTotalSynapseCount()) / 
                              std::max(static_cast<size_t>(1), std::max(brain1->getTotalSynapseCount(), brain2->getTotalSynapseCount()));
    totalScore += synapseCountRatio * 20.0;
    
    // Spike count similarity
    double spikeCountRatio = std::min(brain1->getTotalSpikeCount(), brain2->getTotalSpikeCount()) / 
                           std::max(static_cast<size_t>(1), std::max(brain1->getTotalSpikeCount(), brain2->getTotalSpikeCount()));
    totalScore += spikeCountRatio * 30.0;
    
    // Firing rate similarity
    double firingRateDiff = std::abs(brain1->getAverageFiringRate() - brain2->getAverageFiringRate());
    double firingRateSimilarity = std::max(0.0, 1.0 - firingRateDiff / 10.0); // Normalize to [0,1]
    totalScore += firingRateSimilarity * 30.0;
    
    return totalScore / maxScore * 100.0; // Return percentage
}

bool StateAnalyzer::generateVisualizationData(std::shared_ptr<Brain> brain, std::string& data) {
    std::ostringstream stream;
    stream << "Visualization Data" << std::endl;
    stream << "Generated: " << std::chrono::system_clock::now() << std::endl;
    stream << "Note: This is a placeholder implementation. Real visualization would require 3D rendering or graph generation." << std::endl;
    
    data = stream.str();
    return true;
}

bool StateAnalyzer::exportConnectivityGraph(std::shared_ptr<Brain> brain, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create connectivity graph file: " + filepath);
        return false;
    }
    
    file << "GraphML format or similar visualization format would be written here." << std::endl;
    file << "Nodes: " << (brain ? brain->getTotalNeuronCount() : 0) << std::endl;
    file << "Edges: " << (brain ? brain->getTotalSynapseCount() : 0) << std::endl;
    
    file.close();
    NLM_LOG_INFO("Connectivity graph exported to: " + filepath);
    return true;
}

bool StateAnalyzer::exportActivityPatterns(std::shared_ptr<Brain> brain, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create activity patterns file: " + filepath);
        return false;
    }
    
    file << "Activity Pattern Data" << std::endl;
    file << "Generated: " << std::chrono::system_clock::now() << std::endl;
    file << "Total spikes recorded: " << (brain ? brain->getTotalSpikeCount() : 0) << std::endl;
    
    file.close();
    NLM_LOG_INFO("Activity patterns exported to: " + filepath);
    return true;
}

bool StateAnalyzer::exportMemoryStats(std::shared_ptr<Brain> brain, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create memory stats file: " + filepath);
        return false;
    }
    
    file << "Memory Statistics" << std::endl;
    file << "Generated: " << std::chrono::system_clock::now() << std::endl;
    
    if (brain) {
        if (auto* workingMemory = brain->getWorkingMemory()) {
            file << "Working memory: " << workingMemory->getActiveTraces() << " traces" << std::endl;
        }
        
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            file << "Episodic memory: " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
        }
    }
    
    file.close();
    NLM_LOG_INFO("Memory statistics exported to: " + filepath);
    return true;
}

bool StateAnalyzer::calculateDevelopmentalMetrics(std::shared_ptr<Brain> brain, std::string& metrics) {
    if (!brain) return false;
    
    std::ostringstream stream;
    stream << "=== Developmental Metrics ===" << std::endl;
    stream << "Generated: " << std::chrono::system_clock::now() << std::endl;
    stream << std::endl;
    stream << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    
    // Calculate plasticity rate based on developmental stage
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    double plasticityRate = 1.0;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityRate = 1.0;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityRate = 0.8;
            break;
        case DevelopmentalStage::Maturation:
            plasticityRate = 0.5;
            break;
        case DevelopmentalStage::Adult:
            plasticityRate = 0.2;  // Stable
            break;
    }
    
    stream << "Estimated Plasticity Rate: " << plasticityRate << std::endl;
    stream << "Learning Potential: " << (plasticityRate * 100.0) << "%" << std::endl;
    
    metrics = stream.str();
    return true;
}

// =============================================================================
// PluginManager Implementation
// =============================================================================

PluginManager::PluginManager() {
    NLM_LOG_INFO("PluginManager initialized");
}

PluginManager::~PluginManager() {
    NLM_LOG_INFO("PluginManager destroyed");
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    // Simple plugin loading - for demonstration
    std::string pluginName = pluginPath.substr(pluginPath.find_last_of("/\") + 1);
    
    NLM_LOG_INFO("Loading plugin: " + pluginName + " from " + pluginPath);
    
    // Create a new plugin instance
    auto plugin = std::make_shared<ExamplePlugin>();
    
    // Initialize plugin with brain (would be set later)
    plugins[pluginName] = plugin;
    
    NLM_LOG_INFO("Plugin loaded successfully: " + pluginName);
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    NLM_LOG_INFO("Unloading plugin: " + pluginName);
    plugins.erase(it);
    return true;
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::vector<std::string> pluginNames;
    for (const auto& pair : plugins) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

bool PluginManager::executePluginCommand(const std::string& command, const std::vector<std::string>& args) {
    for (const auto& pair : plugins) {
        auto& plugin = pair.second;
        
        // Execute command through each plugin
        bool commandExecuted = false;
        for (const auto& pluginCommand : plugin->getCommands()) {
            if (pluginCommand == command) {
                if (plugin->execute(command, args)) {
                    commandExecuted = true;
                }
                break;
            }
        }
        
        if (commandExecuted) {
            NLM_LOG_INFO("Command executed through plugin: " + pair.first);
        }
    }
    
    return true;
}

std::vector<PluginManager::PluginInfo> PluginManager::getPluginInfo() const {
    std::vector<PluginInfo> info;
    
    for (const auto& pair : plugins) {
        PluginInfo pluginInfo;
        pluginInfo.name = pair.first;
        pluginInfo.version = "1.0.0"; // Would get from plugin
        pluginInfo.description = "Plugin description would go here";
        pluginInfo.enabled = enabledPlugins.find(pair.first) != enabledPlugins.end();
        info.push_back(pluginInfo);
    }
    
    return info;
}

bool PluginManager::setPluginEnabled(const std::string& pluginName, bool enabled) {
    if (plugins.find(pluginName) == plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    if (enabled) {
        enabledPlugins.insert(pluginName);
        NLM_LOG_INFO("Plugin enabled: " + pluginName);
    } else {
        enabledPlugins.erase(pluginName);
        NLM_LOG_INFO("Plugin disabled: " + pluginName);
    }
    
    return true;
}

bool PluginManager::isPluginEnabled(const std::string& pluginName) const {
    return enabledPlugins.find(pluginName) != enabledPlugins.end();
}

bool PluginManager::savePluginConfig(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save plugin config: " + filepath);
        return false;
    }
    
    file << "Plugin Configuration" << std::endl;
    file << "Generated: " << std::chrono::system_clock::now() << std::endl;
    file << "Loaded plugins: " << plugins.size() << std::endl;
    
    file.close();
    NLM_LOG_INFO("Plugin configuration saved to: " + filepath);
    return true;
}

bool PluginManager::loadPluginConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to load plugin config: " + filepath);
        return false;
    }
    
    // Simple config loading
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Loaded plugins: ") == 0) {
            size_t numPlugins = std::stoul(line.substr(17));
            // Would load plugins based on config
        }
    }
    
    file.close();
    NLM_LOG_INFO("Plugin configuration loaded from: " + filepath);
    return true;
}

// =============================================================================
// ScriptInterpreter Implementation
// =============================================================================

ScriptInterpreter::ScriptInterpreter() {
    NLM_LOG_INFO("ScriptInterpreter initialized");
}

ScriptInterpreter::~ScriptInterpreter() {
    NLM_LOG_INFO("ScriptInterpreter destroyed");
}

bool ScriptInterpreter::executeScript(const std::string& script, std::shared_ptr<Brain> brain,
                                     std::map<std::string, std::string>& variables) {
    NLM_LOG_INFO("=== Executing NLM Script ===");
    
    // Simple script parsing - look for NLM script commands
    std::istringstream stream(script);
    std::string line;
    
    int lineNum = 0;
    while (std::getline(stream, line)) {
        lineNum++;
        
        // Trim whitespace
        size_t startPos = line.find_first_not_of(" \t\r\n");
        size_t endPos = line.find_last_not_of(" \t\r\n");
        if (startPos == std::string::npos || endPos == std::string::npos) {
            continue; // Empty line
        }
        
        std::string trimmedLine = line.substr(startPos, endPos - startPos + 1);
        
        // Handle simple commands
        if (trimmedLine.find("run experiment:") == 0) {
            std::string experiment = trimmedLine.substr(15);
            NLM_LOG_INFO("Running experiment: " + experiment);
            // Would execute experiment
        } else if (trimmedLine.find("set variable:") == 0) {
            // Extract variable assignment
            size_t colonPos = trimmedLine.find(':');
            if (colonPos != std::string::npos) {
                std::string varExpr = trimmedLine.substr(colonPos + 1);
                size_t equalsPos = varExpr.find('=');
                if (equalsPos != std::string::npos) {
                    std::string varName = varExpr.substr(0, equalsPos);
                    std::string varValue = varExpr.substr(equalsPos + 1);
                    variables[varName] = varValue;
                    NLM_LOG_INFO("Variable set: " + varName + " = " + varValue);
                }
            }
        } else if (trimmedLine.find("if ") == 0 && trimmedLine.find(" == ") != std::string::npos) {
            // Handle if statements
            NLM_LOG_INFO("Conditional check would be executed: " + trimmedLine);
        } else if (trimmedLine.find("loop ") == 0 && trimmedLine.find(" times") != std::string::npos) {
            // Handle loops
            NLM_LOG_INFO("Loop would be executed: " + trimmedLine);
        } else if (trimmedLine.find("#") == 0) {
            // Comment - ignore
            NLM_LOG_INFO("Comment: " + trimmedLine);
        } else if (!trimmedLine.empty()) {
            NLM_LOG_INFO("Script line " + std::to_string(lineNum) + ": " + trimmedLine);
        }
    }
    
    return true;
}

bool ScriptInterpreter::executeScriptFile(const std::string& filepath, std::shared_ptr<Brain> brain,
                                         std::map<std::string, std::string>& variables) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open script file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    file.close();
    
    return executeScript(buffer.str(), brain, variables);
}

bool ScriptInterpreter::compileScript(const std::string& script, std::vector<uint8_t>& bytecode) {
    // Simple compilation - just create placeholder bytecode
    bytecode.clear();
    
    // Add magic number for bytecode
    uint32_t magic = 0x4E4C4D53; // "NLM" in hex
    bytecode.insert(bytecode.end(), reinterpret_cast<uint8_t*>(&magic), reinterpret_cast<uint8_t*>(&magic) + sizeof(magic));
    
    // Add script size
    uint32_t scriptSize = static_cast<uint32_t>(script.size());
    bytecode.insert(bytecode.end(), reinterpret_cast<uint8_t*>(&scriptSize), reinterpret_cast<uint8_t*>(&scriptSize) + sizeof(scriptSize));
    
    // Add script content
    bytecode.insert(bytecode.end(), script.begin(), script.end());
    
    NLM_LOG_INFO("Script compiled to " + std::to_string(bytecode.size()) + " bytes");
    return true;
}

bool ScriptInterpreter::executeBytecode(const std::vector<uint8_t>& bytecode, std::shared_ptr<Brain> brain,
                                       std::map<std::string, std::string>& variables) {
    // Simple bytecode execution - just check magic number
    if (bytecode.size() < 8) {
        NLM_LOG_ERROR("Invalid bytecode");
        return false;
    }
    
    uint32_t magic = *reinterpret_cast<const uint32_t*>(bytecode.data());
    if (magic != 0x4E4D4D53) { // "NLM"
        NLM_LOG_ERROR("Invalid bytecode magic number");
        return false;
    }
    
    // Extract script
    uint32_t scriptSize = *reinterpret_cast<const uint32_t*>(bytecode.data() + 4);
    std::string script(bytecode.begin() + 8, bytecode.begin() + 8 + scriptSize);
    
    NLM_LOG_INFO("Bytecode executed (simplified)");
    return executeScript(script, brain, variables);
}

std::vector<std::string> ScriptInterpreter::getScriptCommands() const {
    return {"run experiment", "set variable", "if", "loop", "print", "include", "function", "return"};
}

std::map<std::string, std::string> ScriptInterpreter::getScriptFunctions() const {
    return {
        {"run experiment", "Execute an experiment with given parameters"},
        {"set variable", "Set a script variable"},
        {"if", "Execute code conditionally"},
        {"loop", "Execute code repeatedly"},
        {"print", "Output text to console"},
        {"include", "Include another script file"},
        {"function", "Define a function"},
        {"return", "Return from function"}
    };
}

std::map<std::string, std::string> ScriptInterpreter::getScriptVariables() const {
    return {
        {"brain", "Brain object reference"},
        {"time", "Current simulation time"},
        {"step", "Current simulation step"},
        {"reward", "Current reward"},
        {"curiosity", "Current curiosity level"},
        {"novelty", "Current novelty level"},
        {"dopamine", "Current dopamine level"},
        {"memory", "Memory system reference"}
    };
}

void ScriptInterpreter::setScriptTimeout(double timeoutSeconds) {
    scriptTimeout = timeoutSeconds;
}

double ScriptInterpreter::getScriptTimeout() const {
    return scriptTimeout;
}

void ScriptInterpreter::setScriptMemoryLimit(size_t memoryBytes) {
    scriptMemoryLimit = memoryBytes;
}

size_t ScriptInterpreter::getScriptMemoryLimit() const {
    return scriptMemoryLimit;
}

} // namespace nlm
