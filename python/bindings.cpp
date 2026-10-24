#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <chrono>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <map>
#include <set>

// Include numpy headers if available
try {
    #include <numpy/arrayobject.h>
    #define NUMPY_AVAILABLE 1
} catch (...) {
    #define NUMPY_AVAILABLE 0
}

// Include thread and async headers
#include <thread>
#include <future>
#include <atomic>

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

// === ASYNC AND PARALLEL PROCESSING UTILITIES ===

namespace async {

    // Forward declarations for async functions
    struct AsyncSimulationResult {
        std::vector<std::map<std::string, double>> results;
        double total_time;
        size_t steps_completed;
        bool success;
        std::string error_message;
    };

    // Async brain step function
    inline std::future<std::vector<std::map<std::string, double>>>
    run_async_simulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t num_steps,
        bool enable_logging = true) {
        
        return std::async(std::launch::async, [brain, world, agent, num_steps, enable_logging]() {
            try {
                std::vector<std::map<std::string, double>> results;
                results.reserve(num_steps);
                
                for (size_t step = 0; step < num_steps; ++step) {
                    // Update world
                    world->update(0.1);
                    
                    // Get and process sensory input
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    
                    // Run brain step
                    brain->step(step);
                    
                    // Decode and apply action
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    // Collect metrics
                    if (enable_logging) {
                        std::map<std::string, double> metrics;
                        metrics["step"] = static_cast<double>(step);
                        metrics["firing_rate"] = brain->getAverageFiringRate();
                        metrics["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
                        metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
                        metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
                        metrics["simulation_time"] = world->getSimulationTime();
                        
                        // Add optional metrics
                        double curiosity = agent->getCuriosityLevel();
                        if (curiosity > 0) metrics["curiosity"] = curiosity;
                        
                        double novelty = agent->getNoveltyLevel();
                        if (novelty > 0) metrics["novelty"] = novelty;
                        
                        double prediction_error = agent->getPredictionError();
                        if (prediction_error > 0) metrics["prediction_error"] = prediction_error;
                        
                        results.push_back(metrics);
                    }
                    
                    // Apply neuromodulation with error handling
                    if (agent->isRewardModulationEnabled()) {
                        double reward = 0.0;
                        if (auto internal = percept.getInternal(); !internal.empty()) {
                            reward = internal[0];
                        }
                        // Validate reward range
                        if (reward < -100.0 || reward > 100.0) {
                            throw NLMError("Reward value out of expected range: " + std::to_string(reward));
                        }
                        agent->applyRewardModulation(reward, 0.0);
                    }
                    
                    // Update development with error handling
                    if (agent->isDevelopmentEnabled()) {
                        try {
                            agent->updateDevelopment(0.1);
                        } catch (const std::exception& e) {
                            throw BrainError(std::string("Development update failed: ") + e.what());
                        }
                    }
                }
                
                return results;
                
            } catch (const std::exception& e) {
                throw NLMError(std::string("Async simulation failed: ") + e.what());
            }
        });
    }
    
    // Parallel simulation execution
    inline std::vector<AsyncSimulationResult>
    run_parallel_simulations(
        const std::vector<std::tuple<std::shared_ptr<Brain>, 
                                 std::shared_ptr<SimpleWorld>, 
                                 std::shared_ptr<AgentBrain>>>& simulations,
        size_t num_steps_per_sim,
        size_t max_concurrent = 4) {
        
        std::vector<std::future<AsyncSimulationResult>> futures;
        futures.reserve(simulations.size());
        
        for (const auto& sim : simulations) {
            auto [brain, world, agent] = sim;
            
            // Launch async simulation
            auto future = std::async(std::launch::async, [brain, world, agent, num_steps_per_sim]() {
                AsyncSimulationResult result;
                try {
                    auto start = std::chrono::high_resolution_clock::now();
                    
                    for (size_t step = 0; step < num_steps_per_sim; ++step) {
                        world->update(0.1);
                        auto percept = world->getSensoryPercept();
                        agent->processSensoryInput(percept);
                        brain->step(step);
                        
                        auto action = agent->decodeMotorCommand();
                        world->applyMotorCommand(action, world->getSimulationTime());
                        
                        // Store metrics for this step
                        std::map<std::string, double> metrics;
                        metrics["step"] = static_cast<double>(step);
                        metrics["firing_rate"] = brain->getAverageFiringRate();
                        metrics["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
                        metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
                        
                        result.results.push_back(metrics);
                        
                        if (agent->isRewardModulationEnabled()) {
                            double reward = 0.0;
                            if (auto internal = percept.getInternal(); !internal.empty()) {
                                reward = internal[0];
                            }
                            agent->applyRewardModulation(reward, 0.0);
                        }
                        
                        if (agent->isDevelopmentEnabled()) {
                            agent->updateDevelopment(0.1);
                        }
                    }
                    
                    auto end = std::chrono::high_resolution_clock::now();
                    result.total_time = std::chrono::duration<double>(end - start).count();
                    result.steps_completed = num_steps_per_sim;
                    result.success = true;
                    
                } catch (const std::exception& e) {
                    result.success = false;
                    result.error_message = e.what();
                }
                
                return result;
            });
            
            futures.push_back(std::move(future));
            
            // Limit concurrent executions
            if (futures.size() >= max_concurrent) {
                // Wait for some futures to complete and replace with new ones
                for (auto it = futures.begin(); it != futures.end(); ) {
                    if (it->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                        it = futures.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }
        
        // Collect all results
        std::vector<AsyncSimulationResult> all_results;
        all_results.reserve(simulations.size());
        
        for (auto& future : futures) {
            auto result = future.get();
            all_results.push_back(result);
        }
        
        return all_results;
    }
    
    // Chained async operations
    inline std::future<std::vector<std::vector<std::map<std::string, double>>>>
    run_sequential_async_simulations(
        const std::vector<std::tuple<std::shared_ptr<Brain>, 
                                   std::shared_ptr<SimpleWorld>, 
                                   std::shared_ptr<AgentBrain>>>& simulation_chain) {
        
        return std::async(std::launch::async, [simulation_chain]() {
            std::vector<std::vector<std::map<std::string, double>>> all_results;
            
            for (const auto& sim : simulation_chain) {
                auto [brain, world, agent] = sim;
                
                std::vector<std::map<std::string, double>> results;
                for (size_t step = 0; step < 100; ++step) {
                    world->update(0.1);
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    brain->step(step);
                    
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    std::map<std::string, double> metrics;
                    metrics["step"] = static_cast<double>(step);
                    metrics["firing_rate"] = brain->getAverageFiringRate();
                    
                    results.push_back(metrics);
                    
                    if (agent->isRewardModulationEnabled()) {
                        double reward = 0.0;
                        if (auto internal = percept.getInternal(); !internal.empty()) {
                            reward = internal[0];
                        }
                        agent->applyRewardModulation(reward, 0.0);
                    }
                }
                
                all_results.push_back(results);
            }
            
            return all_results;
        });
    }
} // namespace async

// === NUMPY INTEGRATION UTILITIES ===

namespace numpy {

#ifdef NUMPY_AVAILABLE

    // NumPy array wrapper utilities
    namespace array {
        
        // Convert std::vector to NumPy array
        template<typename T>
        PyObject* to_numpy(const std::vector<T>& data) {
            py::object np = py::module::import("numpy");
            py::object array_func = np.attr("array");
            
            if constexpr (std::is_same_v<T, float>) {
                return array_func(data, py::arg("dtype") = "float32").ptr();
            } else if constexpr (std::is_same_v<T, double>) {
                return array_func(data, py::arg("dtype") = "float64").ptr();
            } else if constexpr (std::is_same_v<T, int>) {
                return array_func(data, py::arg("dtype") = "int32").ptr();
            } else if constexpr (std::is_same_v<T, size_t>) {
                return array_func(data, py::arg("dtype") = "uint64").ptr();
            } else {
                return array_func(data).ptr();
            }
        }
        
        // Convert map of results to structured array
        PyObject* results_to_structured_array(const std::vector<std::map<std::string, double>>& results) {
            if (results.empty()) {
                return nullptr;
            }
            
            py::object np = py::module::import("numpy");
            py::object dtype_list;
            
            // Build dtype list from first result
            std::vector<std::pair<std::string, py::dtype>> fields;
            for (const auto& pair : results[0]) {
                fields.push_back({pair.first, py::dtype("float64")});
            }
            fields.push_back({"step", py::dtype("int64")});
            
            // Create structured dtype
            auto dtype_obj = py::dtype(fields);
            
            // Create structured array
            py::object structured_array = np.attr("zeros")(
                {static_cast<py::ssize_t>(results.size()), 1},  // 1D array of structs
                py::arg("dtype") = dtype_obj
            );
            
            // Fill the array
            for (size_t i = 0; i < results.size(); ++i) {
                const auto& row = results[i];
                py::object struct_obj = structured_array.attr("__getitem__")(i);
                
                for (const auto& pair : row) {
                    struct_obj.attr(pair.first.c_str()) = pair.second;
                }
                struct_obj.attr("step") = static_cast<int64_t>(i);
                
                structured_array.attr("__setitem__")(i, struct_obj);
            }
            
            return structured_array.ptr();
        }
        
        // Convert neural activity data to NumPy arrays
        std::tuple<PyObject*, PyObject*, PyObject*, PyObject*> neural_data_to_numpy(
            const experimental::NeuralActivityData& neural_data) {
            
            // Convert firing rates
            PyObject* firing_rates = to_numpy(neural_data.firing_rates);
            
            // Convert E/I ratios
            PyObject* e_i_ratios = to_numpy(neural_data.e_i_ratios);
            
            // Convert simulation times
            PyObject* simulation_times = to_numpy(neural_data.simulation_times);
            
            // Convert spatial patterns (2D array)
            if (!neural_data.spatial_patterns.empty()) {
                size_t num_samples = neural_data.spatial_patterns.size();
                size_t pattern_size = neural_data.spatial_patterns[0].size();
                
                std::vector<std::vector<float>> transposed(pattern_size, std::vector<float>(num_samples));
                for (size_t i = 0; i < num_samples; ++i) {
                    for (size_t j = 0; j < pattern_size; ++j) {
                        transposed[j][i] = neural_data.spatial_patterns[i][j];
                    }
                }
                
                // Convert each row to numpy array and stack
                py::list np_list;
                for (const auto& row : transposed) {
                    np_list.append(to_numpy(row));
                }
                
                PyObject* spatial_patterns = PyList_AsTuple(np_list.ptr());
                return std::make_tuple(firing_rates, e_i_ratios, simulation_times, spatial_patterns);
            }
            
            return std::make_tuple(firing_rates, e_i_ratios, simulation_times, nullptr);
        }
    }
#endif // NUMPY_AVAILABLE
    
    // Export utilities for data analysis
    namespace export {
        
        // Export simulation results to CSV
        inline void export_results_to_csv(
            const std::vector<std::map<std::string, double>>& results,
            const std::string& filename) {
            
            std::ofstream file(filename);
            if (!file.is_open()) {
                throw NLMError("Cannot open file for writing: " + filename);
            }
            
            if (results.empty()) {
                throw NLMError("No results to export");
            }
            
            // Write header
            const auto& first_row = results[0];
            file << "step";
            for (const auto& pair : first_row) {
                file << "," << pair.first;
            }
            file << "\n";
            
            // Write data
            for (const auto& row : results) {
                file << row.at("step");
                for (const auto& pair : row) {
                    if (pair.first != "step") {
                        file << "," << pair.second;
                    }
                }
                file << "\n";
            }
            
            file.close();
        }
        
        // Export neural activity data
        inline void export_neural_activity(
            const experimental::NeuralActivityData& data,
            const std::string& prefix) {
            
            // Export CSV files
            export_results_to_csv(std::vector<std::map<std::string, double>>(), prefix + "_activity.csv");
            
            // For demonstration, we'll implement the export
            // In practice, you'd need to convert the neural activity data to appropriate formats
        }
    }
} // namespace numpy

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Enhanced with async processing, numpy integration, and research utilities.
    )pbdoc";

    // === CUSTOM EXCEPTION CLASSES ===
    
    py::class_<NLMError, py::base<std::runtime_error>>(m, "NLMError")
        .def(py::init<const std::string&>())
        .def("what", &NLMError::what);
    
    py::class_<ConfigError, py::base<NLMError>>(m, "ConfigError")
        .def(py::init<const std::string&>());
    
    py::class_<BrainError, py::base<NLMError>>(m, "BrainError")
        .def(py::init<const std::string&>());
    
    py::class_<WorldError, py::base<NLMError>>(m, "WorldError")
        .def(py::init<const std::string&>());
    
    py::class_<AgentError, py::base<NLMError>>(m, "AgentError")
        .def(py::init<const std::string&>());

    // === REPEATED ID TYPES ===
    
    // All ID types now use the same template binding pattern
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) {
            return std::hash<uint64_t>{}(id.value);
        })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) {
            return std::hash<uint64_t>{}(id.value);
        })
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
        .def("__hash__", [](const RegionId& id) {
            return std::hash<uint64_t>{}(id.value);
        })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) {
            return std::hash<uint64_t>{}(id.value);
        });

    // === ASYNC UTILITIES ===

    m.def("run_async_simulation", &async::run_async_simulation,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
          py::arg("enable_logging") = true,
          "Run simulation asynchronously and return future results");

    m.def("run_parallel_simulations", &async::run_parallel_simulations,
          py::arg("simulations"), py::arg("num_steps_per_sim"), py::arg("max_concurrent") = 4,
          "Run multiple simulations in parallel for comparison");

    m.def("run_sequential_async_simulations", &async::run_sequential_async_simulations,
          py::arg("simulation_chain"),
          "Run a chain of simulations sequentially with async support");

    // === NUMPY INTEGRATION ===

