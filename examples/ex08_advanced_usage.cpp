// ex08_advanced_usage.cpp - Advanced NLM Usage Examples
// Demonstrates advanced NLM capabilities and integration patterns

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/experiments/Experiment.hpp"
#include "src/environment/Environment.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/visualization/VisualizationInterface.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>

// Custom environment for complex scenarios
class ComplexEnvironment : public nlm::Environment {
public:
    struct Percept {
        std::vector<float> sensoryData;
        float reward;
        double timestamp;
        bool terminal;
    };
    
    ComplexEnvironment() {
        // Initialize complex environment state
        reset();
        setupScenario();
    }
    
    void setupScenario() {
        // Create a complex multi-dimensional environment
        dimensions_ = {
            {"vision", 20},           // Visual sensor array
            {"touch", 10},            // Tactile sensors
            {"proprioception", 6},    // Body position sensors
            {"interoception", 4},     // Internal state sensors
            {"audition", 8},          // Auditory sensors
        };
        
        // Initialize world state
        position_ = {5.0f, 5.0f, 2.0f};
        velocity_ = {0.0f, 0.0f, 0.0f};
        orientation_ = 0.0f;
        
        // Create obstacles
        obstacles_ = {
            {{1.0, 1.0}, {3.0, 3.0}},
            {{8.0, 1.0}, {9.0, 8.0}},
            {{2.0, 7.0}, {4.0, 9.0}},
        };
        
        // Define goals
        goals_ = {
            {"reach_target", {9.0f, 9.0f, 1.5f}, 100.0f},
            {"avoid_obstacle", {0.0f, 0.0f, 1.0f}, 50.0f},
            {"maintain_stability", position_, 10.0f},
        };
    }
    
    std::vector<float> getSensoryInput() const override {
        std::vector<float> input;
        
        // Vision sensors (ray casting simulation)
        for (const auto& ray : castRays()) {
            input.push_back(ray.intensity);
            input.push_back(ray.distance);
            input.push_back(ray.color[0]);
            input.push_back(ray.color[1]);
            input.push_back(ray.color[2]);
        }
        
        // Touch sensors
        auto touchData = getTouchData();
        input.insert(input.end(), touchData.begin(), touchData.end());
        
        // Proprioception
        auto proprioData = getProprioceptionData();
        input.insert(input.end(), proprioData.begin(), proprioData.end());
        
        // Interoception
        auto interoData = getInteroceptionData();
        input.insert(input.end(), interoData.begin(), interoData.end());
        
        // Auditory sensors
        auto audioData = getAudioData();
        input.insert(input.end(), audioData.begin(), audioData.end());
        
        // Environmental context
        input.push_back(getTemperature());
        input.push_back(getHumidity());
        input.push_back(getLighting());
        
        return input;
    }
    
    void applyAction(const nlm::MotorCommand& action) override {
        // Complex action processing
        processMovement(action);
        processInteraction(action);
        applyPhysicalLaws();
        
        // Update sensory processing
        updateSensorySystems();
        
        // Calculate new state
        updateWorldState();
        
        // Check termination conditions
        checkTermination();
        
        // Calculate reward
        calculateReward();
    }
    
    float getReward() const override {
        return currentReward_;
    }
    
    bool isTerminal() const override {
        return episodeComplete_;
    }
    
    void reset() override {
        // Reset to initial state
        position_ = {5.0f, 5.0f, 2.0f};
        velocity_ = {0.0f, 0.0f, 0.0f};
        orientation_ = 0.0f;
        
        // Reset episode variables
        steps_ = 0;
        episodeComplete_ = false;
        currentReward_ = 0.0f;
        
        // Reset sensory systems
        resetSensorySystems();
        
        // Reset neural components
        resetNeuralState();
    }
    
private:
    struct Ray {
        float intensity;
        float distance;
        std::vector<float> color;
        nlm::Vector3 intersectionPoint;
    };
    
    struct Obstacle {
        nlm::Vector2 minCorner;
        nlm::Vector2 maxCorner;
        std::vector<float> materialProperties;
    };
    
    struct Goal {
        std::string type;
        nlm::Vector3 targetPosition;
        float value;
    };
    
    // Environment state
    std::unordered_map<std::string, size_t> dimensions_;
    nlm::Vector3 position_;
    nlm::Vector3 velocity_;
    float orientation_;
    
    std::vector<Obstacle> obstacles_;
    std::vector<Goal> goals_;
    
