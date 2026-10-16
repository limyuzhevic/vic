#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <algorithm>

// Include all the C++ headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/motor/Action.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/world/WorldObject.hpp"

// Include cognitive and memory systems
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/ActionConsequencePredictor.hpp"
#include "../src/prediction/PredictionErrorSignal.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/performance/Performance.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

// Python wrapper for complex nested types and functions
void addException(py::module_ &m) {
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
}

// Helper function to convert vector to numpy array (if available)
template<typename T>
py::array_t<T> vectorToArray(const std::vector<T>& vec) {
    return py::array_t<T>(vec.size(), vec.data());
}

// Helper function to convert array to vector
py::object pyArrayToVector(PyObject* obj) {
    py::array_t<float> arr = py::cast<py::array_t<float>>(obj);
    std::vector<float> vec(arr.size());
    std::memcpy(vec.data(), arr.data(), arr.size() * sizeof(float));
    return py::cast(vec);
}

// High-level cognitive functions
void addHighLevelFunctions(py::module_ &m) {
    // Brain creation and configuration
    m.def("createBrain", [](const std::string& configJson) {
        auto config = std::make_shared<Config>();
        if (!configJson.empty()) {
            config->loadFromFile(configJson);
        }
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }, py::arg("configJson") = "", "Create and initialize a brain with optional JSON configuration");

    m.def("createBrainWithConfig", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a brain with provided configuration");

    m.def("createWorld", [](float width, float height, size_t visionWidth, size_t visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 100.0f, py::arg("height") = 100.0f,
       py::arg("visionWidth") = 20, py::arg("visionHeight") = 20,
       "Create a 2D world with specified dimensions and vision settings");

    m.def("createAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"), "Create an agent brain connected to a world");

    // Simple experiment runner
    m.def("runExperiment", [](std::shared_ptr<Experiment> experiment,
                            std::shared_ptr<Brain> brain,
                            std::shared_ptr<SimpleWorld> world,
                            SimulationStep maxSteps) {
        auto runner = std::make_shared<ExperimentRunner>();
        return runner->runExperiment(experiment, brain, world, maxSteps);
    }, py::arg("experiment"), py::arg("brain"), py::arg("world"),
       py::arg("maxSteps") = 1000, "Run a complete experiment");

    // Brain simulation utilities
    m.def("simulateStep", [](std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time) {
        brain->step(step, time);
    }, py::arg("brain"), py::arg("step") = 0, py::arg("time") = 0.0,
       "Run a single simulation step");

    m.def("simulateEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                               SimulationStep maxSteps, Timestamp timestep) {
        Timestamp currentTime = 0.0;
        for (SimulationStep step = 0; step < maxSteps; ++step, currentTime += timestep) {
            brain->step(step, currentTime);
            if (world->getAgentBody().energy <= 0.0f || world->getAgentBody().health <= 0.0f) {
                break; // Agent died
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("maxSteps") = 1000,
       py::arg("timestep") = 0.1, "Run a complete simulation episode");

    // Data export utilities
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Export brain state to file");

    m.def("importBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Import brain state from file");

    // Monitoring utilities
    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["exc_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = brain->getDevelopmentalStage();
        return stats;
    }, py::arg("brain"), "Get brain statistics as a dictionary");

    m.def("getWorldStats", [](std::shared_ptr<SimpleWorld> world) {
        py::dict stats;
        stats["width"] = world->getWidth();
        stats["height"] = world->getHeight();
        stats["agent_x"] = world->getAgentBody().x;
        stats["agent_y"] = world->getAgentBody().y;
        stats["agent_energy"] = world->getAgentBody().energy;
        stats["agent_health"] = world->getAgentBody().health;
        stats["agent_age"] = world->getAgentBody().age;
        stats["simulation_time"] = world->getSimulationTime();
        return stats;
    }, py::arg("world"), "Get world state as a dictionary");
}

// Visualization utilities
void addVisualizationFunctions(py::module_ &m) {
    // Note: VisualizationInterface would need to be implemented
    // For now, providing placeholder functions that users can extend
    
    m.def("createVisualizer", []() {
        // Return a placeholder for visualization interface
        return py::none();
    }, "Create a visualization interface");

    m.def("plotBrainActivity", [](std::shared_ptr<Brain> brain) {
        // Placeholder for plotting brain activity
        return "Visualization not implemented yet";
    }, py::arg("brain"), "Plot brain activity visualization");

    m.def("plotLearningCurve", [](std::shared_ptr<Experiment> experiment) {
        // Placeholder for plotting learning curves
        return "Visualization not implemented yet";
    }, py::arg("experiment"), "Plot learning curve from experiment");

    m.def("exportResults", [](std::shared_ptr<Experiment> experiment, const std::string& format) {
        // Export experiment results in various formats
        return "Export not implemented yet";
    }, py::arg("experiment"), py::arg("format") = "json", "Export experiment results");
}

// Helper functions for common experiments
void addHelperFunctions(py::module_ &m) {
    // Exploration-exploitation task
    m.def("createExplorationTask", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                     float resourceDensity) {
        // Place resources in world
        for (float x = 0; x < world->getWidth(); x += 10.0f) {
            for (float y = 0; y < world->getHeight(); y += 10.0f) {
                if ((rand() % 100) < resourceDensity * 100) {
                    world->addObject(WorldObject(x, y, WorldObjectType::Resource, 10.0f));
                }
            }
        }
        world->setAgentStart(world->getWidth() / 2, world->getHeight() / 2);
    }, py::arg("brain"), py::arg("world"),
       py::arg("resourceDensity") = 0.3, "Create an exploration task with resources");

    // Navigation task
    m.def("createNavigationTask", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                    float obstacleDensity) {
        // Place obstacles in world
        for (float x = 0; x < world->getWidth(); x += 10.0f) {
            for (float y = 0; y < world->getHeight(); y += 10.0f) {
                if ((rand() % 100) < obstacleDensity * 100 &&
                    x > 20 && x < world->getWidth() - 20 &&
                    y > 20 && y < world->getHeight() - 20) {
                    world->addObject(WorldObject(x, y, WorldObjectType::Wall));
                }
            }
        }
        world->setAgentStart(world->getWidth() / 4, world->getHeight() / 4);
    }, py::arg("brain"), py::arg("world"),
       py::arg("obstacleDensity") = 0.2, "Create a navigation task with obstacles");

    // Monitoring utilities
    m.def("recordMetrics", [](std::shared_ptr<Experiment> experiment,
                           std::shared_ptr<Brain> brain,
                           std::shared_ptr<SimpleWorld> world,
                           SimulationStep step) {
        experiment->recordStep(step,
                              brain->getTotalNeuronCount(),
                              brain->getTotalSynapseCount(),
                              brain->getFiringNeuronCount(),
                              brain->getAverageFiringRate());
    }, py::arg("experiment"), py::arg("brain"), py::arg("world"),
       py::arg("step") = 0, "Record simulation metrics");

    // State management
    m.def("resetAll", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        brain->reset();
        world->reset();
    }, py::arg("brain"), py::arg("world"), "Reset brain and world to initial state");

    // Reward shaping
    m.def("addRewardForAction", [](std::shared_ptr<Experiment> experiment,
                                 ActionType action, float reward) {
        experiment->recordMetric(std::string("reward_") + std::to_string(static_cast<int>(action)), reward);
    }, py::arg("experiment"), py::arg("action"), py::arg("reward"),
       "Add a reward for a specific action");
}

