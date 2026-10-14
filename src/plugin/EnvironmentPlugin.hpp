#pragma once

#include "Plugin.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace nlm {

// Abstract base class for environment plugins
class EnvironmentPlugin : public virtual IPlugin, public virtual IConfigurable {
public:
    virtual ~EnvironmentPlugin() = default;
    
    // Plugin category identification
    const char* getCategory() const override { return "Environment"; }
    
    // Environment plugin specific operations
    virtual void initializeEnvironment(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual void updateEnvironment(TimestepDuration dt) = 0;
    virtual void shutdownEnvironment() = 0;
    virtual bool isEnvironmentReady() const = 0;
    
    // Environment-specific configuration parameters
    virtual std::vector<std::string> getEnvironmentConfigParameters() const {
        return {"worldWidth", "worldHeight", "maxEntities", "timeOfDay", "weatherType"};
    }
    
    virtual std::string getEnvironmentParameterType(const std::string& paramName) const {
        if (paramName == "worldWidth") return "int";
        if (paramName == "worldHeight") return "int";
        if (paramName == "maxEntities") return "int";
        if (paramName == "timeOfDay") return "float";
        if (paramName == "weatherType") return "string";
        return "string";
    }
    
    virtual std::string getEnvironmentParameterDescription(const std::string& paramName) const {
        if (paramName == "worldWidth") return "Width of the simulation world";
        if (paramName == "worldHeight") return "Height of the simulation world";
        if (paramName == "maxEntities") return "Maximum number of entities in the world";
        if (paramName == "timeOfDay") return "Time of day (0.0-24.0)";
        if (paramName == "weatherType") return "Type of weather (clear, rain, snow, etc.)";
        return "";
    }
};

// Example environment plugin: DefaultEnvironmentPlugin
class DefaultEnvironmentPlugin : public EnvironmentPlugin {
public:
    DefaultEnvironmentPlugin() = default;
    ~DefaultEnvironmentPlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "DefaultEnvironmentPlugin", "1.0.0", "Standard simulation environment",
            "NLM Team", {}, 65, "Environment", {
                {"worldWidth", "100"}, {"worldHeight", "100"}, {"maxEntities", "1000"},
                {"timeOfDay", "12.0"}, {"weatherType", "clear"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        initializeEnvironment(config);
        setState(PluginState::Initialized);
        return isEnvironmentReady();
    }
    
    void update(TimestepDuration dt) override {
        updateEnvironment(dt);
    }
    
    void shutdown() override {
        shutdownEnvironment();
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "DefaultEnvironmentPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        worldWidth_ = getIntFromConfig(config, "worldWidth", 100);
        worldHeight_ = getIntFromConfig(config, "worldHeight", 100);
        maxEntities_ = getIntFromConfig(config, "maxEntities", 1000);
        timeOfDay_ = getFloatFromConfig(config, "timeOfDay", 12.0f);
        weatherType_ = getStringFromConfig(config, "weatherType", "clear");
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        return getEnvironmentConfigParameters();
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        return getEnvironmentParameterType(paramName);
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        return getEnvironmentParameterDescription(paramName);
    }
    
    void initializeEnvironment(const std::unordered_map<std::string, std::string>& config) override {
        // Initialize environment with specified parameters
        // Set up world dimensions, entities, and initial state
        initialized_ = true;
        
        // Apply weather effects
        applyWeatherEffects(weatherType_);
    }
    
    void updateEnvironment(TimestepDuration dt) override {
        // Update environment state
        updateTimeOfDay(dt);
        updateWeatherEffects(dt);
        updateEnvironmentalFactors(dt);
    }
    
    void shutdownEnvironment() override {
        // Clean up environment resources
        entities_.clear();
        initialized_ = false;
    }
    
    bool isEnvironmentReady() const override {
        return initialized_ && !entities_.empty();
    }
    
    // Entity management
    void addEntity(const std::string& entityType, const std::unordered_map<std::string, std::string>& properties = {}) {
        if (entities_.size() < (size_t)maxEntities_) {
            entities_.push_back({entityType, properties});
        }
    }
    
    std::vector<std::pair<std::string, std::unordered_map<std::string, std::string>>> getEntities() const {
        return entities_;
    }
    
    void removeEntity(const std::string& entityType) {
        entities_.erase(
            std::remove_if(entities_.begin(), entities_.end(),
                [&entityType](const auto& entity) { return entity.first == entityType; }),
            entities_.end()
        );
    }
    
    // Weather simulation
    void applyWeatherEffects(const std::string& weatherType) {
        weather_ = weatherType;
        // Apply weather-specific effects to environment
        if (weatherType == "rain") {
            rainIntensity_ = 0.5f;
        } else if (weatherType == "snow") {
            snowIntensity_ = 0.3f;
        }
        // Additional weather effects would go here
    }
    
    void updateWeatherEffects(TimestepDuration dt) {
        // Update weather over time
        if (weather_ == "rain") {
            rainIntensity_ = std::min(rainIntensity_ + dt * 0.01f, 1.0f);
        } else if (weather_ == "snow") {
            snowIntensity_ = std::min(snowIntensity_ + dt * 0.005f, 0.5f);
        }
    }
    
    // Time simulation
    void updateTimeOfDay(TimestepDuration dt) {
        timeOfDay_ += dt * 0.01f; // Progress time
        if (timeOfDay_ > 24.0f) {
            timeOfDay_ -= 24.0f;
        }
        
        // Update lighting based on time of day
        updateLighting(timeOfDay_);
    }
    
    void updateLighting(float timeOfDay) {
        // Calculate lighting based on time of day
        if (timeOfDay >= 6.0f && timeOfDay <= 18.0f) {
            lightingIntensity_ = 1.0f;
        } else {
            lightingIntensity_ = 0.2f + 0.8f * (1.0f - ((timeOfDay - 18.0f) / 6.0f));
        }
    }
    
    void updateEnvironmentalFactors(TimestepDuration dt) {
        // Update environmental factors like temperature, humidity, etc.
        // This would be more complex in a real implementation
    }
    
    float getCpuUsage() const override { return rainIntensity_ * 0.3f + snowIntensity_ * 0.2f; }
    size_t getMemoryUsage() const override { 
        size_t total = sizeof(DefaultEnvironmentPlugin);
        for (const auto& entity : entities_) {
            total += entity.first.length() + entity.second.size() * sizeof(std::pair<std::string, std::string>);
        }
        return total;
    }
    size_t getActiveObjects() const override { return entities_.size(); }
    float getPerformanceScore() const override { return lightingIntensity_ * (1.0f - rainIntensity_ * 0.5f - snowIntensity_ * 0.3f); }
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override {
        // Serialize environment state to file
        return false;
    }
    
    bool loadState(const std::string& filepath) override {
        // Deserialize environment state from file
        return false;
    }
    
    bool hasState() const override {
        return !entities_.empty();
    }
    
    void clearState() override {
        entities_.clear();
        shutdownEnvironment();
    }
    
    std::string getStateSignature() const override {
        return "DefaultEnvironmentPlugin_v1_" + std::to_string(entities_.size()) + "_" + weather_;
    }
    
    const std::string& getWeatherType() const { return weather_; }
    float getRainIntensity() const { return rainIntensity_; }
    float getSnowIntensity() const { return snowIntensity_; }
    float getLightingIntensity() const { return lightingIntensity_; }
    float getTimeOfDay() const { return timeOfDay_; }
    
private:
    PluginState state_ = PluginState::Unloaded;
    bool initialized_ = false;
    
    int worldWidth_ = 100;
    int worldHeight_ = 100;
    int maxEntities_ = 1000;
    float timeOfDay_ = 12.0f;
    std::string weatherType_ = "clear";
    
    std::string weather_ = "clear";
    float rainIntensity_ = 0.0f;
    float snowIntensity_ = 0.0f;
    float lightingIntensity_ = 1.0f;
    
    std::vector<std::pair<std::string, std::unordered_map<std::string, std::string>>> entities_;
    std::unordered_map<std::string, std::string> config_;
    
    int getIntFromConfig(const std::unordered_map<std::string, std::string>& config, 
                        const std::string& key, int defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stoi(it->second) : defaultValue;
    }
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    std::string getStringFromConfig(const std::unordered_map<std::string, std::string>& config,
                                   const std::string& key, const std::string& defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? it->second : defaultValue;
    }
    
    void setState(PluginState state) {
        state_ = state;
    }
};

// Example environment plugin: DynamicEnvironmentPlugin
class DynamicEnvironmentPlugin : public EnvironmentPlugin {
public:
    DynamicEnvironmentPlugin() = default;
    ~DynamicEnvironmentPlugin() override = default;
    
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata = {
            "DynamicEnvironmentPlugin", "1.0.0", "Dynamically changing environment with events",
            "NLM Team", {}, 55, "Environment", {
                {"worldWidth", "200"}, {"worldHeight", "200"}, {"maxEntities", "5000"},
                {"timeOfDay", "6.0"}, {"weatherType", "rain"}
            }
        };
        return metadata;
    }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        initializeEnvironment(config);
        setState(PluginState::Initialized);
        return isEnvironmentReady();
    }
    
