// Advanced NLM Framework - Implementation

#include "AdvancedNLM.hpp"
#include "src/core/Random/Random.hpp"
#include <random>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// === CustomNeuronRegistry Implementation ===

void CustomNeuronRegistry::registerNeuronType(const std::string& name, CustomNeuronFactory factory) {
    std::lock_guard<std::mutex> lock(mutex_);
    factories_[name] = std::move(factory);
}

void CustomNeuronRegistry::registerConfig(const std::string& name, const NeuronConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    configs_[name] = config;
}

std::unique_ptr<Neuron> CustomNeuronRegistry::createNeuron(const std::string& name, NeuronId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = factories_.find(name);
    if (it == factories_.end()) {
        throw std::runtime_error("Unknown neuron type: " + name);
    }
    return it->second(id);
}

NeuronConfig CustomNeuronRegistry::getConfig(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = configs_.find(name);
    if (it == configs_.end()) {
        throw std::runtime_error("Unknown neuron configuration: " + name);
    }
    return it->second;
}

bool CustomNeuronRegistry::exists(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return factories_.find(name) != factories_.end();
}

std::vector<std::string> CustomNeuronRegistry::getNeuronTypes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> types;
    types.reserve(factories_.size());
    for (const auto& pair : factories_) {
        types.push_back(pair.first);
    }
    return types;
}

// === IntegrationMethodSelector Implementation ===

std::string IntegrationMethodSelector::methodToString(Method method) {
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

IntegrationMethodSelector::Method IntegrationMethodSelector::stringToMethod(const std::string& methodString) {
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

// === PlasticityRuleRegistry Implementation ===

PlasticityRuleRegistry::PlasticityRuleRegistry() {
    // Register built-in plasticity rules
    registerRule("stdp", []() { return std::make_unique<STDP>(); });
    registerRule("hebbian", []() { return std::make_unique<Hebbian>(); });
    registerRule("reward_modulated_stdp", []() { return std::make_unique<RewardModulatedSTDP>(); });
    registerRule("bcn", []() { return std::make_unique<BCMRule>(); });
    registerRule("homeostatic", []() { return std::make_unique<HomeostaticPlasticity>(); });
    registerRule("predictive_coding", []() { return std::make_unique<PredictiveCodingRule>(); });
}

void PlasticityRuleRegistry::registerRule(const std::string& name, std::function<std::unique_ptr<PlasticityRule>()> factory) {
    std::lock_guard<std::mutex> lock(mutex_);
    ruleFactories_[name] = std::move(factory);
}

std::unique_ptr<PlasticityRule> PlasticityRuleRegistry::createRule(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ruleFactories_.find(name);
    if (it == ruleFactories_.end()) {
        throw std::runtime_error("Unknown plasticity rule: " + name);
    }
    return it->second();
}

bool PlasticityRuleRegistry::exists(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return ruleFactories_.find(name) != ruleFactories_.end();
}

std::vector<std::string> PlasticityRuleRegistry::getRuleNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(ruleFactories_.size());
    for (const auto& pair : ruleFactories_) {
        names.push_back(pair.first);
    }
    return names;
}

// === RewardModulatedSTDP Implementation ===

RewardModulatedSTDP::RewardModulatedSTDP(float learningRate, float rewardFactor)
    : pImpl(new Impl) {
    pImpl->learningRate = learningRate;
    pImpl->rewardFactor = rewardFactor;
    pImpl->timeConstant = 20.0f;
    pImpl->rewardGain = rewardFactor;
}

RewardModulatedSTDP::~RewardModulatedSTDP() {
    delete pImpl;
}

void RewardModulatedSTDP::update(Synapse* synapse, 
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalReward = 0.0f;
    for (Timestamp spikeTime : postSpikes) {
        totalReward += pImpl->rewardFactor;
    }
    
    float weightChange = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {  // Post after pre
                float weightChangePrePost = pImpl->learningRate * 
                    std::exp(-dt / pImpl->timeConstant);
                weightChangePrePost *= totalReward;
                weightChange += weightChangePrePost;
            } else {  // Pre after post (should be rare with proper spike times)
                float weightChangePostPre = -pImpl->learningRate * 
                    std::exp(dt / pImpl->timeConstant);
                weightChangePostPre *= totalReward;
                weightChange += weightChangePostPre;
            }
        }
    }
    
    applyWeightChange(synapse, weightChange);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
    synapse->setEligibilityTrace(delta * pImpl->rewardGain);
}

