// Enhanced Python Bindings Implementation
// 
// This file implements the enhanced Python bindings with:
// 1. Pythonic APIs for all NLM components
// 2. Comprehensive error handling and validation
// 3. Factory methods and advanced features
// 4. Better documentation and examples
// 5. Custom exception types
// 6. Enhanced configuration management
// 7. Built-in analysis and monitoring tools

#include "EnhancedBindings.hpp"
#include "../src/core/Random/Random.hpp"
#include <cmath>
#include <sstream>

namespace nlm {

// EnhancedNeuron Implementation
struct EnhancedNeuron::Impl {
    NeuronId id;
    NeuronType type;
    std::shared_ptr<Neuron> neuron;
    std::shared_ptr<Brain> brain;
    
    Impl(NeuronId neuronId, std::shared_ptr<Brain> brainPtr) 
        : id(neuronId), type(NeuronType::Internal), brain(brainPtr) {
        neuron = std::make_shared<Neuron>(id);
    }
};

EnhancedNeuron::EnhancedNeuron(NeuronId id, std::shared_ptr<Brain> brain)
    : pImpl(std::make_unique<Impl>(id, brain)) {
}

EnhancedNeuron::~EnhancedNeuron() = default;

EnhancedNeuron::EnhancedNeuron(EnhancedNeuron&& other) noexcept
    : pImpl(std::move(other.pImpl)), brain_(std::move(other.brain_)) {
}

EnhancedNeuron& EnhancedNeuron::operator=(EnhancedNeuron&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = std::move(other.brain_);
    }
    return *this;
}

NeuronId EnhancedNeuron::getId() const {
    return pImpl->id;
}

NeuronType EnhancedNeuron::getType() const {
    return pImpl->neuron->getType();
}

void EnhancedNeuron::setType(NeuronType type) {
    pImpl->neuron->setType(type);
}

double EnhancedNeuron::getMembranePotential() const {
    return pImpl->neuron->getMembranePotential();
}

void EnhancedNeuron::setMembranePotential(double potential) {
    pImpl->neuron->setMembranePotential(static_cast<MembranePotential>(potential));
}

double EnhancedNeuron::getThreshold() const {
    return pImpl->neuron->getThreshold();
}

void EnhancedNeuron::setThreshold(double threshold) {
    pImpl->neuron->setThreshold(static_cast<MembranePotential>(threshold));
}

double EnhancedNeuron::getRestingPotential() const {
    return pImpl->neuron->getRestingPotential();
}

double EnhancedNeuron::getFiringRate() const {
    return pImpl->neuron->getFiringRate();
}

bool EnhancedNeuron::isActive() const {
    return pImpl->neuron->isFiring();
}

bool EnhancedNeuron::isRefractory() const {
    return pImpl->neuron->isRefractory();
}

bool EnhancedNeuron::stepLIF(double currentTime, double dt) {
    return pImpl->neuron->stepLIF(static_cast<Timestamp>(currentTime), static_cast<TimestepDuration>(dt));
}

void EnhancedNeuron::injectCurrent(double current) {
    pImpl->neuron->injectCurrent(static_cast<MembranePotential>(current));
}

void EnhancedNeuron::receiveExcitatoryInput(double amplitude) {
    pImpl->neuron->receiveExcitatoryInput(static_cast<MembranePotential>(amplitude));
}

void EnhancedNeuron::receiveInhibitoryInput(double amplitude) {
    pImpl->neuron->receiveInhibitoryInput(static_cast<MembranePotential>(amplitude));
}

double EnhancedNeuron::getExcitabilityScore() const {
    double v = getMembranePotential();
    double thresh = getThreshold();
    return std::max(0.0, (v - thresh) / (thresh - (-80.0)));
}

