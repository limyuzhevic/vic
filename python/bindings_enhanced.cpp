#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "python/bindings.cpp"

namespace py = pybind11;

namespace nlm {

class PythonBrainWrapper {
public:
    PythonBrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {}

    // Simulation control
    void step(size_t steps = 1) {
        for (size_t i = 0; i < steps; ++i) {
            brain_->step(i);
        }
    }

    void stepWithTime(size_t step, double time) {
        brain_->step(step, time);
    }

    // Data recording and analysis
    std::vector<double> recordSpikeTimes(size_t steps) {
        std::vector<double> spike_times;
        size_t initial_spikes = brain_->getTotalSpikeCount();

        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step);
            size_t current_spikes = brain_->getTotalSpikeCount();
            spike_times.insert(spike_times.end(), current_spikes - initial_spikes, step * 0.001);
        }

        return spike_times;
    }

    // Learning analysis
    std::map<std::string, double> analyzeLearning(size_t window_size = 100) {
        std::map<std::string, double> metrics;

        // Calculate various learning metrics
        metrics["firing_rate"] = brain_->getAverageFiringRate();
        metrics["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        metrics["total_spikes"] = static_cast<double>(brain_->getTotalSpikeCount());
        metrics["active_neurons"] = static_cast<double>(brain_->getActiveNeuronCount());
        metrics["firing_neurons"] = static_cast<double>(brain_->getFiringNeuronCount());

        // Add more complex learning metrics based on window analysis
        return metrics;
    }

    // State management
    bool saveState(const std::string& filename) const {
        return brain_->save(filename);
    }

    bool loadState(const std::string& filename) {
        return brain_->load(filename);
    }

    // Configuration access
    std::shared_ptr<Config> getConfig() const {
        return brain_->getConfig();
    }

    // Convenience methods
    void injectSensoryInput(const std::vector<float>& data) {
        // Create a simple sensory input from data
        auto sensory_input = std::make_shared<SensoryInput>();
        sensory_input->setData(data);
        brain_->receiveSensoryInput(*sensory_input);
    }

    std::vector<float> getMotorOutput() {
        auto action = brain_->produceAction();
        std::vector<float> output;

        if (action) {
            output.push_back(static_cast<float>(action->getType()));
            output.insert(output.end(), action->getParameters().begin(), action->getParameters().end());
        }

        return output;
    }

    // Monitoring and statistics
    std::string getStatusReport() const {
        std::ostringstream oss;
        oss << "=== NLM Brain Status ===" << std::endl;
        oss << "Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        oss << "Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        oss << "Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
        oss << "Firing neurons (this step): " << brain_->getFiringNeuronCount() << std::endl;
        oss << "Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
        oss << "Pending spike events: " << brain_->getPendingSpikeEventCount() << std::endl;
        oss << "Average firing rate: " << brain_->getAverageFiringRate() << std::endl;
        oss << "E/I ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;

        return oss.str();
    }

    // Development stage
    std::string getDevelopmentalStage() const {
        switch (brain_->getDevelopmentalStage()) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }

private:
    std::shared_ptr<Brain> brain_;
};

class PythonSimulation {
public:
    PythonSimulation(size_t duration_steps = 1000,
                    const std::string& config_file = "configs/default.cfg")
        : duration_steps_(duration_steps), config_file_(config_file) {}

    PythonSimulation& setTimestep(double timestep) {
        timestep_ = timestep;
        return *this;
    }

    PythonSimulation& setNeuronCount(size_t neuron_count) {
        neuron_count_ = neuron_count;
        return *this;
    }

    PythonSimulation& setRegionCount(size_t region_count) {
        region_count_ = region_count;
        return *this;
    }

    PythonSimulation& enablePlasticity(bool enable = true) {
        enable_plasticity_ = enable;
        return *this;
    }

    PythonSimulation& enableDevelopment(bool enable = true) {
        enable_development_ = enable;
        return *this;
    }

    PythonSimulation& enableNeuromodulation(bool enable = true) {
        enable_neuromodulation_ = enable;
        return *this;
    }

