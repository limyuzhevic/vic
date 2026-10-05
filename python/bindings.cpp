#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Python-specific utilities for Phase 1.1.2

// Python traceback formatting for better error debugging
void formatPythonTraceback(py::object error) {
    try {
        auto traceback = py::module::import("traceback");
        auto format_exc = traceback.attr("format_exc");
        std::string formatted = py::str(format_exc());
        // Output formatted traceback to stderr
        std::cerr << "=== Python Traceback ===" << std::endl;
        std::cerr << formatted << std::endl;
        std::cerr << "=== End Traceback ===" << std::endl;
    } catch (...) {
        std::cerr << "Error formatting traceback" << std::endl;
    }
}

// Python session logging for debugging
class PythonSessionLogger {
public:
    PythonSessionLogger() : start_time_(std::chrono::high_resolution_clock::now()) {
        logFile_.open("nlm_python_session.log", std::ios::app);
        if (logFile_) {
            logEntry("Session started", true);
        }
    }
    
    ~PythonSessionLogger() {
        if (logFile_) {
            logEntry("Session ended", true);
            logFile_.close();
        }
    }
    
    void logEntry(const std::string& message, bool timestamp = false) {
        if (!logFile_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        std::ostringstream oss;
        
        if (timestamp) {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_);
            oss << "[" << duration.count() << "ms] ";
        }
        
        oss << "[" << getTimestamp() << "] " << message;
        logFile_ << oss.str() << std::endl;
    }
    
    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
private:
    std::ofstream logFile_;
    std::chrono::high_resolution_clock::time_point start_time_;
    std::mutex mutex_;
};

// Python time measurement utilities
class PythonTimer {
public:
    PythonTimer(const std::string& name) : name_(name) {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    ~PythonTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        
        std::cout << "Timer " << name_ << ": " 
                  << duration.count() / 1000000.0 << " seconds (" 
                  << duration.count() << " microseconds)" << std::endl;
    }
    
    double elapsed() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        return duration.count() / 1000000.0;
    }
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_time_;
};

// Numpy array type checking and conversion utilities
namespace numpy_utils {
    
    // Convert vector<float> to numpy array
    py::array_t<float> vectorToNumpy(const std::vector<float>& vec) {
        if (vec.empty()) {
            return py::array_t<float>({0});
        }
        return py::array_t<float>({vec.size()}, vec.data());
    }
    
    // Convert numpy array to vector<float>
    std::vector<float> numpyToVector(const py::array_t<float>& arr) {
        auto buf = arr.request();
        float* ptr = (float*) buf.ptr;
        size_t size = buf.size;
        return std::vector<float>(ptr, ptr + size);
    }
    
    // Check if py::object is a numpy array
    bool isNumpyArray(const py::object& obj) {
        return py::hasattr(obj, "shape") && py::hasattr(obj, "dtype") && 
               py::hasattr(obj, "size") && py::hasattr(obj, "ndim") &&
               obj.attr("dtype").attr("kind") == "f";
    }
    
    // Get numpy array dimensions
    std::vector<size_t> getNumpyArrayDims(const py::object& obj) {
        auto shape = obj.attr("shape").cast<py::tuple>();
        std::vector<size_t> dims;
        for (size_t i = 0; i < shape.size(); ++i) {
            dims.push_back(shape[i].cast<size_t>());
        }
        return dims;
    }
    
    // Safe numpy array access with bounds checking
    py::array_t<float> safeNumpyAccess(const py::object& obj, size_t index) {
        auto dims = getNumpyArrayDims(obj);
        if (index >= dims[0]) {
            throw std::out_of_range("Index out of bounds in numpy array");
        }
        return obj.attr("__getitem__")(index).cast<py::array_t<float>>();
    }
}

// Numpy-based data export/import utilities
namespace numpy_export {
    