    // Episode state
    int steps_;
    bool episodeComplete_;
    float currentReward_;
    
    // Physical simulation
    float mass_ = 1.0f;
    float friction_ = 0.1f;
    float gravity_ = 9.81f;
    float timeStep_ = 0.01f;
    
    // Sensory system states
    std::vector<float> visionBuffer_;
    std::vector<float> touchBuffer_;
    std::vector<float> proprioceptionBuffer_;
    std::vector<float> interoceptionBuffer_;
    std::vector<float> audioBuffer_;
    
    // Simulation parameters
    float temperature_ = 20.0f;
    float humidity_ = 50.0f;
    float lighting_ = 100.0f;
    
    // Complex interaction systems
    std::vector<std::function<void()>> actionProcessors_;
    std::vector<std::function<void()>> physicsHandlers_;
    
    std::vector<Ray> castRays() const {
        std::vector<Ray> rays;
        
        // Simulate multiple vision rays
        for (int i = 0; i < 20; ++i) {
            float angle = (i * M_PI / 10.0) - M_PI / 2;
            nlm::Vector3 direction(
                std::cos(angle),
                std::sin(angle),
                0.0f
            );
            
            Ray ray;
            ray.direction = direction;
            ray.intensity = castRayTrace(direction);
            ray.distance = std::hypot(direction.x, direction.y) * 10.0f;
            ray.color = {0.8f, 0.8f, 0.8f};
            
            rays.push_back(ray);
        }
        
        return rays;
    }
    
    float castRayTrace(const nlm::Vector3& direction) const {
        // Simple ray tracing algorithm
        float maxDistance = 100.0f;
        float distance = maxDistance;
        
        // Check intersection with obstacles
        for (const auto& obstacle : obstacles_) {
            // Ray-box intersection
            nlm::Vector2 dir2D(direction.x, direction.y);
            nlm::Vector2 minCorner = obstacle.minCorner;
            nlm::Vector2 maxCorner = obstacle.maxCorner;
            
            float tMin = -std::numeric_limits<float>::max();
            float tMax = std::numeric_limits<float>::max();
            
            for (int i = 0; i < 2; ++i) {
                if (std::abs(dir2D[i]) < 1e-6f) {
                    // Ray parallel to slab plane
                    if (position_[i] < minCorner[i] || position_[i] > maxCorner[i]) {
                        return 0.0f;
                    }
                } else {
                    float invDir = 1.0f / dir2D[i];
                    float t1 = (minCorner[i] - position_[i]) * invDir;
                    float t2 = (maxCorner[i] - position_[i]) * invDir;
                    
                    if (t1 > t2) std::swap(t1, t2);
                    
                    if (t1 > tMin) tMin = t1;
                    if (t2 < tMax) tMax = t2;
                    
                    if (tMin > tMax) return 0.0f;
                    if (tMax < 0.0f) return 0.0f;
                }
            }
            
            distance = std::min(distance, std::max(tMin, 0.0f));
        }
        
        // Attenuate based on distance
        return std::max(0.0f, 1.0f - distance / maxDistance);
    }
    
    std::vector<float> getTouchData() const {
        std::vector<float> touchData;
        
        // Simulate contact with surfaces
        for (const auto& obstacle : obstacles_) {
            // Check if character is inside obstacle
            if (position_.x >= obstacle.minCorner.x && position_.x <= obstacle.maxCorner.x &&
                position_.y >= obstacle.minCorner.y && position_.y <= obstacle.maxCorner.y) {
                // Calculate contact force
                float contactForce = calculateContactForce(position_, obstacle);
                touchData.push_back(contactForce);
                touchData.push_back(static_cast<float>(obstacle.materialProperties.size()));
            }
        }
        
        return touchData;
    }
    
    float calculateContactForce(const nlm::Vector3& pos, const Obstacle& obstacle) const {
        // Simplified physics-based contact calculation
        float penetrationDepth = 0.0f;
        
        if (pos.x < obstacle.minCorner.x) {
            penetrationDepth = obstacle.minCorner.x - pos.x;
        } else if (pos.x > obstacle.maxCorner.x) {
            penetrationDepth = pos.x - obstacle.maxCorner.x;
        }
        
        if (pos.y < obstacle.minCorner.y) {
            penetrationDepth = std::max(penetrationDepth, obstacle.minCorner.y - pos.y);
        } else if (pos.y > obstacle.maxCorner.y) {
            penetrationDepth = std::max(penetrationDepth, pos.y - obstacle.maxCorner.y);
        }
        
        return penetrationDepth * 10.0f;  // Force proportional to penetration
    }
    
