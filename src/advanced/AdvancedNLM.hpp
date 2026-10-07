// Advanced NLM Framework Implementation
// This file provides the core infrastructure for advanced NLM features
// Including custom neurons, advanced learning, visualization, simulation control,
// export/import capabilities, advanced configuration, and memory management

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <variant>
#include <optional>
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <zlib.h>
#include <lz4.h>
#include <brotli/decode.h>
#include <brotli/encode.h>

#include "Brain.hpp"
#include "PlasticityRule.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"
#include "Types/Types.hpp"
#include "Random/Random.hpp"

namespace nlm {

// === CUSTOM NEURON TYPES ===

/**
 * Advanced neuron type classification
 */
enum class AdvancedNeuronType {
    // Standard types (backward compatible)
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal,
    
    // Custom/advanced types
    Pyramidal,
    SpinyStellate,
    Basket,
    chandelier,
    Martinotti,
    PV_inhibitory,
    SOM_inhibitory,
    VIP_inhibitory,
    Intrinsics,
    MGLuR1_excitatory,
    MGLuR5_excitatory,
    GABAergic,
    Glutamatergic,
    dopaminergic,
    serotonergic,
    cholinergic,
    noradrenergic,
    gabaergic_inhibitory,
    glutamatergic_excitatory,
    self_referential,
    social_cognitive,
    predictive_coding,
    memory_associative,
    action_selection,
    reward_predictive,
    novelty_detecting,
    curiosity_driven,
    developmental_stem,
    adult_progenitor,
    migrating_neuron,
    starburst,
    bipolar,
    horizontal,
    amacrine,
    bipolar_gabergic,
    bipolar_glutamatergic,
    retinal_ganglion,
    retinal_amacrine,
    retinal_bipolar,
    optic_chiasm,
    visual_cortex,
    auditory_cortex,
    somatosensory_cortex,
    prefrontal_cortex,
    hippocampus,
    amygdala,
    thalamus,
    basal_ganglia,
    cerebellum,
    brainstem,
    spinal_cord,
    peripheral_sensor,
    interneuron,
    projection_neuron,
    local_circuit,
    relay_neuron,
    primary_motor_neuron,
    upper_motor_neuron,
    lower_motor_neuron,
    spinal_motoneuron,
    interneurons,
    corticospinal,
    reticulospinal,
    vestibulospinal,
    spinocerebellar,
    cuneocerebellar,
    olivocerebellar,
    pontine_mossy,
    climbing_fiber,
    granule_cell,
    purkinje_cell,
    stellate_cell,
    basket_cell,
    martinotti_cell,
    chandelier_cell,
    fast_spiking,
    regular_spiking,
    irregular_spiking,
    tonic_spiking,
    phasic_spiking,
    burst_spiking,
    gamma_oscillations,
    beta_oscillations,
    alpha_oscillations,
    theta_oscillations,
    delta_oscillations,
    high_frequency_oscillations,
    low_frequency_oscillations,
    spindle_oscillations,
    sharp_wave_ripple,
    population_spiking,
    coordinated_activity,
    synchrophasic,
    theta_gamma_coupling,
    cross_frequency_coupling,
    phase_precession,
    phase_resetting,
    phase_dynamics,
    spike_timing_dependent,
    rate_coding,
    temporal_coding,
    population_code,
    sparse_coding,
    efficient_coding,
    sparse_representation,
    overcomplete_representation,
    orthonormal_representation,
    independent_component_analysis,
    principal_component_analysis,
    factor_analysis,
    nonnegative_matrix_factorization,
    tensor_decomposition,
    canonical_correlation_analysis,
    mutual_information,
    information_theory,
    entropy,
    redundancy_reduction,
    dimensional_reduction,
    manifold_learning,
    spectral_embedding,
    laplacian_eigenmaps,
    t_sne,
    umap,
    auto_encoder,
    variational_auto_encoder,
    generative_adversarial_network,
    deep_boltzmann_machine,
    restricted_boltzmann_machine,
    deep_boltzmann_machine_stack,
    stacked_auto_encoders,
    convolutional_neural_network,
    recurrent_neural_network,
    long_short_term_memory,
    gated_recurrent_unit,
    transformer,
    attention_mechanism,
    self_attention,
    multi_head_attention,
    positional_encoding,
    embedding_layer,
    softmax_layer,
    cross_entropy_loss,
    mean_squared_error_loss,
    contrastive_loss,
    triplet_loss,
    siamese_network,
    capsule_network,
    dynamic_routing,
    allocation_layer,
    squash_function,
    feature_maps,
    filter_banks,
    kernels,
    filters,
    convolutional_layers,
    pooling_layers,
    max_pooling,
    average_pooling,
    global_pooling,
    strided_convolution,
    dilated_convolution,
    transposed_convolution,
    upsampling,
    dropout,
    batch_normalization,
    layer_normalization,
    instance_normalization,
    group_normalization,
    spectral_normalization,
    weight_normalization,
    residual_connections,
    skip_connections,
    dense_connections,
    highway_connections,
    recurrent_connections,
    feedback_connections,
    recurrent_weights,
    forget_gates,
    input_gates,
    output_gates,
    update_gates,
    peephole_connections,
    diagonal_connections,
    attention_weights,
    context_vectors,
    memory_cells,
    read_ports,
    write_ports,
    memory_addresses,
    indexing,
    addressing_modes,
    memory_management,
    garbage_collection,
    memory_pools,
    slab_allocation,
    buddy_system_allocation,
    first_fit_allocation,
    best_fit_allocation,
    worst_fit_allocation,
    fixed_size_allocation,
    variable_size_allocation,
    memory_mapped_files,
    virtual_memory,
    swap_space,
    page_management,
    memory_protection,
    memory_coalescing,
    cache_coherency,
    memory_barriers,
    memory_fences,
    memory_orderings,
    memory_constraints,
    memory_profiling,
    memory_analysis,
    memory_leak_detection,
    memory_usage_monitoring,
    memory_usage_tracking,
    memory_usage_reporting,
    memory_usage_alerting,
    memory_usage_monitoring
};

/**
 * Custom neuron factory function signature
 */
using CustomNeuronFactory = std::function<std::unique_ptr<Neuron>(NeuronId)>;

/**
 * Neuron configuration template
 */
struct NeuronConfig {
    std::string name;
    AdvancedNeuronType type;
    std::map<std::string, std::variant<float, int, std::string>> parameters;
    std::string integration_method;  // "euler", "rk4", "exponential_euler"
    std::string plasticity_rule;     // "stdp", "hebbian", "custom"
    double learning_rate;
    double threshold;
    double resting_potential;
    double reset_potential;
    double leak_conductance;
    double time_constant;
    double capacitance;
    int refractory_period;
    bool enable_plasticity;
    bool enable_development;
    bool enable_neuromodulation;
    std::map<std::string, double> custom_params;
};

/**
 * Custom neuron type registry
 */
class CustomNeuronRegistry {
private:
    std::map<std::string, CustomNeuronFactory> factories_;
    std::map<std::string, NeuronConfig> configs_;
    std::mutex mutex_;

public:
    /**
     * Register a custom neuron factory
     */
    void registerNeuronType(const std::string& name, CustomNeuronFactory factory) {
        std::lock_guard<std::mutex> lock(mutex_);
        factories_[name] = std::move(factory);
    }

    /**
     * Register a neuron configuration
     */
    void registerConfig(const std::string& name, const NeuronConfig& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        configs_[name] = config;
    }

    /**
     * Create a neuron using registered factory
     */
    std::unique_ptr<Neuron> createNeuron(const std::string& name, NeuronId id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = factories_.find(name);
        if (it == factories_.end()) {
            throw std::runtime_error("Unknown neuron type: " + name);
        }
        return it->second(id);
    }

    /**
     * Get neuron configuration
     */
    NeuronConfig getConfig(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = configs_.find(name);
        if (it == configs_.end()) {
            throw std::runtime_error("Unknown neuron configuration: " + name);
        }
        return it->second;
    }

    /**
     * Check if neuron type exists
     */
    bool exists(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return factories_.find(name) != factories_.end();
    }

    /**
     * Get all registered neuron types
     */
    std::vector<std::string> getNeuronTypes() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> types;
        types.reserve(factories_.size());
        for (const auto& pair : factories_) {
            types.push_back(pair.first);
        }
        return types;
    }

    /**
     * Create default pyramid neuron factory
     */
    static CustomNeuronFactory createPyramidalNeuronFactory() {
        return [](NeuronId id) {
            auto neuron = std::make_unique<Neuron>(id);
            neuron->setType(NeuronType::Excitatory);
            neuron->setThreshold(-50.0f);
            neuron->setRestingPotential(-65.0f);
            neuron->setResetPotential(-65.0f);
            neuron->setLeakConductance(10.0f);
            neuron->setRefractoryPeriod(5);
            neuron->enablePlasticity(true, true, true);
            return neuron;
        };
    }

    /**
     * Create default basket neuron factory
     */
    static CustomNeuronFactory createBasketNeuronFactory() {
        return [](NeuronId id) {
            auto neuron = std::make_unique<Neuron>(id);
            neuron->setType(NeuronType::Inhibitory);
            neuron->setThreshold(-45.0f);
            neuron->setRestingPotential(-65.0f);
            neuron->setResetPotential(-65.0f);
            neuron->setLeakConductance(8.0f);
            neuron->setRefractoryPeriod(3);
            neuron->enablePlasticity(true, false, true);
            return neuron;
        };
    }

    /**
     * Create default sensory neuron factory
     */
    static CustomNeuronFactory createSensoryNeuronFactory() {
        return [](NeuronId id) {
            auto neuron = std::make_unique<Neuron>(id);
            neuron->setType(NeuronType::Sensory);
            neuron->setThreshold(-55.0f);
            neuron->setRestingPotential(-70.0f);
            neuron->setResetPotential(-70.0f);
            neuron->setLeakConductance(12.0f);
            neuron->setRefractoryPeriod(2);
            neuron->enablePlasticity(false, true, false);
            return neuron;
        };
    }

    /**
     * Initialize default neuron types
     */
    static void initializeDefaultTypes() {
        CustomNeuronRegistry registry;
        
        registry.registerNeuronType("pyramidal", createPyramidalNeuronFactory());
        registry.registerConfig("pyramidal", {
            "name": "Pyramidal Neuron",
            "type": AdvancedNeuronType::Pyramidal,
            "parameters": {
                {"threshold", -50.0f},
                {"leak_conductance", 10.0f},
                {"refractory_period", 5}
            },
            "integration_method": "exponential_euler",
            "plasticity_rule": "stdp",
            "learning_rate": 0.01,
            "threshold": -50.0,
            "resting_potential": -65.0,
            "reset_potential": -65.0,
            "leak_conductance": 10.0,
            "time_constant": 20.0,
            "capacitance": 1.0,
            "refractory_period": 5,
            "enable_plasticity": true,
            "enable_development": true,
            "enable_neuromodulation": true
        });

        registry.registerNeuronType("basket", createBasketNeuronFactory());
        registry.registerConfig("basket", {
            "name": "Basket Cell",
            "type": AdvancedNeuronType::Basket,
            "parameters": {
                {"threshold", -45.0f},
                {"leak_conductance", 8.0f},
                {"refractory_period", 3}
            },
            "integration_method": "exponential_euler",
            "plasticity_rule": "stdp",
            "learning_rate": 0.02,
            "threshold": -45.0,
            "resting_potential": -65.0,
            "reset_potential": -65.0,
            "leak_conductance": 8.0,
            "time_constant": 15.0,
            "capacitance": 1.0,
            "refractory_period": 3,
            "enable_plasticity": true,
            "enable_development": false,
            "enable_neuromodulation": true
        });

        registry.registerNeuronType("sensory", createSensoryNeuronFactory());
        registry.registerConfig("sensory", {
            "name": "Sensory Neuron",
            "type": AdvancedNeuronType::Sensory,
            "parameters": {
                {"threshold", -55.0f},
                {"leak_conductance", 12.0f},
                {"refractory_period", 2}
            },
            "integration_method": "euler",
            "plasticity_rule": "hebbian",
            "learning_rate": 0.005,
            "threshold": -55.0,
            "resting_potential": -70.0,
            "reset_potential": -70.0,
            "leak_conductance": 12.0,
            "time_constant": 25.0,
            "capacitance": 1.0,
            "refractory_period": 2,
            "enable_plasticity": false,
            "enable_development": false,
            "enable_neuromodulation": false
        });
    }
};

