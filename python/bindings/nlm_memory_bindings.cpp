"""
Phase 6 Memory Systems Python Bindings

This module provides Python bindings for all Phase 6 memory systems integrated into the NLM brain.
"""

import pybind11
import pybind11.stl
import pybind11.functional
import numpy as np

namespace nlm {

// Python bindings for WorkingMemory
PYBIND11_MODULE(pynlm_memory, m) {
    m.doc() = R"pbdoc(
        NLM Phase 6 Memory Systems
        ---------------------------
        Complete Phase 6 memory systems with Python bindings
    )pbdoc";

    // WorkingMemory bindings
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"),
             "Store value in working memory for a neuron")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"),
             "Retrieve value from working memory for a neuron")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"),
             "Check if neuron has working memory entry")
        .def("clear", &WorkingMemory::clear, "Clear all working memory")
        .def("getCapacity", &WorkingMemory::getCapacity, "Get working memory capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize, "Get current working memory size")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"),
             "Apply decay to all working memory items")
        .def("__repr__", [](const WorkingMemory& wm) {
            return "<WorkingMemory capacity=" + std::to_string(wm.getCapacity()) +
                   " size=" + std::to_string(wm.getCurrentSize()) + ">";
        });

    // NeuralWorkingMemory bindings
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store neural pattern in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store activation to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory as activity levels")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of specific neuron")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory dynamics")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces, "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"),
             "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate, "Get working memory decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set working memory decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Check if neuron is winning population")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    // EpisodicMemoryItem bindings
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

    // NeuralEpisodicMemory bindings
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new experience")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             "Retrieve similar episodes")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from time window")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5,
             "Retrieve episodes after specific action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay an episode - reactivates neural patterns")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"),
             "Update episode relevance")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"),
             "Get episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Clear old episodes based on relevance")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"),
             "Enable/disable replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"),
             "Get episodes for replay selection")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"),
             "Replay sequence of episodes");

    // NeuralAssociativeMemory bindings
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             "Create association between two neural patterns")
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience,
             py::arg("episode"), "Create association from experience")
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults") = 5,
             "Retrieve patterns associated with query")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"),
             "Get association strength between patterns")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             "Update association based on outcome")
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation,
             py::arg("cuePattern"), py::arg("steps") = 2,
             "Spread activation from cue pattern")
        .def("clear", &NeuralAssociativeMemory::clear, "Clear associations")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
             "Get number of associations");

    // Factory functions
    m.def("createWorkingMemory", []() -> std::shared_ptr<WorkingMemory> {
        return std::make_shared<WorkingMemory>();
    }, "Create a working memory system");

    m.def("createNeuralWorkingMemory", []() -> std::shared_ptr<NeuralWorkingMemory> {
        return std::make_shared<NeuralWorkingMemory>();
    }, "Create a neural working memory system");

    m.def("createNeuralEpisodicMemory", []() -> std::shared_ptr<NeuralEpisodicMemory> {
        return std::make_shared<NeuralEpisodicMemory>();
    }, "Create a neural episodic memory system");

    m.def("createNeuralAssociativeMemory", []() -> std::shared_ptr<NeuralAssociativeMemory> {
        return std::make_shared<NeuralAssociativeMemory>();
    }, "Create a neural associative memory system");
}
