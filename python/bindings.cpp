#include "BindingSystem.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include <memory>

namespace nlm {

// Example Custom Commands for demonstration
class ExampleCustomCommands {
public:
    // Example: Custom function to calculate neural similarity
    static double calculateNeuralSimilarity(const std::vector<float>& pattern1,
                                            const std::vector<float>& pattern2) {
        if (pattern1.size() != pattern2.size()) {
            throw std::runtime_error("Pattern sizes must match for similarity calculation");
        }
        double sum = 0.0;
        double norm1 = 0.0, norm2 = 0.0;
        for (size_t i = 0; i < pattern1.size(); ++i) {
            sum += pattern1[i] * pattern2[i];
            norm1 += pattern1[i] * pattern1[i];
            norm2 += pattern2[i] * pattern2[i];
        }
        double norm = std::sqrt(norm1) * std::sqrt(norm2);
        return norm > 0.0 ? sum / norm : 0.0;
    }
    
    // Example: Custom brain info extractor
    static std::string extractBrainInfo(const std::shared_ptr<Brain>& brain) {
        std::stringstream ss;
        ss << "Brain Info:" << std::endl;
        ss << "  Regions: " << brain->getRegionCount() << std::endl;
        ss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        return ss.str();
    }
    
    // Example: Custom world simulation step
    static std::map<std::string, float> simulateWorldStep(const std::shared_ptr<SimpleWorld>& world,
                                                          double timestep) {
        std::map<std::string, float> results;
        world->update(timestep);
        
        const auto& body = world->getAgentBody();
        results["agent_x"] = body.x;
        results["agent_y"] = body.y;
        results["agent_energy"] = body.energy;
        results["agent_health"] = body.health;
        
        return results;
    }
};

// Module Registrar for automatic class registration
class BindingRegistrar : public ModuleRegistrar {
public:
    void onRegister() override {
        // This will be called automatically when the registrar is instantiated
        // We can register classes here
    }
};

// Global registrar instance for automatic registration
BindingRegistrar globalRegistrar;

// Helper function to create custom commands from Python
template<typename Func>
auto createCustomCommand(py::module_& m, const std::string& name, 
                         const std::string& description, Func&& func) {
    return std::make_unique<TemplateCustomCommand<Func>>(
        std::forward<Func>(func), name, description);
}

// Custom Command Classes for demonstration
class CustomBrainAnalyzer : public CustomCommand {
public:
    std::string getName() const override { return "analyzeBrain"; }
    std::string getDescription() const override { 
        return "Analyze brain statistics and return a summary string"; 
    }
    
    py::object execute(py::args args, py::kwargs kwargs) override {
        try {
            auto brain = args[0].cast<std::shared_ptr<Brain>>();
            return py::cast(ExampleCustomCommands::extractBrainInfo(brain));
        } catch (const std::exception& e) {
            throw py::error_already_set();
        }
    }
};

class CustomNeuralSimilarity : public CustomCommand {
public:
    std::string getName() const override { return "calculateSimilarity"; }
    std::string getDescription() const override { 
        return "Calculate similarity between two neural patterns"; 
    }
    
    py::object execute(py::args args, py::kwargs kwargs) override {
        try {
            auto pattern1 = args[0].cast<std::vector<float>>();
            auto pattern2 = args[1].cast<std::vector<float>>();
            return py::cast(ExampleCustomCommands::calculateNeuralSimilarity(pattern1, pattern2));
        } catch (const std::exception& e) {
            throw py::error_already_set();
        }
    }
};

class CustomWorldSimulation : public CustomCommand {
public:
    std::string getName() const override { return "simulateStep"; }
    std::string getDescription() const override { 
        return "Run one world simulation step and return results"; 
    }
    
    py::object execute(py::args args, py::kwargs kwargs) override {
        try {
            auto world = args[0].cast<std::shared_ptr<SimpleWorld>>();
            double timestep = kwargs.contains("timestep") ? 
                kwargs["timestep"].cast<double>() : 0.1;
            return py::cast(ExampleCustomCommands::simulateWorldStep(world, timestep));
        } catch (const std::exception& e) {
            throw py::error_already_set();
        }
    }
};

// PYBIND11_MODULE macro that uses the new binding system
PYBIND11_MODULE(pynlm, m) {
    // Create the binding system
    PythonBindingSystem bindingSystem;
    
    // Initialize the module
    auto& module = bindingSystem.initializeModule(
        "pynlm",
        R"pbdoc(
            NLM (Neural Learning Machine) Python Bindings
            ---------------------------------------------
            A Python binding for the NLM C++ neural simulation framework.
            
            Features:
            - Modular binding system with automatic registration
            - Custom command support for runtime extensibility
            - Plugin architecture for extending functionality
            - Clean, maintainable code organization
            
            Example Usage:
            ```python
            import pynlm
            
            # Create default components
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            
            # Custom commands available:
            brain_info = pynlm.analyzeBrain(brain)
            similarity = pynlm.calculateSimilarity(pattern1, pattern2)
            simulation_results = pynlm.simulateStep(world, timestep=0.1)
            ```
        )pbdoc"
    );
    
