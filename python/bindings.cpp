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
// Neuromodulation systems
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
// Prediction systems
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
// Memory systems
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
// Cognition systems
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
// Development systems
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"

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

    // ========== NEUROMODULATION CLASSES ==========

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"));

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel)
        .def("getExplorationDrive", &Curiosity::getExplorationDrive)
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"))
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"))
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"))
        .def("reset", &Curiosity::reset);

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("observation"), py::arg("previousObservation"))
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& current, const std::vector<float>& previous) {
            self.detectNovelty(current, previous);
        })
        .def("update", &Novelty::update, py::arg("dt"))
        .def("getHistory", &Novelty::getHistory)
        .def("clearHistory", &Novelty::clearHistory);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getError", &PredictionError::getError)
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"))
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"))
        .def("getHistory", &PredictionError::getHistory)
        .def("clearHistory", &PredictionError::clearHistory)
        .def("getMagnitude", &PredictionError::getMagnitude);

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue)
        .def("setValue", &Reward::setValue, py::arg("value"))
        .def("add", &Reward::add, py::arg("delta"))
        .def("reset", &Reward::reset)
        .def("computeReward", &Reward::computeReward, py::arg("observation"))
        .def("getHistory", &Reward::getHistory)
        .def("clearHistory", &Reward::clearHistory);

    // ========== PREDICTION SYSTEMS ==========

    // ========== MEMORY SYSTEMS ==========

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &NeuralWorkingMemory::retrieve)
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"))
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"))
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate)
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"))
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"))
        .def("runCompetition", &NeuralWorkingMemory::runCompetition)
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"))
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity);

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Episodic memory item)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("sensoryState", &EpisodicMemoryItem::sensoryState)
        .def_readwrite("positionX", &EpisodicMemoryItem::positionX)
        .def_readwrite("positionY", &EpisodicMemoryItem::positionY)
        .def_readwrite("orientation", &EpisodicMemoryItem::orientation)
        .def_readwrite("action", &EpisodicMemoryItem::action)
        .def_readwrite("reward", &EpisodicMemoryItem::reward)
        .def_readwrite("energy", &EpisodicMemoryItem::energy)
        .def_readwrite("novelty", &EpisodicMemoryItem::novelty)
        .def_readwrite("resultingSensoryState", &EpisodicMemoryItem::resultingSensoryState)
        .def_readwrite("resultingReward", &EpisodicMemoryItem::resultingReward)
        .def_readwrite("activeNeurons", &EpisodicMemoryItem::activeNeurons)
        .def_readwrite("neuronActivations", &EpisodicMemoryItem::neuronActivations)
        .def_readwrite("age", &EpisodicMemoryItem::age);

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, py::arg("sensoryPattern"), py::arg("maxResults") = 5)
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal, py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10)
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5)
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction, py::arg("action"), py::arg("maxResults") = 5)
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"))
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance, py::arg("episodeId"), py::arg("relevanceDelta"))
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"))
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"))
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward)
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"))
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"))
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"));

    // ========== DEVELOPMENT SYSTEMS ==========

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("advanceStage", &DevelopmentSystem::advanceStage)
        .def("getStageName", &DevelopmentSystem::getStageName)
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, SimulationStep)>(&DevelopmentSystem::update), py::arg("brain"), py::arg("currentStep"))
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, RandomGenerator&, TimestepDuration)>(&DevelopmentSystem::update), py::arg("brain"), py::arg("rng"), py::arg("dt"))
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier)
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod)
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress)
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge)
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"));

    // Synaptogenesis
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis development)pbdoc")
        .def(py::init<>())
        .def("getFormationRate", &Synaptogenesis::getFormationRate)
        .def("setFormationRate", &Synaptogenesis::setFormationRate, py::arg("rate"))
        .def("getTargetDensity", &Synaptogenesis::getTargetDensity)
        .def("setTargetDensity", &Synaptogenesis::setTargetDensity, py::arg("density"));

    // Pruning
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Pruning development)pbdoc")
        .def(py::init<>())
        .def("getThreshold", &Pruning::getThreshold)
        .def("setThreshold", &Pruning::setThreshold, py::arg("threshold"))
        .def("getPruningRate", &Pruning::getPruningRate)
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"));

    // Maturation
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Maturation development)pbdoc")
        .def(py::init<>())
        .def("getProgress", &Maturation::getProgress)
        .def("setProgress", &Maturation::setProgress, py::arg("progress"))
        .def("getMatureThreshold", &Maturation::getMatureThreshold)
        .def("getMatureRestingPotential", &Maturation::getMatureRestingPotential)
        .def("getMatureTimeConstant", &Maturation::getMatureTimeConstant);

    // NeuralAssociativeMemory
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f)
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience, py::arg("episode"))
        .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("queryPattern"), py::arg("maxResults") = 5)
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("patternA"), py::arg("patternB"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation, py::arg("patternA"), py::arg("patternB"), py::arg("delta"))
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation, py::arg("cuePattern"), py::arg("steps") = 2)
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount);

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
        // Pythonic dict-like interface
        .def("__getitem__", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                value = self.get<int>(key);
                if (!value) {
                    value = self.get<double>(key);
                    if (!value) {
                        value = self.get<bool>(key);
                        if (!value) {
                            value = self.get<std::vector<int>>(key);
                            if (!value) {
                                value = self.get<std::vector<double>>(key);
                                if (!value) {
                                    value = self.get<std::vector<std::string>>(key);
                                }
                            }
                        }
                    }
                }
            }
            // Convert to appropriate Python type
            if (value.has_value()) {
                const ConfigValue& v = *value;
                return py::cast(v);
            }
            throw py::key_error("Key not found: " + key);
        })
        .def("__setitem__", [](Config& self, const std::string& key, const ConfigValue& value) {
            self.set(key, value, ConfigSource::Runtime);
        })
        .def("__setitem__", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        })
        .def("__setitem__", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        })
        .def("__setitem__", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        })
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        })
        .def("__contains__", [](const Config& self, const std::string& key) {
            return self.has(key);
        })
        // Pythonic convenience methods
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                value = self.get<int>(key);
                if (!value) {
                    value = self.get<double>(key);
                    if (!value) {
                        value = self.get<bool>(key);
                        if (!value) {
                            value = self.get<std::vector<int>>(key);
                            if (!value) {
                                value = self.get<std::vector<double>>(key);
                                if (!value) {
                                    value = self.get<std::vector<std::string>>(key);
                                }
                            }
                        }
                    }
                }
            }
            if (value.has_value()) {
                const ConfigValue& v = *value;
                return py::cast(v);
            }
            return py::none();
        }, py::arg("key"), "Get configuration value with automatic type detection")
        .def("get_or", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            if (self.has(key)) {
                auto value = self.get<std::string>(key);
                if (!value) {
                    value = self.get<int>(key);
                    if (!value) {
                        value = self.get<double>(key);
                        if (!value) {
                            value = self.get<bool>(key);
                            if (!value) {
                                value = self.get<std::vector<int>>(key);
                                if (!value) {
                                    value = self.get<std::vector<double>>(key);
                                    if (!value) {
                                        value = self.get<std::vector<std::string>>(key);
                                    }
                                }
                            }
                        }
                    }
                }
                if (value.has_value()) {
                    return py::cast(*value);
                }
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default"), "Get configuration value or default if not found")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                auto value = self.get<std::string>(key);
                if (!value) {
                    value = self.get<int>(key);
                    if (!value) {
                        value = self.get<double>(key);
                        if (!value) {
                            value = self.get<bool>(key);
                            if (!value) {
                                value = self.get<std::vector<int>>(key);
                                if (!value) {
                                    value = self.get<std::vector<double>>(key);
                                    if (!value) {
                                        value = self.get<std::vector<std::string>>(key);
                                    }
                                }
                            }
                        }
                    }
                }
                if (value.has_value()) {
                    result[key] = py::cast(*value);
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = py::str(item.first);
                py::object value = item.second;
                
                if (value.is(py::int_())) {
                    self.set(key, value.cast<int>(), ConfigSource::Runtime);
                } else if (value.is(py::float_())) {
                    self.set(key, value.cast<double>(), ConfigSource::Runtime);
                } else if (value.is(py::bool_())) {
                    self.set(key, value.cast<bool>(), ConfigSource::Runtime);
                } else if (value.is(py::str())) {
                    self.set(key, value.cast<std::string>(), ConfigSource::Runtime);
                } else if (value.is(py::list())) {
                    py::list list = value.cast<py::list>();
                    if (!list.empty()) {
                        auto first = list[0];
                        if (first.is(py::int_())) {
                            std::vector<int> vec;
                            for (auto item : list) {
                                vec.push_back(item.cast<int>());
                            }
                            self.set(key, vec, ConfigSource::Runtime);
                        } else if (first.is(py::float_())) {
                            std::vector<double> vec;
                            for (auto item : list) {
                                vec.push_back(item.cast<double>());
                            }
                            self.set(key, vec, ConfigSource::Runtime);
                        } else if (first.is(py::str())) {
                            std::vector<std::string> vec;
                            for (auto item : list) {
                                vec.push_back(item.cast<std::string>());
                            }
                            self.set(key, vec, ConfigSource::Runtime);
                        }
                    }
                }
            }
            return py::none();
        }, py::arg("data"), "Load configuration from Python dictionary")
        .def("to_json", [](const Config& self) {
            // Simple JSON serialization - could be enhanced with proper JSON library
            std::string json = "{";
            bool first = true;
            for (const auto& key : self.getKeys()) {
                if (!first) json += ", ";
                first = false;
                
                auto value = self.get<std::string>(key);
                if (!value) {
                    value = self.get<int>(key);
                    if (!value) {
                        value = self.get<double>(key);
                        if (!value) {
                            value = self.get<bool>(key);
                            if (!value) {
                                value = self.get<std::vector<int>>(key);
                                if (!value) {
                                    value = self.get<std::vector<double>>(key);
                                    if (!value) {
                                        value = self.get<std::vector<std::string>>(key);
                                    }
                                }
                            }
                        }
                    }
                }
                
                json += "\"" + key + "\": ";
                if (value.has_value()) {
                    json += serializeConfigValue(*value);
                }
            }
            json += "}";
            return json;
        }, "Serialize configuration to JSON string")
        .def("from_json", [](Config& self, const std::string& json) {
            // Simple JSON parsing - could be enhanced with proper JSON library
            // For now, parse a simple format like {"key": value, ...}
            // This is a placeholder - implement proper JSON parsing
            throw py::value_error("from_json not yet implemented - use from_dict instead");
        }, py::arg("json"), "Load configuration from JSON string")
        // Validation
        .def("validate", [](const Config& self, const std::string& key) {
            return self.has(key);
        }, py::arg("key"), "Validate configuration key exists")
        .def("get_all", &Config::getKeys, "Get all configuration keys")
        // Copy assignment
        .def("copy_from", [](Config& self, const Config& other) {
            self.clear();
            for (const auto& key : other.getKeys()) {
                auto value = other.get<std::string>(key);
                if (!value) {
                    value = other.get<int>(key);
                    if (!value) {
                        value = other.get<double>(key);
                        if (!value) {
                            value = other.get<bool>(key);
                            if (!value) {
                                value = other.get<std::vector<int>>(key);
                                if (!value) {
                                    value = other.get<std::vector<double>>(key);
                                    if (!value) {
                                        value = other.get<std::vector<std::string>>(key);
                                    }
                                }
                            }
                        }
                    }
                }
                if (value.has_value()) {
                    self.set(key, *value, ConfigSource::Runtime);
                }
            }
            return py::none();
        }, py::arg("other"), "Copy configuration from another Config object")
        .def("merge", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                auto value = other.get<std::string>(key);
                if (!value) {
                    value = other.get<int>(key);
                    if (!value) {
                        value = other.get<double>(key);
                        if (!value) {
                            value = other.get<bool>(key);
                            if (!value) {
                                value = other.get<std::vector<int>>(key);
                                if (!value) {
                                    value = other.get<std::vector<double>>(key);
                                    if (!value) {
                                        value = other.get<std::vector<std::string>>(key);
                                    }
                                }
                            }
                        }
                    }
                }
                if (value.has_value()) {
                    self.set(key, *value, ConfigSource::Runtime);
                }
            }
            return py::none();
        }, py::arg("other"), "Merge another Config object into this one")
    // Convenience methods for common operations
        .def("create_brain", [](std::shared_ptr<Config> config) {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new Brain instance")
        .def("create_world", []() {
            return std::make_shared<SimpleWorld>();
        }, "Create a new SimpleWorld instance")
        .def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new AgentBrain instance")
        .def("create_default_config", []() {
            return std::make_shared<Config>();
        }, "Create a default configuration")
        .def("create_vision", [](size_t width, size_t height, size_t channels) {
            return std::make_shared<Vision>(width, height, channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create a Vision sensory input")
        .def("create_audio", [](size_t sampleRate, size_t numSamples) {
            return std::make_shared<Audio>(sampleRate, numSamples);
        }, py::arg("sampleRate"), py::arg("numSamples"), "Create an Audio sensory input")
        .def("create_action", [](ActionType type) {
            return std::make_shared<Action>(type);
        }, py::arg("type"), "Create an Action instance")
        .def("create_action_with_params", [](ActionType type, const std::vector<float>& parameters) {
            return std::make_shared<Action>(type, parameters);
        }, py::arg("type"), py::arg("parameters"), "Create an Action with parameters")
        .def("create_world_object", [](float x, float y, WorldObjectType type, float value, float radius) {
            return std::make_shared<WorldObject>(x, y, type, value, radius);
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f, "Create a WorldObject instance")
        .def("create_agent_body", []() {
            return std::make_shared<AgentBody>();
        }, "Create an AgentBody instance")
        .def("clone_config", [](const Config& config) {
            auto newConfig = std::make_shared<Config>();
            newConfig->copy_from(config);
            return newConfig;
        }, py::arg("config"), "Clone a configuration")
        .def("merge_configs", [](const Config& config1, const Config& config2) {
            auto mergedConfig = std::make_shared<Config>();
            mergedConfig->copy_from(config1);
            mergedConfig->merge(config2);
            return mergedConfig;
        }, py::arg("config1"), py::arg("config2"), "Merge two configurations")
        .def("validate_config", [](const Config& config) {
            bool isValid = true;
            for (const auto& key : config.getKeys()) {
                if (!config.has(key)) {
                    isValid = false;
                    break;
                }
            }
            return isValid;
        }, "Validate configuration");

    // Global configuration helper functions
        .def("load_config_from_dict", [](const py::dict& data) {
            auto config = std::make_shared<Config>();
            config->from_dict(data);
            return config;
        }, py::arg("data"), "Load configuration from dictionary")
        .def("save_config_to_dict", [](const Config& config) {
            return config.to_dict();
        }, py::arg("config"), "Save configuration to dictionary")
        .def("create_standard_config", []() {
            auto config = std::make_shared<Config>();
            // Set standard values
            config->set("random_seed", 42, ConfigSource::Runtime);
            config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
            config->set("neuron_count", 1000, ConfigSource::Runtime);
            config->set("region_count", 2, ConfigSource::Runtime);
            config->set("connection_probability", 0.1, ConfigSource::Runtime);
            config->set("plasticity_learning_rate", 0.01, ConfigSource::Runtime);
            config->set("stdp_ltp_weight", 0.01, ConfigSource::Runtime);
            config->set("stdp_ltd_weight", 0.012, ConfigSource::Runtime);
            config->set("stdp_time_constant", 20.0, ConfigSource::Runtime);
            config->set("environment_name", "GridWorld", ConfigSource::Runtime);
            config->set("environment_width", 10, ConfigSource::Runtime);
            config->set("environment_height", 10, ConfigSource::Runtime);
            config->set("log_level", "INFO", ConfigSource::Runtime);
            config->set("max_simulation_steps", 10000, ConfigSource::Runtime);
            return config;
        }, "Create a standard configuration with common values")
        .def("create_minimal_config", []() {
            auto config = std::make_shared<Config>();
            // Set minimal essential values
            config->set("random_seed", 42, ConfigSource::Runtime);
            config->set("neuron_count", 100, ConfigSource::Runtime);
            config->set("region_count", 1, ConfigSource::Runtime);
            return config;
        }, "Create a minimal configuration")
        .def("create_large_scale_config", []() {
            auto config = std::make_shared<Config>();
            // Set values for large scale simulations
            config->set("random_seed", 42, ConfigSource::Runtime);
            config->set("neuron_count", 10000, ConfigSource::Runtime);
            config->set("region_count", 10, ConfigSource::Runtime);
            config->set("connection_probability", 0.05, ConfigSource::Runtime);
            config->set("plasticity_learning_rate", 0.005, ConfigSource::Runtime);
            config->set("max_simulation_steps", 100000, ConfigSource::Runtime);
            config->set("log_level", "DEBUG", ConfigSource::Runtime);
            return config;
        }, "Create a large-scale configuration");

    // Custom NLM exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    py::register_exception<std::logic_error>(m, "LogicError", PyExc_RuntimeError);
    
    py::class_<std::runtime_error, std::exception, py::wrapper<std::runtime_error>>(m, "RuntimeError")
        .def(py::init<const char*>());
    
    py::class_<std::invalid_argument, std::exception, py::wrapper<std::invalid_argument>>(m, "ValueError")
        .def(py::init<const char*>());
    
    py::class_<std::out_of_range, std::exception, py::wrapper<std::out_of_range>>(m, "IndexError")
        .def(py::init<const char*>());
    
    py::class_<std::logic_error, std::exception, py::wrapper<std::logic_error>>(m, "LogicError")
        .def(py::init<const char*>());

    // Configuration-specific exceptions
    py::class_<std::runtime_error, std::exception>(m, "ConfigError")
        .def(py::init<const char*>());
    
    py::class_<std::invalid_argument, std::exception>(m, "InvalidConfigError")
        .def(py::init<const char*>());
    
    py::class_<std::out_of_range, std::exception>(m, "MissingConfigError")
        .def(py::init<const char*>());

    // Neural-specific exceptions
    py::class_<std::runtime_error, std::exception>(m, "NeuralError")
        .def(py::init<const char*>());
    
    py::class_<std::invalid_argument, std::exception>(m, "InvalidNeuronError")
        .def(py::init<const char*>());
    
    py::class_<std::out_of_range, std::exception>(m, "NeuronNotFoundError")
        .def(py::init<const char*>());

    // Memory-specific exceptions
    py::class_<std::runtime_error, std::exception>(m, "MemoryError")
        .def(py::init<const char*>());
    
    py::class_<std::bad_alloc, std::exception>(m, "OutOfMemoryError")
        .def(py::init<const char*>());

    // Error handling for Config class
        .def("get_with_validation", [](Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw MissingConfigError("Configuration key not found: " + key);
            }
            
            auto value = self.get<std::string>(key);
            if (!value) {
                value = self.get<int>(key);
                if (!value) {
                    value = self.get<double>(key);
                    if (!value) {
                        value = self.get<bool>(key);
                        if (!value) {
                            value = self.get<std::vector<int>>(key);
                            if (!value) {
                                value = self.get<std::vector<double>>(key);
                                if (!value) {
                                    value = self.get<std::vector<std::string>>(key);
                                }
                            }
                        }
                    }
                }
            }
            
            if (!value.has_value()) {
                throw InvalidConfigError("Configuration key exists but has no valid value: " + key);
            }
            
            return py::cast(*value);
        }, py::arg("key"), "Get configuration value with validation and exception handling")
        .def("get_or_throw", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                value = self.get<int>(key);
                if (!value) {
                    value = self.get<double>(key);
                    if (!value) {
                        value = self.get<bool>(key);
                        if (!value) {
                            value = self.get<std::vector<int>>(key);
                            if (!value) {
                                value = self.get<std::vector<double>>(key);
                                if (!value) {
                                    value = self.get<std::vector<std::string>>(key);
                                }
                            }
                        }
                    }
                }
            }
            
            if (!value.has_value()) {
                throw MissingConfigError("Configuration key not found: " + key);
            }
            
            return py::cast(*value);
        }, py::arg("key"), "Get configuration value or throw exception if not found")
        .def("set_with_validation", [](Config& self, const std::string& key, const ConfigValue& value) {
            if (key.empty()) {
                throw InvalidConfigError("Configuration key cannot be empty");
            }
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set configuration value with validation")
        .def("set_with_type_validation", [](Config& self, const std::string& key, const py::object& value) {
            if (key.empty()) {
                throw InvalidConfigError("Configuration key cannot be empty");
            }
            
            if (value.is(py::int_())) {
                self.set(key, value.cast<int>(), ConfigSource::Runtime);
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<double>(), ConfigSource::Runtime);
            } else if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>(), ConfigSource::Runtime);
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>(), ConfigSource::Runtime);
            } else if (value.is(py::list())) {
                py::list list = value.cast<py::list>();
                if (!list.empty()) {
                    auto first = list[0];
                    if (first.is(py::int_())) {
                        std::vector<int> vec;
                        for (auto item : list) {
                            vec.push_back(item.cast<int>());
                        }
                        self.set(key, vec, ConfigSource::Runtime);
                    } else if (first.is(py::float_())) {
                        std::vector<double> vec;
                        for (auto item : list) {
                            vec.push_back(item.cast<double>());
                        }
                        self.set(key, vec, ConfigSource::Runtime);
                    } else if (first.is(py::str())) {
                        std::vector<std::string> vec;
                        for (auto item : list) {
                            vec.push_back(item.cast<std::string>());
                        }
                        self.set(key, vec, ConfigSource::Runtime);
                    } else {
                        throw InvalidConfigError("Unsupported list element type for configuration");
                    }
                } else {
                    self.set(key, std::vector<std::string>(), ConfigSource::Runtime);
                }
            } else {
                throw InvalidConfigError("Unsupported type for configuration value");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value with type validation")
        .def("validate_and_get", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw MissingConfigError("Configuration key not found: " + key);
            }
            
            auto value = self.get<std::string>(key);
            if (!value) {
                value = self.get<int>(key);
                if (!value) {
                    value = self.get<double>(key);
                    if (!value) {
                        value = self.get<bool>(key);
                        if (!value) {
                            value = self.get<std::vector<int>>(key);
                            if (!value) {
                                value = self.get<std::vector<double>>(key);
                                if (!value) {
                                    value = self.get<std::vector<std::string>>(key);
                                }
                            }
                        }
                    }
                }
            }
            
            if (!value.has_value()) {
                throw InvalidConfigError("Configuration key exists but has no valid value: " + key);
            }
            
            return std::make_pair(self.has(key), py::cast(*value));
        }, py::arg("key"), "Validate configuration key exists and return (exists, value)")
        .def("try_get", [](const Config& self, const std::string& key) {
            py::object defaultValue = py::none();
            if (self.has(key)) {
                auto value = self.get<std::string>(key);
                if (!value) {
                    value = self.get<int>(key);
                    if (!value) {
                        value = self.get<double>(key);
                        if (!value) {
                            value = self.get<bool>(key);
                            if (!value) {
                                value = self.get<std::vector<int>>(key);
                                if (!value) {
                                    value = self.get<std::vector<double>>(key);
                                    if (!value) {
                                        value = self.get<std::vector<std::string>>(key);
                                    }
                                }
                            }
                        }
                    }
                }
                if (value.has_value()) {
                    return std::make_pair(true, py::cast(*value));
                }
            }
            return std::make_pair(false, defaultValue);
        }, py::arg("key"), "Try to get configuration value, return (found, value)");

    // Error handling for Neural and Spike classes
        .def("get_spike_system", [](Brain& brain) {
            auto spikeSystem = brain.getSpikeSystem();
            if (!spikeSystem) {
                throw NeuralError("Spike system not available");
            }
            return spikeSystem;
        }, "Get spike system with error handling")
        .def("get_total_spikes", [](Brain& brain) {
            auto spikeSystem = brain.getSpikeSystem();
            if (!spikeSystem) {
                throw NeuralError("Spike system not available");
            }
            return spikeSystem->getSpikeCount();
        }, "Get total spike count with error handling")
        .def("get_most_active_neurons", [](Brain& brain, size_t count) {
            auto spikeSystem = brain.getSpikeSystem();
            if (!spikeSystem) {
                throw NeuralError("Spike system not available");
            }
            return spikeSystem->getMostActiveNeurons(count);
        }, py::arg("count"), "Get most active neurons with error handling")
        .def("queue_spike", [](Brain& brain, NeuronId neuron, Timestamp timestamp, SimulationStep step) {
            auto spikeSystem = brain.getSpikeSystem();
            if (!spikeSystem) {
                throw NeuralError("Spike system not available");
            }
            spikeSystem->queueSpike(neuron, timestamp, step);
        }, py::arg("neuron"), py::arg("timestamp"), py::arg("step"), "Queue spike event with error handling")
        .def("process_spikes", [](Brain& brain, SimulationStep currentStep) {
            auto spikeSystem = brain.getSpikeSystem();
            if (!spikeSystem) {
                throw NeuralError("Spike system not available");
            }
            spikeSystem->processSpikes(currentStep);
        }, py::arg("currentStep"), "Process spikes with error handling");

    // Error handling for Memory classes
        .def("get_working_memory", [](Brain& brain) {
            auto workingMemory = brain.getWorkingMemory();
            if (!workingMemory) {
                throw MemoryError("Working memory not available");
            }
            return workingMemory;
        }, "Get working memory with error handling")
        .def("get_episodic_memory", [](Brain& brain) {
            auto episodicMemory = brain.getEpisodicMemory();
            if (!episodicMemory) {
                throw MemoryError("Episodic memory not available");
            }
            return episodicMemory;
        }, "Get episodic memory with error handling")
        .def("get_associative_memory", [](Brain& brain) {
            auto associativeMemory = brain.getAssociativeMemory();
            if (!associativeMemory) {
                throw MemoryError("Associative memory not available");
            }
            return associativeMemory;
        }, "Get associative memory with error handling");

    // Error handling for AgentBrain
        .def("get_agent_brain", [](Brain& brain) {
            auto config = brain.getConfig();
            if (!config) {
                throw NeuralError("Brain not properly configured");
            }
            return std::make_shared<AgentBrain>(std::make_shared<Brain>(config));
        }, "Create agent brain with error handling");

    // Global exception handling functions
        .def("try_safe_operation", [](const std::function<void()>& operation) {
            try {
                operation();
                return std::make_pair(true, std::string("Operation completed successfully"));
            } catch (const std::exception& e) {
                return std::make_pair(false, std::string(e.what()));
            } catch (...) {
                return std::make_pair(false, std::string("Unknown error occurred"));
            }
        }, py::arg("operation"), "Safely execute operation and return (success, message)")
        .def("catch_exception", [](const std::function<void()>& operation, const std::string& operationName) {
            try {
                operation();
                return std::make_pair(true, std::string("Success"));
            } catch (const MissingConfigError& e) {
                return std::make_pair(false, std::string("Config Error: ") + e.what());
            } catch (const InvalidConfigError& e) {
                return std::make_pair(false, std::string("Invalid Config: ") + e.what());
            } catch (const NeuralError& e) {
                return std::make_pair(false, std::string("Neural Error: ") + e.what());
            } catch (const MemoryError& e) {
                return std::make_pair(false, std::string("Memory Error: ") + e.what());
            } catch (const std::exception& e) {
                return std::make_pair(false, std::string("Error: ") + e.what());
            } catch (...) {
                return std::make_pair(false, "Unknown error");
            }
        }, py::arg("operation"), py::arg("operationName"), "Execute operation and catch specific exceptions");

    // Configuration validation with detailed error reporting
        .def("validate_config_comprehensive", [](const Config& config) {
            std::vector<std::string> errors;
            std::vector<std::string> warnings;
            
            // Check for required keys
            if (!config.has("random_seed")) {
                warnings.push_back("random_seed not set - using default");
            }
            
            // Check required key ranges
            if (config.has("neuron_count")) {
                auto neuronCount = config.get<int>("neuron_count");
                if (neuronCount && (*neuronCount <= 0)) {
                    errors.push_back("neuron_count must be positive");
                }
            }
            
            if (config.has("region_count")) {
                auto regionCount = config.get<int>("region_count");
                if (regionCount && (*regionCount <= 0)) {
                    errors.push_back("region_count must be positive");
                }
            }
            
            if (config.has("connection_probability")) {
                auto connProb = config.get<double>("connection_probability");
                if (connProb && (*connProb < 0.0 || *connProb > 1.0)) {
                    errors.push_back("connection_probability must be between 0.0 and 1.0");
                }
            }
            
            return std::make_tuple(errors.empty(), errors, warnings);
        }, "Comprehensive configuration validation with detailed error reporting");
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

    // Pythonic wrappers for Action
        .def("get_action_name", [](Action& self) {
            return self.getName();
        }, "Get action name")
        .def("clone_action", [](Action& self) {
            return std::make_shared<Action>(self.clone());
        }, "Clone action")
        // Pythonic wrappers for WorldObject
        .def("get_position", [](WorldObject& self) {
            return std::make_pair(self.x, self.y);
        }, "Get world object position")
        .def("set_position", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set world object position")
        .def("get_radius_squared", [](WorldObject& self) {
            return self.radius * self.radius;
        }, "Get world object radius squared")
        .def("is_within_range", [](WorldObject& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= (radius * radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Check if world object is within range")
        .def("move", [](WorldObject& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
        }, py::arg("dx"), py::arg("dy"), "Move world object")
        .def("rotate", [](WorldObject& self, float angleRadians) {
            // This is a placeholder - actual rotation would depend on coordinate system
            // For now, just track orientation if it exists
            if (self.type == WorldObjectType::Marker) {
                self.orientation += angleRadians;
            }
        }, py::arg("angleRadians"), "Rotate world object")
        // Pythonic wrappers for AgentBody
        .def("get_velocity_magnitude", [](AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get agent body velocity magnitude")
        .def("get_angle", [](AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get agent body direction angle")
        .def("get_distance_to", [](AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance to position")
        .def("is_moving", [](AgentBody& self) {
            return self.velocityX != 0.0f || self.velocityY != 0.0f;
        }, "Check if agent is moving")
        .def("get_movement_vector", [](AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get movement vector")
        .def("reset_movement", [](AgentBody& self) {
            self.velocityX = 0.0f;
            self.velocityY = 0.0f;
            self.angularVelocity = 0.0f;
        }, "Reset movement vectors")
        // Pythonic wrappers for SensoryPercept
        .def("get_vision_data", [](SensoryPercept& self) {
            return self.getVision();
        }, "Get vision data from percept")
        .def("get_touch_data", [](SensoryPercept& self) {
            return self.getTouch();
        }, "Get touch data from percept")
        .def("get_internal_data", [](SensoryPercept& self) {
            return self.getInternal();
        }, "Get internal data from percept")
        .def("get_proprioception_data", [](SensoryPercept& self) {
            return self.getProprioception();
        }, "Get proprioception data from percept")
        .def("get_audio_data", [](SensoryPercept& self) {
            return self.getAudio();
        }, "Get audio data from percept")
        .def("has_vision", [](SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if percept has vision data")
        .def("has_touch", [](SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if percept has touch data")
        .def("has_internal", [](SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if percept has internal data")
        .def("has_proprioception", [](SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, "Check if percept has proprioception data")
        .def("has_audio", [](SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if percept has audio data")
        // Combined sensory methods
        .def("get_all_sensory_data", [](SensoryPercept& self) {
            py::dict result;
            
            auto vision = self.getVision();
            if (!vision.empty()) {
                result["vision"] = vision;
            }
            
            auto touch = self.getTouch();
            if (!touch.empty()) {
                result["touch"] = touch;
            }
            
            auto internal = self.getInternal();
            if (!internal.empty()) {
                result["internal"] = internal;
            }
            
            auto proprioception = self.getProprioception();
            if (!proprioception.empty()) {
                result["proprioception"] = proprioception;
            }
            
            auto audio = self.getAudio();
            if (!audio.empty()) {
                result["audio"] = audio;
            }
            
            return result;
        }, "Get all sensory data as dictionary")
        .def("set_sensory_data", [](SensoryPercept& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = py::str(item.first);
                py::object value = item.second;
                
                if (key == "vision") {
                    std::vector<float> visionData = value.cast<std::vector<float>>();
                    self.setVision(visionData);
                } else if (key == "touch") {
                    std::vector<float> touchData = value.cast<std::vector<float>>();
                    self.setTouch(touchData);
                } else if (key == "internal") {
                    std::vector<float> internalData = value.cast<std::vector<float>>();
                    self.setInternal(internalData);
                } else if (key == "proprioception") {
                    std::vector<float> proprioceptionData = value.cast<std::vector<float>>();
                    self.setProprioception(proprioceptionData);
                } else if (key == "audio") {
                    std::vector<float> audioData = value.cast<std::vector<float>>();
                    self.setAudio(audioData);
                }
            }
        }, py::arg("data"), "Set sensory data from dictionary")
        .def("has_any_sensory_data", [](SensoryPercept& self) {
            return !self.getVision().empty() || 
                   !self.getTouch().empty() || 
                   !self.getInternal().empty() || 
                   !self.getProprioception().empty() || 
                   !self.getAudio().empty();
        }, "Check if percept has any sensory data")
        .def("get_sensory_summary", [](SensoryPercept& self) {
            std::stringstream ss;
            ss << "Sensory Percept: ";
            
            if (!self.getVision().empty()) {
                ss << "Vision(" << self.getVision().size() << "), ";
            }
            if (!self.getTouch().empty()) {
                ss << "Touch(" << self.getTouch().size() << "), ";
            }
            if (!self.getInternal().empty()) {
                ss << "Internal(" << self.getInternal().size() << "), ";
            }
            if (!self.getProprioception().empty()) {
                ss << "Proprioception(" << self.getProprioception().size() << "), ";
            }
            if (!self.getAudio().empty()) {
                ss << "Audio(" << self.getAudio().size() << "), ";
            }
            
            std::string result = ss.str();
            if (result.size() > 2 && result.back() == ' ' && result[result.size() - 2] == ',') {
                result = result.substr(0, result.size() - 2);
            }
            return result;
        }, "Get sensory data summary");
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