    PythonBrainWrapper run() {
        // Create and configure brain
        auto config = std::make_shared<Config>();
        config->loadFromFile(config_file_);

        // Apply settings
        config->set("simulation_timestep", timestep_, ConfigSource::User);
        config->set("neuron_count", static_cast<int64_t>(neuron_count_), ConfigSource::User);
        config->set("region_count", static_cast<int64_t>(region_count_), ConfigSource::User);

        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();

        // Record initial state
        std::vector<std::map<std::string, double>> history;

        // Run simulation
        for (size_t step = 0; step < duration_steps_; ++step) {
            brain->step(step);

            // Record metrics every 10 steps
            if (step % 10 == 0) {
                std::map<std::string, double> metrics;
                metrics["step"] = static_cast<double>(step);
                metrics["time"] = step * timestep_;
                metrics["firing_rate"] = brain->getAverageFiringRate();
                metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
                metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
                metrics["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());

                history.push_back(metrics);
            }
        }

        // Create wrapper and attach history
        PythonBrainWrapper wrapper(brain);
        // In a real implementation, we would attach the history to the wrapper

        return wrapper;
    }

private:
    size_t duration_steps_ = 1000;
    std::string config_file_ = "configs/default.cfg";
    double timestep_ = 0.001;
    size_t neuron_count_ = 500;
    size_t region_count_ = 1;
    bool enable_plasticity_ = true;
    bool enable_development_ = true;
    bool enable_neuromodulation_ = true;
};

class PythonLearningExperiment {
public:
    PythonLearningExperiment() = default;

    PythonLearningExperiment& setName(const std::string& name) {
        name_ = name;
        return *this;
    }

    PythonLearningExperiment& setDuration(size_t steps) {
        duration_steps_ = steps;
        return *this;
    }

    PythonLearningExperiment& setConfig(const std::string& config_file) {
        config_file_ = config_file;
        return *this;
    }

    PythonLearningExperiment& addInputPattern(const std::vector<float>& pattern) {
        input_patterns_.push_back(pattern);
        return *this;
    }

    PythonLearningExperiment& setPlasticityEnabled(bool enable) {
        plasticity_enabled_ = enable;
        return *this;
    }

    PythonLearningExperiment& setDevelopmentEnabled(bool enable) {
        development_enabled_ = enable;
        return *this;
    }

    std::map<std::string, py::object> run() {
        std::map<std::string, py::object> results;

        try {
            // Create configuration
            auto config = std::make_shared<Config>();
            config->loadFromFile(config_file_);

            // Create brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();

            // Record initial state
            std::vector<float> initial_weights;
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                for (const auto& syn : region->getSynapses()) {
                    initial_weights.push_back(syn->getWeight());
                }
            }

            // Run experiment
            for (size_t step = 0; step < duration_steps_; ++step) {
                // Apply input patterns
                if (step < input_patterns_.size()) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }

                // Apply development if enabled
                if (development_enabled_) {
                    brain->develop();
                }

                brain->step(step);
            }

            // Record final state
            std::vector<float> final_weights;
            if (region) {
                for (const auto& syn : region->getSynapses()) {
                    final_weights.push_back(syn->getWeight());
                }
            }

            // Calculate learning metrics
            std::map<std::string, double> learning_metrics;

            if (!initial_weights.empty() && !final_weights.empty()) {
                float initial_mean = std::accumulate(initial_weights.begin(), initial_weights.end(), 0.0f) / initial_weights.size();
                float final_mean = std::accumulate(final_weights.begin(), final_weights.end(), 0.0f) / final_weights.size();
                learning_metrics["weight_change"] = final_mean - initial_mean;
                learning_metrics["initial_mean_weight"] = initial_mean;
                learning_metrics["final_mean_weight"] = final_mean;
                learning_metrics["learning_significance"] = std::abs(final_mean - initial_mean);
            }

            learning_metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
            learning_metrics["firing_rate"] = brain->getAverageFiringRate();
            learning_metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
            learning_metrics["steps_completed"] = static_cast<double>(duration_steps_);

            results["name"] = name_;
            results["learning_metrics"] = learning_metrics;
            results["initial_weights"] = initial_weights;
            results["final_weights"] = final_weights;
            results["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
            results["success"] = true;

        } catch (const std::exception& e) {
            results["success"] = false;
            results["error"] = std::string(e.what());
        }