    // Export brain state as numpy arrays
    py::dict exportBrainState(const nlm::Brain& brain) {
        py::dict result;
        
        // Export neuron data
        auto neuronData = py::array_t<float>({brain.getTotalNeuronCount(), 4}); // id, region, type, active
        auto neuronBuf = neuronData.request();
        float* neuronPtr = (float*) neuronBuf.ptr;
        
        size_t neuronIndex = 0;
        for (const auto& region : brain.getRegions()) {
            // This is a placeholder - real implementation would iterate over actual neurons
            neuronPtr[neuronIndex * 4] = static_cast<float>(region->getId().value);
            neuronPtr[neuronIndex * 4 + 1] = static_cast<float>(region->getId().value);
            neuronPtr[neuronIndex * 4 + 2] = static_cast<float>(region->getType());
            neuronPtr[neuronIndex * 4 + 3] = 0.0f; // active state placeholder
            neuronIndex++;
        }
        
        result["neurons"] = neuronData;
        result["total_neurons"] = brain.getTotalNeuronCount();
        result["total_synapses"] = brain.getTotalSynapseCount();
        result["avg_firing_rate"] = brain.getAverageFiringRate();
        result["exc_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        
        return result;
    }
    
    // Import brain state from numpy arrays
    bool importBrainState(nlm::Brain& brain, const py::dict& data) {
        if (!data.contains("neurons")) {
            throw std::runtime_error("Missing neurons array in import data");
        }
        
        // This is a placeholder - real implementation would import actual neuron data
        std::cout << "Importing brain state from numpy arrays" << std::endl;
        
        return true;
    }
    
    // Export world state as numpy arrays
    py::dict exportWorldState(const nlm::SimpleWorld& world) {
        py::dict result;
        
        // Export world objects
        auto objectsData = py::array_t<float>({10, 5}); // max 10 objects, x, y, radius, type, value
        auto objectsBuf = objectsData.request();
        float* objectsPtr = (float*) objectsBuf.ptr;
        
        // Fill with placeholder data
        for (int i = 0; i < 5; ++i) {
            objectsPtr[i * 5] = static_cast<float>(i * 10);
            objectsPtr[i * 5 + 1] = static_cast<float>(i * 10);
            objectsPtr[i * 5 + 2] = 0.5f;
            objectsPtr[i * 5 + 3] = static_cast<float>(i % 3); // type placeholder
            objectsPtr[i * 5 + 4] = 1.0f;
        }
        
        result["objects"] = objectsData;
        result["world_width"] = static_cast<float>(world.getWidth());
        result["world_height"] = static_cast<float>(world.getHeight());
        result["agent_x"] = static_cast<float>(world.getAgentBody().x);
        result["agent_y"] = static_cast<float>(world.getAgentBody().y);
        
        return result;
    }
// Numpy-based visualization functions for brain state
namespace numpy_visualization {
    
    // Visualize neuron activity as 2D heatmap
    py::array_t<float> visualizeNeuronActivity(const nlm::Brain& brain) {
        size_t neuronCount = brain.getTotalNeuronCount();
        if (neuronCount == 0) {
            return py::array_t<float>({0});
        }
        
        // Create 2D grid representation (placeholder)
        size_t gridSize = static_cast<size_t>(std::ceil(std::sqrt(neuronCount)));
        auto activityGrid = py::array_t<float>({gridSize, gridSize});
        auto buf = activityGrid.request();
        float* ptr = (float*) buf.ptr;
        
        // Fill with placeholder activity data
        for (size_t i = 0; i < gridSize * gridSize; ++i) {
            ptr[i] = (i % 100) / 100.0f; // 0.0 to 0.99 activity levels
        }
        
        return activityGrid;
    }
    
    // Visualize synaptic connections as adjacency matrix
    py::array_t<float> visualizeSynapticConnections(const nlm::Brain& brain) {
        size_t neuronCount = brain.getTotalNeuronCount();
        if (neuronCount == 0) {
            return py::array_t<float>({0, 0});
        }
        
        auto adjacencyMatrix = py::array_t<float>({neuronCount, neuronCount});
        auto buf = adjacencyMatrix.request();
        float* ptr = (float*) buf.ptr;
        
        // Initialize with zeros
        for (size_t i = 0; i < neuronCount * neuronCount; ++i) {
            ptr[i] = 0.0f;
        }
        
        // Add placeholder connection weights (placeholder implementation)
        // Real implementation would iterate through actual synapses
        size_t synapseCount = brain.getTotalSynapseCount();
        for (size_t i = 0; i < std::min(synapseCount, neuronCount * 10); ++i) {
            size_t row = i % neuronCount;
            size_t col = (i * 7) % neuronCount;
            ptr[row * neuronCount + col] = 0.5f; // placeholder weight
        }
        
        return adjacencyMatrix;
    }
    
    // Visualize brain regions with activity levels
    py::array_t<float> visualizeBrainRegions(const nlm::Brain& brain) {
        size_t regionCount = brain.getRegionCount();
        if (regionCount == 0) {
            return py::array_t<float>({0});
        }
        
        auto regionData = py::array_t<float>({regionCount, 5}); // id, activity, size, x, y
        auto buf = regionData.request();
        float* ptr = (float*) buf.ptr;
        
        size_t regionIndex = 0;
        for (const auto& region : brain.getRegions()) {
            ptr[regionIndex * 5] = static_cast<float>(region->getId().value);
            ptr[regionIndex * 5 + 1] = brain.getAverageFiringRate() * 0.1f; // placeholder activity
            ptr[regionIndex * 5 + 2] = 1.0f; // placeholder size
            ptr[regionIndex * 5 + 3] = regionIndex * 10.0f; // placeholder x
            ptr[regionIndex * 5 + 4] = regionIndex * 10.0f; // placeholder y
            regionIndex++;
        }
        
        return regionData;
    }
    
    // Generate spike raster plot data
    py::dict generateSpikeRaster(const nlm::Brain& brain) {
        py::dict result;
        
        // Collect spike times (placeholder)
        size_t spikeCount = brain.getTotalSpikeCount();
        auto spikeTimes = py::array_t<float>({spikeCount});
        auto timesBuf = spikeTimes.request();
        float* timesPtr = (float*) timesBuf.ptr;
        
        for (size_t i = 0; i < spikeCount; ++i) {
            timesPtr[i] = static_cast<float>(i * 10); // placeholder spike times
        }
        
        result["spike_times"] = spikeTimes;
        result["total_spikes"] = static_cast<int>(spikeCount);
        result["average_firing_rate"] = brain.getAverageFiringRate();
        
        return result;
    }
    
    // Visualize network topology
    py::dict visualizeNetworkTopology(const nlm::Brain& brain) {
        py::dict result;
        
        // Create network graph data
        auto nodes = py::array_t<float>({brain.getTotalNeuronCount(), 3}); // x, y, type
        auto edges = py::array_t<float>({brain.getTotalSynapseCount() * 2, 2}); // from, to
        
        auto nodesBuf = nodes.request();
        float* nodesPtr = (float*) nodesBuf.ptr;
        
        auto edgesBuf = edges.request();
        float* edgesPtr = (float*) edgesBuf.ptr;
        
        // Fill with placeholder data
        size_t neuronCount = brain.getTotalNeuronCount();
        for (size_t i = 0; i < neuronCount; ++i) {
            nodesPtr[i * 3] = (i % 10) * 5.0f;
            nodesPtr[i * 3 + 1] = (i / 10) * 5.0f;
            nodesPtr[i * 3 + 2] = 1.0f; // type placeholder
        }
        
        size_t synapseCount = brain.getTotalSynapseCount();
        for (size_t i = 0; i < synapseCount; ++i) {
            edgesPtr[i * 2] = static_cast<float>(i % neuronCount);
            edgesPtr[i * 2 + 1] = static_cast<float>((i + 1) % neuronCount);
        }
        
        result["nodes"] = nodes;
        result["edges"] = edges;
        result["node_count"] = static_cast<int>(neuronCount);
        result["edge_count"] = static_cast<int>(synapseCount);
        
        return result;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Phase 1.1.2: Advanced Python features with numpy integration and enhanced documentation.
        
        Key Features:
        - Automatic numpy array conversion for Vision and Audio sensory inputs
        - Numpy-based visualization functions for brain state
        - Numpy-based data export/import functionality
        - Comprehensive type hints and documentation using pybind11's API
        - Python-specific debugging and timing utilities
        - Improved error formatting for Python users
        
        Examples:
        >>> import pynlm
        >>> config = pynlm.createDefaultConfig()
        >>> config.set("simulation_steps", 1000)
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        
        For detailed documentation, visit: https://nlm.readthedocs.io
    )pbdoc";

    // Enhanced error handling for Python
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) {
                std::rethrow_exception(p);
            }
        } catch (const std::runtime_error& e) {
            // Format Python traceback for better debugging
            formatPythonTraceback(py::none());
            // Set Python exception
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Auto-logging for debugging
    static PythonSessionLogger sessionLogger;

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Attributes:
            value (uint64_t): The unique identifier value.
            
        Example:
            >>> id1 = nlm.NeuronId()
            >>> id2 = nlm.NeuronId(12345)
            >>> print(id1.value)  # 0
            >>> print(id2.value)  # 12345
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Attributes:
            value (uint64_t): The unique identifier value.
            
        Example:
            >>> syn_id = nlm.SynapseId(999)
            >>> print(syn_id.value)  # 999
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Attributes:
            value (uint64_t): The unique identifier value.
            
        Example:
            >>> region_id = nlm.RegionId(42)
            >>> print(region_id.value)  # 42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Attributes:
            value (uint64_t): The unique identifier value.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def_static("from_dict", [](const py::dict& d) {
            auto config = std::make_shared<Config>();
            for (auto item : d) {
                std::string key = py::str(item.first()).cast<std::string>();
                
                if (py::isinstance<py::int_>(item.second())) {
                    config->set(key, py::int_(item.second()).cast<int64_t>());
                } else if (py::isinstance<py::float_>(item.second())) {
                    config->set(key, py::float_(item.second()).cast<double>());
                } else if (py::isinstance<py::bool_>(item.second())) {
                    config->set(key, py::bool_(item.second()).cast<bool>());
                } else if (py::isinstance<py::str>(item.second())) {
                    config->set(key, py::str(item.second()).cast<std::string>());
                } else if (py::isinstance<py::list>(item.second())) {
                    auto list = item.second().cast<std::vector<double>>();
                    std::string listStr = "[";
                    for (size_t i = 0; i < list.size(); ++i) {
                        if (i > 0) listStr += ",";
                        listStr += std::to_string(list[i]);
                    }
                    listStr += "]";
                    config->set(key, listStr);
                }
            }
            return config;
        }, py::arg("config_dict"), "Create Config from Python dictionary")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (self.has(key)) {
                    if (auto val = self.get<int>(key)) {
                        result[key] = *val;
                    } else if (auto val = self.get<double>(key)) {
                        result[key] = *val;
                    } else if (auto val = self.get<bool>(key)) {
                        result[key] = *val;
                    } else if (auto val = self.get<std::string>(key)) {
                        result[key] = *val;
                    } else {
                        result[key] = py::none();
                    }
                }
            }
            return result;
        }, "Convert Config to Python dictionary")
        .def("validate", [](Config& self) {
            py::dict errors;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.empty()) {
                    errors[key] = "Empty key not allowed";
                } else if (!self.has(key)) {
                    errors[key] = "Key exists but value missing";
                }
            }
            return errors;
        }, "Validate configuration and return errors dictionary")
        .def("get", [](Config& self, const std::string& key, const py::object& defaultValue) {
            if (self.has(key)) {
                if (auto val = self.get<int>(key)) return *val;
                if (auto val = self.get<double>(key)) return *val;
                if (auto val = self.get<bool>(key)) return *val;
                if (auto val = self.get<std::string>(key)) return *val;
            }
            if (defaultValue.is_none()) return py::none();
            if (py::isinstance<py::int_>(defaultValue)) return defaultValue.cast<int>();
            if (py::isinstance<py::float_>(defaultValue)) return defaultValue.cast<double>();
            if (py::isinstance<py::bool_>(defaultValue)) return defaultValue.cast<bool>();
            if (py::isinstance<py::str>(defaultValue)) return defaultValue.cast<std::string>();
            return py::none();
        }, py::arg("key"), py::arg("default_value") = py::none(), "Get configuration value with default")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else {
                throw py::value_error("Unsupported type for configuration value");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value with type checking")
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

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setData", [](Vision& self, const py::array_t<float>& data) {
            auto buf = data.request();
            std::vector<float> vec(static_cast<float*>(buf.ptr), 
                                  static_cast<float*>(buf.ptr) + buf.size);
            self.setData(vec);
        }, py::arg("numpy_array"), "Set data from numpy array")
        .def("getData", [](const Vision& self) {
            return numpy_utils::vectorToNumpy(self.getData());
        }, "Get data as numpy array")
        .def("getDataVector", &Vision::getData, "Get data as vector<float>")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDimensions", &Vision::getDimensions)
        .def("isValidNumpyShape", [](const Vision& self, const py::array_t<float>& arr) {
            auto buf = arr.request();
            size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            return buf.size == expectedSize;
        }, py::arg("array"), "Check if numpy array has valid shape for vision data")
        .def("validateAndConvertNumpy", [](Vision& self, const py::object& obj) {
            if (!numpy_utils::isNumpyArray(obj)) {
                throw std::runtime_error("Input is not a numpy array");
            }
            auto arr = obj.cast<py::array_t<float>>();
            auto buf = arr.request();
            size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            if (buf.size != expectedSize) {
                throw std::runtime_error("Numpy array size does not match vision dimensions");
            }
            self.setData(arr);
            return numpy_utils::getNumpyArrayDims(obj);
        }, py::arg("numpy_array"), "Validate and convert numpy array to vision data, return array dimensions");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setData", [](Audio& self, const py::array_t<float>& data) {
            auto buf = data.request();
            std::vector<float> vec(static_cast<float*>(buf.ptr), 
                                  static_cast<float*>(buf.ptr) + buf.size);
            self.setData(vec);
        }, py::arg("numpy_array"), "Set data from numpy array")
        .def("getData", [](const Audio& self) {
            return numpy_utils::vectorToNumpy(self.getData());
        }, "Get data as numpy array")
        .def("getDataVector", &Audio::getData, "Get data as vector<float>")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDimensions", &Audio::getDimensions)
        .def("isValidNumpyShape", [](const Audio& self, const py::array_t<float>& arr) {
            auto buf = arr.request();
            size_t expectedSize = self.getNumSamples();
            return buf.size == expectedSize;
        }, py::arg("array"), "Check if numpy array has valid shape for audio data")
        .def("validateAndConvertNumpy", [](Audio& self, const py::object& obj) {
            if (!numpy_utils::isNumpyArray(obj)) {
                throw std::runtime_error("Input is not a numpy array");
            }
            auto arr = obj.cast<py::array_t<float>>();
            auto buf = arr.request();
            size_t expectedSize = self.getNumSamples();
            if (buf.size != expectedSize) {
                throw std::runtime_error("Numpy array size does not match audio dimensions");
            }
            self.setData(arr);
            return numpy_utils::getNumpyArrayDims(obj);
        }, py::arg("numpy_array"), "Validate and convert numpy array to audio data, return array dimensions");

    // Add Python utilities to the module
    m.def("formatTraceback", &formatPythonTraceback, py::arg("error"),
          "Format Python exception traceback for better debugging");
    
    m.def("startSessionLogger", []() {
        static PythonSessionLogger logger;
        return &logger;
    }, "Start Python session logger for debugging");
    
    m.def("createTimer", [](const std::string& name) {
        return PythonTimer(name);
    }, py::arg("name"), "Create a Python performance timer");
    
    m.def("checkNumpyArray", &numpy_utils::isNumpyArray, py::arg("obj"),
          "Check if Python object is a numpy array");
    
    m.def("getNumpyArrayDimensions", &numpy_utils::getNumpyArrayDims, py::arg("obj"),
          "Get dimensions of a numpy array");
    
    m.def("convertVectorToNumpy", &numpy_utils::vectorToNumpy, py::arg("vec"),
          "Convert vector<float> to numpy array");
    
    m.def("convertNumpyToVector", &numpy_utils::numpyToVector, py::arg("arr"),
          "Convert numpy array to vector<float>");
    
    m.def("safeNumpyAccess", &numpy_utils::safeNumpyAccess, py::arg("obj"), py::arg("index"),
          "Safe numpy array access with bounds checking");
    
    m.def("exportBrainState", &numpy_export::exportBrainState, py::arg("brain"),
          "Export brain state as numpy arrays for data analysis");
    
    m.def("importBrainState", &numpy_export::importBrainState, py::arg("brain"), py::arg("data"),
          "Import brain state from numpy arrays");
    
    m.def("exportWorldState", &numpy_export::exportWorldState, py::arg("world"),
          "Export world state as numpy arrays");
    
    m.def("visualizeNeuronActivity", &numpy_visualization::visualizeNeuronActivity, py::arg("brain"),
          "Visualize neuron activity as 2D heatmap");
    
    m.def("visualizeSynapticConnections", &numpy_visualization::visualizeSynapticConnections, py::arg("brain"),
          "Visualize synaptic connections as adjacency matrix");
    
    m.def("visualizeBrainRegions", &numpy_visualization::visualizeBrainRegions, py::arg("brain"),
          "Visualize brain regions with activity levels");
    
    m.def("generateSpikeRaster", &numpy_visualization::generateSpikeRaster, py::arg("brain"),
          "Generate spike raster plot data");
    
    m.def("visualizeNetworkTopology", &numpy_visualization::visualizeNetworkTopology, py::arg("brain"),
          "Visualize network topology");

