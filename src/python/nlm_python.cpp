#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace py = pybind11;

// Python wrapper for Config class
void initConfig(py::module_ &m) {
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile)
        .def("loadFromArgs", &Config::loadFromArgs)
        .def("saveToFile", &Config::saveToFile)
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(
            &Config::set, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
        ))
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(
            &Config::set, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
        ))
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(
            &Config::set, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
        ))
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(
            &Config::set, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
        ))
        .def("get", py::overload_cast<const std::string&>(&Config::get<int>, std::placeholders::_1))
        .def("get", py::overload_cast<const std::string&>(&Config::get<int64_t>, std::placeholders::_1))
        .def("get", py::overload_cast<const std::string&>(&Config::get<double>, std::placeholders::_1))
        .def("get", py::overload_cast<const std::string&>(&Config::get<bool>, std::placeholders::_1))
        .def("get", py::overload_cast<const std::string&>(&Config::get<std::string>, std::placeholders::_1))
        .def("getOr", py::overload_cast<const std::string&, const int&>(&Config::getOr<int>, std::placeholders::_1, std::placeholders::_2))
        .def("getOr", py::overload_cast<const std::string&, const int64_t&>(&Config::getOr<int64_t>, std::placeholders::_1, std::placeholders::_2))
        .def("getOr", py::overload_cast<const std::string&, const double&>(&Config::getOr<double>, std::placeholders::_1, std::placeholders::_2))
        .def("getOr", py::overload_cast<const std::string&, const bool&>(&Config::getOr<bool>, std::placeholders::_1, std::placeholders::_2))
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr<std::string>, std::placeholders::_1, std::placeholders::_2))
        .def("has", &Config::has)
        .def("remove", &Config::remove)
        .def("getKeys", &Config::getKeys)
        .def("clear", &Config::clear)
        .def("summary", &Config::summary)
        .def("createDefaultConfig", []() {
            auto config = std::make_shared<Config>();
            // Set up sensible defaults for a typical NLM brain
            config->set("brain.neuron_count", 500, ConfigSource::Default);
            config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
            config->set("brain.connection_probability", 0.15f, ConfigSource::Default);
            config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
            config->set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
            config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
            config->set("brain.v_rest", -70.0f, ConfigSource::Default);
            config->set("brain.v_reset", -75.0f, ConfigSource::Default);
            config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
            config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
            
            // Plasticity defaults
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
            config->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
            config->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config->set("plasticity.structural.enable", true, ConfigSource::Default);
            
            // Neuromodulation defaults
            config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
            config->set("neuromod.novelty.enable", true, ConfigSource::Default);
            
            return config;
        });
}

// Python wrapper for Neuron
void initNeuron(py::module_ &m) {
    py::class_<Neuron>(m, "Neuron")
        .def("getId", &Neuron::getId)
        .def("getType", &Neuron::getType)
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("getThreshold", &Neuron::getThreshold)
        .def("getRefractoryTime", &Neuron::getRefractoryTime)
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("injectCurrent", &Neuron::injectCurrent)
        .def("step", &Neuron::step);
}

// Python wrapper for Brain
void initBrain(py::module_ &m) {
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain")
        .def("initialize", &Brain::initialize)
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&Brain::step))
        .def("step", py::overload_cast<SimulationStep>(&Brain::step))
        .def("receiveSensoryInput", &Brain::receiveSensoryInput)
        .def("injectCurrent", py::overload_cast<NeuronId, MembranePotential>(&Brain::injectCurrent))
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio)
        .def("getRegion", &Brain::getRegion)
        .def("produceAction", &Brain::produceAction)
        .def("reset", &Brain::reset)
        .def("logStatus", &Brain::logStatus)
        .def("save", &Brain::save)
        .def("load", &Brain::load)
        .def("getSpikeSystem", &Brain::getSpikeSystem)
        .def("getSTDP", &Brain::getSTDP)
        .def("getHebbian", &Brain::getHebbian)
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity);
}