/**
 * Integration method selector
 */
class IntegrationMethodSelector {
public:
    enum class Method {
        Euler,
        RungeKutta4,
        ExponentialEuler,
        Adaptive,
        CrankNicolson,
        ForwardEuler,
        BackwardEuler,
        ImplicitEuler,
        TrapezoidalRule,
        Leapfrog,
        VelocityVerlet,
        StochasticEuler,
        Milstein,
        StrangSplitting,
        OperatorSplitting,
        SpectralMethod,
        FiniteDifferenceMethod,
        FiniteElementMethod,
        BoundaryElementMethod,
        MeshMethod,
        ParticleMethod,
        MolecularDynamics,
        MonteCarloMethod,
        MarkovChainMethod,
        StochasticProcess,
        JumpProcess,
        DiffusionProcess,
        WaveEquation,
        HeatEquation,
        LaplaceEquation,
        PoissonEquation,
        HelmholtzEquation,
        SchrödingerEquation,
        DiracEquation,
        EinsteinFieldEquations,
        NavierStokesEquations,
        EulerEquations,
        NavierStokes,
        TurbulenceModel,
        ReynoldsAveragedNavierStokes,
        LargeEddySimulation,
        DirectNumericalSimulation,
        LatticeBoltzmannMethod,
        SmoothedParticleHydrodynamics,
        MeshfreeMethod,
        ElementFreeGalerkinMethod,
        GeneralizedInterpolatingMolecularDynamics,
        DissipativeParticleDynamics,
        MultiScaleMethod,
        MultiPhaseMethod,
        MultiComponentMethod,
        MultiPhysicsMethod,
        MultiDomainMethod,
        MultiTimeMethod,
        MultiScaleTimeMethod,
        MultiPhysicsTimeMethod,
        MultiPhysicsSpaceMethod,
        MultiPhysicsSpaceTimeMethod,
        MultiPhysicsMultiDomainMethod
    };

    static std::string methodToString(Method method) {
        static const std::map<Method, std::string> methodStrings = {
            {Method::Euler, "euler"},
            {Method::RungeKutta4, "rk4"},
            {Method::ExponentialEuler, "exponential_euler"},
            {Method::Adaptive, "adaptive"},
            {Method::CrankNicolson, "crank_nicolson"},
            {Method::ForwardEuler, "forward_euler"},
            {Method::BackwardEuler, "backward_euler"},
            {Method::ImplicitEuler, "implicit_euler"},
            {Method::TrapezoidalRule, "trapezoidal_rule"},
            {Method::Leapfrog, "leapfrog"},
            {Method::VelocityVerlet, "velocity_verlet"},
            {Method::StochasticEuler, "stochastic_euler"},
            {Method::Milstein, "milstein"},
            {Method::StrangSplitting, "strang_splitting"},
            {Method::OperatorSplitting, "operator_splitting"},
            {Method::SpectralMethod, "spectral_method"},
            {Method::FiniteDifferenceMethod, "finite_difference"},
            {Method::FiniteElementMethod, "finite_element"},
            {Method::BoundaryElementMethod, "boundary_element"},
            {Method::MeshMethod, "mesh_method"},
            {Method::ParticleMethod, "particle_method"},
            {Method::MolecularDynamics, "molecular_dynamics"},
            {Method::MonteCarloMethod, "monte_carlo"},
            {Method::MarkovChainMethod, "markov_chain"},
            {Method::StochasticProcess, "stochastic_process"},
            {Method::JumpProcess, "jump_process"},
            {Method::DiffusionProcess, "diffusion_process"},
            {Method::WaveEquation, "wave_equation"},
            {Method::HeatEquation, "heat_equation"},
            {Method::LaplaceEquation, "laplace_equation"},
            {Method::PoissonEquation, "poisson_equation"},
            {Method::HelmholtzEquation, "helmholtz_equation"},
            {Method::SchrödingerEquation, "schrodinger_equation"},
            {Method::DiracEquation, "dirac_equation"},
            {Method::EinsteinFieldEquations, "einstein_equations"},
            {Method::NavierStokesEquations, "navier_stokes"},
            {Method::EulerEquations, "euler_equations"},
            {Method::NavierStokes, "navier_stokes"},
            {Method::TurbulenceModel, "turbulence_model"},
            {Method::ReynoldsAveragedNavierStokes, "rans"},
            {Method::LargeEddySimulation, "les"},
            {Method::DirectNumericalSimulation, "dns"},
            {Method::LatticeBoltzmannMethod, "lattice_boltzmann"},
            {Method::SmoothedParticleHydrodynamics, "smoothed_particle_hydrodynamics"},
            {Method::MeshfreeMethod, "meshfree_method"},
            {Method::ElementFreeGalerkinMethod, "element_free_galerkin"},
            {Method::GeneralizedInterpolatingMolecularDynamics, "gimd"},
            {Method::DissipativeParticleDynamics, "dissipative_particle_dynamics"},
            {Method::MultiScaleMethod, "multiscale"},
            {Method::MultiPhaseMethod, "multiphase"},
            {Method::MultiComponentMethod, "multicomponent"},
            {Method::MultiPhysicsMethod, "multiphysics"},
            {Method::MultiDomainMethod, "multidomain"},
            {Method::MultiTimeMethod, "multitime"},
            {Method::MultiScaleTimeMethod, "multiscale_time"},
            {Method::MultiPhysicsTimeMethod, "multiphysics_time"},
            {Method::MultiPhysicsSpaceMethod, "multiphysics_space"},
            {Method::MultiPhysicsSpaceTimeMethod, "multiphysics_space_time"},
            {Method::MultiPhysicsMultiDomainMethod, "multiphysics_multidomain"}
        };
        
        auto it = methodStrings.find(method);
        return (it != methodStrings.end()) ? it->second : "euler";
    }

    static Method stringToMethod(const std::string& methodString) {
        static const std::map<std::string, Method> stringToMethod = {
            {"euler", Method::Euler},
            {"rk4", Method::RungeKutta4},
            {"exponential_euler", Method::ExponentialEuler},
            {"adaptive", Method::Adaptive},
            {"crank_nicolson", Method::CrankNicolson},
            {"forward_euler", Method::ForwardEuler},
            {"backward_euler", Method::BackwardEuler},
            {"implicit_euler", Method::ImplicitEuler},
            {"trapezoidal_rule", Method::TrapezoidalRule},
            {"leapfrog", Method::Leapfrog},
            {"velocity_verlet", Method::VelocityVerlet},
            {"stochastic_euler", Method::StochasticEuler},
            {"milstein", Method::Milstein},
            {"strang_splitting", Method::StrangSplitting},
            {"operator_splitting", Method::OperatorSplitting},
            {"spectral_method", Method::SpectralMethod},
            {"finite_difference", Method::FiniteDifferenceMethod},
            {"finite_element", Method::FiniteElementMethod},
            {"boundary_element", Method::BoundaryElementMethod},
            {"mesh_method", Method::MeshMethod},
            {"particle_method", Method::ParticleMethod},
            {"molecular_dynamics", Method::MolecularDynamics},
            {"monte_carlo", Method::MonteCarloMethod},
            {"markov_chain", Method::MarkovChainMethod},
            {"stochastic_process", Method::StochasticProcess},
            {"jump_process", Method::JumpProcess},
            {"diffusion_process", Method::DiffusionProcess},
            {"wave_equation", Method::WaveEquation},
            {"heat_equation", Method::HeatEquation},
            {"laplace_equation", Method::LaplaceEquation},
            {"poisson_equation", Method::PoissonEquation},
            {"helmholtz_equation", Method::HelmholtzEquation},
            {"schrodinger_equation", Method::SchrödingerEquation},
            {"dirac_equation", Method::DiracEquation},
            {"einstein_equations", Method::EinsteinFieldEquations},
            {"navier_stokes", Method::NavierStokesEquations},
            {"euler_equations", Method::EulerEquations},
            {"navier_stokes", Method::NavierStokes},
            {"turbulence_model", Method::TurbulenceModel},
            {"rans", Method::ReynoldsAveragedNavierStokes},
            {"les", Method::LargeEddySimulation},
            {"dns", Method::DirectNumericalSimulation},
            {"lattice_boltzmann", Method::LatticeBoltzmannMethod},
            {"smoothed_particle_hydrodynamics", Method::SmoothedParticleHydrodynamics},
            {"meshfree_method", Method::MeshfreeMethod},
            {"element_free_galerkin", Method::ElementFreeGalerkinMethod},
            {"gimd", Method::GeneralizedInterpolatingMolecularDynamics},
            {"dissipative_particle_dynamics", Method::DissipativeParticleDynamics},
            {"multiscale", Method::MultiScaleMethod},
            {"multiphase", Method::MultiPhaseMethod},
            {"multicomponent", Method::MultiComponentMethod},
            {"multiphysics", Method::MultiPhysicsMethod},
            {"multidomain", Method::MultiDomainMethod},
            {"multitime", Method::MultiTimeMethod},
            {"multiscale_time", Method::MultiScaleTimeMethod},
            {"multiphysics_time", Method::MultiPhysicsTimeMethod},
            {"multiphysics_space", Method::MultiPhysicsSpaceMethod},
            {"multiphysics_space_time", Method::MultiPhysicsSpaceTimeMethod},
            {"multiphysics_multidomain", Method::MultiPhysicsMultiDomainMethod}
        };
        
        auto it = stringToMethod.find(methodString);
        return (it != stringToMethod.end()) ? it->second : Method::Euler;
    }
};

/**
 * Neuron integrator interface
 */
class NeuronIntegrator {
public:
    virtual ~NeuronIntegrator() = default;
    
    virtual void integrate(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) = 0;
    virtual IntegrationMethodSelector::Method getMethod() const = 0;
    virtual std::string getName() const = 0;
};

/**
 * Euler integrator
 */
class EulerIntegrator : public NeuronIntegrator {
public:
    void integrate(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) override {
        if (!neuron) return;
        
        auto& state = neuron->getState();
        float V = state.membranePotential;
        float V_rest = state.restingPotential;
        float I_syn = neuron->getTotalCurrent();
        
        float dV_dt = (V_rest - V) / state.leakConductance + I_syn;
        
        V += static_cast<float>(dt) * dV_dt;
        neuron->setMembranePotential(V);
        
        if (V >= state.threshold) {
            neuron->stepLIF(currentTime, dt);
        }
    }
    
    IntegrationMethodSelector::Method getMethod() const override {
        return IntegrationMethodSelector::Method::Euler;
    }
    
    std::string getName() const override {
        return "Euler Integrator";
    }
};

/**
 * Runge-Kutta 4 integrator
 */
class RungeKutta4Integrator : public NeuronIntegrator {
public:
    void integrate(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) override {
        if (!neuron) return;
        
        auto& state = neuron->getState();
        float V = state.membranePotential;
        float V_rest = state.restingPotential;
        float I_syn = neuron->getTotalCurrent();
        
        float k1 = (V_rest - V) / state.leakConductance + I_syn;
        float k2 = (V_rest - (V + 0.5f * static_cast<float>(dt) * k1)) / state.leakConductance + I_syn;
        float k3 = (V_rest - (V + 0.5f * static_cast<float>(dt) * k2)) / state.leakConductance + I_syn;
        float k4 = (V_rest - (V + static_cast<float>(dt) * k3)) / state.leakConductance + I_syn;
        
        float dV_dt = (k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f;
        
        V += static_cast<float>(dt) * dV_dt;
        neuron->setMembranePotential(V);
        
        if (V >= state.threshold) {
            neuron->stepLIF(currentTime, dt);
        }
    }
    
    IntegrationMethodSelector::Method getMethod() const override {
        return IntegrationMethodSelector::Method::RungeKutta4;
    }
    
    std::string getName() const override {
        return "Runge-Kutta 4 Integrator";
    }
};

/**
 * Exponential Euler integrator
 */
class ExponentialEulerIntegrator : public NeuronIntegrator {
public:
    void integrate(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) override {
        if (!neuron) return;
        
        auto& state = neuron->getState();
        float V = state.membranePotential;
        float V_rest = state.restingPotential;
        float I_syn = neuron->getTotalCurrent();
        float tau = 20.0f;  // Time constant (ms)
        
        // Exponential Euler integration
        float decay = std::exp(-static_cast<float>(dt) / tau);
        float steady_state = V_rest + I_syn * state.leakConductance;
        
        V = V * decay + (1.0f - decay) * steady_state;
        neuron->setMembranePotential(V);
        
        if (V >= state.threshold) {
            neuron->stepLIF(currentTime, dt);
        }
    }
    
    IntegrationMethodSelector::Method getMethod() const override {
        return IntegrationMethodSelector::Method::ExponentialEuler;
    }
    
    std::string getName() const override {
        return "Exponential Euler Integrator";
    }
};

/**
 * Integrator factory
 */
class IntegratorFactory {
public:
    static std::unique_ptr<NeuronIntegrator> createIntegrator(
        IntegrationMethodSelector::Method method) {
        switch (method) {
            case IntegrationMethodSelector::Method::Euler:
                return std::make_unique<EulerIntegrator>();
            case IntegrationMethodSelector::Method::RungeKutta4:
                return std::make_unique<RungeKutta4Integrator>();
            case IntegrationMethodSelector::Method::ExponentialEuler:
                return std::make_unique<ExponentialEulerIntegrator>();
            default:
                return std::make_unique<ExponentialEulerIntegrator>();
        }
    }
    
    static std::unique_ptr<NeuronIntegrator> createIntegrator(const std::string& methodName) {
        return createIntegrator(IntegrationMethodSelector::stringToMethod(methodName));
    }
};

// === ADVANCED LEARNING ALGORITHMS ===

/**
 * Reward-modulated STDP
 */
class RewardModulatedSTDP : public PlasticityRule {
private:
    struct Impl;
    Impl* pImpl;

public:
    RewardModulatedSTDP(float learningRate = 0.01f, float rewardFactor = 0.1f);
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setRewardGain(float gain);
    float getRewardGain() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void configure(float rewardGain, float learningRate, float tau);
};

/**
 * BCM (Bienenstock-Cooper-Munro) rule
 */
class BCMRule : public PlasticityRule {
private:
    struct Impl;
    Impl* pImpl;

public:
    BCMRule(float theta = 0.5f, float tau = 100.0f, float beta = 0.02f);
    ~BCMRule() override;
    
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setTheta(float theta);
    float getTheta() const;
    void setTau(float tau);
    float getTau() const;
    void setBeta(float beta);
    float getBeta() const;
};

/**
 * Homeostatic plasticity rule
 */
class HomeostaticPlasticity : public PlasticityRule {
private:
    struct Impl;
    Impl* pImpl;

public:
    HomeostaticPlasticity(float targetRate = 10.0f, float timeConstant = 100.0f, 
                         float maxRateChange = 0.1f);
    ~HomeostaticPlasticity() override;
    
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setTargetRate(float rate);
    float getTargetRate() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void setMaxRateChange(float change);
    float getMaxRateChange() const;
};

/**
 * Predictive coding rule
 */
class PredictiveCodingRule : public PlasticityRule {
private:
    struct Impl;
    Impl* pImpl;

public:
    PredictiveCodingRule(float predictionErrorGain = 0.5f, float learningRate = 0.01f,
                        int predictionHorizon = 10);
    ~PredictiveCodingRule() override;
    
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setPredictionErrorGain(float gain);
    float getPredictionErrorGain() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setPredictionHorizon(int horizon);
    int getPredictionHorizon() const;
};

/**
 * Plasticity rule registry for advanced learning algorithms
 */
class PlasticityRuleRegistry {
private:
    std::map<std::string, std::function<std::unique_ptr<PlasticityRule>()>> ruleFactories_;
    std::mutex mutex_;

public:
    PlasticityRuleRegistry() {
        // Register built-in plasticity rules
        registerRule("stdp", []() { return std::make_unique<STDP>(); });
        registerRule("hebbian", []() { return std::make_unique<Hebbian>(); });
        registerRule("reward_modulated_stdp", []() { return std::make_unique<RewardModulatedSTDP>(); });
        registerRule("bcn", []() { return std::make_unique<BCMRule>(); });
        registerRule("homeostatic", []() { return std::make_unique<HomeostaticPlasticity>(); });
        registerRule("predictive_coding", []() { return std::make_unique<PredictiveCodingRule>(); });
    }
    
    void registerRule(const std::string& name, std::function<std::unique_ptr<PlasticityRule>()> factory) {
        std::lock_guard<std::mutex> lock(mutex_);
        ruleFactories_[name] = std::move(factory);
    }
    
    std::unique_ptr<PlasticityRule> createRule(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = ruleFactories_.find(name);
        if (it == ruleFactories_.end()) {
            throw std::runtime_error("Unknown plasticity rule: " + name);
        }
        return it->second();
    }
    
    bool exists(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return ruleFactories_.find(name) != ruleFactories_.end();
    }
    
    std::vector<std::string> getRuleNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        names.reserve(ruleFactories_.size());
        for (const auto& pair : ruleFactories_) {
            names.push_back(pair.first);
        }
        return names;
    }
};

// === VISUALIZATION TOOLS ===

/**
 * Neuron activity plotter for real-time visualization
 */
class NeuronActivityPlotter {
private:
    struct Impl;
    Impl* pImpl;

public:
    NeuronActivityPlotter(double updateInterval = 0.1, size_t historyLength = 1000);
    ~NeuronActivityPlotter();
    
    void start(std::shared_ptr<Brain> brain);
    void stop();
    bool isRunning() const;
    
    void update(std::shared_ptr<Brain> brain);
    void save(const std::string& filepath) const;
    void show() const;
    
    void setUpdateInterval(double interval);
    double getUpdateInterval() const;
    void setHistoryLength(size_t length);
    size_t getHistoryLength() const;
};

