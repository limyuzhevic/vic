// Factories bindings implementation
#include "factories_bindings.h"

namespace nlm {

void bindFactories(py::module_& m) {
    // Factory functions for creating common objects
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

    // Convenience factory functions for advanced users
    m.def("createVision", [](size_t width, size_t height, size_t channels) {
        return std::make_shared<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
         "Create a Vision object with specified dimensions");

    m.def("createAudio", [](size_t sampleRate, size_t numSamples) {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"),
         "Create an Audio object with specified parameters");

    m.def("createAction", [](ActionType type, std::vector<float> params) {
        return std::make_shared<Action>(type, params);
    }, py::arg("type"), py::arg("params") = std::vector<float>(),
         "Create an Action with type and parameters");

    m.def("createConfig", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!filepath.empty()) {
            config->loadFromFile(filepath);
        }
        return config;
    }, py::arg("filepath") = "", "Create a config and optionally load from file");

    // Advanced factory functions
    m.def("createNeuronIds", [](const std::vector<uint64_t>& values) {
        std::vector<NeuronId> ids;
        ids.reserve(values.size());
        for (auto v : values) ids.emplace_back(v);
        return ids;
    }, py::arg("values"), "Create NeuronIds from list of values");

    m.def("createRegionIds", [](const std::vector<uint64_t>& values) {
        std::vector<RegionId> ids;
        ids.reserve(values.size());
        for (auto v : values) ids.emplace_back(v);
        return ids;
    }, py::arg("values"), "Create RegionIds from list of values");

    // Type conversion helpers
    m.def("neuron_id", [](uint64_t value) { return NeuronId(value); },
         py::arg("value"), "Create a NeuronId from a uint64 value");

    m.def("region_id", [](uint64_t value) { return RegionId(value); },
         py::arg("value"), "Create a RegionId from a uint64 value");

    m.def("population_id", [](uint64_t value) { return PopulationId(value); },
         py::arg("value"), "Create a PopulationId from a uint64 value");

    m.def("synapse_id", [](uint64_t value) { return SynapseId(value); },
         py::arg("value"), "Create a SynapseId from a uint64 value");

    // Utility conversion functions
    m.def("to_int", [](const NeuronId& id) { return id.value; },
          py::arg("neuron_id"), "Convert NeuronId to integer");

    m.def("to_string", [](const NeuronId& id) {
        return std::to_string(id.value);
    }, py::arg("neuron_id"), "Convert NeuronId to string");

    // Batch conversion
    m.def("neuron_ids", [](const std::vector<uint64_t>& values) {
        std::vector<NeuronId> ids;
        ids.reserve(values.size());
        for (auto v : values) ids.emplace_back(v);
        return ids;
    }, py::arg("values"), "Convert list of values to NeuronIds");

    // JSON serialization utilities (for advanced users)
    m.def("brain_to_dict", [](const std::shared_ptr<Brain>& brain) {
        // This would require brain to implement to_dict method
        py::dict result;
        result["neuron_count"] = brain->getTotalNeuronCount();
        result["synapse_count"] = brain->getTotalSynapseCount();
        result["active_neurons"] = brain->getActiveNeuronCount();
        result["firing_neurons"] = brain->getFiringNeuronCount();
        result["average_firing_rate"] = brain->getAverageFiringRate();
        result["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return result;
    }, py::arg("brain"), "Convert brain state to Python dictionary for analysis");

    m.def("config_to_dict", [](const std::shared_ptr<Config>& config) {
        py::dict result;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            result[key.c_str()] = config->get(key, "");
        }
        return result;
    }, py::arg("config"), "Convert config to Python dictionary");

    // Math utilities for advanced users
    m.def("lerp", [](float a, float b, float t) {
        return a + (b - a) * t;
    }, py::arg("a"), py::arg("b"), py::arg("t"), "Linear interpolation between a and b");

    m.def("normalize_vector", [](const std::vector<float>& vec) {
        float magnitude = 0.0f;
        for (float v : vec) magnitude += v * v;
        magnitude = std::sqrt(magnitude);
        if (magnitude > 0.0f) {
            std::vector<float> result = vec;
            for (auto& v : result) v /= magnitude;
            return result;
        }
        return vec;
    }, py::arg("vector"), "Normalize a vector to unit length");

    m.def("dot_product", [](const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size");
        }
        float result = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }, py::arg("a"), py::arg("b"), "Compute dot product of two vectors");

    // Invalid ID constants (for convenience)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm