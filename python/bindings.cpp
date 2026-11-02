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
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        // Advanced Config features
        .def("create_optimized_config", [](const std::string& profile) {
            auto config = std::make_shared<Config>();
            
            if (profile == "fast") {
                config->set("brain.neuron_count", 500);
                config->set("brain.synapse_density", 0.05);
                config->set("plasticity.stdp.enable", false);
                config->set("neuromod.dopamine.scale", 0.0);
            } else if (profile == "balanced") {
                config->set("brain.neuron_count", 1000);
                config->set("brain.synapse_density", 0.1);
                config->set("plasticity.stdp.enable", true);
                config->set("neuromod.dopamine.scale", 1.0);
            } else if (profile == "deep") {
                config->set("brain.neuron_count", 2000);
                config->set("brain.synapse_density", 0.2);
                config->set("plasticity.stdp.enable", true);
                config->set("plasticity.hebbian.enable", true);
                config->set("plasticity.structural.enable", true);
                config->set("neuromod.dopamine.scale", 1.5);
            }
            
            return config;
        }, py::arg("profile") = "balanced",
             "Create an optimized configuration with predefined settings")
        .def("validate_config", [](Config& self) {
            std::vector<std::string> errors;
            
            // Basic validation checks
            if (self.has("brain.neuron_count")) {
                auto count = self.get<int64_t>("brain.neuron_count");
                if (!count || *count <= 0) {
                    errors.push_back("brain.neuron_count must be positive");
                } else if (*count > 10000) {
                    errors.push_back("brain.neuron_count should not exceed 10000 for performance reasons");
                }
            }
            
            if (self.has("brain.synapse_density")) {
                auto density = self.get<double>("brain.synapse_density");
                if (density && (*density < 0.0 || *density > 1.0)) {
                    errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
                }
            }
            
            if (self.has("random_seed")) {
                auto seed = self.get<int64_t>("random_seed");
                if (seed && *seed < 0) {
                    errors.push_back("random_seed must be non-negative");
                }
            }
            
            return errors;
        }, "Validate configuration and return list of errors, empty if valid")
        .def("merge_configs", [](const std::vector<std::shared_ptr<Config>>& configs) {
            auto merged = std::make_shared<Config>();
            for (const auto& config : configs) {
                auto keys = config->getKeys();
                for (const auto& key : keys) {
                    // Use Runtime source for all merged values
                    if (config->has(key)) {
                        // This is a simplified merge - in practice you'd need type-aware merging
                        merged->set(key, "merged_value", ConfigSource::Runtime);
                    }
                }
            }
            return merged;
        }, py::arg("configs"),
             "Merge multiple configuration objects into one")
        .def("export_to_dict", [](Config& self) {
            std::map<std::string, std::string> exportMap;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Simple string conversion for export
                exportMap[key] = "exported_value";
            }
            return exportMap;
        }, "Export configuration to Python dictionary format")
        .def("import_from_dict", [](Config& self, const std::map<std::string, std::string>& data) {
            self.clear();
            for (const auto& pair : data) {
                self.set(pair.first, pair.second, ConfigSource::Runtime);
            }
            return true;
        }, py::arg("data"),
             "Import configuration from Python dictionary format");

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Advanced SimpleWorld features
        .def("create_environment_simulation", [](std::shared_ptr<SimpleWorld> self, int duration,
                                                 const std::vector<float>& agentStart) {
            std::vector<std::pair<float, float>> agentPath;
            std::vector<std::vector<float>> environmentSnapshots;
            
            self->setAgentStart(agentStart[0], agentStart[1]);
            self->reset();
            
            for (int step = 0; step < duration; ++step) {
                self->update(0.1);
                
                auto body = self->getAgentBody();
                agentPath.push_back({body.x, body.y});
                
                auto percept = self->getSensoryPercept();
                environmentSnapshots.push_back(percept.getVision());
                
                // Add some random objects for exploration
                if (step % 10 == 0) {
                    float x = std::rand() % static_cast<int>(self->getWidth() - 1);
                    float y = std::rand() % static_cast<int>(self->getHeight() - 1);
                    self->addObject(WorldObject(x, y, WorldObjectType::Resource, 1.0f));
                }
            }
            
            return std::make_tuple(agentPath, environmentSnapshots);
        }, py::arg("duration"), py::arg("agentStart"),
             "Run extended environment simulation and return trajectory data")
        .def("analyze_environment", [](std::shared_ptr<SimpleWorld> self) {
            auto percept = self->getSensoryPercept();
            auto body = self->getAgentBody();
            
            std::map<std::string, double> analysis = {
                {"agent_x", body.x},
                {"agent_y", body.y},
                {"agent_orientation", body.orientation},
                {"agent_energy", body.energy},
                {"agent_health", body.health},
                {"agent_age", body.age},
                {"vision_variance", std::accumulate(percept.getVision().begin(), percept.getVision().end(), 0.0f) / percept.getVision().size()},
                {"simulation_time", self->getSimulationTime()},
                {"world_width", self->getWidth()},
                {"world_height", self->getHeight()}
            };
            
            return analysis;
        }, "Analyze current world state and agent condition")
        .def("create_demonstration_scenario", [](std::shared_ptr<SimpleWorld> self, const std::string& scenario) {
            if (scenario == "resource_discovery") {
                self->reset();
                self->setAgentStart(5.0, 5.0);
                self->setMaxEnergy(100.0);
                
                // Place resources throughout the world
                for (int x = 2; x < self->getWidth() - 2; x += 3) {
                    for (int y = 2; y < self->getHeight() - 2; y += 3) {
                        self->addObject(WorldObject(static_cast<float>(x), static_cast<float>(y), 
                                                  WorldObjectType::Resource, 1.0f));
                    }
                }
                
                // Place some hazards
                self->addObject(WorldObject(10.0, 10.0, WorldObjectType::Hazard, 0.5f));
                self->addObject(WorldObject(15.0, 15.0, WorldObjectType::Hazard, 0.5f));
                
                // Place walls
                for (int x = 0; x < self->getWidth(); ++x) {
                    self->addObject(WorldObject(static_cast<float>(x), 0.0f, WorldObjectType::Wall, 0.0f));
                    self->addObject(WorldObject(static_cast<float>(x), self->getHeight() - 1, WorldObjectType::Wall, 0.0f));
                }
                for (int y = 0; y < self->getHeight(); ++y) {
                    self->addObject(WorldObject(0.0f, static_cast<float>(y), WorldObjectType::Wall, 0.0f));
                    self->addObject(WorldObject(self->getWidth() - 1, static_cast<float>(y), WorldObjectType::Wall, 0.0f));
                }
            }
            
            return true;
        }, py::arg("scenario"),
             "Create predefined demonstration scenarios for testing")
        .def("export_data", [](std::shared_ptr<SimpleWorld> self, const std::string& filename) {
            // Export world state to file for external analysis
            auto body = self->getAgentBody();
            auto percept = self->getSensoryPercept();
            
            // Simple CSV export
            std::ofstream file(filename);
            if (file.is_open()) {
                file << "simulation_time,agent_x,agent_y,agent_orientation,agent_energy,agent_health\n";
                file << self->getSimulationTime() << ","
                     << body.x << ","
                     << body.y << ","
                     << body.orientation << ","
                     << body.energy << ","
                     << body.health << "\n";
                file << "\nvision_data,";
                for (size_t i = 0; i < percept.getVision().size(); ++i) {
                    file << percept.getVision()[i];
                    if (i < percept.getVision().size() - 1) file << ",";
                }
                file << "\n";
                file.close();
            }
            
            return filename;
        }, py::arg("filename"),
             "Export world state data to file for analysis")
        .def("import_data", [](std::shared_ptr<SimpleWorld> self, const std::string& filename) {
            // Import world state from file (placeholder implementation)
            std::cout << "Import functionality would load world state from " << filename << std::endl;
            return true;
        }, py::arg("filename"),
             "Import world state from file (implementation depends on file format)");

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        // Advanced Python API methods
        .def("run_batch", [](Brain& self, int steps, Timestamp timeStep = 1.0, bool logStats = false) {
            std::vector<float> firingRates;
            firingRates.reserve(steps);
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * timeStep);
                if (logStats) {
                    firingRates.push_back(self.getAverageFiringRate());
                }
            }
            return firingRates;
        }, py::arg("steps"), py::arg("timeStep") = 1.0, py::arg("logStats") = false,
             "Run multiple simulation steps efficiently and return firing rates")
        .def("get_region_by_name", [](Brain& self, const std::string& name) -> NeuralRegion* {
            auto regions = self.getRegions();
            for (auto* region : regions) {
                if (region->getName() == name) return region;
            }
            return nullptr;
        }, py::arg("name"),
             "Get a region by name, returns None if not found")
        .def("create_optimized_config", [](const std::string& profile) {
            auto config = std::make_shared<Config>();
            
            if (profile == "fast") {
                config->set("brain.neuron_count", 500);
                config->set("brain.synapse_density", 0.05);
                config->set("plasticity.stdp.enable", false);
                config->set("neuromod.dopamine.scale", 0.0);
            } else if (profile == "balanced") {
                config->set("brain.neuron_count", 1000);
                config->set("brain.synapse_density", 0.1);
                config->set("plasticity.stdp.enable", true);
                config->set("neuromod.dopamine.scale", 1.0);
            } else if (profile == "deep") {
                config->set("brain.neuron_count", 2000);
                config->set("brain.synapse_density", 0.2);
                config->set("plasticity.stdp.enable", true);
                config->set("plasticity.hebbian.enable", true);
                config->set("plasticity.structural.enable", true);
                config->set("neuromod.dopamine.scale", 1.5);
            }
            
            return config;
        }, py::arg("profile") = "balanced",
             "Create an optimized configuration with predefined settings")
        .def("profile_performance", [](Brain& self, int steps) {
            struct PerfStats {
                double totalTime;
                size_t stepsPerSecond;
                float avgFiringRate;
                size_t memoryUsage;
            };
            
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < steps; ++i) {
                self.step(i);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            double totalTime = std::chrono::duration<double>(end - start).count();
            size_t stepsPerSecond = steps / totalTime;
            float avgFiringRate = self.getAverageFiringRate();
            // Note: memory usage is approximate and platform-dependent
            size_t memoryUsage = sizeof(self) + self.getTotalNeuronCount() * sizeof(float);
            
            return PerfStats{totalTime, stepsPerSecond, avgFiringRate, memoryUsage};
        }, py::arg("steps"),
             "Profile brain performance over specified steps, returns timing and firing statistics")
        .def("get_development_summary", &Brain::getConfig,
             "Get development configuration for advanced planning")
        .def("enable_advanced_features", [](Brain& self, bool stdp, bool hebbian, bool structural, bool neuromod) {
            auto config = self.getConfig();
            if (config) {
                config->set("plasticity.stdp.enable", stdp);
                config->set("plasticity.hebbian.enable", hebbian);
                config->set("plasticity.structural.enable", structural);
                config->set("neuromod.dopamine.scale", neuromod ? 1.0 : 0.0);
            }
        }, py::arg("stdp") = true, py::arg("hebbian") = true, 
         py::arg("structural") = true, py::arg("neuromod") = true,
             "Enable or disable advanced neural plasticity features");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Advanced AgentBrain features
        .def("create_agent_simulation", [](std::shared_ptr<AgentBrain> self, std::shared_ptr<SimpleWorld> world, 
                                          int steps, bool enableLearning = true) {
            std::vector<std::pair<float, float>> trajectory;
            for (int step = 0; step < steps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                self->processSensoryInput(percept);
                
                auto brain = self->getBrain();
                brain->step(step);
                
                auto action = self->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                if (enableLearning) {
                    auto reward = world->getSensoryPercept().getInternal()[0];
                    self->applyRewardModulation(reward, 0.0);
                    self->updateDevelopment(0.1);
                }
                
                trajectory.push_back({self->getBrain()->getTotalSpikeCount(), 
                                     self->getCuriosityLevel()});
            }
            return trajectory;
        }, py::arg("world"), py::arg("steps"), py::arg("enableLearning") = true,
             "Run a complete agent simulation and return trajectory data")
        .def("analyze_behavior", [](std::shared_ptr<AgentBrain> self) {
            auto brain = self->getBrain();
            std::map<std::string, double> metrics = {
                {"firing_rate", brain->getAverageFiringRate()},
                {"excitatory_inhibition_ratio", brain->getExcitationInhibitionRatio()},
                {"curiosity_level", self->getCuriosityLevel()},
                {"novelty_level", self->getNoveltyLevel()},
                {"prediction_error", self->getPredictionError()},
                {"total_spikes", brain->getTotalSpikeCount()},
                {"developmental_stage", static_cast<int>(brain->getDevelopmentalStage())}
            };
            return metrics;
        },
             "Analyze agent behavior and return performance metrics")
        .def("create_advanced_training", [](std::shared_ptr<AgentBrain> self, std::shared_ptr<SimpleWorld> world,
                                            const std::vector<std::string>& behaviors, int stepsPerBehavior) {
            std::vector<std::string> successfulBehaviors;
            
            for (const auto& behavior : behaviors) {
                std::cout << "Training behavior: " << behavior << std::endl;
                
                for (int step = 0; step < stepsPerBehavior; ++step) {
                    world->update(0.1);
                    
                    // Specialized sensory input based on behavior
                    auto percept = world->getSensoryPercept();
                    self->processSensoryInput(percept);
                    
                    auto brain = self->getBrain();
                    brain->step(step);
                    
                    auto action = self->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    // Apply adaptive learning based on behavior type
                    float reward = 0.0f;
                    if (behavior == "exploration") {
                        reward = self->getCuriosityLevel() * 0.1f;
                    } else if (behavior == "cognition") {
                        reward = self->getPredictionError() * 0.5f;
                    } else if (behavior == "memory") {
                        auto workingMemory = brain->getWorkingMemory();
                        reward = workingMemory ? workingMemory->getMemoryStrength() * 0.3f : 0.0f;
                    }
                    
                    self->applyRewardModulation(reward, 0.0);
                    self->updateDevelopment(0.05);
                }
                
                // Evaluate success
                auto metrics = self->analyze_behavior();
                if (metrics["firing_rate"] > 0.1f && metrics["curiosity_level"] > 0.5f) {
                    successfulBehaviors.push_back(behavior);
                }
            }
            
            return successfulBehaviors;
        }, py::arg("world"), py::arg("behaviors"), py::arg("stepsPerBehavior"),
             "Advanced multi-behavior training system")
        .def("get_sensory_analysis", [](std::shared_ptr<AgentBrain> self) {
            // Note: This is a demonstration - in practice, you'd need access to the world
            // to get current sensory percepts. This shows what the analysis would look like.
            std::map<std::string, std::vector<float>> analysis = {
                {"vision_pattern", std::vector<float>(16, 0.0f)},
                {"touch_pattern", std::vector<float>(8, 0.0f)},
                {"internal_state", std::vector<float>(4, 0.0f)},
                {"proprioception", std::vector<float>(6, 0.0f)}
            };
            
            return analysis;
        }, "Get detailed analysis of current sensory input (requires world context)")

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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // Advanced convenience functions
    m.def("run_agent_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                std::shared_ptr<AgentBrain> agent, int steps) {
        auto startTime = std::chrono::high_resolution_clock::now();
        float totalReward = 0.0f;
        int successfulActions = 0;
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            if (result.success) {
                successfulActions++;
                totalReward += result.reward;
                agent->applyRewardModulation(result.reward, 0.0);
            }
            
            agent->updateDevelopment(0.1);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        return std::make_tuple(totalReward, successfulActions, duration);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
         "Run a complete agent episode and return (total_reward, successful_actions, duration)")

    m.def("create_training_scenario", [](const std::string& scenarioType) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0, 20.0, 16, 16);
        
        if (scenarioType == "resource_collection") {
            // Place resources randomly
            for (int i = 0; i < 10; ++i) {
                float x = static_cast<float>(rand() % static_cast<int>(world->getWidth() - 2) + 1);
                float y = static_cast<float>(rand() % static_cast<int>(world->getHeight() - 2) + 1);
                world->addObject(WorldObject(x, y, WorldObjectType::Resource, 1.0f));
            }
        } else if (scenarioType == "obstacle_course") {
            // Create maze-like obstacles
            for (int x = 5; x < 15; ++x) {
                world->addObject(WorldObject(static_cast<float>(x), 5.0f, WorldObjectType::Wall, 0.0f));
                world->addObject(WorldObject(static_cast<float>(x), 15.0f, WorldObjectType::Wall, 0.0f));
            }
            for (int y = 5; y < 15; ++y) {
                world->addObject(WorldObject(5.0f, static_cast<float>(y), WorldObjectType::Wall, 0.0f));
                world->addObject(WorldObject(15.0f, static_cast<float>(y), WorldObjectType::Wall, 0.0f));
            }
        }
        
        return world;
    }, py::arg("scenarioType"),
         "Create a training scenario with predefined layout")

    m.def("benchmark_brain", [](std::shared_ptr<Brain> brain, int steps, const std::string& profile) {
        auto config = brain->getConfig();
        
        // Backup original settings
        auto originalNeuronCount = config->get<int64_t>("brain.neuron_count");
        auto originalDensity = config->get<double>("brain.synapse_density");
        
        std::cout << "Benchmarking brain with " << profile << " profile..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        float avgFiringRate = brain->getAverageFiringRate();
        size_t totalSpikes = brain->getTotalSpikeCount();
        double stepsPerSecond = steps / duration;
        
        std::cout << "Completed " << steps << " steps in " << duration << " seconds" << std::endl;
        std::cout << "Steps/second: " << stepsPerSecond << std::endl;
        std::cout << "Average firing rate: " << avgFiringRate << std::endl;
        std::cout << "Total spikes: " << totalSpikes << std::endl;
        
        // Restore original settings
        config->set("brain.neuron_count", originalNeuronCount.value_or(1000));
        config->set("brain.synapse_density", originalDensity.value_or(0.1));
        
        return std::make_tuple(duration, stepsPerSecond, avgFiringRate, totalSpikes);
    }, py::arg("brain"), py::arg("steps"), py::arg("profile") = "default",
         "Benchmark brain performance, returns (duration, steps_per_second, avg_firing_rate, total_spikes)")

    m.def("create_development_pipeline", [](int numStages, float transitionTime) {
        std::vector<float> stageTransitions;
        for (int i = 0; i < numStages; ++i) {
            stageTransitions.push_back(i * transitionTime);
        }
        
        return stageTransitions;
    }, py::arg("numStages"), py::arg("transitionTime"),
         "Create developmental stage transitions for advanced brain development")

    // Text and communication utilities
    m.def("encode_text", &AgentBrain::encodeText,
         py::arg("text"),
         "Encode text string into neural pattern for text communication")

    m.def("decode_text", [](const std::vector<size_t>& pattern) {
        // Simple text decoding (placeholder - would need actual mapping)
        return "decoded_text_placeholder";
    }, py::arg("pattern"),
         "Decode neural pattern into text (placeholder implementation)")

    // Constants and utility values
    m.attr("DEFAULT_WORLD_WIDTH") = 20.0f;
    m.attr("DEFAULT_WORLD_HEIGHT") = 20.0f;
    m.attr("DEFAULT_VISION_WIDTH") = 8;
    m.attr("DEFAULT_VISION_HEIGHT") = 8;
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.1;
    m.attr("DEFAULT_NEURON_COUNT") = 1000LL;
    m.attr("DEFAULT_SYNAPSE_DENSITY") = 0.1;

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