    std::vector<float> getProprioceptionData() const {
        std::vector<float> proprioData;
        
        // Body position and orientation
        proprioData.push_back(position_.x);
        proprioData.push_back(position_.y);
        proprioData.push_back(position_.z);
        proprioData.push_back(std::cos(orientation_));
        proprioData.push_back(std::sin(orientation_));
        
        // Joint angles (simplified)
        proprioData.push_back(0.0f);  // Shoulder
        proprioData.push_back(0.0f);  // Elbow
        proprioData.push_back(0.0f);  // Hip
        proprioData.push_back(0.0f);  // Knee
        
        return proprioData;
    }
    
    std::vector<float> getInteroceptionData() const {
        std::vector<float> interoData;
        
        // Internal physiological states
        interoData.push_back(getEnergyLevel());
        interoData.push_back(getHungerLevel());
        interoData.push_back(getThirstLevel());
        interoData.push_back(getFatigueLevel());
        
        return interoData;
    }
    
    std::vector<float> getAudioData() const {
        std::vector<float> audioData;
        
        // Environmental sounds
        for (int i = 0; i < 8; ++i) {
            float frequency = 440.0f * (1.0f + (i - 3.5f) * 0.1f);  // Vary frequencies
            float amplitude = calculateSoundAmplitude(i);
            audioData.push_back(frequency);
            audioData.push_back(amplitude);
        }
        
        return audioData;
    }
    
    float calculateSoundAmplitude(int source) const {
        // Distance-based sound attenuation
        float distance = std::hypot(position_.x - 5.0f, position_.y - 5.0f);
        return std::max(0.0f, 1.0f - distance / 20.0f) * (0.5f + source * 0.1f);
    }
    
    float getTemperature() const {
        // Temperature based on environment
        return temperature_;
    }
    
    float getHumidity() const {
        // Humidity based on simulation
        return humidity_;
    }
    
    float getLighting() const {
        // Lighting based on position and time
        float timeOfDay = std::fmod(steps_ * timeStep_ / 3600.0f, 24.0f);
        if (timeOfDay < 6.0f || timeOfDay > 18.0f) {
            return 50.0f;  // Night lighting
        } else {
            return 500.0f; // Daytime lighting
        }
    }
    
    float getEnergyLevel() const {
        // Energy based on activity
        return std::max(0.0f, 100.0f - steps_ * 0.1f);
    }
    
    float getHungerLevel() const {
        // Hunger increases over time
        return std::min(100.0f, steps_ * 0.05f);
    }
    
    float getThirstLevel() const {
        // Thirst increases over time
        return std::min(100.0f, steps_ * 0.03f);
    }
    
    float getFatigueLevel() const {
        // Fatigue based on movement
        return std::min(100.0f, velocity_.length() * steps_ * 0.1f);
    }
    
    void processMovement(const nlm::MotorCommand& action) {
        // Apply physics-based movement
        nlm::Vector3 force;
        
        if (action.forward) force += nlm::Vector3(1.0f, 0.0f, 0.0f) * action.magnitude;
        if (action.backward) force -= nlm::Vector3(1.0f, 0.0f, 0.0f) * action.magnitude;
        if (action.turnLeft) orientation_ += action.magnitude * 0.5f;
        if (action.turnRight) orientation_ -= action.magnitude * 0.5f;
        
        // Apply forces
        velocity_ += force / mass_ * timeStep_;
        
        // Apply friction
        velocity_ *= (1.0f - friction_);
        
        // Apply gravity
        velocity_.z -= gravity_ * timeStep_;
        
        // Update position
        position_ += velocity_ * timeStep_;
        
        // Handle collisions with environment boundaries
        handleBoundaryCollision();
        
        // Handle collisions with obstacles
        handleObstacleCollision();
    }
    
    void processInteraction(const nlm::MotorCommand& action) {
        // Process interaction actions (look, grasp, etc.)
        if (action.interact) {
            interactWithEnvironment();
        }
        
        if (action.lookLeft) {
            // Look left - change vision focus
        }
        
        if (action.lookRight) {
            // Look right - change vision focus
        }
    }
    
