#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

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

// Helper functions for Python bindings
namespace python {

    // Convert NeuronId to string representation
    std::string neuronIdToString(const NeuronId& id) {
        return "<NeuronId: " + std::to_string(id.value) + ">";
    }
    
    // Convert SynapseId to string representation
    std::string synapseIdToString(const SynapseId& id) {
        return "<SynapseId: " + std::to_string(id.value) + ">";
    }
    
    // Convert RegionId to string representation
    std::string regionIdToString(const RegionId& id) {
        return "<RegionId: " + std::to_string(id.value) + ">";
    }
    
    // Format neuron statistics
    std::string formatNeuronStats(const class Brain& brain, const class NeuralRegion* region) {
        if (!region) return "Invalid region";
        std::ostringstream oss;
        oss << "Region " << region->getId().index() << " (" << region->getName() << "):";
        oss << "\n  Neurons: " << region->getTotalNeuronCount();
        oss << "\n  Populations: " << region->getPopulationCount();
        oss << "\n  Synapses: " << region->getSynapseCount();
        oss << "\n  Firing rate: " << std::fixed << std::setprecision(3) << region->getAverageFiringRate();
        oss << " Hz\n  Avg synaptic weight: " << region->getAverageSynapticWeight();
        return oss.str();
    }
    
    // Get top firing neurons from a region
    std::vector<NeuronId> getTopFiringNeurons(const class NeuralRegion* region, size_t count) {
        std::vector<NeuronId> result;
        if (!region) return result;
        
        auto neurons = region->getAllNeurons();
        std::vector<std::pair<float, NeuronId>> neuronActivity;
        
        for (Neuron* neuron : neurons) {
            float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            neuronActivity.emplace_back(activity, neuron->getId());
        }
        
        std::partial_sort(neuronActivity.begin(),
                         neuronActivity.begin() + std::min(count, neuronActivity.size()),
                         neuronActivity.end(),
                         [](const auto& a, const auto& b) { return a.first > b.first; });
        
        for (size_t i = 0; i < std::min(count, neuronActivity.size()); ++i) {
            result.push_back(neuronActivity[i].second);
        }
        
        return result;
    }
    
    // Create map of region statistics
    py::dict getRegionStatistics(const class Brain& brain) {
        py::dict result;
        for (const auto& region : brain.getRegions()) {
            py::dict regionInfo;
            regionInfo["id"] = region->getId().index();
            regionInfo["name"] = region->getName();
            regionInfo["neuron_count"] = region->getTotalNeuronCount();
            regionInfo["population_count"] = region->getPopulationCount();
            regionInfo["synapse_count"] = region->getSynapseCount();
            regionInfo["firing_rate_hz"] = std::fixed << std::setprecision(3) << region->getAverageFiringRate();
            regionInfo["avg_synaptic_weight"] = region->getAverageSynapticWeight();
            
            result[region->getId().index()] = regionInfo;
        }
        return result;
    }
    
    // Get memory system statistics
    py::dict getMemorySystemStats(const class Brain& brain) {
        py::dict stats;
        
        auto* workingMem = brain.getWorkingMemory();
        if (workingMem) {
            stats["working_memory"] = py::dict({
                {"active_traces", workingMem->getActiveTraces()},
                {"capacity", workingMem->getCapacity()},
                {"current_neuron_count", workingMem->getActiveNeuronCount()}
            });
        }
        
        auto* episodicMem = brain.getEpisodicMemory();
        if (episodicMem) {
            stats["episodic_memory"] = py::dict({
                {"episode_count", episodicMem->getEpisodeCount()},
                {"max_episodes", episodicMem->getMaxEpisodes()},
                {"replay_enabled", episodicMem->isReplayEnabled()}
            });
        }
        
        auto* assocMem = brain.getAssociativeMemory();
        if (assocMem) {
            stats["associative_memory"] = py::dict({
                {"pattern_count", assocMem->getPatternCount()},
                {"max_patterns", assocMem->getMaxPatterns()},
                {"activation_threshold", assocMem->getActivationThreshold()}
            });
        }
        
        return stats;
    }
    
    // Get neuromodulation system state
    py::dict getNeuromodulationState(const class Brain& brain) {
        py::dict state;
        
        auto* dopamine = brain.getDopamine();
        if (dopamine) {
            state["dopamine"] = py::dict({
                {"level", dopamine->getLevel()},
                {"plasticity_factor", dopamine->getPlasticityFactor()},
                {"name", dopamine->getName()}
            });
        }
        
        auto* curiosity = brain.getCuriosity();
        if (curiosity) {
            state["curiosity"] = py::dict({
                {"level", curiosity->getLevel()},
                {"exploration_drive", curiosity->getExplorationDrive()}
            });
        }
        
        auto* novelty = brain.getNovelty();
        if (novelty) {
            state["novelty"] = py::dict({
                {"level", novelty->getLevel()}
            });
        }
        
        auto* predError = brain.getPredictionErrorSignal();
        if (predError) {
            state["prediction_error"] = py::dict({
                {"level", predError->getLevel()}
            });
        }
        
        return state;
    }
    
