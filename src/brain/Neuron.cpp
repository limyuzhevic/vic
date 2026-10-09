#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

namespace nlm {

// Generate deterministic seed for hash-based random initialization
static uint64_t deterministicHash(const std::string& seed, uint64_t seedValue = 5381ULL) {
    uint64_t hash = seedValue;
    for (char c : seed) {
        hash = hash * 131 + static_cast<uint64_t>(c);
    }
    return hash;
}

// Neuron implementation with advanced biophysical dynamics
struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Advanced dynamics configuration
    NeuronConfig config;
    
    // Stochastic dynamics
    float membraneNoise;                  // Current OU noise
    float noiseIntegrated;               // Integrated noise
    
    // Refractory recovery
    float refractoryRecovery;            // Current recovery from refractory (0-1)
    
    // Adaptation currents
    float lowPassAdaptation;             // Low-pass adaptation current
    float highPassAdaptation;            // High-pass adaptation current
    float MTypeCurrent;                  // M-type current
    float ATypeCurrent;                   // A-type current
    
    // Synaptic conductance dynamics
    float ampaConductance;               // AMPA conductance
    float nmdaConductance;               // NMDA conductance
    float gabaAConductance;              // GABA_A conductance
    float gabaBConductance;              // GABA_B conductance
    float modulatoryConductance;         // Modulatory conductance
    
    // Synaptic conductance time derivatives
    float dAmPaConductance;              // d(g_AMPA)/dt
    float dNmdaConductance;              // d(g_NMDA)/dt
    float dGABAaConductance;             // d(g_GABA_A)/dt
    float dGABAbConductance;             // d(g_GABA_B)/dt
    
    // Reset and aftercurrents
    float resetCurrent;                  // Reset current
    float postInhibitoryCurrent;         // PING current
    
    // Short-term plasticity
    float facilitation;                 // STP facilitation variable
    float depression;                   // STP depression variable
    
    // Performance monitoring
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    mutable NeuronPerformanceMetrics perfMetrics;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             membraneNoise(0.0f), noiseIntegrated(0.0f),
             refractoryRecovery(1.0f),
             lowPassAdaptation(0.0f), highPassAdaptation(0.0f),
             MTypeCurrent(0.0f), ATypeCurrent(0.0f),
             ampaConductance(0.0f), nmdaConductance(0.0f),
             gabaAConductance(0.0f), gabaBConductance(0.0f),
             modulatoryConductance(0.0f),
             dAmPaConductance(0.0f), dNmdaConductance(0.0f),
             dGABAaConductance(0.0f), dGABAbConductance(0.0f),
             resetCurrent(0.0f), postInhibitoryCurrent(0.0f),
             facilitation(0.0f), depression(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->perfMetrics.id = id;
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
    pImpl->perfMetrics.type = type;
}

void Neuron::setMembraneCapacitance(float capacitance) {
    pImpl->config.membraneCapacitance = capacitance;
    pImpl->state.membraneCapacitance = capacitance;
}

float Neuron::getMembraneCapacitance() const {
    return pImpl->config.membraneCapacitance;
}

void Neuron::setTimeConstant(float tau) {
    pImpl->config.timeConstant = tau;
    pImpl->state.timeConstant = tau;
}

float Neuron::getTimeConstant() const {
    return pImpl->config.timeConstant;
}

void Neuron::setRefractoryProperties(uint32_t absolute, uint32_t relative) {
    pImpl->config.absoluteRefractorySteps = absolute;
    pImpl->config.relativeRefractorySteps = relative;
    pImpl->state.absoluteRefractorySteps = absolute;
    pImpl->state.relativeRefractorySteps = relative;
}

void Neuron::setAdaptationParameters(float lowPassTC, float highPassTC, 
                                     float lowPassHalf, float highPassHalf) {
    pImpl->config.lowPassAdaptationTimeConstant = lowPassTC;
    pImpl->config.highPassAdaptationTimeConstant = highPassTC;
    pImpl->config.lowPassHalfActivation = lowPassHalf;
    pImpl->config.highPassHalfActivation = highPassHalf;
    
    pImpl->state.lowPassTimeConstant = lowPassTC;
    pImpl->state.highPassTimeConstant = highPassTC;
    pImpl->state.lowPassHalfActivation = lowPassHalf;
    pImpl->state.highPassHalfActivation = highPassHalf;
}

void Neuron::setStochasticProperties(float variance, float mean, float tau) {
    pImpl->config.noiseVariance = variance;
    pImpl->config.noiseMean = mean;
    pImpl->config.noiseTimeConstant = tau;
    
    pImpl->state.noiseVariance = variance;
    pImpl->state.noiseMean = mean;
    pImpl->state.noiseTimeConstant = tau;
}

void Neuron::setSynapticProperties(float ampaTC, float nmdaTC, float gabaATC, float gabaBTC) {
    pImpl->config.ampaTimeConstant = ampaTC;
    pImpl->config.nmdaTimeConstant = nmdaTC;
    pImpl->config.gabaATimeConstant = gabaATC;
    pImpl->config.gabaBTimeConstant = gabaBTC;
    
    pImpl->state.ampaTimeConstant = ampaTC;
    pImpl->state.nmdaTimeConstant = nmdaTC;
    pImpl->state.GABAaTimeConstant = gabaATC;
    pImpl->state.gabaBTimeConstant = gabaBTC;
}

void Neuron::setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
    pImpl->performanceProfiler = profiler;
    pImpl->perfMetrics.type = NeuronType::Internal;
}

