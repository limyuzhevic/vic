// Memory Systems Module for NLM Python Bindings
// This file contains Python bindings for memory-related systems

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/core/Config/Config.hpp"

namespace py = pybind11;
namespace nlm {

void bind_memory_systems(py::module_ &m) {
    // === NEURAL WORKING MEMORY ===
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", 
        "Working memory with persistent activity and competition")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize,
            py::arg("brain"), "Initialize working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
            py::arg("neuronId"), py::arg("activation"), 
            "Store activation to neuron in working memory")
        .def("update", &NeuralWorkingMemory::update,
            py::arg("timestep"), "Update working memory dynamics")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
            "Get number of active memory traces")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
            "Get neurons currently in working memory")
        .def("clear", &NeuralWorkingMemory::clear,
            "Clear all working memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
            "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity,
            py::arg("capacity"), "Set working memory capacity");
    
    // === NEURAL EPISODIC MEMORY ===
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory",
        "Episodic memory with experience encoding and replay")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize,
            py::arg("brain"), "Initialize episodic memory")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode,
            py::arg("episode"), "Store an episode")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
            "Get number of stored episodes")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay,
            py::arg("maxEpisodes"), "Get episodes for replay")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode,
            py::arg("episode"), "Replay an episode")
        .def("consolidate", &NeuralEpisodicMemory::consolidate,
            py::arg("threshold"), "Consolidate important memories")
        .def("clear", &NeuralEpisodicMemory::clear,
            "Clear all episodic memories")
        .def("getMaxEpisodes", &NeuralEpisodicMemory::getMaxEpisodes,
            "Get maximum number of episodes")
        .def("setMaxEpisodes", &NeuralEpisodicMemory::setMaxEpisodes,
            py::arg("maxEpisodes"), "Set maximum number of episodes");
    
    // === NEURAL ASSOCIATIVE MEMORY ===
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory",
        "Associative memory with Hebbian pattern associations")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize,
            py::arg("brain"), "Initialize associative memory")
        .def("storePattern", &NeuralAssociativeMemory::storePattern,
            py::arg("pattern"), py::arg("strength"), 
            "Store a pattern association")
        .def("recallPattern", &NeuralAssociativeMemory::recallPattern,
            py::arg("pattern"), "Recall associated pattern")
        .def("getPatternStrength", &NeuralAssociativeMemory::getPatternStrength,
            py::arg("pattern"), "Get strength of pattern")
        .def("clear", &NeuralAssociativeMemory::clear,
            "Clear all associative memories")
        .def("getPatternCount", &NeuralAssociativeMemory::getPatternCount,
            "Get number of stored patterns");
    
    // === EPISODIC MEMORY ITEM STRUCT ===
    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem",
        "Represents a complete experience episode")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("reward", &EpisodicMemoryItem::reward)
        .def_readwrite("activeNeurons", &EpisodicMemoryItem::activeNeurons)
        .def_readwrite("neuronActivations", &EpisodicMemoryItem::neuronActivations)
        .def("addActiveNeuron", &EpisodicMemoryItem::activeNeurons.emplace_back,
            py::arg("neuronId"), "Add neuron to episode")
        .def("addNeuronActivation", &EpisodicMemoryItem::neuronActivations.emplace_back,
            py::arg("activation"), "Add neuron activation to episode");
}

// === FACTORY FUNCTIONS ===

void bind_memory_factories(py::module_ &m) {
    m.def("createWorkingMemory", []() {
        return std::make_unique<NeuralWorkingMemory>();
    }, "Create a working memory instance");
    
    m.def("createEpisodicMemory", []() {
        return std::make_unique<NeuralEpisodicMemory>();
    }, "Create an episodic memory instance");
    
    m.def("createAssociativeMemory", []() {
        return std::make_unique<NeuralAssociativeMemory>();
    }, "Create an associative memory instance");
    
    m.def("createDefaultMemorySystem", []() {
        auto workingMem = std::make_unique<NeuralWorkingMemory>();
        auto episodicMem = std::make_unique<NeuralEpisodicMemory>();
        auto associativeMem = std::make_unique<NeuralAssociativeMemory>();
        
        return std::tuple<std::unique_ptr<NeuralWorkingMemory>,
                         std::unique_ptr<NeuralEpisodicMemory>,
                         std::unique_ptr<NeuralAssociativeMemory>>(
            std::move(workingMem),
            std::move(episodicMem),
            std::move(associativeMem)
        );
    }, "Create a complete default memory system");
}

} // namespace nlm