    void update(TimestepDuration dt) override {
        updateEnvironment(dt);
        updateEvents(dt); // Update environmental events
    }
    
    void shutdown() override {
        shutdownEnvironment();
        setState(PluginState::Unloaded);
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    const char* getName() const override { return "DynamicEnvironmentPlugin"; }
    const char* getVersion() const override { return "1.0.0"; }
    
    bool isValid() const override {
        return state_ == PluginState::Initialized || state_ == PluginState::Running;
    }
    
    bool configure(const std::unordered_map<std::string, std::string>& config) override {
        worldWidth_ = getIntFromConfig(config, "worldWidth", 200);
        worldHeight_ = getIntFromConfig(config, "worldHeight", 200);
        maxEntities_ = getIntFromConfig(config, "maxEntities", 5000);
        timeOfDay_ = getFloatFromConfig(config, "timeOfDay", 6.0f);
        weatherType_ = getStringFromConfig(config, "weatherType", "rain");
        
        // Additional dynamic parameters
        eventFrequency_ = getFloatFromConfig(config, "eventFrequency", 0.01f);
        environmentChangeRate_ = getFloatFromConfig(config, "environmentChangeRate", 0.05f);
        
        return true;
    }
    
    const std::unordered_map<std::string, std::string>& getConfiguration() const override {
        return config_;
    }
    
    std::vector<std::string> getConfigParameters() const override {
        auto params = getEnvironmentConfigParameters();
        params.insert(params.end(), {"eventFrequency", "environmentChangeRate"});
        return params;
    }
    
    std::string getParameterType(const std::string& paramName) const override {
        if (paramName == "eventFrequency") return "float";
        if (paramName == "environmentChangeRate") return "float";
        return getEnvironmentParameterType(paramName);
    }
    
    std::string getParameterDescription(const std::string& paramName) const override {
        if (paramName == "eventFrequency") return "Frequency of environmental events";
        if (paramName == "environmentChangeRate") return "Rate of environment changes";
        return getEnvironmentParameterDescription(paramName);
    }
    
    void initializeEnvironment(const std::unordered_map<std::string, std::string>& config) override {
        // Initialize with dynamic environment
        initialized_ = true;
        
        // Set up events
        lastEventTime_ = 0.0;
        weather_ = weatherType_;
        
        // Generate initial entities
        generateRandomEntities(50);
        
        // Apply weather effects
        applyWeatherEffects(weatherType_);
    }
    
    void updateEnvironment(TimestepDuration dt) override {
        // Update environment with dynamics
        updateTimeOfDay(dt);
        updateWeatherEffects(dt);
        updateEnvironmentalFactors(dt);
        updateEvents(dt);
    }
    
    void shutdownEnvironment() override {
        // Clean up all entities
        entities_.clear();
        events_.clear();
        initialized_ = false;
    }
    
    bool isEnvironmentReady() const override {
        return initialized_ && !entities_.empty();
    }
    
    // Event system
    void updateEvents(TimestepDuration dt) {
        lastEventTime_ += dt;
        
        // Generate random events
        float eventProb = eventFrequency_ * dt;
        if (eventProb > 0.0f) {
            static std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            if (dist(rng) < eventProb) {
                generateRandomEvent();
            }
        }
        
        // Update existing events
        for (auto& event : events_) {
            event->update(dt);
            if (event->isCompleted()) {
                handleEventCompletion(event.get());
            }
        }
        
        // Clean up completed events
        events_.erase(
            std::remove_if(events_.begin(), events_.end(),
                [](const std::unique_ptr<EnvironmentEvent>& e) { return e->isCompleted(); }),
            events_.end()
        );
    }
    
    void generateRandomEvent() {
        // Generate various types of environmental events
        int eventType = std::rand() % 3;
        
        switch (eventType) {
            case 0:
                // Weather event
                events_.push_back(std::make_unique<WeatherEvent>("storm", 30.0f, 10.0f));
                break;
            case 1:
                // Entity event
                events_.push_back(std::make_unique<EntityEvent>("spawn_flock", 20));
                break;
            case 2:
                // Resource event
                events_.push_back(std::make_unique<ResourceEvent>("resource_spike", 100.0f));
                break;
        }
    }
    
    void handleEventCompletion(EnvironmentEvent* event) {
        // Handle event completion effects
        // This would modify environment state
    }
    
    void generateRandomEntities(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            std::string entityType = (std::rand() % 2 == 0) ? "animal" : "resource";
            std::unordered_map<std::string, std::string> properties;
            
            if (entityType == "animal") {
                properties["speed"] = std::to_string(1.0f + (std::rand() % 10) * 0.1f);
                properties["size"] = std::to_string(0.5f + (std::rand() % 10) * 0.05f);
            } else {
                properties["quantity"] = std::to_string(10 + std::rand() % 50);
                properties["renewal_rate"] = std::to_string(0.01f + (std::rand() % 10) * 0.001f);
            }
            
            addEntity(entityType, properties);
        }
    }
    
    // Event base class
    class EnvironmentEvent {
    public:
        virtual ~EnvironmentEvent() = default;
        virtual void update(TimestepDuration dt) = 0;
        virtual bool isCompleted() const = 0;
        virtual const std::string& getType() const = 0;
    };
    
    // Weather event
    class WeatherEvent : public EnvironmentEvent {
    public:
        WeatherEvent(const std::string& type, float duration, float intensity)
            : type_(type), duration_(duration), intensity_(intensity), elapsed_(0.0f) {}
        
        void update(TimestepDuration dt) override {
            elapsed_ += dt;
        }
        
        bool isCompleted() const override {
            return elapsed_ >= duration_;
        }
        
        const std::string& getType() const override {
            return type_;
        }
        
        float getIntensity() const { return intensity_; }
        
    private:
        std::string type_;
        float duration_;
        float intensity_;
        float elapsed_;
    };
    
    // Entity event
    class EntityEvent : public EnvironmentEvent {
    public:
        EntityEvent(const std::string& type, int count) : type_(type), count_(count), spawned_(0) {}
        
        void update(TimestepDuration dt) override {
            // Simulate spawning
            if (spawned_ < count_) {
                spawned_++;
            }
        }
        
        bool isCompleted() const override {
            return spawned_ >= count_;
        }
        
