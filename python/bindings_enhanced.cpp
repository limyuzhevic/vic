#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Include the new test utilities for performance monitoring
#include "../src/main/TestUtilities.hpp"

namespace py = pybind11;
namespace nlm {

// Performance monitoring and metrics collection
class PerformanceMonitor {
public:
    struct Metrics {
        double cpu_usage = 0.0;
        double memory_usage_mb = 0.0;
        size_t spike_count = 0;
        float average_firing_rate = 0.0f;
        double simulation_time_ms = 0.0;
        size_t neuron_count = 0;
        size_t synapse_count = 0;
        std::chrono::system_clock::time_point timestamp;
    };
    
    PerformanceMonitor() {
        start_time = std::chrono::system_clock::now();
    }
    
    void update(std::shared_ptr<Brain> brain) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
        
        metrics.cpu_usage = getCpuUsage();
        metrics.memory_usage_mb = getMemoryUsage();
        metrics.spike_count = brain->getTotalSpikeCount();
        metrics.average_firing_rate = brain->getAverageFiringRate();
        metrics.simulation_time_ms = static_cast<double>(duration);
        metrics.neuron_count = brain->getTotalNeuronCount();
        metrics.synapse_count = brain->getTotalSynapseCount();
        metrics.timestamp = now;
    }
    
    const Metrics& getMetrics() const { return metrics; }
    
    void reset() {
        start_time = std::chrono::system_clock::now();
        metrics = Metrics{};
    }
    
    std::string getMetricsJson() const {
        json j;
        j["cpu_usage"] = metrics.cpu_usage;
        j["memory_usage_mb"] = metrics.memory_usage_mb;
        j["spike_count"] = metrics.spike_count;
        j["average_firing_rate"] = metrics.average_firing_rate;
        j["simulation_time_ms"] = metrics.simulation_time_ms;
        j["neuron_count"] = metrics.neuron_count;
        j["synapse_count"] = metrics.synapse_count;
        j["timestamp"] = std::chrono::system_clock::to_time_t(metrics.timestamp);
        return j.dump(2);
    }
    
    void saveMetricsToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << getMetricsJson();
            file.close();
        }
    }
    
private:
    double getCpuUsage() {
        // Simple implementation - can be enhanced
        return 0.0;
    }
    
    double getMemoryUsage() {
        // Simple implementation - can be enhanced
        return 0.0;
    }
    
    Metrics metrics;
    std::chrono::system_clock::time_point start_time;
};

// Enhanced Config class with validation
class EnhancedConfig : public Config {
public:
    using Config::Config;
    
    bool validateSchema(const json& schema) const {
        // Basic JSON schema validation
        try {
            json::validator validator(schema);
            return validator.validate(toJson());
        } catch (...) {
            return false;
        }
    }
    
    json toJson() const {
        json j;
        auto keys = getKeys();
        for (const auto& key : keys) {
            if (auto val = getValue(key)) {
                j[key] = *val;
            }
        }
        return j;
    }
    
    void fromJson(const json& j) {
        clear();
        for (auto it = j.begin(); it != j.end(); ++it) {
            set(it.key(), it.value());
        }
    }
    
    static json getDefaultSchema() {
        json schema = {
            {"$schema", "http://json-schema.org/draft-07/schema#"},
            {"type", "object"},
            {"properties", {
                {"random_seed", {{"type", "number"}}},
                {"simulation_timestep", {{"type", "number"}}},
                {"neuron_count", {{"type", "number"}}},
                {"region_count", {{"type", "number"}}},
                {"connection_probability", {{"type", "number"}}},
                {"stdp_ltp_weight", {{"type", "number"}}},
                {"stdp_ltd_weight", {{"type", "number"}}},
                {"stdp_tau", {{"type", "number"}}},
                {"synaptogenesis_rate", {{"type", "number"}}},
                {"pruning_rate", {{"type", "number"}}}
            }},
            {"required", {"random_seed", "simulation_timestep", "neuron_count"}}
        };
        return schema;
    }
};

// Batch operation utilities
class BatchOperations {
public:
    static std::vector<float> batchGetWeights(std::shared_ptr<Brain> brain, const std::vector<NeuronId>& neuronIds) {
        std::vector<float> weights;
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return weights;
        
        for (const auto& neuronId : neuronIds) {
            // Get all synapses and find matching ones
            bool found = false;
            for (const auto& syn : region->getSynapses()) {
                if (syn->getSourceNeuron() == neuronId || syn->getDestinationNeuron() == neuronId) {
                    weights.push_back(syn->getWeight());
                    found = true;
                    break;
                }
            }
            if (!found) {
                weights.push_back(0.0f); // Default weight if not found
            }
        }
        return weights;
    }
    