    void interactWithEnvironment() {
        // Complex interaction logic
        // Pick up objects, open doors, etc.
        
        // Check for nearby interactive objects
        for (const auto& obstacle : obstacles_) {
            if (isObjectInteractable(position_, obstacle)) {
                // Perform interaction
                performInteraction(position_, obstacle);
                break;
            }
        }
    }
    
    bool isObjectInteractable(const nlm::Vector3& pos, const Obstacle& obstacle) const {
        // Check if position is within interaction range
        float distance = std::hypot(pos.x - (obstacle.minCorner.x + obstacle.maxCorner.x) / 2,
                                   pos.y - (obstacle.minCorner.y + obstacle.maxCorner.y) / 2);
        
        return distance < 2.0f && obstacle.materialProperties.size() > 0;
    }
    
    void performInteraction(const nlm::Vector3& pos, const Obstacle& obstacle) {
        // Perform interaction based on material properties
        // For example, open a door, pick up an object, etc.
        // Update environment state accordingly
    }
    
    void applyPhysicalLaws() {
        // Apply physics simulation
        // Integration using Verlet integration or similar
        
        // Update velocity based on forces
        velocity_ += (gravity_ * nlm::Vector3(0.0f, 0.0f, -1.0f) - friction_ * velocity_) * timeStep_;
        
        // Update position
        position_ += velocity_ * timeStep_;
        
        // Apply air resistance
        velocity_ *= 0.99f;
    }
    
    void handleBoundaryCollision() {
        // Handle collisions with environment boundaries
        float boundary = 10.0f;  // World size
        
        if (position_.x < -boundary) {
            position_.x = -boundary;
            velocity_.x = -velocity_.x * 0.5f;  // Bounce
        } else if (position_.x > boundary) {
            position_.x = boundary;
            velocity_.x = -velocity_.x * 0.5f;
        }
        
        if (position_.y < -boundary) {
            position_.y = -boundary;
            velocity_.y = -velocity_.y * 0.5f;
        } else if (position_.y > boundary) {
            position_.y = boundary;
            velocity_.y = -velocity_.y * 0.5f;
        }
        
        if (position_.z < 0.0f) {
            position_.z = 0.0f;
            velocity_.z = -velocity_.z * 0.7f;
        }
    }
    
    void handleObstacleCollision() {
        // Handle collisions with dynamic obstacles
        for (auto& obstacle : obstacles_) {
            // Check for collision
            if (isColliding(position_, velocity_, obstacle)) {
                // Resolve collision
                resolveCollision(position_, velocity_, obstacle);
            }
        }
    }
    
    bool isColliding(const nlm::Vector3& pos, const nlm::Vector3& vel, const Obstacle& obstacle) const {
        // Check if moving sphere collides with axis-aligned bounding box
        float radius = 0.5f;  // Character radius
        
        // Find closest point on box to circle center
        float closestX = std::clamp(pos.x, obstacle.minCorner.x, obstacle.maxCorner.x);
        float closestY = std::clamp(pos.y, obstacle.minCorner.y, obstacle.maxCorner.y);
        
        // Calculate distance
        float distance = std::sqrt(
            (pos.x - closestX) * (pos.x - closestX) +
            (pos.y - closestY) * (pos.y - closestY)
        );
        
        return distance < radius;
    }
    
    void resolveCollision(nlm::Vector3& pos, nlm::Vector3& vel, const Obstacle& obstacle) {
        // Simple collision response
        float radius = 0.5f;
        
        // Find collision normal
        nlm::Vector3 closestPoint(
            std::clamp(pos.x, obstacle.minCorner.x, obstacle.maxCorner.x),
            std::clamp(pos.y, obstacle.minCorner.y, obstacle.maxCorner.y),
            pos.z
        );
        
        nlm::Vector3 normal = normalize(pos - closestPoint);
        
        // Resolve collision
        float penetration = radius - (pos - closestPoint).length();
        pos += normal * penetration;
        
        // Reflect velocity
        vel = reflect(vel, normal) * 0.5f;
    }
    
    nlm::Vector3 normalize(const nlm::Vector3& v) const {
        float length = v.length();
        if (length < 1e-6f) return nlm::Vector3(0.0f, 0.0f, 0.0f);
        return v / length;
    }
    
    nlm::Vector3 reflect(const nlm::Vector3& v, const nlm::Vector3& normal) const {
        return v - 2.0f * dot(v, normal) * normal;
    }
    
