"""
Python bindings for NLM (Neural Learning Machine)

This module provides Python bindings for the NLM neural network simulation framework,
allowing users to create and simulate artificial brains from Python.
"""

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/functional.h"

#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/environment/SimpleWorld.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine)";
    
    // Version information
    m.attr("__version__") = "0.1.0";
    
    // Config module
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("set", &nlm::Config::set)
        .def("get", &nlm::Config::get<int64_t>)
        .def("get", &nlm::Config::get<double>)
        .def("get", &nlm::Config::get<bool>)
        .def("get", &nlm::Config::get<std::string>)
        .def("getOr", &nlm::Config::getOr<int64_t>)
        .def("getOr", &nlm::Config::getOr<double>)
        .def("getOr", &nlm::Config::getOr<bool>)
        .def("getOr", &nlm::Config::getOr<std::string>)
        .def("has", &nlm::Config::has)
        .def("loadFromFile", &nlm::Config::loadFromFile)
        .def("saveToFile", &nlm::Config::saveToFile)
        .def("clear", &nlm::Config::clear);
    
    m.def("createDefaultConfig", []() {
        return std::make_shared<nlm::Config>();
    });
    
    // Logger module
    py::class_<nlm::Logger>(m, "Logger")
        .def_static("getGlobal", &nlm::Logger::getGlobal)
        .def("setLevel", &nlm::Logger::setLevel);
    
    // Brain module
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>>(m, "Brain")
        .def("initialize", &nlm::Brain::initialize)
        .def("step", &nlm::Brain::step)
        .def("reset", &nlm::Brain::reset)
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("getWorkingMemory", &nlm::Brain::getWorkingMemory)
        .def("getEpisodicMemory", &nlm::Brain::getEpisodicMemory)
        .def("getNeuromodulationLevel", &nlm::Brain::getNeuromodulationLevel)
        .def("getPredictionSystem", &nlm::Brain::getPredictionSystem)
        .def("getDevelopmentSystem", &nlm::Brain::getDevelopmentSystem)
        .def("logStatus", &nlm::Brain::logStatus);
    
    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) {
        return std::make_shared<nlm::Brain>(config);
    });
    
    // AgentBrain module
    py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def("initialize", &nlm::AgentBrain::initialize)
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment)
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment)
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity)
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError)
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel)
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage);
    
    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) {
        return std::make_unique<nlm::AgentBrain>(brain);
    });
    
    // SimpleWorld module
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld")
        .def("configure", &nlm::SimpleWorld::configure)
        .def("reset", &nlm::SimpleWorld::reset)
        .def("update", &nlm::SimpleWorld::update)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart)
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand)
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept)
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody)
        .def("computeReward", &nlm::SimpleWorld::computeReward)
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime);
    
    m.def("createSimpleWorld", []() {
        return std::make_shared<nlm::SimpleWorld>();
    });
    
    // Utility functions
    m.def("runSimulation", [](std::shared_ptr<nlm::Brain> brain,
                             std::shared_ptr<nlm::SimpleWorld> world,
                             std::shared_ptr<nlm::AgentBrain> agent,
                             int num_steps) {
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::map<std::string, double>{{"spikes", brain->getTotalSpikeCount()},
                                            {"firing_rate", brain->getAverageFiringRate()},
                                            {"curiosity", agent->getCuriosityLevel()}};
    });
    
    // High-level convenience functions
    m.def("run_agent_simulation", [](int num_steps) {
        // Create default configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 1000);
        
        // Create brain and initialize
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Create agent
        auto agent = std::make_unique<nlm::AgentBrain>(brain);
        
        // Create and configure world
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        // Initialize agent
        agent->initialize(world);
        
        // Enable learning features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run simulation
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward (simplified)
            float reward = 0.0f;
            if (percept.getInternal().size() > 0) {
                reward = percept.getInternal()[0];
            }
            
            agent->applyRewardModulation(reward, 0.0);
            agent->updateDevelopment(0.1);
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": "
                          << brain->getFiringNeuronCount() << " firing neurons, "
                          << "Curiosity: " << agent->getCuriosityLevel() << std::endl;
            }
        }
        
        return std::map<std::string, double>{
            {"total_spikes", brain->getTotalSpikeCount()},
            {"avg_firing_rate", brain->getAverageFiringRate()},
            {"curiosity", agent->getCuriosityLevel()},
            {"novelty", agent->getNoveltyLevel()},
            {"dev_stage", agent->getDevelopmentalStage()}
        };
    });
}
