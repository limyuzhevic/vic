#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/brain/Brain.hpp"

namespace py = pybind11;

void bind_neural_working_memory(py::module& m) {
    py::class_<nlm::NeuralWorkingMemory>(m, "NeuralWorkingMemory")
        .def(py::init<>())
        .def("initialize", &nlm::NeuralWorkingMemory::initialize,
             py::arg("brain"), "Initialize working memory system")
        .def("store", &nlm::NeuralWorkingMemory::store,
             py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information pattern in working memory")
        .def("storeToNeuron", &nlm::NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"),
             "Store activation to specific neuron")
        .def("retrieve", &nlm::NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content")
        .def("contains", &nlm::NeuralWorkingMemory::contains,
             py::arg("neuron"), "Check if specific neuron is in working memory")
        .def("getNeuronActivation", &nlm::NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"), "Get activation level of specific neuron")
        .def("update", &nlm::NeuralWorkingMemory::update,
             py::arg("dt"), "Update working memory state")
        .def("clear", &nlm::NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("strengthenMemory", &nlm::NeuralWorkingMemory::strengthenMemory,
             py::arg("factor"), "Strengthen working memory representation")
        .def("runCompetition", &nlm::NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &nlm::NeuralWorkingMemory::isWinning,
             py::arg("neuron"), "Check if neuron is part of winning population")
        .def("getMemoryActivity", &nlm::NeuralWorkingMemory::getMemoryActivity,
             "Get overall activity level of working memory")
        .def("getCapacity", &nlm::NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &nlm::NeuralWorkingMemory::setCapacity,
             py::arg("cap"), "Set working memory capacity")
        .def("getDecayRate", &nlm::NeuralWorkingMemory::getDecayRate,
             "Get current decay rate")
        .def("setDecayRate", &nlm::NeuralWorkingMemory::setDecayRate,
             py::arg("rate"), "Set decay rate");
}

void bind_neural_episodic_memory(py::module& m) {
    py::class_<nlm::EpisodicMemoryItem>(m, "EpisodicMemoryItem")
        .def(py::init<>())
        .def_readwrite("timestamp", &nlm::EpisodicMemoryItem::timestamp)
        .def_readwrite("sensoryState", &nlm::EpisodicMemoryItem::sensoryState)
        .def_readwrite("positionX", &nlm::EpisodicMemoryItem::positionX)
        .def_readwrite("positionY", &nlm::EpisodicMemoryItem::positionY)
        .def_readwrite("orientation", &nlm::EpisodicMemoryItem::orientation)
        .def_readwrite("action", &nlm::EpisodicMemoryItem::action)
        .def_readwrite("reward", &nlm::EpisodicMemoryItem::reward)
        .def_readwrite("energy", &nlm::EpisodicMemoryItem::energy)
        .def_readwrite("novelty", &nlm::EpisodicMemoryItem::novelty)
        .def_readwrite("resultingSensoryState", &nlm::EpisodicMemoryItem::resultingSensoryState)
        .def_readwrite("resultingReward", &nlm::EpisodicMemoryItem::resultingReward)
        .def_readwrite("activeNeurons", &nlm::EpisodicMemoryItem::activeNeurons)
        .def_readwrite("neuronActivations", &nlm::EpisodicMemoryItem::neuronActivations)
        .def_readwrite("age", &nlm::EpisodicMemoryItem::age);

    py::class_<nlm::NeuralEpisodicMemory>(m, "NeuralEpisodicMemory")
        .def(py::init<>())
        .def("initialize", &nlm::NeuralEpisodicMemory::initialize,
             py::arg("brain"), "Initialize with brain reference")
        .def("storeEpisode", &nlm::NeuralEpisodicMemory::storeEpisode,
             py::arg("episode"), "Store a new experience")
        .def("retrieveSimilar", &nlm::NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             "Retrieve episodes similar to a query pattern")
        .def("retrieveTemporal", &nlm::NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from a specific time window")
        .def("retrieveByLocation", &nlm::NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location")
        .def("replayEpisode", &nlm::NeuralEpisodicMemory::replayEpisode,
             py::arg("episode"), "Replay an episode")
        .def("updateRelevance", &nlm::NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"),
             "Update episode relevance")
        .def("getEpisode", &nlm::NeuralEpisodicMemory::getEpisode,
             py::arg("index"), "Get episode by index")
        .def("getEpisodeCount", &nlm::NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getRecentEpisodes", &nlm::NeuralEpisodicMemory::getRecentEpisodes,
             py::arg("count"), "Get recent episodes")
        .def("getAverageReward", &nlm::NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &nlm::NeuralEpisodicMemory::consolidate,
             py::arg("relevanceThreshold"), "Consolidate")
        .def("clear", &nlm::NeuralEpisodicMemory::clear,
             "Clear all episodes")
        .def("enableReplay", &nlm::NeuralEpisodicMemory::enableReplay,
             py::arg("enable"), "Enable/disable replay")
        .def("getEpisodesForReplay", &nlm::NeuralEpisodicMemory::getEpisodesForReplay,
             py::arg("count"), "Get episodes for replay");
}

void init_memory_bindings(py::module& m) {
    bind_neural_working_memory(m);
    bind_neural_episodic_memory(m);
}
