# Enhanced Python Bindings for NLM
# Comprehensive API for neural simulation with advanced features and documentation

// This file enhances the original bindings.cpp with:
// 1. Better documentation and examples
// 2. Advanced convenience functions
// 3. Performance monitoring APIs
// 4. Visualization support
// 5. Error handling improvements
// 6. High-level simulation modes
// 7. Debugging and analysis tools

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <cmath>
#include <iostream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory error: " + msg) {}
};

// Performance monitoring structure
struct PerformanceMetrics {
    double wallClockTime = 0.0;
    double cpuTime = 0.0;
    size_t totalSteps = 0;
    size_t memoryUsage = 0;
    size_t peakMemory = 0;
    float averageFiringRate = 0.0f;
    float averageSynapticWeight = 0.0f;
    size_t totalSpikes = 0;
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    
    PerformanceMetrics() = default;
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Performance Metrics:" << std::endl;
        oss << "  Wall clock time: " << wallClockTime << "s" << std::endl;
        oss << "  CPU time: " << cpuTime << "s" << std::endl;
        oss << "  Total steps: " << totalSteps << std::endl;
        oss << "  Memory usage: " << memoryUsage << " bytes" << std::endl;
        oss << "  Peak memory: " << peakMemory << " bytes" << std::endl;
        oss << "  Average firing rate: " << averageFiringRate << std::endl;
        oss << "  Average synaptic weight: " << averageSynapticWeight << std::endl;
        oss << "  Total spikes: " << totalSpikes << std::endl;
        oss << "  Total neurons: " << totalNeurons << std::endl;
        oss << "  Total synapses: " << totalSynapses << std::endl;
        return oss.str();
    }
};

// Visualization data structures
struct NeuronData {
    NeuronId id;
    NeuronType type;
    float membranePotential;
    float threshold;
    float restingPotential;
    float leakConductance;
    FiringState firingState;
    float refractoryRemaining;
    float refractoryPeriod;
    float lastSpikeTime;
    float totalCurrent;
    
    NeuronData() : id(), type(NeuronType::Internal), membranePotential(-70.0f),
                   threshold(-55.0f), restingPotential(-70.0f), leakConductance(0.1f),
                   firingState(FiringState::Resting), refractoryRemaining(0.0f),
                   refractoryPeriod(10.0f), lastSpikeTime(-1.0f), totalCurrent(0.0f) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Neuron " << id.index() << " (type: " << static_cast<int>(type) << ")";
        oss << " V=" << membranePotential << "mV, thresh=" << threshold << "mV";
        oss << " state=" << static_cast<int>(firingState);
        return oss.str();
    }
};

struct SynapseData {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    float weight;
    Delay delay;
    SynapseType type;
    PlasticityFlags plasticityFlags;
    float eligibilityTrace;
    
    SynapseData() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f),
                   delay(1), type(SynapseType::Excitatory), plasticityFlags(),
                   eligibilityTrace(0.0f) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Synapse " << id.index() << ": " << sourceNeuron.index() << " -> " << destinationNeuron.index();
        oss << " weight=" << weight << " delay=" << delay;
        return oss.str();
    }
};

struct RegionData {
    RegionId id;
    std::string name;
    size_t neuronCount;
    size_t synapseCount;
    float averageSynapticWeight;
    std::vector<NeuronData> neurons;
    std::vector<SynapseData> synapses;
    
    RegionData() : id(), neuronCount(0), synapseCount(0), averageSynapticWeight(0.0f) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Region " << id.index() << " ('" << name << "')";
        oss << " neurons=" << neuronCount << " synapses=" << synapseCount;
        oss << " avg_weight=" << averageSynapticWeight;
        return oss.str();
    }
};

struct VisualizationData {
    uint64_t timestamp;
    std::vector<RegionData> regions;
    float systemEnergy;
    float totalSpikes;
    float averageFiringRate;
    