// Cognitive system getters (with defaults for missing systems)
void addCognitiveSystems(py::module_ &m) {
    // Since some cognitive systems may not be implemented yet,
    // we provide fallback implementations

    m.def("getNeuralWorkingMemory", [](std::shared_ptr<Brain> brain) {
        // Access working memory through brain if available
        auto workingMemory = brain->getWorkingMemory();
        if (!workingMemory) {
            throw std::runtime_error("Working memory not available in this brain");
        }
        return workingMemory;
    }, py::arg("brain"), "Get neural working memory system");

    m.def("getPredictionSystem", [](std::shared_ptr<Brain> brain) {
        // Access prediction system through brain if available
        auto predictionSystem = brain->getPredictionSystem();
        if (!predictionSystem) {
            throw std::runtime_error("Prediction system not available in this brain");
        }
        return predictionSystem;
    }, py::arg("brain"), "Get prediction system");

    m.def("getPlanner", [](std::shared_ptr<Brain> brain) {
        // Access planner through brain if available
        auto planner = brain->getPlanner();
        if (!planner) {
            throw std::runtime_error("Planner not available in this brain");
        }
        return planner;
    }, py::arg("brain"), "Get neural planner");

    m.def("getConceptFormation", [](std::shared_ptr<Brain> brain) {
        // Access concept formation through brain if available
        auto conceptFormation = brain->getConceptFormation();
        if (!conceptFormation) {
            throw std::runtime_error("Concept formation not available in this brain");
        }
        return conceptFormation;
    }, py::arg("brain"), "Get concept formation system");

    m.def("getAttention", [](std::shared_ptr<Brain> brain) {
        // Access attentional selection through brain if available
        auto attention = brain->getAttention();
        if (!attention) {
            throw std::runtime_error("Attentional selection not available in this brain");
        }
        return attention;
    }, py::arg("brain"), "Get attentional selection system");

    m.def("getDevelopmentSystem", [](std::shared_ptr<Brain> brain) {
        // Access development system through brain if available
        auto developmentSystem = brain->getDevelopmentSystem();
        if (!developmentSystem) {
            throw std::runtime_error("Development system not available in this brain");
        }
        return developmentSystem;
    }, py::arg("brain"), "Get development system");

    m.def("getDopamine", [](std::shared_ptr<Brain> brain) {
        // Access dopamine system through brain if available
        auto dopamine = brain->getDopamine();
        if (!dopamine) {
            throw std::runtime_error("Dopamine system not available in this brain");
        }
        return dopamine;
    }, py::arg("brain"), "Get dopamine neuromodulation system");

    m.def("getCuriosity", [](std::shared_ptr<Brain> brain) {
        // Access curiosity system through brain if available
        auto curiosity = brain->getCuriosity();
        if (!curiosity) {
            throw std::runtime_error("Curiosity system not available in this brain");
        }
        return curiosity;
    }, py::arg("brain"), "Get curiosity neuromodulation system");

    m.def("getNovelty", [](std::shared_ptr<Brain> brain) {
        // Access novelty detection through brain if available
        auto novelty = brain->getNovelty();
        if (!novelty) {
            throw std::runtime_error("Novelty detection system not available in this brain");
        }
        return novelty;
    }, py::arg("brain"), "Get novelty detection system");

    m.def("getPredictionErrorSignal", [](std::shared_ptr<Brain> brain) {
        // Access prediction error signal through brain if available
        auto predictionError = brain->getPredictionErrorSignal();
        if (!predictionError) {
            throw std::runtime_error("Prediction error signal not available in this brain");
        }
        return predictionError;
    }, py::arg("brain"), "Get prediction error signal system");
}