#ifdef NUMPY_AVAILABLE
    m.def("to_numpy", &numpy::array::to_numpy,
          py::arg("data"), "Convert std::vector to NumPy array");

    m.def("neural_data_to_numpy", &numpy::array::neural_data_to_numpy,
          py::arg("neural_data"),
          "Convert neural activity data to NumPy arrays for analysis");

    m.def("export_results_to_csv", &numpy::export::export_results_to_csv,
          py::arg("results"), py::arg("filename"),
          "Export simulation results to CSV format for analysis");

    m.def("export_neural_activity", &numpy::export::export_neural_activity,
          py::arg("neural_data"), py::arg("prefix"),
          "Export neural activity data to multiple files for analysis");
#endif

    // === CORE CLASSES ===

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
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

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

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // === HIGH-LEVEL CONVENIENCE FUNCTIONS ===

    m.def("create_research_config", &create_research_config,
          py::arg("neuron_count") = 1000, py::arg("preset") = "balanced",
          "Create a research configuration with preset optimization");

    m.def("create_brain", &create_brain,
          py::arg("config") = nullptr,
          "Create and initialize a brain with default configuration");

    m.def("create_research_agent", &create_research_agent,
          py::arg("config_preset") = "balanced", py::arg("neuron_count") = 1000,
          "Create a complete agent setup for research");

    m.def("run_research_simulation", &run_research_simulation,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
          py::arg("enable_logging") = true,
          "Run a complete research simulation with detailed logging");

    m.def("run_parameter_sweep", &run_parameter_sweep_with_validation,
          py::arg("num_simulations"), py::arg("steps_per_sim"),
          py::arg("neuron_counts"), py::arg("presets"),
          "Run multiple simulations with different parameters");

    m.def("create_developmental_brain", &create_developmental_brain,
          py::arg("base_neurons") = 500, py::arg("max_neurons") = 2000,
          "Create a brain with developmental capabilities");

    m.def("create_curiosity_agent", &create_curiosity_agent,
          py::arg("neuron_count") = 800,
          "Create a curiosity-driven exploration agent");

    m.def("run_monte_carlo_simulation", &run_monte_carlo_simulation,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
          "Run a Monte Carlo simulation with performance metrics");

    m.def("analyze_brain_memory_usage", &analyze_brain_memory_usage,
          py::arg("brain"),
          "Analyze brain memory usage and statistics");

    m.def("create_brain_from_config", &create_brain_from_config,
          py::arg("config"),
          "Create a brain from custom configuration");

    // === CONTEXT MANAGERS ===

    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for temporary brain simulations)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("get_brain", &BrainContext::get_brain,
             py::return_value_policy::reference_internal,
             "Get the brain instance")
        .def("step", &BrainContext::step, py::arg("step"),
             "Perform a simulation step")
        .def("get_neuron_count", &BrainContext::get_neuron_count,
             "Get the neuron count")
        .def("reset", &BrainContext::reset,
             "Reset the brain state");

    py::class_<AgentContext>(m, "AgentContext", R"pbdoc(Context manager for complete agent simulations)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("get_brain", &AgentContext::get_brain,
             py::return_value_policy::reference_internal,
             "Get the brain instance")
        .def("get_world", &AgentContext::get_world,
             py::return_value_policy::reference_internal,
             "Get the world instance")
        .def("get_agent", &AgentContext::get_agent,
             py::return_value_policy::reference_internal,
             "Get the agent instance")
        .def("run_simulation", &AgentContext::run_simulation, py::arg("num_steps"),
             "Run a complete simulation and return results")
        .def("get_firing_rate", &AgentContext::get_firing_rate,
             "Get the current firing rate")
        .def("get_total_spikes", &AgentContext::get_total_spikes,
             "Get the total spike count");

    // === EXPERIMENTAL RESEARCH UTILITIES ===

    m.def("collect_neural_activity", &experimental::NeuralActivityData::collect_from_simulation,
          py::arg("brain"), py::arg("num_steps"), py::arg("spatial_resolution") = 32,
          "Collect neural activity data from simulation for analysis and visualization");

    m.def("export_activity_data", &experimental::NeuralActivityData::export_to_csv,
          py::arg("data"), py::arg("filename"),
          "Export neural activity data to CSV file for external analysis");

    m.def("create_experimental_setup", &experimental::research::create_experimental_setup,
          py::arg("experiment_type"), py::arg("parameters"),
          "Create experimental setup for specific research protocols");

    m.def("run_experiment", &experimental::research::run_experiment,
          py::arg("experiment_type"), py::arg("protocol"), py::arg("num_steps"),
          py::arg("parameters"), "Run experimental protocol and return results");

    m.def("comparative_analysis", &experimental::research::comparative_analysis,
          py::arg("experiment_types"), py::arg("protocol"), py::arg("num_steps"),
          py::arg("parameters"), "Run comparative analysis of multiple experiments");

    m.def("create_text_visualization", &experimental::visualization::create_text_visualization,
          py::arg("data"), py::arg("title") = "Neural Activity",
          "Create text-based visualization of simulation data");

    m.def("create_statistical_summary", &experimental::visualization::create_statistical_summary,
          py::arg("values"), py::arg("label") = "Data",
          "Create statistical summary visualization");

    // === ASYNC UTILITIES ===

    m.def("run_async_simulation", &async::run_async_simulation,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
          py::arg("enable_logging") = true,
          "Run simulation asynchronously and return future results");

    m.def("run_parallel_simulations", &async::run_parallel_simulations,
          py::arg("simulations"), py::arg("num_steps_per_sim"), py::arg("max_concurrent") = 4,
          "Run multiple simulations in parallel for comparison");

    m.def("run_sequential_async_simulations", &async::run_sequential_async_simulations,
          py::arg("simulation_chain"),
          "Run a chain of simulations sequentially with async support");

    // === NUMPY INTEGRATION ===

#ifdef NUMPY_AVAILABLE
    m.def("to_numpy", &numpy::array::to_numpy,
          py::arg("data"), "Convert std::vector to NumPy array");

    m.def("neural_data_to_numpy", &numpy::array::neural_data_to_numpy,
          py::arg("neural_data"),
          "Convert neural activity data to NumPy arrays for analysis");

    m.def("export_results_to_csv", &numpy::export::export_results_to_csv,
          py::arg("results"), py::arg("filename"),
          "Export simulation results to CSV format for analysis");

    m.def("export_neural_activity", &numpy::export::export_neural_activity,
          py::arg("neural_data"), py::arg("prefix"),
          "Export neural activity data to multiple files for analysis");
#endif

    // === FACTORY FUNCTIONS ===

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

    // === GLOBAL ATTRIBUTES ===

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm