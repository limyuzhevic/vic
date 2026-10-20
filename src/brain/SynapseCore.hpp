#pragma once

#include "../brain/Synapse.hpp"

namespace nlm {

class SynapseCore {
public:
    SynapseCore(Synapse::Impl* pImpl) : pImpl(pImpl) {}
    
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    void setDelay(Delay delay);
    void setType(SynapseType type);
    bool isExcitatory() const;
    bool isInhibitory() const;
    void setEfficacy(float efficacy);
    float getEfficacy() const;
    void reset();
    void initializeRandom(RandomGenerator& rng);

private:
    Synapse::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Synapse.hpp"
#include <vector>

namespace nlm {

class SynapseSTDP {
public:
    SynapseSTDP(Synapse::Impl* pImpl) : pImpl(pImpl) {}
    
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    void clearHistory();
    void enablePlasticity(bool enable);
    void update(Synapse* syn, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration dt);
    void reset();
    
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;

private:
    Synapse::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Synapse.hpp"

namespace nlm {

class SynapseHebbian {
public:
    SynapseHebbian(Synapse::Impl* pImpl) : pImpl(pImpl) {}
    
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    void clearHistory();
    void enablePlasticity(bool enable);
    void update(Synapse* syn, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration dt);
    void reset();

private:
    Synapse::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Synapse.hpp"

namespace nlm {

class SynapseShortTermPlasticity {
public:
    SynapseShortTermPlasticity(Synapse::Impl* pImpl) : pImpl(pImpl) {}
    
    void step(Timestamp currentTime);
    void reset();

private:
    Synapse::Impl* pImpl;
};

} // namespace nlm

#pragma once

#include "../brain/Synapse.hpp"

namespace nlm {

class SynapseEligibility {
public:
    SynapseEligibility(Synapse::Impl* pImpl) : pImpl(pImpl) {}
    
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    void clearHistory();
    void enablePlasticity(bool enable);
    void setEligibilityTrace(float trace);
    float getEligibilityTrace() const;
    void decayEligibilityTrace(float decayRate);
    void step(Timestamp currentTime);
    void reset();

private:
    Synapse::Impl* pImpl;
};

} // namespace nlm