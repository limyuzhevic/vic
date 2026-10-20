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

    // Configuration management enhancements
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system with JSON/YAML support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/YAML or key=value file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", &Config::validate,
             "Validate configuration and return true if valid")
        .def("getValidationError", &Config::getValidationError,
             "Get validation error message if configuration is invalid")
        .def("setDescription", &Config::setDescription, py::arg("key"), py::arg("description"),
             "Set description for a configuration key")
        .def("getDescription", &Config::getDescription, py::arg("key"),
             "Get description for a configuration key")
        .def("applyPreset", &Config::applyPreset, py::arg("presetName"),
             "Apply a named configuration preset")
        .def("hasPreset", &Config::hasPreset, py::arg("presetName"),
             "Check if a preset exists")
        .def("getPresetNames", &Config::getPresetNames,
             "Get all available preset names")
        .def("getEntryCount", &Config::getEntryCount,
             "Get the number of configuration entries")
        .def("getEntrySources", &Config::getEntrySources,
             "Get sources of all configuration entries")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Advanced sensor input types with enhanced functionality
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for advanced sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("normalize", &SensoryInput::normalize, "Normalize sensory data")
        .def("filter", &SensoryInput::filter, py::arg("kernel"), "Apply filter to sensory data")
        .def("extractFeatures", &SensoryInput::extractFeatures, "Extract features from sensory data")
        .def("enhanceContrast", &SensoryInput::enhanceContrast, "Enhance contrast in sensory data");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Advanced vision sensory input with enhancement capabilities)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("setFilter", &Vision::setFilter, py::arg("filterType"), "Set vision filter type")
        .def("applySobelEdgeDetection", &Vision::applySobelEdgeDetection, "Apply Sobel edge detection")
        .def("applyCannyEdgeDetection", &Vision::applyCannyEdgeDetection, py::arg("threshold1"), py::arg("threshold2"),
             "Apply Canny edge detection")
        .def("detectObjects", &Vision::detectObjects, "Detect objects in vision data")
        .def("trackMotion", &Vision::trackMotion, "Track motion in vision data");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Advanced audio sensory input with analysis capabilities)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("applyFilter", &Audio::applyFilter, py::arg("filterType"), "Apply audio filter")
        .def("extractFeatures", &Audio::extractFeatures, "Extract audio features")
        .def("analyzeFrequencySpectrum", &Audio::analyzeFrequencySpectrum, "Analyze frequency spectrum")
        .def("detectPitch", &Audio::detectPitch, "Detect pitch in audio data");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Advanced internal signals with processing capabilities)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("normalize", &InternalSignals::normalize, "Normalize internal signals")
        .def("applySmoothing", &InternalSignals::applySmoothing, "Apply smoothing filter")
        .def("detectAnomalies", &InternalSignals::detectAnomalies, "Detect anomalies in internal signals")
        .def("extractTemporalFeatures", &InternalSignals::extractTemporalFeatures, "Extract temporal features");

    // Enhanced Action class with more capabilities
    py::class_<Action>(m, "Action", R"pbdoc(Advanced action representation with enhanced parameters)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getConfidence", &Action::getConfidence)
        .def("setConfidence", &Action::setConfidence, py::arg("confidence"))
        .def("clone", &Action::clone)
        .def("normalizeParameters", &Action::normalizeParameters)
        .def("scaleParameters", &Action::scaleParameters, py::arg("factor"))
        .def("applyNoise", &Action::applyNoise, py::arg("stddev"), "Add Gaussian noise to parameters");

    // Enhanced AgentBrain with research capabilities
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface with research features)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world and advanced settings")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept with advanced preprocessing")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity with uncertainty estimation")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation with Bayesian updating")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system with enhanced dynamics")
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
        // Research features
        .def("enableAdvancedFeatures", &AgentBrain::enableAdvancedFeatures,
             "Enable advanced research features")
        .def("configureResearchMode", &AgentBrain::configureResearchMode,
             "Configure for research experiments")
        .def("enableDebugLogging", &AgentBrain::enableDebugLogging,
             "Enable detailed debug logging")
        .def("configureProgressReporting", &AgentBrain::configureProgressReporting,
             py::arg("interval"), "Configure progress reporting interval")
        .def("getSystemStats", &AgentBrain::getSystemStats,
             "Get comprehensive system statistics")
        .def("validateNeuronDistribution", &AgentBrain::validateNeuronDistribution,
             "Validate neuron distribution")
        .def("recoverFromCorruption", &AgentBrain::recoverFromCorruption,
             "Recover from corruption state")
        .def("enableDevelopmentalStages", &AgentBrain::enableDevelopmentalStages,
             py::arg("enable"), "Enable or disable developmental stages")
        .def("configureLearningRate", &AgentBrain::configureLearningRate,
             py::arg("rate"), "Configure learning rate")
        .def("enableMetaLearning", &AgentBrain::enableMetaLearning,
             py::arg("enable"), "Enable or disable meta-learning")
        .def("enableExperimentalMode", &AgentBrain::enableExperimentalMode,
             py::arg("mode"), "Enable experimental mode with specific mode")
        .def("configureCustomParameters", &AgentBrain::configureCustomParameters,
             py::arg("params"), "Configure custom parameters")
        .def("optimizeMemoryAccess", &AgentBrain::optimizeMemoryAccess,
             "Optimize memory access patterns")
        .def("configureBatchProcessing", &AgentBrain::configureBatchProcessing,
             py::arg("enable"), "Enable or disable batch processing")
        .def("calculateActivityBalance", &AgentBrain::calculateActivityBalance,
             "Calculate excitation/inhibition balance")
        .def("calculateProcessingEfficiency", &AgentBrain::calculateProcessingEfficiency,
             "Calculate processing efficiency")
        .def("getNeuralActivityProfile", &AgentBrain::getNeuralActivityProfile,
             "Get neural activity profile")
        .def("saveState", &AgentBrain::saveState, py::arg("filepath"),
             "Save agent brain state to file")
        .def("loadState", &AgentBrain::loadState, py::arg("filepath"),
             "Load agent brain state from file")
        .def("configureCheckpointing", &AgentBrain::configureCheckpointing,
             py::arg("enable"), py::arg("interval"),
             "Configure checkpointing system")
        // Configuration features
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Research-oriented helper functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with research presets");

    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Apply research presets
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        return config;
    }, "Create a research-oriented configuration");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Minimal configuration for testing
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.05f);
        return config;
    }, "Create a minimal configuration for testing");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with advanced configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with enhanced features");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface with research features");

    // Research utilities
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return std::make_pair(brain->getTotalSpikeCount(), duration.count());
    }, py::arg("brain"), py::arg("steps"),
       "Benchmark brain performance and return spike count and execution time");

    m.def("compareBrains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2, int steps) {
        brain1->reset();
        brain2->reset();
        
        int spikes1 = 0, spikes2 = 0;
        for (int i = 0; i < steps; ++i) {
            brain1->step(i);
            brain2->step(i);
            spikes1 += brain1->getFiringNeuronCount();
            spikes2 += brain2->getFiringNeuronCount();
        }
        
        return std::make_pair(spikes1, spikes2);
    }, py::arg("brain1"), py::arg("brain2"), py::arg("steps"),
       "Compare two brains and return spike counts for both");

    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, int steps) {
        agent->initialize(*world);
        agent->enableAdvancedFeatures();
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.01);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(), brain->getAverageFiringRate(),
                               agent->getCuriosityLevel(), agent->getNoveltyLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run a complete simulation and return statistics");

    m.def("saveExperimentResults", [](const std::string& filepath,
                                     const std::vector<std::tuple<int, float, float, float>>& results) {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "Experiment Results\n";
            file << "===================\n\n";
            for (size_t i = 0; i < results.size(); ++i) {
                auto& result = results[i];
                file << "Episode " << i << ":\n";
                file << "  Spike Count: " << std::get<0>(result) << "\n";
                file << "  Firing Rate: " << std::get<1>(result) << "\n";
                file << "  Curiosity Level: " << std::get<2>(result) << "\n";
                file << "  Novelty Level: " << std::get<3>(result) << "\n\n";
            }
        }
    }, py::arg("filepath"), py::arg("results"),
       "Save experiment results to a file");

    m.def("loadExperimentResults", [](const std::string& filepath) {
        std::vector<std::tuple<int, float, float, float>> results;
        std::ifstream file(filepath);
        if (file.is_open()) {
            std::string line;
            int episode = 0;
            int spikeCount = 0;
            float firingRate = 0.0f;
            float curiosityLevel = 0.0f;
            float noveltyLevel = 0.0f;
            
            while (std::getline(file, line)) {
                if (line.find("Episode ") == 0) {
                    // Save previous episode
                    if (episode > 0) {
                        results.emplace_back(spikeCount, firingRate, curiosityLevel, noveltyLevel);
                    }
                    episode = std::stoi(line.substr(8, line.find(':') - 8));
                    spikeCount = 0;
                    firingRate = 0.0f;
                    curiosityLevel = 0.0f;
                    noveltyLevel = 0.0f;
                } else if (line.find("Spike Count:") == 0) {
                    spikeCount = std::stoi(line.substr(12));
                } else if (line.find("Firing Rate:") == 0) {
                    firingRate = std::stof(line.substr(13));
                } else if (line.find("Curiosity Level:") == 0) {
                    curiosityLevel = std::stof(line.substr(16));
                } else if (line.find("Novelty Level:") == 0) {
                    noveltyLevel = std::stof(line.substr(14));
                }
            }
            
            if (episode > 0) {
                results.emplace_back(spikeCount, firingRate, curiosityLevel, noveltyLevel);
            }
        }
        return results;
    }, py::arg("filepath"),
       "Load experiment results from a file");

    // Constants for advanced research
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Research mode constants
    m.attr("RESEARCH_MODE_ADVANCED") = 1;
    m.attr("RESEARCH_MODE_EXPERIMENTAL") = 2;
    m.attr("RESEARCH_MODE_OPTIMIZATION") = 3;

    // Configuration preset names
    m.def("listPresetNames", []() {
        return std::vector<std::string>{
            "default",
            "research",
            "minimal",
            "development",
            "production",
            "experimental"
        };
    }, "List all available configuration presets");

    m.def("getPresetDescription", [](const std::string& name) {
        std::map<std::string, std::string> descriptions = {
            {"default", "Default configuration with standard parameters"},
            {"research", "Research-oriented configuration with enhanced learning"},
            {"minimal", "Minimal configuration for testing and debugging"},
            {"development", "Development configuration with extensive logging"},
            {"production", "Production configuration with optimized performance"},
            {"experimental", "Experimental configuration with custom parameters"}
        };
        
        auto it = descriptions.find(name);
        if (it != descriptions.end()) {
            return it->second;
        }
        return std::string("Unknown preset: ") + name;
    }, py::arg("name"), "Get description for a specific preset");
}

} // namespace nlm