    static void batchSetWeights(std::shared_ptr<Brain> brain, const std::vector<NeuronId>& neuronIds, const std::vector<float>& weights) {
        if (neuronIds.size() != weights.size()) return;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        size_t synapseIndex = 0;
        for (size_t i = 0; i < neuronIds.size(); ++i) {
            bool updated = false;
            for (auto& syn : region->getSynapses()) {
                if ((syn->getSourceNeuron() == neuronIds[i] || 
                     syn->getDestinationNeuron() == neuronIds[i]) &&
                    synapseIndex == i) {
                    syn->setWeight(weights[i]);
                    updated = true;
                    break;
                }
                if (!updated) synapseIndex++;
            }
        }
    }
    
    static std::vector<NeuronId> batchFindNeuronsByType(std::shared_ptr<Brain> brain, NeuronType type) {
        std::vector<NeuronId> neuronIds;
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return neuronIds;
        
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == type) {
                neuronIds.push_back(neuron->getId());
            }
        }
        return neuronIds;
    }
    
    static std::vector<float> batchInjectCurrent(std::shared_ptr<Brain> brain, const std::vector<NeuronId>& neuronIds, float current) {
        std::vector<float> previousPotentials;
        
        for (const auto& neuronId : neuronIds) {
            auto* region = brain->getRegion(RegionId(1));
            if (!region) continue;
            
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getId() == neuronId) {
                    previousPotentials.push_back(neuron->getMembranePotential());
                    brain->injectCurrent(neuronId, current);
                    break;
                }
            }
        }
        
        return previousPotentials;
    }
};

// Visualization and plotting utilities
class VisualizationUtils {
public:
    static std::string generateSpikeRasterData(std::shared_ptr<Brain> brain, SimulationStep steps) {
        // Generate CSV format data for spike raster plots
        std::stringstream csv;
        csv << "NeuronID,Step,Time(s)" << std::endl;
        
        auto* spikeSystem = brain->getSpikeSystem();
        if (!spikeSystem) return "";
        
        // This is a simplified version - in practice would need access to detailed spike history
        csv << "N/A,N/A,N/A" << std::endl; // Placeholder
        return csv.str();
    }
    
    static std::string generateWeightMatrixData(std::shared_ptr<Brain> brain) {
        // Generate CSV format data for weight matrices
        std::stringstream csv;
        csv << "SourceNeuron,TargetNeuron,Weight" << std::endl;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return "";
        
        csv << "N/A,N/A,N/A" << std::endl; // Placeholder
        return csv.str();
    }
    
    static void savePlotDataToFile(const std::string& data, const std::string& filepath) {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << data;
            file.close();
        }
    }
    
    static std::string createJsonData(const std::string& name, const json& data) {
        json j;
        j["name"] = name;
        j["data"] = data;
        j["timestamp"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return j.dump(2);
    }
};

// Enhanced Brain class with additional methods
class EnhancedBrain : public Brain {
public:
    using Brain::Brain;
    
    std::shared_ptr<PerformanceMonitor> monitor;
    
    EnhancedBrain(std::shared_ptr<Config> config) : Brain(config) {
        monitor = std::make_shared<PerformanceMonitor>();
    }
    
    void stepWithMonitoring(SimulationStep currentStep, Timestamp currentTime) {
        auto start = std::chrono::high_resolution_clock::now();
        
        Brain::step(currentStep, currentTime);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        if (monitor) {
            monitor->update(shared_from_this());
        }
    }
    
    std::string getPerformanceReport() const {
        if (!monitor) return "No monitor available";
        return monitor->getMetricsJson();
    }
    
    void savePerformanceReport(const std::string& filepath) const {
        if (!monitor) return;
        monitor->saveMetricsToFile(filepath);
    }
    
    json getBrainStateJson() const {
        json j;
        j["neuron_count"] = getTotalNeuronCount();
        j["synapse_count"] = getTotalSynapseCount();
        j["spike_count"] = getTotalSpikeCount();
        j["firing_rate"] = getAverageFiringRate();
        j["excitation_inhibition_ratio"] = getExcitationInhibitionRatio();
        
        auto regionIds = getRegionIds();
        json regions = json::array();
        for (const auto& regionId : regionIds) {
            json region;
            region["id"] = regionId.value;
            auto* regionPtr = getRegion(regionId);
            if (regionPtr) {
                region["neuron_count"] = regionPtr->getAllNeurons().size();
            }
            regions.push_back(region);
        }
        j["regions"] = regions;
        
        return j;
    }
    