    float dot(const nlm::Vector3& a, const nlm::Vector3& b) const {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    
    void updateSensorySystems() {
        // Update sensory buffers with new data
        updateVisionBuffer();
        updateTouchBuffer();
        updateProprioceptionBuffer();
        updateInteroceptionBuffer();
        updateAudioBuffer();
    }
    
    void updateVisionBuffer() {
        // Update vision buffer with new ray tracing data
        std::vector<float> newVision = castRays();
        visionBuffer_ = newVision;
    }
    
    void updateTouchBuffer() {
        // Update touch buffer
        std::vector<float> newTouch = getTouchData();
        touchBuffer_ = newTouch;
    }
    
    void updateProprioceptionBuffer() {
        // Update proprioception buffer
        std::vector<float> newProprio = getProprioceptionData();
        proprioceptionBuffer_ = newProprio;
    }
    
    void updateInteroceptionBuffer() {
        // Update interoception buffer
        std::vector<float> newIntero = getInteroceptionData();
        interoceptionBuffer_ = newIntero;
    }
    
    void updateAudioBuffer() {
        // Update audio buffer
        std::vector<float> newAudio = getAudioData();
        audioBuffer_ = newAudio;
    }
    
    void resetSensorySystems() {
        // Reset all sensory buffers
        visionBuffer_.clear();
        touchBuffer_.clear();
        proprioceptionBuffer_.clear();
        interoceptionBuffer_.clear();
        audioBuffer_.clear();
    }
    
    void resetNeuralState() {
        // Reset neural state for new episode
        // This would involve brain reset and initialization
    }
    
    void updateWorldState() {
        // Update any world-level state
        // Handle time progression, environmental changes, etc.
    }
    
    void checkTermination() {
        // Check if episode should end
        // Based on goals, time limits, etc.
        
        // Check goal completion
        checkGoalCompletion();
        
        // Check time limit
        if (steps_ >= 1000) {
            episodeComplete_ = true;
            return;
        }
        
        // Check other termination conditions
        if (position_.z < -1.0f) {
            episodeComplete_ = true;
            return;
        }
    }
    
    void checkGoalCompletion() {
        // Check if any goals have been completed
        for (const auto& goal : goals_) {
            if (checkGoal(goal)) {
                currentReward_ += goal.value;
                std::cout << "Goal completed: " << goal.type << " (reward: " << goal.value << ")" << std::endl;
            }
        }
    }
    
    bool checkGoal(const Goal& goal) const {
        if (goal.type == "reach_target") {
            return std::hypot(position_.x - goal.targetPosition.x,
                             position_.y - goal.targetPosition.y) < 1.0f;
        } else if (goal.type == "avoid_obstacle") {
            // Check if target is still safe
            for (const auto& obstacle : obstacles_) {
                if (isPositionInObstacle(goal.targetPosition, obstacle)) {
                    return false;
                }
            }
            return true;
        } else if (goal.type == "maintain_stability") {
            // Check if position is stable
            return velocity_.length() < 0.1f;
        }
        return false;
    }
    
    bool isPositionInObstacle(const nlm::Vector3& pos, const Obstacle& obstacle) const {
        return pos.x >= obstacle.minCorner.x && pos.x <= obstacle.maxCorner.x &&
               pos.y >= obstacle.minCorner.y && pos.y <= obstacle.maxCorner.y;
    }
    
    void calculateReward() {
        // Calculate reward based on current state
        currentReward_ = 0.0f;
        
        // Distance-based reward
        float distanceToTarget = calculateDistanceToTarget();
        currentReward_ += std::max(0.0f, 100.0f - distanceToTarget);
        
        // Velocity-based reward (encourage movement)
        currentReward_ += velocity_.length() * 10.0f;
        
        // Goal completion bonus
        for (const auto& goal : goals_) {
            if (checkGoal(goal)) {
                currentReward_ += goal.value * 2.0f;  // Bonus for completed goals
            }
        }
        
        // Penalty for collision with obstacles
        for (const auto& obstacle : obstacles_) {
            if (isPositionInObstacle(position_, obstacle)) {
                currentReward_ -= 50.0f;
            }
        }
        
        // Environmental penalties
        if (getEnergyLevel() < 20.0f) {
            currentReward_ -= 10.0f;  // Rest penalty
        }
    }
    
    float calculateDistanceToTarget() const {
        nlm::Vector3 target = goals_[0].targetPosition;
        return std::sqrt(std::pow(position_.x - target.x, 2) +
                        std::pow(position_.y - target.y, 2) +
                        std::pow(position_.z - target.z, 2));
    }
};

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== Advanced NLM Usage Example ===" << std::endl;
    std::cout << "Demonstrating complex NLM integration with custom environment" << std::endl;
    
