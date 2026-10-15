// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation (Improved Version)
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// IMPROVEMENTS:
// - Configurable test parameters via command line
// - Better error handling and recovery
// - Enhanced logging and reporting
// - Modular test structure
// - Performance monitoring

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <algorithm>
#include <map>
#include <limits>

using namespace nlm;

// Configuration for experimental parameters
struct ExperimentConfig {
    // Test parameters
    struct ConnectivityTest {
        size_t numNeuronsToStimulate = 10;
        SimulationStep numSteps = 50;
        MembranePotential stimulationCurrent = 50.0f;
        bool verbose = true;
    } connectivity;
    
    struct PlasticityTest {
        SimulationStep numSteps = 1000;
        MembranePotential stimulationCurrent = 30.0f;
        size_t numInputNeurons = 20;
        bool enableHebbian = true;
        bool enableStdp = true;
        bool enableStructural = false;
        bool verbose = true;
    } plasticity;
    
    struct StdPTest {
        size_t numSynapses = 5;
        int numTrials = 50;
        MembranePotential prePostCurrent = 60.0f;
        bool verbose = true;
    } stdp;
    
    // Learning experiment parameters
    struct LearningExperiment {
        size_t regionIndex = 1;
        float weightChangeThreshold = 0.01f;
        bool computeAllStats = true;
    } learning;
    
    // Global parameters
    SimulationStep timestep = 0.001;
    uint64_t randomSeed = 42;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.15f;
    
    // Plasticity parameters
    float stdpLtpWeight = 0.02f;
    float stdpLtdWeight = 0.015f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
};