// Main bindings module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A comprehensive Python API for the NLM C++ neural simulation framework.
        
        This module provides:
        - Low-level bindings for core NLM classes (Brain, Config, SensoryInput, etc.)
        - High-level cognitive function wrappers for easy experiment setup
        - Helper functions for common experiment types
        - Monitoring and visualization utilities
        - Experiment management tools
        
        The API is designed to be both low-level (for fine control) and high-level
        (for easy prototyping and research), making it accessible to both
        developers and researchers.
    )pbdoc";

    // Add exceptions
    addException(m);

    // Add low-level bindings (original bindings but with improved signatures)
    
    // NeuronId, SynapseId, RegionId, PopulationId
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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class
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

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", &SensoryInput::clone, "Create a copy of this sensory input");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", &Vision::setData, py::arg("data"), "Set image data")
        .def("setData", [](Vision& self, py::array_t<float> data) {
            std::vector<float> vec(data.size());
            std::memcpy(vec.data(), data.data(), vec.size() * sizeof(float));
            self.setData(vec);
        }, py::arg("data"), "Set image data from numpy array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getType", &Vision::getType)
        .def("getDimensions", &Vision::getDimensions);

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", &Audio::setData, py::arg("data"), "Set audio data")
        .def("setData", [](Audio& self, py::array_t<float> data) {
            std::vector<float> vec(data.size());
            std::memcpy(vec.data(), data.data(), vec.size() * sizeof(float));
            self.setData(vec);
        }, py::arg("data"), "Set audio data from numpy array")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getType", &Audio::getType)
        .def("getDimensions", &Audio::getDimensions);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getSignals", [](InternalSignals& self) {
            return self.getData();
        }, "Get all signals as vector")
        .def("getNumSignals", [](InternalSignals& self) {
            return self.getData().size();
        }, "Get number of signals");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& a) {
            return "<Action: " + std::to_string(static_cast<int>(a.getType())) + ">";
        });

    // WorldObject class
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
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: x=" + std::to_string(obj.x) + ", y=" + std::to_string(obj.y) +
                   ", type=" + std::to_string(static_cast<int>(obj.type)) + ">";
        });

    // AgentBody class
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
        .def("reset", &AgentBody::reset)
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: x=" + std::to_string(body.x) + ", y=" + std::to_string(body.y) +
                   ", energy=" + std::to_string(body.energy) + ">";
        });

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision settings")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class
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

    // AgentBrain class
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

    // High-level cognitive functions
    addHighLevelFunctions(m);

    // Helper functions
    addHelperFunctions(m);

    // Visualization functions
    addVisualizationFunctions(m);

    // Cognitive systems
    addCognitiveSystems(m);

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Function to get version
    m.def("getVersion", []() { return "1.0.0"; }, "Get NLM Python bindings version");

    // Documentation
    m.def("getDocumentation", []() {
        return R"doc(
        NLM Python API Documentation:
        
        Key Classes:
        - Brain: Main neural simulation engine
        - Config: Configuration manager
        - SimpleWorld: 2D simulation environment
        - AgentBrain: Connects brain to world
        - Vision, Audio, InternalSignals: Sensory inputs
        - Action: Motor outputs
        - AgentBody: Agent physical state
        
        Key Functions:
        - createBrain(configJson): Create and initialize brain
        - createBrainWithConfig(config): Create brain with config object
        - createWorld(width, height, visionWidth, visionHeight): Create simulation world
        - createAgent(brain, world): Create agent interface
        - simulateStep(brain, step, time): Run simulation step
        - simulateEpisode(brain, world, maxSteps, timestep): Run complete episode
        
        Helper Functions:
        - createExplorationTask(brain, world, resourceDensity): Setup exploration task
        - createNavigationTask(brain, world, obstacleDensity): Setup navigation task
        - getBrainStats(brain): Get brain statistics
        - getWorldStats(world): Get world state
        
        For more information, visit: https://github.com/nlm/NLM
        )doc";
    }, "Get documentation string");

} // namespace nlm
