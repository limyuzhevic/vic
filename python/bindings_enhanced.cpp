// Enhanced NLM Python Bindings
// Comprehensive neuroscience simulation framework

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

// Core NLM includes
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include "src/brain/NeuralRegion.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/sensory/Vision.hpp"
#include "src/sensory/Audio.hpp"
#include "src/sensory/InternalSignals.hpp"
#include "src/motor/Action.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Reward.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/Memory.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/prediction/NeuralPrediction.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/cognition/AttentionalSelection.hpp"
#include "src/development/DevelopmentSystem.hpp"
#include "src/development/Synaptogenesis.hpp"
#include "src/development/Pruning.hpp"
#include "src/development/Maturation.hpp"
#include "src/plasticity/StructuralPlasticity.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/performance/CheckpointSystem.hpp"
#include "src/dynamics/SpikeSystem.hpp"
#include "src/dynamics/NeuralDynamics.hpp"
#include "src/environment/Environment.hpp"
#include "src/environment/Observation.hpp"
#include "src/experiments/Experiment.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include "src/experiments/Metrics.hpp"
#include "src/visualization/VisualizationInterface.hpp"

// Helper functions and utilities
namespace {

// Python type conversion helpers
template<typename T>
T from_py_object(pybind11::object obj) {
    return obj.cast<T>();
}

// Configuration batch operations
void config_batch_set(pybind11::object config_obj, std::map<std::string, std::string> settings) {
    auto& config = config_obj.cast<std::shared_ptr<nlm::Config>>();
    for (const auto& pair : settings) {
        config->set(pair.first, pair.second, nlm::ConfigSource::CommandLine);
    }
}

// Simulation controller helper
class SimulationController {
public:
    SimulationController(std::shared_ptr<nlm::Brain> brain,
                         std::shared_ptr<nlm::SimpleWorld> world,
                         std::shared_ptr<nlm::AgentBrain> agent)
        : brain_(brain), world_(world), agent_(agent) {}

    void run_episode(size_t max_steps) {
        brain_->reset();
        brain_->initialize();
        world_->reset();
        agent_->reset();

        for (size_t step = 0; step < max_steps; ++step) {
            // Update world
            world_->update(0.1);

            // Get sensory input
            const auto& percept = world_->getSensoryPercept();
            agent_->processSensoryInput(percept);

            // Brain step
            brain_->step(step);

            // Motor control
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());

            // Neuromodulation (simplified)
            if (agent_->isRewardModulationEnabled()) {
                auto reward = world_->getSensoryPercept().getInternal();
                float reward_val = reward.empty() ? 0.0f : reward[0];
                agent_->applyRewardModulation(reward_val, 0.0f);
            }

            // Development update
            if (agent_->isDevelopmentEnabled()) {
                agent_->updateDevelopment(0.1);
            }
        }
    }

    void run_curiosity_driven(size_t max_time, float curiosity_threshold = 0.5f) {
        brain_->reset();
        brain_->initialize();
        world_->reset();
        agent_->reset();

        size_t step = 0;
        while (step < max_time) {
            world_->update(0.1);

            const auto& percept = world_->getSensoryPercept();
            agent_->processSensoryInput(percept);
            brain_->step(step);
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());

            // Curiosity-based decision making
            float curiosity_level = agent_->getCuriosityLevel();
            if (curiosity_level > curiosity_threshold) {
                // Encourage exploration
                if (step % 10 == 0) {
                    // Random action for exploration
                    auto random_action = static_cast<nlm::MotorCommand>(
                        std::rand() % static_cast<int>(nlm::MotorCommand::Wait));
                    world_->applyMotorCommand(random_action, world_->getSimulationTime());
                }
            }

            step++;
        }
    }

    void run_with_memory_management(float memory_threshold = 0.8f) {
        brain_->reset();
        brain_->initialize();
        world_->reset();
        agent_->reset();

        size_t step = 0;
        while (true) {
            world_->update(0.1);
            const auto& percept = world_->getSensoryPercept();
            agent_->processSensoryInput(percept);
            brain_->step(step);
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());

            // Memory management check
            if (agent_->getWorkingMemory()) {
                auto traces = agent_->getWorkingMemory()->getActiveTraces();
                size_t total_capacity = 1000; // Example capacity
                if (static_cast<float>(traces) / total_capacity > memory_threshold) {
                    // Clear working memory
                    agent_->getWorkingMemory()->clear();
                }
            }

            step++;
            if (step > 10000) break; // Safety limit
        }
    }

