#include "Neuromodulator.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <unordered_map>
#include <queue>

namespace nlm {

// Biologically realistic dopamine parameters based on experimental data
namespace DopamineParams {
    // Firing modes
    constexpr float PHASIC_THRESHOLD = 1.5f;      // Dopamine level threshold for phasic burst
    constexpr float TONIC_PEAK = 0.2f;           // Peak tonic dopamine level
    constexpr float BURST_PEAK = 5.0f;           // Peak phasic burst level
    constexpr float BURST_DURATION = 100.0f;      // Burrough burst duration in ms
    constexpr float BURST_INTERBUTRAL = 500.0f;   // Time between bursts
    
    // Release and reuptake kinetics
    constexpr float BASELINE_DA = 0.05f;         // Baseline dopamine level (nM)
    constexpr float MAX_DA = 15.0f;              // Maximum dopamine concentration (nM)
    constexpr float REUPTAKE_RATE = 0.15f;       // DAT reuptake rate constant (1/ms)
    constexpr float REUPTAKE_CAPACITY = 50.0f;   // Maximum reuptake rate (nM/ms)
    constexpr float RELEASE_RATE = 2.0f;         // Vesicle release rate (nM/ms)
    constexpr float RELEASE_PROBABILITY = 0.4f;  // Probability of vesicle release per spike
    constexpr float VESICLE_POOL_SIZE = 100.0f;   // Number of available vesicles
    
    // Degradation by MAO enzymes
    constexpr float MAO_CONCENTRATION = 0.05f;   // MAO enzyme concentration (nM)
    constexpr float K_MAO = 0.1f;                // Michaelis constant for MAO (nM)
    constexpr float V_MAX_MAO = 0.3f;            // Maximum MAO velocity (nM/ms)
    
    // Reversal potential
    constexpr float E_DA = 0.0f;                // Dopamine reversal potential (mV)
    
    // Synaptic parameters
    constexpr float D1_DENSITY = 1.0f;            // D1 receptor density (receptors/µm²)
    constexpr float D2_DENSITY = 0.8f;            // D2 receptor density (receptors/µm²)
    constexpr float K_D1 = 0.05f;                // Dissociation constant for D1 (nM)
    constexpr float K_D2 = 0.08f;                // Dissociation constant for D2 (nM)
    
    // STDP modulation parameters
    constexpr float DA_LTP_BOOST = 0.3f;         // Dopamine boost to LTP magnitude
    constexpr float DA_LTD_SUPPRESS = 0.7f;       // Dopamine suppression of LTD magnitude
    constexpr float STDP_TIME_CONST = 20.0f;      // STDP time window (ms)
    
    // Temporal difference learning parameters
    constexpr float TD_GAMMA = 0.9f;              // Discount factor for TD learning
    constexpr float TD_LEARNING_RATE = 0.01f;     // TD learning rate
    constexpr float RPE_MAX = 2.0f;               // Maximum RPE value (nM)
    
    // Behavioral modulation parameters
    constexpr float EXPLORATION_BASE = 0.3f;     // Baseline exploration rate
    constexpr float NOVELTY_THRESHOLD = 0.15f;    // Novelty detection threshold
    constexpr float INEPTIVE_GAIN = 0.8f;        // Incentive salience gain
    
    // Pathway parameters
    constexpr float MESOLIMBIC_GAIN = 1.0f;       // Mesolimbic pathway gain
    constexpr float MESOCORTICAL_GAIN = 0.7f;     // Mesocortical pathway gain
    constexpr float PALLIDAL_GAIN = 0.5f;         // Pallidal pathway gain
}

struct Dopamine::Impl {
    // Dopamine concentration state
    float daConcentration;      // Current dopamine concentration (nM)
    float tonicLevel;           // Tonic firing mode baseline (nM)
    float phasicLevel;          // Phasic burst level (nM)
    
    // Synaptic dynamics
    float vesiclePool;          // Available vesicles (normalized 0-1)
    float replenishmentRate;    // Vesicle pool replenishment rate (1/ms)
    float depletionRate;        // Vesicle pool depletion rate (1/ms)
    
