#pragma once

// Basic Python bindings for NLM - Core API
// This file provides the essential Python bindings for NLM users

#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "World.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include <string>
#include <vector>
#include <memory>

namespace py = pybind11;

// Core types
void bindCoreTypes(py::module_ &m) {
    // Neuron type
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    // Developmental stage
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Action type
    py::enum_<ActionType>(m, "ActionType")
        .value("MoveForward", ActionType::MoveForward)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .export_values();
    
    // World object type
    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
}

// Basic Brain binding
void bindBasicBrain(py::module_ &m) {
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>&>())
        .def("initialize", &Brain::initialize)
        .def("step", &Brain::step)
        .def("step", [](Brain& b, SimulationStep step, Timestamp time) {
            b.step(step, time);
        })
        .def("reset", &Brain::reset)
        .def("save", &Brain::save)
        .def("load", &Brain::load)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio)
        .def("getDevelopmentStage", &Brain::getDevelopmentalStage)
        .def("logStatus", &Brain::logStatus);
}

// Basic Config binding
void bindBasicConfig(py::module_ &m) {
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile)
        .def("loadFromArgs", &Config::loadFromArgs)
        .def("saveToFile", &Config::saveToFile)
        .def("get", &Config::get)
        .def("getOr", [](const Config& self, const std::string& key, auto defaultValue) {
            return self.getOr(key, defaultValue);
        })
        .def("set", &Config::set)
        .def("has", &Config::has)
        .def("getKeys", &Config::getKeys)
        .def("summary", &Config::summary);
}

// Basic Agent binding
void bindBasicAgent(py::module_ &m) {
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>&>())
        .def("initialize", &AgentBrain::initialize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation)
        .def("updateDevelopment", &AgentBrain::updateDevelopment)
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &AgentBrain::enableDevelopment)
        .def("enableCuriosity", &AgentBrain::enableCuriosity)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel);
}

// Basic World binding
void bindBasicWorld(py::module_ &m) {
    py::class_<World>(m, "World")
        .def("reset", &World::reset)
        .def("update", &World::update)
        .def("getSimulationTime", &World::getSimulationTime)
        .def("getSensoryPercept", &World::getSensoryPercept)
        .def("applyMotorCommand", &World::applyMotorCommand)
        .def("computeReward", &World::computeReward)
        .def("setAgentStart", &World::setAgentStart)
        .def("getWidth", &World::getWidth)
        .def("getHeight", &World::getHeight)
        .def("getVisionWidth", &World::getVisionWidth)
        .def("getVisionHeight", &World::getVisionHeight);

    py::class_<SimpleWorld, World>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed)
        .def("addObject", &SimpleWorld::addObject)
        .def("removeObject", &SimpleWorld::removeObject)
        .def("isValidPosition", &SimpleWorld::isValidPosition)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy);
}

// Helper functions
void bindHelperFunctions(py::module_ &m) {
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    });
    
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    });
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    });
    
    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    });
}

// Main module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine) - A brain-inspired artificial developmental brain system";
    
    // Bind core types
    bindCoreTypes(m);
    
    // Bind main classes
    bindBasicConfig(m);
    bindBasicBrain(m);
    bindBasicAgent(m);
    bindBasicWorld(m);
    
    // Bind helper functions
    bindHelperFunctions(m);
}
