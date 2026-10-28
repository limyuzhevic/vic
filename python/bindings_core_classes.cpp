// Python bindings for NLM core classes
// Module for Config, Brain, and related core functionality

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>

#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/core/Random/Random.hpp"

namespace py = pybind11;
namespace nlm {

void bindConfig(py::module_ &m) {
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}

void bindBrain(py::module_ &m) {
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
}

void bindRandomGenerator(py::module_ &m) {
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Deterministic random number generator)pbdoc")
        .def(py::init<uint64_t>(), py::arg("seed"))
        .def(py::init<>())
        .def("seed", &RandomGenerator::seed, py::arg("seed"),
             "Set random seed")
        .def("getSeed", &RandomGenerator::getSeed,
             "Get current random seed")
        .def("uniformInt", [](RandomGenerator& self, uint32_t min, uint32_t max) {
            return self.uniformInt(min, max);
        }, py::arg("min"), py::arg("max"),
           "Uniform integer in [min, max]")
        .def("uniformInt64", [](RandomGenerator& self, uint64_t min, uint64_t max) {
            return self.uniformInt(min, max);
        }, py::arg("min"), py::arg("max"),
           "Uniform integer in [min, max]")
        .def("uniformReal", [](RandomGenerator& self, double min, double max) {
            return self.uniformReal(min, max);
        }, py::arg("min"), py::arg("max"),
           "Uniform real in [min, max)")
        .def("uniformRealFloat", [](RandomGenerator& self, float min, float max) {
            return self.uniformReal(min, max);
        }, py::arg("min"), py::arg("max"),
           "Uniform real in [min, max)")
        .def("normal", [](RandomGenerator& self, double mean, double stddev) {
            return self.normal(mean, stddev);
        }, py::arg("mean"), py::arg("stddev"),
           "Normal distribution")
        .def("bernoulli", &RandomGenerator::bernoulli, py::arg("p") = 0.5,
             "Bernoulli distribution")
        .def("poisson", &RandomGenerator::poisson, py::arg("lambda"),
             "Poisson distribution")
        .def("exponential", &RandomGenerator::exponential, py::arg("lambda"),
             "Exponential distribution");
}

void bindCoreClasses(py::module_ &m) {
    bindConfig(m);
    bindBrain(m);
    bindRandomGenerator(m);
}
