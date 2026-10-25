// Improved Python bindings for NLM
// 
// This file provides a better organized and more user-friendly Python interface
// for the NLM neural simulation framework with convenience functions and helper classes
//
// The bindings are organized into logical modules:
// - Core types (NeuronId, SynapseId, etc.)
// - Core classes (Config, Brain, etc.)
// - Agent system
// - Sensory/Motor system
// - Visualization and utilities
// - Convenience functions and helpers

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"

namespace py = pybind11;
namespace nlm {

    // ============================================================================
    // Core Utility Functions and Helper Classes
    // ============================================================================

    // Configuration loading convenience functions
    std::shared_ptr<Config> load_config_from_string(const std::string& json_str);
    std::shared_ptr<Config> load_config_from_file(const std::string& filepath);

    // Brain creation helper functions
    std::shared_ptr<Brain> create_brain_with_config(const std::string& config_file = "configs/default.cfg");
    std::shared_ptr<Brain> create_standard_brain(size_t neuron_count = 500);

    // Simulation running convenience functions
    void run_learning_experiment(std::shared_ptr<Brain> brain, size_t steps = 1000);
    void run_stdp_test(std::shared_ptr<Brain> brain);
    void run_connectivity_test(std::shared_ptr<Brain> brain);

    // ============================================================================
    // Core Types and Enums
    // ============================================================================

    void export_core_types(py::module& m);

    void export_core_types(py::module& m) {
        // Neuron identifiers
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

        // Synapse identifiers
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

        // Region identifiers
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

        // Population identifiers
        py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
            .def(py::init<>())
            .def(py::init<uint64_t>(), py::arg("value"))
            .def_readwrite("value", &PopulationId::value)
            .def("index", &PopulationId::index)
            .def("__eq__", &PopulationId::operator==)
            .def("__ne__", &PopulationId::operator!=);

        // Neuron type enumeration
        py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
            .value("Excitatory", NeuronType::Excitatory)
            .value("Inhibitory", NeuronType::Inhibitory)
            .value("Modulatory", NeuronType::Modulatory)
            .value("Sensory", NeuronType::Sensory)
            .value("Motor", NeuronType::Motor)
            .value("Internal", NeuronType::Internal)
            .export_values();

        // Synapse type enumeration
        py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
            .value("Excitatory", SynapseType::Excitatory)
            .value("Inhibitory", SynapseType::Inhibitory)
            .value("Modulatory", SynapseType::Modulatory)
            .value("Electrical", SynapseType::Electrical)
            .value("GapJunction", SynapseType::GapJunction)
            .export_values();

        // Developmental stage enumeration
        py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
            .value("Initial", DevelopmentalStage::Initial)
            .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
            .value("Maturation", DevelopmentalStage::Maturation)
            .value("Adult", DevelopmentalStage::Adult)
            .value("Aging", DevelopmentalStage::Aging)
            .export_values();

        // Firing state enumeration
        py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
            .value("Resting", FiringState::Resting)
            .value("Active", FiringState::Active)
            .value("Refractory", FiringState::Refractory)
            .value("Inhibited", FiringState::Inhibited)
            .export_values();

        // Action type enumeration
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

        // Motor command enumeration
        py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
            .value("MoveForward", MotorCommand::MoveForward)
            .value("MoveBackward", MotorCommand::MoveBackward)
            .value("TurnLeft", MotorCommand::TurnLeft)
            .value("TurnRight", MotorCommand::TurnRight)
            .value("LookLeft", MotorCommand::LookLeft)
            .value("LookRight", MotorCommand::LookRight)
            .value("Interact", MotorCommand::Interact)
            .value("Wait", MotorCommand::Wait)
            .export_values();

        // World object type enumeration
        py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
            .value("Empty", WorldObjectType::Empty)
            .value("Resource", WorldObjectType::Resource)
            .value("Hazard", WorldObjectType::Hazard)
            .value("Wall", WorldObjectType::Wall)
            .value("Marker", WorldObjectType::Marker)
            .export_values();
    }

    // ============================================================================
    // Configuration System
    // ============================================================================

    void export_config_system(py::module& m);