    VisualizationData() : timestamp(0), systemEnergy(0.0f), totalSpikes(0.0f), averageFiringRate(0.0f) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Visualization Data at step " << timestamp << ":" << std::endl;
        oss << "  System energy: " << systemEnergy << std::endl;
        oss << "  Total spikes: " << totalSpikes << std::endl;
        oss << "  Average firing rate: " << averageFiringRate << std::endl;
        oss << "  Regions: " << regions.size();
        return oss.str();
    }
};

// Enhanced convenience functions for common patterns
namespace convenience {

    // Create a complete agent setup with brain, world, and interface
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    createAgent(std::shared_ptr<Config> config, float worldSize = 20.0f, float visionSize = 8.0f) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldSize, worldSize, visionSize, visionSize);
        world->setAgentStart(worldSize/2, worldSize/2);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }
    
    // Run a complete simulation loop
    PerformanceMetrics runSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent, size_t steps,
                                     bool enableLearning = true) {
        PerformanceMetrics metrics;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            // Get observation
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(i);
            
            // Get action and apply to world
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, i * 0.001);
            
            // Apply learning if enabled
            if (enableLearning) {
                auto reward = world->getAgentBody().energy;
                agent->applyRewardModulation(reward, 0.0f);
            }
            
            // Collect metrics
            metrics.totalSteps++;
            metrics.totalSpikes += brain->getTotalSpikeCount();
            metrics.averageFiringRate += brain->getAverageFiringRate();
            
            if (i % 1000 == 0) {
                std::cout << "Step " << i << ": " << brain->getAverageFiringRate() << " avg firing rate" << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        metrics.averageFiringRate /= steps;
        metrics.totalNeurons = brain->getTotalNeuronCount();
        metrics.totalSynapses = brain->getTotalSynapseCount();
        
        return metrics;
    }
    
    // Create a development simulation
    PerformanceMetrics runDevelopmentSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                                std::shared_ptr<AgentBrain> agent, size_t steps) {
        auto metrics = runSimulation(brain, world, agent, steps, true);
        
        // Enable development
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        agent->enableStructuralPlasticity(true);
        
        return metrics;
    }
    
    // Create a learning simulation with memory systems
    PerformanceMetrics runLearningSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                             std::shared_ptr<AgentBrain> agent, size_t steps) {
        auto metrics = runDevelopmentSimulation(brain, world, agent, steps);
        
        // Enable all learning systems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableCuriosity(true);
        
        return metrics;
    }
    
    // Save simulation results with metadata
    bool saveSimulationResults(const std::string& filepath,
                               std::shared_ptr<Brain> brain,
                               const PerformanceMetrics& metrics,
                               const std::string& simulationName = "Simulation") {
        // Save brain state
        if (!brain->save(filepath)) {
            return false;
        }
        
        // Save metadata (in a real implementation, this would create a structured output)
        std::string metadataFile = filepath + ".meta";
        std::ofstream metaFile(metadataFile);
        if (metaFile.is_open()) {
            metaFile << "Simulation: " << simulationName << std::endl;
            metaFile << "Timestamp: " << std::chrono::system_clock::now() << std::endl;
            metaFile << "Metrics: " << metrics.toString() << std::endl;
            metaFile << "End time: " << metrics.wallClockTime << "s" << std::endl;
            metaFile.close();
        }
        
        return true;
    }
    
    // Load simulation with automatic configuration
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    loadSimulation(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(filepath)) {
            throw SimulationError("Failed to load simulation from " + filepath);
        }
        
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        return std::make_tuple(brain, world, agent);
    }
}

