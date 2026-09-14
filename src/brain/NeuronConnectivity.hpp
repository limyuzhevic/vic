#pragma once

namespace nlm {

class NeuronConnectivity {
public:
    void recordSpike(Timestamp timestamp);
    void clearSpikeHistory();
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<Timestamp>& getSpikeHistory() const;
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
};

}