    // Get plasticity system state
    py::dict getPlasticityState(const class Brain& brain) {
        py::dict state;
        
        auto* stdp = brain.getSTDP();
        if (stdp) {
            state["stdp"] = py::dict({
                {"enabled", stdp->isEnabled()},
                {"learning_rate", stdp->getLTPWeight()},
                {"ltd_weight", stdp->getLTDWeight()},
                {"time_constant", stdp->getTau()}
            });
        }
        
        auto* hebbian = brain.getHebbian();
        if (hebbian) {
            state["hebbian"] = py::dict({
                {"enabled", hebbian->isEnabled()},
                {"learning_rate", hebbian->getLearningRate()}
            });
        }
        
        auto* structural = brain.getStructuralPlasticity();
        if (structural) {
            state["structural"] = py::dict({
                {"enabled", structural->isEnabled()},
                {"synaptogenesis_rate", structural->getSynaptogenesisRate()},
                {"pruning_rate", structural->getPruningRate()}
            });
        }
        
        return state;
    }
    
    // Create action from dictionary
    std::unique_ptr<Action> actionFromDict(const py::dict& action_dict) {
        std::unique_ptr<Action> action;
        
        if (action_dict.contains("type")) {
            ActionType type = action_dict["type"].cast<ActionType>();
            action = std::make_unique<Action>(type);
            
            if (action_dict.contains("parameters")) {
                std::vector<float> params = action_dict["parameters"].cast<std::vector<float>>();
                action->setParameters(params);
            }
        }
        
        return action;
    }
    
    // Get action as dictionary
    py::dict actionToDict(const Action* action) {
        py::dict result;
        if (action) {
            result["type"] = action->getType();
            result["name"] = action->getName();
            result["parameters"] = action->getParameters();
        }
        return result;
    }
    