const char* RewardModulatedSTDP::getName() const {
    return "Reward-Modulated STDP";
}

void RewardModulatedSTDP::setRewardGain(float gain) {
    pImpl->rewardGain = std::max(0.0f, gain);
}

float RewardModulatedSTDP::getRewardGain() const {
    return pImpl->rewardGain;
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(1.0f, tau);
}

float RewardModulatedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void RewardModulatedSTDP::configure(float rewardGain, float learningRate, float tau) {
    setRewardGain(rewardGain);
    setLearningRate(learningRate);
    setTimeConstant(tau);
}

// === BCMRule Implementation ===

BCMRule::BCMRule(float theta, float tau, float beta)
    : pImpl(new Impl) {
    pImpl->theta = theta;
    pImpl->tau = tau;
    pImpl->beta = beta;
}

BCMRule::~BCMRule() {
    delete pImpl;
}

void BCMRule::update(Synapse* synapse, 
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float postActivity = static_cast<float>(postSpikes.size()) / dt;
    float newTheta = pImpl->theta + pImpl->beta * (postActivity - pImpl->theta);
    
    float weightChange = pImpl->beta * postActivity * (newTheta - synapse->getWeight());
    
    applyWeightChange(synapse, weightChange);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM Rule";
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::max(0.0f, theta);
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setTau(float tau) {
    pImpl->tau = std::max(1.0f, tau);
}

float BCMRule::getTau() const {
    return pImpl->tau;
}

void BCMRule::setBeta(float beta) {
    pImpl->beta = std::max(0.0f, beta);
}

float BCMRule::getBeta() const {
    return pImpl->beta;
}

// === HomeostaticPlasticity Implementation ===

HomeostaticPlasticity::HomeostaticPlasticity(float targetRate, float timeConstant, float maxRateChange)
    : pImpl(new Impl) {
    pImpl->targetRate = targetRate;
    pImpl->timeConstant = timeConstant;
    pImpl->maxRateChange = maxRateChange;
    pImpl->currentRate = targetRate;
    pImpl->deviation = 0.0f;
}

HomeostaticPlasticity::~HomeostaticPlasticity() {
    delete pImpl;
}

void HomeostaticPlasticity::update(Synapse* synapse, 
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    if (!synapse || dt <= 0.0f) {
        return;
    }
    
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    pImpl->deviation = (postRate - pImpl->targetRate) / pImpl->targetRate;
    
    float scaleFactor = 1.0f + pImpl->maxRateChange * pImpl->deviation * dt / pImpl->timeConstant;
    
    float weightChange = synapse->getWeight() * (scaleFactor - 1.0f);
    
    applyWeightChange(synapse, weightChange);
}

void HomeostaticPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* HomeostaticPlasticity::getName() const {
    return "Homeostatic Plasticity";
}

void HomeostaticPlasticity::setTargetRate(float rate) {
    pImpl->targetRate = std::max(0.0f, rate);
}

float HomeostaticPlasticity::getTargetRate() const {
    return pImpl->targetRate;
}

void HomeostaticPlasticity::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(1.0f, tau);
}

float HomeostaticPlasticity::getTimeConstant() const {
    return pImpl->timeConstant;
}

void HomeostaticPlasticity::setMaxRateChange(float change) {
    pImpl->maxRateChange = std::max(0.0f, change);
}

float HomeostaticPlasticity::getMaxRateChange() const {
    return pImpl->maxRateChange;
}

