#include "SpikeHandler.hpp"

namespace nlm {

SpikeHandler::SpikeHandler() = default;

void SpikeHandler::recordSpike(Timestamp timestamp) {
    spikeHistory_.push_back(timestamp);
    if (spikeHistory_.size() > MAX_SPIKE_HISTORY) {
        spikeHistory_.erase(spikeHistory_.begin());
    }
}

const std::vector<Timestamp>& SpikeHandler::getSpikeHistory() const {
    return spikeHistory_;
}

void SpikeHandler::clearSpikeHistory() {
    spikeHistory_.clear();
}

bool SpikeHandler::firedRecently(Timestamp currentTime, float maxAge) const {
    if (spikeHistory_.empty()) return false;
    return (currentTime - spikeHistory_.back()) <= maxAge;
}

float SpikeHandler::getLastSpikeTime() const {
    if (spikeHistory_.empty()) return -1.0f;
    return spikeHistory_.back();
}

void SpikeHandler::recordPreSpikeForPlasticity(Timestamp timestamp) {
    preSpikeHistory_.push_back(timestamp);
    if (preSpikeHistory_.size() > MAX_SPIKE_HISTORY) {
        preSpikeHistory_.erase(preSpikeHistory_.begin());
    }
}

void SpikeHandler::recordPostSpikeForPlasticity(Timestamp timestamp) {
    postSpikeHistory_.push_back(timestamp);
    if (postSpikeHistory_.size() > MAX_SPIKE_HISTORY) {
        postSpikeHistory_.erase(postSpikeHistory_.begin());
    }
}

void SpikeHandler::clearPlasticitySpikeHistory() {
    preSpikeHistory_.clear();
    postSpikeHistory_.clear();
}

} // namespace nlm