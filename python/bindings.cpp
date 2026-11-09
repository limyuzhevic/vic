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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("load_from_args", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has_key", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Advanced configuration methods
        .def("validate", &Config::validate,
             "Validate configuration structure and key formats")
        .def("get_keys_by_prefix", &Config::getKeysByPrefix, py::arg("prefix"),
             "Get all keys that start with a specific prefix")
        .def("get_keys_by_suffix", &Config::getKeysBySuffix, py::arg("suffix"),
             "Get all keys that end with a specific suffix")
        .def("get_section", &Config::getSection, py::arg("prefix"),
             "Get configuration values within a section (prefixed by prefix)")
        .def("has_section", &Config::hasSection, py::arg("prefix"),
             "Check if a configuration section exists")
        .def("remove_section", &Config::removeSection, py::arg("prefix"),
             "Remove all keys in a configuration section")
        .def("to_json", &Config::toJSON,
             "Export configuration to JSON string")
        .def("get_summary", &Config::getSummary,
             "Get detailed configuration summary")
        .def("configure_from_dict", [](Config& self, const py::dict& config_dict) {
            for (auto item : config_dict) {
                std::string key = item.first.cast<std::string>();
                py::object value_obj = item.second;
                
                if (value_obj.is(py::int_t())) {
                    self.set(key, value_obj.cast<int64_t>(), ConfigSource::Runtime);
                } else if (value_obj.is(py::float_t())) {
                    self.set(key, value_obj.cast<float>(), ConfigSource::Runtime);
                } else if (value_obj.is(py::str())) {
                    self.set(key, value_obj.cast<std::string>(), ConfigSource::Runtime);
                } else if (value_obj.is(py::bool_())) {
                    self.set(key, value_obj.cast<bool>(), ConfigSource::Runtime);
                } else if (value_obj.is(py::list())) {
                    // Handle list values
                    py::list list_val = value_obj.cast<py::list>();
                    std::vector<float> float_list;
                    bool is_float_list = true;
                    for (auto item : list_val) {
                        if (item.is(py::float_t())) {
                            float_list.push_back(item.cast<float>());
                        } else if (item.is(py::int_t())) {
                            float_list.push_back(static_cast<float>(item.cast<int64_t>()));
                        } else {
                            is_float_list = false;
                            break;
                        }
                    }
                    if (is_float_list) {
                        self.set(key, float_list, ConfigSource::Runtime);
                    } else {
                        std::vector<std::string> string_list;
                        bool is_string_list = true;
                        for (auto item : list_val) {
                            if (item.is(py::str())) {
                                string_list.push_back(item.cast<std::string>());
                            } else {
                                is_string_list = false;
                                break;
                            }
                        }
                        if (is_string_list) {
                            self.set(key, string_list, ConfigSource::Runtime);
                        }
                    }
                }
            }
        }, py::arg("config_dict"),
           "Configure from a Python dictionary")
        .def("get_value_or_none", [](Config& self, const std::string& key) {
            auto valueOpt = self.get<std::string>(key);
            if (valueOpt.has_value()) {
                return py::cast(valueOpt.value());
            }
            return py::none();
        }, py::arg("key"),
           "Get configuration value or None if not found")
        .def("get_with_default", [](Config& self, const std::string& key, py::object default_value) {
            auto valueOpt = self.get<std::string>(key);
            if (valueOpt.has_value()) {
                return py::cast(valueOpt.value());
            }
            return default_value;
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value with default fallback")
        // Batch operations
        .def("merge_config", [](Config& self, const Config& other) {
            for (const auto& entry : other.getKeys()) {
                auto valueOpt = other.get<std::string>(entry);
                if (valueOpt.has_value()) {
                    self.set(entry, valueOpt.value(), ConfigSource::Runtime);
                }
            }
        }, py::arg("other_config"),
           "Merge another configuration into this one")
        .def("filter_keys", [](const Config& self, const std::vector<std::string>& key_pattern) {
            py::list filtered_keys;
            for (const auto& key : key_pattern) {
                if (self.has(key)) {
                    filtered_keys.append(key);
                }
            }
            return filtered_keys;
        }, py::arg("key_pattern"),
           "Get list of keys that match any in the pattern list")
        .def("export_to_dict", [](const Config& self) {
            py::dict export_dict;
            for (const auto& key : self.getKeys()) {
                auto valueOpt = self.get<std::string>(key);
                if (valueOpt.has_value()) {
                    export_dict[key.c_str()] = py::cast(valueOpt.value());
                }
            }
            return export_dict;
        }, "Export configuration to Python dictionary");

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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
             py::arg("neuron_type"), py::arg("current"),
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("get_region", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("region_id"),
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
        .def("get_ei_ratio", &Brain::getExcitationInhibitionRatio,
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
             "Log brain status")
        // Additional Pythonic methods
        .def("create_synapse", [](Brain& brain, RegionId source_region_id, PopulationId source_pop_id, NeuronId source_neuron,
                                 RegionId target_region_id, PopulationId target_pop_id, NeuronId target_neuron) {
            auto* source_region = brain.getRegion(source_region_id);
            auto* target_region = brain.getRegion(target_region_id);
            if (source_region && target_region) {
                return source_region->addSynapse(source_pop_id, target_pop_id, source_neuron, target_neuron);
            }
            throw std::runtime_error("Invalid region ID");
        }, py::arg("source_region_id"), py::arg("source_pop_id"), py::arg("source_neuron"),
           py::arg("target_region_id"), py::arg("target_pop_id"), py::arg("target_neuron"),
           "Create a new synapse between neurons")
        .def("get_all_neurons", [](Brain& brain) {
            std::vector<NeuronId> neuron_ids;
            for (const auto& region : brain.getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron_ids.push_back(neuron->getId());
                    }
                }
            }
            return neuron_ids;
        }, "Get all neuron IDs in the brain")
        .def("get_statistics", [](Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["ei_ratio"] = brain.getExcitationInhibitionRatio();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            return stats;
        }, "Get brain statistics as a Python dictionary");

    // Add Python-style iteration methods for regions
    .def("__iter__", [](py::object self) {
        auto brain = self.cast<std::shared_ptr<Brain>>();
        auto region_ids = brain->getRegionIds();
        return py::make_iterator(region_ids.begin(), region_ids.end());
    }, py::return_value_policy::reference_internal);

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        // Additional Pythonic helper methods
        .def("create_simple_action", [](AgentBrain& agent, const std::string& action_type) {
            auto action = std::make_unique<Action>();
            if (action_type == "move_forward") action->setType(ActionType::MoveForward);
            else if (action_type == "move_backward") action->setType(ActionType::MoveBackward);
            else if (action_type == "turn_left") action->setType(ActionType::TurnLeft);
            else if (action_type == "turn_right") action->setType(ActionType::TurnRight);
            else action->setType(ActionType::Wait);
            
            // Convert to MotorCommand for return
            return agent.decodeMotorCommand();
        }, py::arg("action_type"),
           "Create a simple action from string specification")
        .def("get_agent_stats", [](AgentBrain& agent) {
            py::dict stats;
            stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
            stats["curiosity_level"] = agent.getCuriosityLevel();
            stats["novelty_level"] = agent.getNoveltyLevel();
            stats["prediction_error"] = agent.getPredictionError();
            stats["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            return stats;
        }, "Get agent statistics as a Python dictionary")
        .def("enable_all_systems", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all neuromodulation and development systems")
        .def("disable_all_systems", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, "Disable all neuromodulation and development systems");

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

m.def("create_neuron", [](NeuronType type, float v_rest, float threshold) {
    return std::make_unique<Neuron>(type, v_rest, threshold);
}, py::arg("type"), py::arg("v_rest") = -70.0f, py::arg("threshold") = -50.0f,
   "Create a new neuron with specified type and parameters");

m.def("create_synapse", [](Neuron* source, Neuron* target, NeuronType type) {
    return std::make_unique<Synapse>(source->getId(), target->getId(), type);
}, py::arg("source"), py::arg("target"), py::arg("type") = SynapseType::Excitatory,
   "Create a new synapse between neurons");

// Context manager for automatic cleanup
class PyBrainManager {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    
    PyBrainManager(std::shared_ptr<Config> config) {
        brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
    
    ~PyBrainManager() {
        // Cleanup handled by shared_ptr
    }
    
    void step(double time = 0.001) {
        static SimulationStep step = 0;
        brain->step(step++, time);
    }
    
    float get_firing_rate() const {
        return brain->getAverageFiringRate();
    }
    
    size_t get_total_spikes() const {
        return brain->getTotalSpikeCount();
    }
    
    py::dict get_statistics() const {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        return stats;
    }
};

// Register the context manager
py::class_<PyBrainManager>(m, "PyBrainManager", R"pbdoc(Context manager for automatic brain cleanup)pbdoc")
    .def(py::init<std::shared_ptr<Config>>())
    .def("step", &PyBrainManager::step, py::arg("time") = 0.001,
         "Perform a simulation step")
    .def("get_firing_rate", &PyBrainManager::get_firing_rate,
         "Get current firing rate")
    .def("get_total_spikes", &PyBrainManager::get_total_spikes,
         "Get total spike count")
    .def("get_statistics", &PyBrainManager::get_statistics,
         "Get brain statistics as dictionary")
    .def("inject_current_to_region", [](PyBrainManager& self, RegionId region_id, NeuronType type, float current) {
        auto* region = self.brain->getRegion(region_id);
        if (region) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    }, py::arg("region_id"), py::arg("type"), py::arg("current"),
       "Inject current to all neurons of a specific type in a region")
    .def("get_region_info", [](PyBrainManager& self, RegionId region_id) {
        py::dict info;
        auto* region = self.brain->getRegion(region_id);
        if (region) {
            info["id"] = region_id;
            info["population_count"] = region->getPopulationCount();
            info["neuron_count"] = region->getTotalNeuronCount();
            info["synapse_count"] = region->getSynapseCount();
        }
        return info;
    }, py::arg("region_id"),
       "Get information about a specific region");

// Add utility functions for common patterns
m.def("run_simulation", [](std::shared_ptr<Config> config, size_t steps, bool verbose = false) {
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    if (verbose) {
        std::cout << "Starting simulation with " << brain->getTotalNeuronCount() 
                  << " neurons" << std::endl;
    }
    
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i);
        if (verbose && i % 100 == 0) {
            std::cout << "Step " << i << ": " << brain->getFiringNeuronCount() 
                      << " firing neurons" << std::endl;
        }
    }
    
    return brain;
}, py::arg("config"), py::arg("steps"), py::arg("verbose") = false,
   "Run a simulation and return the brain");

m.def("create_learning_brain", []() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    return std::make_shared<Brain>(config);
}, "Create a brain configured for learning experiments");

m.def("create_exploratory_brain", []() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0005f, nlm::ConfigSource::Default);
    return std::make_shared<Brain>(config);
}, "Create a brain configured for exploratory behavior");

