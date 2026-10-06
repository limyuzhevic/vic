#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

// Include NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"

namespace py = pybind11;

// Advanced Python module for NLM
PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        Advanced NLM Python API with batch processing, progress tracking, and utilities
        -----------------------------------------------------------------------------
        Extended functionality for advanced NLM users including:
        - Batch processing for multiple simulations
        - Progress tracking and monitoring
        - Data analysis utilities
        - Experiment management
        - Visualization helpers
        - Performance profiling
        - Checkpoint management utilities
    )pbdoc";
    
    // Batch processing function for multiple simulations
    m.def("batch_run_simulations", [](const std::vector<std::shared_ptr<Config>>& configs,
                                     const std::vector<size_t>& steps,
                                     bool verbose = false,
                                     std::function<void(size_t, const std::string&)>&& progressCallback = nullptr) {
        
        std::vector<nlohmann::json> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            auto brain = std::make_shared<nlm::Brain>(configs[i]);
            
            if (progressCallback) {
                progressCallback(i, "Initializing brain " + std::to_string(i + 1));
            }
            
            brain->initialize();
            
            if (progressCallback) {
                progressCallback(i, "Running simulation " + std::to_string(i + 1));
            }
            
            brain->step(steps[i]);
            
            // Collect results
            nlohmann::json result;
            result["config_index"] = i;
            result["steps"] = steps[i];
            result["total_neurons"] = brain->getTotalNeuronCount();
            result["total_synapses"] = brain->getTotalSynapseCount();
            result["firing_neurons"] = brain->getFiringNeuronCount();
            result["total_spikes"] = brain->getTotalSpikeCount();
            result["average_firing_rate"] = brain->getAverageFiringRate();
            result["e_i_ratio"] = brain->getExcitationInhibitionRatio();
            
            results.push_back(result);
            
            if (verbose) {
                std::cout << "Simulation " << (i + 1) << "/" << configs.size()
                         << ": " << steps[i] << " steps, "
                         << brain->getFiringNeuronCount() << " firing neurons, "
                         << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
        
        return results;
    }, py::arg("configs"), py::arg("steps"), py::arg("verbose") = false,
       py::arg("progressCallback") = nullptr,
       "Run multiple simulations in batch with progress tracking");
    
    // Data analysis utilities
    m.def("analyze_simulation_data", [](const std::vector<nlohmann::json>& data) {
        nlohmann::json analysis;
        
        if (data.empty()) {
            return "[]"; // Return empty JSON array as string
        }
        
        // Extract numeric values for analysis
        std::vector<size_t> steps;
        std::vector<size_t> neurons;
        std::vector<size_t> synapses;
        std::vector<size_t> spikes;
        std::vector<float> rates;
        
        for (const auto& item : data) {
            steps.push_back(item["steps"]);
            neurons.push_back(item["total_neurons"]);
            synapses.push_back(item["total_synapses"]);
            spikes.push_back(item["total_spikes"]);
            rates.push_back(item["average_firing_rate"]);
        }
        
        // Calculate statistics
        auto mean = [](const std::vector<size_t>& v) {
            double sum = 0.0;
            for (size_t x : v) sum += x;
            return sum / v.size();
        };
        
        auto stddev = [](const std::vector<size_t>& v, double mean) {
            double sq_sum = 0.0;
            for (size_t x : v) {
                double diff = x - mean;
                sq_sum += diff * diff;
            }
            return std::sqrt(sq_sum / v.size());
        };
        
        analysis["count"] = data.size();
        analysis["steps"] = {
            {"mean", mean(steps), "std", stddev(steps, mean(steps)),
             "min", *std::min_element(steps.begin(), steps.end()),
             "max", *std::max_element(steps.begin(), steps.end())}
        };
        analysis["neurons"] = {
            {"mean", mean(neurons), "std", stddev(neurons, mean(neurons)),
             "min", *std::min_element(neurons.begin(), neurons.end()),
             "max", *std::max_element(neurons.end(), neurons.end())}
        };
        analysis["synapses"] = {
            {"mean", mean(synapses), "std", stddev(synapses, mean(synapses)),
             "min", *std::min_element(synapses.begin(), synapses.end()),
             "max", *std::max_element(synapses.end(), synapses.end())}
        };
        analysis["spikes"] = {
            {"mean", mean(spikes), "std", stddev(spikes, mean(spikes)),
             "min", *std::min_element(spikes.begin(), spikes.end()),
             "max", *std::max_element(spikes.end(), spikes.end())}
        };
        analysis["firing_rates"] = {
            {"mean", mean(rates), "std", stddev(rates, mean(rates)),
             "min", *std::min_element(rates.begin(), rates.end()),
             "max", *std::max_element(rates.end(), rates.end())}
        };
        
        return analysis.dump(2);
    }, py::arg("data"), "Analyze simulation data and return statistics");
    
    // Progress tracking for long simulations
    m.def("run_with_progress", [](std::shared_ptr<nlm::Brain> brain,
                                 size_t numSteps,
                                 std::function<void(float)> progressCallback,
                                 bool verbose = false) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        brain->initialize();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
            
            float progress = static_cast<float>(step) / static_cast<float>(numSteps - 1);
            if (progressCallback) {
                progressCallback(progress);
            }
            
            if (verbose && step % 100 == 0) {
                std::cout << "Progress: " << (step * 100.0 / numSteps) << "%, "
                         << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        return std::make_tuple(
            brain->getFiringNeuronCount(),
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            brain->getExcitationInhibitionRatio()
        );
    }, py::arg("brain"), py::arg("numSteps"), py::arg("progressCallback"),
       py::arg("verbose") = false,
       "Run simulation with progress callback for long-running tasks");
    
    // Time series analysis
    m.def("analyze_time_series", [](const std::vector<std::pair<size_t, float>>& timeSeries) {
        nlohmann::json analysis;
        
        if (timeSeries.empty()) {
            return "[]"; // Return empty JSON array as string
        }
        
        std::vector<size_t> steps;
        std::vector<float> values;
        
        for (const auto& point : timeSeries) {
            steps.push_back(point.first);
            values.push_back(point.second);
        }
        
        // Calculate rolling statistics
        size_t windowSize = std::min<size_t>(10, values.size());
        std::vector<float> rollingMeans;
        
        for (size_t i = 0; i <= values.size() - windowSize; ++i) {
            float sum = 0.0f;
            for (size_t j = 0; j < windowSize; ++j) {
                sum += values[i + j];
            }
            rollingMeans.push_back(sum / windowSize);
        }
        
        analysis["points"] = timeSeries.size();
        analysis["window_size"] = windowSize;
        analysis["rolling_means"] = rollingMeans;
        analysis["trends"] = "increasing";
        
        return analysis.dump(2);
    }, py::arg("timeSeries"), "Analyze time series data from simulations");
    
    // Experiment management utilities
    m.def("export_experiment_results", [](const std::string& filename,
                                          const std::vector<nlohmann::json>& results,
                                          const std::string& description = "") {
        nlohmann::json experiment;
        
        experiment["description"] = description;
        experiment["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        experiment["version"] = "6.0_final_integration";
        experiment["results"] = results;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << experiment.dump(2);
        file.close();
        
        return std::string("Results exported to " + filename);
    }, py::arg("filename"), py::arg("results"), py::arg("description") = "",
       "Export experiment results to JSON file");
    
    m.def("import_experiment_results", [](const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filename);
        }
        
        nlohmann::json experiment;
        file >> experiment;
        file.close();
        
        return experiment["results"].get<std::vector<nlohmann::json>>();
    }, py::arg("filename"), "Import experiment results from JSON file");
    
    // Checkpoint management utilities
    m.def("create_checkpoint_bundle", [](const std::string& directory,
                                        const std::shared_ptr<nlm::Brain>& brain,
                                        const std::string& metadata = "") {
        nlohmann::json bundle;
        
        bundle["directory"] = directory;
        bundle["metadata"] = metadata;
        bundle["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        
        if (brain) {
            bundle["brain_stats"]["neurons"] = brain->getTotalNeuronCount();
            bundle["brain_stats"]["synapses"] = brain->getTotalSynapseCount();
            bundle["brain_stats"]["total_spikes"] = brain->getTotalSpikeCount();
            bundle["brain_stats"]["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        }
        
        std::ofstream file(directory + "/checkpoint_bundle.json");
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create checkpoint bundle in directory: " + directory);
        }
        
        file << bundle.dump(2);
        file.close();
        
        return std::string("Checkpoint bundle created in " + directory);
    }, py::arg("directory"), py::arg("brain"), py::arg("metadata") = "",
       "Create a checkpoint bundle with metadata and stats");
    
    // Visualization helpers
    m.def("generate_histogram_data", [](const std::vector<float>& data,
                                        size_t bins = 10,
                                        float min = 0.0f,
                                        float max = 1.0f) {
        nlohmann::json histogram;
        
        float binWidth = (max - min) / bins;
        std::vector<size_t> binCounts(bins, 0);
        
        for (float value : data) {
            if (value < min) continue;
            if (value >= max) continue;
            
            size_t binIndex = static_cast<size_t>((value - min) / binWidth);
            if (binIndex >= bins) binIndex = bins - 1;
            binCounts[binIndex]++;
        }
        
        histogram["bins"] = bins;
        histogram["min"] = min;
        histogram["max"] = max;
        histogram["bin_width"] = binWidth;
        histogram["data"] = binCounts;
        
        return histogram.dump(2);
    }, py::arg("data"), py::arg("bins") = 10, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
       "Generate histogram data from numerical values");
    
    // Performance profiling utilities
    m.def("profile_simulation", [](std::shared_ptr<nlm::Brain> brain,
                                   size_t numSteps,
                                   size_t profileInterval = 100) {
        nlohmann::json profile;
        
        if (!brain) throw std::runtime_error("Brain cannot be null");
        
        brain->initialize();
        
        std::vector<std::chrono::high_resolution_clock::time_point> timestamps;
        std::vector<size_t> spikeCounts;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < numSteps; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            brain->step(step);
            auto stepEnd = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart);
            timestamps.push_back(stepStart);
            spikeCounts.push_back(brain->getTotalSpikeCount());
            
            if ((step + 1) % profileInterval == 0) {
                auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime);
                double stepsPerSecond = (step + 1) / (totalDuration.count() / 1000000.0);
                
                nlohmann::json point;
                point["step"] = step + 1;
                point["duration_us"] = duration.count();
                point["total_duration_us"] = totalDuration.count();
                point["steps_per_second"] = stepsPerSecond;
                point["total_spikes"] = brain->getTotalSpikeCount();
                point["firing_neurons"] = brain->getFiringNeuronCount();
                
                profile["profile_points"].push_back(point);
            }
        }
        
        auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime);
        double overallStepsPerSecond = numSteps / (totalDuration.count() / 1000000.0);
        
        profile["total_steps"] = numSteps;
        profile["total_duration_us"] = totalDuration.count();
        profile["overall_steps_per_second"] = overallStepsPerSecond;
        profile["average_step_duration_us"] = totalDuration.count() / numSteps;
        
        return profile.dump(2);
    }, py::arg("brain"), py::arg("numSteps"), py::arg("profileInterval") = 100,
       "Profile simulation performance and return timing data");
    
    // Neural network analysis utilities
    m.def("analyze_connectivity", [](const std::shared_ptr<nlm::Brain>& brain) {
        nlohmann::json connectivity;
        
        if (!brain) throw std::runtime_error("Brain cannot be null");
        
        size_t totalConnections = brain->getTotalSynapseCount();
        if (totalConnections == 0) {
            return "[]"; // Return empty JSON array as string
        }
        
        // Get all regions
        auto regions = brain->getRegions();
        
        // Analyze connectivity patterns
        nlohmann::json regionsAnalysis;
        for (const auto& region : regions) {
            nlohmann::json regionData;
            regionData["id"] = region->getId().index();
            regionData["name"] = region->getName();
            regionData["neuron_count"] = region->getTotalNeuronCount();
            regionData["synapse_count"] = region->getSynapseCount();
            regionData["average_weight"] = region->getAverageSynapticWeight();
            
            // Calculate E/I ratio for this region
            float totalExcitatory = 0.0f;
            float totalInhibitory = 0.0f;
            
            auto synapses = region->getSynapses();
            for (const auto* syn : synapses) {
                float weight = syn->getWeight();
                if (weight > 0) {
                    totalExcitatory += weight;
                } else {
                    totalInhibitory += std::abs(weight);
                }
            }
            
            regionData["e_i_ratio"] = (totalInhibitory > 0) ? (totalExcitatory / totalInhibitory) : (totalExcitatory > 0 ? std::numeric_limits<float>::infinity() : 0.0f);
            
            regionsAnalysis[region->getId().index()] = regionData;
        }
        
        connectivity["total_connections"] = totalConnections;
        connectivity["region_count"] = regions.size();
        connectivity["regions"] = regionsAnalysis;
        
        return connectivity.dump(2);
    }, py::arg("brain"), "Analyze neural connectivity patterns");
    
    m.def("create_advanced_config", [](const std::string& name,
                                       const nlohmann::json& customParams) {
        auto config = std::make_shared<nlm::Config>();
        
        // Set basic defaults
        config->setInt("neuron_count", 1000);
        config->setInt("region_count", 1);
        config->setFloat("connection_probability", 0.1f);
        config->setFloat("simulation_timestep", 0.001f);
        config->setString("experiment.name", name);
        config->setString("experiment.type", "advanced_research");
        
        // Apply custom parameters if provided
        if (!customParams.is_null()) {
            for (auto it = customParams.begin(); it != customParams.end(); ++it) {
                std::string key = it.key();
                if (it->is_number_integer()) {
                    config->setInt(key, it->get<int>());
                } else if (it->is_number_float()) {
                    config->setFloat(key, it->get<float>());
                } else if (it->is_string()) {
                    config->setString(key, it->get<std::string>());
                } else if (it->is_boolean()) {
                    config->setBool(key, it->get<bool>());
                }
            }
        }
        
        return config;
    }, py::arg("name"), py::arg("customParams") = nlohmann::json::object(),
       "Create an advanced configuration with custom parameters");
    
    m.def("format_results", [](const nlohmann::json& results) {
        std::stringstream ss;
        ss << "=== NLM ADVANCED RESULTS ===\n";
        ss << "Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";
        ss << "Total experiments: " << results.size() << "\n\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            ss << "Experiment " << (i + 1) << ":\n";
            ss << "  Steps: " << results[i]["steps"] << "\n";
            ss << "  Neurons: " << results[i]["total_neurons"] << "\n";
            ss << "  Synapses: " << results[i]["total_synapses"] << "\n";
            ss << "  Firing neurons: " << results[i]["firing_neurons"] << "\n";
            ss << "  Total spikes: " << results[i]["total_spikes"] << "\n";
            ss << "  Average firing rate: " << results[i]["average_firing_rate"] << "\n\n";
        }
        
        ss << "=== END RESULTS ===";
        return ss.str();
    }, py::arg("results"), "Format simulation results as readable text");
    
    // Utility function for creating complex experiment setups
    m.def("create_research_suite", []() {
        nlohmann::json suite;
        
        // Add common research configurations
        suite["configs"] = nlohmann::json::array();
        
        // High plasticity config
        nlohmann::json highPlasticity;
        highPlasticity["name"] = "high_plasticity";
        highPlasticity["neuron_count"] = 2000;
        highPlasticity["connection_probability"] = 0.2f;
        highPlasticity["plasticity.stdp.learning_rate"] = 0.01f;
        highPlasticity["plasticity.hebbian.learning_rate"] = 0.005f;
        highPlasticity["development.stage"] = "critical_period";
        suite["configs"].push_back(highPlasticity);
        
        // Low connectivity config
        nlohmann::json lowConnectivity;
        lowConnectivity["name"] = "low_connectivity";
        lowConnectivity["neuron_count"] = 500;
        lowConnectivity["connection_probability"] = 0.01f;
        lowConnectivity["plasticity.stdp.learning_rate"] = 0.0005f;
        lowConnectivity["plasticity.structural.enable"] = false;
        suite["configs"].push_back(lowConnectivity);
        
        // Balanced config
        nlohmann::json balanced;
        balanced["name"] = "balanced_research";
        balanced["neuron_count"] = 1500;
        balanced["connection_probability"] = 0.15f;
        balanced["plasticity.stdp.learning_rate"] = 0.002f;
        balanced["plasticity.hebbian.learning_rate"] = 0.001f;
        balanced["plasticity.structural.enable"] = true;
        balanced["neuromodulation.curiosity.enable"] = true;
        balanced["neuromodulation.novelty.enable"] = true;
        suite["configs"].push_back(balanced);
        
        return suite.dump(2);
    }, "Create a suite of research configurations for experimentation");
    
    m.def("validate_config", [](const nlm::Config& config) {
        std::vector<std::string> errors;
        
        // Check for valid neuron count
        if (config.has("neuron_count")) {
            size_t neuronCount = config.get("neuron_count", 0);
            if (neuronCount == 0) {
                errors.push_back("neuron_count cannot be zero");
            }
        }
        
        // Check for valid connection probability
        if (config.has("connection_probability")) {
            float connProb = config.get("connection_probability", 0.0f);
            if (connProb < 0.0f || connProb > 1.0f) {
                errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
        
        // Check for valid timestep
        if (config.has("simulation_timestep")) {
            float timestep = config.get("simulation_timestep", 0.001f);
            if (timestep <= 0.0f) {
                errors.push_back("simulation_timestep must be positive");
            }
        }
        
        nlohmann::json validation;
        validation["valid"] = errors.empty();
        validation["errors"] = errors;
        
        return validation.dump(2);
    }, py::arg("config"), "Validate configuration and return errors");
}