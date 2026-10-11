# NLM Python Bindings - Enhanced user interaction and scripting interface
# Provides Python access to brain simulation, control, and analysis

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace py = pybind11;

namespace nlm {

// Python wrapper for Brain class
class BrainWrapper {
public:
    BrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Brain properties
    py::dict getBrainState() const {
        py::dict state;
        state["step"] = brain_->getTotalSpikeCount();
        state["neurons"] = brain_->getTotalNeuronCount();
        state["synapses"] = brain_->getTotalSynapseCount();
        state["active_neurons"] = brain_->getActiveNeuronCount();
        state["firing_neurons"] = brain_->getFiringNeuronCount();
        state["spikes"] = brain_->getTotalSpikeCount();
        state["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        return state;
    }
    
    // Configuration methods
    bool setConfig(const std::string& key, const py::object& value) {
        try {
            if (value.is(py::int_())) {
                return brain_->getConfig()->set(key, value.cast<int64_t>(), ConfigSource::Python);
            } else if (value.is(py::float_())) {
                return brain_->getConfig()->set(key, value.cast<float>(), ConfigSource::Python);
            } else if (value.is(py::str())) {
                return brain_->getConfig()->set(key, value.cast<std::string>(), ConfigSource::Python);
            } else if (value.is(py::bool_())) {
                return brain_->getConfig()->set(key, value.cast<bool>(), ConfigSource::Python);
            }
            return false;
        } catch (const std::exception& e) {
            std::cerr << "Error setting config parameter: " << e.what() << std::endl;
            return false;
        }
    }
    
    py::object getConfig(const std::string& key) const {
        try {
            auto config = brain_->getConfig();
            if (auto intVal = config->get<int64_t>(key)) {
                return py::int_(*intVal);
            } else if (auto floatVal = config->get<float>(key)) {
                return py::float_(*floatVal);
            } else if (auto stringVal = config->get<std::string>(key)) {
                return py::str(*stringVal);
            } else if (auto boolVal = config->get<bool>(key)) {
                return py::bool_(*boolVal);
            }
            return py::none();
        } catch (const std::exception& e) {
            std::cerr << "Error getting config parameter: " << e.what() << std::endl;
            return py::none();
        }
    }
    
    // Control methods
    void step(int steps = 1) {
        for (int i = 0; i < steps; ++i) {
            brain_->step(i, i * 0.001);
        }
    }
    
    void reset() {
        brain_->reset();
    }
    
    bool save(const std::string& filepath) {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Data access methods
    py::array_t<float> getNeuronPotentials() const {
        // Return membrane potentials of all neurons
        size_t neuronCount = brain_->getTotalNeuronCount();
        py::array_t<float> potentials({neuronCount});
        auto buf = potentials.mutable_unchecked<1>();
        
        size_t idx = 0;
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    buf[idx++] = neuron->getMembranePotential();
                }
            }
        }
        
        return potentials;
    }
    
