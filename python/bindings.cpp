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

    py::enum_<LogLevel>(m, "LogLevel", R"pbdoc(Log level enumeration)pbdoc")
        .value("Debug", LogLevel::Debug)
        .value("Info", LogLevel::Info)
        .value("Warning", LogLevel::Warning)
        .value("Error", LogLevel::Error)
        .value("Critical", LogLevel::Critical)
        .export_values();

    py::enum_<LogLevel>(m, "LogLevel", R"pbdoc(Log level enumeration)pbdoc")
        .value("Debug", LogLevel::Debug)
        .value("Info", LogLevel::Info)
        .value("Warning", LogLevel::Warning)
        .value("Error", LogLevel::Error)
        .value("Critical", LogLevel::Critical)
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

    // World object factories and advanced utilities
    m.def("createWorldObject", [](float x, float y, WorldObjectType type, float value, float radius) {
        return WorldObject(x, y, type, value, radius);
    }, py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("type") = WorldObjectType::Empty,
       py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
       "Create a world object with specified parameters");
    
    m.def("createResourceObject", [](float x, float y, float value) {
        return WorldObject(x, y, WorldObjectType::Resource, value);
    }, py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("value") = 1.0f,
       "Create a resource object");
    
    m.def("createHazardObject", [](float x, float y, float radius = 0.5f) {
        return WorldObject(x, y, WorldObjectType::Hazard, 1.0f, radius);
    }, py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("radius") = 0.5f,
       "Create a hazard object");
    
    m.def("createWallObject", [](float x, float y, float width = 1.0f, float height = 1.0f) {
        return WorldObject(x, y, WorldObjectType::Wall, 1.0f, std::max(width, height));
    }, py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("width") = 1.0f, py::arg("height") = 1.0f,
       "Create a wall object");
    
    // Advanced sensory input utilities
    m.def("createVisionInput", [](size_t width, size_t height, size_t channels, const std::vector<float>& data) {
        auto vision = std::make_unique<Vision>(width, height, channels);
        vision->setData(data);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, py::arg("data") = std::vector<float>(),
       "Create a vision sensory input with specified dimensions and data");
    
    m.def("createAudioInput", [](size_t sampleRate, const std::vector<float>& samples) {
        auto audio = std::make_unique<Audio>(sampleRate, samples.size());
        audio->setData(samples);
        return audio;
    }, py::arg("sampleRate"), py::arg("samples"), py::arg("data") = std::vector<float>(),
       "Create an audio sensory input");
    
    m.def("createInternalSignals", [](const std::vector<float>& signals) {
        auto internal = std::make_unique<InternalSignals>();
        internal->clearSignals();
        for (float signal : signals) {
            internal->addSignal(signal);
        }
        return internal;
    }, py::arg("signals") = std::vector<float>(),
       "Create internal signals sensory input");
    
    // Advanced action utilities
    m.def("createAction", [](ActionType type, const std::vector<float>& parameters) {
        auto action = std::make_unique<Action>(type, parameters);
        return action;
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
       "Create an action with type and parameters");
    
    m.def("createMovementAction", [](ActionType type, float speed, float angle) {
        std::vector<float> params = {speed, angle};
        auto action = std::make_unique<Action>(type, params);
        return action;
    }, py::arg("type") = ActionType::MoveForward, py::arg("speed") = 1.0f, py::arg("angle") = 0.0f,
       "Create a movement action");
    
    m.def("createInteractionAction", [](const std::string& targetObject, const std::string& actionName) {
        std::vector<float> params = {static_cast<float>(std::hash<std::string>{}(targetObject)), 
                                     static_cast<float>(std::hash<std::string>{}(actionName))};
        auto action = std::make_unique<Action>(ActionType::Interact, params);
        return action;
    }, py::arg("targetObject") = std::string(""), py::arg("actionName") = std::string(""),
       "Create an interaction action");
    
    // Brain configuration utilities
    m.def("createAdvancedConfig", [](const std::string& description) {
        auto config = std::make_shared<Config>();
        config->set("config.description", description, ConfigSource::Runtime);
        return config;
    }, py::arg("description") = std::string("Advanced NLM configuration"),
       "Create an advanced configuration with description");
    
    m.def("configureBrain", [](std::shared_ptr<Brain> brain, const std::unordered_map<std::string, std::variant<int, double, bool, std::string>> settings) {
        for (const auto& setting : settings) {
            if (std::holds_alternative<int>(setting.second)) {
                brain->getConfig()->set(setting.first, std::get<int>(setting.second), ConfigSource::Runtime);
            } else if (std::holds_alternative<double>(setting.second)) {
                brain->getConfig()->set(setting.first, std::get<double>(setting.second), ConfigSource::Runtime);
            } else if (std::holds_alternative<bool>(setting.second)) {
                brain->getConfig()->set(setting.first, std::get<bool>(setting.second), ConfigSource::Runtime);
            } else if (std::holds_alternative<std::string>(setting.second)) {
                brain->getConfig()->set(setting.first, std::get<std::string>(setting.second), ConfigSource::Runtime);
            }
        }
    }, py::arg("brain"), py::arg("settings"),
       "Configure brain with advanced settings dictionary");
    
    // Agent utilities
    m.def("createAdvancedAgent", [](std::shared_ptr<Brain> brain) {
        auto agent = std::make_shared<AgentBrain>(brain);
        return agent;
    }, py::arg("brain"), "Create an advanced agent brain interface");
    
    m.def("configureAgentAdvanced", [](std::shared_ptr<AgentBrain> agent, 
                                       bool rewardMod, bool structuralPlastic, 
                                       bool development, bool curiosity,
                                       float noveltyDecay) {
        agent->enableRewardModulation(rewardMod);
        agent->enableStructuralPlasticity(structuralPlastic);
        agent->enableDevelopment(development);
        agent->enableCuriosity(curiosity);
        // Note: sensoryNoveltyDecay_ is not directly exposed but can be modified internally
    }, py::arg("agent"), py::arg("rewardModulation") = true, 
       py::arg("structuralPlasticity") = true, py::arg("development") = true,
       py::arg("curiosity") = true, py::arg("noveltyDecay") = 0.99f,
       "Configure agent with advanced settings");
    
    // World utilities
    m.def("createWorldAdvanced", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       "Create an advanced world with configuration");
    
    // Simulation utilities
    m.def("runSimulationAdvanced", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                     std::shared_ptr<AgentBrain> agent, size_t steps, 
                                     const std::function<bool(size_t, const SensoryPercept&)>& stopCondition) {
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            auto internal = percept.getInternal();
            float reward = internal.empty() ? 0.0f : internal[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Check stop condition
            if (stopCondition && stopCondition(step, percept)) {
                break;
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps") = 1000,
       py::arg("stopCondition") = nullptr,
       "Run an advanced simulation with customizable stop condition");
    
    // Statistics and analysis utilities
    m.def("analyzeBrainStats", [](std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["exc_inh_ratio"] = brain->getExcitationInhibitionRatio();
        return stats;
    }, py::arg("brain"), "Analyze and return brain statistics as dictionary");
    
    m.def("printDetailedStats", [](std::shared_ptr<Brain> brain) {
        std::cout << "=== Detailed Brain Statistics ===" << std::endl;
        std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        brain->logStatus();
    }, py::arg("brain"), "Print detailed brain statistics");
    
    // Debug and monitoring utilities
    m.def("brainDebugReport", [](std::shared_ptr<Brain> brain) {
        std::cout << "=== BRAIN DEBUG REPORT ===" << std::endl;
        std::cout << "Configuration Summary:" << std::endl;
        brain->getConfig()->summary();
        std::cout << std::endl << "Region Information:" << std::endl;
        for (auto regionId : brain->getRegionIds()) {
            auto region = brain->getRegion(regionId);
            std::cout << "  Region " << regionId << ": " 
                     << region->getPopulationCount() << " populations, "
                     << region->getNeuronCount() << " neurons" << std::endl;
        }
        std::cout << std::endl << "Advanced Memory Status:" << std::endl;
        auto workingMem = brain->getWorkingMemory();
        auto episodicMem = brain->getEpisodicMemory();
        auto associativeMem = brain->getAssociativeMemory();
        std::cout << "  Working Memory Active Patterns: " << (workingMem ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Episodic Memory Capacity: " << (episodicMem ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Associative Memory Connections: " << (associativeMem ? "Enabled" : "Disabled") << std::endl;
        std::cout << std::endl << "Prediction System Status:" << std::endl;
        auto predSys = brain->getPredictionSystem();
        std::cout << "  Prediction Error: " << (predSys ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Novelty Detection: " << (brain->getNovelty() ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Curiosity System: " << (brain->getCuriosity() ? "Enabled" : "Disabled") << std::endl;
    }, py::arg("brain"), "Generate comprehensive debug report for brain");
    
    // Recording and data export utilities
    m.def("exportBrainData", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        // Create a simple data export format
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# NLM Brain Data Export" << std::endl;
            file << "# Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
            file << "# Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
            file << "# Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
            file << "# Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
            file << "# Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            file << std::endl;
            
            // Export region information
            file << "Region Data:" << std::endl;
            for (auto regionId : brain->getRegionIds()) {
                auto region = brain->getRegion(regionId);
                file << "  Region " << regionId << ": " << region->getNeuronCount() << " neurons, "
                     << region->getSynapseCount() << " synapses" << std::endl;
            }
            
            file.close();
            return true;
        }
        return false;
    }, py::arg("brain"), py::arg("filename"), "Export brain data to file");
    
    // Advanced simulation utilities
    m.def("createRandomSimulation", [](std::shared_ptr<Config> config, size_t steps) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable advanced features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        std::cout << "Starting random simulation with " << steps << " steps..." << std::endl;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = 0.0f;
            if (!percept.getInternal().empty()) {
                reward = percept.getInternal()[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            
            agent->updateDevelopment(0.1);
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        return brain;
    }, py::arg("config") = nullptr, py::arg("steps") = 1000, "Create and run a random simulation");
    
    // Configuration file utilities
    m.def("saveConfigAdvanced", [](std::shared_ptr<Config> config, const std::string& filename) {
        config->saveToFile(filename);
    }, py::arg("config"), py::arg("filename"), "Save configuration to advanced format");
    
    m.def("loadConfigAdvanced", [](const std::string& filename) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filename);
        return config;
    }, py::arg("filename"), "Load configuration from advanced format");

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
}

} // namespace nlm
