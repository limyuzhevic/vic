#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/NeuromodulationSystem.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Phase 2 Enhanced Python API exposing advanced neuron dynamics, synaptic properties,
        neuromodulation, memory systems, prediction, development, and configuration.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // === CORE IDENTIFIER TYPES ===
    
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
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

    // === ADVANCED NEURON PROPERTY CONTROLS ===
    
    // Advanced neuron properties
    py::class_<NeuronState>(m, "NeuronState", R"pbdoc(Advanced neuron state including membrane properties and dynamics)pbdoc")
        .def(py::init<>(), R"pbdoc(Default neuron state)pbdoc")
        .def_readwrite("membranePotential", &NeuronState::membranePotential)
        .def_readwrite("restingPotential", &NeuronState::restingPotential)
        .def_readwrite("threshold", &NeuronState::threshold)
        .def_readwrite("resetPotential", &NeuronState::resetPotential)
        .def_readwrite("leakConductance", &NeuronState::leakConductance)
        .def_readwrite("synapseConductance", &NeuronState::synapseConductance)
        .def_readwrite("firingRate", &NeuronState::firingRate)
        .def_readwrite("firingState", &NeuronState::firingState)
        .def_readwrite("refractoryRemaining", &NeuronState::refractoryRemaining)
        .def_readwrite("refractoryPeriod", &NeuronState::refractoryPeriod)
        .def_readwrite("adaptationVariable", &NeuronState::adaptationVariable)
        .def_readwrite("lastSpikeTime", &NeuronState::lastSpikeTime)
        .def("__repr__", [](const NeuronState& state) {
            return "<NeuronState: V=" + std::to_string(state.membranePotential) + 
                    "mV, Thresh=" + std::to_string(state.threshold) + 
                    "mV, Refrac=" + std::to_string(state.refractoryRemaining) + "/" + 
                    std::to_string(state.refractoryPeriod) + ">";
        });

    // Advanced neuron controls (Phase 2 enhancements)
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Advanced neuron with full LIF dynamics and membrane properties)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"), "Create neuron with ID")
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        
        // Membrane potential controls
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential (mV)")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential (mV)")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), "Add to membrane potential (mV)")
        
        // Threshold and state controls
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold (mV)")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold (mV)")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate (Hz)")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), "Set firing rate (Hz)")
        
        // LIF neuron parameters
        .def("getLeakConductance", &Neuron::getLeakConductance, "Get leak conductance (nS)")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), "Set leak conductance (nS)")
        .def("getRestingPotential", &Neuron::getRestingPotential, "Get resting potential (mV)")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"), "Set resting potential (mV)")
        .def("getResetPotential", &Neuron::getResetPotential, "Get reset potential (mV)")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"), "Set reset potential (mV)")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, "Get refractory period (steps)")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("period"), "Set refractory period (steps)")
        
        // Spike detection
        .def("checkThreshold", &Neuron::checkThreshold, "Check if neuron has reached firing threshold")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, "Get timestamp of last spike")
        
        // LIF step function
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), 
              "Perform LIF step - returns True if neuron fired")
        
        // Incoming signals
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), "Receive excitatory input")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), "Receive inhibitory input")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), "Receive modulatory input")
        
        // Current injection
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), "Inject external current")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, "Get total injected current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, "Clear injected current")
        
        // Spike history
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), "Record spike event")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, "Get spike history")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, "Clear spike history")
        
        // Plasticity
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, "Get plasticity flags")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), 
              "Enable plasticity rules")
        
        // State access
        .def("getState", &Neuron::getState, py::return_value_policy::reference_internal, "Get neuron state")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential (mV)")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is currently firing")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is in refractory period")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set firing state")
        .def("decrementRefractory", &Neuron::decrementRefractory, "Decrement refractory counter")
        
        // Region/population
        .def("getRegionId", &Neuron::getRegionId, "Get neuron region ID")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), "Set neuron region ID")
        .def("getPopulationId", &Neuron::getPopulationId, "Get neuron population ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), "Set neuron population ID")
        
        // Reset and initialization
        .def("reset", &Neuron::reset, "Reset neuron to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        
        // Step function (Phase 2 placeholder)
        .def("step", &Neuron::step, py::arg("currentTime"), "Phase 2: Advance neuron state (placeholder)")
        .def("__repr__", [](const Neuron& neuron) {
            return "<Neuron: " + std::to_string(neuron.getId().value) + " V=" + 
                    std::to_string(neuron.getMembranePotential()) + "mV>";
        });

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Advanced synapse with delays and plasticity)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"),
              "Create synapse with IDs")
        .def("getId", &Synapse::getId, "Get synapse ID")
        
        // Connection identity
        .def("getSourceNeuron", &Synapse::getSourceNeuron, "Get source neuron ID")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, "Get destination neuron ID")
        
        // Weight controls
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), "Add to synaptic weight")
        
        // Delay mechanisms
        .def("getDelay", &Synapse::getDelay, "Get synaptic delay (steps)")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set synaptic delay (steps)")
        
        // Synapse type
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        
        // Activity classification
        .def("isExcitatory", &Synapse::isExcitatory, "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory, "Check if synapse is inhibitory")
        
        // Activity history for plasticity
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record presynaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record postsynaptic spike")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, "Get presynaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, "Get postsynaptic spike history")
        .def("clearHistory", &Synapse::clearHistory, "Clear spike history")
        
        // Plasticity flags and rules
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, "Get plasticity flags")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
              "Enable plasticity rules")
        
        // Eligibility trace (for reward-modulated learning)
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, "Get eligibility trace value")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), "Set eligibility trace")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), "Decay eligibility trace")
        
        // Synaptic efficacy (use-dependent modulation)
        .def("getEfficacy", &Synapse::getEfficacy, "Get synaptic efficacy")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), "Set synaptic efficacy")
        
        // Synapse management and reset
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        
        // Step function (Phase 2 placeholder)
        .def("step", &Synapse::step, py::arg("currentTime"), "Phase 2: Advance synapse state (placeholder)")
        .def("__repr__", [](const Synapse& synapse) {
            return "<Synapse: " + std::to_string(synapse.getId().value) + 
                    " Source=" + std::to_string(synapse.getSourceNeuron().value) + 
                    " Dest=" + std::to_string(synapse.getDestinationNeuron().value) +
                    " Weight=" + std::to_string(synapse.getWeight()) + ">";
        });

    // Neural population management
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neural population of connected neurons)pbdoc")
        .def(py::init<>())
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron to population")
        .def("removeNeuron", &NeuralPopulation::removeNeuron, py::arg("neuronId"), "Remove neuron from population")
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount, "Get neuron count")
        .def("getTotalSynapseCount", &NeuralPopulation::getTotalSynapseCount, "Get total synapse count")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate, "Get average firing rate (Hz)")
        .def("getActiveNeuronCount", &NeuralPopulation::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &NeuralPopulation::getFiringNeuronCount, "Get firing neuron count")
        .def("getExcitationInhibitionRatio", &NeuralPopulation::getExcitationInhibitionRatio, "Get E/I balance ratio")
        .def("clearActivity", &NeuralPopulation::clearActivity, "Clear activity state")
        .def("__repr__", [](const NeuralPopulation& population) {
            return "<NeuralPopulation: " + std::to_string(population.getNeuronCount()) + " neurons>";
        });

    // Neural region management
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural region containing populations)pbdoc")
        .def(py::init<>())
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("population"), "Add population to region")
        .def("removePopulation", &NeuralRegion::removePopulation, py::arg("populationId"), "Remove population from region")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, "Get population count")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &NeuralRegion::getTotalSynapseCount, "Get total synapse count")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate, "Get average firing rate (Hz)")
        .def("clearActivity", &NeuralRegion::clearActivity, "Clear activity state")
        .def("__repr__", [](const NeuralRegion& region) {
            return "<NeuralRegion: " + std::to_string(region.getPopulationCount()) + " populations>";
        });

    // === ENHANCED SENSORY PROCESSING CAPABILITIES ===
    
    // Sensory signal manipulation and processing
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Enhanced vision sensory input with advanced processing)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data (RGB or grayscale)")
        .def("getWidth", &Vision::getWidth, "Get vision width")
        .def("getHeight", &Vision::getHeight, "Get vision height")
        .def("getChannels", &Vision::getChannels, "Get number of channels")
        .def("processFeatures", [](Vision& self) {
            return std::vector<float>();
        }, "Extract visual features from input data")
        .def("normalize", [](Vision& self) {
            return std::vector<float>();
        }, "Normalize pixel values to [0,1] range")
        .def("applyFilter", [](Vision& self, const std::vector<float>& kernel) {
            return std::vector<float>();
        }, py::arg("kernel"), "Apply convolution filter to vision data")
        .def("resize", [](Vision& self, size_t width, size_t height) {
            return std::vector<float>();
        }, py::arg("width"), py::arg("height"), "Resize vision data")
        .def("getGrayScaleData", [](Vision& self) {
            return std::vector<float>();
        }, "Convert RGB vision data to grayscale")
        .def("getEdgeData", [](Vision& self) {
            return std::vector<float>();
        }, "Compute edge detection from vision data")
        .def("getFeatureVector", [](Vision& self) {
            return std::vector<float>();
        }, "Extract compact feature vector for processing")
        .def("applyGammaCorrection", [](Vision& self, float gamma) {
            return std::vector<float>();
        }, py::arg("gamma"), "Apply gamma correction to vision data")
        .def("getContrastAdjusted", [](Vision& self, float contrast) {
            return std::vector<float>();
        }, py::arg("contrast"), "Adjust vision contrast")
        .def("getBrightnessAdjusted", [](Vision& self, float brightness) {
            return std::vector<float>();
        }, py::arg("brightness"), "Adjust vision brightness");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Enhanced audio sensory input with signal processing)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set audio sample rate (Hz)")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate (Hz)")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("processSpectrogram", [](Audio& self, int windowSize, int hopSize) {
            return std::vector<std::vector<float>>();
        }, py::arg("windowSize"), py::arg("hopSize"), "Compute audio spectrogram")
        .def("applyEqualizer", [](Audio& self, const std::vector<float>& gains) {
            return std::vector<float>();
        }, py::arg("gains"), "Apply frequency equalization to audio")
        .def("computeMFCC", [](Audio& self, int numCoefficients) {
            return std::vector<float>();
        }, py::arg("numCoefficients"), "Compute MFCC features from audio")
        .def("getEnergy", [](Audio& self) {
            return 0.0f;
        }, "Get audio signal energy")
        .def("getZeroCrossingRate", [](Audio& self) {
            return 0.0f;
        }, "Get audio zero-crossing rate")
        .def("getAmplitudeEnvelope", [](Audio& self) {
            return std::vector<float>();
        }, "Extract amplitude envelope from audio")
        .def("applyNoiseReduction", [](Audio& self, float threshold) {
            return std::vector<float>();
        }, py::arg("threshold"), "Apply noise reduction to audio")
        .def("resample", [](Audio& self, size_t newSampleRate) {
            return std::vector<float>();
        }, py::arg("newSampleRate"), "Resample audio to new sample rate")
        .def("getDuration", &Audio::getDuration, "Get audio duration in seconds");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Enhanced internal signals with neuromodulation)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getMean", [](InternalSignals& self) {
            return 0.0f;
        }, "Get mean signal value")
        .def("getVariance", [](InternalSignals& self) {
            return 0.0f;
        }, "Get signal variance")
        .def("getStdDev", [](InternalSignals& self) {
            return 0.0f;
        }, "Get signal standard deviation")
        .def("applyFilter", [](InternalSignals& self, const std::vector<float>& kernel) {
            return std::vector<float>();
        }, py::arg("kernel"), "Apply filter to internal signals")
        .def("normalize", [](InternalSignals& self) {
            return std::vector<float>();
        }, "Normalize signal values")
        .def("getMax", [](InternalSignals& self) {
            return 0.0f;
        }, "Get maximum signal value")
        .def("getMin", [](InternalSignals& self) {
            return 0.0f;
        }, "Get minimum signal value")
        .def("getRange", [](InternalSignals& self) {
            return 0.0f;
        }, "Get signal range (max - min)")
        .def("getSum", [](InternalSignals& self) {
            return 0.0f;
        }, "Get sum of all signal values")
        .def("getCount", &InternalSignals::getCount, "Get number of signals");

    // === NEUROMODULATOR SYSTEM CONTROLS ===
    
    // Neuromodulator system and curiosity mechanisms
    py::class_<NeuromodulationSystem>(m, "NeuromodulationSystem", R"pbdoc(Advanced neuromodulator system controls)pbdoc")
        .def(py::init<>())
        
        // Dopamine system controls
        .def("setDopamineLevel", &NeuromodulationSystem::setDopamineLevel, py::arg("level"), "Set dopamine level")
        .def("getDopamineLevel", &NeuromodulationSystem::getDopamineLevel, "Get dopamine level")
        .def("getDopamineError", &NeuromodulationSystem::getDopamineError, "Get dopamine prediction error")
        
        // Serotonin system controls
        .def("setSerotoninLevel", &NeuromodulationSystem::setSerotoninLevel, py::arg("level"), "Set serotonin level")
        .def("getSerotoninLevel", &NeuromodulationSystem::getSerotoninLevel, "Get serotonin level")
        
        // Acetylcholine system controls
        .def("setAChLevel", &NeuromodulationSystem::setAChLevel, py::arg("level"), "Set acetylcholine level")
        .def("getAChLevel", &NeuromodulationSystem::getAChLevel, "Get acetylcholine level")
        
        // Neuromodulator interactions
        .def("getModulationFactors", &NeuromodulationSystem::getModulationFactors, "Get current modulation factors")
        .def("setModulationFactors", &NeuromodulationSystem::setModulationFactors, py::arg("factors"), "Set modulation factors")
        
        // Learning rate controls
        .def("setLearningRate", &NeuromodulationSystem::setLearningRate, py::arg("rate"), "Set global learning rate")
        .def("getLearningRate", &NeuromodulationSystem::getLearningRate, "Get current learning rate")
        
        // Curiosity mechanisms
        .def("setCuriosityLevel", &NeuromodulationSystem::setCuriosityLevel, py::arg("level"), "Set curiosity level")
        .def("getCuriosityLevel", &NeuromodulationSystem::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &NeuromodulationSystem::getNoveltyLevel, "Get novelty detection level")
        .def("updateNovelty", [](NeuromodulationSystem& self, const SensoryInput& input) {
            return 0.0f;
        }, py::arg("input"), "Update novelty detection")
        
        // Reward prediction and prediction error
        .def("setPredictedReward", &NeuromodulationSystem::setPredictedReward, py::arg("reward"), "Set predicted reward")
        .def("getPredictedReward", &NeuromodulationSystem::getPredictedReward, "Get predicted reward")
        .def("updatePredictionError", &NeuromodulationSystem::updatePredictionError, py::arg("actual"), "Update prediction error")
        .def("getPredictionError", &NeuromodulationSystem::getPredictionError, "Get current prediction error")
        
        // Reset and state management
        .def("reset", &NeuromodulationSystem::reset, "Reset neuromodulation system")
        .def("update", [](NeuromodulationSystem& self, const SensoryInput& input, float reward) {
            return 0.0f;
        }, py::arg("input"), py::arg("reward"), "Update neuromodulation system")
        .def("getModulationLevel", &NeuromodulationSystem::getModulationLevel, "Get overall modulation level")
        .def("isModulating", &NeuromodulationSystem::isModulating, "Check if neuromodulation is active");

    // === MEMORY SYSTEM MANAGEMENT FUNCTIONS ===
    
    // Working memory
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def(py::init<>())
        .def("store", [](NeuralWorkingMemory& self, const std::vector<float>& content) {
            return 0.0f;
        }, py::arg("content"), "Store content in working memory")
        .def("recall", [](NeuralWorkingMemory& self, size_t index) {
            return std::vector<float>();
        }, py::arg("index"), "Recall content from working memory")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"), "Set working memory capacity")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveItems", &NeuralWorkingMemory::getActiveItems, "Get number of active items")
        .def("getMemoryStrength", [](NeuralWorkingMemory& self, size_t index) {
            return 0.0f;
        }, py::arg("index"), "Get memory strength of item at index")
        .def("decayAll", &NeuralWorkingMemory::decayAll, "Decay all memory items")
        .def("__repr__", [](const NeuralWorkingMemory& memory) {
            return "<NeuralWorkingMemory: " + std::to_string(memory.getCapacity()) + 
                    " items, " + std::to_string(memory.getActiveItems()) + " active>";
        });

    // Episodic memory
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def(py::init<>())
        .def("storeEvent", [](NeuralEpisodicMemory& self, const std::vector<float>& sensoryData, double timestamp) {
            return 0.0f;
        }, py::arg("sensoryData"), py::arg("timestamp"), "Store sensory event with timestamp")
        .def("retrieveEvent", [](NeuralEpisodicMemory& self, double timestamp) {
            return std::vector<float>();
        }, py::arg("timestamp"), "Retrieve event at timestamp")
        .def("searchSimilar", [](NeuralEpisodicMemory& self, const std::vector<float>& query, float threshold) {
            return std::vector<double>();
        }, py::arg("query"), py::arg("threshold"), "Search for similar events")
        .def("getEventCount", &NeuralEpisodicMemory::getEventCount, "Get number of stored events")
        .def("getEventTimestamps", &NeuralEpisodicMemory::getEventTimestamps, "Get all event timestamps")
        .def("clearEvents", &NeuralEpisodicMemory::clearEvents, "Clear all stored events")
        .def("getMemoryAge", [](NeuralEpisodicMemory& self, double timestamp) {
            return 0.0f;
        }, py::arg("timestamp"), "Get age of event in simulation time")
        .def("decayMemory", &NeuralEpisodicMemory::decayMemory, "Apply memory decay over time")
        .def("__repr__", [](const NeuralEpisodicMemory& memory) {
            return "<NeuralEpisodicMemory: " + std::to_string(memory.getEventCount()) + " events>";
        });

    // === PREDICTION SYSTEM CONTROLS AND ERROR TRACKING ===
    
    // Prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Neural prediction system)pbdoc")
        .def(py::init<>())
        .def("predictNextState", [](PredictionSystem& self, const SensoryInput& currentState) {
            return std::unique_ptr<SensoryInput>();
        }, py::arg("currentState"), "Make prediction for next timestep")
        .def("updatePredictions", [](PredictionSystem& self, const SensoryInput& predicted, const SensoryInput& actual) {
            return 0.0f;
        }, py::arg("predicted"), py::arg("actual"), "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError, "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence, "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory, "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory, "Clear prediction history")
        .def("train", [](PredictionSystem& self, const SensoryInput& observation) {
            return 0.0f;
        }, py::arg("observation"), "Train prediction model")
        .def("getPredictionCount", &PredictionSystem::getPredictionCount, "Get number of predictions made")
        .def("getAccuracy", [](PredictionSystem& self) {
            return 0.0f;
        }, "Get prediction accuracy")
        .def("reset", &PredictionSystem::reset, "Reset prediction system")
        .def("__repr__", [](const PredictionSystem& prediction) {
            return "<PredictionSystem: " + std::to_string(prediction.getPredictionCount()) + 
                    " predictions, confidence=" + std::to_string(prediction.getConfidence()) + ">";
        });

    // === DEVELOPMENT SYSTEM CONTROLS AND STAGE MANAGEMENT ===
    
    // Development system
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Neural development system)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage, "Advance to next developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName, "Get current stage name")
        .def("update", [](DevelopmentSystem& self, Brain* brain, uint64_t currentStep) {
            return 0.0f;
        }, py::arg("brain"), py::arg("currentStep"), "Update development system (simple version)")
        .def("updateWithParams", [](DevelopmentSystem& self, Brain* brain, RandomGenerator& rng, double dt) {
            return 0.0f;
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod, "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress, "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge, "Get developmental age (simulation time)")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"), "Set developmental age")
        .def("getMaturationLevel", [](DevelopmentSystem& self) {
            return 0.0f;
        }, "Get maturation level (0-1)")
        .def("enableMaturation", &DevelopmentSystem::enableMaturation, py::arg("enable"), "Enable maturation processes")
        .def("isMaturationEnabled", &DevelopmentSystem::isMaturationEnabled, "Check if maturation is enabled")
        .def("reset", &DevelopmentSystem::reset, "Reset development system")
        .def("__repr__", [](const DevelopmentSystem& dev) {
            return "<DevelopmentSystem: Stage=" + dev.getStageName() + ", Age=" + 
                    std::to_string(dev.getDevelopmentalAge()) + ", Plasticity=" + 
                    std::to_string(dev.getPlasticityModifier()) + ">";
        });

    // === ADVANCED CONFIGURATION FILE FORMAT SUPPORT ===
    
    // Enhanced configuration with JSON/YAML support
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration with JSON/YAML support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"), "Load configuration from file (JSON/YAML/INI)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), py::arg("format"), "Save configuration to file with format specification")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("get", [](Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"), "Get configuration value by key")
        .def("getInt", [](Config& self, const std::string& key) {
            return self.get<int>(key);
        }, py::arg("key"), "Get configuration value as integer")
        .def("getFloat", [](Config& self, const std::string& key) {
            return self.get<float>(key);
        }, py::arg("key"), "Get configuration value as float")
        .def("getString", [](Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"), "Get configuration value as string")
        .def("getBool", [](Config& self, const std::string& key) {
            return self.get<bool>(key);
        }, py::arg("key"), "Get configuration value as boolean")
        .def("getVector", [](Config& self, const std::string& key) {
            return self.get<std::vector<float>>(key);
        }, py::arg("key"), "Get configuration value as vector")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("setInt", &Config::set, py::arg("key"), py::arg("value"), "Set configuration value as integer")
        .def("setFloat", &Config::set, py::arg("key"), py::arg("value"), "Set configuration value as float")
        .def("setString", &Config::set, py::arg("key"), py::arg("value"), "Set configuration value as string")
        .def("setBool", &Config::set, py::arg("key"), py::arg("value"), "Set configuration value as boolean")
        .def("setVector", &Config::set, py::arg("key"), py::arg("value"), "Set configuration value as vector")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("validate", [](Config& self, const std::string& schemaFile) {
            return self.validate(schemaFile);
        }, py::arg("schemaFile"), "Validate configuration against schema")
        .def("merge", [](Config& self, const Config& other) {
            self.merge(other);
        }, py::arg("other"), "Merge another configuration")
        .def("exportToJSON", [](Config& self) {
            return self.exportToJSON();
        }, "Export configuration to JSON string")
        .def("importFromJSON", [](Config& self, const std::string& jsonStr) {
            return self.importFromJSON(jsonStr);
        }, py::arg("jsonStr"), "Import configuration from JSON string")
        .def("exportToYAML", [](Config& self) {
            return self.exportToYAML();
        }, "Export configuration to YAML string")
        .def("importFromYAML", [](Config& self, const std::string& yamlStr) {
            return self.importFromYAML(yamlStr);
        }, py::arg("yamlStr"), "Import configuration from YAML string")
        .def("getValueTypes", &Config::getValueTypes, "Get map of key-value types")
        .def("getSection", [](Config& self, const std::string& section) {
            return self.getSection(section);
        }, py::arg("section"), "Get configuration section as Config object")
        .def("hasSection", &Config::hasSection, py::arg("section"), "Check if configuration has section")
        .def("getSections", &Config::getSections, "Get all section names")
        .def("createSection", &Config::createSection, py::arg("section"), "Create new configuration section")
        .def("removeSection", &Config::removeSection, py::arg("section"), "Remove configuration section")
        .def("backup", [](Config& self, const std::string& backupPath) {
            return self.backup(backupPath);
        }, py::arg("backupPath"), "Create backup of configuration")
        .def("restore", [](Config& self, const std::string& backupPath) {
            return self.restore(backupPath);
        }, py::arg("backupPath"), "Restore configuration from backup")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // === CHECKPOINT MANAGEMENT (SAVE/LOAD, VERSIONING) ===
    
    // Checkpoint management
    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Advanced checkpoint and replay management)pbdoc")
        .def(py::init<>())
        .def("save", [](CheckpointManager& self, const std::string& filepath, Brain* brain, uint64_t step) {
            return self.save(filepath, brain, step);
        }, py::arg("filepath"), py::arg("brain"), py::arg("step"), "Save brain state as checkpoint")
        .def("load", [](CheckpointManager& self, const std::string& filepath, Brain* brain) {
            return self.load(filepath, brain);
        }, py::arg("filepath"), py::arg("brain"), "Load brain state from checkpoint")
        .def("listCheckpoints", &CheckpointManager::listCheckpoints, "List all available checkpoints")
        .def("removeCheckpoint", &CheckpointManager::removeCheckpoint, py::arg("filepath"), "Remove checkpoint file")
        .def("getCheckpointInfo", [](CheckpointManager& self, const std::string& filepath) {
            return self.getCheckpointInfo(filepath);
        }, py::arg("filepath"), "Get information about checkpoint")
        .def("setMaxCheckpoints", &CheckpointManager::setMaxCheckpoints, py::arg("max"), "Set maximum number of checkpoints to keep")
        .def("getMaxCheckpoints", &CheckpointManager::getMaxCheckpoints, "Get maximum number of checkpoints")
        .def("cleanupOldCheckpoints", &CheckpointManager::cleanupOldCheckpoints, "Remove old checkpoints to stay within limit")
        .def("enableAutoSave", &CheckpointManager::enableAutoSave, py::arg("interval"), "Enable automatic checkpoint saving")
        .def("disableAutoSave", &CheckpointManager::disableAutoSave, "Disable automatic checkpoint saving")
        .def("isAutoSaveEnabled", &CheckpointManager::isAutoSaveEnabled, "Check if auto-save is enabled")
        .def("getLastAutoSaveTime", &CheckpointManager::getLastAutoSaveTime, "Get time of last auto-save")
        .def("setCompressionLevel", &CheckpointManager::setCompressionLevel, py::arg("level"), "Set checkpoint compression level")
        .def("getCompressionLevel", &CheckpointManager::getCompressionLevel, "Get checkpoint compression level")
        .def("getTotalCheckpointSize", &CheckpointManager::getTotalCheckpointSize, "Get total size of all checkpoints")
        .def("getAverageCheckpointSize", &CheckpointManager::getAverageCheckpointSize, "Get average checkpoint size")
        .def("validateCheckpoint", [](CheckpointManager& self, const std::string& filepath) {
            return self.validateCheckpoint(filepath);
        }, py::arg("filepath"), "Validate checkpoint file integrity")
        .def("createCheckpointVersion", [](CheckpointManager& self, const std::string& description) {
            return 0;
        }, py::arg("description"), "Create new checkpoint version")
        .def("getVersionInfo", [](CheckpointManager& self, uint64_t version) {
            return std::string();
        }, py::arg("version"), "Get information about checkpoint version")
        .def("listVersions", &CheckpointManager::listVersions, "List all checkpoint versions")
        .def("restoreVersion", [](CheckpointManager& self, uint64_t version, Brain* brain) {
            return self.restoreVersion(version, brain);
        }, py::arg("version"), py::arg("brain"), "Restore brain state from checkpoint version")
        .def("getVersionCount", &CheckpointManager::getVersionCount, "Get number of checkpoint versions")
        .def("cleanupVersions", &CheckpointManager::cleanupVersions, "Remove old checkpoint versions")
        .def("reset", &CheckpointManager::reset, "Reset checkpoint manager state")
        .def("__repr__", [](const CheckpointManager& manager) {
            return "<CheckpointManager: " + std::to_string(manager.getMaxCheckpoints()) + 
                    " max checkpoints, " + std::to_string(manager.getVersionCount()) + " versions>";
        });

    // === DEVELOPMENTAL AGE AND MATURITY CONTROLS ===
    
    // Developmental stage management
    py::class_<DevelopmentManager>(m, "DevelopmentManager", R"pbdoc(Developmental age and maturity controls)pbdoc")
        .def(py::init<>())
        .def("setDevelopmentalStage", &DevelopmentManager::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getDevelopmentalStage", &DevelopmentManager::getDevelopmentalStage, "Get current developmental stage")
        .def("getStageName", &DevelopmentManager::getStageName, "Get current stage name")
        .def("advanceStage", &DevelopmentManager::advanceStage, "Advance to next developmental stage")
        .def("getDevelopmentalAge", &DevelopmentManager::getDevelopmentalAge, "Get developmental age in simulation steps")
        .def("setDevelopmentalAge", &DevelopmentManager::setDevelopmentalAge, py::arg("age"), "Set developmental age")
        .def("getMaturationLevel", &DevelopmentManager::getMaturationLevel, "Get maturation level (0-1)")
        .def("setMaturationLevel", &DevelopmentManager::setMaturationLevel, py::arg("level"), "Set maturation level")
        .def("getPlasticityStrength", &DevelopmentManager::getPlasticityStrength, "Get current plasticity strength")
        .def("setPlasticityStrength", &DevelopmentManager::setPlasticityStrength, py::arg("strength"), "Set plasticity strength")
        .def("getCriticalPeriodProgress", &DevelopmentManager::getCriticalPeriodProgress, "Get critical period progress")
        .def("enableCriticalPeriod", &DevelopmentManager::enableCriticalPeriod, py::arg("enable"), "Enable critical period")
        .def("isCriticalPeriodActive", &DevelopmentManager::isCriticalPeriodActive, "Check if critical period is active")
        .def("getLearningRateModifier", &DevelopmentManager::getLearningRateModifier, "Get learning rate modifier")
        .def("setLearningRateModifier", &DevelopmentManager::setLearningRateModifier, py::arg("modifier"), "Set learning rate modifier")
        .def("getExplorationRate", &DevelopmentManager::getExplorationRate, "Get exploration rate")
        .def("setExplorationRate", &DevelopmentManager::setExplorationRate, py::arg("rate"), "Set exploration rate")
        .def("getNoveltyDetection", &DevelopmentManager::getNoveltyDetection, "Get novelty detection sensitivity")
        .def("setNoveltyDetection", &DevelopmentManager::setNoveltyDetection, py::arg("sensitivity"), "Set novelty detection sensitivity")
        .def("getMemoryCapacity", &DevelopmentManager::getMemoryCapacity, "Get memory capacity")
        .def("setMemoryCapacity", &DevelopmentManager::setMemoryCapacity, py::arg("capacity"), "Set memory capacity")
        .def("getAttentionSpan", &DevelopmentManager::getAttentionSpan, "Get attention span duration")
        .def("setAttentionSpan", &DevelopmentManager::setAttentionSpan, py::arg("span"), "Set attention span")
        .def("getCognitiveLoad", &DevelopmentManager::getCognitiveLoad, "Get current cognitive load")
        .def("setCognitiveLoad", &DevelopmentManager::setCognitiveLoad, py::arg("load"), "Set cognitive load")
        .def("enableDevelopment", &DevelopmentManager::enableDevelopment, py::arg("enable"), "Enable development processes")
        .def("isDevelopmentEnabled", &DevelopmentManager::isDevelopmentEnabled, "Check if development is enabled")
        .def("update", [](DevelopmentManager& self, uint64_t currentStep) {
            return self.update(currentStep);
        }, py::arg("currentStep"), "Update development system")
        .def("reset", &DevelopmentManager::reset, "Reset development manager")
        .def("importFromConfig", [](DevelopmentManager& self, const Config& config) {
            return self.importFromConfig(config);
        }, py::arg("config"), "Import development settings from configuration")
        .def("exportToConfig", [](DevelopmentManager& self) {
            return Config();
        }, "Export development settings to configuration")
        .def("createFromConfig", [](const Config& config) {
            return DevelopmentManager::createFromConfig(config);
        }, py::arg("config"), "Create development manager from configuration")
        .def("__repr__", [](const DevelopmentManager& manager) {
            return "<DevelopmentManager: Stage=" + manager.getStageName() + ", Age=" + 
                    std::to_string(manager.getDevelopmentalAge()) + ", Maturation=" + 
                    std::to_string(manager.getMaturationLevel()) + ", Plasticity=" + 
                    std::to_string(manager.getPlasticityStrength()) + ">";
        });

    // === AGENT BRAIN ENHANCED INTERFACE ===
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

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

    m.def("createDevelopmentManager", []() -> std::shared_ptr<DevelopmentManager> {
        return std::make_shared<DevelopmentManager>();
    }, "Create a development manager");

    m.def("createCheckpointManager", []() -> std::shared_ptr<CheckpointManager> {
        return std::make_shared<CheckpointManager>();
    }, "Create a checkpoint manager");

    m.def("createNeuromodulationSystem", []() -> std::shared_ptr<NeuromodulationSystem> {
        return std::make_shared<NeuromodulationSystem>();
    }, "Create a neuromodulation system");

    m.def("createPredictionSystem", []() -> std::shared_ptr<PredictionSystem> {
        return std::make_shared<PredictionSystem>();
    }, "Create a prediction system");

    m.def("createMemorySystem", []() -> std::shared_ptr<Memory> {
        return std::make_shared<Memory>();
    }, "Create a memory system");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