/**
 * Network topology visualizer
 */
class NetworkTopologyVisualizer {
private:
    struct Impl;
    Impl* pImpl;

public:
    NetworkTopologyVisualizer(const std::string& layout = "spring_embedded",
                            double nodeSize = 5.0, double edgeWidth = 1.0,
                            bool highlightActivity = true);
    ~NetworkTopologyVisualizer();
    
    void visualize(std::shared_ptr<Brain> brain, const std::string& filepath = "");
    void show() const;
    
    void setLayout(const std::string& layout);
    void setNodeSize(double size);
    void setEdgeWidth(double width);
    void setHighlightActivity(bool highlight);
    
    std::string getLayout() const;
    double getNodeSize() const;
    double getEdgeWidth() const;
    bool getHighlightActivity() const;
};

/**
 * Learning curve analyzer
 */
class LearningCurveAnalyzer {
private:
    struct Impl;
    Impl* pImpl;

public:
    LearningCurveAnalyzer(const std::vector<std::string>& metrics = {"accuracy", "loss", "firing_rate"},
                         size_t smoothingWindow = 50);
    ~LearningCurveAnalyzer();
    
    void record(size_t step, const std::map<std::string, double>& metrics);
    void analyze();
    void generateCurves(const std::string& filepath = "") const;
    void show() const;
    
    void setSmoothingWindow(size_t window);
    size_t getSmoothingWindow() const;
    std::vector<std::string> getMetrics() const;
    std::map<std::string, double> getAnalysisResults() const;
};

/**
 * Statistical visualizer
 */
class StatisticalVisualizer {
private:
    struct Impl;
    Impl* pImpl;

public:
    StatisticalVisualizer(const std::vector<std::string>& statistics = {"mean", "std", "percentile_25", "percentile_75"},
                         double confidenceInterval = 0.95);
    ~StatisticalVisualizer();
    
    void visualize(std::shared_ptr<Brain> brain, const std::string& filepath = "");
    void show() const;
    
    void setConfidenceInterval(double interval);
    double getConfidenceInterval() const;
    std::vector<std::string> getStatistics() const;
};

// === SIMULATION CONTROL ===

/**
 * Custom timestep scheduler
 */
class CustomTimestepScheduler {
public:
    virtual ~CustomTimestepScheduler() = default;
    
    virtual TimestepDuration getTimestep(std::shared_ptr<Brain> brain, SimulationStep currentStep) = 0;
    virtual std::string getName() const = 0;
};

/**
 * Event-driven simulation controller
 */
class EventDrivenSimulator {
private:
    struct Impl;
    Impl* pImpl;

public:
    EventDrivenSimulator();
    ~EventDrivenSimulator();
    
    void start(std::shared_ptr<Brain> brain);
    void stop();
    bool isRunning() const;
    
    void runToEvent(std::shared_ptr<Brain> brain);
    void runToTime(std::shared_ptr<Brain> brain, Timestamp targetTime);
    
    void setSpikeDriven(bool enabled);
    bool getSpikeDriven() const;
    
    void setSynapseDriven(bool enabled);
    bool getSynapseDriven() const;
    
    void setPlasticityDriven(bool enabled);
    bool getPlasticityDriven() const;
};

/**
 * Parallel processing controller
 */
class ParallelProcessor {
public:
    struct Config {
        bool neuronUpdate = true;
        bool synapseUpdate = false;
        bool spikePropagation = true;
        bool plasticityUpdate = false;
        int numThreads = std::thread::hardware_concurrency();
        bool enableLoadBalancing = true;
        bool enableWorkStealing = true;
    };

    ParallelProcessor(const Config& config = Config{});
    ~ParallelProcessor();
    
    void processNeurons(std::shared_ptr<Brain> brain, SimulationStep step);
    void processSynapses(std::shared_ptr<Brain> brain);
    void processSpikes(std::shared_ptr<Brain> brain);
    
    void setConfig(const Config& config);
    const Config& getConfig() const;
    int getActiveThreads() const;
    double getUtilization() const;
};

/**
 * Memory management controller
 */
class MemoryManager {
public:
    struct Config {
        size_t neuronPoolSize = 10000;
        size_t spikeQueueSize = 1000000;
        uint64_t checkpointInterval = 1000;
        double maxMemoryGB = 16.0;
        double gcThreshold = 0.8;
        std::string compressionLevel = "balanced";
        bool enableMemoryPools = true;
        bool enableGarbageCollection = true;
        bool enableCheckpointing = true;
        size_t maxCheckpoints = 10;
    };

    MemoryManager(const Config& config = Config{});
    ~MemoryManager();
    
    void update(std::shared_ptr<Brain> brain, SimulationStep step);
    void allocateMemory(std::shared_ptr<Brain> brain);
    void deallocateMemory(std::shared_ptr<Brain> brain);
    
    void triggerGC();
    void setGCThreshold(double threshold);
    double getMemoryUsage() const;
    double getGCThreshold() const;
    size_t getAllocatedNeurons() const;
    size_t getAllocatedSpikes() const;
    
    void enableMemoryPools(bool enable);
    void enableGarbageCollection(bool enable);
    void enableCheckpointing(bool enable);
};

// === EXPORT/IMPORT CAPABILITIES ===

/**
 * Brain exporter for multiple formats
 */
class BrainExporter {
private:
    struct Impl;
    Impl* pImpl;

public:
    BrainExporter(std::shared_ptr<Brain> brain);
    ~BrainExporter();
    
    bool exportToFile(const std::string& filepath, const std::string& format = "json");
    bool exportToStream(std::ostream& stream, const std::string& format = "json");
    
    std::string exportToString(const std::string& format = "json");
    
    bool exportNeurons(const std::string& filepath, const std::string& format = "json");
    bool exportSynapses(const std::string& filepath, const std::string& format = "json");
    bool exportConnections(const std::string& filepath, const std::string& format = "json");
    bool exportPlasticity(const std::string& filepath, const std::string& format = "json");
    bool exportDevelopment(const std::string& filepath, const std::string& format = "json");
    bool exportNeuromodulation(const std::string& filepath, const std::string& format = "json");
    bool exportMemory(const std::string& filepath, const std::string& format = "json");
    bool exportSpikeHistory(const std::string& filepath, const std::string& format = "json");
    bool exportRandomState(const std::string& filepath, const std::string& format = "json");
    bool exportSimulationState(const std::string& filepath, const std::string& format = "json");
    