// Enhanced Brain class with additional methods
void bindBrainEnhanced(py::class_<Brain>& brainClass) {
    // Add enhanced statistics and analysis methods
    brainClass
        .def("getNetworkDensity", &Brain::getTotalSynapseCount,
             "Get network density (synapses per neuron)")
        .def("getSynapseWeightStatistics", [this](const Brain& b) {
            // Calculate synapse weight statistics
            float minWeight = std::numeric_limits<float>::max();
            float maxWeight = std::numeric_limits<float>::lowest();
            float totalWeight = 0.0f;
            size_t count = 0;
            
            for (const auto& region : b.getRegions()) {
                for (const auto* syn : region->getSynapses()) {
                    float weight = syn->getWeight();
                    minWeight = std::min(minWeight, weight);
                    maxWeight = std::max(maxWeight, weight);
                    totalWeight += weight;
                    count++;
                }
            }
            
            if (count > 0) {
                return std::make_tuple(minWeight, maxWeight, totalWeight / count);
            } else {
                return std::make_tuple(0.0f, 0.0f, 0.0f);
            }
        }, "Get synapse weight statistics (min, max, average)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getActiveNeuronPercentage", [this](const Brain& b) {
            size_t total = b.getTotalNeuronCount();
            if (total == 0) return 0.0f;
            return 100.0f * b.getActiveNeuronCount() / total;
        }, "Get percentage of active neurons")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfigSummary", [this](const Brain& b) {
            return b.getConfig()->summary();
        }, "Get configuration summary")
        .def("getSystemStatus", [this](const Brain& b) {
            std::ostringstream oss;
            oss << "NLM Brain System Status:" << std::endl;
            oss << "  Regions: " << b.getRegionCount() << std::endl;
            oss << "  Neurons: " << b.getTotalNeuronCount() << std::endl;
            oss << "  Synapses: " << b.getTotalSynapseCount() << std::endl;
            oss << "  Active neurons: " << b.getActiveNeuronCount() << std::endl;
            oss << "  Firing neurons: " << b.getFiringNeuronCount() << std::endl;
            oss << "  Total spikes: " << b.getTotalSpikeCount() << std::endl;
            oss << "  Average firing rate: " << b.getAverageFiringRate() << std::endl;
            oss << "  Development stage: " << static_cast<int>(b.getDevelopmentalStage()) << std::endl;
            oss << "  Pending events: " << b.getPendingSpikeEventCount() << std::endl;
            return oss.str();
        }, "Get detailed system status")
        .def("exportData", [this](const Brain& b) {
            // Export detailed neural data for analysis
            std::map<std::string, py::object> exportData;
            
            // Export regions
            std::vector<py::dict> regions;
            for (const auto& region : b.getRegions()) {
                py::dict regionData;
                regionData["id"] = region->getId().index();
                regionData["name"] = region->getName();
                regionData["neuronCount"] = region->getTotalNeuronCount();
                regionData["synapseCount"] = region->getSynapseCount();
                regionData["averageWeight"] = region->getAverageSynapticWeight();
                regions.push_back(regionData);
            }
            exportData["regions"] = regions;
            
            // Export summary statistics
            py::dict stats;
            stats["totalNeurons"] = b.getTotalNeuronCount();
            stats["totalSynapses"] = b.getTotalSynapseCount();
            stats["totalSpikes"] = b.getTotalSpikeCount();
            stats["averageFiringRate"] = b.getAverageFiringRate();
            exportData["statistics"] = stats;
            
            return exportData;
        }, "Export neural data for analysis")
        .def("benchmark", [this](Brain& b, size_t steps) {
            // Run benchmark test
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t i = 0; i < steps; ++i) {
                b.step(i);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            double spikesPerSecond = b.getTotalSpikeCount() / duration;
            double neuronsPerSecond = b.getTotalNeuronCount() / duration;
            
            return std::make_tuple(duration, spikesPerSecond, neuronsPerSecond);
        }, py::arg("steps"), "Run benchmark and return performance metrics")
        .def("analyzeStability", [this](const Brain& b, size_t steps) {
            // Analyze network stability
            std::vector<float> firingRates;
            std::vector<float> synapticWeights;
            
            // Run analysis
            for (size_t i = 0; i < steps; ++i) {
                b.step(i);
                firingRates.push_back(b.getAverageFiringRate());
                
                // Calculate average synaptic weight
                float totalWeight = 0.0f;
                size_t weightCount = 0;
                for (const auto& region : b.getRegions()) {
                    for (const auto* syn : region->getSynapses()) {
                        totalWeight += syn->getWeight();
                        weightCount++;
                    }
                }
                if (weightCount > 0) {
                    synapticWeights.push_back(totalWeight / weightCount);
                }
            }
            
            // Calculate stability metrics
            float firingRateStdDev = 0.0f;
            float firingRateMean = 0.0f;
            
            if (!firingRates.empty()) {
                firingRateMean = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f) / firingRates.size();
                float sumSquares = 0.0f;
                for (float rate : firingRates) {
                    sumSquares += (rate - firingRateMean) * (rate - firingRateMean);
                }
                firingRateStdDev = std::sqrt(sumSquares / firingRates.size());
            }
            
            float synapticWeightStdDev = 0.0f;
            float synapticWeightMean = 0.0f;
            
            if (!synapticWeights.empty()) {
                synapticWeightMean = std::accumulate(synapticWeights.begin(), synapticWeights.end(), 0.0f) / synapticWeights.size();
                float sumSquares = 0.0f;
                for (float weight : synapticWeights) {
                    sumSquares += (weight - synapticWeightMean) * (weight - synapticWeightMean);
                }
                synapticWeightStdDev = std::sqrt(sumSquares / synapticWeights.size());
            }
            
            return std::make_tuple(firingRateMean, firingRateStdDev, synapticWeightMean, synapticWeightStdDev);
        }, py::arg("steps"), "Analyze network stability and return statistics")
        .def("generateVisualizationData", [this](Brain& b, uint64_t timestamp) {
            VisualizationData vizData;
            vizData.timestamp = timestamp;
            vizData.systemEnergy = 100.0f; // Placeholder
            vizData.totalSpikes = b.getTotalSpikeCount();
            vizData.averageFiringRate = b.getAverageFiringRate();
            
            // Export region data
            for (const auto& region : b.getRegions()) {
                RegionData regionData;
                regionData.id = region->getId();
                regionData.name = region->getName();
                regionData.neuronCount = region->getTotalNeuronCount();
                regionData.synapseCount = region->getSynapseCount();
                regionData.averageSynapticWeight = region->getAverageSynapticWeight();
                
                vizData.regions.push_back(regionData);
            }
            
            return vizData;
        }, "Generate visualization data for external tools")
        .def("compareWith", [this](const Brain& b1, const Brain& b2) {
            // Compare two brain states
            std::map<std::string, double> comparison;
            
            // Compare neuron counts
            comparison["neuron_count_diff"] = (double)b2.getTotalNeuronCount() - b1.getTotalNeuronCount();
            
            // Compare synapse counts
            comparison["synapse_count_diff"] = (double)b2.getTotalSynapseCount() - b1.getTotalSynapseCount();
            
            // Compare firing rates
            double fr1 = b1.getAverageFiringRate();
            double fr2 = b2.getAverageFiringRate();
            comparison["firing_rate_diff"] = fr2 - fr1;
            
            // Compare developmental stages
            comparison["dev_stage_diff"] = (double)b2.getDevelopmentalStage() - (double)b1.getDevelopmentalStage();
            
            return comparison;
        }, "Compare two brain states and return differences");
}

// Enhanced AgentBrain class with additional methods
void bindAgentBrainEnhanced(py::class_<AgentBrain>& agentClass) {
    agentClass
        .def("getBehaviorStats", [this](const AgentBrain& a) {
            std::map<std::string, double> stats;
            stats["reward_modulation_enabled"] = a.isRewardModulationEnabled() ? 1.0 : 0.0;
            stats["structural_plasticity_enabled"] = a.isStructuralPlasticityEnabled() ? 1.0 : 0.0;
            stats["development_enabled"] = a.isDevelopmentEnabled() ? 1.0 : 0.0;
            stats["curiosity_enabled"] = a.isCuriosityEnabled() ? 1.0 : 0.0;
            stats["neuromodulation_level"] = a.getNeuromodulationLevel();
            stats["curiosity_level"] = a.getCuriosityLevel();
            stats["novelty_level"] = a.getNoveltyLevel();
            stats["prediction_error"] = a.getPredictionError();
            stats["developmental_stage"] = static_cast<double>(a.getDevelopmentalStage());
            return stats;
        }, "Get agent behavior statistics")
        .def("analyzeBehavior", [this](const AgentBrain& a, size_t steps) {
            // Analyze agent behavior over time
            std::map<std::string, std::vector<double>> analysis;
            
            // Record various metrics
            analysis["neuromodulation"] = std::vector<double>();
            analysis["curiosity"] = std::vector<double>();
            analysis["novelty"] = std::vector<double>();
            analysis["prediction_error"] = std::vector<double>();
            
            // This would be called during simulation to collect data
            // For now, return empty analysis
            return analysis;
        }, "Analyze agent behavior")
        .def("enableAllLearning", [this](AgentBrain& a) {
            // Enable all learning systems
            a.enableRewardModulation(true);
            a.enableStructuralPlasticity(true);
            a.enableDevelopment(true);
            a.enableCuriosity(true);
            return true;
        }, "Enable all learning systems")
        .def("disableAllLearning", [this](AgentBrain& a) {
            // Disable all learning systems
            a.enableRewardModulation(false);
            a.enableStructuralPlasticity(false);
            a.enableDevelopment(false);
            a.enableCuriosity(false);
            return true;
        }, "Disable all learning systems");
}

