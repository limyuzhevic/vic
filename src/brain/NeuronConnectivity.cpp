#include "NeuronConnectivity.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

void NeuronConnectivity::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void NeuronConnectivity::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void NeuronConnectivity::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void NeuronConnectivity::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

}