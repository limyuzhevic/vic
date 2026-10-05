#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
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

// Convenience functions and factory methods
namespace {
    // Default configuration presets
    std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        // Set biologically plausible default values
        config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
        config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
        config->set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
        config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
        config->set("brain.v_rest", -70.0f, ConfigSource::Default);
        config->set("brain.v_reset", -75.0f, ConfigSource::Default);
        config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
        config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
        
        // STDP parameters
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
        
        // Hebbian parameters
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", 0.0001f, ConfigSource::Default);
        
        // Structural plasticity
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("plasticity.structural.pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Neuromodulation
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        return config;
    }
    
    std::shared_ptr<Config> createExplorerConfig() {
        auto config = createDefaultConfig();
        // Higher exploration parameters for curiosity-driven learning
        config->set("brain.neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("neuromod.curiosity.enabled", true, ConfigSource::Default);
        config->set("neuromod.curiosity.exploration_rate", 0.3f, ConfigSource::Default);
        config->set("plasticity.structural.synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        return config;
    }
    
    std::shared_ptr<Config> createStorageConfig() {
        auto config = createDefaultConfig();
        // Higher memory parameters for episodic learning
        config->set("brain.neuron_count", static_cast<int64_t>(1500), ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", 0.0002f, ConfigSource::Default);
        config->set("neuromod.novelty.sensitivity", 0.8f, ConfigSource::Default);
        config->set("plasticity.structural.pruning_rate", 0.00005f, ConfigSource::Default);
        return config;
    }
    
    std::shared_ptr<Config> createSocialConfig() {
        auto config = createDefaultConfig();
        // Parameters optimized for social learning and imitation
        config->set("brain.neuron_count", static_cast<int64_t>(2500), ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.002f, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.5f, ConfigSource::Default);
        config->set("plasticity.structural.synaptogenesis_rate", 0.00015f, ConfigSource::Default);
        return config;
    }
    
    // Simulation convenience functions
    std::vector<float> runSimulationSteps(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, 
                                          int numSteps, bool verbose = false) {
        std::vector<float> firingRates;
        
        if (verbose) {
            std::cout << "Starting simulation with " << numSteps << " steps" << std::endl;
        }
        
        for (int step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get statistics
            firingRates.push_back(brain->getAverageFiringRate());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() << " firing neurons, "
                         << brain->getAverageFiringRate() << " Hz average rate" << std::endl;
            }
        }
        
        if (verbose) {
            std::cout << "Simulation completed. Final state: " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
        
        return firingRates;
    }
    
    // Brain statistics and diagnostics
    py::dict getBrainStatistics(std::shared_ptr<Brain> brain) {
        py::dict stats;
        
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["active_neuron_count"] = brain->getActiveNeuronCount();
        stats["firing_neuron_count"] = brain->getFiringNeuronCount();
        stats["total_spike_count"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Get region information
        auto regionIds = brain->getRegionIds();
        py::list regionStats;
        for (auto rid : regionIds) {
            py::dict regionInfo;
            auto* region = brain->getRegion(rid);
            if (region) {
                regionInfo["id"] = rid.value;
                regionInfo["neuron_count"] = region->getNeuronCount();
                regionInfo["synapse_count"] = region->getSynapseCount();
            }
            regionStats.append(regionInfo);
        }
        stats["regions"] = regionStats;
        
        return stats;
    }
    
    // Convenience helper for creating and initializing complete agent system
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> createAgentSystem(
        std::shared_ptr<Config> config = nullptr,
        int worldWidth = 20, int worldHeight = 20,
        int visionWidth = 8, int visionHeight = 8) {
        
        if (!config) {
            config = createDefaultConfig();
        }
        
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create and configure world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
        world->reset();
        world->setAgentStart(static_cast<float>(worldWidth) / 2.0f, static_cast<float>(worldHeight) / 2.0f);
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return std::make_tuple(brain, world, agent);
    }
    
    // Run complete agent simulation with learning
    py::dict runCompleteAgentSimulation(int numSteps = 1000, std::string preset = "default") {
        std::shared_ptr<Config> config;
        
        if (preset == "explorer") {
            config = createExplorerConfig();
        } else if (preset == "storage") {
            config = createStorageConfig();
        } else if (preset == "social") {
            config = createSocialConfig();
        } else {
            config = createDefaultConfig();
        }
        
        // Create agent system
        auto system = createAgentSystem(config, 30, 30, 12, 12);
        auto brain = std::get<0>(system);
        auto world = std::get<1>(system);
        auto agent = std::get<2>(system);
        
        // Enable learning subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run simulation
        auto firingRates = runSimulationSteps(brain, world, agent, numSteps, false);
        
        // Get final statistics
        auto stats = getBrainStatistics(brain);
        
        // Add simulation results
        stats["simulation_steps"] = numSteps;
        stats["firing_rate_mean"] = []() {
            float sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f);
            float mean = sum / firingRates.size();
            return mean;
        }();
        stats["firing_rate_std"] = []() {
            float sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f);
            float mean = sum / firingRates.size();
            float sqSum = 0.0f;
            for (float rate : firingRates) {
                sqSum += (rate - mean) * (rate - mean);
            }
            return std::sqrt(sqSum / firingRates.size());
        }();
        
        stats["learning_enabled"] = true;
        
        return stats;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Enhanced with convenience methods, configuration presets, and high-level APIs
        for easier and more powerful neural network simulations.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic types and enumerations (existing functionality)
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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

    // Configuration class enhancements
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

    // Convenience factory methods
    m.def("createDefaultConfig", &createDefaultConfig, R"pbdoc(
        Create a default configuration with biologically plausible parameters.
        
        Returns:
            Config: A default configuration object
    )pbdoc");

    m.def("createExplorerConfig", &createExplorerConfig, R"pbdoc(
        Create an explorer configuration optimized for curiosity-driven learning.
        
        Returns:
            Config: An explorer configuration object
    )pbdoc");

    m.def("createStorageConfig", &createStorageConfig, R"pbdoc(
        Create a storage configuration optimized for episodic memory.
        
        Returns:
            Config: A storage configuration object
    )pbdoc");

    m.def("createSocialConfig", &createSocialConfig, R"pbdoc(
        Create a social configuration optimized for social learning.
        
        Returns:
            Config: A social configuration object
    )pbdoc");

    // Enhanced brain creation with default config
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a brain with the specified configuration.
        
        Args:
            config: Configuration object for the brain
            
        Returns:
            Brain: A new brain object
    )pbdoc");

    m.def("createBrain", []() -> std::shared_ptr<Brain> {
        auto config = createDefaultConfig();
        return std::make_shared<Brain>(config);
    }, R"pbdoc(
        Create a brain with default configuration.
        
        Returns:
            Brain: A new brain object with default configuration
    )pbdoc");

    // Enhanced agent system creation
    m.def("createAgentSystem", &createAgentSystem, py::arg("config") = nullptr,
          py::arg("worldWidth") = 20, py::arg("worldHeight") = 20,
          py::arg("visionWidth") = 8, py::arg("visionHeight") = 8, R"pbdoc(
        Create and initialize a complete agent system (brain + world + agent).
        
        Args:
            config: Configuration object (optional, creates default if None)
            worldWidth: Width of the simulation world
            worldHeight: Height of the simulation world
            visionWidth: Width of agent's visual field
            visionHeight: Height of agent's visual field
            
        Returns:
            tuple: (brain, world, agent) - The three components of the agent system
    )pbdoc");

    // High-level simulation functions
    m.def("runSimulationSteps", &runSimulationSteps, py::arg("brain"), py::arg("world"), 
          py::arg("agent"), py::arg("numSteps"), py::arg("verbose") = false, R"pbdoc(
        Run a complete simulation with brain, world, and agent.
        
        Args:
            brain: The neural brain
            world: The simulation world
            agent: The agent interface
            numSteps: Number of simulation steps to run
            verbose: Whether to print progress information
            
        Returns:
            list: Average firing rates for each simulation step
    )pbdoc");

    m.def("getBrainStatistics", &getBrainStatistics, py::arg("brain"), R"pbdoc(
        Get comprehensive statistics and diagnostics for a brain.
        
        Args:
            brain: The brain to analyze
            
        Returns:
            dict: Dictionary containing brain statistics including neuron counts,
                  firing rates, developmental stage, and regional information
    )pbdoc");

    m.def("runCompleteAgentSimulation", &runCompleteAgentSimulation, py::arg("numSteps") = 1000,
          py::arg("preset") = "default", R"pbdoc(
        Run a complete agent simulation with learning enabled.
        
        This is a high-level convenience function that creates a complete agent system,
        enables learning subsystems, and runs a simulation.
        
        Args:
            numSteps: Number of simulation steps to run
            preset: Configuration preset ("default", "explorer", "storage", or "social")
            
        Returns:
            dict: Statistics from the simulation including firing rates,
                  learning metrics, and agent system statistics
    )pbdoc");

    // Existing class definitions (continuing from original bindings)
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
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
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
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
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class (existing functionality)
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Factory function for creating AgentBrain
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Version and module information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Neural Learning Machine - Enhanced Python bindings with convenience methods";

    // Type aliases for convenience
    py::class_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
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

    py::class_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::class_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Invalid constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
