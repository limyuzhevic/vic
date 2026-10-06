#include "core/Types/Types.hpp"
#include <string>

namespace nlm {

const char* neuronTypeToString(NeuronType type) {
    switch (type) {
        case NeuronType::Excitatory: return "Excitatory";
        case NeuronType::Inhibitory: return "Inhibitory";
        case NeuronType::Modulatory: return "Modulatory";
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Internal: return "Internal";
        case NeuronType::Glial: return "Glial";
        case NeuronType::StemCell: return "StemCell";
        default: return "Unknown";
    }
}

const char* synapseTypeToString(SynapseType type) {
    switch (type) {
        case SynapseType::Excitatory: return "Excitatory";
        case SynapseType::Inhibitory: return "Inhibitory";
        case SynapseType::Modulatory: return "Modulatory";
        case SynapseType::Electrical: return "Electrical";
        case SynapseType::GapJunction: return "GapJunction";
        case SynapseType::Metabotropic: return "Metabotropic";
        default: return "Unknown";
    }
}

const char* developmentalStageToString(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

const char* firingStateToString(FiringState state) {
    switch (state) {
        case FiringState::Resting: return "Resting";
        case FiringState::Active: return "Active";
        case FiringState::Refractory: return "Refractory";
        case FiringState::Inhibited: return "Inhibited";
        case FiringState::Bursting: return "Bursting";
        case FiringState::Adapting: return "Adapting";
        default: return "Unknown";
    }
}

} // namespace nlm