    void setCompressionLevel(const std::string& level);
    std::string getCompressionLevel() const;
    void setFormatOptions(const std::map<std::string, std::string>& options);
    std::map<std::string, std::string> getFormatOptions() const;
};

/**
 * Differential compressor
 */
class DeltaCompressor {
private:
    struct Impl;
    Impl* pImpl;

public:
    DeltaCompressor(const std::string& baseCheckpoint = "",
                    const std::string& compression = "lz4");
    ~DeltaCompressor();
    
    std::vector<uint8_t> createDelta(const std::vector<uint8_t>& baseState,
                                     const std::vector<uint8_t>& currentState);
    bool save(const std::string& filepath, const std::vector<uint8_t>& delta);
    std::vector<uint8_t> load(const std::string& filepath);
    
    void setCompression(const std::string& algorithm);
    std::string getCompression() const;
};

/**
 * Incremental checkpoint manager
 */
class IncrementalCheckpointManager {
private:
    struct Impl;
    Impl* pImpl;

public:
    IncrementalCheckpointManager();
    ~IncrementalCheckpointManager();
    
    bool createCheckpoint(const std::string& name, std::shared_ptr<Brain> brain);
    bool loadCheckpoint(const std::string& name, std::shared_ptr<Brain> brain);
    
    bool hasCheckpoint(const std::string& name) const;
    std::vector<std::string> getCheckpointNames() const;
    
    void setBaseCheckpoint(const std::string& name);
    std::string getBaseCheckpoint() const;
    
    std::vector<uint8_t> getDelta(const std::string& fromName, const std::string& toName);
};

// === ADVANCED CONFIGURATION ===

/**
 * Configuration template system
 */
class ConfigurationTemplates {
private:
    std::map<std::string, std::map<std::string, std::string>> templates_;
    std::mutex mutex_;

public:
    ConfigurationTemplates();
    ~ConfigurationTemplates();
    
    bool addTemplate(const std::string& name, const std::map<std::string, std::string>& config);
    bool removeTemplate(const std::string& name);
    bool hasTemplate(const std::string& name) const;
    
    std::map<std::string, std::string> getTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;
    
    std::map<std::string, std::string> getFastLearningTemplate() const;
    std::map<std::string, std::string> getStableLearningTemplate() const;
    std::map<std::string, std::string> getBalancedTemplate() const;
    std::map<std::string, std::string> getExploratoryTemplate() const;
    std::map<std::string, std::string> getExploitativeTemplate() const;
    
    void saveTemplate(const std::string& name, const std::map<std::string, std::string>& config);
};

/**
 * Parameter optimizer for hyperparameter tuning
 */
class ParameterOptimizer {
public:
    enum class Method { RandomSearch, BayesianOptimization, GridSearch, GeneticAlgorithm, SimulatedAnnealing };
    
    struct Config {
        Method method = Method::RandomSearch;
        std::map<std::string, std::vector<double>> parameterSpace;
        size_t nTrials = 50;
        size_t nInitialPoints = 10;
        double acquisitionThreshold = 0.01;
        bool parallel = false;
        size_t maxIterations = 1000;
        double tolerance = 1e-6;
        std::string model = "random_forest";
        std::string acquisitionFunction = "ei";
        size_t populationSize = 50;
        double mutationRate = 0.1;
        double crossoverRate = 0.8;
        double temperature = 1.0;
        double coolingRate = 0.99;
    };

    ParameterOptimizer(const Config& config = Config{});
    ~ParameterOptimizer();
    
    std::map<std::string, double> optimize(std::function<double(const std::map<std::string, double>&)> objectiveFunction);
    std::pair<std::map<std::string, double>, double> optimizeWithDetails(std::function<double(const std::map<std::string, double>&)> objectiveFunction);
    
    void setConfig(const Config& config);
    const Config& getConfig() const;
    void setObjectiveFunction(std::function<double(const std::map<std::string, double>&)> func);
    std::function<double(const std::map<std::string, double>&)> getObjectiveFunction() const;
};

/**
 * Bayesian optimization support
 */
class BayesianOptimizer {
private:
    struct Impl;
    Impl* pImpl;

public:
    BayesianOptimizer(size_t nInitialPoints = 10, size_t nIterations = 30,
                     const std::string& model = "random_forest",
                     const std::string& acquisitionFunction = "ei");
    ~BayesianOptimizer();
    
    std::pair<std::map<std::string, double>, double> optimize(
        std::function<double(const std::map<std::string, double>&)> objectiveFunction,
        const std::map<std::string, std::pair<double, double>>& parameterBounds);
    
    std::map<std::string, double> suggestNextParameters(const std::map<std::string, double>& previousResults);
    void updateModel(const std::map<std::string, double>& parameters, double result);
    
    void setModel(const std::string& model);
    void setAcquisitionFunction(const std::string& function);
    std::string getModel() const;
    std::string getAcquisitionFunction() const;
};

// === MEMORY MANAGEMENT ===

/**
 * Advanced memory pool manager
 */
class AdvancedMemoryPool {
private:
    struct Impl;
    Impl* pImpl;

public:
    AdvancedMemoryPool(size_t size = 1000000, size_t alignment = 64,
                      const std::string& type = "numa_aware");
    ~AdvancedMemoryPool();
    
    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);
    size_t getAllocatedSize() const;
    double getUsageRatio() const;
    
    void enableBurstSupport(bool enable);
    bool getBurstSupport() const;
    void setAllocationStrategy(const std::string& strategy);
};

/**
 * Smart garbage collector
 */
class SmartGarbageCollector {
private:
    struct Impl;
    Impl* pImpl;

public:
    SmartGarbageCollector(double threshold = 0.8, size_t collectionFrequency = 100,
                         size_t maxPromotions = 3, double pauseDuration = 10.0,
                         bool aggressiveMode = false);
    ~SmartGarbageCollector();
    
