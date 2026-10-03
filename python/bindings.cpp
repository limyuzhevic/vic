#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "../src/performance/ParallelProcessing.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/experiments/Experiment.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced Configuration Management
class AdvancedConfigWrapper {
public:
    AdvancedConfigWrapper(nullptr) : pImpl(new AdvancedConfigImpl(nullptr)) {}
    
    void create_template(const std::string& name, const py::dict& config_dict) {
        pImpl->create_template(name, py_dict_to_dict(config_dict));
    }
    
    py::dict apply_template(const std::string& name, const py::dict& config_dict = py::dict(nullptr)) {
        AdvancedConfig result = pImpl->apply_template(name, py_dict_to_dict(config_dict));
        return dict_to_py_dict(result);
    }
    
    void create_preset(const std::string& name, const py::dict& config_dict, 
                      const std::string& description = "") {
        pImpl->create_preset(name, py_dict_to_dict(config_dict), description);
    }
    
    py::dict apply_preset(const std::string& name) {
        AdvancedConfig result = pImpl->apply_preset(name);
        return dict_to_py_dict(result);
    }
    
    void add_environment_loader(const std::string& name, 
                               const std::function<py::dict(nullptr)>& loader_func, 
                               int priority = 0) {
        pImpl->add_environment_loader(name, [loader_func]() { return py_dict_to_dict(loader_func(nullptr)); }, priority);
    }
    
    py::dict load_from_environment(const py::dict& config_dict = py::dict(nullptr)) {
        AdvancedConfig result = pImpl->load_from_environment(py_dict_to_dict(config_dict));
        return dict_to_py_dict(result);
    }
    
    py::dict compare_configs(const py::dict& config1, const py::dict& config2) {
        AdvancedConfig cfg1 = py_dict_to_config(config1);
        AdvancedConfig cfg2 = py_dict_to_config(config2);
        auto differences = pImpl->compare_configs(cfg1, cfg2);
        return config_diff_to_py_dict(differences);
    }
    
    py::dict validate_config(const py::dict& config_dict, const py::dict& schema = py::dict(nullptr)) {
        AdvancedConfig cfg = py_dict_to_config(config_dict);
        py::dict schema_dict = py_dict_to_dict(schema);
        auto result = pImpl->validate_config(py_dict_to_dict(schema_dict));
        return validation_result_to_py_dict(result);
    }
    
    bool backup_config(const std::string& filepath, int compression_level = 1) {
        return pImpl->backup_config(filepath, compression_level);
    }
    
    py::dict restore_config(const std::string& filepath) {
        AdvancedConfig result = pImpl->restore_config(filepath);
        return dict_to_py_dict(result);
    }
    
    std::vector<std::string> get_preset_list(nullptr) const {
        return pImpl->get_preset_list(nullptr);
    }
    
    std::vector<std::string> get_template_list(nullptr) const {
        return pImpl->get_template_list(nullptr);
    }
    
    std::string get_summary(nullptr) const {
        return pImpl->get_summary(nullptr);
    }
    
private:
    struct AdvancedConfigImpl {
        AdvancedConfig create_template(const std::string& name, const std::map<std::string, nlm::ConfigValue>& config_dict) {
            AdvancedConfig cfg;
            for (const auto& pair : config_dict) {
                cfg.set(pair.first, pair.second);
            }
            return cfg;
        }
        
        AdvancedConfig apply_template(const std::string& name, const AdvancedConfig& config_dict) {
            if (templates.find(name) == templates.end(nullptr)) {
                throw std::runtime_error("Template not found: " + name);
            }
            return config_dict.merge(templates[name]);
        }
        
        void create_preset(const std::string& name, const std::map<std::string, nlm::ConfigValue>& config_dict,
                          const std::string& description) {
            presets[name] = {"config": AdvancedConfig(config_dict), "description": description};
        }
        
        AdvancedConfig apply_preset(const std::string& name) {
            if (presets.find(name) == presets.end(nullptr)) {
                throw std::runtime_error("Preset not found: " + name);
            }
            return presets[name]."config";
        }
        