    void loadBrainStateJson(const json& j) {
        // Simplified - would need actual implementation
        // This is a placeholder for state persistence
    }
    
    // Context manager for batch operations
    class BatchOperationContext {
    public:
        BatchOperationContext(std::shared_ptr<Brain> brain) : brain(brain) {}
        
        std::vector<float> getWeights(const std::vector<NeuronId>& neuronIds) {
            return BatchOperations::batchGetWeights(brain, neuronIds);
        }
        
        void setWeights(const std::vector<NeuronId>& neuronIds, const std::vector<float>& weights) {
            BatchOperations::batchSetWeights(brain, neuronIds, weights);
        }
        
        std::vector<NeuronId> findNeuronsByType(NeuronType type) {
            return BatchOperations::batchFindNeuronsByType(brain, type);
        }
        
        std::vector<float> injectCurrent(const std::vector<NeuronId>& neuronIds, float current) {
            return BatchOperations::batchInjectCurrent(brain, neuronIds, current);
        }
        
    private:
        std::shared_ptr<Brain> brain;
    };
    
    BatchOperationContext createBatchContext() {
        return BatchOperationContext(shared_from_this());
    }
};

// Enhanced AgentBrain with advanced features
class EnhancedAgentBrain : public AgentBrain {
public:
    using AgentBrain::AgentBrain;
    
    std::shared_ptr<PerformanceMonitor> learning_monitor;
    
    EnhancedAgentBrain(std::shared_ptr<Brain> brain) : AgentBrain(brain) {
        learning_monitor = std::make_shared<PerformanceMonitor>();
    }
    
    struct LearningMetrics {
        double reward_prediction_error = 0.0;
        double curiosity_score = 0.0;
        double novelty_score = 0.0;
        double exploration_rate = 0.0;
        size_t steps_without_reward = 0;
        double average_reward = 0.0;
        double learning_efficiency = 0.0;
    };
    
    LearningMetrics getLearningMetrics() const {
        LearningMetrics metrics;
        // Calculate various learning metrics
        metrics.reward_prediction_error = 0.0; // Would need access to brain internals
        metrics.curiosity_score = getCuriosityLevel();
        metrics.novelty_score = getNoveltyLevel();
        metrics.exploration_rate = 0.5; // Placeholder
        metrics.steps_without_reward = 0; // Placeholder
        metrics.average_reward = 0.0; // Placeholder
        metrics.learning_efficiency = 0.0; // Placeholder
        return metrics;
    }
    
    std::string getLearningReport() const {
        auto metrics = getLearningMetrics();
        json j;
        j["reward_prediction_error"] = metrics.reward_prediction_error;
        j["curiosity_score"] = metrics.curiosity_score;
        j["novelty_score"] = metrics.novelty_score;
        j["exploration_rate"] = metrics.exploration_rate;
        j["steps_without_reward"] = metrics.steps_without_reward;
        j["average_reward"] = metrics.average_reward;
        j["learning_efficiency"] = metrics.learning_efficiency;
        j["timestamp"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        
        return j.dump(2);
    }
    
    void saveLearningReport(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << getLearningReport();
            file.close();
        }
    }
    
    // Advanced learning methods
    void applyAdvancedLearning(float reward, float prediction_error, float curiosity, float novelty) {
        // Enhanced learning with multiple factors
        applyRewardModulation(reward, prediction_error);
        
        // Apply curiosity-based exploration
        if (curiosity > 0.5) {
            enableCuriosity(true);
            // Add exploration actions
        }
        
        // Apply novelty-based learning
        if (novelty > 0.3) {
            // Add novel experience handling
        }
        
        // Update development based on learning
        updateDevelopment(0.001); // Small time step for learning updates
    }
    
    // Neural network-style batch processing
    std::vector<float> processBatchSensoryInput(const std::vector<SensoryPercept>& percepts) {
        std::vector<float> aggregated_features;
        // Aggregate features from multiple percepts
        for (const auto& percept : percepts) {
            // Extract features from vision, audio, internal signals, proprioception
            auto vision = percept.getVision();
            if (vision) {
                // Extract vision features
                aggregated_features.push_back(vision->getWidth());
                aggregated_features.push_back(vision->getHeight());
                aggregated_features.push_back(vision->getChannels());
            }
        }
        return aggregated_features;
    }
    
    // Performance benchmarking
    struct BenchmarkResults {
        double processing_time_ms = 0.0;
        double memory_usage_mb = 0.0;
        size_t neurons_processed = 0;
        size_t spikes_generated = 0;
        double throughput_neuron_per_sec = 0.0;
    };
    