// === PredictiveCodingRule Implementation ===

PredictiveCodingRule::PredictiveCodingRule(float predictionErrorGain, float learningRate, int predictionHorizon)
    : pImpl(new Impl) {
    pImpl->predictionErrorGain = predictionErrorGain;
    pImpl->learningRate = learningRate;
    pImpl->predictionHorizon = predictionHorizon;
}

PredictiveCodingRule::~PredictiveCodingRule() {
    delete pImpl;
}

void PredictiveCodingRule::update(Synapse* synapse, 
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float predictionError = calculatePredictionError(preSpikes, postSpikes, dt);
    
    float weightChange = pImpl->learningRate * predictionError * pImpl->predictionErrorGain;
    
    applyWeightChange(synapse, weightChange);
}

void PredictiveCodingRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* PredictiveCodingRule::getName() const {
    return "Predictive Coding";
}

void PredictiveCodingRule::setPredictionErrorGain(float gain) {
    pImpl->predictionErrorGain = std::max(0.0f, gain);
}

float PredictiveCodingRule::getPredictionErrorGain() const {
    return pImpl->predictionErrorGain;
}

void PredictiveCodingRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

float PredictiveCodingRule::getLearningRate() const {
    return pImpl->learningRate;
}

void PredictiveCodingRule::setPredictionHorizon(int horizon) {
    pImpl->predictionHorizon = std::max(1, horizon);
}

int PredictiveCodingRule::getPredictionHorizon() const {
    return pImpl->predictionHorizon;
}

float PredictiveCodingRule::calculatePredictionError(const std::vector<Timestamp>& preSpikes,
                                                   const std::vector<Timestamp>& postSpikes,
                                                   TimestepDuration dt) {
    if (postSpikes.empty()) {
        return 0.0f;
    }
    
    float meanPostTime = 0.0f;
    for (Timestamp time : postSpikes) {
        meanPostTime += static_cast<float>(time);
    }
    meanPostTime /= static_cast<float>(postSpikes.size());
    
    float expectedPostTime = meanPostTime + pImpl->predictionHorizon * dt;
    
    float actualPostTime = 0.0f;
    for (Timestamp time : postSpikes) {
        actualPostTime += static_cast<float>(time);
    }
    actualPostTime /= static_cast<float>(postSpikes.size());
    
    return actualPostTime - expectedPostTime;
}

// === ConfigurationTemplates Implementation ===

