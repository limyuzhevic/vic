// NLM Python Bindings Module
// This file provides Python bindings for the NLM library components

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "memory/Memory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "development/DevelopmentSystem.hpp"
#include "agent/AgentBrain.hpp"
#include "world/Environment.hpp"
#include "experiments/Experiment.hpp"

namespace py = pybind11;

PYBIND11_MODULE(nlm_python_ext, m) {
    m.doc() = "NLM - Neural Learning Machine Python Bindings";

    // Core module
    py::module core = m.def_submodule("core", "Core neural system components");

    py::class_<nlm::Config>(core, "Config")
        .def(py::init<>())
        .def("set", &nlm::Config::set)
        .def("get", &nlm::Config::get)
        .def("get_or", &nlm::Config::getOr)
        .def("has", &nlm::Config::has)
        .def("remove", &nlm::Config::remove)
        .def("get_keys", &nlm::Config::getKeys);

    py::class_<nlm::Logger>(core, "Logger")
        .def(py::init<>());

    py::class_<nlm::Random>(core, "Random")
        .def(py::init<>())
        .def("rand", &nlm::Random::rand)
        .def("randf", &nlm::Random::randf);

    py::class_<nlm::SimulationClock>(core, "SimulationClock")
        .def(py::init<double>())
        .def("step", &nlm::SimulationClock::step)
        .def("getTime", &nlm::SimulationClock::getTime);

    py::class_<nlm::RegionId>(core, "RegionId")
        .def(py::init<size_t>())
        .def("getId", &nlm::RegionId::getId);

    // Brain module
    py::module brain = m.def_submodule("brain", "Neural computation components");

    py::class_<nlm::Neuron>(brain, "Neuron")
        .def(py::init<>())
        .def("injectCurrent", &nlm::Neuron::injectCurrent)
        .def("step", &nlm::Neuron::step)
        .def("getMembranePotential", &nlm::Neuron::getMembranePotential)
        .def("getThreshold", &nlm::Neuron::getThreshold)
        .def("isSpiking", &nlm::Neuron::isSpiking);

    py::class_<nlm::Synapse>(brain, "Synapse")
        .def(py::init<>())
        .def("getWeight", &nlm::Synapse::getWeight)
        .def("setWeight", &nlm::Synapse::setWeight)
        .def("enablePlasticity", &nlm::Synapse::enablePlasticity);

    py::class_<nlm::NeuralRegion>(brain, "NeuralRegion")
        .def("getAllNeurons", &nlm::NeuralRegion::getAllNeurons)
        .def("getAllSynapses", &nlm::NeuralRegion::getAllSynapses);

    py::class_<nlm::Brain>(brain, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>())
        .def("initialize", &nlm::Brain::initialize)
        .def("step", &nlm::Brain::step)
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount)
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount)
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons);

    // Memory module
    py::module memory = m.def_submodule("memory", "Memory systems");

    py::class_<nlm::Memory>(memory, "Memory")
        .def(py::init<>());

    // Environment module
    py::module world = m.def_submodule("world", "Environment/world system");

    py::class_<nlm::Environment>(world, "Environment")
        .def(py::init<>())
        .def("reset", &nlm::Environment::reset)
        .def("step", &nlm::Environment::step);

    // Agent module
    py::module agent = m.def_submodule("agent", "Agent brain interface");

    py::class_<nlm::AgentBrain>(agent, "AgentBrain")
        .def(py::init<>())
        .def("perceive", &nlm::AgentBrain::perceive)
        .def("act", &nlm::AgentBrain::act);

    // Export key constants
    m.attr("NeuronType") = py::module::import("nlm.core").attr("NeuronType");
    m.attr("LogLevel") = py::module::import("nlm.core").attr("LogLevel");
}