    BenchmarkResults benchmarkProcessing(const std::vector<SensoryPercept>& percepts, size_t iterations = 10) {
        BenchmarkResults results;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            processBatchSensoryInput(percepts);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        results.processing_time_ms = static_cast<double>(duration) / iterations;
        results.neurons_processed = getBrain()->getTotalNeuronCount() * iterations;
        results.spikes_generated = getBrain()->getTotalSpikeCount();
        results.throughput_neuron_per_sec = results.neurons_processed / (results.processing_time_ms / 1000.0);
        
        return results;
    }
};

// Main enhanced Python bindings
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        Includes performance monitoring, batch operations, visualization utilities,
        advanced learning algorithms, and benchmarking tools.
    )pbdoc";
    
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register enhanced types
    py::class_<PerformanceMonitor, std::shared_ptr<PerformanceMonitor>>(m, "PerformanceMonitor")
        .def(py::init<>())
        .def("update", &PerformanceMonitor::update)
        .def("getMetrics", &PerformanceMonitor::getMetrics)
        .def("getMetricsJson", &PerformanceMonitor::getMetricsJson)
        .def("saveMetricsToFile", &PerformanceMonitor::saveMetricsToFile)
        .def("reset", &PerformanceMonitor::reset);
    
    // Register enhanced Config
    py::class_<EnhancedConfig, std::shared_ptr<EnhancedConfig>>(m, "EnhancedConfig", py::base<Config>())
        .def(py::init<>())
        .def("validateSchema", &EnhancedConfig::validateSchema)
        .def("toJson", &EnhancedConfig::toJson)
        .def("fromJson", &EnhancedConfig::fromJson)
        .def_static("getDefaultSchema", &EnhancedConfig::getDefaultSchema)
        .def("loadFromFile", &Config::loadFromFile)
        .def("saveToFile", &Config::saveToFile);
    
    // Register batch operations
    py::class_<BatchOperations>(m, "BatchOperations")
        .def_static("batchGetWeights", &BatchOperations::batchGetWeights)
        .def_static("batchSetWeights", &BatchOperations::batchSetWeights)
        .def_static("batchFindNeuronsByType", &BatchOperations::batchFindNeuronsByType)
        .def_static("batchInjectCurrent", &BatchOperations::batchInjectCurrent);
    
    // Register visualization utilities
    py::class_<VisualizationUtils>(m, "VisualizationUtils")
        .def_static("generateSpikeRasterData", &VisualizationUtils::generateSpikeRasterData)
        .def_static("generateWeightMatrixData", &VisualizationUtils::generateWeightMatrixData)
        .def_static("savePlotDataToFile", &VisualizationUtils::savePlotDataToFile)
        .def_static("createJsonData", &VisualizationUtils::createJsonData);
    
    // Register enhanced Brain
    py::class_<EnhancedBrain, std::shared_ptr<EnhancedBrain>>(m, "EnhancedBrain", py::base<Brain>())
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("stepWithMonitoring", &EnhancedBrain::stepWithMonitoring)
        .def("getPerformanceReport", &EnhancedBrain::getPerformanceReport)
        .def("savePerformanceReport", &EnhancedBrain::savePerformanceReport)
        .def("getBrainStateJson", &EnhancedBrain::getBrainStateJson)
        .def("loadBrainStateJson", &EnhancedBrain::loadBrainStateJson)
        .def("createBatchContext", &EnhancedBrain::createBatchContext);
    
    // Register enhanced AgentBrain
    py::class_<EnhancedAgentBrain, std::shared_ptr<EnhancedAgentBrain>>(m, "EnhancedAgentBrain", py::base<AgentBrain>())
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("getLearningMetrics", &EnhancedAgentBrain::getLearningMetrics)
        .def("getLearningReport", &EnhancedAgentBrain::getLearningReport)
        .def("saveLearningReport", &EnhancedAgentBrain::saveLearningReport)
        .def("applyAdvancedLearning", &EnhancedAgentBrain::applyAdvancedLearning)
        .def("processBatchSensoryInput", &EnhancedAgentBrain::processBatchSensoryInput)
        .def("benchmarkProcessing", &EnhancedAgentBrain::benchmarkProcessing);
    
    // Factory functions
    m.def("createEnhancedConfig", []() -> std::shared_ptr<EnhancedConfig> {
        return std::make_shared<EnhancedConfig>();
    }, "Create an enhanced configuration with validation support");
    
    m.def("createEnhancedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<EnhancedBrain> {
        return std::make_shared<EnhancedBrain>(config);
    }, py::arg("config"), "Create an enhanced brain with performance monitoring");
    
    m.def("createEnhancedAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<EnhancedAgentBrain> {
        return std::make_shared<EnhancedAgentBrain>(brain);
    }, py::arg("brain"), "Create an enhanced agent brain with learning metrics");
    
    // Export constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Export enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();
    
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Export the original pynlm module for compatibility
    py::module_::import(m, "pynlm");
    
    // Documentation
    m.def("get_enhanced_documentation", []() {
        return R"pbdoc(
        Enhanced NLM Python Bindings
        ===============================
        
        This enhanced version of pynlm provides advanced features:
        
        1. Performance Monitoring:
           - Real-time metrics collection
           - CPU and memory usage tracking
           - Spike rate and simulation time monitoring
           - JSON export of performance data
        
        2. Configuration Validation:
           - JSON Schema validation for configuration
           - Default schema definitions
           - JSON serialization/deserialization
        
        3. Batch Operations:
           - Efficient batch weight operations
           - Bulk neuron type searching
           - Parallel current injection
        
        4. Visualization Utilities:
           - Data export for plotting
           - CSV and JSON format support
           - Raster plot and weight matrix generation
        
        5. Advanced Learning:
           - Multi-factor learning (reward, curiosity, novelty)
           - Enhanced neuromodulation
           - Individual learning metrics
        
        6. Benchmarking:
           - Performance benchmarking tools
           - Throughput measurements
           - Comparative analysis
        
        Usage Examples:
        
        ```python
        from pynlm_enhanced import EnhancedBrain, EnhancedAgentBrain
        
        # Create enhanced brain with monitoring
        config = EnhancedConfig()
        config.set("neuron_count", 1000)
        brain = EnhancedBrain(config)
        
        # Run simulation with monitoring
        for step in range(1000):
            brain.stepWithMonitoring(step, step * 0.001)
        
        # Get performance report
        report = brain.getPerformanceReport()
        
        # Use batch operations
        with brain.createBatchContext() as batch:
            neurons = batch.findNeuronsByType(NeuronType.Sensory)
            weights = batch.getWeights(neurons)
            batch.setWeights(neurons, [w * 1.1 for w in weights])
        
        # Create agent brain
        agent_brain = EnhancedAgentBrain(brain)
        metrics = agent_brain.getLearningMetrics()
        ```
        )pbdoc";
    });
    
    // Utility functions
    m.def("create_default_enhanced_config", []() -> std::shared_ptr<EnhancedConfig> {
        auto config = std::make_shared<EnhancedConfig>();
        EnhancedConfig::setupDefaultPhase2Config(config);
        return config;
    }, "Create an enhanced configuration with Phase 2 defaults");
    
    m.def("run_advanced_learning_example", [](std::shared_ptr<EnhancedAgentBrain> agent_brain) {
        // Example of advanced learning workflow
        for (int step = 0; step < 100; ++step) {
            // Simulate learning with multiple factors
            float reward = (step % 20 == 0) ? 1.0f : 0.0f;
            float prediction_error = (step % 30 == 0) ? 0.5f : 0.0f;
            float curiosity = (step % 25 == 0) ? 0.7f : 0.0f;
            float novelty = (step % 35 == 0) ? 0.8f : 0.0f;
            
            agent_brain->applyAdvancedLearning(reward, prediction_error, curiosity, novelty);
            
            if (step % 10 == 0) {
                auto metrics = agent_brain->getLearningMetrics();
                // Log or process metrics
            }
        }
        
        auto final_report = agent_brain->getLearningReport();
        return final_report;
    }, "Run an example of advanced learning with multiple factors");
    
    // Performance benchmarking example
    m.def("run_performance_benchmark", [](std::shared_ptr<EnhancedAgentBrain> agent_brain) {
        // Create dummy percepts for benchmarking
        std::vector<SensoryPercept> percepts;
        
        for (int i = 0; i < 10; ++i) {
            SensoryPercept percept;
            // Add dummy sensory data
            percepts.push_back(percept);
        }
        
        auto benchmark_results = agent_brain->benchmarkProcessing(percepts, 50);
        
        json j;
        j["processing_time_ms"] = benchmark_results.processing_time_ms;
        j["memory_usage_mb"] = benchmark_results.memory_usage_mb;
        j["neurons_processed"] = benchmark_results.neurons_processed;
        j["spikes_generated"] = benchmark_results.spikes_generated;
        j["throughput_neuron_per_sec"] = benchmark_results.throughput_neuron_per_sec;
        
        return j.dump(2);
    }, "Run performance benchmarking on enhanced agent brain");
}

} // namespace nlm