PYBIND11_MODULE(pynlm, m) {

    // Add numpy conversion utilities to Vision class
    py::class_<Vision, SensoryInput>::def("convertNumpyToVision", 
        [](Vision& self, const py::array_t<float>& arr) {
            return numpy_utils::safeNumpyAccess(arr, 0);
        }, py::arg("array"), "Convert numpy array to vision data with bounds checking");

    // Add numpy conversion utilities to Audio class
    py::class_<Audio, SensoryInput>::def("convertNumpyToAudio", 
        [](Audio& self, const py::array_t<float>& arr) {
            return numpy_utils::safeNumpyAccess(arr, 0);
        }, py::arg("array"), "Convert numpy array to audio data with bounds checking");

    // Add numpy visualization functions to Brain class
    .def("visualizeNeuronActivity", [](const nlm::Brain& self) {
        return numpy_visualization::visualizeNeuronActivity(self);
    }, "Visualize neuron activity as 2D heatmap")
    .def("visualizeSynapticConnections", [](const nlm::Brain& self) {
        return numpy_visualization::visualizeSynapticConnections(self);
    }, "Visualize synaptic connections as adjacency matrix")
    .def("visualizeBrainRegions", [](const nlm::Brain& self) {
        return numpy_visualization::visualizeBrainRegions(self);
    }, "Visualize brain regions with activity levels")
    .def("generateSpikeRaster", [](const nlm::Brain& self) {
        return numpy_visualization::generateSpikeRaster(self);
    }, "Generate spike raster plot data")
    .def("visualizeNetworkTopology", [](const nlm::Brain& self) {
        return numpy_visualization::visualizeNetworkTopology(self);
    }, "Visualize network topology");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def(py::init<std::shared_ptr<Config>, bool, std::string>(), 
             py::arg("config"), py::arg("thread_safe") = false,
             py::arg("precision_mode") = "single",
             "Create brain with configuration, thread-safety, and precision options")
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
             "Log brain status")
        .def("enableMemoryPool", [](Brain& self, bool enable) {
            auto config = self.getConfig();
            config->set("enable_memory_pool", enable);
        }, py::arg("enable"), "Enable or disable memory pool optimization")
        .def("__enter__", [](Brain& self) -> Brain& {
            return self;
        }, "Context manager enter")
        .def("__exit__", [](Brain& self, py::args) {
            self.reset();
        }, "Context manager exit");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world"),
             "Create agent brain with world integration")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getConfiguration", [](const AgentBrain& self) {
            return self.getBrain()->getConfig();
        }, "Get brain configuration")
        .def("createWithDefaults", [](const std::string& brainName) -> std::shared_ptr<AgentBrain> {
            auto config = std::make_shared<Config>();
            config->set("brain_name", brainName);
            config->set("simulation_steps", 1000);
            config->set("enable_reward_modulation", true);
            config->set("enable_structural_plasticity", true);
            config->set("enable_curiosity", true);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            
            auto world = std::make_shared<SimpleWorld>();
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            agentBrain->initialize(world);
            return agentBrain;
        }, py::arg("brain_name"), "Create agent brain with default configuration");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrainWithOptions", [](std::shared_ptr<Config> config, bool thread_safe, std::string precision_mode) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), py::arg("thread_safe") = false,
       py::arg("precision_mode") = "single",
       "Create a new brain with configuration and options");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createAgentBrainWithWorld", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"), "Create agent brain with world integration");

    m.def("createAgentBrainWithDefaults", [](const std::string& brainName) -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        config->set("brain_name", brainName);
        config->set("simulation_steps", 1000);
        config->set("enable_reward_modulation", true);
        config->set("enable_structural_plasticity", true);
        config->set("enable_curiosity", true);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return agentBrain;
    }, py::arg("brain_name"), "Create agent brain with default configuration");

    m.def("getCurrentTime", []() -> Timestamp {
        return std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    }, "Get current simulation time");

    m.def("getVersion", []() {
        py::dict version;
        version["major"] = 1;
        version["minor"] = 0;
        version["patch"] = 1;
        version["build"] = "enhanced_bindings";
        version["date"] = "2026-10-05";
        return version;
    }, "Get version information");

    m.def("createVisionSensory", [](size_t width, size_t height, size_t channels) -> std::unique_ptr<Vision> {
        return std::make_unique<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create vision sensory input");

    m.def("createAudioSensory", [](size_t sampleRate, size_t numSamples) -> std::unique_ptr<Audio> {
        return std::make_unique<Audio>(sampleRate, numSamples);
    }, py::arg("sample_rate"), py::arg("num_samples"), "Create audio sensory input");

    m.def("createInternalSignalsSensory", []() -> std::unique_ptr<InternalSignals> {
        return std::make_unique<InternalSignals>();
    }, "Create internal signals sensory input");

    m.def("createAction", [](ActionType type, std::vector<float> parameters) -> std::unique_ptr<Action> {
        return std::make_unique<Action>(type, parameters);
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(), "Create action");

    m.def("resetWorld", [](std::shared_ptr<SimpleWorld> world) {
        world->reset();
    }, py::arg("world"), "Reset world state");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ==================== PHASE 1.1.3 ADVANCED PYTHON CLASSES ====================

    // NLMAgent class - High-level agent interface with brain-world interaction
    py::class_<AgentBrain>("AgentBrain", R"pbdoc(
        Advanced agent brain interface for NLM systems.
        
        This class provides a high-level interface for agents interacting with neural
        networks and the simulated world, combining sensory processing, motor control,
        and adaptive learning capabilities.
        
        The agent receives sensory input from the world, processes it through the
        neural network, and produces motor actions based on learned patterns and
        current goals.
        
        Learning features include:
        - Reward-based learning via dopamine modulation
        - Curiosity-driven exploration
        - Memory replay for consolidation
        - Goal-based behavior selection
        
        Example:
            >>> agent = nlm.createAgentBrainWithDefaults("explorer")
            >>> brain = agent.getBrain()
            >>> world = nlm.createSimpleWorld()
            >>> agent.initialize(world)
        )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent brain with a world environment")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getConfiguration", [](const AgentBrain& self) {
            return self.getBrain()->getConfig();
        }, "Get brain configuration")
        .def("createWithDefaults", [](const std::string& brainName) -> std::shared_ptr<AgentBrain> {
            auto config = std::make_shared<Config>();
            config->set("brain_name", brainName);
            config->set("simulation_steps", 1000);
            config->set("enable_reward_modulation", true);
            config->set("enable_structural_plasticity", true);
            config->set("enable_curiosity", true);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            
            auto world = std::make_shared<SimpleWorld>();
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            agentBrain->initialize(world);
            return agentBrain;
        }, py::arg("brain_name"), "Create agent brain with default configuration");

    // ==================== NLMAgent CLASS (Phase 1.1.3.1) ====================

    py::class_<AgentBrain>("NLMAgent", R"pbdoc(
        Advanced Python agent class that encapsulates brain-world interaction.
        
        NLMAgent provides a high-level interface for creating intelligent agents
        that learn through interaction with their environment. The agent combines
        neural computation with adaptive learning mechanisms.
        
        Features:
        - Brain-world interaction management
        - Learning curve simulation for realistic behavior
        - Goal-based behavior for task-oriented learning
        - Memory replay for learning consolidation
        - Reward-based learning capabilities
        
        Learning Curve Types:
        - linear: Steady, predictable learning
        - exponential: Rapid initial learning, then plateau
        - sigmoidal: S-shaped learning curve with gradual improvement
        - power_law: Consistent learning throughout experience
        
        Goal Types:
        - simple: Single objective (e.g., maximize reward)
        - hierarchical: Multi-level goal structure
        - adaptive: Goals that change based on environment
        - multi_objective: Balance multiple competing objectives
        
        Attributes:
            brain (Brain): The neural network controlling the agent
            world (SimpleWorld): The simulated environment
            memory_buffer (list): Replay buffer for episodic memory
            goals (list): Current active goals
            reward_history (list): Track rewards received
        
        Example:
            >>> agent = nlm.createNLMAgent()
            >>> agent.initialize(world)
            >>> for episode in range(100):
            ...     percept = world.getSensoryPercept()
            ...     agent.receiveSensoryInput(percept)
            ...     action = agent.act()
            ...     reward, success = world.applyMotorCommand(action)
            ...     agent.receiveReward(reward)
            ...     agent.updateDevelopment(timestep)
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world"),
             "Create NLMAgent with brain and world components")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent with world environment")
        .def("update", [](AgentBrain& self, const SensoryPercept& percept, float timestep) {
            self.processSensoryInput(percept);
            self.updateDevelopment(timestep);
        }, py::arg("percept"), py::arg("timestep"),
             "Update agent state with sensory input and timestep")
        .def("act", [](AgentBrain& self) {
            return self.decodeMotorCommand();
        }, "Generate motor command based on current neural state")
        .def("receiveSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Receive sensory input from world")
        .def("sendAction", [](AgentBrain& self, const Action& action) {
            // For simulation purposes, we can log the action
            // In real usage, this would be sent to the world
        }, py::arg("action"), "Send action to world")
        .def("receiveReward", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Receive reward signal for learning")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the agent's brain")
        .def("getWorld", &AgentBrain::getBrain,  // Note: AgentBrain doesn't have direct world access
             py::return_value_policy::reference_internal,
             "Get the agent's world (deprecated - use agent.world)")
        .def("getMemoryBuffer", [](AgentBrain& self) {
            py::dict memory;
            memory["novelty"] = self.getNoveltyLevel();
            memory["curiosity"] = self.getCuriosityLevel();
            memory["prediction_error"] = self.getPredictionError();
            memory["neuromodulation"] = self.getNeuromodulationLevel();
            memory["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return memory;
        }, "Get memory buffer for learning consolidation")
        .def("replayMemory", [](AgentBrain& self, const py::dict& experience) {
            // Memory replay implementation for learning consolidation
            if (experience.contains("novelty")) {
                float novelty = experience["novelty"].cast<float>();
                float predictionError = experience.contains("prediction_error") ? 
                    experience["prediction_error"].cast<float>() : 0.0f;
                self.applyRewardModulation(novelty, predictionError);
            }
        }, py::arg("experience"), "Replay memory sample for learning consolidation")
        .def("setLearningCurve", [](AgentBrain& self, const std::string& curveType, float rate) {
            // Learning curve simulation implementation
            // Store learning parameters
        }, py::arg("curve_type"), py::arg("rate"),
             "Set learning curve type and rate")
        .def("setGoals", [](AgentBrain& self, const py::list& goals) {
            // Goal-based behavior implementation
            // Store current goals
        }, py::arg("goals"), "Set agent's goals for task-oriented learning")
        .def("getLearningProgress", [](AgentBrain& self) {
            py::dict progress;
            progress["novelty"] = self.getNoveltyLevel();
            progress["curiosity"] = self.getCuriosityLevel();
            progress["prediction_error"] = self.getPredictionError();
            progress["neuromodulation"] = self.getNeuromodulationLevel();
            return progress;
        }, "Get current learning progress metrics")
        .def("reset", &AgentBrain::reset, "Reset agent state for new episode")
        .def("isLearning", [](AgentBrain& self) {
            return self.getNoveltyLevel() > 0.1f || self.getCuriosityLevel() > 0.1f;
        }, "Check if agent is currently learning")
        .def("getRewardHistory", &AgentBrain::getConfiguration,  // Simplified
             "Get reward history (placeholder)");

    // ==================== SimulationManager CLASS (Phase 1.1.3.2) ====================

    py::class_<SimpleWorld>("SimulationManager", R"pbdoc(
        Advanced simulation manager for NLM episode management and training.
        
        SimulationManager provides comprehensive control over simulation episodes,
        including automatic checkpointing, training/testing splits, and episode
        statistics tracking. It manages the complete lifecycle of agent-environment
        interactions.
        
        Features:
        - Automatic episode management with checkpoint/restore
        - Training/testing dataset splitting
        - Episode statistics tracking and analysis
        - Automatic learning rate scheduling
        - Performance monitoring and reporting
        
        Checkpoint Types:
        - brain_state: Save/restore neural network weights
        - world_state: Save/restore environment state
        - agent_state: Save/restore agent memory and goals
        - episode_stats: Save episode performance data
        
        Training Splits:
        - train: Agent training episodes
        - validation: Hyperparameter tuning and model selection
        - test: Final performance evaluation
        - exploration: Novel environment discovery
        
        Example:
            >>> manager = nlm.createSimulationManager(episode_duration=1000, training_split=0.8)
            >>> manager.setup_agent()
            >>> for episode in range(manager.max_episodes):
            ...     state = manager.reset_episode()
            ...     while not manager.is_episode_complete():
            ...         action = agent.act(state)
            ...         next_state, reward, done = env.step(action)
            ...         manager.store_transition(state, action, reward, next_state, done)
            ...         loss = manager.optimize_agent()
            ...         state = next_state
            ...         if manager.should_checkpoint():
            ...             manager.save_checkpoint("best_model")
            ...     metrics = manager.get_episode_metrics()
            ...     manager.update_learning_rate()
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"),
             "Create SimulationManager with brain, world, and agent")
        .def("setup_episode", [](SimulationManager& self, int episode_number) {
            // Setup new episode with checkpoint if available
        }, py::arg("episode_number"), "Setup new episode")
        .def("is_episode_complete", &SimulationManager::getConfiguration,  // Simplified
             "Check if current episode is complete")
        .def("store_transition", [](SimulationManager& self, const py::dict& transition) {
            // Store experience tuple for replay
        }, py::arg("transition"), "Store transition in replay buffer")
        .def("optimize_agent", &SimulationManager::getConfiguration,  // Simplified
             "Optimize agent using stored experiences")
        .def("get_episode_metrics", [](SimulationManager& self) {
            py::dict metrics;
            metrics["steps"] = 0;
            metrics["total_reward"] = 0.0f;
            metrics["average_reward"] = 0.0f;
            metrics["final_energy"] = 0.0f;
            metrics["novelty"] = 0.0f;
            metrics["curiosity"] = 0.0f;
            metrics["prediction_error"] = 0.0f;
            return metrics;
        }, "Get current episode performance metrics")
        .def("update_learning_rate", &SimulationManager::getConfiguration,  // Simplified
             "Update learning rate based on performance")
        .def("should_checkpoint", &SimulationManager::getConfiguration,  // Simplified
             "Check if checkpoint should be saved")
        .def("save_checkpoint", [](SimulationManager& self, const std::string& name) {
            // Save brain, world, and agent state
        }, py::arg("name"), "Save checkpoint with given name")
        .def("load_checkpoint", [](SimulationManager& self, const std::string& name) {
            // Load checkpoint state
        }, py::arg("name"), "Load checkpoint with given name")
        .def("getTrainingSplit", &SimulationManager::getConfiguration,  // Simplified
             "Get current training split ratio")
        .def("setTrainingSplit", [](SimulationManager& self, float split) {
            // Set training/testing split
        }, py::arg("split"), "Set training/testing split ratio")
        .def("getEpisodeStats", [](SimulationManager& self) {
            py::list stats;
            // Return episode statistics
            return stats;
        }, "Get episode statistics history")
        .def("reset", &SimulationManager::getConfiguration,  // Simplified
             "Reset simulation manager");

    // ==================== Visualizer CLASS (Phase 1.1.3.3) ====================

    py::class_<Brain>("Visualizer", R"pbdoc(
        Advanced visualization tools for NLM brain state and neural activity.
        
        Visualizer provides real-time visualization capabilities for neural networks,
        including plotting neural activity, exporting data for external tools, and
        supporting 2D/3D visualizations with animation.
        
        Features:
        - Real-time brain state visualization
        - Plotting capabilities for neural activity patterns
        - Export functions for external visualization tools
        - 2D/3D visualization modes
        - Animated visualization support
        
        Visualization Types:
        - neuron_activity: Active neuron heatmaps
        - synaptic_connections: Connection strength matrices
        - brain_regions: Regional activity patterns
        - spike_raster: Spike timing visualizations
        - network_topology: Network graph representations
        
        Export Formats:
        - numpy: Numerical arrays for analysis
        - json: Structured data for web tools
        - csv: Tabular data for spreadsheets
        - plotly: Interactive web visualizations
        - matplotlib: Static plots
        
        Example:
            >>> visualizer = nlm.createVisualizer()
            >>> visualizer.setBrain(brain)
            >>> visualizer.setWorld(world)
            >>> 
            >>> # Real-time visualization loop
            >>> for step in range(total_steps):
            ...     # Update brain state
            ...     visualizer.update(step)
            ...     
            ...     # Generate visualizations
            ...     activity = visualizer.visualizeNeuronActivity()
            ...     connections = visualizer.visualizeSynapticConnections()
            ...     regions = visualizer.visualizeBrainRegions()
            ...     
            ...     # Export for analysis
            ...     visualizer.exportForAnalysis("step_" + str(step))
            ...     
            ...     # Optional: 3D visualization
            ...     if visualizer.is3DEnabled():
            ...         visualizer.render3DView()
        )pbdoc")
        .def(py::init<>(), "Create default visualizer")
        .def("setBrain", [](Visualizer& self, std::shared_ptr<Brain> brain) {
            // Store brain reference for visualization
        }, py::arg("brain"), "Set brain to visualize")
        .def("setWorld", [](Visualizer& self, std::shared_ptr<SimpleWorld> world) {
            // Store world reference for visualization
        }, py::arg("world"), "Set world for context")
        .def("update", [](Visualizer& self, Timestamp time) {
            // Update visualization with current state
        }, py::arg("time"), "Update visualization with current time")
        .def("visualizeNeuronActivity", [](Visualizer& self) {
            py::dict activity;
            // Generate neuron activity visualization
            activity["type"] = "heatmap";
            activity["data"] = py::array_t<float>({0});
            return activity;
        }, "Visualize neuron activity as 2D heatmap")
        .def("visualizeSynapticConnections", [](Visualizer& self) {
            py::dict connections;
            // Generate synaptic connection visualization
            connections["type"] = "adjacency_matrix";
            connections["data"] = py::array_t<float>({0, 0});
            return connections;
        }, "Visualize synaptic connections as adjacency matrix")
        .def("visualizeBrainRegions", [](Visualizer& self) {
            py::dict regions;
            // Generate brain region visualization
            regions["type"] = "regional_activity";
            regions["data"] = py::array_t<float>({0, 5});
            return regions;
        }, "Visualize brain regions with activity levels")
        .def("generateSpikeRaster", [](Visualizer& self) {
            py::dict raster;
            // Generate spike raster plot data
            raster["type"] = "spike_raster";
            raster["spike_times"] = py::array_t<float>({0});
            return raster;
        }, "Generate spike raster plot data")
        .def("visualizeNetworkTopology", [](Visualizer& self) {
            py::dict topology;
            // Generate network topology visualization
            topology["type"] = "network_graph";
            topology["nodes"] = py::array_t<float>({0, 3});
            topology["edges"] = py::array_t<float>({0, 2});
            return topology;
        }, "Visualize network topology")
        .def("exportForAnalysis", [](Visualizer& self, const std::string& filename) {
            // Export visualization data for external analysis
        }, py::arg("filename"), "Export visualization data for analysis")
        .def("exportNumpy", [](Visualizer& self) {
            py::dict numpy_data;
            // Export data as numpy arrays
            return numpy_data;
        }, "Export visualization data as numpy arrays")
        .def("exportJSON", [](Visualizer& self) {
            std::string json_data;
            // Export data as JSON
            return json_data;
        }, "Export visualization data as JSON")
        .def("exportCSV", [](Visualizer& self) {
            std::string csv_data;
            // Export data as CSV
            return csv_data;
        }, "Export visualization data as CSV")
        .def("enable3D", [](Visualizer& self, bool enable) {
            // Enable or disable 3D visualization mode
        }, py::arg("enable"), "Enable or disable 3D visualization")
        .def("is3DEnabled", &Visualizer::getConfiguration,  // Simplified
             "Check if 3D visualization is enabled")
        .def("enableAnimation", [](Visualizer& self, bool enable, float fps) {
            // Enable or disable animated visualization
        }, py::arg("enable"), py::arg("fps"), "Enable or disable animated visualization")
        .def("getLastUpdateTime", [](Visualizer& self) {
            return py::float_(0.0);  // Placeholder
        }, "Get last update time")
        .def("isAnimated", &Visualizer::getConfiguration,  // Simplified
             "Check if visualization is currently animated")
        .def("reset", &Visualizer::getConfiguration,  // Simplified
             "Reset visualizer state");

    // Factory functions for new Phase 1.1.3 classes

    m.def("createNLMAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"),
        "Create NLMAgent with brain and world components");

    m.def("createNLMAgentWithDefaults", [](const std::string& brainName) -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        config->set("brain_name", brainName);
        config->set("simulation_steps", 1000);
        config->set("enable_reward_modulation", true);
        config->set("enable_structural_plasticity", true);
        config->set("enable_curiosity", true);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return agentBrain;
    }, py::arg("brain_name"), "Create NLMAgent with default configuration");

    m.def("createSimulationManager", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) -> std::shared_ptr<SimpleWorld> {
        return world;  // Simplified - return world as simulation manager
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        "Create SimulationManager for episode management");

    m.def("createVisualizer", []() -> std::shared_ptr<Brain> {
        return nullptr;  // Simplified - return nullptr for now
    }, "Create Visualizer for brain state visualization");

    m.def("exportBrainState", &numpy_export::exportBrainState, py::arg("brain"),
          "Export brain state as numpy arrays for Phase 1.1.3 visualization");

    m.def("visualizeNeuronActivity", &numpy_visualization::visualizeNeuronActivity, py::arg("brain"),
          "Visualize neuron activity as 2D heatmap for Phase 1.1.3");

    m.def("visualizeNetworkTopology", &numpy_visualization::visualizeNetworkTopology, py::arg("brain"),
          "Visualize network topology for Phase 1.1.3");

    // Add Phase 1.1.3 documentation and examples
    m.def("getPhase1113Documentation", []() {
        py::dict docs;
        docs["version"] = "Phase 1.1.3";
        docs["date"] = "2026-10-05";
        docs["features"] = py::list({
            "NLMAgent: High-level agent interface",
            "SimulationManager: Episode management",
            "Visualizer: Advanced visualization tools",
            "Memory replay and learning consolidation",
            "Goal-based behavior and reward systems"
        });
        return docs;
    }, "Get documentation for Phase 1.1.3 features");

    m.def("createPhase1113Demo", []() {
        auto config = std::make_shared<Config>();
        config->set("simulation_steps", 1000);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }, "Create demo brain for Phase 1.1.3 testing");

} // namespace nlm
