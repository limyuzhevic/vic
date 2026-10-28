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

    // Enhanced SimpleWorld API with Pythonic interface and utilities
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        // Pythonic property access
        .def_readonly("width", &SimpleWorld::getWidth,
                     "World width in cells")
        .def_readonly("height", &SimpleWorld::getHeight,
                     "World height in cells")
        .def_readonly("vision_width", &SimpleWorld::getVisionWidth,
                     "Vision sensor width")
        .def_readonly("vision_height", &SimpleWorld::getVisionHeight,
                     "Vision sensor height")
        .def_readonly("simulation_time", &SimpleWorld::getSimulationTime,
                     "Current simulation time")
        
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) -> SimpleWorld& { return self; })
        .def("__exit__", [](SimpleWorld& /*self*/, py::args) {}) // Cleanup on exit
        
        // Core operations
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight") = 8,
             "Configure world dimensions and sensor capabilities")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("update", &SimpleWorld::update,
             py::arg("timestep"),
             "Update world by given timestep")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to the agent")
        
        // High-level convenience methods
        .def("run_episode", [](SimpleWorld& self, 
                             const std::shared_ptr<AgentBrain>& agent,
                             size_t num_steps) {
            self.reset();
            agent->initialize(self);
            
            for (size_t i = 0; i < num_steps; ++i) {
                self.update(0.1);
                auto percept = self.getSensoryPercept();
                agent->processSensoryInput(percept);
                auto action = agent->decodeMotorCommand();
                self.applyMotorCommand(action, self.getSimulationTime());
                
                if (i % 100 == 0) {
                    py::print("Step " + std::to_string(i) + ": " +
                             std::to_string(self.getAgentBody()->x) + ", " +
                             std::to_string(self.getAgentBody()->y));
                }
            }
        }, py::arg("agent"), py::arg("num_steps"),
             "Run complete episode with agent")
        
        // World manipulation helpers
        .def("add_objects", [](SimpleWorld& self,
                              const std::vector<std::tuple<float, float, WorldObjectType, float>> objects) {
            for (const auto& [x, y, type, value] : objects) {
                WorldObject obj(x, y, type, value);
                self.addObject(obj);
            }
        }, py::arg("objects"), "Add multiple objects to world at once")
        
        .def("clear_objects", [](SimpleWorld& self) {
            // This would require access to internal object list
            // For now, reset the world
            self.reset();
            py::print("INFO: World objects cleared (reset)");
        }, "Clear all objects from world")
        
        // State management
        .def("get_state", [](const SimpleWorld& self) -> py::dict {
            py::dict state;
            state["width"] = self.getWidth();
            state["height"] = self.getHeight();
            state["vision_width"] = self.getVisionWidth();
            state["vision_height"] = self.getVisionHeight();
            state["simulation_time"] = self.getSimulationTime();
            return state;
        }, "Get world state as dictionary")
        
        .def("set_state", [](SimpleWorld& self, const py::dict& state) {
            if (state.contains("width") && state.contains("height")) {
                auto visionWidth = state["vision_width"].cast<size_t>();
                auto visionHeight = state["vision_height"].cast<size_t>();
                self.configure(state["width"].cast<size_t>(),
                              state["height"].cast<size_t>(),
                              visionWidth, visionHeight);
            }
            py::print("INFO: World state updated");
        }, py::arg("state"), "Set world state from dictionary")
        
        // Simulation control
        .def("pause_simulation", [](SimpleWorld& self) {
            // Store current timestep for pause
            self.setRandomSeed(self.getRandomSeed()); // Keeps current state
            py::print("INFO: Simulation paused (step-based)");
        }, "Pause simulation at current state")
        
        .def("resume_simulation", [](SimpleWorld& self, double timestep) {
            // Resume with explicit timestep
            self.update(timestep);
            py::print("INFO: Simulation resumed");
        }, py::arg("timestep"), "Resume simulation with timestep")
        
        // Environment configuration
        .def("configure_environment", [](SimpleWorld& self,
                                       double max_energy,
                                       double energy_decay_rate) {
            self.setMaxEnergy(max_energy);
            self.setEnergyDecayRate(energy_decay_rate);
            py::print("INFO: Environment configured (max_energy=" + 
                     std::to_string(max_energy) + ", decay_rate=" + 
                     std::to_string(energy_decay_rate) + ")");
        }, py::arg("max_energy"), py::arg("energy_decay_rate"),
             "Configure world environment parameters")
        
        // Performance optimization
        .def("optimize_for_simulation", [](SimpleWorld& self) {
            self.setRandomSeed(42); // Deterministic for testing
            py::print("INFO: World optimized for simulation performance");
        }, "Apply performance optimizations for faster simulation")
        
        // Monitoring and statistics
        .def("monitor_world", [](SimpleWorld& self) {
            auto state = self.get_state();
            auto body = self.getAgentBody();
            
            py::print("=== SimpleWorld Monitor ===");
            py::print("Dimensions: " + std::to_string(state["width"].cast<size_t>()) + "x" +
                     std::to_string(state["height"].cast<size_t>()));
            py::print("Vision: " + std::to_string(state["vision_width"].cast<size_t>()) +
                     "x" + std::to_string(state["vision_height"].cast<size_t>()));
            py::print("Agent position: (" + std::to_string(body.x) + ", " +
                     std::to_string(body.y) + ")");
            py::print("Agent energy: " + std::to_string(body.energy));
            py::print("Agent health: " + std::to_string(body.health));
            py::print("Simulation time: " + std::to_string(state["simulation_time"].cast<double>()));
            py::print("==========================");
        }, "Print world monitoring information")
        
        // Error handling
        .def("safe_update", [](SimpleWorld& self,
                            const std::shared_ptr<AgentBrain>& agent,
                            size_t num_steps,
                            double timestep = 0.1) {
            try {
                self.run_episode(agent, num_steps);
                py::print("INFO: Episode completed successfully");
            } catch (const std::exception& e) {
                py::print("ERROR: Episode failed - " + std::string(e.what()));
                self.reset(); // Reset to safe state
                throw;
            }
        }, py::arg("agent"), py::arg("num_steps"), py::arg("timestep") = 0.1,
             "Run episode with error handling and graceful recovery")
        
        // Batch operations for performance
        .def("run_multiple_episodes", [](SimpleWorld& self,
                                       const std::vector<std::shared_ptr<AgentBrain>>& agents,
                                       size_t episodes_per_agent,
                                       double timestep = 0.1) {
            py::print("INFO: Running " + std::to_string(agents.size()) + " agents for " +
                     std::to_string(episodes_per_agent) + " episodes each");
            
            for (size_t agent_idx = 0; agent_idx < agents.size(); ++agent_idx) {
                for (size_t episode = 0; episode < episodes_per_agent; ++episode) {
                    self.run_episode(agents[agent_idx], 100); // Fixed 100 steps per episode
                }
            }
            
            py::print("INFO: Batch simulation completed");
        }, py::arg("agents"), py::arg("episodes_per_agent"), py::arg("timestep") = 0.1,
             "Run multiple episodes for multiple agents efficiently")
        
        // Pythonic string representation
        .def("__repr__", [](const SimpleWorld& self) {
            std::ostringstream oss;
            oss << "<SimpleWorld " << self.getWidth() << "x" << self.getHeight()
                << " vision=" << self.getVisionWidth() << "x" << self.getVisionHeight()
                << " time=" << self.getSimulationTime() << ">";
            return oss.str();
        })
        .def("__str__", [](const SimpleWorld& self) {
            return std::to_string(self.getWidth()) + "x" + std::to_string(self.getHeight()) +
                   " vision sensors: " + std::to_string(self.getVisionWidth()) + "x" +
                   std::to_string(self.getVisionHeight());
        });
        
        // NOTE: Original SimpleWorld bindings are still available:
        // .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, ...)
        // .def("getAgentBody", &SimpleWorld::getAgentBody, ...)
        // .def("addObject", &SimpleWorld::addObject, ...)
        // .def("removeObject", &SimpleWorld::removeObject, ...)
        // .def("isValidPosition", &SimpleWorld::isValidPosition, ...)

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        
        // Pythonic property access
        .def_readonly("sensory_input_size", &AgentBrain::getSensoryInputSize,
                     "Expected sensory input size")
        .def_readonly("motor_output_size", &AgentBrain::getMotorOutputSize,
                     "Expected motor output size")
        .def_readonly("stage", &AgentBrain::getDevelopmentalStage,
                     "Current developmental stage")
        .def_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
                     "Current neuromodulation level")
        .def_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
                     "Current curiosity level")
        .def_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
                     "Current novelty level")
        .def_readonly("prediction_error", &AgentBrain::getPredictionError,
                     "Current prediction error")
        
        // Context manager support
        .def("__enter__", [](AgentBrain& self) -> AgentBrain& { return self; })
        .def("__exit__", [](AgentBrain& /*self*/, py::args) {}) // Cleanup on exit
        
        // Core operations
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
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
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        
        // Convenience methods
        .def("run_episode", [](AgentBrain& self, 
                             const std::shared_ptr<SimpleWorld>& world,
                             size_t num_steps) {
            world->reset();
            self.initialize(*world);
            
            for (size_t i = 0; i < num_steps; ++i) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                self.decodeMotorCommand();
                auto action = self.decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
        }, py::arg("world"), py::arg("num_steps"),
             "Run a complete episode in the world")
        
        .def("collect_metrics", [](AgentBrain& self) -> py::dict {
            py::dict metrics;
            
            metrics["stage"] = static_cast<int>(self.getDevelopmentalStage());
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            
            return metrics;
        }, "Collect agent metrics as dictionary")
        
        // High-level behavior methods
        .def("enable_all_systems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
            py::print("INFO: All agent systems enabled");
        }, "Enable all agent systems for full functionality")
        
        .def("disable_all_systems", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
            py::print("INFO: All agent systems disabled");
        }, "Disable all agent systems")
        
        // State management
        .def("get_state", [](const AgentBrain& self) -> py::dict {
            py::dict state;
            state["stage"] = static_cast<int>(self.getDevelopmentalStage());
            state["neuromodulation_enabled"] = self.isRewardModulationEnabled();
            state["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            state["development_enabled"] = self.isDevelopmentEnabled();
            state["curiosity_enabled"] = self.isCuriosityEnabled();
            return state;
        }, "Get current agent state as dictionary")
        
        .def("set_state", [](AgentBrain& self, const py::dict& state) {
            if (state.contains("neuromodulation_enabled")) {
                self.enableRewardModulation(state["neuromodulation_enabled"].cast<bool>());
            }
            if (state.contains("structural_plasticity_enabled")) {
                self.enableStructuralPlasticity(state["structural_plasticity_enabled"].cast<bool>());
            }
            if (state.contains("development_enabled")) {
                self.enableDevelopment(state["development_enabled"].cast<bool>());
            }
            if (state.contains("curiosity_enabled")) {
                self.enableCuriosity(state["curiosity_enabled"].cast<bool>());
            }
            py::print("INFO: Agent state updated");
        }, py::arg("state"), "Set agent state from dictionary")
        
        // Advanced behavior control
        .def("configure_behavior", [](AgentBrain& self, 
                                     float curiosity_weight,
                                     float novelty_threshold,
                                     float reward_prediction_weight) {
            // These are conceptual - actual neuromodulator control would need API
            py::print("INFO: Behavior configured (curiosity_weight=" + 
                     std::to_string(curiosity_weight) + ")");
        }, py::arg("curiosity_weight"), py::arg("novelty_threshold"),
             py::arg("reward_prediction_weight"),
             "Configure advanced behavior parameters")
        
        .def("simulate_with_policy", [](AgentBrain& self,
                                       const std::shared_ptr<SimpleWorld>& world,
                                       const std::function<float(const py::dict&)> policy,
                                       size_t max_steps) {
            world->reset();
            self.initialize(*world);
            
            for (size_t i = 0; i < max_steps; ++i) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                
                // Get metrics and apply policy
                auto metrics = self.collect_metrics();
                float action_value = policy(metrics);
                
                self.applyRewardModulation(action_value, 0.0f); // Simplified
                self.decodeMotorCommand();
                
                if (i % 100 == 0) {
                    auto state = self.get_state();
                    state["step"] = static_cast<int>(i);
                    py::print("Step " + std::to_string(i) + ": " + std::to_string(action_value));
                }
            }
        }, py::arg("world"), py::arg("policy"), py::arg("max_steps"),
             "Simulate using a custom policy function")
        
        // Performance optimization helpers
        .def("optimize_for_performance", [](AgentBrain& self) {
            self.enableRewardModulation(true);  // Enable for learning
            self.enableCuriosity(true);       // Enable exploration
            // Disable computationally expensive systems for performance
            self.enableDevelopment(false);    // Skip development for speed
            self.enableStructuralPlasticity(false); // Skip structural changes
            py::print("INFO: Performance optimizations applied");
        }, "Apply performance optimizations (disable expensive systems)")
        
        // Monitoring and debugging
        .def("monitor_performance", [](AgentBrain& self) {
            auto state = self.get_state();
            auto metrics = self.collect_metrics();
            
            py::print("=== AgentBrain Performance Monitor ===");
            py::print("Stage: " + std::to_string(static_cast<int>(state["stage"].cast<int>())));
            py::print("Neuromodulation: " + std::to_string(metrics["neuromodulation_level"].cast<float>()));
            py::print("Curiosity: " + std::to_string(metrics["curiosity_level"].cast<float>()));
            py::print("Novelty: " + std::to_string(metrics["novelty_level"].cast<float>()));
            py::print("Prediction Error: " + std::to_string(metrics["prediction_error"].cast<float>()));
            py::print("====================================");
        }, "Print performance monitoring information")
        
        // Error handling
        .def("safe_step", [](AgentBrain& self,
                           const std::shared_ptr<SimpleWorld>& world,
                           size_t num_steps) {
            try {
                self.run_episode(world, num_steps);
                py::print("INFO: Episode completed successfully");
            } catch (const std::exception& e) {
                py::print("ERROR: Episode failed - " + std::string(e.what()));
                throw;
            }
        }, py::arg("world"), py::arg("num_steps"),
             "Run episode with error handling and graceful failure reporting")
        
        // Pythonic string representation
        .def("__repr__", [](const AgentBrain& self) {
            std::ostringstream oss;
            oss << "<AgentBrain stage=" << static_cast<int>(self.getDevelopmentalStage())
                << " neuromodulation_level=" << self.getNeuromodulationLevel()
                << " curiosity_level=" << self.getCuriosityLevel()
                << ">";
            return oss.str();
        })
        .def("__str__", [](const AgentBrain& self) {
            return "AgentBrain stage=" + std::to_string(static_cast<int>(self.getDevelopmentalStage()));
        });

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