        return results;
    }

private:
    std::string name_ = "Learning Experiment";
    size_t duration_steps_ = 1000;
    std::string config_file_ = "configs/default.cfg";
    std::vector<std::vector<float>> input_patterns_;
    bool plasticity_enabled_ = true;
    bool development_enabled_ = true;
};

void exportEnhancedPythonBindings(py::module& m) {
    // PythonBrainWrapper
    py::class_<PythonBrainWrapper>(m, "BrainWrapper")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("step", &PythonBrainWrapper::step, py::arg("steps") = 1)
        .def("step_with_time", &PythonBrainWrapper::stepWithTime,
             py::arg("step"), py::arg("time"))
        .def("record_spike_times", &PythonBrainWrapper::recordSpikeTimes,
             py::arg("steps") = 100)
        .def("analyze_learning", &PythonBrainWrapper::analyzeLearning,
             py::arg("window_size") = 100)
        .def("save_state", &PythonBrainWrapper::saveState, py::arg("filename"))
        .def("load_state", &PythonBrainWrapper::loadState, py::arg("filename"))
        .def("get_config", &PythonBrainWrapper::getConfig)
        .def("inject_sensory_input", &PythonBrainWrapper::injectSensoryInput)
        .def("get_motor_output", &PythonBrainWrapper::getMotorOutput)
        .def("get_status_report", &PythonBrainWrapper::getStatusReport)
        .def("get_developmental_stage", &PythonBrainWrapper::getDevelopmentalStage);

    // PythonSimulation
    py::class_<PythonSimulation>(m, "Simulation")
        .def(py::init<size_t, const std::string&>(),
             py::arg("duration_steps") = 1000,
             py::arg("config_file") = "configs/default.cfg")
        .def("set_timestep", &PythonSimulation::setTimestep, py::arg("timestep"))
        .def("set_neuron_count", &PythonSimulation::setNeuronCount, py::arg("neuron_count"))
        .def("set_region_count", &PythonSimulation::setRegionCount, py::arg("region_count"))
        .def("enable_plasticity", &PythonSimulation::enablePlasticity, py::arg("enable") = true)
        .def("enable_development", &PythonSimulation::enableDevelopment, py::arg("enable") = true)
        .def("enable_neuromodulation", &PythonSimulation::enableNeuromodulation, py::arg("enable") = true)
        .def("run", &PythonSimulation::run);

    // PythonLearningExperiment
    py::class_<PythonLearningExperiment>(m, "LearningExperiment")
        .def(py::init<>())
        .def("set_name", &PythonLearningExperiment::setName, py::arg("name"))
        .def("set_duration", &PythonLearningExperiment::setDuration, py::arg("steps"))
        .def("set_config", &PythonLearningExperiment::setConfig, py::arg("config_file"))
        .def("add_input_pattern", &PythonLearningExperiment::addInputPattern)
        .def("set_plasticity_enabled", &PythonLearningExperiment::setPlasticityEnabled)
        .def("set_development_enabled", &PythonLearningExperiment::setDevelopmentEnabled)
        .def("run", &PythonLearningExperiment::run);

    // Factory functions
    m.def("create_simulation", [](size_t duration_steps, const std::string& config_file) {
        return PythonSimulation(duration_steps, config_file);
    }, py::arg("duration_steps") = 1000,
       py::arg("config_file") = "configs/default.cfg",
       "Create a new simulation with the specified duration and configuration");

    m.def("create_learning_experiment", []() {
        return PythonLearningExperiment();
    }, "Create a new learning experiment");

    m.def("create_brain_wrapper", [](std::shared_ptr<Brain> brain) {
        return PythonBrainWrapper(brain);
    }, py::arg("brain"), "Create a brain wrapper for enhanced Python API");

    // Convenience functions
    m.def("run_phase2_demo", []() {
        // Run the classic Phase 2 demo
        return PythonSimulation(1000).enablePlasticity().enableDevelopment().enableNeuromodulation().run();
    }, "Run the classic Phase 2 demonstration");

    m.def("run_learning_experiment", []() {
        // Run a standard learning experiment
        return PythonLearningExperiment()
            .set_name("Standard Learning Experiment")
            .set_duration(1000)
            .set_plasticity_enabled(true)
            .set_development_enabled(true)
            .run();
    }, "Run a standard learning experiment");
}

} // namespace nlm
