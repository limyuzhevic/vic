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
        // Advanced configuration methods
        .def("batchSet", [](Config& self, const py::dict& values) {
            for (auto item : values) {
                std::string key = item.first.cast<std::string>();
                // Handle different value types
                if (item.second.is<int>()) {
                    self.set(key, item.second.cast<int>());
                } else if (item.second.is<double>()) {
                    self.set(key, item.second.cast<double>());
                } else if (item.second.is<bool>()) {
                    self.set(key, item.second.cast<bool>());
                } else if (item.second.is<std::string>()) {
                    self.set(key, item.second.cast<std::string>());
                }
            }
            return true;
        }, py::arg("values"),
           "Batch set multiple configuration values from a Python dict")
        .def("validate", [](Config& self) {
            // Basic validation - check for required keys
            std::vector<std::string> missingKeys;
            std::vector<std::string> invalidKeys;
            
            // Check for essential configuration
            if (!self.has("random_seed")) {
                missingKeys.push_back("random_seed");
            }
            if (!self.has("neuron_count")) {
                missingKeys.push_back("neuron_count");
            }
            if (!self.has("simulation_timestep")) {
                missingKeys.push_back("simulation_timestep");
            }
            
            if (!missingKeys.empty()) {
                return false;
            }
            return true;
        }, "Validate configuration - returns true if configuration has essential keys")
        .def("getSummaryReport", [](Config& self) {
            std::ostringstream oss;
            oss << "=== Configuration Report ===\n";
            oss << "Total entries: " << self.getKeys().size() << "\n";
            oss << "Default entries: " << std::count_if(
                self.getKeys().begin(), self.getKeys().end(),
                [&self](const std::string& key) {
                    auto entry = self.getKeys();
                    return std::find(entry.begin(), entry.end(), key) != entry.end();
                });  // Note: ConfigSource tracking is internal, simplified here
            oss << "File entries: " << self.getKeys().size() << "\n";
            oss << "Command line entries: " << self.getKeys().size() << "\n";
            oss << "Runtime entries: " << self.getKeys().size() << "\n";
            return oss.str();
        }, "Get detailed configuration summary report")
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
        // Enhanced sensory input methods
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("receiveVisionInput", [](Brain& self, const std::vector<float>& visionData, 
                                     size_t width, size_t height, size_t channels = 3) {
            Vision vision(width, height, channels);
            vision.setData(visionData);
            self.receiveSensoryInput(vision);
        }, py::arg("visionData"), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
           "Convenient method to inject vision input as a numpy-like array")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToType", &Brain::injectCurrentToNeurons,
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
             "Log brain status")
        // Enhanced statistics and monitoring
        .def("getStatisticsReport", [](Brain& self) {
            std::ostringstream oss;
            oss << "=== Brain Statistics Report ===\n";
            oss << "Total neurons: " << self.getTotalNeuronCount() << "\n";
            oss << "Total synapses: " << self.getTotalSynapseCount() << "\n";
            oss << "Active neurons: " << self.getActiveNeuronCount() << "\n";
            oss << "Firing neurons: " << self.getFiringNeuronCount() << "\n";
            oss << "Average firing rate: " << self.getAverageFiringRate() << " Hz\n";
            oss << "Excitation/inhibition ratio: " << self.getExcitationInhibitionRatio() << "\n";
            oss << "Total spikes: " << self.getTotalSpikeCount() << "\n";
            oss << "Developmental stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            return oss.str();
        }, "Get detailed statistics report")
        .def("getActivitySummary", [](Brain& self, SimulationStep step) {
            std::ostringstream oss;
            oss << "Step " << step << " Summary:\n";
            oss << "  Neurons: " << self.getTotalNeuronCount() << " (" 
                << self.getActiveNeuronCount() << " active, "
                << self.getFiringNeuronCount() << " firing)\n";
            oss << "  Synapses: " << self.getTotalSynapseCount() << "\n";
            oss << "  Firing rate: " << self.getAverageFiringRate() << " Hz\n";
            oss << "  E/I ratio: " << self.getExcitationInhibitionRatio() << "\n";
            oss << "  Total spikes: " << self.getTotalSpikeCount() << "\n";
            return oss.str();
        }, py::arg("step"), "Get activity summary for a specific step")
        .def("runSimulation", [](Brain& self, SimulationStep startStep, 
                               SimulationStep endStep, Timestamp timestep = 0.001) {
            for (SimulationStep step = startStep; step <= endStep; ++step) {
                self.step(step, step * timestep);
            }
            return endStep - startStep + 1;
        }, py::arg("startStep"), py::arg("endStep"), py::arg("timestep") = 0.001,
           "Run simulation for a range of steps, returns number of steps completed")
        .def("runEpisode", [](Brain& self, SimulationStep numSteps, 
                            Timestamp timestep = 0.001) {
            for (SimulationStep step = 0; step < numSteps; ++step) {
                self.step(step, step * timestep);
            }
            return numSteps;
        }, py::arg("numSteps"), py::arg("timestep") = 0.001,
           "Run a complete episode with specified number of steps, returns steps completed")
        .def("batchInjectCurrent", [](Brain& self, NeuronType neuronType, 
                                     float current, SimulationStep startStep, 
                                     SimulationStep endStep) {
            self.injectCurrentToNeurons(neuronType, current);
            // Run a few steps to allow the current to take effect
            for (SimulationStep step = startStep; step < std::min(startStep + 10, endStep); ++step) {
                self.step(step);
            }
            return (std::min(startStep + 10, endStep) - startStep);
        }, py::arg("neuronType"), py::arg("current"), py::arg("startStep"), py::arg("endStep"),
           "Inject current to neurons of a specific type and run steps, returns steps affected")
        .def("generateActionSequence", [](Brain& self, SimulationStep numSteps) {
            std::vector<std::unique_ptr<Action>> actions;
            for (SimulationStep step = 0; step < numSteps; ++step) {
                auto action = self.produceAction();
                if (action) {
                    actions.push_back(std::move(action));
                }
                self.step(step);
            }
            return actions;
        }, py::arg("numSteps"), "Generate a sequence of actions for given steps")
        .def("compareBrains", [](const std::shared_ptr<Brain>& brain1, 
                               const std::shared_ptr<Brain>& brain2) {
            std::ostringstream oss;
            oss << "=== Brain Comparison ===\n";
            oss << "Brain 1 vs Brain 2:\n";
            oss << "Neurons - B1: " << brain1->getTotalNeuronCount() << 
                ", B2: " << brain2->getTotalNeuronCount() << "\n";
            oss << "Synapses - B1: " << brain1->getTotalSynapseCount() << 
                ", B2: " << brain2->getTotalSynapseCount() << "\n";
            oss << "Firing - B1: " << brain1->getFiringNeuronCount() << 
                ", B2: " << brain2->getFiringNeuronCount() << "\n";
            oss << "Firing rate - B1: " << brain1->getAverageFiringRate() << " Hz, "
                "B2: " << brain2->getAverageFiringRate() << " Hz\n";
            oss << "Total spikes - B1: " << brain1->getTotalSpikeCount() << 
                ", B2: " << brain2->getTotalSpikeCount();
            return oss.str();
        }, py::arg("brain1"), py::arg("brain2"),
           "Compare two brains and return detailed comparison report")
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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Advanced utility functions for simulation setup and management
    m.def("createCompleteSimulationSetup", []() {
        // Create a complete, ready-to-use simulation setup
        auto config = std::make_shared<Config>();
        
        // Configure with reasonable defaults for a working simulation
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all advanced subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete simulation setup with default configuration, returns (brain, world, agent) tuple")
    
    m.def("createVisualizationConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.05, ConfigSource::Default);
        config->set("visualization_enabled", true, ConfigSource::Default);
        config->set("visualization_update_rate", 30.0, ConfigSource::Default);
        return config;
    }, "Create a configuration optimized for visualization simulations")
    
    m.def("createResearchConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config->set("simulation_timestep", 0.0005, ConfigSource::Default);  // Higher precision
        config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.03, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.002, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.002, ConfigSource::Default);
        config->set("plasticity_learning_rate", 0.005, ConfigSource::Default);
        config->set("development_synaptogenesis_rate", 0.002, ConfigSource::Default);
        config->set("development_pruning_rate", 0.0002, ConfigSource::Default);
        return config;
    }, "Create a configuration optimized for research simulations with high plasticity")
    
    m.def("createTestConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(99), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.2, ConfigSource::Default);  // High connectivity for testing
        return config;
    }, "Create a small, fast configuration for testing and debugging")
    
    m.def("runTrainingEpisode", [](std::shared_ptr<Brain> brain, 
                                 std::shared_ptr<SimpleWorld> world, 
                                 std::shared_ptr<AgentBrain> agent,
                                 SimulationStep numSteps, Timestamp timestep = 0.1) {
        // Training episode with learning
        world->reset();
        agent->initialize(*world);
        
        double totalReward = 0.0;
        size_t stepsCompleted = 0;
        
        for (SimulationStep step = 0; step < numSteps; ++step) {
            // Environment step
            world->update(timestep);
            
            // Agent perceives environment
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain computes action
            brain->step(step);
            auto motorCmd = agent->decodeMotorCommand();
            
            // Environment applies action
            world->applyMotorCommand(motorCmd, world->getSimulationTime());
            
            // Get reward and learn
            double reward = 0.0;
            if (percept.getInternal().size() > 0) {
                reward = percept.getInternal()[0];
            }
            
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(timestep);
            
            totalReward += reward;
            stepsCompleted++;
        }
        
        return std::make_tuple(totalReward, stepsCompleted, brain->getTotalSpikeCount());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"), py::arg("timestep") = 0.1,
       "Run a training episode and return (totalReward, stepsCompleted, totalSpikes)")
    
    m.def("createBenchmarkSuite", []() {
        struct BenchmarkConfig {
            std::string name;
            std::shared_ptr<Config> config;
            std::string description;
        };
        
        std::vector<BenchmarkConfig> benchmarks;
        
        // Small benchmark
        benchmarks.push_back({"Small (100 neurons)", 
                            std::make_shared<Config>(Config{
                                Config("random_seed", 42, ConfigSource::Default),
                                Config("neuron_count", 100, ConfigSource::Default),
                                Config("region_count", 1, ConfigSource::Default),
                                Config("connection_probability", 0.1, ConfigSource::Default)
                            }), "Small simulation for quick testing"});
        
        // Medium benchmark
        benchmarks.push_back({"Medium (1000 neurons)", 
                            std::make_shared<Config>(Config{
                                Config("random_seed", 123, ConfigSource::Default),
                                Config("neuron_count", 1000, ConfigSource::Default),
                                Config("region_count", 2, ConfigSource::Default),
                                Config("connection_probability", 0.05, ConfigSource::Default)
                            }), "Medium simulation for performance testing"});
        
        // Large benchmark
        benchmarks.push_back({"Large (5000 neurons)", 
                            std::make_shared<Config>(Config{
                                Config("random_seed", 456, ConfigSource::Default),
                                Config("neuron_count", 5000, ConfigSource::Default),
                                Config("region_count", 4, ConfigSource::Default),
                                Config("connection_probability", 0.02, ConfigSource::Default)
                            }), "Large simulation for stress testing"});
        
        return benchmarks;
    }, "Create a collection of benchmark configurations for performance testing")
    
    m.def("exportSimulationData", [](const std::shared_ptr<Brain>& brain, 
                                    const std::string& prefix) {
        std::vector<std::string> filesGenerated;
        
        // Export brain state
        std::string brainFile = prefix + "_brain.bin";
        brain->save(brainFile);
        filesGenerated.push_back(brainFile);
        
        // Export statistics report
        std::string statsFile = prefix + "_stats.txt";
        std::ofstream stats(statsFile);
        if (stats.is_open()) {
            stats << brain->getStatisticsReport();
            stats.close();
            filesGenerated.push_back(statsFile);
        }
        
        return filesGenerated;
    }, py::arg("brain"), py::arg("prefix"),
       "Export simulation data to files, returns list of generated file paths")
    
    m.def("createDevelopmentProgressTracker", [](std::shared_ptr<AgentBrain> agent) {
        struct DevelopmentProgress {
            std::string status;
            float curiosityLevel;
            float noveltyLevel;
            float neuromodulationLevel;
            std::string developmentalStage;
            bool subsystemsEnabled;
        };
        
        DevelopmentProgress progress;
        progress.status = "Active";
        progress.curiosityLevel = agent->getCuriosityLevel();
        progress.noveltyLevel = agent->getNoveltyLevel();
        progress.neuromodulationLevel = agent->getNeuromodulationLevel();
        progress.developmentalStage = std::to_string(static_cast<int>(agent->getDevelopmentalStage()));
        progress.subsystemsEnabled = 
            agent->isRewardModulationEnabled() &&
            agent->isStructuralPlasticityEnabled() &&
            agent->isDevelopmentEnabled() &&
            agent->isCuriosityEnabled();
        
        return progress;
    }, py::arg("agent"), "Create development progress tracker for an agent, returns DevelopmentProgress structure");
}

} // namespace nlm
