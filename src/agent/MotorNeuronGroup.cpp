#include "MotorNeuronGroup.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

MotorNeuronGroup::MotorNeuronGroup(const std::string& name, int priority)
    : name_(name), priority_(priority), activity_(0.0f), firingRate_(0.0f), actionTypeName_(name) {}

void MotorNeuronGroup::addNeuron(Neuron* neuron) {
    if (neuron) {
        neurons_.push_back(neuron);
        NLM_LOG_DEBUG("Added neuron to motor group " + name_);
    }
}

float MotorNeuronGroup::getActivity() const {
    return activity_;
}

float MotorNeuronGroup::getFiringRate() const {
    return firingRate_;
}

size_t MotorNeuronGroup::getRefractoryCount() const {
    size_t count = 0;
    for (const Neuron* n : neurons_) {
        if (n && n->getState().firingState == FiringState::Refractory) {
            ++count;
        }
    }
    return count;
}

void MotorNeuronGroup::clear() {
    neurons_.clear();
    activity_ = 0.0f;
    firingRate_ = 0.0f;
}

void MotorNeuronGroup::setActivity(float activity) {
    activity_ = activity;
}

void MotorNeuronGroup::setFiringRate(float rate) {
    firingRate_ = rate;
}

} // namespace nlm