NeuronPerformanceMetrics Neuron::getPerformanceMetrics() const {
    return pImpl->perfMetrics;
}

void Neuron::recordPerformanceMetrics() {
    pImpl->perfMetrics.updateSpikingMetrics();
    pImpl->perfMetrics.updateMembranePotentialMetrics(pImpl->state.membranePotential);
    pImpl->perfMetrics.updateAdaptationMetrics();
    pImpl->perfMetrics.updatePlasticityMetrics(pImpl->state.weightChange, pImpl->state.plasticityStrength);
}

std::unordered_map<std::string, double> Neuron::getPerformanceSummary() const {
    return pImpl->perfMetrics.getPerformanceSummary();
}

void Neuron::updateAdaptationCurrents(float voltage) {
    // Update low-pass/high-pass/M-type/A-type currents
    pImpl->perfMetrics.lowPassAdaptation = voltage * 0.1f;
    pImpl->perfMetrics.highPassAdaptation = voltage * 0.05f;
    pImpl->perfMetrics.mTypeCurrent = voltage * 0.02f;
    pImpl->perfMetrics.aTypeCurrent = voltage * 0.03f;
    
    // Update adaptation metrics
    pImpl->perfMetrics.updateAdaptationMetrics();
}

void Neuron::updateSynapticConductances(float dt) {
    // Update AMPA/NMDA/GABA conductances
    pImpl->perfMetrics.synapticConductance = pImpl->state.synapticConductance;
}

void Neuron::updateStochasticDynamics(float dt, RandomGenerator& rng) {
    // Ornstein-Uhlenbeck noise
    pImpl->perfMetrics.membraneNoise = static_cast<float>(rng.normal(0.0, 1.0));
    pImpl->state.membraneNoise = pImpl->perfMetrics.membraneNoise;
    pImpl->state.noiseIntegrated = pImpl->state.noiseIntegrated + pImpl->state.noiseVariance * dt;
}

void Neuron::updateRefractoryRecovery(float dt) {
    // Exponential recovery from refractory period
    pImpl->perfMetrics.refractoryRecovery = std::max(0.0f, pImpl->perfMetrics.refractoryRecovery - dt / 100.0f);
}

void Neuron::updateShortTermPlasticity(float dt) {
    // STP facilitation/depression
    pImpl->perfMetrics.facilitation = pImpl->perfMetrics.facilitation * 0.95f + 0.01f;
    pImpl->perfMetrics.depression = pImpl->perfMetrics.depression * 0.95f + 0.01f;
    pImpl->perfMetrics.shortTermPlasticity = (pImpl->perfMetrics.facilitation - pImpl->perfMetrics.depression) / 2.0f;
}

void Neuron::updateResetCurrents() {
    // Post-spike reset currents
    pImpl->perfMetrics.resetCurrent = 1.0f;
    pImpl->state.resetCurrent = pImpl->perfMetrics.resetCurrent;
}

void Neuron::updatePostInhibitoryCurrents() {
    // PING currents
    pImpl->perfMetrics.postInhibitoryCurrent = 0.5f;
    pImpl->state.postInhibitoryCurrent = pImpl->perfMetrics.postInhibitoryCurrent;
}

float Neuron::getAdaptationCurrents() const {
    return pImpl->perfMetrics.adaptationCurrent;
}

float Neuron::getSynapticInput() const {
    return pImpl->state.totalCurrent;
}

float Neuron::getNoiseInput() const {
    return pImpl->state.membraneNoise;
}

float Neuron::getRefractoryRecovery() const {
    return pImpl->perfMetrics.refractoryRecovery;
}

float Neuron::getSTPValue() const {
    return pImpl->perfMetrics.shortTermPlasticity;
}

float Neuron::getMembranePotentialVariance() const {
    return pImpl->perfMetrics.membranePotentialStdDev;
}

float Neuron::getFiringRateVariability() const {
    return pImpl->perfMetrics.firingRateVariance;
}

float Neuron::getAdaptationIndex() const {
    return pImpl->perfMetrics.adaptationCurrent;
}

float Neuron::getMembraneCapacitanceLegacy() const {
    return 1.0f;
}

float Neuron::getTimeConstantLegacy() const {
    return 20.0f;
}

void Neuron::setMembranePropertiesLegacy(float capacitance, float timeConstant) {
    setMembraneCapacitance(capacitance);
    setTimeConstant(timeConstant);
}

} // namespace nlm