// Python wrapper for AgentBrain
void initAgentBrain(py::module_ &m) {
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain")
        .def("initialize", &AgentBrain::initialize)
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &AgentBrain::updateDevelopment)
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace)
        .def("getLearnedPreference", &AgentBrain::getLearnedPreference)
        .def("getReflexBonusCount", &AgentBrain::getReflexBonusCount)
        .def("getReflexSuppressCount", &AgentBrain::getReflexSuppressCount)
        .def("getWtaPulseCount", &AgentBrain::getWtaPulseCount)
        .def("encodeText", &AgentBrain::encodeText)
        .def("enableRewardModulation", [](AgentBrain& agent, bool enable) {
            // This would need a method to enable/disable subsystems
            // For now, we assume subsystems can be enabled via config
        })
        .def("enableStructuralPlasticity", [](AgentBrain& agent, bool enable) {
            // Similar to above
        })
        .def("enableDevelopment", [](AgentBrain& agent, bool enable) {
            // Similar to above
        })
        .def("enableCuriosity", [](AgentBrain& agent, bool enable) {
            // Similar to above
        });
}

// Python wrapper for SensoryInput
void initSensoryInput(py::module_ &m) {
    py::class_<SensoryInput>(m, "SensoryInput")
        .def(py::init<>())
        .def("setVision", &SensoryInput::setVision)
        .def("setAudio", &SensoryInput::setAudio)
        .def("setInternal", &SensoryInput::setInternal)
        .def("getVision", &SensoryInput::getVision)
        .def("getAudio", &SensoryInput::getAudio)
        .def("getInternal", &SensoryInput::getInternal);
}

// Python wrapper for SensoryPercept
void initSensoryPercept(py::module_ &m) {
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def("getVision", &SensoryPercept::getVision)
        .def("getAudio", &SensoryPercept::getAudio)
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setVision", &SensoryPercept::setVision)
        .def("setAudio", &SensoryPercept::setAudio)
        .def("setInternal", &SensoryPercept::setInternal);
}

// Python wrapper for SimpleWorld
void initSimpleWorld(py::module_ &m) {
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure)
        .def("reset", &SimpleWorld::reset)
        .def("update", &SimpleWorld::update)
        .def("setAgentStart", &SimpleWorld::setAgentStart)
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand)
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept)
        .def("getAgentBody", &SimpleWorld::getAgentBody)
        .def("addObject", &SimpleWorld::addObject)
        .def("removeObject", &SimpleWorld::removeObject)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed);
}

// Python wrapper for Action
void initAction(py::module_ &m) {
    py::class_<Action>(m, "Action")
        .def("getType", &Action::getType)
        .def("getPower", &Action::getPower)
        .def("getParameters", &Action::getParameters);
}

// Python wrapper for MotorCommand
void initMotorCommand(py::module_ &m) {
    py::class_<MotorCommand>(m, "MotorCommand")
        .def("getType", &MotorCommand::getType)
        .def("getPower", &MotorCommand::getPower)
        .def("getParameters", &MotorCommand::getParameters);
}

// Convenience factory functions
void initFactories(py::module_ &m) {
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    });
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    });
    
    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    });
    
    m.def("createSensoryInput", []() {
        return std::make_shared<SensoryInput>();
    });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM Python Bindings - Neural Learning Machine"
    
    // Version info
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";
    
    // Initialize all components
    initConfig(m);
    initNeuron(m);
    initBrain(m);
    initAgentBrain(m);
    initSensoryInput(m);
    initSensoryPercept(m);
    initSimpleWorld(m);
    initAction(m);
    initMotorCommand(m);
    initFactories(m);
    
    // Provide additional helper functions for convenience
    m.def("run_demo", [](int neuron_count, int steps) {
        // Run a simple demo with the given parameters
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuron_count, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        return brain;
    });
    
    m.def("run_advanced_demo", [](const std::string& config_file, bool enable_logging) {
        auto config = std::make_shared<Config>();
        
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run the brain with various stimuli
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        for (int step = 0; step < 100; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
        }
        
        return brain;
    });
}