    // Transporter dynamics
    float extracellularDA;       // Extracellular dopamine (nM)
    float intracellularDA;      // Intracellular dopamine (nM)
    float datCapacity;          // DAT transporter capacity (nM)
    float datUtilization;       // Current DAT utilization (0-1)
    
    // Degradation dynamics
    float maoLevel;             // MAO enzyme level (nM)
    float degradationRate;       // Current degradation rate (1/ms)
    
    // Receptor dynamics
    float d1ReceptorOccupancy;   // D1 receptor occupancy (0-1)
    float d2ReceptorOccupancy;   // D2 receptor occupancy (0-1)
    float d1Activation;         // D1 receptor activation level
    float d2Activation;         // D2 receptor activation level
    
    // Learning systems
    float valueEstimate;         // Current value estimate (TD learning)
    float predictionError;      // Reward prediction error (nM)
    float temporalDifference;   // Temporal difference error
    float rewardHistory[10];     // Recent reward history (last 10 steps)
    int rewardHistoryIndex;      // Index in reward history buffer
    
    // Plasticity state
    float eligibilityTrace;     // Synaptic eligibility trace for reward-modulated learning
    float calciumCurrent;       // Postsynaptic calcium concentration
    float calciumBaseline;      // Resting calcium level (nM)
    float calciumDecay;         // Calcium decay rate (1/ms)
    
    // Behavioral modulation
    float explorationBias;      // Exploration vs exploitation bias (0-1)
    float noveltySignal;        // Novelty detection signal
    float incentiveSalience;    // Current motivational drive (0-1)
    float uncertaintySignal;    // Reward uncertainty measure
    
    // Pathway segregation
    float mesolimbicSignal;     // Mesolimbic pathway activity
    float mesocorticalSignal;    // Mesocortical pathway activity
    float pallidalSignal;        // Pallidal pathway activity
    
    // Temporal processing
    std::queue<float> spikeTimes;  // Spike timing history
    float lastBurstTime;          // Time of last phasic burst
    float burstCounter;            // Current burst count
    
