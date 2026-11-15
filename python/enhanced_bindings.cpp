# This enhanced version provides additional Python utilities and improved interfaces for NLM
# It extends the basic bindings with more advanced features for power users

// Additional Python utilities for NLM
void setupSimulationDefaults(py::module &m) {
    m.def("createSimulationConfig", []() {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults for typical use
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.15f);
        config->set("plasticity_learning_rate", 0.01f);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("development_synaptogenesis_rate", 0.0001f);
        config->set("development_pruning_rate", 0.00001f);
        config->set("dopamine_baseline", 0.1f);
        return config;
    }, "Create a simulation-ready configuration with sensible defaults");

    // Batch configuration loading utility
    m.def("loadConfigurationsFromFile", [](const std::vector<std::string>& filepaths) {
        std::vector<std::shared_ptr<Config>> configs;
        for (const auto& path : filepaths) {
            auto config = std::make_shared<Config>();
            if (config->loadFromFile(path)) {
                configs.push_back(config);
            } else {
                // Skip invalid files
                std::cerr << "Failed to load config: " << path << std::endl;
            }
        }
        return configs;
    }, "Load multiple configuration files");

    // Simulation runner utility
    m.def("runSimulation", [](std::shared_ptr<Config> config, uint64_t steps,
                            bool verbose = true) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        float totalReward = 0.0f;
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step);
            
            if (verbose) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount()
                          << " neurons firing, " << brain->getTotalSpikeCount()
                          << " total spikes" << std::endl;
            }
        }
        
        return std::make_tuple(brain->getFiringNeuronCount(), 
                               brain->getTotalSpikeCount(),
                               brain->getAverageFiringRate(),
                               totalReward);
    }, "Run a simple simulation and return statistics");

    // Utility for converting numpy arrays to sensory input
    m.def("createVisionFromImage", [](const py::array_t<float> image,
                                     size_t width, size_t height) {
        py::buffer_info buf = image.request();
        if (buf.ndim != 2 || buf.format != py::dtype("float").c_str() ||
            buf.format.size != sizeof(float)) {
            throw py::value_error("Image buffer must be float64 and 2D");
        }
        
        const float* data = static_cast<const float*>(buf.ptr);
        
        Vision vision(width, height, 3);
        std::vector<float> visionData;
        visionData.reserve(width * height * 3);
        
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                size_t idx = y * width + x;
                // Convert RGB to grayscale for vision
                float gray = 0.299 * data[idx * 3] + 0.587 * data[idx * 3 + 1] + 0.114 * data[idx * 3 + 2];
                visionData.push_back(gray);
            }
        }
        
        vision.setData(visionData);
        return vision;
    }, "Create Vision sensory input from a numpy image array");

    // Better error handling for configuration
    py::register_exception<std::exception>(m, "NLMError");
    py::register_exception<std::runtime_error>(m, "NLMRuntimeError");
}