    void collect();
    void triggerGC();
    void setThreshold(double threshold);
    double getThreshold() const;
    void setCollectionFrequency(size_t frequency);
    size_t getCollectionFrequency() const;
    void setMaxPromotions(size_t promotions);
    size_t getMaxPromotions() const;
    void setPauseDuration(double duration);
    double getPauseDuration() const;
    void setAggressiveMode(bool aggressive);
    bool getAggressiveMode() const;
    double getCurrentMemoryUsage() const;
    size_t getGCCount() const;
};

/**
 * Memory monitor
 */
class MemoryMonitor {
private:
    struct Impl;
    Impl* pImpl;

public:
    MemoryMonitor(double samplingInterval = 1.0,
                  const std::vector<std::string>& metrics = {"usage", "growth", "leaks"},
                  const std::map<std::string, std::string>& alerts = {});
    ~MemoryMonitor();
    
    void startMonitoring(std::shared_ptr<Brain> brain);
    void stopMonitoring();
    bool isMonitoring() const;
    
    double getMemoryUsage() const;
    double getGrowthRate() const;
    double getLeakRate() const;
    size_t getAllocatedNeurons() const;
    size_t getAllocatedSpikes() const;
    
    void setAlert(const std::string& metric, double threshold, const std::string& action);
    void removeAlert(const std::string& metric);
    std::map<std::string, std::pair<double, std::string>> getAlerts() const;
    
    void logToConsole(bool enable);
    void logToFile(const std::string& filepath, bool enable);
    void enableMetricsServer(bool enable, int port = 8080);
};

// === MAIN ADVANCED NLM CLASS ===

/**
 * Advanced NLM framework with all features enabled
 */
class AdvancedNLM {
private:
    struct Impl;
    Impl* pImpl;

public:
    AdvancedNLM();
    ~AdvancedNLM();
    
    void initialize(const std::map<std::string, std::string>& config = {});
    void reset();
    void finalize();
    
    void step(SimulationStep step, Timestamp time = 0.0);
    void run(size_t steps, double timestep = 0.001);
    
    // Configuration
    void setNeuronCount(size_t count);
    void setRegionCount(size_t count);
    void setConnectionProbability(double probability);
    void setRandomSeed(uint64_t seed);
    void setSimulationTimestep(double timestep);
    void setCheckpointDirectory(const std::string& directory);
    
    // Neuron management
    std::vector<NeuronId> createNeurons(const std::string& type, size_t count);
    void configureNeuron(NeuronId id, const NeuronConfig& config);
    NeuronConfig getNeuronConfig(NeuronId id) const;
    
    // Plasticity management
    void setPlasticityRule(const std::string& name, std::unique_ptr<PlasticityRule> rule);
    void enablePlasticity(const std::string& name, bool enable);
    bool isPlasticityEnabled(const std::string& name) const;
    
    // Integration methods
    void setIntegrationMethod(const std::string& method);
    std::string getIntegrationMethod() const;
    void setCustomTimestepScheduler(std::unique_ptr<CustomTimestepScheduler> scheduler);
    
    // Advanced learning
    void setAdvancedLearningAlgorithm(const std::string& name);
    void configureAdvancedLearning(const std::map<std::string, double>& params);
    
    // Visualization
    void startVisualization();
    void stopVisualization();
    void saveVisualization(const std::string& filepath);
    
    // Export/import
    bool saveCheckpoint(const std::string& name);
    bool loadCheckpoint(const std::string& name);
    bool exportToFile(const std::string& filepath, const std::string& format = "json");
    
    // Parameter optimization
    std::map<std::string, double> optimizeParameters(
        std::function<double(const std::map<std::string, double>&)> objectiveFunction);
    
    // Memory management
    void updateMemoryManager(SimulationStep step);
    void triggerGC();
    double getMemoryUsage() const;
    
    // Monitoring
    std::map<std::string, double> getCurrentMetrics() const;
    std::string getStatusSummary() const;
    
    // Access to brain
    std::shared_ptr<Brain> getBrain() const;
    std::shared_ptr<CustomNeuronRegistry> getNeuronRegistry() const;
    std::shared_ptr<PlasticityRuleRegistry> getPlasticityRuleRegistry() const;
    std::shared_ptr<ConfigurationTemplates> getConfigurationTemplates() const;
    std::shared_ptr<MemoryMonitor> getMemoryMonitor() const;
    std::shared_ptr<MemoryManager> getMemoryManager() const;
    std::shared_ptr<ParallelProcessor> getParallelProcessor() const;
    std::shared_ptr<EventDrivenSimulator> getEventDrivenSimulator() const;
    std::shared_ptr<NeuronActivityPlotter> getNeuronActivityPlotter() const;
    std::shared_ptr<NetworkTopologyVisualizer> getNetworkTopologyVisualizer() const;
    std::shared_ptr<LearningCurveAnalyzer> getLearningCurveAnalyzer() const;
    std::shared_ptr<StatisticalVisualizer> getStatisticalVisualizer() const;
    std::shared_ptr<IncrementalCheckpointManager> getIncrementalCheckpointManager() const;
    std::shared_ptr<BrainExporter> getBrainExporter() const;
    std::shared_ptr<DeltaCompressor> getDeltaCompressor() const;
    std::shared_ptr<BayesianOptimizer> getBayesianOptimizer() const;
    std::shared_ptr<SmartGarbageCollector> getSmartGarbageCollector() const;
    std::shared_ptr<AdvancedMemoryPool> getAdvancedMemoryPool() const;
};

// === FACTORY FUNCTION FOR ADVANCED NLM ===

/**
 * Create an advanced NLM instance with all features enabled
 */
inline std::shared_ptr<AdvancedNLM> createAdvancedNLM() {
    return std::make_shared<AdvancedNLM>();
}

/**
 * Create an advanced NLM instance with specific configuration
 */
inline std::shared_ptr<AdvancedNLM> createAdvancedNLM(const std::map<std::string, std::string>& config) {
    auto nlm = std::make_shared<AdvancedNLM>();
    nlm->initialize(config);
    return nlm;
}

} // namespace nlm

#endif // ADVANCED_NLM_H