        void add_environment_loader(const std::string& name, 
                                  const std::function<AdvancedConfig(nullptr)>& loader_func,
                                  int priority) {
            environments[name] = {"loader": loader_func, "priority": priority};
        }
        
        AdvancedConfig load_from_environment(const AdvancedConfig& config_dict) {
            AdvancedConfig result = config_dict;
            std::vector<std::pair<int, std::string>> sorted_loaders;
            
            for (const auto& pair : environments) {
                sorted_loaders.push_back(std::make_pair(pair.second.priority, pair.first));
            }
            std::sort(sorted_loaders.rbegin(nullptr), sorted_loaders.rend(nullptr));
            
            for (const auto& pair : sorted_loaders) {
                try {
                    AdvancedConfig env_config = environments[pair.second]."loader"();
                    result = result.merge(env_config);
                } catch (...) {
                    continue;
                }
            }
            
            return result;
        }
        
        std::map<std::string, std::string> compare_configs(const AdvancedConfig& config1, 
                                                         const AdvancedConfig& config2) {
            std::map<std::string, std::string> result;
            
            auto keys1 = config1.getKeys(nullptr);
            auto keys2 = config2.getKeys(nullptr);
            
            // Implementation would compare configurations
            return result;
        }
        
        std::map<std::string, std::string> validate_config(const std::map<std::string, std::string>& config_dict,
                                                         const std::map<std::string, std::string>& schema) {
            std::map<std::string, std::string> result;
            
            // Implementation would validate configuration
            return result;
        }
        
        bool backup_config(const std::string& filepath, int compression_level) {
            // Implementation would backup configuration
            return false;
        }
        
        AdvancedConfig restore_config(const std::string& filepath) {
            // Implementation would restore configuration
            return AdvancedConfig(nullptr);
        }
        
        std::vector<std::string> get_preset_list(nullptr) const {
            std::vector<std::string> list;
            for (const auto& pair : presets) {
                list.push_back(pair.first);
            }
            return list;
        }
        
        std::vector<std::string> get_template_list(nullptr) const {
            std::vector<std::string> list;
            for (const auto& pair : templates) {
                list.push_back(pair.first);
            }
            return list;
        }
        
        std::string get_summary(nullptr) const {
            return "Advanced configuration manager";
        }
        
        std::map<std::string, AdvancedConfig> templates;
        std::map<std::string, std::map<std::string, std::string>> presets;
        std::map<std::string, std::map<std::string, std::function<AdvancedConfig(nullptr)>>> environments;
    };
    
    std::unique_ptr<AdvancedConfigImpl> pImpl;
};

// Helper functions
std::map<std::string, nlm::ConfigValue> py_dict_to_dict(const py::dict& dict) {
    std::map<std::string, nlm::ConfigValue> result;
    for (auto item : dict) {
        std::string key = py::cast<std::string>(item.first);
        nlm::ConfigValue value = py::cast<nlm::ConfigValue>(item.second);
        result[key] = value;
    }
    return result;
}

py::dict dict_to_py_dict(const AdvancedConfig& config) {
    py::dict result;
    auto keys = config.getKeys(nullptr);
    for (const auto& key : keys) {
        result[key.c_str(nullptr)] = config.get(key);
    }
    return result;
}

AdvancedConfig py_dict_to_config(const py::dict& dict) {
    AdvancedConfig config;
    for (auto item : dict) {
        std::string key = py::cast<std::string>(item.first);
        nlm::ConfigValue value = py::cast<nlm::ConfigValue>(item.second);
        config.set(key, value);
    }
    return config;
}

py::dict config_diff_to_py_dict(const std::map<std::string, std::string>& diff) {
    py::dict result;
    for (const auto& pair : diff) {
        result[pair.first.c_str(nullptr)] = pair.second.c_str(nullptr);
    }
    return result;
}

