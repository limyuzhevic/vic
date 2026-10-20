#pragma once

#include "../brain/Neuron.hpp"
#include <vector>

namespace nlm {

class NeuronDynamics {
public:
    NeuronDynamics(Neuron::Impl* pImpl) : pImpl(pImpl) {}
    
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    void initializeRandom(RandomGenerator& rng);

private:
    Neuron::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Neuron.hpp"

namespace nlm {

class NeuronInputHandling {
public:
    NeuronInputHandling(Neuron::Impl* pImpl) : pImpl(pImpl) {}
    
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    void injectCurrent(MembranePotential current);
    void clearTotalCurrent();

private:
    Neuron::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Neuron.hpp"
#include <vector>

namespace nlm {

class NeuronStateManagement {
public:
    NeuronStateManagement(Neuron::Impl* pImpl) : pImpl(pImpl) {}
    
    void recordSpike(Timestamp timestamp);
    void clearSpikeHistory();
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    void reset();
    const std::vector<Timestamp>& getSpikeHistory() const;

private:
    Neuron::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Neuron.hpp"

namespace nlm {

class NeuronPlasticity {
public:
    NeuronPlasticity(Neuron::Impl* pImpl) : pImpl(pImpl) {}
    
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();

private:
    Neuron::Impl* pImpl;
};

} // namespace nlm