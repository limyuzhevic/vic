// AgentBody.hpp - Header (was missing from directory scan)
#pragma once

#include <memory>
#include <string>

namespace nlm {

class AgentBody {
public:
    AgentBody();
    ~AgentBody();
    
    void initialize();
    void update(float dt);
    
    void setPosition(float x, float y, float z);
    void setVelocity(float vx, float vy, float vz);
    void setOrientation(float pitch, float yaw, float roll);
    
    void applyForce(float fx, float fy, float fz);
    void applyTorque(float tx, float ty, float tz);
    
    float getX() const;
    float getY() const;
    float getZ() const;
    
    float getVX() const;
    float getVY() const;
    float getVZ() const;
    
    float getPitch() const;
    float getYaw() const;
    float getRoll() const;
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm