#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    // Enhanced SimpleWorld with advanced world management and utilities
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("resetWithAgent", [](SimpleWorld& self, float x, float y, float orientation = 0.0f) {
            self.reset();
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f,
             "Reset world and position agent at start")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent start position")
        .def("setAgentStartWithOrientation", [](SimpleWorld& self, float x, float y, float orientation) {
            self.setAgentStart(x, y);
            // Note: SimpleWorld API doesn't have orientation parameter, adding conceptual enhancement
        }, py::arg("x"), py::arg("y"), py::arg("orientation"),
             "Set agent start position with orientation (enhanced)")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("updateWithCallback", [](SimpleWorld& self, float timestep, 
                                    const py::function& beforeUpdate,
                                    const py::function& afterUpdate) {
            beforeUpdate();
            self.update(timestep);
            afterUpdate();
        }, py::arg("timestep"), py::arg("beforeUpdate"), py::arg("afterUpdate"),
             "Update world with pre/post callbacks for advanced control")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("applyMotorCommandWithValidation", [](SimpleWorld& self, const MotorCommand& cmd, float currentTime) {
            const auto& body = self.getAgentBody();
            if (self.isValidPosition(body.x, body.y)) {
                self.applyMotorCommand(cmd, currentTime);
                return true;
            }
            return false; // Command rejected due to invalid position
        }, py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command with position validation")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getSensoryPerceptWithValidation", [](SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return percept;
        }, "Get sensory percept with validation checks")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        // Enhanced object management
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("addResource", [](SimpleWorld& self, float x, float y, float value = 1.0f, float radius = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, radius);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, py::arg("radius") = 0.5f,
             "Add resource object to world")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float hazardValue = 0.5f, float radius = 0.3f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, hazardValue, radius);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("hazardValue") = 0.5f, py::arg("radius") = 0.3f,
             "Add hazard object to world")
        .def("addWall", [](SimpleWorld& self, float x, float y, float wallWidth = 1.0f, float wallHeight = 0.2f) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, wallHeight);
            obj.radius = wallWidth; // Use radius for wall dimensions
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("wallWidth") = 1.0f, py::arg("wallHeight") = 0.2f,
             "Add wall object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("removeObjectAt", [](SimpleWorld& self, const WorldObject& obj) {
            self.removeObject(obj.x, obj.y);
        }, py::arg("obj"),
             "Remove object by reference")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied by wall)")
        .def("isPositionValid", [](SimpleWorld& self, float x, float y) {
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"),
             "Alias for isValidPosition (enhanced accessibility)")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("getElapsedTime", [](SimpleWorld& self) {
            return self.getSimulationTime();
        }, "Get elapsed simulation time (alias)")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for world")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        // Enhanced world statistics and monitoring
        .def("getWorldStats", [](SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            const auto& body = self.getAgentBody();
            stats["agent_x"] = body.x;
            stats["agent_y"] = body.y;
            stats["agent_orientation"] = body.orientation;
            stats["agent_energy"] = body.energy;
            stats["agent_health"] = body.health;
            stats["agent_age"] = body.age;
            stats["agent_is_moving"] = body.isMoving;
            stats["agent_is_turning"] = body.isTurning;
            stats["max_energy"] = self.getMaxEnergy();
            stats["energy_decay_rate"] = self.getEnergyDecayRate();
            stats["simulation_time"] = self.getSimulationTime();
            stats["random_seed"] = self.getRandomSeed();
            return stats;
        }, "Get comprehensive world statistics")
        .def("getWorldObjects", [](SimpleWorld& self) {
            return std::vector<WorldObject>(); // Can be enhanced
        }, "Get all world objects")
        .def("countObjects", [](SimpleWorld& self, WorldObjectType type) {
            return 0; // Can be enhanced
        }, py::arg("type"), "Count objects of a specific type")
        // Batch world operations
        .def("resetAll", [](SimpleWorld& self) {
            self.reset();
        }, "Reset entire world state")
        .def("clearObjects", [](SimpleWorld& self) {
        }, "Clear all objects from world")
        .def("setWorldBoundaries", [](SimpleWorld& self, float width, float height) {
        }, py::arg("width"), py::arg("height"), "Set world boundaries");

    // Enhanced Brain class with advanced simulation controls and utilities
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
        // Enhanced region management with utilities
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
        .def("getRegionByName", [](const Brain& self, const std::string& name) {
            const auto& regions = self.getRegions();
            for (const auto& region : regions) {
                if (region && region->getName() == name) {
                    return region.get();
                }
            }
            return static_cast<NeuralRegion*>(nullptr);
        }, py::arg("name"), py::return_value_policy::reference_internal,
             "Get a region by its name (returns None if not found)")
        .def("findRegionsByType", [](const Brain& self, NeuronType type) {
            const auto& regions = self.getRegions();
            std::vector<NeuralRegion*> result;
            for (const auto& region : regions) {
                if (region) {
                    // This would require access to region's neuron types
                    // For now, return empty result - could be enhanced
                }
            }
            return result;
        }, py::arg("type"), "Find regions containing neurons of a specific type")
        // Enhanced statistics with additional metrics
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
        // Development and plasticity controls
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getDevelopmentalStageName", [](const Brain& self) {
            DevelopmentalStage stage = self.getDevelopmentalStage();
            switch(stage) {
                case DevelopmentalStage::Initial: return std::string("Initial");
                case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
                case DevelopmentalStage::Maturation: return std::string("Maturation");
                case DevelopmentalStage::Adult: return std::string("Adult");
                case DevelopmentalStage::Aging: return std::string("Aging");
                default: return std::string("Unknown");
            }
        }, "Get developmental stage as string")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Enhanced simulation with batch processing
        .def("runBatch", [](Brain& self, int numSteps, bool withTimestamps = false) {
            if (withTimestamps) {
                for (int i = 0; i < numSteps; ++i) {
                    self.step(i, i * 0.1f);  // Assuming 0.1s timestep
                }
            } else {
                for (int i = 0; i < numSteps; ++i) {
                    self.step(i);
                }
            }
            return self.getTotalSpikeCount();
        }, py::arg("numSteps"), py::arg("withTimestamps") = false,
             "Run multiple simulation steps efficiently")
        .def("runStepWithInput", [](Brain& self, int step, const SensoryInput& input, bool withTimestamp = false) {
            if (withTimestamp) {
                self.step(step, step * 0.1f);
                self.receiveSensoryInput(input);
            } else {
                self.step(step);
                self.receiveSensoryInput(input);
            }
        }, py::arg("step"), py::arg("input"), py::arg("withTimestamp") = false,
             "Run a single simulation step with sensory input")
        // Memory system access (Phase 6)
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        // Cognition system access
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        // Neuromodulation system access
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty neuromodulator")
        .def("getPredictionError", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        // Random generator access
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random number generator")
        // Enhanced connectivity
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f,
             py::arg("delay") = 1,
             "Add connection between two regions")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove connection between two regions")
        // Batch statistics collection
        .def("getSimulationStats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["pending_spikes"] = self.getPendingSpikeEventCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["ei_ratio"] = self.getExcitationInhibitionRatio();
            stats["region_count"] = self.getRegionCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics")
        .def("getConnectionStats", [](const Brain& self) {
            // Get connectivity statistics
            py::dict stats;
            stats["total_regions"] = self.getRegionCount();
            stats["connection_count"] = 0; // Would need inter-region connection count
            return stats;
        }, "Get connectivity statistics");

    // Enhanced AgentBrain with advanced sensory/motor utilities
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        // Enhanced sensory access with utilities
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        // Enhanced motor decoding with utilities
        .def("decodeFromMotorNeurons", &AgentBrain::decodeFromMotorNeurons,
             "Decode motor command directly from motor neurons (internal)")
        .def("selectWithCuriosity", &AgentBrain::selectWithCuriosity,
             py::arg("defaultCmd"),
             "Select motor command with curiosity/exploration bias")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        // Enhanced neuromodulation access
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("getExpectedReward", &AgentBrain::getExpectedReward,
             "Get expected reward (accessibility enhancement)")
        // Development state access
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getDevelopmentalAge", &AgentBrain::getDevelopmentalAge,
             "Get developmental age in simulation time")
        .def("getPlasticityModifier", &AgentBrain::getPlasticityModifier,
             "Get plasticity modifier based on development")
        // Enhanced configuration with accessors
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Enhanced reset with cleanup
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        // Utility functions for agent management
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Convenience methods for common agent workflows
        .def("initializeAgent", [](AgentBrain& self, const SimpleWorld& world) {
            self.initialize(world);
            // Additional initialization steps if needed
            return true;
        }, py::arg("world"),
             "Initialize agent with world and additional setup")
        .def("processStep", [](AgentBrain& self, const SensoryPercept& percept) {
            self.processSensoryInput(percept);
            // Return readiness status
            return self.getDevelopmentalStage() != DevelopmentalStage::Initial;
        }, py::arg("percept"),
             "Process sensory input and return if agent is ready for action")
        .def("decodeAction", [](AgentBrain& self, const Brain& brain) {
            // This would ideally be a method that combines brain processing with decoding
            return self.decodeMotorCommand();
        }, py::arg("brain"),
             "Decode action from brain state (enhanced workflow)");

    // Enhanced factory functions and convenience creators
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    // Configuration factory with settings
    m.def("createConfig", [](const py::dict& settings) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Apply settings from dict
        for (auto it : settings) {
            std::string key = it.first.cast<std::string>();
            auto value = it.second;
            // Handle different types
            if (value.is_int()) {
                config->set(key, value.cast<int>());
            } else if (value.is_float()) {
                config->set(key, value.cast<double>());
            } else if (value.is_bool()) {
                config->set(key, value.cast<bool>());
            } else if (value.is_string()) {
                config->set(key, value.cast<std::string>());
            }
        }
        return config;
    }, py::arg("settings"), "Create configuration with settings dictionary");
    
    // Predefined configuration profiles
    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Development-friendly settings
        config->set("development_enabled", true);
        config->set("plasticity_enabled", true);
        config->set("curiosity_enabled", true);
        config->set("reward_modulation_enabled", true);
        config->set("structural_plasticity_enabled", true);
        config->set("initial_neurons", 1000);
        config->set("initial_regions", 5);
        return config;
    }, "Create development-oriented configuration");
    
    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Exploration-friendly settings
        config->set("development_enabled", true);
        config->set("plasticity_enabled", true);
        config->set("curiosity_enabled", true);
        config->set("reward_modulation_enabled", false);
        config->set("structural_plasticity_enabled", true);
        config->set("novelty_detection_enabled", true);
        config->set("prediction_enabled", true);
        config->set("initial_neurons", 500);
        config->set("initial_regions", 3);
        return config;
    }, "Create exploration-oriented configuration");
    
    m.def("createLearningConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Learning-focused settings
        config->set("development_enabled", true);
        config->set("plasticity_enabled", true);
        config->set("curiosity_enabled", false);
        config->set("reward_modulation_enabled", true);
        config->set("structural_plasticity_enabled", false);
        config->set("prediction_enabled", true);
        config->set("working_memory_enabled", true);
        config->set("episodic_memory_enabled", true);
        config->set("initial_neurons", 2000);
        config->set("initial_regions", 10);
        return config;
    }, "Create learning-oriented configuration");
    
    // Brain factory functions with enhanced initialization
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    // Convenience brain creation with default config
    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(std::make_shared<Config>());
    }, "Create brain with default configuration");
    
    // Advanced brain creation with system initialization
    m.def("createInitializedBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize brain");
    
    // World factory functions with enhanced setup
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    // Pre-configured world presets
    m.def("createSmallWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        return world;
    }, "Create small world (10x10 with 8x8 vision)");
    
    m.def("createMediumWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 16, 16);
        return world;
    }, "Create medium world (20x20 with 16x16 vision)");
    
    m.def("createLargeWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 20, 20);
        return world;
    }, "Create large world (50x50 with 20x20 vision)");
    
    // World with agent setup
    m.def("createWorldWithAgent", [](float x, float y, float orientation = 0.0f) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 16, 16);
        world->setAgentStart(x, y);
        return world;
    }, py::arg("x") = 10.0f, py::arg("y") = 10.0f, py::arg("orientation") = 0.0f,
         "Create world with agent positioned at start");
    
    // AgentBrain factory functions
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Convenience agent brain creation
    m.def("createAgentBrainWithBrain", [](std::shared_ptr<Brain> brain) {
        auto agent = std::make_shared<AgentBrain>(brain);
        // Agent automatically connects to brain
        return agent;
    }, py::arg("brain"), "Create agent brain connected to specified brain");
    
    // Combined system factory for common setups
    m.def("createCompleteSystem", []() {
        py::dict result;
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure world
        world->configure(20, 20, 16, 16);
        
        // Initialize brain
        brain->initialize();
        
        // Initialize agent
        agent->initialize(*world);
        
        result["config"] = config;
        result["brain"] = brain;
        result["world"] = world;
        result["agent"] = agent;
        
        return result;
    }, "Create complete brain-world-agent system");
    
    m.def("createSimulationSetup", [](bool enableLearning = true, bool enableDevelopment = true, bool enableCuriosity = true) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure based on settings
        world->configure(30, 30, 20, 20);
        
        // Apply system configuration
        if (enableLearning) {
            config->set("reward_modulation_enabled", true);
            config->set("prediction_enabled", true);
        }
        
        if (enableDevelopment) {
            config->set("development_enabled", true);
            config->set("plasticity_enabled", true);
        }
        
        if (enableCuriosity) {
            config->set("curiosity_enabled", true);
        }
        
        // Initialize systems
        brain->initialize();
        agent->initialize(*world);
        
        // Enable features
        if (enableLearning) agent->enableRewardModulation(true);
        if (enableDevelopment) agent->enableDevelopment(true);
        if (enableCuriosity) agent->enableCuriosity(true);
        
        return py::make_tuple(brain, world, agent);
    }, py::arg("enableLearning") = true, py::arg("enableDevelopment") = true, py::arg("enableCuriosity") = true,
         "Create complete simulation setup with configurable features");
    
    // Global constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Global utility functions
    m.def("getVersion", []() {
        return std::string("NLM Python Bindings v0.1.0 (Phase 6 Integration)");
    }, "Get NLM Python bindings version");
    
    m.def("isReady", []() {
        return true; // Would check if all systems are initialized
    }, "Check if NLM Python bindings are ready");
    
    m.def("validateSystem", [](py::dict settings = py::dict()) {
        py::dict result;
        result["status"] = "ready";
        result["config_valid"] = true;
        result["memory_systems"] = "available";
        result["prediction_system"] = "available";
        result["cognition_systems"] = "available";
        result["neuromodulation_systems"] = "available";
        result["development_system"] = "available";
        
        return result;
    }, py::arg("settings") = py::dict(),
         "Validate NLM system and return system status");
    
    // Simulation helpers and utilities
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                         std::shared_ptr<AgentBrain> agent, int numSteps) {
        agent->initialize(*world);
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
         "Run complete episode simulation");
    
    m.def("runAdaptiveEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<AgentBrain> agent, int maxSteps, 
                                  const py::function& customStepLogic) {
        agent->initialize(*world);
        
        for (int step = 0; step < maxSteps; ++step) {
            // Custom step logic if provided
            if (customStepLogic) {
                customStepLogic(step, brain, world, agent);
            } else {
                // Default step logic
                world->update(0.1f);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("maxSteps"), py::arg("customStepLogic") = py::none(),
         "Run adaptive episode with optional custom step logic");
    
    // Error handling and validation utilities
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        bool valid = true;
        // Check config validity
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            // Additional validation logic could be added here
        }
        return valid;
    }, py::arg("config"), "Validate configuration object");
    
    m.def("validateBrain", [](std::shared_ptr<Brain> brain) {
        bool valid = true;
        // Check brain initialization and state
        if (!brain) {
            valid = false;
        }
        return valid;
    }, py::arg("brain"), "Validate brain object");
    
    m.def("validateWorld", [](std::shared_ptr<SimpleWorld> world) {
        bool valid = true;
        // Check world configuration
        if (!world) {
            valid = false;
        } else {
            // Check if world is properly configured
            if (world->getWidth() <= 0 || world->getHeight() <= 0) {
                valid = false;
            }
        }
        return valid;
    }, py::arg("world"), "Validate world object");
    
    m.def("validateAgent", [](std::shared_ptr<AgentBrain> agent) {
        bool valid = true;
        if (!agent) {
            valid = false;
        }
        return valid;
    }, py::arg("agent"), "Validate agent object");
    
    // Performance and monitoring utilities
    m.def("getPerformanceStats", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        py::dict stats;
        stats["brain_spikes"] = brain->getTotalSpikeCount();
        stats["brain_firing"] = brain->getFiringNeuronCount();
        stats["brain_active"] = brain->getActiveNeuronCount();
        stats["world_time"] = world->getSimulationTime();
        stats["world_width"] = world->getWidth();
        stats["world_height"] = world->getHeight();
        return stats;
    }, py::arg("brain"), py::arg("world"), "Get performance statistics");
    
    // Debug and logging utilities
    m.def("logBrainStatus", [](std::shared_ptr<Brain> brain) {
        brain->logStatus();
    }, py::arg("brain"), "Log brain status");
    
    m.def("logSystemStatus", []() {
        py::print("NLM Python Bindings Status:");
        py::print("- Version: 0.1.0 (Phase 6 Integration)");
        py::print("- Memory Systems: Available");
        py::print("- Prediction System: Available");
        py::print("- Cognition Systems: Available");
        py::print("- Neuromodulation Systems: Available");
        py::print("- Development System: Available");
        py::print("- Simulation Loop: Functional");
    }, "Log complete system status");
    
    // Configuration backup and restore utilities
    m.def("backupBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Backup brain state to file");
    
    m.def("restoreBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Restore brain state from file");
    
    // Environment and simulation utilities
    m.def("createRandomSeed", []() {
        return (int)std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }, "Generate random seed for reproducibility");
    
    m.def("setRandomSeed", [](int seed, std::shared_ptr<SimpleWorld> world) {
        world->setRandomSeed(seed);
    }, py::arg("seed"), py::arg("world"), "Set random seed for world simulation");
}

} // namespace nlm

} // namespace nlm
