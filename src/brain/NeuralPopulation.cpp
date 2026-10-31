#include "NeuralPopulation.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<Neuron*> neurons;
    std::vector<std::unique_ptr<Neuron>> ownedNeurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {}
};

NeuralPopulation::NeuralPopulation(PopulationId id, size_t size) : 
    id_(id), size_(size), neurons_(), ownedNeurons_(), 
    neuronType_(NeuronType::Internal), membranePotentials_() {}

NeuralPopulation::~NeuralPopulation() = default;

NeuralPopulation::NeuralPopulation(NeuralPopulation&& other) noexcept : 
    id_(std::exchange(other.id_, PopulationId())), 
    size_(std::exchange(other.size_, 0)), 
    neurons_(std::move(other.neurons_)), 
    ownedNeurons_(std::move(other.ownedNeurons_)), 
    neuronType_(std::exchange(other.neuronType_, NeuronType::Internal)), 
    membranePotentials_(std::move(other.membranePotentials_)) {}

NeuralPopulation& NeuralPopulation::operator=(NeuralPopulation&& other) noexcept {
    if (this != &other) {
        id_ = std::exchange(other.id_, PopulationId());
        size_ = std::exchange(other.size_, 0);
        neurons_ = std::move(other.neurons_);
        ownedNeurons_ = std::move(other.ownedNeurons_);
        neuronType_ = std::exchange(other.neuronType_, NeuronType::Internal);
        membranePotentials_ = std::move(other.membranePotentials_);
    }
    return *this;
}

PopulationId NeuralPopulation::getId() const {
    return id_;
}

size_t NeuralPopulation::getSize() const {
    return size_;
}

bool NeuralPopulation::isEmpty() const {
    return neurons_.empty();
}

Neuron* NeuralPopulation::getNeuron(NeuronIndex index) {
    if (index >= neurons_.size()) {
        return nullptr;
    }
    return neurons_[index];
}

const Neuron* NeuralPopulation::getNeuron(NeuronIndex index) const {
    if (index >= neurons_.size()) {
        return nullptr;
    }
    return neurons_[index];
}

void NeuralPopulation::createNeurons(NeuronId startId) {
    ownedNeurons_.clear();
    neurons_.clear();
    neurons_.reserve(size_);
    ownedNeurons_.reserve(size_);
    for (size_t i = 0; i < size_; ++i) {
        auto neuron = std::make_unique<Neuron>(NeuronId(startId.index() + i));
        neuron->setType(neuronType_);
        neurons_.push_back(neuron.get());
        ownedNeurons_.push_back(std::move(neuron));
    }
}

void NeuralPopulation::addNeuron(Neuron* neuron) {
    neurons_.push_back(neuron);
    if (size_ < neurons_.size()) {
        size_ = neurons_.size();
    }
}

const std::vector<Neuron*>& NeuralPopulation::getNeurons() const {
    return neurons_;
}

NeuronType NeuralPopulation::getNeuronType() const {
    return neuronType_;
}

void NeuralPopulation::setNeuronType(NeuronType type) {
    neuronType_ = type;
    for (auto* neuron : neurons_) {
        neuron->setType(type);
    }
}

size_t NeuralPopulation::getActiveCount() const {
    size_t count = 0;
    for (const auto* neuron : neurons_) {
        if (neuron->isFiring() || neuron->getFiringRate() > 0.0f) {
            ++count;
        }
    }
    return count;
}

size_t NeuralPopulation::getFiringCount() const {
    size_t count = 0;
    for (const auto* neuron : neurons_) {
        if (neuron->isFiring()) {
            ++count;
        }
    }
    return count;
}

float NeuralPopulation::getAverageFiringRate() const {
    if (neurons_.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (const auto* neuron : neurons_) {
        sum += neuron->getFiringRate();
    }
    return sum / static_cast<float>(neurons_.size());
}

float NeuralPopulation::getActivityLevel() const {
    if (neurons_.empty()) {
        return 0.0f;
    }
    return static_cast<float>(getActiveCount()) / static_cast<float>(neurons_.size());
}

void NeuralPopulation::initializeRandom(RandomGenerator& rng) {
    for (auto* neuron : neurons_) {
        neuron->initializeRandom(rng);
    }
}

void NeuralPopulation::step(Timestamp currentTime) {
    for (auto* neuron : neurons_) {
        neuron->step(currentTime);
    }
}

void NeuralPopulation::reset() {
    for (auto* neuron : neurons_) {
        neuron->reset();
    }
    membranePotentials_.clear();
}

std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(neurons_.size());
    for (const auto* neuron : neurons_) {
        ids.push_back(neuron->getId());
    }
    return ids;
}

void NeuralPopulation::recordActivity() {
    membranePotentials_.clear();
    membranePotentials_.reserve(neurons_.size());
    for (const auto* neuron : neurons_) {
        membranePotentials_.push_back(neuron->getMembranePotential());
    }
}

float NeuralPopulation::getMeanMembranePotential() const {
    if (membranePotentials_.empty()) {
        return 0.0f;
    }
    float sum = std::accumulate(membranePotentials_.begin(), 
                                 membranePotentials_.end(), 0.0f);
    return sum / static_cast<float>(membranePotentials_.size());
}

float NeuralPopulation::getVarianceMembranePotential() const {
    if (membranePotentials_.size() < 2) {
        return 0.0f;
    }
    float mean = getMeanMembranePotential();
    float sumSquares = 0.0f;
    for (float v : membranePotentials_) {
        float diff = v - mean;
        sumSquares += diff * diff;
    }
    return sumSquares / static_cast<float>(membranePotentials_.size() - 1);
}

} // namespace nlm
