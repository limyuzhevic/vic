#include "../core/BrainConfigBuilder.hpp"

namespace nlm {

BrainConfigBuilder::BrainConfigBuilder()
    : pImpl(std::make_unique<Impl>()) {
}

BrainConfigBuilder::~BrainConfigBuilder() = default;

// Forward declarations and implementation details for BrainConfigBuilder::Impl
struct BrainConfigBuilder::Impl {
    size_t neuronCount = 1000;
    size_t regionCount = 4;
    float connectionProbability = 0.1f;
    double timestep = 0.001;
    uint64_t randomSeed = 0;
    bool neuromodulationEnabled = true;
    bool developmentEnabled = true;
    bool curiosityEnabled = true;
    bool structuralPlasticityEnabled = true;
    bool memoryEnabled = true;
    bool predictionEnabled = true;
    bool cognitionEnabled = true;
    std::function<void(std::shared_ptr<Config>)> customConfigCallback;
};

// Python bindings for BrainConfigBuilder
void BrainConfigBuilder::registerPythonBindings(py::module_& m) {
    py::class_<BrainConfigBuilder>(m, "BrainConfigBuilder")
        .def(py::init<>())
        .def("set_neuron_count", &BrainConfigBuilder::setNeuronCount,
             "Set the number of neurons", py::arg("count"))
        .def("set_region_count", &BrainConfigBuilder::setRegionCount,
             "Set the number of regions", py::arg("count"))
        .def("set_connection_probability", &BrainConfigBuilder::setConnectionProbability,
             "Set connection probability between neurons", py::arg("prob"))
        .def("set_timestep", &BrainConfigBuilder::setTimestep,
             "Set simulation timestep in seconds", py::arg("timestep"))
        .def("set_random_seed", &BrainConfigBuilder::setRandomSeed,
             "Set random seed for reproducible results", py::arg("seed"))
        .def("enable_neuromodulation", &BrainConfigBuilder::enableNeuromodulation,
             "Enable neuromodulation", py::arg("enable") = true)
        .def("enable_development", &BrainConfigBuilder::enableDevelopment,
             "Enable developmental plasticity", py::arg("enable") = true)
        .def("enable_curiosity", &BrainConfigBuilder::enableCuriosity,
             "Enable curiosity-driven exploration", py::arg("enable") = true)
        .def("enable_structural_plasticity", &BrainConfigBuilder::enableStructuralPlasticity,
             "Enable structural plasticity", py::arg("enable") = true)
        .def("enable_memory", &BrainConfigBuilder::enableMemory,
             "Enable memory systems", py::arg("enable") = true)
        .def("enable_prediction", &BrainConfigBuilder::enablePrediction,
             "Enable prediction system", py::arg("enable") = true)
        .def("enable_cognition", &BrainConfigBuilder::enableCognition,
             "Enable cognitive functions", py::arg("enable") = true)
        .def("set_custom_config", &BrainConfigBuilder::setCustomConfig,
             "Set custom configuration callback", py::arg("callback"))
        .def("build", &BrainConfigBuilder::build,
             "Build and return the configuration")
        .def("apply_to", &BrainConfigBuilder::applyTo,
             "Apply configuration to existing config object", py::arg("config"))
        .def_static("default_config", &BrainConfigBuilder::defaultConfig,
                    "Create default configuration")
        .def_static("minimal_config", &BrainConfigBuilder::minimalConfig,
                    "Create minimal configuration")
        .def_static("experimental_config", &BrainConfigBuilder::experimentalConfig,
                    "Create experimental configuration")
        .def_static("research_config", &BrainConfigBuilder::researchConfig,
                    "Create research configuration");
}

} // namespace nlm