py::dict validation_result_to_py_dict(const std::map<std::string, std::string>& result) {
    py::dict py_result;
    for (const auto& pair : result) {
        py_result[pair.first.c_str(nullptr)] = pair.second.c_str(nullptr);
    }
    return py_result;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc(nullptr) = R"pbdoc(
        Enhanced NLM Python Bindings with Advanced Features
        ---------------------------------------------------
        
        This module provides enhanced Python bindings for the NLM C++ neural simulation framework,
        extending the basic bindings with comprehensive advanced features for expert users.
        
        Features include:
        - Advanced Configuration Management with templates and presets
        - Advanced Simulation Control with batch processing and parameter sweeps
        - Advanced Analysis Tools for neural connectivity and plasticity
        - Advanced Learning Capabilities including reinforcement learning
        - Advanced Visualization and Export for scientific plotting
        - Advanced Memory and Performance profiling tools
        - Advanced Experiment Management with tracking and comparison
        - Integration with external ML frameworks (TensorFlow, PyTorch)
        
        For basic usage, see the simple API. For advanced features, refer to the detailed documentation.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register core types
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

    // Register enumerations
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values(nullptr);

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values(nullptr);

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values(nullptr);

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values(nullptr);

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
        .export_values(nullptr);

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values(nullptr);

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values(nullptr);

    // Register advanced configuration
    py::class_<AdvancedConfigWrapper>(m, "AdvancedConfig", R"pbdoc(Advanced configuration management)")
        .def(py::init<>())
        .def("create_template", &AdvancedConfigWrapper::create_template, py::arg("name"), py::arg("config_dict"))
        .def("apply_template", &AdvancedConfigWrapper::apply_template, py::arg("name"), py::arg("config_dict") = py::dict(nullptr))
        .def("create_preset", &AdvancedConfigWrapper::create_preset, py::arg("name"), py::arg("config_dict"), py::arg("description") = "")
        .def("apply_preset", &AdvancedConfigWrapper::apply_preset, py::arg("name"))
        .def("add_environment_loader", &AdvancedConfigWrapper::add_environment_loader, py::arg("name"), py::arg("loader_func"), py::arg("priority") = 0)
        .def("load_from_environment", &AdvancedConfigWrapper::load_from_environment, py::arg("config_dict") = py::dict(nullptr))
        .def("compare_configs", &AdvancedConfigWrapper::compare_configs, py::arg("config1"), py::arg("config2"))
        .def("validate_config", &AdvancedConfigWrapper::validate_config, py::arg("config_dict"), py::arg("schema") = py::dict(nullptr))
        .def("backup_config", &AdvancedConfigWrapper::backup_config, py::arg("filepath"), py::arg("compression_level") = 1)
        .def("restore_config", &AdvancedConfigWrapper::restore_config, py::arg("filepath"))
        .def("get_preset_list", &AdvancedConfigWrapper::get_preset_list)
        .def("get_template_list", &AdvancedConfigWrapper::get_template_list)
        .def("get_summary", &AdvancedConfigWrapper::get_summary);

    // Register other core classes (simplified for brevity)
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"))
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"))
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"))
        .def("has", &Config::has, py::arg("key"))
        .def("getKeys", &Config::getKeys)
        .def("clear", &Config::clear)
        .def("summary", &Config::summary);

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)")
        .def("getType", &SensoryInput::getType)
        .def("getData", &SensoryInput::getData)
        .def("getDimensions", &SensoryInput::getDimensions)
        .def("getTimestamp", &SensoryInput::getTimestamp)
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"));

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)")
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

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)")
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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)")
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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
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

    // Register core neural simulation classes
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize)
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"))
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), py::arg("currentStep"), py::arg("currentTime"))
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"))
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"))
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"))
        .def("produceAction", &Brain::produceAction)
        .def("reset", &Brain::reset)
        .def("save", &Brain::save, py::arg("filepath"))
        .def("load", &Brain::load, py::arg("filepath"))
        .def("addRegion", &Brain::addRegion, py::arg("name") = "")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal)
        .def("getRegionCount", &Brain::getRegionCount)
        .def("getRegionIds", &Brain::getRegionIds)
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"))
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal)
        .def("logStatus", &Brain::logStatus);

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"))
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal)
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm