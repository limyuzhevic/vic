#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include <string>
#include <vector>
#include <map>
#include <algorithm>

namespace py = pybind11;
namespace nlm {

// Convenience wrapper functions for easier Python usage

// Wrapper for running a complete episode simulation
void runEpisodeWrapper(AgentBrain& agent, SimpleWorld& world, size_t numSteps, bool verbose = false) {
    for (size_t step = 0; step < numSteps; ++step) {
        world.update(0.1);
        
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        // Note: This is a simplified wrapper - in real usage, you'd call brain.step()
        // but we don't have direct access to the brain from AgentBrain in this simple example
        
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        if (verbose && step % 100 == 0) {
            std::cout << "Step " << step << ": " << agent.getCuriosityLevel() 
                     << " curiosity, " << agent.getNoveltyLevel() << " novelty" 
                     << std::endl;
        }
    }
}

// Wrapper for getting comprehensive internal statistics
py::dict getInternalStatsWrapper(const Brain& brain) {
    py::dict stats;
    stats["totalNeurons"] = brain.getTotalNeuronCount();
    stats["totalSynapses"] = brain.getTotalSynapseCount();
    stats["firingNeurons"] = brain.getFiringNeuronCount();
    stats["totalSpikes"] = brain.getTotalSpikeCount();
    stats["avgFiringRate"] = brain.getAverageFiringRate();
    stats["eI_Ratio"] = brain.getExcitationInhibitionRatio();
    
    if (auto workingMemory = brain.getWorkingMemory()) {
        stats["workingMemoryNeurons"] = workingMemory->getMemoryNeurons().size();
        stats["workingMemoryActivations"] = workingMemory->getMemoryActivations();
    }
    
    if (auto episodicMemory = brain.getEpisodicMemory()) {
        stats["episodesStored"] = episodicMemory->getEpisodes().size();
    }
    
    return stats;
}

// Wrapper for getting performance metrics for agent evaluation
py::dict getPerformanceMetricsWrapper(const SimpleWorld& world, const AgentBrain& agent) {
    py::dict metrics;
    
    auto body = world.getAgentBody();
    metrics["agentPosition"] = py::make_tuple(body.x, body.y);
    metrics["agentOrientation"] = body.orientation;
    metrics["agentEnergy"] = body.energy;
    metrics["agentAge"] = body.age;
    
    metrics["curiosityLevel"] = agent.getCuriosityLevel();
    metrics["noveltyLevel"] = agent.getNoveltyLevel();
    metrics["predictionError"] = agent.getPredictionError();
    metrics["neuromodulationLevel"] = agent.getNeuromodulationLevel();
    
    metrics["developmentalStage"] = static_cast<int>(agent.getDevelopmentalStage());
    metrics["learnedPreference"] = agent.getLearnedPreference();
    
    // Get brain stats
    auto brain = agent.getBrain();
    metrics["brainStats"] = getInternalStatsWrapper(*brain);
    
    return metrics;
}

// Wrapper for exporting episode data to file
bool exportEpisodeDataWrapper(const SimpleWorld& world, const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "World Export" << std::endl;
    file << "=============" << std::endl;
    file << "Timestamp: " << world.getSimulationTime() << std::endl;
    file << "Width: " << world.getWidth() << std::endl;
    file << "Height: " << world.getHeight() << std::endl;
    file << "Agent Position: (" << world.getAgentBody().x << ", " << world.getAgentBody().y << ")" << std::endl;
    file << "Agent Orientation: " << world.getAgentBody().orientation << std::endl;
    file << "Agent Energy: " << world.getAgentBody().energy << std::endl;
    file << "Agent Health: " << world.getAgentBody().health << std::endl;
    
    // Export world objects
    file << "\nWorld Objects:" << std::endl;
    // Note: This would require access to internal world object list
    // For now, we'll just indicate this functionality
    
    file.close();
    return true;
}

// Set learning rate for plasticity (convenience method)
void setLearningRate(Brain& brain, float rate) {
    // This would need to access plasticity systems
    // For now, we'll provide a placeholder implementation
    auto stdp = brain.getSTDP();
    if (stdp) {
        // Apply rate scaling to STDP parameters
        float ltp = stdp->getLTPWeight() * rate;
        float ltd = stdp->getLTDWeight() * rate;
        stdp->configure(ltp, ltd, stdp->getTau());
    }
    
    auto hebbian = brain.getHebbian();
    if (hebbian) {
        // Apply rate scaling to Hebbian parameters
        float learningRate = hebbian->getLearningRate() * rate;
        hebbian->configure(learningRate);
    }
}

PYBIND11_MODULE(pynlm, m) {
    // Register the convenience functions
    m.def("runEpisode", &runEpisodeWrapper, py::arg("agent"), py::arg("world"), 
          py::arg("numSteps"), py::arg("verbose") = false,
          "Run complete episode simulation with automatic world updates");
    
    m.def("getInternalStats", &getInternalStatsWrapper,
          py::arg("brain"), "Get comprehensive internal statistics for monitoring");
    
    m.def("getPerformanceMetrics", &getPerformanceMetricsWrapper,
          py::arg("world"), py::arg("agent"), "Get performance metrics for agent evaluation");
    
    m.def("exportEpisodeData", &exportEpisodeDataWrapper,
          py::arg("world"), py::arg("filePath"), "Export episode data to file for analysis");
    
    m.def("setLearningRate", &setLearningRate,
          py::arg("brain"), py::arg("rate"), "Set learning rate for plasticity systems");
    
    // Enhanced SensorInput class with convenience methods
    py::class_<SensoryInput, std::shared_ptr<SensoryInput>>(m, "EnhancedSensoryInput", R"pbdoc(Enhanced sensory input with convenience methods)pbdoc")
        .def("getScalarValue", &SensoryInput::getData, "Get single scalar value from sensory input (for compatible inputs)")
        .def("getMaxValue", [](const SensoryInput& self) {
            auto data = self.getData();
            return data.empty() ? 0.0f : *std::max_element(data.begin(), data.end());
        }, "Get maximum value from sensory input")
        .def("getMinValue", [](const SensoryInput& self) {
            auto data = self.getData();
            return data.empty() ? 0.0f : *std::min_element(data.begin(), data.end());
        }, "Get minimum value from sensory input")
        .def("getAverageValue", [](const SensoryInput& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get average value from sensory input");
    
    // Enhanced Config class with validation and presets
    py::class_<Config, std::shared_ptr<Config>>(m, "ValidatedConfig", R"pbdoc(Validated configuration with preset profiles)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("validate", &validateConfig, py::arg("config"), "Validate configuration parameters")
        .def("applyPreset", &applyPreset, py::arg("config"), py::arg("preset"), "Apply a named preset profile")
        .def("getPresetNames", &getPresetNames, "Get available preset profile names");
}

// Helper functions for validation and presets
bool validateConfig(const Config& config) {
    // Basic validation - check for essential parameters
    bool valid = true;
    
    if (!config.has("brain.neuron_count") || config.get<int64_t>("brain.neuron_count") <= 0) {
        std::cerr << "Warning: neuron_count is not set or invalid" << std::endl;
        valid = false;
    }
    
    if (!config.has("brain.connection_probability") || 
        config.get<float>("brain.connection_probability") < 0.0f ||
        config.get<float>("brain.connection_probability") > 1.0f) {
        std::cerr << "Warning: connection_probability is invalid (should be 0.0-1.0)" << std::endl;
        valid = false;
    }
    
    return valid;
}

void applyPreset(Config& config, const std::string& presetName) {
    // Apply common presets
    if (presetName == "minimal") {
        config.set("brain.neuron_count", 100);
        config.set("brain.connection_probability", 0.05f);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
    }
    else if (presetName == "standard") {
        config.set("brain.neuron_count", 1000);
        config.set("brain.connection_probability", 0.1f);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
        config.set("plasticity.structural.enable", true);
        config.set("neuromod.dopamine.scale", 1.0f);
        config.set("neuromod.curiosity.enable", true);
        config.set("neuromod.novelty.enable", true);
    }
    else if (presetName == "high_performance") {
        config.set("brain.neuron_count", 5000);
        config.set("brain.connection_probability", 0.15f);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
        config.set("plasticity.structural.enable", true);
        config.set("neuromod.dopamine.scale", 2.0f);
        config.set("neuromod.curiosity.enable", true);
        config.set("neuromod.novelty.enable", true);
        config.set("performance.enable_vectorization", true);
    }
    else {
        throw std::runtime_error("Unknown preset: " + presetName);
    }
}

std::vector<std::string> getPresetNames() {
    return {"minimal", "standard", "high_performance"};
}

} // namespace nlm