std::string EnhancedNeuron::getStateDescription() const {
    std::stringstream ss;
    ss << "Enhanced Neuron " << getId() << " (Type: " << static_cast<int>(getType()) << ")\n";
    ss << "  Membrane Potential: " << getMembranePotential() << " mV\n";
    ss << "  Threshold: " << getThreshold() << " mV\n";
    ss << "  Resting Potential: " << getRestingPotential() << " mV\n";
    ss << "  Firing Rate: " << getFiringRate() << " Hz\n";
    ss << "  Active: " << (isActive() ? "Yes" : "No") << "\n";
    ss << "  Refractory: " << (isRefractory() ? "Yes" : "No") << "\n";
    ss << "  Excitability Score: " << getExcitabilityScore();
    return ss.str();
}

void EnhancedNeuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->neuron->addIncomingSynapse(handle);
}

void EnhancedNeuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->neuron->addOutgoingSynapse(handle);
}

std::shared_ptr<Neuron> EnhancedNeuron::getInternalNeuron() {
    return pImpl->neuron;
}

std::shared_ptr<EnhancedNeuron> EnhancedNeuron::createRandom(int id, double seed) {
    auto randomGen = std::make_shared<RandomGenerator>(static_cast<uint64_t>(seed));
    auto neuron = std::make_shared<EnhancedNeuron>(NeuronId(static_cast<uint64_t>(id)));
    neuron->pImpl->neuron->initializeRandom(*randomGen);
    return neuron;
}

// EnhancedSynapse Implementation
struct EnhancedSynapse::Impl {
    SynapseId id;
    std::shared_ptr<Synapse> synapse;
    std::shared_ptr<Brain> brain;
    
    Impl(SynapseId synapseId, std::shared_ptr<Brain> brainPtr) 
        : id(synapseId), brain(brainPtr) {
        synapse = std::make_shared<Synapse>(id);
    }
};

EnhancedSynapse::EnhancedSynapse(SynapseId id, std::shared_ptr<Brain> brain)
    : pImpl(std::make_unique<Impl>(id, brain)) {
}

EnhancedSynapse::~EnhancedSynapse() = default;

EnhancedSynapse::EnhancedSynapse(EnhancedSynapse&& other) noexcept
    : pImpl(std::move(other.pImpl)), brain_(std::move(other.brain_)) {
}

EnhancedSynapse& EnhancedSynapse::operator=(EnhancedSynapse&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = std::move(other.brain_);
    }
    return *this;
}

SynapseId EnhancedSynapse::getId() const {
    return pImpl->id;
}

double EnhancedSynapse::getWeight() const {
    return pImpl->synapse->getWeight();
}

void EnhancedSynapse::setWeight(double weight) {
    pImpl->synapse->setWeight(static_cast<float>(weight));
}

bool EnhancedSynapse::isEnabled() const {
    return pImpl->synapse->isEnabled();
}

bool EnhancedSynapse::isPlastic() const {
    return pImpl->synapse->isPlastic();
}

void EnhancedSynapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->synapse->enablePlasticity(hebbian, stdp, rewardModulated);
}

void EnhancedSynapse::setSTDPParameters(double ltpWeight, double ltdWeight, double tau) {
    pImpl->synapse->setSTDPParameters(
        static_cast<float>(ltpWeight),
        static_cast<float>(ltdWeight),
        static_cast<float>(tau)
    );
}

double EnhancedSynapse::getStrengthScore() const {
    double w = getWeight();
    return std::max(0.0, (w - (-1.0)) / 2.0);
}

std::string EnhancedSynapse::getStateDescription() const {
    std::stringstream ss;
    ss << "Enhanced Synapse " << getId() << "\n";
    ss << "  Weight: " << getWeight() << "\n";
    ss << "  Enabled: " << (isEnabled() ? "Yes" : "No") << "\n";
    ss << "  Plastic: " << (isPlastic() ? "Yes" : "No") << "\n";
    ss << "  Strength Score: " << getStrengthScore();
    return ss.str();
}

std::shared_ptr<Synapse> EnhancedSynapse::getInternalSynapse() {
    return pImpl->synapse;
}

