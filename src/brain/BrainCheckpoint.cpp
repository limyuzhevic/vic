// BrainCheckpoint.cpp - Checkpoint system integration implementation
#include "BrainCheckpoint.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

BrainCheckpoint::BrainCheckpoint() : brain_(nullptr) {}

BrainCheckpoint::~BrainCheckpoint() = default;

bool BrainCheckpoint::save(const std::string& filepath) const {
    if (!brain_) {
        NLM_LOG_ERROR("BrainCheckpoint: Cannot save - no brain reference!");
        return false;
    }
    
    return brain_->save(filepath);
}

bool BrainCheckpoint::load(const std::string& filepath) {
    if (!brain_) {
        NLM_LOG_ERROR("BrainCheckpoint: Cannot load - no brain reference!");
        return false;
    }
    
    return brain_->load(filepath);
}

std::string BrainCheckpoint::getCheckpointInfo() const {
    if (!brain_) return "No brain reference";
    
    std::stringstream ss;
    ss << "Brain Checkpoint Info:" << std::endl;
    ss << "  Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    ss << "  Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    ss << "  Regions: " << brain_->getRegionCount() << std::endl;
    ss << "  Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
    ss << "  Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
    
    return ss.str();
}

} // namespace nlm