private:
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::SimpleWorld> world_;
    std::shared_ptr<nlm::AgentBrain> agent_;
};

// Data export utilities
void export_neural_data(std::shared_ptr<nlm::Brain> brain, const std::string& filename, size_t window = 1000) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    file << "step,total_spikes,active_neurons,firing_rate,e_i_ratio\n";

    for (size_t step = 0; step < window; ++step) {
        brain_->step(step);
        file << step << ","
             << brain_->getTotalSpikeCount() << ","
             << brain_->getActiveNeuronCount() << ","
             << brain_->getAverageFiringRate() << ","
             << brain_->getExcitationInhibitionRatio() << "\n";
    }
}

void export_synapse_weights(std::shared_ptr<nlm::Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    file << "source,destination,weight,delay,type\n";

    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            file << syn->getSourceNeuron().index() << ","
                 << syn->getDestinationNeuron().index() << ","
                 << syn->getWeight() << ","
                 << syn->getDelay() << ","
                 << static_cast<int>(syn->getType()) << "\n";
        }
    }
}

// Visualization helper
class NeuralVisualization {
public:
    static void plot_firing_rates(std::shared_ptr<nlm::Brain> brain, const std::string& filename) {
        // Simple CSV output for external plotting
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }

        file << "neuron_id,firing_rate,activity\n";

        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    file << neuron->getId().index() << ","
                         << neuron->getFiringRate() << ","
                         << (std::abs(neuron->getMembranePotential() - 
                                    neuron->getRestingPotential()) / 20.0f) << "\n";
                }
            }
        }
    }
};

} // namespace

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        -----------------------------
        Comprehensive neuroscience simulation framework with
        neural plasticity, memory systems, neuromodulation,
        and cognitive modeling capabilities.
    )pbdoc";

    // Error types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::logic_error>(m, "LogicError");

    // Core types
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

    // Enhanced Neuron class with full LIF dynamics
    py::class_<Neuron, std::shared_ptr<Neuron>>(m, "Neuron", R"pbdoc(Individual LIF neuron with full neuroscience dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId)
        .def("getType", &Neuron::getType)
        .def("setType", &Neuron::setType, py::arg("type"))
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"))
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"))
        .def("getTotalCurrent", &Neuron::getTotalCurrent)
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent)
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"))
        .def("checkThreshold", &Neuron::checkThreshold)
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime)
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"))
        .def("getSpikeHistory", &Neuron::getSpikeHistory)
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory)
        .def("reset", &Neuron::reset)
        .def("enablePlasticity", &Neuron::enablePlasticity,
             py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("getFiringState", &Neuron::getFiringState)
        .def("isFiring", &Neuron::isFiring)
        .def("isRefractory", &Neuron::isRefractory)
        .def("getState", &Neuron::getState)
        .def("setState", &Neuron::setState, py::arg("state"))
        .def("getRegionId", &Neuron::getRegionId)
        .def("setRegionId", &Neuron::setRegionId, py::arg("regionId"))
        .def("getPopulationId", &Neuron::getPopulationId)
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("popId"));

    // Synapse class with full plasticity support
    py::class_<Synapse, std::shared_ptr<Synapse>>(m, "Synapse", R"pbdoc(Synaptic connection with plasticity and delays)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(),
             py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId)
        .def("getSourceNeuron", &Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron)
        .def("getWeight", &Synapse::getWeight)
        .def("setWeight", &Synapse::setWeight, py::arg("weight"))
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"))
        .def("getDelay", &Synapse::getDelay)
        .def("setDelay", &Synapse::setDelay, py::arg("delay"))
        .def("getType", &Synapse::getType)
        .def("setType", &Synapse::setType, py::arg("type"))
        .def("isExcitatory", &Synapse::isExcitatory)
        .def("isInhibitory", &Synapse::isInhibitory)
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"))
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"))
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory)
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory)
        .def("clearHistory", &Synapse::clearHistory)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"))
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("reset", &Synapse::reset)
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags);

    // Neural population
    py::class_<NeuralPopulation, std::shared_ptr<NeuralPopulation>>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId)
        .def("getSize", &NeuralPopulation::getSize)
        .def("isEmpty", &NeuralPopulation::isEmpty)
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"))
        .def("getNeuronCount", &NeuralPopulation::getSize)
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"))
        .def("createNeurons", &NeuralPopulation::createNeurons, py::arg("startId"))
        .def("getNeurons", &NeuralPopulation::getNeurons)
        .def("getNeuronType", &NeuralPopulation::getNeuronType)
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"))
        .def("getActiveCount", &NeuralPopulation::getActiveCount)
        .def("getFiringCount", &NeuralPopulation::getFiringCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel)
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"))
        .def("step", &NeuralPopulation::step, py::arg("currentTime"))
        .def("reset", &NeuralPopulation::reset)
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds);

    // Neural region
    py::class_<NeuralRegion, std::shared_ptr<NeuralRegion>>(m, "NeuralRegion", R"pbdoc(Brain region containing neural populations)pbdoc")
        .def(py::init<RegionId, const std::string&>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId)
        .def("getName", &NeuralRegion::getName)
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("population"))
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"))
        .def("addNeuron", &NeuralRegion::addNeuron, py::arg("neuron"))
        .def("getAllNeurons", &NeuralRegion::getAllNeurons)
        .def("getPopulations", &NeuralRegion::getPopulations)
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight)
        .def("step", &NeuralRegion::step, py::arg("currentTime"))
        .def("reset", &NeuralRegion::reset);

    // Enhanced Config class
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration system for NLM)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"), "Load configuration from arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), "Save configuration to file")
        .def("has", &Config::has, py::arg("key"), "Check if key exists")
        .def("get", [](const Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (!val) {
                py::object py_none = py::none();
                return py_none;
            }
            return *val;
        }, py::arg("key"), "Get configuration value")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto val = self.get<int>(key);
            if (!val) {
                py::object py_none = py::none();
                return py_none;
            }
            return *val;
        }, py::arg("key"), "Get integer configuration value")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto val = self.get<float>(key);
            if (!val) {
                py::object py_none = py::none();
                return py_none;
            }
            return *val;
        }, py::arg("key"), "Get float configuration value")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            if (!val) {
                py::object py_none = py::none();
                return py_none;
            }
            return *val;
        }, py::arg("key"), "Get boolean configuration value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration")
        .def("summary", &Config::summary, "Get configuration summary")
        .def("batch_set", &config_batch_set, py::arg("settings"), "Batch set multiple configuration values")
        .def("validate", &Config::validate, "Validate configuration");

    // Working memory system
    py::class_<NeuralWorkingMemory>(m, "WorkingMemory", R"pbdoc(Short-term memory with trace maintenance)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"))
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuronId"), py::arg("activation"))
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"))
        .def("hasTrace", &NeuralWorkingMemory::hasTrace, py::arg("neuronId"))
        .def("getActivation", &NeuralWorkingMemory::getActivation, py::arg("neuronId"))
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("getMemoryActivations", &NeuralWorkingMemory::getMemoryActivations)
        .def("getMemoryTimestamps", &NeuralWorkingMemory::getMemoryTimestamps)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("clear", &NeuralWorkingMemory::clear)
        .def("restoreSnapshot", &NeuralWorkingMemory::restoreSnapshot,
             py::arg("neurons"), py::arg("activations"), py::arg("timestamps"));

    // Episodic memory system
    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Long-term memory with episode storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("setMaxEpisodes", &NeuralEpisodicMemory::setMaxEpisodes, py::arg("maxEpisodes"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("getEpisodes", &NeuralEpisodicMemory::getEpisodes)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"))
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"))
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("restoreEpisodes", &NeuralEpisodicMemory::restoreEpisodes, py::arg("episodes"));

    // Neuromodulation systems
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Reward prediction error signal)pbdoc")
        .def(py::init<>())
        .def("update", &Dopamine::update, py::arg("dt"))
        .def("getLevel", &Dopamine::getLevel)
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor);

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Exploration drive and novelty seeking)pbdoc")
        .def(py::init<>())
        .def("update", &Curiosity::update,
             py::arg("novelty"), py::arg("predictionError"), py::arg("dt"))
        .def("getLevel", &Curiosity::getLevel);

    py::class_<NoveltyDetector>(m, "NoveltyDetector", R"pbdoc(Change detection and surprise signals)pbdoc")
        .def(py::init<>())
        .def("update", &NoveltyDetector::update, py::arg("observation"), py::arg("dt"))
        .def("getLevel", &NoveltyDetector::getLevel);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signals for learning)pbdoc")
        .def(py::init<>())
        .def("update", &PredictionError::update, py::arg("predicted"), py::arg("actual"), py::arg("dt"))
        .def("getLevel", &PredictionError::getLevel);

    // Prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Forward models and prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"))
        .def("updatePrediction", &PredictionSystem::updatePrediction,
             py::arg("observation"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getPredictedState", &PredictionSystem::getPredictedState);

    // Enhanced AgentBrain with memory access
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface with memory access)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"))
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getWorkingMemory", &AgentBrain::getWorkingMemory, py::return_value_policy::reference_internal)
        .def("getEpisodicMemory", &AgentBrain::getEpisodicMemory, py::return_value_policy::reference_internal)
        .def("getPredictionSystem", &AgentBrain::getPredictionSystem, py::return_value_policy::reference_internal);

    // Enhanced Brain with development access
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Complete neural simulation brain)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize brain")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform simulation step with time")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"))
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"))
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"))
        .def("produceAction", &Brain::produceAction, "Produce motor action")
        .def("reset", &Brain::reset, "Reset brain")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state")
        .def("addRegion", &Brain::addRegion, py::arg("name"), "Add brain region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"), py::return_value_policy::reference_internal)
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
        .def("logStatus", &Brain::logStatus)
        .def("getWorkingMemory", &Brain::getWorkingMemory, py::return_value_policy::reference_internal)
        .def("getEpisodicMemory", &Brain::getEpisodicMemory, py::return_value_policy::reference_internal)
        .def("getPredictionSystem", &Brain::getPredictionSystem, py::return_value_policy::reference_internal)
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem, py::return_value_policy::reference_internal)
        .def("getCuriosity", &Brain::getCuriosity, py::return_value_policy::reference_internal)
        .def("getNoveltyDetector", &Brain::getNoveltyDetector, py::return_value_policy::reference_internal)
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal, py::return_value_policy::reference_internal)
        .def("getLearnedPreference", &Brain::getLearnedPreference)
        .def("setLearnedPreference", &Brain::setLearnedPreference, py::arg("preference"));

    // Enhanced SimpleWorld
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D simulation world)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
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
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getVisionWidth", &SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &SimpleWorld::getVisionHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Simulation controller factory
    m.def("createSimulationController",
          [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
              return std::make_shared<SimulationController>(brain, world, agent);
          },
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          "Create a simulation controller for episode management");

    // Data export functions
    m.def("export_neural_data",
          &export_neural_data,
          py::arg("brain"), py::arg("filename"), py::arg("window") = 1000,
          "Export neural activity data to CSV");

    m.def("export_synapse_weights",
          &export_synapse_weights,
          py::arg("brain"), py::arg("filename"),
          "Export synaptic connections to CSV");

    // Visualization functions
    m.def("plot_firing_rates",
          &NeuralVisualization::plot_firing_rates,
          py::arg("brain"), py::arg("filename"),
          "Plot firing rates for all neurons");

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

    // Version and utility attributes
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__license__") = "MIT";

    // Type aliases for user convenience
    py::class_<SimulationStep>(m, "SimulationStep", R"pbdoc(Simulation step identifier)")
        .def(py::init<uint64_t>(), py::arg("value"));

    py::class_<Timestamp>(m, "Timestamp", R"pbdoc(Simulation time in seconds)")
        .def(py::init<double>(), py::arg("value"));

    // Action types
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Motor action types)")
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

    // Developmental stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage)")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Neuron types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron types)")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse types
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse types)")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // World object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object types)")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