    py::array_t<float> getNeuronActivations() const {
        // Return activation levels (deviation from resting potential)
        size_t neuronCount = brain_->getTotalNeuronCount();
        py::array_t<float> activations({neuronCount});
        auto buf = activations.mutable_unchecked<1>();
        
        size_t idx = 0;
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    float activation = std::abs(neuron->getState().membranePotential - 
                                              neuron->getState().restingPotential);
                    buf[idx++] = activation;
                }
            }
        }
        
        return activations;
    }
    
    py::list getSynapseWeights() const {
        py::list weights;
        
        for (const auto& region : brain_->getRegions()) {
            for (const auto* synapse : region->getSynapses()) {
                weights.append(synapse->getWeight());
            }
        }
        
        return weights;
    }
    
    // Simulation control
    void runExperiment(const std::string& type, const py::dict& parameters) {
        if (type == "plasticity") {
            int steps = parameters.get("steps", py::int_(1000)).cast<int>();
            float current = parameters.get("current", py::float_(30.0f)).cast<float>();
            
            // Run plasticity experiment
            brain_->reset();
            brain_->initialize();
            
            for (int step = 0; step < steps; ++step) {
                // Inject current based on pattern
                if (step % 10 == 0) {
                    brain_->injectCurrentToNeurons(NeuronType::Sensory, current);
                }
                
                brain_->step(step, step * 0.001);
            }
        } else if (type == "connectivity") {
            int steps = parameters.get("steps", py::int_(50)).cast<int>();
            float current = parameters.get("current", py::float_(50.0f)).cast<float>();
            
            // Run connectivity test
            for (int step = 0; step < steps; ++step) {
                brain_->injectCurrentToNeurons(NeuronType::Internal, current);
                brain_->step(step, step * 0.001);
            }
        } else if (type == "simulation") {
            double duration = parameters.get("duration", py::float_(10.0f)).cast<double>();
            int steps = static_cast<int>(duration * 1000.0); // 1ms timestep
            
            for (int step = 0; step < steps; ++step) {
                brain_->step(step, step * 0.001);
            }
        }
    }
    
    // Visualization and analysis
    std::string generateActivityPlot() const {
        // Generate simple ASCII plot of neural activity
        std::stringstream plot;
        auto potentials = getNeuronPotentials();
        
        plot << "Neural Activity Distribution\n";
        plot << "==========================\n";
        
        // Calculate statistics
        float min = potentials[0];
        float max = potentials[0];
        float sum = 0.0f;
        
        for (size_t i = 0; i < potentials.size(); ++i) {
            float val = potentials[i];
            if (val < min) min = val;
            if (val > max) max = val;
            sum += val;
        }
        
        float avg = sum / potentials.size();
        plot << "Min: " << min << "\n";
        plot << "Max: " << max << "\n";
        plot << "Avg: " << avg << "\n";
        plot << "Count: " << potentials.size() << "\n";
        
        return plot.str();
    }
    
    // Event callbacks
    void setSpikeCallback(std::function<void(int, float)> callback) {
        // Register callback for spike events
        if (auto spikeSystem = brain_->getSpikeSystem()) {
            spikeSystem->registerHandler([callback](const DetailedSpikeEvent& event) {
                callback(event.neuron_id, event.time);
            });
        }
    }
    
    // Statistics and metrics
    py::dict getPerformanceMetrics() const {
        py::dict metrics;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Measure step performance
        int testSteps = 100;
        auto testStart = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < testSteps; ++i) {
            brain_->step(i, i * 0.001);
        }
        
        auto testEnd = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(testEnd - testStart);
        
        metrics["steps_per_second"] = static_cast<double>(testSteps) / (duration.count() / 1000000.0);
        metrics["neurons_processed"] = brain_->getTotalNeuronCount();
        metrics["synapses_updated"] = brain_->getTotalSynapseCount();
        metrics["spikes_generated"] = brain_->getTotalSpikeCount();
        
        return metrics;
    }
    
    // Version and info
    std::string getVersion() const {
        return "NLM Python Bindings 1.0.0";
    }
    
    std::string getDescription() const {
        return "Python interface for Neural Learning Machine (NLM) brain simulation";
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Python module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine) - Advanced brain simulation";
    
    // Register exception classes
    py::register_exception<BrainException, std::runtime_error>(m, "BrainException");
    py::register_exception<BrainInitializationError, BrainException>(m, "BrainInitializationError");
    py::register_exception<BrainStepError, BrainException>(m, "BrainStepError");
    
    // Register configuration classes
    py::class_<Configuration>(m, "Configuration")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("validate", &Configuration::validateSchema)
        .def("set", &Configuration::set<std::any>)
        .def("get", &Configuration::getOr<std::any>)
        .def("load_from_file", &Configuration::loadFromFile)
        .def("load_from_args", &Configuration::loadFromArgs);
    
    // Register brain wrapper class
    py::class_<BrainWrapper>(m, "Brain")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("get_state", &BrainWrapper::getBrainState)
        .def("step", &BrainWrapper::step)
        .def("reset", &BrainWrapper::reset)
        .def("save", &BrainWrapper::save)
        .def("load", &BrainWrapper::load)
        .def("set_config", &BrainWrapper::setConfig)
        .def("get_config", &BrainWrapper::getConfig)
        .def("get_neuron_potentials", &BrainWrapper::getNeuronPotentials)
        .def("get_neuron_activations", &BrainWrapper::getNeuronActivations)
        .def("get_synapse_weights", &BrainWrapper::getSynapseWeights)
        .def("run_experiment", &BrainWrapper::runExperiment)
        .def("generate_activity_plot", &BrainWrapper::generateActivityPlot)
        .def("get_performance_metrics", &BrainWrapper::getPerformanceMetrics)
        .def("set_spike_callback", &BrainWrapper::setSpikeCallback)
        .def("get_version", &BrainWrapper::getVersion)
        .def("get_description", &BrainWrapper::getDescription);
    
    // Register utility functions
    m.def("create_brain", [](const std::string& configFile = "configs/default.cfg") {
        auto config = std::make_shared<Config>();
        config->loadFromFile(configFile);
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return std::make_shared<BrainWrapper>(brain);
        }
        throw BrainInitializationError("Failed to initialize brain");
    }, py::arg("config_file") = "configs/default.cfg",
       "Create and initialize an NLM brain instance");
    
    m.def("get_version", []() {
        return "NLM Python Bindings 1.0.0";
    });
    
    m.def("get_available_commands", []() {
        py::list commands;
        commands.append("connectivity");
        commands.append("plasticity");
        commands.append("stdp");
        commands.append("simulation");
        commands.append("interactive");
        commands.append("benchmark");
        commands.append("save");
        commands.append("load");
        commands.append("reset");
        return commands;
    });
    
    // Register NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Sensory", NeuronType::Sensory)
        .value("Internal", NeuronType::Internal)
        .value("Motor", NeuronType::Motor)
        .export_values();
    
    // Register MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("Wait", MotorCommand::Wait)
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("Interact", MotorCommand::Interact)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .export_values();
}

} // namespace nlm
