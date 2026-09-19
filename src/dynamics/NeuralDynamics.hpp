// Abstract base class for neural dynamics
class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state for one step
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Update synapse state for one step
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    // Apply synaptic input from spike
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    // Check if neuron should fire
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    // Reset dynamics state
    virtual void reset() = 0;
};

// Integrate-and-fire dynamics (simple model)
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics();
    ~IntegrateAndFireDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Parameters
    void setMembraneTimeConstant(float tau);
    float getMembraneTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hodgkin-Huxley dynamics (detailed model)
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Parameters
    void setMembraneTimeConstant(float tau);
    float getMembraneTimeConstant() const;
    
    // HH-specific parameters
    void setThresholdVoltage(float V_th) { thresholdVoltage = V_th; }
    void setRestingVoltage(float V_rest) { restingVoltage = V_rest; }
    void setCapacitance(float C_m) { capacitance = C_m; }
    void setConductance(float g_Na_max, float g_K_max, float g_L) { 
        g_Na_max_ = g_Na_max; g_K_max_ = g_K_max; g_L_ = g_L; }
    
    float getThresholdVoltage() const { return thresholdVoltage; }
    float getRestingVoltage() const { return restingVoltage; }
    float getCapacitance() const { return capacitance; }
    
    // HH gating variables
    float getSodiumActivation() const { return m_; }
    float getSodiumInactivation() const { return h_; }
    float getPotassiumActivation() const { return n_; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // HH parameters
    float thresholdVoltage;
    float restingVoltage;
    float capacitance;
    float g_Na_max_;
    float g_K_max_;
    float g_L_;
    
    // HH gating variables
    float m_;  // Sodium activation
    float h_;  // Sodium inactivation
    float n_;  // Potassium activation
};

// FitzHugh-Nagumo dynamics (simplified excitation model)
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics();
    ~FitzHughNagumoDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Parameters
    void setTimeConstant(float tau) { timeConstant = tau; }
    void setSensitivity(float beta) { sensitivity = beta; }
    float getTimeConstant() const { return timeConstant; }
    void setRecoveryVariableTimeConstant(float tau_r) { recoveryTimeConstant = tau_r; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // FHN parameters
    float timeConstant;
    float sensitivity;
    float recoveryTimeConstant;
    
    // FHN state variables
    float v_;    // Membrane potential
    float w_;    // Recovery variable
};

} // namespace nlm