// EnhancedBrain Implementation
struct EnhancedBrain::Impl {
    std::shared_ptr<Brain> brain;
    SimulationStep currentStep;
    
    Impl(std::shared_ptr<Config> config) {
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        currentStep = 0;
    }
};

EnhancedBrain::EnhancedBrain(std::shared_ptr<Config> config)
    : pImpl(std::make_unique<Impl>(config)) {
}

EnhancedBrain::~EnhancedBrain() = default;

EnhancedBrain::EnhancedBrain(EnhancedBrain&& other) noexcept
    : pImpl(std::move(other.pImpl)) {
}

EnhancedBrain& EnhancedBrain::operator=(EnhancedBrain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void EnhancedBrain::configure(size_t neuronCount, size_t regionCount, float connectionProb) {
    auto config = pImpl->brain->getConfig();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Programmatic);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Programmatic);
    config->set("connection_probability", connectionProb, ConfigSource::Programmatic);
}

void EnhancedBrain::step() {
    pImpl->brain->step(pImpl->currentStep, pImpl->currentStep * 0.001);
    pImpl->currentStep++;
}

void EnhancedBrain::step(double timestep) {
    pImpl->brain->step(pImpl->currentStep, pImpl->currentStep * timestep);
    pImpl->currentStep++;
}

void EnhancedBrain::step(SimulationStep step, double time) {
    pImpl->brain->step(step, time);
}

void EnhancedBrain::runForSteps(size_t steps, double timestep) {
    for (size_t i = 0; i < steps; ++i) {
        step(timestep);
    }
}

void EnhancedBrain::runUntilSpikeCount(size_t targetSpikes, double timestep) {
    while (getTotalSpikes() < targetSpikes) {
        step(timestep);
    }
}

void EnhancedBrain::injectCurrentToAll(NeuronType type, double current) {
    pImpl->brain->injectCurrentToNeurons(type, static_cast<MembranePotential>(current));
}

void EnhancedBrain::injectToNeuron(NeuronId id, double current) {
    pImpl->brain->injectCurrent(id, static_cast<MembranePotential>(current));
}

size_t EnhancedBrain::getTotalNeurons() const {
    return pImpl->brain->getTotalNeuronCount();
}

size_t EnhancedBrain::getTotalSynapses() const {
    return pImpl->brain->getTotalSynapseCount();
}

size_t EnhancedBrain::getActiveNeurons() const {
    return pImpl->brain->getActiveNeuronCount();
}

size_t EnhancedBrain::getFiringNeurons() const {
    return pImpl->brain->getFiringNeuronCount();
}

double EnhancedBrain::getAverageFiringRate() const {
    return pImpl->brain->getAverageFiringRate();
}

double EnhancedBrain::getExcitationInhibitionRatio() const {
    return pImpl->brain->getExcitationInhibitionRatio();
}

size_t EnhancedBrain::getTotalSpikes() const {
    return pImpl->brain->getTotalSpikeCount();
}

void EnhancedBrain::advanceDevelopment(double timestep) {
    pImpl->brain->updateDevelopment(timestep);
}

void EnhancedBrain::applyReward(double reward, double predicted) {
    pImpl->brain->applyRewardModulation(reward, predicted);
}

void EnhancedBrain::addRegion(const std::string& name) {
    pImpl->brain->addRegion(name);
}

std::shared_ptr<Neuron> EnhancedBrain::getNeuronAtRegion(RegionId regionId, size_t index) {
    auto region = pImpl->brain->getRegion(regionId);
    if (region) {
        auto neurons = region->getAllNeurons();
        if (index < neurons.size()) {
            return neurons[index];
        }
    }
    throw NeuronError("Neuron not found at region " + std::to_string(regionId.value) + ", index " + std::to_string(index));
}

