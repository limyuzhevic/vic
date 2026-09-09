#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

class AgentBody {
public:
    AgentBody();
    ~AgentBody();
    
    // Position and movement
    void setPosition(float x, float y);
    void setOrientation(float orientation);
    void move(float dx, float dy);
    
    float getX() const { return x_; }
    float getY() const { return y_; }
    float getOrientation() const { return orientation_; }
    
    // Energy and state
    float getEnergy() const { return energy_; }
    void setEnergy(float energy) { energy_ = energy; }
    
    // Sensory capabilities
    void setVisionWidth(int width) { visionWidth_ = width; }
    void setVisionHeight(int height) { visionHeight_ = height; }
    int getVisionWidth() const { return visionWidth_; }
    int getVisionHeight() const { return visionHeight_; }
    
    // Internal state
    void setInternalSignal(int index, float value);
    float getInternalSignal(int index) const;
    
    // Movement capabilities
    void setMaxSpeed(float speed) { maxSpeed_ = speed; }
    float getMaxSpeed() const { return maxSpeed_; }
    
    // State
    void reset();
    
private:
    float x_;
    float y_;
    float orientation_;
    float energy_;
    int visionWidth_;
    int visionHeight_;
    std::vector<float> internalSignals_;
    float maxSpeed_;
};

} // namespace nlm
