//=============================================================================
// Python Bindings for NLM (Neural Learning Machine)
//=============================================================================

/**
 * Complete Python bindings infrastructure for NLM
 * Provides Python interface to the C++ neural simulation framework
 * Includes comprehensive wrapper classes and utilities
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <pybind11/chrono.h>

// Core NLM headers
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuralRegion.hpp"
#include "dynamics/NeuralDynamics.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/SemanticMemory.hpp"
#include "memory/ProceduralMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "sensory/SensoryInput.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "motor/MotorSystem.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "environment/Observation.hpp"
#include "environment/Action.hpp"
#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

using namespace nlm;

namespace py = pybind11;

//=============================================================================
// UTILITY FUNCTIONS AND HELPERS
//=============================================================================

namespace pybind11 {
    // Custom type casters for NLM types
    template<typename T>
    struct enum_caster : public std::underlying_type<T> {
        static py::object convert(T src) {
            return py::int_(static_cast<typename std::underlying_type<T>::type>(src));
        }
    };
}

// NLM-specific type aliases and utilities
using namespace nlm;

// Forward declarations for circular dependencies
class PyBrainWrapper;
class PyNeuronWrapper;
class PySynapseWrapper;

//=============================================================================
// CORE SYSTEM WRAPPERS
//=============================================================================

class PyConfigWrapper {
public:
    PyConfigWrapper() : config(std::make_shared<Config>()) {}
    PyConfigWrapper(const std::string& filename) : config(std::make_shared<Config>()) {
        if (!config->loadFromFile(filename)) {
            throw std::runtime_error("Failed to load config file: " + filename);
        }
    }
    
    void set(const std::string& key, const std::string& value) {
        config->set(key, value, ConfigSource::User);
    }
    
    void setInt(const std::string& key, int64_t value) {
        config->set(key, value, ConfigSource::User);
    }
    
    void setFloat(const std::string& key, float value) {
        config->set(key, value, ConfigSource::User);
    }
    
    void setDouble(const std::string& key, double value) {
        config->set(key, value, ConfigSource::User);
    }
    
    std::string getString(const std::string& key) const {
        return config->getOr<std::string>(key, "");
    }
    
    int64_t getInt(const std::string& key) const {
        return config->getOr<int64_t>(key, 0);
    }
    
    float getFloat(const std::string& key) const {
        return config->getOr<float>(key, 0.0f);
    }
    
    double getDouble(const std::string& key) const {
        return config->getOr<double>(key, 0.0);
    }
    
    bool loadFromFile(const std::string& filename) {
        return config->loadFromFile(filename);
    }
    
    bool saveToFile(const std::string& filename) const {
        return config->saveToFile(filename);
    }
    
    std::shared_ptr<Config> getConfig() const { return config; }
    
    py::dict toDict() const {
        py::dict result;
        // Implement serialization to dictionary
        return result;
    }
    
private:
    std::shared_ptr<Config> config;
};

class PyRandomWrapper {
public:
    PyRandomWrapper() : random(std::make_shared<Random>()) {}
    PyRandomWrapper(uint64_t seed) : random(std::make_shared<Random>(seed)) {}
    
    int64_t nextInt(int64_t min, int64_t max) {
        return random->nextInt(min, max);
    }
    
    float nextFloat() {
        return random->nextFloat();
    }
    
    float nextFloat(float min, float max) {
        return random->nextFloat(min, max);
    }
    
    double nextDouble() {
        return random->nextDouble();
    }
    
    bool nextBool() {
        return random->nextBool();
    }
    
    void setSeed(uint64_t seed) {
        random->setSeed(seed);
    }
    
    uint64_t getSeed() const {
        return random->getSeed();
    }
    
    std::shared_ptr<Random> getRandom() const { return random; }
    
private:
    std::shared_ptr<Random> random;
};

class PyLoggerWrapper {
public:
    PyLoggerWrapper() : logger(std::make_shared<Logger>()) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
    }
    
    PyLoggerWrapper(LogLevel level) : logger(std::make_shared<Logger>()) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        logger->addLogger(consoleLogger);
    }
    
    void logInfo(const std::string& message) {
        NLM_LOG_INFO(message);
    }
    
    void logWarning(const std::string& message) {
        NLM_LOG_WARNING(message);
    }
    
    void logError(const std::string& message) {
        NLM_LOG_ERROR(message);
    }
    
    void logDebug(const std::string& message) {
        NLM_LOG_DEBUG(message);
    }
    
    void setLevel(LogLevel level) {
        logger->setLevel(level);
    }
    
    LogLevel getLevel() const {
        return logger->getLevel();
    }
    
    std::shared_ptr<Logger> getLogger() const { return logger; }
    
    void addFileLogger(const std::string& filename, LogLevel level) {
        auto fileLogger = std::make_shared<FileLogger>(filename, level);
        logger->addLogger(fileLogger);
    }
    
private:
    std::shared_ptr<Logger> logger;
};

class PySimulationClockWrapper {
public:
    PySimulationClockWrapper(double timestep) : clock(std::make_shared<SimulationClock>(timestep)) {}
    
    double getTimeStep() const {
        return clock->getTimeStep();
    }
    
    double getCurrentTime() const {
        return clock->getCurrentTime();
    }
    
    SimulationStep getCurrentStep() const {
        return clock->getCurrentStep();
    }
    
    void advance(double seconds) {
        clock->advance(seconds);
    }
    
    void step() {
        clock->step();
    }
    
    std::shared_ptr<SimulationClock> getClock() const { return clock; }
    
private:
    std::shared_ptr<SimulationClock> clock;
};

//=============================================================================
// NEURAL COMPONENTS WRAPPERS
//=============================================================================

class PyNeuronWrapper {
public:
    PyNeuronWrapper(std::shared_ptr<Neuron> neuron) : neuron(neuron) {}
    
    // Basic neuron properties
    NeuronId getId() const { return neuron->getId(); }
    NeuronType getType() const { return neuron->getType(); }
    double getMembranePotential() const { return neuron->getMembranePotential(); }
    double getThreshold() const { return neuron->getThreshold(); }
    double getRestPotential() const { return neuron->getRestPotential(); }
    double getRefractoryPeriod() const { return neuron->getRefractoryPeriod(); }
    
    // Dynamics
    void injectCurrent(double current) { neuron->injectCurrent(current); }
    void update(double timestep) { neuron->update(timestep); }
    bool isSpiking() const { return neuron->isSpiking(); }
    bool isInRefractory() const { return neuron->isInRefractory(); }
    
    // Plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool structural) {
        neuron->enablePlasticity(hebbian, stdp, structural);
    }
    
    // State
    bool isActive() const { return neuron->isActive(); }
    int getSpikeCount() const { return neuron->getSpikeCount(); }
    double getFiringRate() const { return neuron->getFiringRate(); }
    
    // Connectivity
    size_t getInputConnectionCount() const { return neuron->getInputConnectionCount(); }
    size_t getOutputConnectionCount() const { return neuron->getOutputConnectionCount(); }
    
    std::shared_ptr<Neuron> getNeuron() const { return neuron; }
    
private:
    std::shared_ptr<Neuron> neuron;
};

class PySynapseWrapper {
public:
    PySynapseWrapper(std::shared_ptr<Synapse> synapse) : synapse(synapse) {}
    
    // Basic properties
    SynapseId getId() const { return synapse->getId(); }
    double getWeight() const { return synapse->getWeight(); }
    double getDelay() const { return synapse->getDelay(); }
    double getMaxWeight() const { return synapse->getMaxWeight(); }
    double getMinWeight() const { return synapse->getMinWeight(); }
    
    // Pre- and post-synaptic neurons
    NeuronId getPreSynapseId() const { return synapse->getPreSynapseId(); }
    NeuronId getPostSynapseId() const { return synapse->getPostSynapseId(); }
    
    // Plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool structural) {
        synapse->enablePlasticity(hebbian, stdp, structural);
    }
    
    void setWeight(double weight) { synapse->setWeight(weight); }
    void updateWeight(double dt) { synapse->updateWeight(dt); }
    
    // State
    bool isPlastic() const { return synapse->isPlastic(); }
    bool isActive() const { return synapse->isActive(); }
    
    std::shared_ptr<Synapse> getSynapse() const { return synapse; }
    
private:
    std::shared_ptr<Synapse> synapse;
};

class PyNeuralPopulationWrapper {
public:
    PyNeuralPopulationWrapper(std::shared_ptr<NeuralPopulation> population) 
        : population(population) {}
    
    std::string getId() const { return population->getId(); }
    std::vector<PyNeuronWrapper> getNeurons() {
        std::vector<PyNeuronWrapper> neurons;
        auto rawNeurons = population->getNeurons();
        for (auto& neuron : rawNeurons) {
            neurons.emplace_back(neuron);
        }
        return neurons;
    }
    
    size_t getNeuronCount() const { return population->getNeuronCount(); }
    double getAverageFiringRate() const { return population->getAverageFiringRate(); }
    size_t getSpikeCount() const { return population->getSpikeCount(); }
    
    std::shared_ptr<NeuralPopulation> getPopulation() const { return population; }
    
private:
    std::shared_ptr<NeuralPopulation> population;
};

class PyNeuralRegionWrapper {
public:
    PyNeuralRegionWrapper(std::shared_ptr<NeuralRegion> region) : region(region) {}
    
    RegionId getId() const { return region->getId(); }
    std::string getType() const { return region->getType(); }
    
    PyNeuralPopulationWrapper getPopulation(size_t index) {
        return PyNeuralPopulationWrapper(region->getPopulation(index));
    }
    
    std::vector<PyNeuralPopulationWrapper> getPopulations() {
        std::vector<PyNeuralPopulationWrapper> populations;
        auto rawPopulations = region->getPopulations();
        for (auto& population : rawPopulations) {
            populations.emplace_back(population);
        }
        return populations;
    }
    
    size_t getPopulationCount() const { return region->getPopulationCount(); }
    double getTotalActivity() const { return region->getTotalActivity(); }
    
    std::shared_ptr<NeuralRegion> getRegion() const { return region; }
    
private:
    std::shared_ptr<NeuralRegion> region;
};

//=============================================================================
// BRAIN AND AGENT WRAPPERS
//=============================================================================

class PyBrainWrapper {
public:
    PyBrainWrapper(std::shared_ptr<Brain> brain) : brain(brain) {}
    
    // Core functionality
    bool initialize() { return brain->initialize(); }
    bool reset() { return brain->reset(); }
    
    // Simulation control
    void step(SimulationStep step, double time) { brain->step(step, time); }
    
    // Accessors
    PyNeuralRegionWrapper getRegion(size_t index) {
        return PyNeuralRegionWrapper(brain->getRegion(RegionId(index)));
    }
    
    std::vector<PyNeuralRegionWrapper> getRegions() {
        std::vector<PyNeuralRegionWrapper> regions;
        auto rawRegions = brain->getRegions();
        for (auto& region : rawRegions) {
            regions.emplace_back(region);
        }
        return regions;
    }
    
    size_t getRegionCount() const { return brain->getRegionCount(); }
    size_t getNeuronCount() const { return brain->getNeuronCount(); }
    size_t getSynapseCount() const { return brain->getTotalSynapseCount(); }
    
    // Statistics
    double getTotalSpikeCount() const { return brain->getTotalSpikeCount(); }
    double getAverageFiringRate() const { return brain->getAverageFiringRate(); }
    size_t getFiringNeuronCount() const { return brain->getFiringNeuronCount(); }
    
    // Memory systems
    std::shared_ptr<WorkingMemory> getWorkingMemory() const { return brain->getWorkingMemory(); }
    std::shared_ptr<EpisodicMemory> getEpisodicMemory() const { return brain->getEpisodicMemory(); }
    std::shared_ptr<AssociativeMemory> getAssociativeMemory() const { return brain->getAssociativeMemory(); }
    
    // Neuromodulation
    std::shared_ptr<Neuromodulator> getDopamine() const { return brain->getDopamine(); }
    std::shared_ptr<Neuromodulator> getCuriosity() const { return brain->getCuriosity(); }
    std::shared_ptr<Neuromodulator> getNovelty() const { return brain->getNovelty(); }
    
    // Prediction
    std::shared_ptr<PredictionSystem> getPredictionSystem() const { return brain->getPredictionSystem(); }
    
    // Development
    std::shared_ptr<DevelopmentSystem> getDevelopmentSystem() const { return brain->getDevelopmentSystem(); }
    
    // Cognition
    std::shared_ptr<NeuralPlanner> getPlanner() const { return brain->getPlanner(); }
    std::shared_ptr<ConceptFormation> getConceptFormation() const { return brain->getConceptFormation(); }
    std::shared_ptr<Attention> getAttention() const { return brain->getAttention(); }
    
    // Sensory/Motor
    std::shared_ptr<SensoryInput> getSensoryInput() const { return brain->getSensoryInput(); }
    std::shared_ptr<MotorSystem> getMotorSystem() const { return brain->getMotorSystem(); }
    
    // Environment
    std::shared_ptr<Environment> getEnvironment() const { return brain->getEnvironment(); }
    
    // Saving/Loading
    bool save(const std::string& filename) const { return brain->save(filename); }
    bool load(const std::string& filename) { return brain->load(filename); }
    
    // Configuration
    std::shared_ptr<Config> getConfig() const { return brain->getConfig(); }
    
    // Log status
    void logStatus() { brain->logStatus(); }
    
    std::shared_ptr<Brain> getBrain() const { return brain; }
    
private:
    std::shared_ptr<Brain> brain;
};

//=============================================================================
// ENVIRONMENT AND SENSORIMOTOR WRAPPERS
//=============================================================================

class PyEnvironmentWrapper {
public:
    PyEnvironmentWrapper(std::shared_ptr<Environment> env) : environment(env) {}
    
    bool initialize(int width, int height) {
        // Cast to SimpleWorld for initialization
        auto simpleWorld = std::dynamic_pointer_cast<SimpleWorld>(environment);
        if (simpleWorld) {
            return simpleWorld->initialize(width, height);
        }
        return false;
    }
    
    // Observation processing
    SensoryPercept observe() {
        return environment->observe();
    }
    
    // Action processing
    void applyAction(const MotorCommand& action) {
        environment->applyAction(action);
    }
    
    // Reward computation
    float computeReward() {
        return environment->computeReward();
    }
    
    // Environment properties
    int getWidth() const {
        auto simpleWorld = std::dynamic_pointer_cast<SimpleWorld>(environment);
        return simpleWorld ? simpleWorld->getWidth() : 0;
    }
    
    int getHeight() const {
        auto simpleWorld = std::dynamic_pointer_cast<SimpleWorld>(environment);
        return simpleWorld ? simpleWorld->getHeight() : 0;
    }
    
    std::shared_ptr<Environment> getEnvironment() const { return environment; }
    
private:
    std::shared_ptr<Environment> environment;
};

class PyAgentWrapper {
public:
    PyAgentWrapper(std::shared_ptr<AgentBrain> agent) : agent(agent) {}
    
    void initialize(PyEnvironmentWrapper& environment) {
        agent->initialize(*(environment.getEnvironment()));
    }
    
    void processSensoryInput(const SensoryPercept& percept) {
        agent->processSensoryInput(percept);
    }
    
    MotorCommand decodeMotorCommand() {
        return agent->decodeMotorCommand();
    }
    
    PyBrainWrapper getBrain() {
        return PyBrainWrapper(agent->getBrain());
    }
    
    void enableRewardModulation(bool enable) {
        agent->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agent->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agent->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agent->enableCuriosity(enable);
    }
    
    void applyRewardModulation(float reward, float predictionError) {
        agent->applyRewardModulation(reward, predictionError);
    }
    
    void updateDevelopment(double dt) {
        agent->updateDevelopment(dt);
    }
    
    // Neuromodulation levels
    double getDopamineLevel() const { return agent->getNeuromodulationLevel(); }
    double getCuriosityLevel() const { return agent->getCuriosityLevel(); }
    double getNoveltyLevel() const { return agent->getNoveltyLevel(); }
    
    // States
    bool isLearning() const { return agent->isLearning(); }
    bool isExploring() const { return agent->isExploring(); }
    
    std::shared_ptr<AgentBrain> getAgent() const { return agent; }
    
private:
    std::shared_ptr<AgentBrain> agent;
};

//=============================================================================
// EXPERIMENT WRAPPERS
//=============================================================================

class PyPhase6IntegratedExperimentWrapper {
public:
    PyPhase6IntegratedExperimentWrapper() : experiment(std::make_shared<Phase6IntegratedExperiment>()) {}
    
    bool verifyIntegration() {
        return experiment->verifyIntegration();
    }
    
    bool testMemoryIntegration() {
        return experiment->testMemoryIntegration();
    }
    
    bool testNeuromodulationIntegration() {
        return experiment->testNeuromodulationIntegration();
    }
    
    bool testCheckpointing() {
        return experiment->testCheckpointing();
    }
    
    bool testReplay() {
        return experiment->testReplay();
    }
    
    // Run the main experiment
    py::dict run(PPhase6Config config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegrationResult result = experiment->run(*config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        py::dict resultDict;
        resultDict["total_reward"] = result.totalReward;
        resultDict["avg_firing_rate"] = result.avgFiringRate;
        resultDict["memory_episodes_stored"] = result.memoryEpisodesStored;
        resultDict["dopamine_level"] = result.dopamineLevel;
        resultDict["novelty_level"] = result.noveltyLevel;
        resultDict["curiosity_level"] = result.curiosityLevel;
        resultDict["total_wall_clock_time"] = result.totalWallClockTime;
        resultDict["execution_time"] = executionTime;
        
        // Integration status
        py::dict integrationDict;
        integrationDict["working_memory"] = result.memoryWorkingMemoryIntegrated;
        integrationDict["episodic_memory"] = result.memoryEpisodicMemoryIntegrated;
        integrationDict["neuromodulation"] = result.neuromodulationIntegrated;
        integrationDict["prediction"] = result.predictionIntegrated;
        integrationDict["development"] = result.developmentIntegrated;
        integrationDict["checkpointing"] = result.checkpointingWorks;
        
        resultDict["integration_status"] = integrationDict;
        
        return resultDict;
    }
    
    std::shared_ptr<Phase6IntegratedExperiment> getExperiment() const { return experiment; }
    
private:
    std::shared_ptr<Phase6IntegratedExperiment> experiment;
};

class PyPhase6ConfigWrapper {
public:
    PyPhase6ConfigWrapper() : config(std::make_shared<Phase6Config>()) {}
    
    // Setters
    void setNeuronCount(int count) { config->neuronCount = count; }
    void setMaxSteps(int steps) { config->maxSteps = steps; }
    void setRegionCount(int count) { config->regionCount = count; }
    void setConnectionProbability(float prob) { config->connectionProbability = prob; }
    
    void enableCheckpointing(bool enable) { config->enableCheckpointing = enable; }
    void enableReplay(bool enable) { config->enableReplay = enable; }
    void enableDevelopment(bool enable) { config->enableDevelopment = enable; }
    
    // Getters
    int getNeuronCount() const { return config->neuronCount; }
    int getMaxSteps() const { return config->maxSteps; }
    int getRegionCount() const { return config->regionCount; }
    float getConnectionProbability() const { return config->connectionProbability; }
    
    bool isCheckpointingEnabled() const { return config->enableCheckpointing; }
    bool isReplayEnabled() const { return config->enableReplay; }
    bool isDevelopmentEnabled() const { return config->enableDevelopment; }
    
    // Configuration file
    bool loadFromFile(const std::string& filename) {
        auto cfg = std::make_shared<Config>();
        if (cfg->loadFromFile(filename)) {
            // Copy values to our config
            config->neuronCount = cfg->getOr<int64_t>("neuron_count", config->neuronCount);
            config->maxSteps = cfg->getOr<int64_t>("max_steps", config->maxSteps);
            config->regionCount = cfg->getOr<int64_t>("region_count", config->regionCount);
            config->connectionProbability = cfg->getOr<float>("connection_probability", config->connectionProbability);
            return true;
        }
        return false;
    }
    
    std::shared_ptr<Phase6Config> getConfig() const { return config; }
    
private:
    std::shared_ptr<Phase6Config> config;
};

//=============================================================================
// PYTHON MODULE DEFINITION
//=============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine) - Advanced AI/ML Framework";
    
    // Version information
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__author__") = "NLM Project Team";
    m.attr("__description__") = "Experimental artificial developmental brain simulation framework";
    
    // Enums
    py::enum_<LogLevel>(m, "LogLevel")
        .value("DEBUG", LogLevel::Debug)
        .value("INFO", LogLevel::Info)
        .value("WARNING", LogLevel::Warning)
        .value("ERROR", LogLevel::Error)
        .value("CRITICAL", LogLevel::Critical)
        .export_values();
    
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .export_values();
    
    py::enum_<ConfigSource>(m, "ConfigSource")
        .value("Default", ConfigSource::Default)
        .value("User", ConfigSource::User)
        .value("File", ConfigSource::File)
        .export_values();
    
    // Basic type casters for std::shared_ptr
    py::class_<std::shared_ptr<Config>>(m, "ConfigPtr")
        .def(py::init<>());
    
    // Core system wrappers
    py::class_<PyConfigWrapper>(m, "Config")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("set", &PyConfigWrapper::set)
        .def("set_int", &PyConfigWrapper::setInt)
        .def("set_float", &PyConfigWrapper::setFloat)
        .def("set_double", &PyConfigWrapper::setDouble)
        .def("get_string", &PyConfigWrapper::getString)
        .def("get_int", &PyConfigWrapper::getInt)
        .def("get_float", &PyConfigWrapper::getFloat)
        .def("get_double", &PyConfigWrapper::getDouble)
        .def("load_from_file", &PyConfigWrapper::loadFromFile)
        .def("save_to_file", &PyConfigWrapper::saveToFile);
    
    py::class_<PyRandomWrapper>(m, "Random")
        .def(py::init<>())
        .def(py::init<uint64_t>())
        .def("next_int", &PyRandomWrapper::nextInt)
        .def("next_float", &PyRandomWrapper::nextFloat)
        .def("next_float", py::overload_cast<float, float>(&PyRandomWrapper::nextFloat))
        .def("next_double", &PyRandomWrapper::nextDouble)
        .def("next_bool", &PyRandomWrapper::nextBool)
        .def("set_seed", &PyRandomWrapper::setSeed)
        .def("get_seed", &PyRandomWrapper::getSeed);
    
    py::class_<PyLoggerWrapper>(m, "Logger")
        .def(py::init<>())
        .def(py::init<LogLevel>())
        .def("log_info", &PyLoggerWrapper::logInfo)
        .def("log_warning", &PyLoggerWrapper::logWarning)
        .def("log_error", &PyLoggerWrapper::logError)
        .def("log_debug", &PyLoggerWrapper::logDebug)
        .def("set_level", &PyLoggerWrapper::setLevel)
        .def("get_level", &PyLoggerWrapper::getLevel)
        .def("add_file_logger", &PyLoggerWrapper::addFileLogger);
    
    py::class_<PySimulationClockWrapper>(m, "SimulationClock")
        .def(py::init<double>())
        .def("get_time_step", &PySimulationClockWrapper::getTimeStep)
        .def("get_current_time", &PySimulationClockWrapper::getCurrentTime)
        .def("get_current_step", &PySimulationClockWrapper::getCurrentStep)
        .def("advance", &PySimulationClockWrapper::advance)
        .def("step", &PySimulationClockWrapper::step);
    
    // Neural component wrappers
    py::class_<PyNeuronWrapper>(m, "Neuron")
        .def("get_id", &PyNeuronWrapper::getId)
        .def("get_type", &PyNeuronWrapper::getType)
        .def("get_membrane_potential", &PyNeuronWrapper::getMembranePotential)
        .def("get_threshold", &PyNeuronWrapper::getThreshold)
        .def("get_rest_potential", &PyNeuronWrapper::getRestPotential)
        .def("get_refractory_period", &PyNeuronWrapper::getRefractoryPeriod)
        .def("inject_current", &PyNeuronWrapper::injectCurrent)
        .def("update", &PyNeuronWrapper::update)
        .def("is_spiking", &PyNeuronWrapper::isSpiking)
        .def("is_in_refractory", &PyNeuronWrapper::isInRefractory)
        .def("enable_plasticity", &PyNeuronWrapper::enablePlasticity)
        .def("is_active", &PyNeuronWrapper::isActive)
        .def("get_spike_count", &PyNeuronWrapper::getSpikeCount)
        .def("get_firing_rate", &PyNeuronWrapper::getFiringRate)
        .def("get_input_connection_count", &PyNeuronWrapper::getInputConnectionCount)
        .def("get_output_connection_count", &PyNeuronWrapper::getOutputConnectionCount);
    
    py::class_<PySynapseWrapper>(m, "Synapse")
        .def("get_id", &PySynapseWrapper::getId)
        .def("get_weight", &PySynapseWrapper::getWeight)
        .def("get_delay", &PySynapseWrapper::getDelay)
        .def("get_max_weight", &PySynapseWrapper::getMaxWeight)
        .def("get_min_weight", &PySynapseWrapper::getMinWeight)
        .def("get_pre_synapse_id", &PySynapseWrapper::getPreSynapseId)
        .def("get_post_synapse_id", &PySynapseWrapper::getPostSynapseId)
        .def("enable_plasticity", &PySynapseWrapper::enablePlasticity)
        .def("set_weight", &PySynapseWrapper::setWeight)
        .def("update_weight", &PySynapseWrapper::updateWeight)
        .def("is_plastic", &PySynapseWrapper::isPlastic)
        .def("is_active", &PySynapseWrapper::isActive);
    
    py::class_<PyNeuralPopulationWrapper>(m, "NeuralPopulation")
        .def("get_id", &PyNeuralPopulationWrapper::getId)
        .def("get_neuron_count", &PyNeuralPopulationWrapper::getNeuronCount)
        .def("get_average_firing_rate", &PyNeuralPopulationWrapper::getAverageFiringRate)
        .def("get_spike_count", &PyNeuralPopulationWrapper::getSpikeCount);
    
    py::class_<PyNeuralRegionWrapper>(m, "NeuralRegion")
        .def("get_id", &PyNeuralRegionWrapper::getId)
        .def("get_type", &PyNeuralRegionWrapper::getType)
        .def("get_population_count", &PyNeuralRegionWrapper::getPopulationCount)
        .def("get_total_activity", &PyNeuralRegionWrapper::getTotalActivity);
    
    // Brain wrapper
    py::class_<PyBrainWrapper>(m, "Brain")
        .def("initialize", &PyBrainWrapper::initialize)
        .def("reset", &PyBrainWrapper::reset)
        .def("step", &PyBrainWrapper::step)
        .def("get_region_count", &PyBrainWrapper::getRegionCount)
        .def("get_neuron_count", &PyBrainWrapper::getNeuronCount)
        .def("get_synapse_count", &PyBrainWrapper::getSynapseCount)
        .def("get_total_spike_count", &PyBrainWrapper::getTotalSpikeCount)
        .def("get_average_firing_rate", &PyBrainWrapper::getAverageFiringRate)
        .def("get_firing_neuron_count", &PyBrainWrapper::getFiringNeuronCount)
        .def("get_working_memory", &PyBrainWrapper::getWorkingMemory)
        .def("get_episodic_memory", &PyBrainWrapper::getEpisodicMemory)
        .def("get_associative_memory", &PyBrainWrapper::getAssociativeMemory)
        .def("get_dopamine", &PyBrainWrapper::getDopamine)
        .def("get_curiosity", &PyBrainWrapper::getCuriosity)
        .def("get_novelty", &PyBrainWrapper::getNovelty)
        .def("get_prediction_system", &PyBrainWrapper::getPredictionSystem)
        .def("get_development_system", &PyBrainWrapper::getDevelopmentSystem)
        .def("get_planner", &PyBrainWrapper::getPlanner)
        .def("get_concept_formation", &PyBrainWrapper::getConceptFormation)
        .def("get_attention", &PyBrainWrapper::getAttention)
        .def("get_sensory_input", &PyBrainWrapper::getSensoryInput)
        .def("get_motor_system", &PyBrainWrapper::getMotorSystem)
        .def("get_environment", &PyBrainWrapper::getEnvironment)
        .def("save", &PyBrainWrapper::save)
        .def("load", &PyBrainWrapper::load)
        .def("log_status", &PyBrainWrapper::logStatus)
        .def("get_config", &PyBrainWrapper::getConfig);
    
    // Environment wrapper
    py::class_<PyEnvironmentWrapper>(m, "Environment")
        .def("initialize", &PyEnvironmentWrapper::initialize)
        .def("observe", &PyEnvironmentWrapper::observe)
        .def("apply_action", &PyEnvironmentWrapper::applyAction)
        .def("compute_reward", &PyEnvironmentWrapper::computeReward)
        .def("get_width", &PyEnvironmentWrapper::getWidth)
        .def("get_height", &PyEnvironmentWrapper::getHeight);
    
    // Agent wrapper
    py::class_<PyAgentWrapper>(m, "Agent")
        .def("initialize", &PyAgentWrapper::initialize)
        .def("process_sensory_input", &PyAgentWrapper::processSensoryInput)
        .def("decode_motor_command", &PyAgentWrapper::decodeMotorCommand)
        .def("get_brain", &PyAgentWrapper::getBrain)
        .def("enable_reward_modulation", &PyAgentWrapper::enableRewardModulation)
        .def("enable_structural_plasticity", &PyAgentWrapper::enableStructuralPlasticity)
        .def("enable_development", &PyAgentWrapper::enableDevelopment)
        .def("enable_curiosity", &PyAgentWrapper::enableCuriosity)
        .def("apply_reward_modulation", &PyAgentWrapper::applyRewardModulation)
        .def("update_development", &PyAgentWrapper::updateDevelopment)
        .def("get_dopamine_level", &PyAgentWrapper::getDopamineLevel)
        .def("get_curiosity_level", &PyAgentWrapper::getCuriosityLevel)
        .def("get_novelty_level", &PyAgentWrapper::getNoveltyLevel)
        .def("is_learning", &PyAgentWrapper::isLearning)
        .def("is_exploring", &PyAgentWrapper::isExploring);
    
    // Experiment wrappers
    py::class_<PyPhase6ConfigWrapper>(m, "Phase6Config")
        .def(py::init<>())
        .def("set_neuron_count", &PyPhase6ConfigWrapper::setNeuronCount)
        .def("set_max_steps", &PyPhase6ConfigWrapper::setMaxSteps)
        .def("set_region_count", &PyPhase6ConfigWrapper::setRegionCount)
        .def("set_connection_probability", &PyPhase6ConfigWrapper::setConnectionProbability)
        .def("enable_checkpointing", &PyPhase6ConfigWrapper::enableCheckpointing)
        .def("enable_replay", &PyPhase6ConfigWrapper::enableReplay)
        .def("enable_development", &PyPhase6ConfigWrapper::enableDevelopment)
        .def("load_from_file", &PyPhase6ConfigWrapper::loadFromFile);
    
    py::class_<PyPhase6IntegratedExperimentWrapper>(m, "Phase6IntegratedExperiment")
        .def(py::init<>())
        .def("verify_integration", &PyPhase6IntegratedExperimentWrapper::verifyIntegration)
        .def("test_memory_integration", &PyPhase6IntegratedExperimentWrapper::testMemoryIntegration)
        .def("test_neuromodulation_integration", &PyPhase6IntegratedExperimentWrapper::testNeuromodulationIntegration)
        .def("test_checkpointing", &PyPhase6IntegratedExperimentWrapper::testCheckpointing)
        .def("test_replay", &PyPhase6IntegratedExperimentWrapper::testReplay)
        .def("run", &PyPhase6IntegratedExperimentWrapper::run);
    
    // Convenience factory functions
    m.def("create_brain", [](std::shared_ptr<Config> config) {
        return PyBrainWrapper(std::make_shared<Brain>(config));
    }, py::arg("config") = nullptr);
    
    m.def("create_agent", [](std::shared_ptr<Brain> brain) {
        return PyAgentWrapper(std::make_shared<AgentBrain>(brain));
    }, py::arg("brain") = nullptr);
    
    m.def("create_simple_world", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->initialize(16, 16);
        return PyEnvironmentWrapper(world);
    });
    
    // Utility functions
    m.def("get_version", []() {
        py::dict versionInfo;
        versionInfo["major"] = 0;
        versionInfo["minor"] = 1;
        versionInfo["patch"] = 0;
        versionInfo["build"] = "enhanced";
        versionInfo["full"] = "0.1.0-enhanced";
        return versionInfo;
    });
    
    m.def("get_system_info", []() {
        py::dict info;
        info["name"] = "NLM - Neural Learning Machine";
        info["description"] = "Experimental artificial developmental brain";
        info["architecture"] = "Modular neural architecture with configurable components";
        info["features"] = py::list({
            "Real spiking neurons (LIF model)",
            "Event-driven spike propagation",
            "Multiple plasticity rules (STDP, Hebbian, structural)",
            "Memory systems (working, episodic, associative)",
            "Neuromodulation (dopamine, curiosity, novelty)",
            "Prediction and planning systems",
            "Development and learning mechanisms",
            "Sensory-motor integration",
            "Environment interaction",
            "Checkpoint/restore capabilities",
            "Replay and consolidation"
        });
        return info;
    });
    
    // Example usage function
    m.def("create_example_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return PyBrainWrapper(brain);
        } else {
            throw std::runtime_error("Failed to initialize example brain");
        }
    });
    
    // Exception handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}