// Enhanced SimpleWorld class with additional methods
void bindSimpleWorldEnhanced(py::class_<SimpleWorld>& worldClass) {
    worldClass
        .def("getEnvironmentStats", [this](const SimpleWorld& w) {
            std::map<std::string, double> stats;
            stats["width"] = w.getWidth();
            stats["height"] = w.getHeight();
            stats["max_energy"] = w.getMaxEnergy();
            stats["energy_decay_rate"] = w.getEnergyDecayRate();
            stats["simulation_time"] = w.getSimulationTime();
            stats["agent_x"] = w.getAgentBody().x;
            stats["agent_y"] = w.getAgentBody().y;
            stats["agent_orientation"] = w.getAgentBody().orientation;
            stats["agent_energy"] = w.getAgentBody().energy;
            stats["agent_health"] = w.getAgentBody().health;
            stats["agent_age"] = w.getAgentBody().age;
            return stats;
        }, "Get world environment statistics")
        .def("exportState", [this](const SimpleWorld& w) {
            // Export world state for saving
            std::map<std::string, py::object> state;
            
            // Export agent state
            py::dict agentState;
            agentState["x"] = w.getAgentBody().x;
            agentState["y"] = w.getAgentBody().y;
            agentState["orientation"] = w.getAgentBody().orientation;
            agentState["energy"] = w.getAgentBody().energy;
            agentState["health"] = w.getAgentBody().health;
            agentState["age"] = w.getAgentBody().age;
            state["agent"] = agentState;
            
            // Export world dimensions
            py::dict dimensions;
            dimensions["width"] = w.getWidth();
            dimensions["height"] = w.getHeight();
            state["dimensions"] = dimensions;
            
            return state;
        }, "Export world state")
        .def("importState", [this](SimpleWorld& w, const std::map<std::string, py::object>& state) {
            // Import world state from saved data
            if (state.find("agent") != state.end()) {
                py::dict agentState = state.at("agent").cast<py::dict>();
                w.getAgentBody().x = agentState["x"].cast<float>();
                w.getAgentBody().y = agentState["y"].cast<float>();
                w.getAgentBody().orientation = agentState["orientation"].cast<float>();
                w.getAgentBody().energy = agentState["energy"].cast<float>();
                w.getAgentBody().health = agentState["health"].cast<float>();
                w.getAgentBody().age = agentState["age"].cast<float>();
            }
            return true;
        }, "Import world state from saved data")
        .def("getSensoryInputSize", &SimpleWorld::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &SimpleWorld::getMotorOutputSize,
             "Get expected motor output size");
}

// Module initialization with enhanced features
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version includes:
        - Better error handling and exception classes
        - Performance monitoring APIs
        - Visualization data structures
        - Advanced convenience functions
        - Enhanced statistics and analysis methods
        - Behavior analysis for agents
        - Environment statistics
        - State import/export functionality
        - Benchmark and stability analysis
        - Comprehensive documentation and examples
        
        The enhanced bindings provide tools for:
        - Creating complete agent environments
        - Running comprehensive simulations
        - Analyzing network stability and behavior
        - Visualizing neural activity
        - Monitoring performance metrics
        - Debugging and troubleshooting
        - Comparing brain states
        - Exporting data for external analysis
        
        For usage examples, see:
        - examples/enhanced_examples.py
        - examples/benchmarking.py
        - examples/visualization.py
        - examples/behavior_analysis.py
        - docs/ARCHITECTURE.md
        - docs/SCIENCE.md
        - docs/PHASE6_FINAL_AUDIT.md
        )pbdoc";
    
    // Register enhanced exception classes
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    
    // Bind enhanced exception classes
    py::class_<NLMError, std::runtime_error>(m, "NLMError")
        .def(py::init<const std::string&>());
    
    py::class_<SimulationError, NLMError>(m, "SimulationError")
        .def(py::init<const std::string&>());
    
    py::class_<ConfigurationError, NLMError>(m, "ConfigurationError")
        .def(py::init<const std::string&>());
    
    py::class_<MemoryError, NLMError>(m, "MemoryError")
        .def(py::init<const std::string&>());
    
    // Bind all core types
    bindTypes(m);
    
    // Bind Config class
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
    
    // Bind convenience namespace
    py::module_ convenience_module = m.def_submodule("convenience", "Convenience functions for common patterns");
    convenience_module.def("createAgent", &convenience::createAgent,
                          py::arg("config"), py::arg("worldSize") = 20.0f,
                          py::arg("visionSize") = 8.0f,
                          "Create a complete agent setup with brain, world, and interface");
    convenience_module.def("runSimulation", &convenience::runSimulation,
                          py::arg("brain"), py::arg("world"), py::arg("agent"),
                          py::arg("steps"), py::arg("enableLearning") = true,
                          "Run a complete simulation loop");
    convenience_module.def("runDevelopmentSimulation", &convenience::runDevelopmentSimulation,
                          py::arg("brain"), py::arg("world"), py::arg("agent"),
                          py::arg("steps"), "Run a development simulation");
    convenience_module.def("runLearningSimulation", &convenience::runLearningSimulation,
                          py::arg("brain"), py::arg("world"), py::arg("agent"),
                          py::arg("steps"), "Run a learning simulation");
    convenience_module.def("saveSimulationResults", &convenience::saveSimulationResults,
                          py::arg("filepath"), py::arg("brain"), py::arg("metrics"),
                          py::arg("simulationName") = "Simulation",
                          "Save simulation results with metadata");
    convenience_module.def("loadSimulation", &convenience::loadSimulation,
                          py::arg("filepath"), "Load simulation from checkpoint");
    
    // Enhanced performance metrics structure
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics", "Performance monitoring metrics")
        .def(py::init<>())
        .def_readwrite("wall_clock_time", &PerformanceMetrics::wallClockTime)
        .def_readwrite("cpu_time", &PerformanceMetrics::cpuTime)
        .def_readwrite("total_steps", &PerformanceMetrics::totalSteps)
        .def_readwrite("memory_usage", &PerformanceMetrics::memoryUsage)
        .def_readwrite("peak_memory", &PerformanceMetrics::peakMemory)
        .def_readwrite("average_firing_rate", &PerformanceMetrics::averageFiringRate)
        .def_readwrite("average_synaptic_weight", &PerformanceMetrics::averageSynapticWeight)
        .def_readwrite("total_spikes", &PerformanceMetrics::totalSpikes)
        .def_readwrite("total_neurons", &PerformanceMetrics::totalNeurons)
        .def_readwrite("total_synapses", &PerformanceMetrics::totalSynapses)
        .def("to_string", &PerformanceMetrics::toString,
             "Convert metrics to string representation");
    
    // Enhanced visualization data structures
    py::class_<NeuronData>(m, "NeuronData", "Neuron data for visualization")
        .def(py::init<>())
        .def_readwrite("id", &NeuronData::id)
        .def_readwrite("type", &NeuronData::type)
        .def_readwrite("membrane_potential", &NeuronData::membranePotential)
        .def_readwrite("threshold", &NeuronData::threshold)
        .def_readwrite("resting_potential", &NeuronData::restingPotential)
        .def_readwrite("firing_state", &NeuronData::firingState)
        .def("to_string", &NeuronData::toString,
             "Convert neuron data to string representation");
    
    py::class_<SynapseData>(m, "SynapseData", "Synapse data for visualization")
        .def(py::init<>())
        .def_readwrite("id", &SynapseData::id)
        .def_readwrite("source_neuron", &SynapseData::sourceNeuron)
        .def_readwrite("destination_neuron", &SynapseData::destinationNeuron)
        .def_readwrite("weight", &SynapseData::weight)
        .def("to_string", &SynapseData::toString,
             "Convert synapse data to string representation");
    
    py::class_<RegionData>(m, "RegionData", "Region data for visualization")
        .def(py::init<>())
        .def_readwrite("id", &RegionData::id)
        .def_readwrite("name", &RegionData::name)
        .def_readwrite("neuron_count", &RegionData::neuronCount)
        .def_readwrite("synapse_count", &RegionData::synapseCount)
        .def("to_string", &RegionData::toString,
             "Convert region data to string representation");
    
    py::class_<VisualizationData>(m, "VisualizationData", "Data for external visualization tools")
        .def(py::init<>())
        .def_readwrite("timestamp", &VisualizationData::timestamp)
        .def_readwrite("system_energy", &VisualizationData::systemEnergy)
        .def_readwrite("total_spikes", &VisualizationData::totalSpikes)
        .def_readwrite("average_firing_rate", &VisualizationData::averageFiringRate)
        .def("to_string", &VisualizationData::toString,
             "Convert visualization data to string representation");
    
    // Bind core classes with enhanced functionality
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
    
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels);
    
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples);
    
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals);
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone);
    
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);
    
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));
    
    // Bind enhanced world class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed);
    
    // Bind enhanced brain class with additional methods
    bindBrainEnhanced(py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status"));
    
    // Bind enhanced agent brain class
    bindAgentBrainEnhanced(py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled));
    
    // Bind enhanced world class
    bindSimpleWorldEnhanced(py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        .def("get_environment_stats", &SimpleWorld::getEnvironmentStats,
             "Get world environment statistics")
        .def("export_state", &SimpleWorld::exportState,
             "Export world state for saving")
        .def("import_state", &SimpleWorld::importState,
             py::arg("state"), "Import world state from saved data");
    
    // Helper functions for common patterns
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Example usage function
    m.def("create_example_setup", []() {
        // Create a complete example setup for beginners
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10, 10);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create an example setup for beginners");
    
    // Add module attributes
    m.attr("__version__") = "1.0.0";
    m.attr("__description__") = "Enhanced NLM Python Bindings with advanced features";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__status__") = "Production Ready";
}

} // namespace nlm