    // Register exceptions using the binding system
    bindingSystem.registerException<std::runtime_error>(module, "RuntimeError");
    
    // Register all classes using the new modular system
    // The binding system will automatically register all standard classes
    
    // Register factory functions
    module.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    module.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    module.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    module.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Register custom commands using the binding system
    bindingSystem.registerCustomCommand(std::make_unique<CustomBrainAnalyzer>());
    bindingSystem.registerCustomCommand(std::make_unique<CustomNeuralSimilarity>());
    bindingSystem.registerCustomCommand(std::make_unique<CustomWorldSimulation>());
    
    // Register constants
    module.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    module.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    module.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    module.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Register visualization API functions
    module.def("createVisualizationAPI", []() -> std::shared_ptr<VisualizationAPI> {
        return std::make_shared<VisualizationAPI>();
    }, "Create a new visualization API");
    
    module.def("generateBrainVisualization", 
        [](std::shared_ptr<Brain> brain, const std::string& filename, 
           const std::string& format) {
            return brain->generateVisualization(filename, format);
        },
        py::arg("brain"), py::arg("filename"), py::arg("format") = "png",
        "Generate brain visualization and export to file");
    
    module.def("exportBrainData",
        [](std::shared_ptr<Brain> brain, const std::string& filename,
           const std::string& format) {
            return brain->exportBrainData(filename, format);
        },
        py::arg("brain"), py::arg("filename"), py::arg("format") = "json",
        "Export brain data to file");
    
    module.def("exportConnectivityMatrix",
        [](std::shared_ptr<Brain> brain, const std::string& filename,
           const std::string& format) {
            return brain->exportConnectivityMatrix(filename, format);
        },
        py::arg("brain"), py::arg("filename"), py::arg("format") = "csv",
        "Export connectivity matrix to file");
    
    module.def("exportTimeSeriesData",
        [](std::shared_ptr<Brain> brain, const std::string& filename,
           const std::string& format) {
            return brain->exportTimeSeriesData(filename, format);
        },
        py::arg("brain"), py::arg("filename"), py::arg("format") = "csv",
        "Export time series data to file");
    
    module.def("updateBrainVisualization",
        [](std::shared_ptr<Brain> brain, double deltaTime) {
            brain->updateVisualization(deltaTime);
        },
        py::arg("brain"), py::arg("delta_time") = 0.0,
        "Update brain visualization with new data");
    
    module.def("renderBrainVisualization",
        [](std::shared_ptr<Brain> brain) {
            brain->renderVisualization();
        },
        py::arg("brain"),
        "Render brain visualization");
    
    module.def("isBrainVisualizationEnabled",
        [](std::shared_ptr<Brain> brain) {
            return brain->isVisualizationEnabled();
        },
        py::arg("brain"),
        "Check if brain visualization is enabled");
    
    module.def("enableBrainVisualization",
        [](std::shared_ptr<Brain> brain, bool enabled) {
            brain->enableVisualization(enabled);
        },
        py::arg("brain"), py::arg("enabled"),
        "Enable or disable brain visualization");
    
    module.def("getBrainVisualizationStats",
        [](std::shared_ptr<Brain> brain) {
            py::dict stats;
            stats["frame_count"] = brain->getVisualizationFrameCount();
            stats["fps"] = brain->getVisualizationFPS();
            stats["update_time"] = brain->getVisualizationUpdateTime();
            stats["render_time"] = brain->getVisualizationRenderTime();
            return stats;
        },
        py::arg("brain"),
        "Get brain visualization statistics");
    
    module.def("getAvailableVisualizationTypes",
        []() {
            return std::vector<std::string>{
                "network", "activity", "spikes", "weights", "state_space", "connectivity"
            };
        },
        "Get available brain visualization types");
    
    module.def("getAvailableExportFormats",
        []() {
            return std::vector<std::string>{
                "png", "json", "csv", "hdf5", "parquet"
            };
        },
        "Get available export formats");
    
    module.def("setBrainVisualizationPreference",
        [](std::shared_ptr<Brain> brain, const std::string& key,
           const std::string& value) {
            brain->setVisualizationPreference(key, value);
        },
        py::arg("brain"), py::arg("key"), py::arg("value"),
        "Set visualization preference");
    
    module.def("getBrainVisualizationPreference",
        [](std::shared_ptr<Brain> brain, const std::string& key) {
            return brain->getVisualizationPreference(key);
        },
        py::arg("brain"), py::arg("key"),
        "Get visualization preference value");
}

} // namespace nlm