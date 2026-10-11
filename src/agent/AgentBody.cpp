// AgentBody.cpp - Implementation
#include "AgentBody.hpp"
#include <cmath>

namespace nlm {

struct AgentBody::Impl {
    float x, y, z;              // Position
    float vx, vy, vz;           // Velocity
    float pitch, yaw, roll;     // Orientation Euler angles
    float fx, fy, fz;           // Force
    float tx, ty, tz;           // Torque
    float mass;                 // Mass
    float inertiaX, inertiaY, inertiaZ; // Principal moments of inertia
    
    Impl() : x(0.0f), y(0.0f), z(0.0f),
             vx(0.0f), vy(0.0f), vz(0.0f),
             pitch(0.0f), yaw(0.0f), roll(0.0f),
             fx(0.0f), fy(0.0f), fz(0.0f),
             tx(0.0f), ty(0.0f), tz(0.0f),
             mass(1.0f), inertiaX(1.0f), inertiaY(1.0f), inertiaZ(1.0f) {}
};

AgentBody::AgentBody() : pImpl(new Impl) {}

AgentBody::~AgentBody() {
    delete pImpl;
}

void AgentBody::initialize() {
    // Reset to initial state
    reset();
}

void AgentBody::update(float dt) {
    if (dt <= 0.0f) return;
    
    // Update position based on velocity (simple Euler integration)
    pImpl->x += pImpl->vx * dt;
    pImpl->y += pImpl->vy * dt;
    pImpl->z += pImpl->vz * dt;
    
    // Update velocity based on force (F = ma)
    pImpl->vx += (pImpl->fx / pImpl->mass) * dt;
    pImpl->vy += (pImpl->fy / pImpl->mass) * dt;
    pImpl->vz += (pImpl->fz / pImpl->mass) * dt;
    
    // Update orientation based on torque (simplified)
    // Angular acceleration = torque / inertia
    float alphaX = pImpl->tx / pImpl->inertiaX;
    float alphaY = pImpl->ty / pImpl->inertiaY;
    float alphaZ = pImpl->tz / pImpl->inertiaZ;
    
    pImpl->pitch += alphaX * dt;
    pImpl->yaw += alphaY * dt;
    pImpl->roll += alphaZ * dt;
    
    // Keep angles in reasonable range
    pImpl->pitch = fmodf(pImpl->pitch, 2.0f * 3.14159f);
    pImpl->yaw = fmodf(pImpl->yaw, 2.0f * 3.14159f);
    pImpl->roll = fmodf(pImpl->roll, 2.0f * 3.14159f);
    
    // Reset forces and torques after update
    pImpl->fx = 0.0f; pImpl->fy = 0.0f; pImpl->fz = 0.0f;
    pImpl->tx = 0.0f; pImpl->ty = 0.0f; pImpl->tz = 0.0f;
}

void AgentBody::setPosition(float x, float y, float z) {
    pImpl->x = x; pImpl->y = y; pImpl->z = z;
}

void AgentBody::setVelocity(float vx, float vy, float vz) {
    pImpl->vx = vx; pImpl->vy = vy; pImpl->vz = vz;
}

void AgentBody::setOrientation(float pitch, float yaw, float roll) {
    pImpl->pitch = pitch; pImpl->yaw = yaw; pImpl->roll = roll;
}

void AgentBody::applyForce(float fx, float fy, float fz) {
    pImpl->fx += fx; pImpl->fy += fy; pImpl->fz += fz;
}

void AgentBody::applyTorque(float tx, float ty, float tz) {
    pImpl->tx += tx; pImpl->ty += ty; pImpl->tz += tz;
}

float AgentBody::getX() const {
    return pImpl->x;
}

float AgentBody::getY() const {
    return pImpl->y;
}

float AgentBody::getZ() const {
    return pImpl->z;
}

float AgentBody::getVX() const {
    return pImpl->vx;
}

float AgentBody::getVY() const {
    return pImpl->vy;
}

float AgentBody::getVZ() const {
    return pImpl->vz;
}

float AgentBody::getPitch() const {
    return pImpl->pitch;
}

float AgentBody::getYaw() const {
    return pImpl->yaw;
}

float AgentBody::getRoll() const {
    return pImpl->roll;
}

void AgentBody::reset() {
    pImpl->x = 0.0f; pImpl->y = 0.0f; pImpl->z = 0.0f;
    pImpl->vx = 0.0f; pImpl->vy = 0.0f; pImpl->vz = 0.0f;
    pImpl->pitch = 0.0f; pImpl->yaw = 0.0f; pImpl->roll = 0.0f;
    pImpl->fx = 0.0f; pImpl->fy = 0.0f; pImpl->fz = 0.0f;
    pImpl->tx = 0.0f; pImpl->ty = 0.0f; pImpl->tz = 0.0f;
}

} // namespace nlm