    // Random number generator for stochastic processes
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    Impl() : daConcentration(DopamineParams::BASELINE_DA),
             tonicLevel(DopamineParams::TONIC_PEAK),
             phasicLevel(DopamineParams::BURST_PEAK),
             vesiclePool(1.0f),
             replenishmentRate(1.0f / 2000.0f),  // Replenish over 2 seconds
             depletionRate(0.001f),
             extracellularDA(DopamineParams::BASELINE_DA),
             intracellularDA(DopamineParams::BASELINE_DA),
             datCapacity(DopamineParams::REUPTAKE_CAPACITY),
             datUtilization(0.0f),
             maoLevel(DopamineParams::MAO_CONCENTRATION),
             degradationRate(0.01f),
             d1ReceptorOccupancy(0.0f),
             d2ReceptorOccupancy(0.0f),
             d1Activation(0.0f),
             d2Activation(0.0f),
             valueEstimate(0.0f),
             predictionError(0.0f),
             temporalDifference(0.0f),
             rewardHistoryIndex(0),
             eligibilityTrace(0.0f),
             calciumCurrent(DopamineParams::calciumBaseline),
             calciumBaseline(0.1f),
             calciumDecay(0.05f),
             explorationBias(DopamineParams::EXPLORATION_BASE),
             noveltySignal(0.0f),
             incentiveSalience(1.0f),
             uncertaintySignal(0.0f),
             mesolimbicSignal(0.0f),
             mesocorticalSignal(0.0f),
             pallidalSignal(0.0f),
             lastBurstTime(0.0f),
             burstCounter(0.0f),
             uniformDist(0.0f, 1.0f),
             normalDist(0.0f, 1.0f) {
        // Initialize with random seed
        std::random_device rd;
        rng.seed(rd());
        
        // Initialize reward history
        for (int i = 0; i < 10; ++i) {
            rewardHistory[i] = 0.0f;
        }
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->daConcentration;
}

void Dopamine::setLevel(float level) {
    pImpl->daConcentration = std::clamp(level, 0.0f, DopamineParams::MAX_DA);
}

void Dopamine::update(TimestepDuration dt) {
    float delta_t = static_cast<float>(dt);
    
    // Update vesicle pool dynamics
    pImpl->vesiclePool += (pImpl->replenishmentRate - pImpl->depletionRate) * delta_t;
    pImpl->vesiclePool = std::clamp(pImpl->vesiclePool, 0.0f, 1.0f);
    
    // Update intracellular dopamine
    pImpl->intracellularDA *= (1.0f - DopamineParams::REUPTAKE_RATE * pImpl->datUtilization) * delta_t;
    
    // Update extracellular dopamine - differential equation for dopamine dynamics
    // dDA/dt = k_release * vesiclePool - k_reuptake * DA - k_degradation * DA - k_clearance * DA
    float releaseTerm = DopamineParams::RELEASE_RATE * pImpl->vesiclePool;
    float reuptakeTerm = DopamineParams::REUPTAKE_RATE * pImpl->datUtilization * pImpl->extracellularDA;
    float degradationTerm = (DopamineParams::V_MAX_MAO * pImpl->maoxnDA) / 
                           (DopamineParams::K_MAO + pImpl->extracellularDA);
    
    pImpl->extracellularDA += (releaseTerm - reuptakeTerm - degradationTerm) * delta_t;
    pImpl->extracellularDA = std::clamp(pImpl->extracellularDA, 
                                       DopamineParams::BASELINE_DA, 
                                       DopamineParams::MAX_DA);
    
    // Update total dopamine level (weighted combination of intracellular and extracellular)
    pImpl->daConcentration = 0.3f * pImpl->intracellularDA + 0.7f * pImpl->extracellularDA;
    
    // Update receptor occupancy using Hill equation
    float daForReceptors = pImpl->daConcentration;
    pImpl->d1ReceptorOccupancy = (daForReceptors * DopamineParams::D1_DENSITY) / 
                                (DopamineParams::K_D1 + daForReceptors);
    pImpl->d2ReceptorOccupancy = (daForReceptors * DopamineParams::D2_DENSITY) / 
                                (DopamineParams::K_D2 + daForReceptors);
    
    // Update receptor activation with kinetics
    pImpl->d1Activation += (pImpl->d1ReceptorOccupancy - pImpl->d1Activation) * 0.1f * delta_t;
    pImpl->d2Activation += (pImpl->d2ReceptorOccupancy - pImpl->d2Activation) * 0.1f * delta_t;
    
    // Update calcium dynamics
    pImpl->calciumCurrent *= (1.0f - pImpl->calciumDecay) * delta_t;
    
    // Update eligibility trace for reward-modulated learning
    pImpl->eligibilityTrace *= std::exp(-1.0f / DopamineParams::STDP_TIME_CONST) * delta_t;
    
    // Update novelty signal decay
    pImpl->noveltySignal *= (1.0f - 0.01f) * delta_t;
    
    // Update pathway signals based on dopamine level
    pImpl->mesolimbicSignal = pImpl->daConcentration * DopamineParams::MESOLIMBIC_GAIN;
    pImpl->mesocorticalSignal = pImpl->daConcentration * DopamineParams::MESOCORTICAL_GAIN;
    pImpl->pallidalSignal = pImpl->daConcentration * DopamineParams::PALLIDAL_GAIN;
    
    // Update behavioral modulation
    pImpl->explorationBias = DopamineParams::EXPLORATION_BASE * 
                            (1.0f - pImpl->incentiveSalience) * 
                            (1.0f + pImpl->noveltySignal * 0.5f);
    pImpl->explorationBias = std::clamp(pImpl->explorationBias, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Generate phasic dopamine burst based on reward magnitude
    // Stochastic release mechanism based on vesicle availability
    
    if (pImpl->vesiclePool > DopamineParams::RELEASE_PROBABILITY && 
        pImpl->uniformDist(pImpl->rng) < DopamineParams::RELEASE_PROBABILITY) {
        
        // Calculate burst amplitude based on reward magnitude with saturation
        float burstAmplitude = std::min(reward * DopamineParams::INEPTIVE_GAIN, 
                                       DopamineParams::BURST_PEAK);
        
        // Add phasic component to extracellular dopamine
        pImpl->extracellularDA += burstAmplitude;
        
        // Update vesicle pool
        pImpl->vesiclePool *= (1.0f - DopamineParams::depletionRate);
        
        // Update temporal processing
        float currentTime = pImpl->spikeTimes.size() > 0 ? pImpl->spikeTimes.back() : 0.0f;
        pImpl->spikeTimes.push(currentTime);
        
        // Keep spike history limited
        if (pImpl->spikeTimes.size() > 100) {
            pImpl->spikeTimes.pop();
        }
        
        // Generate burst if close to burst time
        if (currentTime - pImpl->lastBurstTime > DopamineParams::BURST_INTERBUTRAL) {
            pImpl->burstCounter = DopamineParams::BURST_PEAK * pImpl->vesiclePool;
            pImpl->lastBurstTime = currentTime;
        }
    }
    
    // Update reward history for TD learning
    pImpl->rewardHistory[pImpl->rewardHistoryIndex] = reward;
    pImpl->rewardHistoryIndex = (pImpl->rewardHistoryIndex + 1) % 10;
    
    // Update incentive salience based on reward history
    float avgReward = 0.0f;
    for (int i = 0; i < 10; ++i) {
        avgReward += pImpl->rewardHistory[i];
    }
    avgReward /= 10.0f;
    
    pImpl->incentiveSalience = std::tanh(avgReward * DopamineParams::INEPTIVE_GAIN);
    
    // Update uncertainty signal based on reward variance
    float rewardVariance = 0.0f;
    float sumSq = 0.0f;
    for (int i = 0; i < 10; ++i) {
        float diff = pImpl->rewardHistory[i] - avgReward;
        sumSq += diff * diff;
    }
    rewardVariance = sumSq / 10.0f;
    pImpl->uncertaintySignal = std::min(1.0f, rewardVariance * 2.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Compute reward prediction error using temporal difference learning
    // RPE = reward + γ*V(s') - V(s)
    
    // Clip error to reasonable range to prevent instability
    float clippedError = std::clamp(error, -DopamineParams::RPE_MAX, DopamineParams::RPE_MAX);
    
    // Update prediction error state
    pImpl->predictionError = clippedError;
    pImpl->temporalDifference = clippedError;
    
    // Update eligibility trace for reward-modulated learning
    // Dopaminergic signals gate synaptic eligibility traces
    pImpl->eligibilityTrace += clippedError * DopamineParams::TD_LEARNING_RATE;
    
    // Modulate synaptic plasticity based on RPE
    // Positive RPE enhances LTP, negative RPE enhances LTD
    if (clippedError > 0) {
        // Phasic dopamine burst (reward prediction error)
        pImpl->extracellularDA += clippedError * 2.0f;  // Strong burst for positive RPE
    } else {
        // Phasic dopamine dip (reward prediction error)
        pImpl->extracellularDA += clippedError * 0.5f;  // Smaller dip for negative RPE
    }
    
    // Update value estimates using TD learning
    pImpl->valueEstimate += clippedError * DopamineParams::TD_LEARNING_RATE;
    
    // Update calcium currents based on RPE
    if (clippedError > 0) {
        // Positive RPE increases calcium influx
        pImpl->calciumCurrent += clippedError * 0.1f;
    } else {
        // Negative RPE decreases calcium influx
        pImpl->calciumCurrent += clippedError * 0.05f;
    }
}

float Dopamine::getPlasticityFactor() const {
    // Calculate plasticity factor based on dopamine receptor activation
    // D1/D5 receptors (excitatory) enhance plasticity, D2/D3 receptors (inhibitory) suppress plasticity
    
    float d1PlasticityBoost = pImpl->d1Activation * DopamineParams::DA_LTP_BOOST;
    float d2PlasticitySuppress = pImpl->d2Activation * DopamineParams::DA_LTD_SUPPRESS;
    
    // Combine receptor effects
    float plasticityFactor = 1.0f + d1PlasticityBoost - d2PlasticitySuppress;
    
    // Add calcium-dependent plasticity modulation
    float calciumPlasticity = pImpl->calciumCurrent / (pImpl->calciumBaseline + 0.01f);
    plasticityFactor *= (1.0f + calciumPlasticity * 0.2f);
    
    // Add eligibility trace contribution
    plasticityFactor *= (1.0f + pImpl->eligibilityTrace * 0.5f);
    
    // Clamp to reasonable bounds
    return std::clamp(plasticityFactor, 0.1f, 5.0f);
}

} // namespace nlm
