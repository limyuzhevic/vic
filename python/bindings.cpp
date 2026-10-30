#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

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

// Python-friendly configuration helper functions
namespace config_helpers {
    // Common configuration presets
    std::shared_ptr<Config> createSmallConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        return config;
    }
    
    std::shared_ptr<Config> createMediumConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        return config;
    }
    
    std::shared_ptr<Config> createLargeConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        return config;
    }
    
    // Configuration validation helper
    bool validateConfig(const Config& config, std::vector<std::string>& errors) {
        errors.clear();
        
        // Basic validation
        if (!config.has("neuron_count")) {
            errors.push_back("Missing required configuration: neuron_count");
        }
        
        if (!config.has("region_count")) {
            errors.push_back("Missing required configuration: region_count");
        }
        
        if (config.has("neuron_count")) {
            auto neuronCount = config.get<int64_t>("neuron_count");
            if (neuronCount && neuronCount < 10) {
                errors.push_back("neuron_count must be at least 10");
            }
        }
        
        if (config.has("connection_probability")) {
            auto prob = config.get<float>("connection_probability");
            if (prob && (prob < 0.0f || prob > 1.0f)) {
                errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
        
        if (config.has("simulation_timestep")) {
            auto timestep = config.get<double>("simulation_timestep");
            if (timestep && timestep <= 0.0) {
                errors.push_back("simulation_timestep must be positive");
            }
        }
        
        return errors.empty();
    }
    
    // Quick start helper for common scenarios
    std::pair<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>> createSimpleSensoryMotorLoop(
        const std::string& worldSize = "20x20",
        int neuronCount = 500,
        bool enableDevelopment = true,
        bool enableMemory = true,
        bool enableNeuromodulation = true
    ) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        // Enable advanced features
        config->set("enable_development", enableDevelopment, ConfigSource::Runtime);
        config->set("enable_memory", enableMemory, ConfigSource::Runtime);
        config->set("enable_neuromodulation", enableNeuromodulation, ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        
        // Parse world size
        size_t xPos = 20, yPos = 20;
        size_t pos = worldSize.find('x');
        if (pos != std::string::npos) {
            xPos = std::stoul(worldSize.substr(0, pos));
            yPos = std::stoul(worldSize.substr(pos + 1));
        }
        
        world->configure(xPos, yPos, 8, 8);
        
        return std::make_pair(brain, world);
    }
    
    // Learning experiment helper
    struct ExperimentResults {
        float totalReward = 0.0f;
        int totalSteps = 0;
        float avgFiringRate = 0.0f;
        int memoryEpisodes = 0;
        float curiosityLevel = 0.0f;
        float noveltyLevel = 0.0f;
        float dopamineLevel = 0.0f;
        std::vector<float> actionRewards;
        std::vector<float> firingRates;
    };
    
    ExperimentResults runSensoryMotorExperiment(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        int numEpisodes = 10,
        int maxStepsPerEpisode = 200,
        bool enableVisualization = false
    ) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        agentBrain->enableRewardModulation(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        
        ExperimentResults results;
        
        for (int episode = 0; episode < numEpisodes; ++episode) {
            world->reset();
            agentBrain->reset();
            brain->reset();
            
            float episodeReward = 0.0f;
            
            for (int step = 0; step < maxStepsPerEpisode; ++step) {
                // Get sensory percept from world
                const SensoryPercept& percept = world->getSensoryPercept();
                
                // Process sensory input
                agentBrain->processSensoryInput(percept);
                
                // Brain step
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(neuralStep);
                }
                
                // Decode motor command
                MotorCommand cmd = agentBrain->decodeMotorCommand();
                
                // Apply action to world
                ActionResult result = world->applyMotorCommand(cmd, world->getSimulationTime());
                episodeReward += result.reward;
                
                // Apply reward modulation
                agentBrain->applyRewardModulation(result.reward, 0.0f);
                
                // Update world
                world->update(0.01);  // 10ms timestep
                
                // Update development
                agentBrain->updateDevelopment(0.01);
                
                // Collect metrics
                results.actionRewards.push_back(result.reward);
                results.firingRates.push_back(brain->getAverageFiringRate());
            }
            
            results.totalReward += episodeReward;
            results.totalSteps += maxStepsPerEpisode;
            
            if (enableVisualization && episode % 5 == 0) {
                std::cout << "Episode " << episode << ": Reward=" << episodeReward 
                          << ", Actions=" << results.actionRewards.size()
                          << ", Avg Firing=" << results.firingRates.back() << std::endl;
            }
        }
        
        // Calculate averages
        if (results.totalSteps > 0) {
            results.avgFiringRate = 0.0f;
            for (float rate : results.firingRates) {
                results.avgFiringRate += rate;
            }
            results.avgFiringRate /= results.firingRates.size();
            
            results.totalReward /= numEpisodes;
        }
        
        // Get final levels
        results.curiosityLevel = agentBrain->getCuriosityLevel();
        results.noveltyLevel = agentBrain->getNoveltyLevel();
        results.dopamineLevel = agentBrain->getNeuromodulationLevel();
        
        // Get memory stats
        if (brain->getEpisodicMemory()) {
            results.memoryEpisodes = brain->getEpisodicMemory()->getEpisodeCount();
        }
        
        return results;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ================================================
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides access to biologically-inspired neural computation
        with experience-based learning, memory systems, and developmental plasticity.
        
        Key Features:
        - Real spiking neural dynamics with LIF neurons
        - Event-driven spike propagation and plasticity
        - Memory systems (working, episodic, semantic, procedural)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Cognitive capabilities (attention, planning, concept formation)
        - Developmental plasticity and structural learning
        - Sensory-motor loop integration
        - Checkpoint/restore functionality
        - Python-friendly API with high-level convenience functions
        
        Quick Start Example:
        ```python
        import pynlm
        
        # Create a simple brain with default configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Or use the quick start helper for a sensory-motor loop
        brain, world = pynlm.createSimpleSensoryMotorLoop()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run a simple learning experiment
        results = pynlm.runSensoryMotorExperiment(brain, world, numEpisodes=5)
        print(f"Total reward: {results.totalReward}")
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register basic type aliases for Python
    m.attr("NeuronId") = py::type<NeuronId>();
    m.attr("SynapseId") = py::type<SynapseId>();
    m.attr("RegionId") = py::type<RegionId>();
    m.attr("PopulationId") = py::type<PopulationId>();

    // Register NeuronId convenience methods
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

    // Register SynapseId convenience methods
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // Register RegionId convenience methods
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

    // Register PopulationId convenience methods
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Register NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Register SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Register DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Register FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Register ActionType enum
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

    // Register MotorCommand enum
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

    // Register WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class with validation helpers
    py::class_<Config>(m, "Config", R"pbdoc(Extended configuration class with validation and helpers)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("loadFromJson", &Config::loadFromJson, py::arg("jsonStr"),
             "Load configuration from JSON string")
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

    // Convenience factory functions
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createSmallConfig", config_helpers::createSmallConfig,
          "Create a small (100 neurons) configuration for quick testing");

    m.def("createMediumConfig", config_helpers::createMediumConfig,
          "Create a medium (500 neurons) configuration for development and testing");

    m.def("createLargeConfig", config_helpers::createLargeConfig,
          "Create a large (1000 neurons) configuration for production and research");

    m.def("validateConfig", config_helpers::validateConfig,
          py::arg("config"), py::arg("errors"),
          "Validate a configuration and return error messages");

    m.def("createSimpleSensoryMotorLoop", config_helpers::createSimpleSensoryMotorLoop,
          py::arg("worldSize") = "20x20",
          py::arg("neuronCount") = 500,
          py::arg("enableDevelopment") = true,
          py::arg("enableMemory") = true,
          py::arg("enableNeuromodulation") = true,
          "Create a simple sensory-motor loop for quick experimentation");

    m.def("runSensoryMotorExperiment", config_helpers::runSensoryMotorExperiment,
          py::arg("brain"), py::arg("world"),
          py::arg("numEpisodes") = 10,
          py::arg("maxStepsPerEpisode") = 200,
          py::arg("enableVisualization") = false,
          "Run a complete sensory-motor learning experiment and return results");

    // Experiment results structure
    py::class_<config_helpers::ExperimentResults>(m, "ExperimentResults",
        R"pbdoc(Results from a sensory-motor experiment)pbdoc")
        .def(py::init<>())
        .def_readwrite("totalReward", &config_helpers::ExperimentResults::totalReward)
        .def_readwrite("totalSteps", &config_helpers::ExperimentResults::totalSteps)
        .def_readwrite("avgFiringRate", &config_helpers::ExperimentResults::avgFiringRate)
        .def_readwrite("memoryEpisodes", &config_helpers::ExperimentResults::memoryEpisodes)
        .def_readwrite("curiosityLevel", &config_helpers::ExperimentResults::curiosityLevel)
        .def_readwrite("noveltyLevel", &config_helpers::ExperimentResults::noveltyLevel)
        .def_readwrite("dopamineLevel", &config_helpers::ExperimentResults::dopamineLevel)
        .def_readwrite("actionRewards", &config_helpers::ExperimentResults::actionRewards)
        .def_readwrite("firingRates", &config_helpers::ExperimentResults::firingRates)
        .def("__repr__", [](const config_helpers::ExperimentResults& r) {
            std::ostringstream oss;
            oss << "<ExperimentResults: totalReward=" << r.totalReward
                << ", avgFiringRate=" << r.avgFiringRate
                << ", episodes=" << r.memoryEpisodes << ">";
            return oss.str();
        });

    // Enhanced SensoryInput hierarchy with additional methods
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("isValid", &SensoryInput::isValid,
             "Check if the sensory input is valid");

    // Enhanced Vision class with additional methods
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Enhanced vision sensory input with utility methods)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set visual data as a vector")
        .def("getDataAsArray", [](Vision& self) {
            return self.getData();
        }, "Get visual data as Python list")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getResolution", [](Vision& self) {
            return std::make_pair(self.getWidth(), self.getHeight());
        }, "Get resolution as (width, height) tuple")
        .def("normalize", &Vision::normalize,
             "Normalize pixel values to [0, 1] range");

    // Enhanced Audio class with additional methods
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Enhanced audio sensory input with utility methods)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as a vector")
        .def("getDataAsArray", [](Audio& self) {
            return self.getData();
        }, "Get audio data as Python list")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get duration in seconds")
        .def("normalize", &Audio::normalize,
             "Normalize audio samples to [-1, 1] range");

    // Enhanced InternalSignals class with additional methods
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Enhanced internal signals with utility methods)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("addSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            for (float val : signals) self.addSignal(val);
        }, py::arg("signals"), "Add multiple signal values")
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("getMean", &InternalSignals::getMean,
             "Get mean of all signals")
        .def("getVariance", &InternalSignals::getVariance,
             "Get variance of all signals")
        .def("getStandardDeviation", &InternalSignals::getStandardDeviation,
             "Get standard deviation of all signals")
        .def("getAsArray", [](InternalSignals& self) {
            return self.getData();
        }, "Get all signals as Python list");

    // Enhanced Action class with additional methods
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action class with utility methods)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("addParameter", &Action::addParameter, py::arg("value"),
             "Add a parameter value")
        .def("getParameterCount", &Action::getParameterCount,
             "Get number of parameters")
        .def("getName", &Action::getName)
        .def("getDescription", &Action::getDescription,
             "Get action description")
        .def("clone", &Action::clone,
             "Create a copy of this action")
        .def("equals", &Action::equals, py::arg("other"),
             "Check if this action equals another action");

    // Enhanced WorldObject class with additional methods
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object with utility methods)pbdoc")
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
        .def("distanceTo", [](WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to position (x, y)")
        .def("isWithinRadius", [](WorldObject& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            float dist = std::sqrt(dx*dx + dy*dy);
            return dist <= radius;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Check if object is within radius of position")
        .def("moveTo", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
             "Move object to position (x, y)")
        .def("setType", &WorldObject::setType, py::arg("type"),
             "Set object type");

    // Enhanced AgentBody class with additional methods
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body with utility methods)pbdoc")
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
        .def("reset", &AgentBody::reset)
        .def("moveTo", [](AgentBody& self, float x, float y, float orientation) {
            self.x = x;
            self.y = y;
            self.orientation = orientation;
            self.isMoving = true;
        }, py::arg("x"), py::arg("y"), py::arg("orientation"),
             "Move agent to position and orientation")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"),
             "Set agent velocity")
        .def("updatePosition", [](AgentBody& self, float dt) {
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
            self.orientation += self.angularVelocity * dt;
        }, py::arg("dt"),
             "Update agent position based on velocity")
        .def("getSpeed", [](AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("getDirection", [](AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get movement direction in radians");

    // Enhanced ActionResult class with additional methods
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result with utility methods)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isGood", [](ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if action was successful and rewarding")
        .def("isBad", [](ActionResult& self) {
            return !self.success || self.reward < -0.5f;
        }, "Check if action was unsuccessful or harmful")
        .def("getCategory", [](ActionResult& self) {
            if (self.success) {
                if (self.reward > 0.0f) return "GoodAction";
                return "NeutralAction";
            } else {
                if (self.reward < -0.5f) return "BadAction";
                return "FailedAction";
            }
        }, "Categorize action result");

    // Enhanced SensoryPercept class with additional methods
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept with utility methods)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getVisionChannels", [](SensoryPercept& self) {
            auto vision = self.getVision();
            return vision.getChannels();
        }, "Get vision channel count")
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("getSignalStrength", [](SensoryPercept& self) {
            auto signals = self.getAllSignals();
            float sum = 0.0f;
            for (float val : signals) sum += std::abs(val);
            return sum / signals.size();
        }, "Calculate overall signal strength")
        .def("normalizeSignals", [](SensoryPercept& self) {
            auto signals = self.getAllSignals();
            float maxVal = 0.0f;
            for (float val : signals) maxVal = std::max(maxVal, std::abs(val));
            if (maxVal > 0.0f) {
                for (float& val : signals) val /= maxVal;
                self.setInternal(signals);
            }
        }, "Normalize all signal values to [-1, 1] range");

    // Enhanced SimpleWorld class with additional methods
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple world with utility methods)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent sensor capabilities")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world dynamics")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and return result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getTimeStep", &SimpleWorld::getTimeStep,
             "Get current simulation time step")
        .def("setTimeStep", &SimpleWorld::setTimeStep, py::arg("dt"),
             "Set simulation time step")
        .def("getObjectCount", &SimpleWorld::getObjectCount,
             "Get number of objects in world")
        .def("getObjects", &SimpleWorld::getObjects,
             py::return_value_policy::reference_internal,
             "Get all objects in world")
        .def("findObjectAt", [](SimpleWorld& self, float x, float y) {
            auto objects = self.getObjects();
            for (auto& obj : objects) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                if (dx*dx + dy*dy <= obj.radius*obj.radius) {
                    return obj;
                }
            }
            return WorldObject{}; // Return empty object if not found
        }, py::arg("x"), py::arg("y"),
             "Find object at position (x, y)");

    // Enhanced Brain class with additional methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced brain class with utility methods and high-level APIs)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
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
             "Log brain status")
        .def("getBrainState", [](Brain& self) {
            std::ostringstream oss;
            oss << "Neurons: " << self.getTotalNeuronCount()
                << ", Synapses: " << self.getTotalSynapseCount()
                << ", Active: " << self.getActiveNeuronCount()
                << ", Firing: " << self.getFiringNeuronCount()
                << ", Stage: " << static_cast<int>(self.getDevelopmentalStage());
            return oss.str();
        }, "Get human-readable brain state string")
        .def("isHealthy", [](Brain& self) {
            return self.getTotalNeuronCount() > 0 &&
                   self.getTotalSynapseCount() > 0 &&
                   self.getDevelopmentalStage() != DevelopmentalStage::Aging;
        }, "Check if brain is in healthy state");

    // Enhanced AgentBrain class with additional methods
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with high-level APIs)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent brain interface")
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
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getActionValue", [](AgentBrain& self, const ActionType& actionType) {
            // Placeholder for action value estimation
            return 0.0f;
        }, py::arg("actionType"),
             "Estimate value of an action (to be implemented)")
        .def("selectAction", [](AgentBrain& self, const std::vector<float>& state) {
            // Placeholder for action selection
            return MotorCommand::Wait;
        }, py::arg("state"),
             "Select action based on current state (to be implemented)");

    // High-level convenience functions
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createSmallConfig", config_helpers::createSmallConfig,
          "Create a small (100 neurons) configuration for quick testing");

    m.def("createMediumConfig", config_helpers::createMediumConfig,
          "Create a medium (500 neurons) configuration for development and testing");

    m.def("createLargeConfig", config_helpers::createLargeConfig,
          "Create a large (1000 neurons) configuration for production and research");

    m.def("validateConfig", config_helpers::validateConfig,
          py::arg("config"), py::arg("errors"),
          "Validate a configuration and return error messages");

    m.def("createSimpleSensoryMotorLoop", config_helpers::createSimpleSensoryMotorLoop,
          py::arg("worldSize") = "20x20",
          py::arg("neuronCount") = 500,
          py::arg("enableDevelopment") = true,
          py::arg("enableMemory") = true,
          py::arg("enableNeuromodulation") = true,
          "Create a simple sensory-motor loop for quick experimentation");

    m.def("runSensoryMotorExperiment", config_helpers::runSensoryMotorExperiment,
          py::arg("brain"), py::arg("world"),
          py::arg("numEpisodes") = 10,
          py::arg("maxStepsPerEpisode") = 200,
          py::arg("enableVisualization") = false,
          "Run a complete sensory-motor learning experiment and return results");

    // Experiment results structure
    py::class_<config_helpers::ExperimentResults>(m, "ExperimentResults",
        R"pbdoc(Results from a sensory-motor experiment)pbdoc")
        .def(py::init<>())
        .def_readwrite("totalReward", &config_helpers::ExperimentResults::totalReward)
        .def_readwrite("totalSteps", &config_helpers::ExperimentResults::totalSteps)
        .def_readwrite("avgFiringRate", &config_helpers::ExperimentResults::avgFiringRate)
        .def_readwrite("memoryEpisodes", &config_helpers::ExperimentResults::memoryEpisodes)
        .def_readwrite("curiosityLevel", &config_helpers::ExperimentResults::curiosityLevel)
        .def_readwrite("noveltyLevel", &config_helpers::ExperimentResults::noveltyLevel)
        .def_readwrite("dopamineLevel", &config_helpers::ExperimentResults::dopamineLevel)
        .def_readwrite("actionRewards", &config_helpers::ExperimentResults::actionRewards)
        .def_readwrite("firingRates", &config_helpers::ExperimentResults::firingRates)
        .def("__repr__", [](const config_helpers::ExperimentResults& r) {
            std::ostringstream oss;
            oss << "<ExperimentResults: totalReward=" << r.totalReward
                << ", avgFiringRate=" << r.avgFiringRate
                << ", episodes=" << r.memoryEpisodes << ">";
            return oss.str();
        });

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "Enhanced NLM Python bindings with high-level convenience functions";
    m.attr("__author__") = "NLM Team";
    m.attr("__license__") = "MIT";

} // namespace nlm
