#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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

// Python exception for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

// Helper function to convert C++ exceptions to Python exceptions
void register_exceptions(py::module_& m) {
    py::register_exception<NLMError>(m, "NLMError");
}

// Enhanced convenience functions for Python users
namespace py_utils {
    // Create brain with preset configurations
    std::shared_ptr<Brain> createBrainWithConfig(const std::string& config_name) {
        auto config = std::make_shared<Config>();
        
        // Preset configurations
        if (config_name == "default") {
            config->set("neuron_count", static_cast<int64_t>(1000));
            config->set("region_count", static_cast<int64_t>(1));
            config->set("connection_probability", 0.1f);
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
            config->set("synaptogenesis_rate", 0.0001f);
            config->set("pruning_rate", 0.00001f);
        } else if (config_name == "small") {
            config->set("neuron_count", static_cast<int64_t>(100));
            config->set("region_count", static_cast<int64_t>(1));
            config->set("connection_probability", 0.05f);
        } else if (config_name == "large") {
            config->set("neuron_count", static_cast<int64_t>(5000));
            config->set("region_count", static_cast<int64_t>(3));
            config->set("connection_probability", 0.15f);
        } else {
            throw NLMError("Unknown configuration: " + config_name);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw NLMError("Failed to initialize brain with config: " + config_name);
        }
        return brain;
    }
    
    // Create complete agent setup (brain + world + agent brain)
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> 
    createAgentSetup(const std::string& config_name, int vision_width, int vision_height) {
        auto brain = createBrainWithConfig(config_name);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(vision_width, vision_height, vision_width, vision_height);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }
    
    // Run simulation with automatic reward tracking
    struct SimulationResult {
        float totalReward;
        float avgFiringRate;
        size_t totalSteps;
        size_t finalSpikeCount;
        std::vector<float> firingRates;
    };
    
