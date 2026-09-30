#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * STDP Algorithm: Spike-Timing-Dependent Plasticity
     * 
     * Algorithm Section: Mathematical formulation with step-by-step derivation
     * ======================================================================
     * 
     * **1. Problem Definition**:
     * STDP solves the fundamental question of how synaptic weights should change
     * based on the relative timing of pre- and post-synaptic spikes. Unlike pure
     * Hebbian learning ("fire together, wire together"), STDP incorporates
     * temporal order information to determine direction of change.
     * 
     * **2. Mathematical Derivation**:
     * The STDP learning rule can be derived from the principle of spike-timing
     * correlation and calcium dynamics:
     * 
     * Consider the calcium concentration in a dendritic compartment as the
     * coincidence detector for synaptic modification:
     * 
     * **Calcium model**:
     *   d[Ca]/dt = -[Ca]/τ_Ca + Σ_i δ(t - t_i^pre) * α_i^pre
     *                + Σ_j δ(t - t_j^post) * α_j^post
     * 
     * **Synaptic modification**:
     *   Δw_ij ∝ ∫[Ca](t) * [Ca](t) dt over appropriate time window
     * 
     * **Integration yields**:
     *   Δw_ij = {
     *     A+ * exp(-(t_post - t_pre)/τ+)  if t_post > t_pre (pre before post)
     *     A- * exp(-(t_pre - t_post)/τ-)  if t_pre > t_post (post before pre)
     *     0                                 if t_post ≈ t_pre (simultaneous)
     *   }
     * 
     * **3. Biological Inspiration**:
     * - **NMDA Receptor Activation**: NMDA receptors require both presynaptic
     *   glutamate AND postsynaptic depolarization (removes Mg²⁺ block)
     *   - **Calcium Dynamics**: NMDA activation causes Ca²⁺ influx, which
     *   activates calcium-dependent kinases (CaMKII) for LTP
     *   - **Backpropagating Action Potentials**: Post-before-pre causes
     *   action potentials to travel back into dendrites, triggering LTD
     * - **Metabotropic Glutamate Receptors (mGluRs)**: Modulate STDP via
     *   second messenger pathways
     * 
     * **4. Implementation Notes**:
     * This implementation uses a simplified pairwise STDP rule:
     * 
     * **Algorithm Steps**:
     *   1. For each presynaptic spike (preTime)
     *   2. For each postsynaptic spike (postTime)  
     *   3. Compute timing difference: Δt = t_post - t_pre
     *   4. If Δt > 0: Apply LTP with exponential decay
     *   5. If Δt < 0: Apply LTD with exponential decay
     *   6. Accumulate all pairwise contributions
     *   7. Apply eligibility trace for reward modulation
     * 
     * **5. Performance Considerations**:
     * - **Time Complexity**: O(n²) where n is spike count
     *   *Optimization*: Use spike queues or sorted lists for O(n) computation
     * - **Space Complexity**: O(N_spikes) for spike history storage
     * - **Numerical stability**: Use exp(-Δt/τ) rather than direct calculation
     * - **Parallelization**: Independent spike pairs can be processed in parallel
     * 
     * **6. Limitations and Trade-offs**:
     * - **Simplified pairwise rule**: Ignores triplet interactions (Zhang et al., 2001)
     * - **Single exponential window**: Real STDP has asymmetric windows with different τ+
     * - **No eligibility traces**: Requires additional mechanism for reward modulation
     * - **No metaplasticity**: Synaptic learning rates don't adapt to recent history
     * 
     * **7. Biological References**:
     * - Bi & Poo (2001): Calcium control of synaptic modification
     * - Markram et al. (1997): Asymmetric spike timing dependence
     * - Song et al. (2000): STDP in network models
     * - Batchelor & Neymotin (2018): Computational STDP reviews
     */
    
    // Input validation
    VALIDATE_PARAMETER(synapse != nullptr, "Synapse pointer is null");
    VALIDATE_PARAMETER(!preSpikes.empty(), "Pre spike history is empty");
    VALIDATE_PARAMETER(!postSpikes.empty(), "Post spike history is empty");
    VALIDATE_PARAMETER(dt > 0.0, "Timestep duration must be positive");
    
    // Compute total weight change from all spike timing differences
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Pairwise spike interaction (O(n²) complexity)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (delta_t > 0) {
                // Pre before post: **LTP (Long-Term Potentiation)**
                // "The earlier the presynaptic spike relative to postsynaptic,
                // the stronger the potentiation"
                // This matches NMDA receptor activation during postsynaptic
                // depolarization following presynaptic glutamate release
                float potentiation = pImpl->ltpWeight * std::exp(-delta_t / tau);
                totalDelta += potentiation;
            } else if (delta_t < 0) {
                // Post before pre: **LTD (Long-Term Depression)**
                // "The earlier the postsynaptic spike relative to presynaptic,
                // the stronger the depression"
                // This matches backpropagating action potential effects during
                // presynaptic glutamate release
                // Note: delta_t is negative, so exp(delta_t/tau) < 1
                float depression = -pImpl->ltdWeight * std::exp(delta_t / tau);
                totalDelta += depression;
            }
            // delta_t ≈ 0: Simultaneous spikes produce minimal change
        }
    }
    
    // Apply accumulated weight change if significant
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy (short-term plasticity modulation)
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change to synapse
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    VALIDATE_PARAMETER(synapse != nullptr, "Synapse pointer is null");
    VALIDATE_PARAMETER(delta >= -1.0f && delta <= 1.0f, "Weight delta out of bounds");
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    VALIDATE_PARAMETER(weight >= 0.0f && weight <= 1.0f, "LTP weight out of bounds");
    pImpl->ltpWeight = weight;
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    VALIDATE_PARAMETER(weight >= 0.0f && weight <= 1.0f, "LTD weight out of bounds");
    pImpl->ltdWeight = weight;
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    VALIDATE_PARAMETER(tau >= 1.0f && tau <= 100.0f, "Time constant out of bounds");
    pImpl->timeConstant = tau;
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    VALIDATE_PARAMETER(ltpWeight >= 0.0f && ltpWeight <= 1.0f, "LTP weight out of bounds");
    VALIDATE_PARAMETER(ltdWeight >= 0.0f && ltdWeight <= 1.0f, "LTD weight out of bounds");
    VALIDATE_PARAMETER(tau >= 1.0f && tau <= 100.0f, "Time constant out of bounds");
    
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm

