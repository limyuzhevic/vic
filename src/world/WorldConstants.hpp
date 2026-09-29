#pragma once

namespace nlm {

// Physics simulation constants
struct WorldConstants {
    // World dimensions
    static constexpr float DEFAULT_WIDTH = 20.0f;
    static constexpr float DEFAULT_HEIGHT = 20.0f;
    
    // Vision parameters
    static constexpr size_t DEFAULT_VISION_WIDTH = 16;
    static constexpr size_t DEFAULT_VISION_HEIGHT = 16;
    
    // Energy system
    static constexpr float DEFAULT_MAX_ENERGY = 100.0f;
    static constexpr float DEFAULT_ENERGY_DECAY_RATE = 0.01f;
    static constexpr float DEFAULT_RESOURCE_ENERGY_GAIN = 30.0f;
    
    // Physics
    static constexpr float MOVE_SPEED = 3.0f;
    static constexpr float TURN_SPEED = 2.0f;
    static constexpr float WALL_FRICTION = 0.9f;
    
    // Collision and interaction
    static constexpr float INTERACT_RANGE = 1.0f;
    static constexpr float HAZARD_DAMAGE_PER_FRAME = 0.01f;
    static constexpr float PASSIVE_HAZARD_DAMAGE = 0.01f;
    static constexpr float RESOURCE_ENERGY_GAIN = 30.0f;
    static constexpr float HAZARD_ENERGY_LOSS = 15.0f;
    
    // Vision system
    static constexpr int RAY_COUNT = 16;
    static constexpr float FOV_ANGLE = 1.57079632679f; // PI / 2.0f
    static constexpr float VISION_MAX_RANGE = 8.0f;
    static constexpr float RAY_STEP_SIZE = 0.1f;
    
    // Touch/proximity sensors
    static constexpr float TOUCH_MAX_RANGE = 1.5f;
    
    // Movement
    static constexpr float MOVEMENT_UPDATE_TIME = 0.1f;
    static constexpr float MIN_TIME_BETWEEN_ACTIONS = 0.1f;
    
    // Boundaries and walls
    static constexpr float WALL_BUFFER = 0.01f;
    static constexpr float OBJECT_RADIUS = 0.5f;
    static constexpr float SMALL_OBJECT_RADIUS = 0.4f;
    static constexpr float MEDIUM_OBJECT_RADIUS = 0.3f;
    
    // Rewards and penalties
    static constexpr float MOVEMENT_COST = 0.01f;
    static constexpr float BACKWARD_MOVEMENT_COST = 0.02f;
    static constexpr float TURN_COST = 0.005f;
    static constexpr float WALL_HIT_COST = 0.05f;
    
    // Agent properties
    static constexpr float DEFAULT_AGENT_RADIUS = 0.2f;
    static constexpr float MAX_VELOCITY = 5.0f;
    static constexpr float MAX_ANGULAR_VELOCITY = 3.0f;
    
    // Time and simulation
    static constexpr double DEFAULT_SIMULATION_TIME = 0.0;
    static constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    
    // Object placement
    static constexpr int NUM_RESOURCES = 5;
    static constexpr int NUM_HAZARDS = 3;
    
    // Vision encoding
    static constexpr float RESOURCE_BRIGHTNESS = 0.8f;
    static constexpr float HAZARD_DARKNESS = 0.3f;
    static constexpr float EMPTY_VISION_VALUE = 0.5f;
    
    // Sensory signal normalization
    static constexpr float MAX_VELOCITY_NORMALIZATION = 5.0f;
    static constexpr float MAX_ANGULAR_VELOCITY_NORMALIZATION = 3.0f;
};

} // namespace nlm