    SimulationResult runSimulation(std::shared_ptr<Brain> brain, 
                                  std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<AgentBrain> agent,
                                  size_t steps, bool track_firing_rate = true) {
        SimulationResult result;
        result.totalSteps = steps;
        result.firingRates.reserve(steps);
        
        for (size_t step = 0; step < steps; ++step) {
            // Get sensory input
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process in agent
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get action
            MotorCommand action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward if available
            float reward = 0.0f;
            if (world->getSensoryPercept().getInternal().size() > 0) {
                reward = world->getSensoryPercept().getInternal()[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.001);
            
            // Track metrics
            if (track_firing_rate) {
                result.firingRates.push_back(brain->getAverageFiringRate());
            }
            
            result.totalReward += reward;
        }
        
        result.avgFiringRate = result.totalSteps > 0 ? 
            std::accumulate(result.firingRates.begin(), result.firingRates.end(), 0.0f) / result.firingRates.size() : 0.0f;
        result.finalSpikeCount = brain->getTotalSpikeCount();
        
        return result;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        Provides enhanced API with better error handling, convenience methods,
        and research-ready tools for neural simulation.
        
        Key improvements:
        - Enhanced error handling with NLMError exceptions
        - Convenience methods for common setup patterns
        - Simulation result tracking and analysis
        - Better documentation and examples
        - Research-ready utilities
    )pbdoc";

    // Register exceptions
    register_exceptions(m);

    // Type definitions for Python
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
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

    // Config class with enhanced Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
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
        // Pythonic convenience methods
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("get_int", [](const Config& self, const std::string& key, int default_value) {
            auto value = self.get<int>(key);
            return value ? value.value() : default_value;
        }, py::arg("key"), py::arg("default_value") = 0, "Get integer configuration value")
        .def("get_float", [](const Config& self, const std::string& key, float default_value) {
            auto value = self.get<double>(key);
            return value ? static_cast<float>(value.value()) : default_value;
        }, py::arg("key"), py::arg("default_value") = 0.0f, "Get float configuration value")
        .def("get_bool", [](const Config& self, const std::string& key, bool default_value) {
            auto value = self.get<bool>(key);
            return value ? value.value() : default_value;
        }, py::arg("key"), py::arg("default_value") = false, "Get boolean configuration value");

    // Enhanced Brain class with Pythonic methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
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
        .def("get_region", &Brain::getRegion, py::arg("id"),
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
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
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
        // Pythonic convenience methods
        .def("get_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count (alias for get_total_neuron_count)")
        .def("get_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count (alias for get_total_synapse_count)")
        .def("get_firing_rate", &Brain::getAverageFiringRate,
             "Get firing rate (alias for get_average_firing_rate)");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
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
             py::arg("reward"), py::arg("predictedReward"),
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
        // Pythonic convenience methods
        .def("enable_all_features", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all advanced features (reward, plasticity, development, curiosity)");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth,
             "Get world width")
        .def("get_height", &SimpleWorld::getHeight,
             "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             "Get random seed");

    // Enhanced SensoryInput and related classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("to_array", [](Vision& self) {
            return self.getData();
        }, "Get vision data as Python list (alias for get_data)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone)
        // Pythonic convenience methods
        .def("to_dict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getType();
            result["parameters"] = action.getParameters();
            return result;
        }, "Convert action to Python dictionary")
        .def("from_dict", [](Action& action, py::dict data) {
            ActionType type = py::cast<ActionType>(data["type"].cast<int>());
            std::vector<float> params = py::cast<std::vector<float>>(data["parameters"].cast<py::list>());
            return Action(type, params);
        }, "Create action from Python dictionary");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object representation)pbdoc")
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
        // Pythonic convenience methods
        .def("position", [](WorldObject& obj) {
            return py::make_tuple(obj.x, obj.y);
        }, "Get object position as tuple")
        .def("to_dict", [](const WorldObject& obj) {
            py::dict result;
            result["x"] = obj.x;
            result["y"] = obj.y;
            result["type"] = static_cast<int>(obj.type);
            result["value"] = obj.value;
            result["radius"] = obj.radius;
            result["active"] = obj.active;
            return result;
        }, "Convert world object to Python dictionary")
        .def("from_dict", [](WorldObject& obj, py::dict data) {
            obj.x = py::cast<float>(data["x"]);
            obj.y = py::cast<float>(data["y"]);
            obj.type = py::cast<WorldObjectType>(data["type"].cast<int>());
            obj.value = py::cast<float>(data["value"]);
            obj.radius = py::cast<float>(data["radius"]);
            obj.active = py::cast<bool>(data["active"]);
            return obj;
        }, "Update world object from Python dictionary");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        // Pythonic convenience methods
        .def("position", [](AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get agent position as tuple")
        .def("velocity", [](AgentBody& body) {
            return py::make_tuple(body.velocityX, body.velocityY);
        }, "Get agent velocity as tuple")
        .def("update", [](AgentBody& body, float x, float y, float orientation) {
            body.x = x;
            body.y = y;
            body.orientation = orientation;
        }, "Update agent position and orientation");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Pythonic convenience methods
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict result;
            result["vision"] = percept.getVision();
            result["touch"] = percept.getTouch();
            result["internal"] = percept.getInternal();
            result["proprioception"] = percept.getProprioception();
            result["audio"] = percept.getAudio();
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Convert sensory percept to Python dictionary")
        .def("from_dict", [](SensoryPercept& percept, py::dict data) {
            if (data.contains("vision")) {
                percept.setVision(py::cast<std::vector<float>>(data["vision"].cast<py::list>()));
            }
            if (data.contains("touch")) {
                percept.setTouch(py::cast<std::vector<float>>(data["touch"].cast<py::list>()));
            }
            if (data.contains("internal")) {
                percept.setInternal(py::cast<std::vector<float>>(data["internal"].cast<py::list>()));
            }
            if (data.contains("proprioception")) {
                percept.setProprioception(py::cast<std::vector<float>>(data["proprioception"].cast<py::list>()));
            }
            if (data.contains("audio")) {
                percept.setAudio(py::cast<std::vector<float>>(data["audio"].cast<py::list>()));
            }
            if (data.contains("timestamp")) {
                percept.setTimestamp(py::cast<double>(data["timestamp"]));
            }
            return percept;
        }, "Update sensory percept from Python dictionary");

    // Enhanced SimulationResult class
    py::class_<py_utils::SimulationResult>(m, "SimulationResult", R"pbdoc(Results from enhanced simulation runs)pbdoc")
        .def_readwrite("total_reward", &py_utils::SimulationResult::totalReward)
        .def_readwrite("avg_firing_rate", &py_utils::SimulationResult::avgFiringRate)
        .def_readwrite("total_steps", &py_utils::SimulationResult::totalSteps)
        .def_readwrite("final_spike_count", &py_utils::SimulationResult::finalSpikeCount)
        .def_readwrite("firing_rates", &py_utils::SimulationResult::firingRates)
        .def("to_dict", [](const py_utils::SimulationResult& result) {
            py::dict result_dict;
            result_dict["total_reward"] = result.totalReward;
            result_dict["avg_firing_rate"] = result.avgFiringRate;
            result_dict["total_steps"] = result.totalSteps;
            result_dict["final_spike_count"] = result.finalSpikeCount;
            result_dict["firing_rates"] = result.firingRates;
            return result_dict;
        }, "Convert simulation result to Python dictionary")
        .def("print_summary", [](const py_utils::SimulationResult& result) {
            std::cout << "=== Simulation Summary ===" << std::endl;
            std::cout << "Total Reward: " << result.totalReward << std::endl;
            std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
            std::cout << "Final Spike Count: " << result.finalSpikeCount << std::endl;
            std::cout << "Total Steps: " << result.totalSteps << std::endl;
        }, "Print human-readable summary of simulation results");

    // Enhanced convenience functions
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

    // Enhanced convenience functions
    m.def("create_brain_with_config", &py_utils::createBrainWithConfig,
         py::arg("config_name"), 
         "Create a brain with preset configuration (default, small, large)");

    m.def("create_agent_setup", &py_utils::createAgentSetup,
         py::arg("config_name"), py::arg("vision_width"), py::arg("vision_height"),
         "Create complete agent setup (brain + world + agent brain)");

    m.def("run_simulation", &py_utils::runSimulation,
         py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
         py::arg("track_firing_rate") = true,
         "Run enhanced simulation with automatic tracking");

    // Version and utility information
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation";

    // Error types
    py::class_<NLMError>(m, "NLMError", R"pbdoc(NLM-specific Python exception)pbdoc")
        .def(py::init<const std::string&>());

    // System information
    m.def("get_system_info", []() {
        py::dict info;
        info["version"] = "2.0.0";
        info["backend"] = "NLM C++ Neural Simulation Framework";
        info["phase"] = "Phase 6 - Final Integration";
        info["supported_features"] = py::list({
            "Neural Networks", "Working Memory", "Episodic Memory",
            "Neuromodulation", "Prediction Systems", "Development",
            "Plasticity", "Curiosity", "Novelty Detection"
        });
        return info;
    }, "Get system information and capabilities");

    // Research utilities
    m.def("create_research_brain", [](int neuron_count, int region_count) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count));
        config->set("region_count", static_cast<int64_t>(region_count));
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw NLMError("Failed to initialize research brain");
        }
        return brain;
    }, py::arg("neuron_count") = 1000, py::arg("region_count") = 1,
       "Create a research-ready brain with specified size");

    m.def("create_test_world", [](int width, int height, int vision_width, int vision_height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        world->reset();
        return world;
    }, py::arg("width") = 16, py::arg("height") = 16,
       py::arg("vision_width") = 8, py::arg("vision_height") = 8,
       "Create a test world with specified dimensions");

    m.def("create_test_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enable_all_features();
        return agent;
    }, py::arg("brain"), py::arg("world"),
       "Create a research-ready agent with all features enabled");

    // Compatibility functions (for backward compatibility)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (backward compatibility)");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (backward compatibility)");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (backward compatibility)");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (backward compatibility)");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

// Additional functions that need to be defined for the bindings to work
namespace nlm {
    // These would normally be in separate headers, but are declared here for the bindings
    // In a real implementation, these would be properly defined in the main codebase
}
