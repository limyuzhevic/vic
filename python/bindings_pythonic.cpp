// bindings_pythonic.cpp
// Pythonic improvements to the NLM API implementation

#include "bindings_pythonic.hpp"

namespace nlm {

void bind_pythonic_features(py::module_ &m) {
    // Pythonic factory function with automatic config creation
    m.def("create_brain", 
        [](uint64_t neuron_count, uint64_t region_count, float connection_prob) {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", neuron_count);
            cfg->set("region_count", region_count);
            cfg->set("connection_probability", connection_prob);
            return std::make_shared<Brain>(cfg);
        },
        py::arg("neuron_count") = 1000u,
        py::arg("region_count") = 1u,
        py::arg("connection_prob") = 0.1f,
        "Create a brain with specified configuration using Pythonic interface");
    
    // Pythonic world configuration
    m.def("create_world", 
        [](size_t width, size_t height, size_t vision_width, size_t vision_height) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, vision_width, vision_height);
            return world;
        },
        py::arg("width") = 16u,
        py::arg("height") = 16u,
        py::arg("vision_width") = 8u,
        py::arg("vision_height") = 8u,
        "Create and configure a world with Pythonic interface");
    
    // Agent initialization helper
    m.def("create_agent", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world.get());
            return agent;
        },
        py::arg("brain"),
        py::arg("world"),
        "Create an agent with brain and world");
    
    // Enhanced sensory input creation with Pythonic API
    m.def("create_vision",
        [](size_t width, size_t height, size_t channels) {
            return std::make_shared<Vision>(width, height, channels);
        },
        py::arg("width") = 64u,
        py::arg("height") = 64u,
        py::arg("channels") = 3u,
        "Create vision sensory input");
    
    m.def("create_audio",
        [](size_t sample_rate, size_t num_samples) {
            return std::make_shared<Audio>(sample_rate, num_samples);
        },
        py::arg("sample_rate") = 44100u,
        py::arg("num_samples") = 1024u,
        "Create audio sensory input");
    
    m.def("create_internal_signals",
        []() {
            return std::make_shared<InternalSignals>();
        },
        "Create internal signals sensory input");
    
    // Enhanced action creation with Pythonic API
    m.def("create_action",
        [](ActionType type, std::vector<float> parameters) {
            return std::make_shared<Action>(type, parameters);
        },
        py::arg("type"),
        py::arg("parameters") = std::vector<float>(),
        "Create an action with type and parameters");
    
    m.def("create_action_simple",
        [](ActionType type) {
            return std::make_shared<Action>(type);
        },
        py::arg("type"),
        "Create a simple action with only type");
    
    // Helper for common action types
    m.def("move_action",
        [](float x, float y) {
            auto action = std::make_shared<Action>(ActionType::Custom);
            std::vector<float> params = {x, y};
            action->setParameters(params);
            return action;
        },
        py::arg("x") = 1.0f,
        py::arg("y") = 1.0f,
        "Create a movement action");
    
    m.def("look_action",
        [](float angle) {
            auto action = std::make_shared<Action>(ActionType::Look);
            std::vector<float> params = {angle};
            action->setParameters(params);
            return action;
        },
        py::arg("angle") = 0.0f,
        "Create a look action");
    
    // Enhanced agent body with Pythonic interface
    m.def("create_agent_body",
        [](float x, float y, float orientation) {
            auto body = std::make_shared<AgentBody>();
            body->x = x;
            body->y = y;
            body->orientation = orientation;
            return body;
        },
        py::arg("x") = 0.0f,
        py::arg("y") = 0.0f,
        py::arg("orientation") = 0.0f,
        "Create an agent body with initial position and orientation");
    
    // Simulation utilities
    m.def("run_step",
        [](std::shared_ptr<Brain> brain, uint64_t step, float time) {
            brain->step(step, time);
        },
        py::arg("brain"),
        py::arg("step") = 0u,
        py::arg("time") = 0.0f,
        "Run a single simulation step");
    
    m.def("run_steps",
        [](std::shared_ptr<Brain> brain, uint64_t steps, float timestep) {
            for (uint64_t i = 0; i < steps; ++i) {
                brain->step(i, i * timestep);
            }
        },
        py::arg("brain"),
        py::arg("steps") = 100u,
        py::arg("timestep") = 0.001f,
        "Run multiple simulation steps");
    
    // Convenience function for common setup
    m.def("setup_simulation",
        [](uint64_t neurons, uint64_t steps, float timestep) {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", neurons);
            cfg->set("simulation_timestep", timestep);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            // Run some initial steps
            for (uint64_t i = 0; i < std::min(steps, 1000ULL); ++i) {
                brain->step(i, i * timestep);
            }
            
            return std::make_tuple(brain, cfg);
        },
        py::arg("neurons") = 100u,
        py::arg("steps") = 100u,
        py::arg("timestep") = 0.001f,
        "Setup a basic simulation with sensible defaults");
    
    // Error handling improvements
    m.def("validate_config",
        [](const Config& cfg) {
            std::vector<std::string> errors;
            
            if (!cfg.has("neuron_count") || cfg.getOr<int64_t>("neuron_count", 0) <= 0) {
                errors.push_back("neuron_count must be a positive integer");
            }
            
            if (!cfg.has("region_count") || cfg.getOr<int64_t>("region_count", 0) <= 0) {
                errors.push_back("region_count must be a positive integer");
            }
            
            if (cfg.has("connection_probability")) {
                float prob = cfg.getOr<float>("connection_probability", 0.0f);
                if (prob < 0.0f || prob > 1.0f) {
                    errors.push_back("connection_probability must be between 0.0 and 1.0");
                }
            }
            
            return errors;
        },
        py::arg("cfg"),
        "Validate a configuration and return any errors");
    
    // Performance monitoring utilities
    m.def("get_simulation_stats",
        [](const std::shared_ptr<Brain>& brain) {
            std::unordered_map<std::string, float> stats;
            stats["total_spikes"] = static_cast<float>(brain->getTotalSpikeCount());
            stats["active_neurons"] = static_cast<float>(brain->getActiveNeuronCount());
            stats["firing_neurons"] = static_cast<float>(brain->getFiringNeuronCount());
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            return stats;
        },
        py::arg("brain"),
        "Get simulation statistics as a dictionary");
    
    // Data export utilities
    m.def("export_brain_data",
        [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
            return brain->save(filepath);
        },
        py::arg("brain"),
        py::arg("filepath"),
        "Export brain data to file");
    
    m.def("import_brain_data",
        [](const std::string& filepath) {
            auto cfg = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            if (brain->load(filepath)) {
                return brain;
            } else {
                throw std::runtime_error("Failed to load brain data from file");
            }
        },
        py::arg("filepath"),
        "Import brain data from file");
}

} // namespace nlm