std::vector<std::shared_ptr<Neuron>> EnhancedBrain::getAllNeurons() {
    std::vector<std::shared_ptr<Neuron>> allNeurons;
    auto regions = pImpl->brain->getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        allNeurons.insert(allNeurons.end(), neurons.begin(), neurons.end());
    }
    return allNeurons;
}

std::string EnhancedBrain::getSystemStatus() const {
    std::stringstream ss;
    ss << "=== Enhanced NLM Brain Status ===\n";
    ss << "Total Neurons: " << getTotalNeurons() << "\n";
    ss << "Total Synapses: " << getTotalSynapses() << "\n";
    ss << "Active Neurons: " << getActiveNeurons() << "\n";
    ss << "Firing Neurons: " << getFiringNeurons() << "\n";
    ss << "Total Spikes: " << getTotalSpikes() << "\n";
    ss << "Average Firing Rate: " << getAverageFiringRate() << " Hz\n";
    ss << "Excitatory/Inhibitory Ratio: " << getExcitationInhibitionRatio();
    return ss.str();
}

void EnhancedBrain::saveState(const std::string& filename) {
    pImpl->brain->save(filename);
}

void EnhancedBrain::loadState(const std::string& filename) {
    pImpl->brain->load(filename);
}

std::shared_ptr<Brain> EnhancedBrain::getInternalBrain() {
    return pImpl->brain;
}

// EnhancedConfig Implementation
struct EnhancedConfig::Impl {
    std::shared_ptr<Config> config;
    
    Impl() {
        config = std::make_shared<Config>();
    }
};

EnhancedConfig::EnhancedConfig()
    : pImpl(std::make_unique<Impl>()) {
}

EnhancedConfig::~EnhancedConfig() = default;

EnhancedConfig::EnhancedConfig(EnhancedConfig&& other) noexcept
    : pImpl(std::move(other.pImpl)) {
}

EnhancedConfig& EnhancedConfig::operator=(EnhancedConfig&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void EnhancedConfig::set(const std::string& key, int value) {
    pImpl->config->set(key, static_cast<int64_t>(value), ConfigSource::Programmatic);
}

void EnhancedConfig::set(const std::string& key, double value) {
    pImpl->config->set(key, value, ConfigSource::Programmatic);
}

void EnhancedConfig::set(const std::string& key, const std::string& value) {
    pImpl->config->set(key, value, ConfigSource::Programmatic);
}

bool EnhancedConfig::has(const std::string& key) const {
    return pImpl->config->has(key);
}

int EnhancedConfig::getInt(const std::string& key, int defaultValue) const {
    return pImpl->config->getOr<int64_t>(key, static_cast<int64_t>(defaultValue));
}

double EnhancedConfig::getDouble(const std::string& key, double defaultValue) const {
    return pImpl->config->getOr<double>(key, defaultValue);
}

std::string EnhancedConfig::getString(const std::string& key, const std::string& defaultValue) const {
    return pImpl->config->getOr<std::string>(key, defaultValue);
}

bool EnhancedConfig::validate() const {
    return !pImpl->config->getKeys().empty();
}

std::string EnhancedConfig::getValidationError() const {
    if (validate()) {
        return "";
    }
    return "Configuration has no keys";
}

void EnhancedConfig::clear() {
    pImpl->config->clear();
}

std::vector<std::string> EnhancedConfig::getKeys() const {
    return pImpl->config->getKeys();
}

std::string EnhancedConfig::getSummary() const {
    return pImpl->config->summary();
}

bool EnhancedConfig::loadFromFile(const std::string& filename) {
    return pImpl->config->loadFromFile(filename);
}

bool EnhancedConfig::saveToFile(const std::string& filename) const {
    return pImpl->config->saveToFile(filename);
}

std::shared_ptr<Config> EnhancedConfig::getInternalConfig() {
    return pImpl->config;
}

std::shared_ptr<EnhancedConfig> EnhancedConfig::createDefault() {
    return std::make_shared<EnhancedConfig>();
}

} // namespace nlm