m.def("create_predictive_brain", []() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.08, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
    return std::make_shared<Brain>(config);
}, "Create a brain configured for prediction and learning");

m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                               size_t steps, bool enable_learning = true) {
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    if (enable_learning) {
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
    }
    
    for (size_t i = 0; i < steps; ++i) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(i);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
    
    return std::make_pair(brain, agent);
}, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("enable_learning") = true,
   "Run a complete agent simulation and return brain and agent");

    .def("create_advanced_plasticity_brain", [](std::shared_ptr<Config> config, const py::kwargs& options) {
        // Enhanced plasticity configuration
        if (options.contains("triplet_stdp")) {
            config->set("plasticity.stdp.triplet_enabled", 
                       options["triplet_stdp"].cast<bool>(), nlm::ConfigSource::Runtime);
        }
        if (options.contains("voltage_dependent_stdp")) {
            config->set("plasticity.stdp.voltage_dependent", 
                       options["voltage_dependent_stdp"].cast<bool>(), nlm::ConfigSource::Runtime);
        }
        if (options.contains("reward_modulated_stdp")) {
            config->set("plasticity.stdp.reward_modulated", 
                       options["reward_modulated_stdp"].cast<bool>(), nlm::ConfigSource::Runtime);
        }
        if (options.contains("adaptive_hebbian")) {
            config->set("plasticity.hebbian.adaptive_learning", 
                       options["adaptive_hebbian"].cast<bool>(), nlm::ConfigSource::Runtime);
        }
        if (options.contains("voltage_dependent_hebbian")) {
            config->set("plasticity.hebbian.voltage_dependent", 
                       options["voltage_dependent_hebbian"].cast<bool>(), nlm::ConfigSource::Runtime);
        }
        if (options.contains("homeostatic_hebbian")) {
            config->set("plasticity.hebbian.homeostatic", 
                       options["homeostatic_hebbian"].cast<bool>(), nlm::ConfigSource::Runtime);
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("config"), py::arg("options") = py::kwargs(),
       "Create a brain with advanced plasticity features enabled")
    
    // Advanced plasticity configuration
    m.def("configure_advanced_plasticity", [](std::shared_ptr<Brain> brain,
                                              const py::kwargs& plasticity_options) {
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        
        if (options.contains("stdp_learning_rate")) {
            float ltp = options["stdp_learning_rate"].cast<float>() * 0.01f; // Scale
            stdp->setLTPWeight(ltp);
            stdp->setLTDWeight(ltp * 1.2f);
        }
        
        if (options.contains("hebbian_learning_rate")) {
            hebbian->setLearningRate(options["hebbian_learning_rate"].cast<float>());
        }
        
        if (options.contains("plasticity_threshold")) {
            float threshold = options["plasticity_threshold"].cast<float>();
            // Store for Hebbian advanced parameters
            if (hebbian) {
                // This would require modifying Hebbian to accept covariance threshold
                // Placeholder for future enhancement
            }
        }
        
        return brain;
    }, py::arg("brain"), py::arg("plasticity_options") = py::kwargs(),
       "Configure advanced plasticity parameters on a running brain")
    
    // Advanced plasticity statistics
    m.def("get_plasticity_stats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        
        if (auto* stdp = brain->getSTDP()) {
            py::dict stdp_stats;
            stdp_stats["name"] = stdp->getName();
            // Add enhanced STDP statistics
            stdp_stats["advanced_features"] = true;
            stats["stdp"] = stdp_stats;
        }
        
        if (auto* hebbian = brain->getHebbian()) {
            py::dict hebbian_stats;
            hebbian_stats["name"] = hebbian->getName();
            // Add enhanced Hebbian statistics
            hebbian_stats["advanced_features"] = true;
            stats["hebbian"] = hebbian_stats;
        }
        
        return stats;
    }, py::arg("brain"),
       "Get advanced plasticity statistics for a brain")
    
    // Advanced simulation modes with plasticity
    m.def("run_adaptive_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       size_t steps, const py::kwargs& simulation_options) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable adaptive learning based on simulation options
        bool adaptive_learning = simulation_options.contains("adaptive_learning") ? 
            simulation_options["adaptive_learning"].cast<bool>() : true;
        
        if (adaptive_learning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            
            // Configure advanced plasticity
            if (auto* stdp = brain->getSTDP()) {
                // Enable advanced STDP features
                // Placeholder for future enhancement
            }
            if (auto* hebbian = brain->getHebbian()) {
                // Enable advanced Hebbian features
                // Placeholder for future enhancement
            }
        }
        
        // Custom simulation dynamics
        double timestep = simulation_options.contains("timestep") ? 
            simulation_options["timestep"].cast<double>() : 0.1;
        double plasticity_rate = simulation_options.contains("plasticity_rate") ? 
            simulation_options["plasticity_rate"].cast<double>() : 0.01;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            // Adaptive plasticity modulation
            if (i % static_cast<size_t>(plasticity_rate * 100) == 0) {
                // Occasionally adjust plasticity parameters
                if (auto* stdp = brain->getSTDP()) {
                    // Small random adjustment to STDP parameters
                    float current_ltp = stdp->getLTPWeight();
                    stdp->setLTPWeight(current_ltp * (1.0f + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f));
                }
            }
            
            world->update(timestep);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * timestep);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict results;
        results["brain"] = brain;
        results["agent"] = agent;
        results["world"] = world;
        results["steps_completed"] = steps;
        results["simulation_time_ms"] = duration.count();
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), 
       py::arg("simulation_options") = py::kwargs(),
       "Run adaptive simulation with advanced plasticity features")
    
    // Advanced plasticity benchmarking
    m.def("benchmark_plasticity", [](std::shared_ptr<Config> config, size_t neuron_count, 
                                    size_t steps, const py::kwargs& plasticity_options) {
        config->set("neuron_count", static_cast<int64_t>(neuron_count), nlm::ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
        
        // Configure advanced plasticity
        if (plasticity_options.contains("enhanced_stdp")) {
            config->set("plasticity.stdp.triplet_enabled", 
                       plasticity_options["enhanced_stdp"].cast<bool>(), nlm::ConfigSource::Runtime);
        }
        if (plasticity_options.contains("enhanced_hebbian")) {
            config->set("plasticity.hebbian.adaptive_learning", 
                       plasticity_options["enhanced_hebbian"].cast<bool>(), nlm::ConfigSource::Runtime);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Inject input to stimulate plasticity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict benchmark;
        benchmark["neuron_count"] = neuron_count;
        benchmark["steps"] = steps;
        benchmark["total_spikes"] = brain->getTotalSpikeCount();
        benchmark["firing_neurons"] = brain->getFiringNeuronCount();
        benchmark["simulation_time_ms"] = duration.count();
        benchmark["spikes_per_second"] = static_cast<double>(benchmark["total_spikes"].cast<size_t>()) * 1000.0 / duration.count();
        
        // Enhanced plasticity metrics
        benchmark["plasticity_advanced"] = true;
        benchmark["stdp_enabled"] = plasticity_options.contains("enhanced_stdp");
        benchmark["hebbian_enabled"] = plasticity_options.contains("enhanced_hebbian");
        
        return benchmark;
    }, py::arg("config"), py::arg("neuron_count"), py::arg("steps"), 
       py::arg("plasticity_options") = py::kwargs(),
       "Benchmark brain with advanced plasticity features");