    // Create configuration for advanced simulation
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 1000);
    config->set("brain.learningRate", 0.005);
    config->set("brain.simulation.maxSteps", 5000);
    
    // Enable all advanced features
    config->set("brain.memory.workingMemory.enabled", true);
    config->set("brain.memory.workingMemory.capacity", 500);
    config->set("brain.memory.episodicMemory.enabled", true);
    config->set("brain.memory.episodicMemory.maxEpisodes", 1000);
    
    config->set("brain.neuromodulation.dopamine.enabled", true);
    config->set("brain.neuromodulation.curiosity.enabled", true);
    config->set("brain.neuromodulation.novelty.enabled", true);
    
    config->set("brain.development.enabled", true);
    config->set("brain.development.plasticityDecayRate", 0.02);
    
    config->set("brain.prediction.enabled", true);
    config->set("brain.prediction.errorSignal.enabled", true);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Advanced brain initialized successfully!" << std::endl;
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Memory systems enabled" << std::endl;
    std::cout << "  Neuromodulation enabled" << std::endl;
    std::cout << "  Prediction system enabled" << std::endl;
    
    // Create complex environment
    ComplexEnvironment environment;
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(environment);
    
    std::cout << "✓ Complex environment initialized!" << std::endl;
    std::cout << "  Environment dimensions: vision=" << environment.getSensoryInput().size() 
              << " touch sensors, proprioception sensors" << std::endl;
    
    // Create experiment to track performance
    auto experiment = std::make_shared<nlm::Experiment>("Advanced Usage", brain);
    
    std::cout << "\n=== Advanced Simulation Started ===" << std::endl;
    std::cout << "Running complex environment with custom physics, perception, and interaction" << std::endl;
    
    // Main simulation loop
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        // Get sensory input from environment
        auto sensoryInput = environment.getSensoryInput();
        auto percept = nlm::SensoryPercept(sensoryInput);
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Step brain
        brain->step(step);
        
        // Get motor output
        auto action = agent->decodeMotorCommand();
        
        // Apply action to environment
        environment.applyAction(action);
        
        // Apply neuromodulation based on reward
        float reward = environment.getReward();
        float predictionError = 0.0f; // Would come from prediction system
        agent->applyRewardModulation(reward, predictionError);
        
        // Log progress every 50 steps
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                      << "Position: (" << environment.getPosition().x << ", " 
                      << environment.getPosition().y << ", " << environment.getPosition().z << "), "
                      << "Velocity: (" << environment.getVelocity().x << ", " 
                      << environment.getVelocity().y << ", " << environment.getVelocity().z << "), "
                      << "Reward: " << reward << std::endl;
        }
        
        if (environment.isTerminal()) {
            std::cout << "Episode completed at step " << step << " with reward " << reward << std::endl;
            environment.reset();
            agent->reset();
            brain->reset();
            experiment->recordTrial(step, reward);
            break;
        }
    }
    
    // Save final brain state
    brain->save("advanced_usage_checkpoint.brain");
    std::cout << "\n✓ Final brain state saved to 'advanced_usage_checkpoint.brain'" << std::endl;
    
    // Get experiment results
    auto results = experiment->getResults();
    std::cout << "\n=== Advanced Simulation Complete ===" << std::endl;
    std::cout << "Advanced simulation completed successfully!" << std::endl;
    std::cout << "Average reward per step: " << results.averageReward << std::endl;
    std::cout << "Total trials completed: " << results.trialsCompleted << std::endl;
    std::cout << "Total simulation time: " << results.totalTime << " seconds" << std::endl;
    
    // Performance analysis
    std::cout << "\n=== Performance Analysis ===" << std::endl;
    std::cout << "Experience gained: Advanced environment interaction, physics simulation, "
              << "sensory processing, and neural integration" << std::endl;
    std::cout << "Key capabilities demonstrated:" << std::endl;
    std::cout << "  • Complex multi-sensory perception" << std::endl;
    std::cout << "  • Realistic physics-based interaction" << std::endl;
    std::cout << "  • Goal-directed behavior" << std::endl;
    std::cout << "  • Neuromodulation and learning" << std::endl;
    std::cout << "  • Memory systems integration" << std::endl;
    std::cout << "  • Prediction and error correction" << std::endl;
    
    return 0;
}