ConfigurationTemplates::ConfigurationTemplates() {
    // Initialize with default templates
    templates_["fast_learning"] = {
        {"neuron_count", "5000"},
        {"plasticity.stdp.enabled", "true"},
        {"plasticity.stdp.learning_rate", "0.05"},
        {"plasticity.hebbian.enabled", "false"},
        {"plasticity.reward_modulated.enabled", "true"},
        {"plasticity.reward_modulated.gain", "0.2"},
        {"development.critical_period", "50"},
        {"development.maturation_rate", "0.02"},
        {"simulation.timestep", "0.001"},
        {"visualization.enabled", "true"},
        {"export.format", "json"}
    };
    
    templates_["stable_learning"] = {
        {"neuron_count", "2000"},
        {"plasticity.stdp.enabled", "true"},
        {"plasticity.stdp.learning_rate", "0.01"},
        {"plasticity.hebbian.enabled", "true"},
        {"plasticity.reward_modulated.enabled", "false"},
        {"development.critical_period", "200"},
        {"development.maturation_rate", "0.005"},
        {"simulation.timestep", "0.005"},
        {"visualization.enabled", "true"},
        {"export.format", "binary"}
    };
    
    templates_["balanced"] = {
        {"neuron_count", "3000"},
        {"plasticity.stdp.enabled", "true"},
        {"plasticity.stdp.learning_rate", "0.02"},
        {"plasticity.hebbian.enabled", "true"},
        {"plasticity.reward_modulated.enabled", "true"},
        {"plasticity.reward_modulated.gain", "0.1"},
        {"plasticity.bcm.enabled", "true"},
        {"development.critical_period", "100"},
        {"development.maturation_rate", "0.01"},
        {"simulation.timestep", "0.002"},
        {"visualization.enabled", "true"},
        {"export.format", "json"}
    };
    
    templates_["exploratory"] = {
        {"neuron_count", "10000"},
        {"plasticity.stdp.enabled", "true"},
        {"plasticity.stdp.learning_rate", "0.03"},
        {"plasticity.hebbian.enabled", "true"},
        {"plasticity.reward_modulated.enabled", "true"},
        {"plasticity.reward_modulated.gain", "0.5"},
        {"plasticity.homeostatic.enabled", "true"},
        {"plasticity.predictive_coding.enabled", "true"},
        {"development.critical_period", "25"},
        {"development.maturation_rate", "0.05"},
        {"simulation.timestep", "0.0005"},
        {"visualization.enabled", "true"},
        {"visualization.real_time", "true"},
        {"export.format", "compressed"},
        {"export.compression_level", "best"}
    };
    
    templates_["exploitative"] = {
        {"neuron_count", "15000"},
        {"plasticity.stdp.enabled", "true"},
        {"plasticity.stdp.learning_rate", "0.04"},
        {"plasticity.hebbian.enabled", "true"},
        {"plasticity.reward_modulated.enabled", "true"},
        {"plasticity.reward_modulated.gain", "0.8"},
        {"plasticity.bcm.enabled", "true"},
        {"plasticity.homeostatic.enabled", "true"},
        {"plasticity.predictive_coding.enabled", "true"},
        {"development.critical_period", "10"},
        {"development.maturation_rate", "0.08"},
        {"simulation.timestep", "0.0001"},
        {"visualization.enabled", "true"},
        {"visualization.real_time", "true"},
        {"visualization.network_topology", "true"},
        {"export.format", "differential"},
        {"export.checkpoint_interval", "100"},
        {"memory.gc.enabled", "true"},
        {"memory.pools.enabled", "true"},
        {"parallel.processing.enabled", "true"},
        {"parallel.neuron_update", "true"},
        {"parallel.spike_propagation", "true"}
    };
}

ConfigurationTemplates::~ConfigurationTemplates() {
}

bool ConfigurationTemplates::addTemplate(const std::string& name, const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (templates_.find(name) != templates_.end()) {
        return false; // Template already exists
    }
    templates_[name] = config;
    return true;
}

bool ConfigurationTemplates::removeTemplate(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = templates_.find(name);
    if (it == templates_.end()) {
        return false; // Template not found
    }
    templates_.erase(it);
    return true;
}

bool ConfigurationTemplates::hasTemplate(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return templates_.find(name) != templates_.end();
}

std::map<std::string, std::string> ConfigurationTemplates::getTemplate(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = templates_.find(name);
    if (it == templates_.end()) {
        throw std::runtime_error("Unknown template: " + name);
    }
    return it->second;
}

std::vector<std::string> ConfigurationTemplates::getTemplateNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(templates_.size());
    for (const auto& pair : templates_) {
        names.push_back(pair.first);
    }
    return names;
}

std::map<std::string, std::string> ConfigurationTemplates::getFastLearningTemplate() const {
    return getTemplate("fast_learning");
}

std::map<std::string, std::string> ConfigurationTemplates::getStableLearningTemplate() const {
    return getTemplate("stable_learning");
}

std::map<std::string, std::string> ConfigurationTemplates::getBalancedTemplate() const {
    return getTemplate("balanced");
}

std::map<std::string, std::string> ConfigurationTemplates::getExploratoryTemplate() const {
    return getTemplate("exploratory");
}

std::map<std::string, std::string> ConfigurationTemplates::getExploitativeTemplate() const {
    return getTemplate("exploitative");
}

void ConfigurationTemplates::saveTemplate(const std::string& name, const std::map<std::string, std::string>& config) {
    addTemplate(name, config);
}

} // namespace nlm
