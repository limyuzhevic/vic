#include "SensoryNeuronGroup.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

SensoryNeuronGroup::SensoryNeuronGroup(const std::string& name, int priority)
    : name_(name), priority_(priority), activity_(0.0f), firingRate_(0.0f) {}

void SensoryNeuronGroup::addNeuron(Neuron* neuron) {
    if (neuron) {
        neurons_.push_back(neuron);
        NLM_LOG_DEBUG("Added neuron to sensory group " + name_);
    }
}

float SensoryNeuronGroup::getActivity() const {
    return activity_;
}

float SensoryNeuronGroup::getFiringRate() const {
    return firingRate_;
}

size_t SensoryNeuronGroup::getRefractoryCount() const {
    size_t count = 0;
    for (const Neuron* n : neurons_) {
        if (n && n->getState().firingState == FiringState::Refractory) {
            ++count;
        }
    }
    return count;
}

void SensoryNeuronGroup::clear() {
    neurons_.clear();
    activity_ = 0.0f;
    firingRate_ = 0.0f;
}

void SensoryNeuronGroup::setActivity(float activity) {
    activity_ = activity;
}

void SensoryNeuronGroup::setFiringRate(float rate) {
    firingRate_ = rate;
}

} // namespace nlm