    void export_config_system(py::module& m) {
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

        // Convenience functions for configuration
        m.def("load_config_from_string", &load_config_from_string,
              py::arg("json_str"), "Load configuration from JSON string",
              py::call_guard<py::gil_scoped_release>());

        m.def("load_config_from_file", &load_config_from_file,
              py::arg("filepath"), "Load configuration from file",
              py::call_guard<py::gil_scoped_release>());
    }

    // ============================================================================
    // Sensory System
    // ============================================================================

    void export_sensory_system(py::module& m);

    void export_sensory_system(py::module& m) {
        py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
            .def("getType", &SensoryInput::getType, "Get the type of sensory input")
            .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
            .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
            .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
            .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
                 "Set the timestamp");

        // Vision sensory input
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

        // Audio sensory input
        py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
            .def(py::init<>())
            .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
            .def("setData", [](Audio& self, const std::vector<float>& data) {
                self.setData(data);
            }, py::arg("data"))
            .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
            .def("getSampleRate", &Audio::getSampleRate)
            .def("getNumSamples", &Audio::getNumSamples);

        // Internal signals
        py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
            .def(py::init<>())
            .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
            .def("clearSignals", &InternalSignals::clearSignals);
    }

    // ============================================================================
    // Motor System
    // ============================================================================

    void export_motor_system(py::module& m);

    void export_motor_system(py::module& m) {
        py::class_<Action>(m, "Action", R"pbdoc(Motor action)pbdoc")
            .def(py::init<>())
            .def("getType", &Action::getType)
            .def("getValue", &Action::getValue)
            .def("getTimestamp", &Action::getTimestamp)
            .def("setType", &Action::setType, py::arg("type"))
            .def("setValue", &Action::setValue, py::arg("value"))
            .def("setTimestamp", &Action::setTimestamp, py::arg("timestamp"));
    }

    // ============================================================================
    // Agent System
    // ============================================================================

    void export_agent_system(py::module& m);

    void export_agent_system(py::module& m) {
        py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept for agent)pbdoc")
            .def(py::init<>())
            .def("getVision", &SensoryPercept::getVision)
            .def("getTouch", &SensoryPercept::getTouch)
            .def("getInternal", &SensoryPercept::getInternal)
            .def("getTimestamp", &SensoryPercept::getTimestamp)
            .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
            .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
            .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"));

        py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Brain with agent capabilities)pbdoc")
            .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
            .def("initialize", &AgentBrain::initialize, py::arg("world"),
                 "Initialize the agent brain with a world")
            .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
            .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
            .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
                 "Process sensory input and update internal state")
            .def("produceAction", &AgentBrain::produceAction,
                 "Produce an action based on current state")
            .def("getAction", &AgentBrain::getAction, py::arg("index"),
                 "Get a specific motor action")
            .def("getBrain", &AgentBrain::getBrain,
                 "Get the underlying NLM brain");

        py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Physical body for agent)pbdoc")
            .def(py::init<>())
            .def("getPosition", &AgentBody::getPosition)
            .def("getOrientation", &AgentBody::getOrientation)
            .def("setPosition", &AgentBody::setPosition, py::arg("position"))
            .def("setOrientation", &AgentBody::setOrientation, py::arg("orientation"))
            .def("getVisionWidth", &AgentBody::getVisionWidth)
            .def("getVisionHeight", &AgentBody::getVisionHeight)
            .def("getMaxSpeed", &AgentBody::getMaxSpeed)
            .def("setMaxSpeed", &AgentBody::setMaxSpeed, py::arg("speed"));
    }

    // ============================================================================
    // Environment and World
    // ============================================================================

    void export_environment_system(py::module& m);

    void export_environment_system(py::module& m) {
        py::class_<Environment>(m, "Environment", R"pbdoc(Environment simulation)pbdoc")
            .def(py::init<>())
            .def("reset", &Environment::reset,
                 "Reset the environment to initial state")
            .def("step", &Environment::step, py::arg("action"),
                 "Advance environment by one step given an action")
            .def("isDone", &Environment::isDone,
                 "Check if simulation is complete")
            .def("getLastReward", &Environment::getLastReward,
                 "Get the last reward received")
            .def("getLastObservation", &Environment::getLastObservation,
                 "Get the last observation");

        py::class_<Observation>(m, "Observation", R"pbdoc(Observation from environment)pbdoc")
            .def(py::init<>())
            .def("getVision", &Observation::getVision)
            .def("getTouch", &Observation::getTouch)
            .def("getInternal", &Observation::getInternal)
            .def("getReward", &Observation::getReward)
            .def("getTimestamp", &Observation::getTimestamp)
            .def("setVision", &Observation::setVision, py::arg("vision"))
            .def("setTouch", &Observation::setTouch, py::arg("touch"))
            .def("setInternal", &Observation::setInternal, py::arg("internal"))
            .def("setReward", &Observation::setReward, py::arg("reward"));

        py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple world simulation)pbdoc")
            .def(py::init<>())
            .def("reset", &SimpleWorld::reset,
                 "Reset the world to initial state")
            .def("step", &SimpleWorld::step, py::arg("action"),
                 "Advance world by one step")
            .def("isDone", &SimpleWorld::isDone,
                 "Check if world simulation is complete")
            .def("getVisionWidth", &SimpleWorld::getVisionWidth)
            .def("getVisionHeight", &SimpleWorld::getVisionHeight)
            .def("getResources", &SimpleWorld::getResources)
            .def("addResource", &SimpleWorld::addResource, py::arg("position"),
                 "Add a resource to the world")
            .def("getVision", &SimpleWorld::getVision, py::arg("agent_pos"),
                 "Get vision data for an agent at given position");
    }

    // ============================================================================
    // Main Brain Class
    // ============================================================================

    void export_brain_class(py::module& m);

    void export_brain_class(py::module& m) {
        py::class_<Brain>(m, "Brain", R"pbdoc(Neural Learning Machine brain)pbdoc")
            .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
            .def("initialize", &Brain::initialize,
                 "Initialize the brain with current configuration")
            .def("step", &Brain::step, py::arg("step"), py::arg("timestep"),
                 "Advance the brain by one simulation step")
            .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
                 "Get total number of neurons")
            .def("getTotalSynapseCount", &Brain::getSynapseCount,
                 "Get total number of synapses")
            .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
                 "Get number of currently firing neurons")
            .def("getAverageFiringRate", &Brain::getAverageFiringRate,
                 "Get average firing rate in Hz")
            .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
                 "Get total spike count")
            .def("getSpikeSystem", &Brain::getSpikeSystem,
                 "Get the spike system")
            .def("getRegion", &Brain::getRegion, py::arg("region_id"),
                 "Get a specific brain region")
            .def("getAllRegions", &Brain::getAllRegions,
                 "Get all brain regions")
            .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
                 py::arg("neuron_type"), py::arg("current"),
                 "Inject current into all neurons of a specific type")
            .def("produceAction", &Brain::produceAction,
                 "Produce an action based on current state")
            .def("receiveSensoryInput", &Brain::receiveSensoryInput,
                 py::arg("input"), "Receive sensory input")
            .def("reset", &Brain::reset,
                 "Reset brain to initial state")
            .def("logStatus", &Brain::logStatus,
                 "Log brain status and statistics")
            .def("__repr__", [](const Brain& brain) {
                return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) +
                       " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                       " synapses>";
            });
    }

    // ============================================================================
    // Convenience Functions and High-Level API
    // ============================================================================

    void export_convenience_functions(py::module& m);

    void export_convenience_functions(py::module& m) {
        // Brain creation helpers
        m.def("create_brain_with_config", &create_brain_with_config,
              py::arg("config_file") = "configs/default.cfg",
              "Create a brain with configuration from file",
              py::call_guard<py::gil_scoped_release>());

        m.def("create_standard_brain", &create_standard_brain,
              py::arg("neuron_count") = 500,
              "Create a standard brain with specified neuron count",
              py::call_guard<py::gil_scoped_release>());

        // Experiment running helpers
        m.def("run_learning_experiment", &run_learning_experiment,
              py::arg("brain"), py::arg("steps") = 1000,
              "Run a learning experiment on a brain",
              py::call_guard<py::gil_scoped_release>());

        m.def("run_stdp_test", &run_stdp_test,
              py::arg("brain"), "Run STDP test on a brain",
              py::call_guard<py::gil_scoped_release>());

        m.def("run_connectivity_test", &run_connectivity_test,
              py::arg("brain"), "Run connectivity test on a brain",
              py::call_guard<py::gil_scoped_release>());
    }

    // Implementation of convenience functions
    std::shared_ptr<Config> load_config_from_string(const std::string& json_str) {
        auto config = std::make_shared<Config>();
        
        try {
            auto json = nlohmann::json::parse(json_str);
            
            // Process JSON object recursively
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                if (it->is_string()) {
                    config->set(key, it->get<std::string>(), ConfigSource::File);
                } else if (it->is_number_integer()) {
                    config->set(key, it->get<int64_t>(), ConfigSource::File);
                } else if (it->is_number_float()) {
                    config->set(key, it->get<double>(), ConfigSource::File);
                } else if (it->is_boolean()) {
                    config->set(key, it->get<bool>(), ConfigSource::File);
                }
            }
            
            return config;
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to parse JSON: " + std::string(e.what()));
        }
    }

    std::shared_ptr<Config> load_config_from_file(const std::string& filepath) {
        return Config::loadFromFile(filepath) ? 
            std::make_shared<Config>() : nullptr;
    }

    std::shared_ptr<Brain> create_brain_with_config(const std::string& config_file) {
        auto config = std::make_shared<Config>();
        
        if (config->loadFromFile(config_file)) {
            return std::make_shared<Brain>(config);
        }
        
        return nullptr;
    }

    std::shared_ptr<Brain> create_standard_brain(size_t neuron_count) {
        auto config = std::make_shared<Config>();
        
        // Set standard configuration
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }

    void run_learning_experiment(std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        // Reset brain for experiment
        brain->reset();
        brain->initialize();
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run learning experience
        for (SimulationStep step = 0; step < steps; ++step) {
            // Inject sensory input
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
        }
    }

    void run_stdp_test(std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 5) {
            throw std::runtime_error("Not enough synapses for STDP test");
        }
        
        // Record initial weights
        auto& synapses = region->getSynapses();
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
        }
        
        // Run STDP test
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            for (int trial = 0; trial < 50; ++trial) {
                Neuron* preNeuron = neurons[0];
                Neuron* postNeuron = neurons[1];
                
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
    }

    void run_connectivity_test(std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            throw std::runtime_error("No region found");
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            throw std::runtime_error("No neurons found");
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into neurons
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        
        if (spikes == 0) {
            throw std::runtime_error("No spikes generated - connectivity test failed");
        }
    }

    // Main module initialization
    PYBIND11_MODULE(pynlm, m) {
        m.doc() = R"pbdoc(
            NLM (Neural Learning Machine) Python Bindings
            ---------------------------------------------
            A Python binding for the NLM C++ neural simulation framework.
            
            This module provides:
            - Core neural system classes (Brain, Config, etc.)
            - Agent system for embodied agents
            - Sensory and motor interfaces
            - Environment simulation capabilities
            - Convenience functions for common tasks
            - Benchmarking and testing utilities
            
            The API is designed to be intuitive and Pythonic, while maintaining
            full access to the C++ neural simulation capabilities.
            
            Example usage:
            
            ```python
            import pynlm
            
            # Create a standard brain
            brain = pynlm.create_standard_brain(500)
            
            # Run a learning experiment
            pynlm.run_learning_experiment(brain, 1000)
            
            # Create an agent
            agent = pynlm.AgentBrain(brain)
            
            # Run the agent in a world
            world = pynlm.SimpleWorld()
            agent.initialize(world)
            ```
        )pbdoc";

        // Register exception types
        py::register_exception<std::runtime_error>(m, "RuntimeError");

        // Export all components
        export_core_types(m);
        export_config_system(m);
        export_sensory_system(m);
        export_motor_system(m);
        export_brain_class(m);
        export_agent_system(m);
        export_environment_system(m);
        export_convenience_functions(m);
    }

} // namespace nlm