        const std::string& getType() const override {
            return type_;
        }
        
    private:
        std::string type_;
        int count_;
        int spawned_;
    };
    
    // Resource event
    class ResourceEvent : public EnvironmentEvent {
    public:
        ResourceEvent(const std::string& type, float amount) : type_(type), amount_(amount) {}
        
        void update(TimestepDuration dt) override {
            // Resource has been spawned
        }
        
        bool isCompleted() const override {
            return true; // One-time event
        }
        
        const std::string& getType() const override {
            return type_;
        }
        
        float getAmount() const { return amount_; }
        
    private:
        std::string type_;
        float amount_;
    };
    
    float getCpuUsage() const override { 
        return rainIntensity_ * 0.4f + snowIntensity_ * 0.3f + events_.size() * 0.1f; 
    }
    
    size_t getMemoryUsage() const override { 
        size_t total = sizeof(DynamicEnvironmentPlugin);
        for (const auto& entity : entities_) {
            total += entity.first.length() + entity.second.size() * sizeof(std::pair<std::string, std::string>);
        }
        for (const auto& event : events_) {
            total += sizeof(EnvironmentEvent*);
        }
        return total;
    }
    
    size_t getActiveObjects() const override { 
        return entities_.size() + events_.size(); 
    }
    
    float getPerformanceScore() const override { 
        return lightingIntensity_ * (1.0f - rainIntensity_ * 0.6f - snowIntensity_ * 0.4f) / (1.0f + events_.size() * 0.1f);
    }
    
    void resetPerformanceMetrics() override {}
    
    bool saveState(const std::string& filepath) const override {
        // Serialize environment and event state
        return false;
    }
    
    bool loadState(const std::string& filepath) override {
        // Deserialize environment and event state
        return false;
    }
    
    bool hasState() const override {
        return !entities_.empty() || !events_.empty();
    }
    
    void clearState() override {
        entities_.clear();
        events_.clear();
        shutdownEnvironment();
    }
    
    std::string getStateSignature() const override {
        return "DynamicEnvironmentPlugin_v1_" + std::to_string(entities_.size()) + "_" + std::to_string(events_.size()) + "_" + weather_;
    }
    
private:
    PluginState state_ = PluginState::Unloaded;
    bool initialized_ = false;
    
    int worldWidth_ = 200;
    int worldHeight_ = 200;
    int maxEntities_ = 5000;
    float timeOfDay_ = 6.0f;
    std::string weatherType_ = "rain";
    
    std::string weather_ = "rain";
    float rainIntensity_ = 0.5f;
    float snowIntensity_ = 0.0f;
    float lightingIntensity_ = 0.8f;
    
    std::vector<std::pair<std::string, std::unordered_map<std::string, std::string>>> entities_;
    std::vector<std::unique_ptr<EnvironmentEvent>> events_;
    std::unordered_map<std::string, std::string> config_;
    
    float lastEventTime_ = 0.0f;
    float eventFrequency_ = 0.01f;
    float environmentChangeRate_ = 0.05f;
    
    int getIntFromConfig(const std::unordered_map<std::string, std::string>& config, 
                        const std::string& key, int defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stoi(it->second) : defaultValue;
    }
    
    float getFloatFromConfig(const std::unordered_map<std::string, std::string>& config,
                            const std::string& key, float defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? std::stof(it->second) : defaultValue;
    }
    
    std::string getStringFromConfig(const std::unordered_map<std::string, std::string>& config,
                                   const std::string& key, const std::string& defaultValue) const {
        auto it = config.find(key);
        return it != config.end() ? it->second : defaultValue;
    }
    
    void setState(PluginState state) {
        state_ = state;
    }
};

} // namespace nlm