#pragma once

#include "SensoryInput.hpp"
#include <vector>

namespace nlm {

// Touch/proximity sensors (binary collision signals)
class Touch : public SensoryInput {
public:
    Touch();
    explicit Touch(size_t numSensors);
    ~Touch() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Set touch data
    void setData(const std::vector<float>& data);
    void setData(float* data, size_t size);
    
    // Get number of touch sensors
    size_t getNumSensors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Proprioception: body position/velocity signals
class Proprioception : public SensoryInput {
public:
    Proprioception();
    ~Proprioception() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Set proprioception data (x, y, orientation, velocityX, velocityY, angularVelocity)
    void setBodyState(float x, float y, float orientation,
                     float velocityX, float velocityY, float angularVelocity);
    void setBodyState(const std::vector<float>& state);
    
    // Get body state
    float getX() const;
    float getY() const;
    float getOrientation() const;
    float getVelocityX() const;
    float getVelocityY() const;
    float getAngularVelocity() const;
    
    // Get sensor index
    static size_t getNumSensors();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm