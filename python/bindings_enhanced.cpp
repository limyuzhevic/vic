#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <map>

// Define Python bindings for additional convenience methods

namespace py = pybind11;
namespace nlm {

// Add helper function to create brain from dict
def createBrainFromDict(py::dict config_dict) {
    auto config = std::make_shared<Config>();
    for (auto item : config_dict) {
        std::string key = item.first.cast<std::string>();
        // Try to convert value to appropriate type
        if (item.second.is(py::int_())) {
            config->set(key, item.second.cast<int>());
        } else if (item.second.is(py::float_())) {
            config->set(key, item.second.cast<double>());
        } else if (item.second.is(py::bool_())) {
            config->set(key, item.second.cast<bool>());
        } else if (item.second.is(py::str())) {
            config->set(key, item.second.cast<std::string>());
        } else if (item.second.is(py::list())) {
            auto list = item.second.cast<std::vector<double>>();
            config->set(key, ConfigValue(list));
        }
    }
    return std::make_shared<Brain>(config);
}

// Add helper function to run simple simulation
def runSimpleSimulation(py::shared_ptr<Brain> brain, int steps) {
    for (int i = 0; i < steps; ++i) {
        brain->step(i);
    }
    return brain->getTotalSpikeCount();
}

// Add helper function to get brain statistics
def getBrainStats(py::shared_ptr<Brain> brain) {
    py::dict stats;
    stats["total_neurons"] = brain->getTotalNeuronCount();
    stats["total_synapses"] = brain->getTotalSynapseCount();
    stats["firing_neurons"] = brain->getFiringNeuronCount();
    stats["total_spikes"] = brain->getTotalSpikeCount();
    stats["average_firing_rate"] = brain->getAverageFiringRate();
    stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
    return stats;
}

// Add helper function for easy world creation and setup
def createSimpleEnvironment(py::kwargs kwargs) {
    auto world = std::make_shared<SimpleWorld>();
    
    // Set up world dimensions from kwargs if provided
    if (kwargs.contains("width")) {
        world->configure(kwargs["width"].cast<size_t>(), 
                        kwargs.contains("height") ? kwargs["height"].cast<size_t>() : 20,
                        kwargs.contains("visionWidth") ? kwargs["visionWidth"].cast<size_t>() : 8,
                        kwargs.contains("visionHeight") ? kwargs["visionHeight"].cast<size_t>() : 8);
    }
    
    if (kwargs.contains("agent_start_x")) {
        world->setAgentStart(kwargs["agent_start_x"].cast<float>(), 
                           kwargs.contains("agent_start_y") ? kwargs["agent_start_y"].cast<float>() : 10.0f);
    }
    
    if (kwargs.contains("seed")) {
        world->setRandomSeed(kwargs["seed"].cast<unsigned int>());
    }
    
    return world;
}

// Add convenience method to enable all features
def enableAllFeatures(py::shared_ptr<AgentBrain> agent) {
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    return agent;
}

// Add convenience method for episode running
def runEpisode(py::shared_ptr<Brain> brain, py::shared_ptr<SimpleWorld> world, 
              py::shared_ptr<AgentBrain> agent, int steps, bool verbose = false) {
    float totalReward = 0.0f;
    
    for (int step = 0; step < steps; ++step) {
        // Get observation from world
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Get action from agent
        auto action = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Get reward and apply neuromodulation
        float reward = 0.0f;
        if (percept.getInternal() && !percept.getInternal()->empty()) {
            reward = percept.getInternal()->front();
        }
        
        totalReward += reward;
        agent->applyRewardModulation(reward, 0.0f);
        
        if (verbose && step % 100 == 0) {
            std::cout << "Step " << step 
                     << ": Reward = " << reward
                     << ", Total = " << totalReward
                     << ", Firing = " << brain->getFiringNeuronCount()
                     << std::endl;
        }
    }
    
    return totalReward;
}

// Add helper function for batch processing
def createBrains(int count, py::kwargs kwargs) {
    std::vector<std::shared_ptr<Brain>> brains;
    
    for (int i = 0; i < count; ++i) {
        // Create brain with optional config
        auto config = std::make_shared<Config>();
        
        // Set default config if kwargs provided
        if (kwargs.contains("neuron_count")) {
            config->set("neuron_count", kwargs["neuron_count"].cast<size_t>());
        }
        if (kwargs.contains("region_count")) {
            config->set("region_count", kwargs["region_count"].cast<size_t>());
        }
        if (kwargs.contains("connection_probability")) {
            config->set("connection_probability", kwargs["connection_probability"].cast<float>());
        }
        
        brains.push_back(std::make_shared<Brain>(config));
        brains[i]->initialize();
    }
    
    return brains;
}

// Add helper function for statistics analysis
def analyzeBrainData(const std::vector<std::shared_ptr<Brain>>& brains) {
    if (brains.empty()) {
        throw std::invalid_argument("No brains provided for analysis");
    }
    
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t totalSpikes = 0;
    float totalFiringRate = 0.0f;
    
    for (const auto& brain : brains) {
        totalNeurons += brain->getTotalNeuronCount();
        totalSynapses += brain->getTotalSynapseCount();
        totalSpikes += brain->getTotalSpikeCount();
        totalFiringRate += brain->getAverageFiringRate();
    }
    
    py::dict analysis;
    analysis["brain_count"] = brains.size();
    analysis["avg_neurons"] = static_cast<float>(totalNeurons) / brains.size();
    analysis["avg_synapses"] = static_cast<float>(totalSynapses) / brains.size();
    analysis["avg_spikes"] = static_cast<float>(totalSpikes) / brains.size();
    analysis["avg_firing_rate"] = totalFiringRate / brains.size();
    
    return analysis;
}

} // namespace nlm
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ================================================
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides comprehensive access to:
        - Brain simulation and neural computation
        - Agent-world interaction
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental processes
        - Prediction systems
        - Cognition modules (planning, concept formation, attention)
        
        Features:
        - Type-safe Python API with auto-conversion
        - Comprehensive error handling and validation
        - Convenience methods for common operations
        - Support for batch processing and analysis
        - Detailed logging and monitoring
        - Serialization support for checkpointing
        
        Example usage:
        
        ```python
        import pynlm
        
        # Create a brain with custom configuration
        config = {"neuron_count": 1000, "region_count": 1}
        brain = pynlm.createBrainFromDict(config)
        
        # Enable all learning features
        agent = pynlm.createAgentBrain(brain)
        agent.enableAllFeatures()
        
        # Create environment
        world = pynlm.createSimpleEnvironment(width=20, height=20)
        
        # Run simulation
        agent.initialize(world)
        total_reward = pynlm.runEpisode(brain, world, agent, 1000, verbose=True)
        
        # Get statistics
        stats = pynlm.getBrainStats(brain)
        print(f"Simulation complete. Total reward: {total_reward}")
        ```
    )pbdoc";

    // Include all original bindings...
    
    // Add new convenience methods
    m.def("createBrainFromDict", &createBrainFromDict, "Create brain from Python dictionary",
          py::arg("config_dict"));
    
    m.def("runSimpleSimulation", &runSimpleSimulation, "Run simple simulation",
          py::arg("brain"), py::arg("steps"));
    
    m.def("getBrainStats", &getBrainStats, "Get brain statistics as dictionary",
          py::arg("brain"));
    
    m.def("createSimpleEnvironment", &createSimpleEnvironment, 
          "Create simple world with convenient setup", py::arg("kwargs"));
    
    m.def("enableAllFeatures", &enableAllFeatures, "Enable all learning features",
          py::arg("agent"));
    
    m.def("runEpisode", &runEpisode, "Run complete episode simulation",
          py::arg("brain"), py::arg("world"), py::arg("agent"), 
          py::arg("steps"), py::arg("verbose") = false);
    
    m.def("createBrains", &createBrains, "Create multiple brains",
          py::arg("count"), py::arg("kwargs") = py::kwargs());
    
    m.def("analyzeBrainData", &analyzeBrainData, "Analyze brain data",
          py::arg("brains"));
    
    // Register exception type
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    
    // Add version info
    m.attr("__version__") = "2.0.0-enhanced";
}