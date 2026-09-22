#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "IntegrationAbstractions.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <span>
#include <optional>
#include <string_view>
#include <chrono>
#include <expected>
#include <compare>
#include <concepts>
#include <numeric>
#include <algorithm>

namespace nlm {

// C++20: Concepts for better template constraints
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<typename T>
concept PointerToNeuron = std::is_pointer_v<T> && 
                         std::is_convertible_v<T, Neuron*>;

// C++20: Type aliases for common patterns
template<typename T>
using Result = std::expected<T, std::string>;

template<typename T>
using View = std::span<T>;

// C++20: In-place new type for perfect forwarding
template<typename T>
using InPlace = std::in_place_t;

// C++20: Union type for discriminated unions
template<typename... Types>
using Variant = std::variant<Types...>;

// Forward declarations for better encapsulation
class IntegratedSensoryProcessor;
class Neuron;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // C++20: Use structured binding in constructor initializer list
    explicit AgentBrain(std::shared_ptr<Brain> brain)
        : brain_(std::move(brain))
        , dopamineLevel_(0.0f), noveltyLevel_(0.0f), curiosityLevel_(0.0f)
        , predictionError_(0.0f), expectedReward_(0.0f), developmentalAge_(0.0)
        , plasticityModifier_(1.0f), rewardModulationEnabled_(true)
        , structuralPlasticityEnabled_(true), developmentEnabled_(true)
        , curiosityEnabled_(true), sensoryNoveltyDecay_(0.99f)
        , motorController_(nullptr), neuralIntegrator_(nullptr) {};
    
    ~AgentBrain() = default;
    
    // Delete copy, enable move for efficiency
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept = default;
    AgentBrain& operator=(AgentBrain&&) noexcept = default;
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    [[nodiscard]] size_t getSensoryInputSize() const noexcept;
    
    // Get motor output size expected
    [[nodiscard]] size_t getMotorOutputSize() const noexcept;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept) noexcept;
    
    // Decode brain motor activity into motor command
    [[nodiscard]] MotorCommand decodeMotorCommand() noexcept;
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward) noexcept;
    
    // Update development system
    void updateDevelopment(double timestep) noexcept;
    
    // Get current developmental stage
    [[nodiscard]] DevelopmentalStage getDevelopmentalStage() const noexcept;
    
    // Get neuromodulation level
    [[nodiscard]] float getNeuromodulationLevel() const noexcept { return dopamineLevel_; }
    
    // Get curiosity level
    [[nodiscard]] float getCuriosityLevel() const noexcept { return curiosityLevel_; }
    
    // Get novelty level
    [[nodiscard]] float getNoveltyLevel() const noexcept { return noveltyLevel_; }
    
    // Get prediction error
    [[nodiscard]] float getPredictionError() const noexcept { return predictionError_; }
    
    // Reset agent for new episode
    void reset() noexcept;
    
    // Get brain pointer (unsafe - use with caution)
    [[nodiscard]] Brain* getBrain() noexcept { return brain_.get(); }
    [[nodiscard]] const Brain* getBrain() const noexcept { return brain_.get(); }
    
    // Configuration methods with better const correctness
    void enableRewardModulation(bool enable) noexcept { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) noexcept { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) noexcept { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) noexcept { curiosityEnabled_ = enable; }
    
    [[nodiscard]] bool isRewardModulationEnabled() const noexcept { return rewardModulationEnabled_; }
    [[nodiscard]] bool isStructuralPlasticityEnabled() const noexcept { return structuralPlasticityEnabled_; }
    [[nodiscard]] bool isDevelopmentEnabled() const noexcept { return developmentEnabled_; }
    [[nodiscard]] bool isCuriosityEnabled() const noexcept { return curiosityEnabled_; }
    
    // Integration abstractions access
    void configureWithIntegratedSystems();
    
    [[nodiscard]] const IntegratedMotorController* getMotorController() const noexcept { return motorController_.get(); }
    [[nodiscard]] const IntegratedNeuralIntegrator* getNeuralIntegrator() const noexcept { return neuralIntegrator_.get(); }
    
    // Process sensory input using integrated processor
    void processSensoryInput(const AgentBody& agent, const SimpleWorld& world) noexcept;
    
    // Update integrated systems
    void updateIntegratedSystems(double timestep) noexcept;
    
    // Get neural integration statistics
    void getIntegrationStatistics(std::map<std::string, float>& stats) const noexcept;
    
    // C++20: Three-way comparison operator for sorting
    auto operator<=>(const AgentBrain&) const = default;
    
    // C++20: Equality comparison
    bool operator==(const AgentBrain&) const = default;
    
    // Configuration struct for better API organization
    struct Configuration {
        bool rewardModulation = true;
        bool structuralPlasticity = true;
        bool development = true;
        bool curiosity = true;
        
        [[nodiscard]] bool allEnabled() const noexcept {
            return rewardModulation && structuralPlasticity && development && curiosity;
        }
    };
    
    // Set configuration from struct
    void configure(const Configuration& config) noexcept {
        rewardModulationEnabled_ = config.rewardModulation;
        structuralPlasticityEnabled_ = config.structuralPlasticity;
        developmentEnabled_ = config.development;
        curiosityEnabled_ = config.curiosity;
    }
    
    // C++20: Factory method for creating brain with common configurations
    static Result<std::unique_ptr<AgentBrain>> createStandardBrain(const std::string& configFile = "");
    
    // C++20: Visitor pattern for brain operations
    template<typename Visitor>
    void visit(Visitor&& visitor) & {
        visitor.visitSensorySystems(*this);
        visitor.visitMotorSystems(*this);
        visitor.visitDevelopment(*this);
    }
    
private:
    // Motor decoding: convert neural activity to motor command
    [[nodiscard]] MotorCommand decodeFromMotorNeurons() noexcept;
    
    // Motor command selection with curiosity/exploration
    [[nodiscard]] MotorCommand selectWithCuriosity(MotorCommand defaultCmd) noexcept;
    
    std::shared_ptr<Brain> brain_;
    
    // Integration components
    std::unique_ptr<IntegratedMotorController> motorController_;
    std::unique_ptr<IntegratedNeuralIntegrator> neuralIntegrator_;
    
    // Motor neuron groups - using std::span for better memory safety
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    std::vector<Neuron*> motorNeurons_;  // All motor neurons collected for integration
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    std::vector<Neuron*> sensoryNeurons_;  // All sensory neurons collected for integration
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