    // Get sensory percept as dictionary
    py::dict perceptToDict(const SensoryPercept* percept) {
        py::dict result;
        if (percept) {
            result["vision_width"] = percept->getVisionWidth();
            result["vision_height"] = percept->getVisionHeight();
            
            // Get vision data if available
            auto vision = percept->getVision();
            if (!vision.empty()) {
                result["vision"] = std::vector<float>(vision.begin(), vision.end());
            }
            
            // Get touch data if available
            auto touch = percept->getTouch();
            if (!touch.empty()) {
                result["touch"] = std::vector<float>(touch.begin(), touch.end());
            }
            
            // Get internal signals if available
            auto internal = percept->getInternal();
            if (!internal.empty()) {
                result["internal"] = std::vector<float>(internal.begin(), internal.end());
            }
            
            // Get proprioception if available
            auto proprio = percept->getProprioception();
            if (!proprio.empty()) {
                result["proprioception"] = std::vector<float>(proprio.begin(), proprio.end());
            }
            
            // Get audio if available
            auto audio = percept->getAudio();
            if (!audio.empty()) {
                result["audio"] = std::vector<float>(audio.begin(), audio.end());
            }
            
            result["timestamp"] = percept->getTimestamp();
        }
        return result;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Complete brain simulation with neural computation
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Plasticity systems (STDP, Hebbian, structural plasticity)
        - Cognitive systems (planning, concept formation, attention)
        - Development and learning capabilities
        - Visualization and monitoring tools
        - Checkpoint saving/loading
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId type
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

    // SynapseId type
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

    // RegionId type
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

    // PopulationId type
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration
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

    // MotorCommand enumeration
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

    // WorldObjectType enumeration
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
        })
        // Python-specific methods
        .def("get", [](Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"), "Get configuration value as string")
        .def("get_int", [](Config& self, const std::string& key) {
            auto val = self.get<int64_t>(key);
            return val.has_value() ? py::cast(val.value()) : py::none();
        }, py::arg("key"), "Get configuration value as int")
        .def("get_float", [](Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            return val.has_value() ? py::cast(val.value()) : py::none();
        }, py::arg("key"), "Get configuration value as float")
        .def("get_bool", [](Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            return val.has_value() ? py::cast(val.value()) : py::none();
        }, py::arg("key"), "Get configuration value as bool")
        .def("set_string", &Config::set, py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("set_int", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value");

    // SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
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

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

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
        .def("clone", &Action::clone);

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
        .def_readwrite("active", &WorldObject::active);

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
        .def("reset", &AgentBody::reset);

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("to_dict", [](const SensoryPercept& percept) {
            return python::perceptToDict(&percept);
        }, "Convert percept to Python dictionary")
        .def_static("from_dict", [](const py::dict& data) {
            SensoryPercept percept;
            
            if (data.contains("vision")) {
                std::vector<float> vision = data["vision"].cast<std::vector<float>>();
                percept.setVision(vision);
            }
            
            if (data.contains("touch")) {
                std::vector<float> touch = data["touch"].cast<std::vector<float>>();
                percept.setTouch(touch);
            }
            
            if (data.contains("internal")) {
                std::vector<float> internal = data["internal"].cast<std::vector<float>>();
                percept.setInternal(internal);
            }
            
            if (data.contains("proprioception")) {
                std::vector<float> proprio = data["proprioception"].cast<std::vector<float>>();
                percept.setProprioception(proprio);
            }
            
            if (data.contains("audio")) {
                std::vector<float> audio = data["audio"].cast<std::vector<float>>();
                percept.setAudio(audio);
            }
            
            if (data.contains("timestamp")) {
                percept.setTimestamp(data["timestamp"].cast<double>());
            }
            
            return percept;
        }, py::arg("data"), "Create percept from Python dictionary");

    // SimpleWorld class
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Missing Brain method bindings (Phase 1)
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulation system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
        // Helper methods for Python
        .def("getRegionStats", [](Brain& brain, RegionId regionId) {
            return python::formatNeuronStats(brain, brain.getRegion(regionId));
        }, py::arg("region_id"), "Get formatted statistics for a region")
        .def("getTopFiringNeurons", [](Brain& brain, RegionId regionId, size_t count) {
            return python::getTopFiringNeurons(brain.getRegion(regionId), count);
        }, py::arg("region_id"), py::arg("count") = 10, "Get top firing neurons in region")
        .def("getRegionStatistics", [](Brain& brain) {
            return python::getRegionStatistics(brain);
        }, "Get statistics for all brain regions as dictionary")
        .def("getMemoryStats", [](Brain& brain) {
            return python::getMemorySystemStats(brain);
        }, "Get memory system statistics as dictionary")
        .def("getNeuromodulationState", [](Brain& brain) {
            return python::getNeuromodulationState(brain);
        }, "Get neuromodulation system state as dictionary")
        .def("getPlasticityState", [](Brain& brain) {
            return python::getPlasticityState(brain);
        }, "Get plasticity system state as dictionary")
        .def("getDevelopmentProgress", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("isDecreasingPlasticity", [](Brain& brain) {
            auto stage = brain.getDevelopmentalStage();
            return stage == DevelopmentalStage::Maturation || stage == DevelopmentalStage::Adult;
        }, "Check if brain is in stable development phase")
        .def("shouldReacquire", [](Brain& brain, SimulationStep step) {
            return brain.getEpisodicMemory() && 
                   step % brain.getEpisodicMemory()->getReplaysPerEpisode() == 0;
        }, py::arg("step"), "Check if memory replay should occur");

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

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, size_t num_steps) {
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       "Run a complete simulation with brain, world, and agent");

    m.def("quick_test", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        std::cout << "Quick NLM Test:" << std::endl;
        std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
        
        for (size_t i = 0; i < 100; ++i) {
            brain->step(i);
        }
        
        std::cout << "  Steps completed: 100" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        return brain;
    }, "Run a quick test of the NLM brain");

    // Version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "NLM (Neural Learning Machine) - Python bindings for brain simulation";

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Python module documentation
    m.doc() = R"pbdoc(
    NLM Python Bindings
    ====================
    
    This module provides Python bindings for the NLM (Neural Learning Machine) C++ framework.
    It enables Python users to create and simulate artificial brains with neural computation,
    memory systems, plasticity, and neuromodulation.
    
    Key Features:
    - **Brain Simulation**: Create brains with configurable neural architectures
    - **Memory Systems**: Access working memory, episodic memory, and associative memory
    - **Neuromodulation**: Use dopamine, curiosity, novelty, and prediction error signals
    - **Plasticity**: Apply STDP, Hebbian, and structural plasticity rules
    - **Cognition**: Access planning, concept formation, and attentional systems
    - **Development**: Simulate brain development and learning over time
    - **Checkpointing**: Save and load brain states
    - **Monitoring**: Get statistics and state information
    
    Usage Example:
    ```python
    import nlm
    
    # Create a default brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get statistics
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    ```
    
    For more examples, see:
    - `easy_usage.md` - Simple beginner examples
    - `HOW_TO_USE.md` - Comprehensive documentation
    - `docs/ARCHITECTURE.md` - Technical architecture
    )pbdoc";

} // namespace nlm

#undef pybind11_DETAILED_HELP
#undef pybind11_DETAILED_ERROR
#undef pybind11_MODULE
#undef pybind11_DETAILED_MODULE_HELP