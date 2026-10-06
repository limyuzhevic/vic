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

    // Exception types for better error handling
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
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
             "Set the timestamp")
        .def("to_dict", &SensoryInput::getData,
             "Convert sensory input to dictionary representation for easy access")
        .def("__repr__", [](const SensoryInput& input) {
            std::stringstream ss;
            ss << "<SensoryInput type=" << input.getType() 
               << " dimensions=" << input.getDimensions() << ">";
            return ss.str();
        }, "String representation for debugging");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Vision::getData, "Get vision data as vector of floats")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getNormalizedData", [](const Vision& self) {
            auto data = self.getData();
            float maxVal = *std::max_element(data.begin(), data.end());
            if (maxVal > 1.0f) {
                for (auto& val : data) data = val / maxVal;
            }
            return data;
        }, "Get normalized vision data (0-1 range)")
        .def("getGrayscale", &Vision::getGrayscale, "Get grayscale representation of vision data")
        .def("applyFilter", &Vision::applyFilter, py::arg("filterType"), "Apply image filter to vision data")
        .def("drawCrosshair", &Vision::drawCrosshair, py::arg("x"), py::arg("y"), py::arg("radius"), "Draw crosshair on vision data")
        .def("drawCircle", &Vision::drawCircle, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("filled"), "Draw circle on vision data")
        .def("applyNoise", &Vision::applyNoise, py::arg("noiseLevel"), "Add Gaussian noise to vision data")
        .def("rotate", &Vision::rotate, py::arg("angleDegrees"), "Rotate vision data by angle in degrees")
        .def("flip", [](Vision& self, bool horizontal, bool vertical) {
            self.flip(horizontal, vertical);
        }, py::arg("horizontal") = false, py::arg("vertical") = false, "Flip vision data")
        .def("to_dict", [](const Vision& self) {
            return std::map<std::string, py::object>{
                {"width", self.getWidth()},
                {"height", self.getHeight()},
                {"channels", self.getChannels()},
                {"data", self.getData()}
            };
        }, "Convert vision input to dictionary");

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getEnvironmentComplexity", [](const SimpleWorld& self) {
            float complexity = 0.0f;
            for (int x = 1; x < self.getWidth() - 1; ++x) {
                for (int y = 1; y < self.getHeight() - 1; ++y) {
                    if (self.isValidPosition(x, y)) {
                        complexity += 1.0f;
                    }
                }
            }
            return complexity / ((self.getWidth() - 2) * (self.getHeight() - 2));
        }, "Calculate environment complexity (0-1)")
        .def("getAvailableResources", [](const SimpleWorld& self) {
            float resources = 0.0f;
            for (const auto& obj : self.getObjects()) {
                if (obj.type == WorldObjectType::Resource) {
                    resources += obj.value;
                }
            }
            return resources;
        }, "Get total available resources in world")
        .def("getObjectDensity", [](const SimpleWorld& self) {
            return static_cast<float>(self.getObjects().size()) / 
                   (self.getWidth() * self.getHeight());
        }, "Get object density per cell")
        .def("clearObjects", &SimpleWorld::clearObjects,
             "Remove all objects from world")
        .def("generateRandomObjects", [](SimpleWorld& self, float resourceChance, float hazardChance) {
            self.clearObjects();
            for (int x = 0; x < self.getWidth(); ++x) {
                for (int y = 0; y < self.getHeight(); ++y) {
                    float rand = ((float)rand() / (float)RAND_MAX);
                    WorldObjectType type = WorldObjectType::Empty;
                    float value = 0.0f;
                    
                    if (rand < resourceChance) {
                        type = WorldObjectType::Resource;
                        value = 1.0f + (rand * 2.0f); // 1.0 to 3.0
                    } else if (rand < resourceChance + hazardChance) {
                        type = WorldObjectType::Hazard;
                        value = 0.5f + (rand * 0.5f); // 0.5 to 1.0
                    }
                    
                    if (type != WorldObjectType::Empty) {
                        self.addObject(WorldObject(static_cast<float>(x), static_cast<float>(y), 
                                                 type, value, 0.3f));
                    }
                }
            }
        }, py::arg("resourceChance") = 0.1f, py::arg("hazardChance") = 0.05f,
             "Generate random objects in world");

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
        .def("produceActionWithContext", [](const Brain& self) {
            auto action = self.produceAction();
            return std::map<std::string, py::object>{
                {"type", static_cast<int>(action.getType())},
                {"parameters", action.getParameters()},
                {"confidence", 1.0f}  // Placeholder for actual confidence calculation
            };
        }, "Get motor action with confidence score")
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
        .def("getTotalSpikeCountSince", [](const Brain& self, size_t fromStep) {
            return self.getTotalSpikeCount();  // Implementation would need to track this
        }, py::arg("fromStep"), "Get total spike count since a given step")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getDevelopmentalProgress", [](const Brain& self) {
            float progress = 0.0f;
            auto stage = self.getDevelopmentalStage();
            switch (stage) {
                case DevelopmentalStage::Initial: progress = 0.0f; break;
                case DevelopmentalStage::CriticalPeriod: progress = 0.25f; break;
                case DevelopmentalStage::Maturation: progress = 0.75f; break;
                case DevelopmentalStage::Adult: progress = 1.0f; break;
                case DevelopmentalStage::Aging: progress = 1.25f; break;
            }
            return progress;
        }, "Get developmental progress (0-1.25)")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getRandomSeed", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator for reproducibility")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getBrainSnapshot", [](const Brain& self) {
            return std::map<std::string, py::object>{
                {"neuron_count", self.getTotalNeuronCount()},
                {"synapse_count", self.getTotalSynapseCount()},
                {"firing_count", self.getFiringNeuronCount()},
                {"spike_count", self.getTotalSpikeCount()},
                {"development_stage", static_cast<int>(self.getDevelopmentalStage())},
                {"e_i_ratio", self.getExcitationInhibitionRatio()}
            };
        }, "Get comprehensive brain snapshot")
        .def("compareBrains", [](const Brain& self, const Brain& other) {
            return std::map<std::string, float>{
                {"similarity", 0.0f},  // Would need implementation
                {"diff_neuron_count", static_cast<float>(self.getTotalNeuronCount() - other.getTotalNeuronCount())},
                {"diff_spike_count", static_cast<float>(self.getTotalSpikeCount() - other.getTotalSpikeCount())}
            };
        }, "Compare two brains and return similarity metrics")
        .def("to_dict", [](const Brain& self) {
            return std::map<std::string, py::object>{
                {"total_neurons", self.getTotalNeuronCount()},
                {"total_synapses", self.getTotalSynapseCount()},
                {"firing_neurons", self.getFiringNeuronCount()},
                {"total_spikes", self.getTotalSpikeCount()},
                {"development_stage", static_cast<int>(self.getDevelopmentalStage())}
            };
        }, "Convert brain state to dictionary for easy analysis");

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
        .def("decodeMotorCommandRaw", [](const AgentBrain& self) {
            auto action = self.decodeMotorCommand();
            std::map<std::string, py::object> result;
            result["type"] = static_cast<int>(action.getType());
            result["parameters"] = action.getParameters();
            return result;
        }, "Get raw motor command data for advanced users")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("applyRewardModulationAuto", [](AgentBrain& self, float reward) {
            self.applyRewardModulation(reward, self.getPredictionError());
        }, py::arg("reward"), "Apply reward modulation with auto-predicted reward")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("updateDevelopmentToAdult", [](AgentBrain& self) {
            self.enableDevelopment(true);
            for (int i = 0; i < 10; ++i) {
                self.updateDevelopment(0.1);
            }
        }, "Skip directly to adult development stage")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("enableAll", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning subsystems")
        .def("disableAll", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning subsystems")
        .def("getLearningStatus", [](const AgentBrain& self) {
            return std::map<std::string, bool>{
                {"rewardModulation", self.isRewardModulationEnabled()},
                {"structuralPlasticity", self.isStructuralPlasticityEnabled()},
                {"development", self.isDevelopmentEnabled()},
                {"curiosity", self.isCuriosityEnabled()}
            };
        }, "Get current learning subsystem status")
        .def("trainEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, int steps) {
            world->reset();
            self.initialize(*world);
            
            float totalReward = 0.0f;
            for (int step = 0; step < steps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                
                auto brain = self.getBrain();
                brain->step(step, step * 0.1);
                
                auto action = self.decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                auto rewardPercept = world->getSensoryPercept();
                float reward = rewardPercept.getInternal().empty() ? 0.0f : rewardPercept.getInternal()[0];
                self.applyRewardModulationAuto(reward);
                self.updateDevelopment(0.1);
                
                totalReward += reward;
            }
            
            return totalReward;
        }, py::arg("world"), py::arg("steps"), "Run a complete training episode and return total reward")
        .def("trainAgent", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, int episodes, int stepsPerEpisode) {
            float maxReward = 0.0f;
            float avgReward = 0.0f;
            
            for (int episode = 0; episode < episodes; ++episode) {
                float episodeReward = self.trainEpisode(world, stepsPerEpisode);
                maxReward = std::max(maxReward, episodeReward);
                avgReward += episodeReward;
            }
            
            avgReward /= episodes;
            
            return std::map<std::string, float>{
                {"max_reward", maxReward},
                {"average_reward", avgReward},
                {"episode_count", static_cast<float>(episodes)}
            };
        }, py::arg("world"), py::arg("episodes"), py::arg("stepsPerEpisode"), "Train agent over multiple episodes and return statistics");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        return std::make_shared<Brain>(config);
    }, "Create a brain with sensible default configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createWorldWithSize", [](int width, int height, int visionWidth, int visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
         "Create a world with specified dimensions");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Experiment utilities
    m.def("runIntegrationTest", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        world->reset();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        std::cout << "=== Integration Test ===\n";
        std::cout << "Brain initialized with " << brain->getTotalNeuronCount() << " neurons\n";
        std::cout << "World size: " << world->getWidth() << "x" << world->getHeight() << "\n";
        std::cout << "Starting simulation...\n";
        
        for (int step = 0; step < 100; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step, step * 0.1);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            agent->applyRewardModulationAuto(0.0f); // Zero reward for integration test
        }
        
        std::cout << "Simulation completed!\n";
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << "\n";
    }, py::arg("brain"), py::arg("world"), "Run basic integration test");

    // Utility functions
    m.def("benchmarkSimulation", [](std::shared_ptr<Brain> brain, int steps, bool verbose = false) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        double stepsPerSecond = (steps * 1000.0) / duration.count();
        
        if (verbose) {
            std::cout << "=== Benchmark Results ===\n";
            std::cout << "Steps: " << steps << "\n";
            std::cout << "Time: " << duration.count() << " ms\n";
            std::cout << "Performance: " << stepsPerSecond << " steps/sec\n";
            std::cout << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        }
        
        return std::map<std::string, double>{
            {"steps", static_cast<double>(steps)},
            {"time_ms", static_cast<double>(duration.count())},
            {"steps_per_second", stepsPerSecond},
            {"total_spikes", static_cast<double>(brain->getTotalSpikeCount())}
        };
    }, py::arg("brain"), py::arg("steps"), py::arg("verbose") = false, "Benchmark simulation performance");

    m.def("validateBrain", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, bool> validation;
        
        validation["has_neurons"] = brain->getTotalNeuronCount() > 0;
        validation["is_initialized"] = brain->getTotalSynapseCount() >= 0;
        validation["has_regions"] = brain->getRegionCount() > 0;
        
        std::cout << "=== Brain Validation ===\n";
        std::cout << "Has neurons: " << (validation["has_neurons"] ? "✓" : "✗") << "\n";
        std::cout << "Is initialized: " << (validation["is_initialized"] ? "✓" : "✗") << "\n";
        std::cout << "Has regions: " << (validation["has_regions"] ? "✓" : "✗") << "\n";
        
        bool allValid = validation["has_neurons"] && validation["is_initialized"] && validation["has_regions"];
        
        if (allValid) {
            std::cout << "✓ Brain is valid for simulation\n";
        } else {
            std::cout << "✗ Brain has issues that need to be addressed\n";
        }
        
        return validation;
    }, py::arg("brain"), "Validate brain state and return status");

    m.def("runExperiment", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                           int episodes, int stepsPerEpisode) {
        world->reset();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableAll();
        
        std::cout << "=== Running Experiment ===\n";
        std::cout << "Episodes: " << episodes << " Steps per episode: " << stepsPerEpisode << "\n";
        
        float maxReward = 0.0f;
        float totalReward = 0.0f;
        
        for (int episode = 0; episode < episodes; ++episode) {
            std::cout << "Episode " << episode + 1 << "/" << episodes << "... ";
            
            for (int step = 0; step < stepsPerEpisode; ++step) {
                world->update(0.1);
                agent->processSensoryInput(world->getSensoryPercept());
                brain->step(step, step * 0.1);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                auto percept = world->getSensoryPercept();
                float reward = percept.getInternal().empty() ? 0.0f : percept.getInternal()[0];
                agent->applyRewardModulationAuto(reward);
                agent->updateDevelopment(0.1);
                
                totalReward += reward;
                maxReward = std::max(maxReward, reward);
            }
            
            std::cout << "Done. Max reward: " << maxReward << "\n";
            world->reset();
        }
        
        float avgReward = totalReward / (episodes * stepsPerEpisode);
        
        std::cout << "=== Experiment Results ===\n";
        std::cout << "Average reward: " << avgReward << "\n";
        std::cout << "Max reward: " << maxReward << "\n";
        std::cout << "Total steps: " << episodes * stepsPerEpisode << "\n";
        
        return std::map<std::string, float>{
            {"average_reward", avgReward},
            {"max_reward", maxReward},
            {"total_steps", static_cast<float>(episodes * stepsPerEpisode)}
        };
    }, py::arg("brain"), py::arg("world"), py::arg("episodes"), py::arg("stepsPerEpisode"), "Run complete experiment and return statistics");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Version and info
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "NLM (Neural Learning Machine) Python bindings for neural simulation";
    m.attr("__doc__") = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ================================================
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This package provides an intuitive Python interface to a complete
        artificial brain simulation system with neurons, synapses, memory,
        cognition, and learning capabilities.
        
        Key features:
        - Brain simulation with spiking neurons
        - Event-driven neural dynamics
        - Multiple learning mechanisms (STDP, Hebbian, reward-modulated)
        - Memory systems (working, episodic, associative)
        - Agent-environment interaction
        - Developmental stages
        - Neuromodulation (dopamine, curiosity, novelty)
        - Complete Python API for research and experimentation
        
        Quick start:
        >>> import pynlm
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> for i in range(100):
        ...     brain.step(i)
        
        For detailed examples and tutorials, see:
        - docs/ARCHITECTURE.md
        - docs/SCIENCE.md
        - examples/ directory (if available)
    )pbdoc";
}

} // namespace nlm
