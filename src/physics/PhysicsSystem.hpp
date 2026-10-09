#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace nlm {

// Physics constants
constexpr float GRAVITY = 9.81f;
constexpr float DEFAULT_FRICTION = 0.5f;
constexpr float DEFAULT_RESTITUTION = 0.3f;
constexpr float DEFAULT_MASS = 1.0f;
constexpr float TIME_STEP = 0.016f; // 60 FPS

// Physics velocity vector (2D)
struct PhysicsVector {
    float x;
    float y;
    float angular; // Angular velocity in radians per second
    
    PhysicsVector() : x(0.0f), y(0.0f), angular(0.0f) {}
    PhysicsVector(float px, float py, float pang) : x(px), y(py), angular(pang) {}
    
    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }
    
    void normalize() {
        float mag = magnitude();
        if (mag > 0.0f) {
            x /= mag;
            y /= mag;
        }
    }
};

// Rigid body structure for object dynamics
struct RigidBody {
    // Position and orientation
    float x;
    float y;
    float orientation;
    
    // Physics properties
    float mass;
    float inertia;
    float radius;
    
    // Velocity and forces
    PhysicsVector linearVelocity;
    float angularVelocity;
    PhysicsVector force;
    float torque;
    
    // Material properties
    float friction;
    float restitution;
    float density;
    
    // State
    bool active;
    bool isStatic;
    
    RigidBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , mass(DEFAULT_MASS), inertia(0.5f * mass * 0.5f * 0.5f)
        , radius(0.5f), linearVelocity(), angularVelocity(0.0f)
        , force(), torque(0.0f), friction(DEFAULT_FRICTION)
        , restitution(DEFAULT_RESTITUTION), density(1.0f)
        , active(true), isStatic(false) {}
    
    RigidBody(float px, float py, float pradius, float pmass = DEFAULT_MASS)
        : x(px), y(py), orientation(0.0f)
        , mass(pmass), inertia(0.5f * mass * radius * radius)
        , radius(pradius), linearVelocity(), angularVelocity(0.0f)
        , force(), torque(0.0f), friction(DEFAULT_FRICTION)
        , restitution(DEFAULT_RESTITUTION), density(1.0f)
        , active(true), isStatic(false) {}
    
    // Update position based on velocity and forces
    void updatePosition(float dt) {
        if (isStatic) return;
        
        // Integrate velocity
        x += linearVelocity.x * dt;
        y += linearVelocity.y * dt;
        orientation += angularVelocity * dt;
        
        // Normalize orientation
        while (orientation > M_PI) orientation -= 2.0f * M_PI;
        while (orientation < -M_PI) orientation += 2.0f * M_PI;
        
        // Apply force
        if (force.x != 0.0f || force.y != 0.0f) {
            float ax = force.x / mass;
            float ay = force.y / mass;
            linearVelocity.x += ax * dt;
            linearVelocity.y += ay * dt;
        }
        
        // Apply torque
        if (torque != 0.0f) {
            float alpha = torque / inertia;
            angularVelocity += alpha * dt;
        }
        
        // Apply friction
        if (!isStatic) {
            float linearFriction = friction;
            linearVelocity.x *= std::pow(1.0f - linearFriction, dt);
            linearVelocity.y *= std::pow(1.0f - linearFriction, dt);
            angularVelocity *= std::pow(1.0f - friction, dt);
        }
        
        // Reset forces
        force.x = 0.0f;
        force.y = 0.0f;
        torque = 0.0f;
    }
    
    // Apply impulse (instantaneous force change)
    void applyImpulse(const PhysicsVector& impulse, const PhysicsVector& contactPoint) {
        if (isStatic) return;
        
        // Linear impulse
        linearVelocity.x += impulse.x / mass;
        linearVelocity.y += impulse.y / mass;
        
        // Angular impulse (torque impulse)
        PhysicsVector relativePoint(contactPoint.x - x, contactPoint.y - y, 0.0f);
        float crossProduct = relativePoint.x * impulse.y - relativePoint.y * impulse.x;
        angularVelocity += crossProduct / inertia;
    }
    
    // Apply force
    void applyForce(float fx, float fy, float ftorque = 0.0f) {
        force.x += fx;
        force.y += fy;
        torque += ftorque;
    }
    
    // Check collision with another rigid body
    bool checkCollision(const RigidBody& other, PhysicsVector& contactPoint, PhysicsVector& contactNormal) const {
        float dx = other.x - x;
        float dy = other.y - y;
        float distance = std::sqrt(dx * dx + dy * dy);
        float minDistance = radius + other.radius;
        
        if (distance < minDistance && distance > 0.0f) {
            // Collision detected
            contactNormal.x = dx / distance;
            contactNormal.y = dy / distance;
            contactPoint.x = x + contactNormal.x * radius;
            contactPoint.y = y + contactNormal.y * radius;
            return true;
        }
        return false;
    }
    
    // Resolve collision with another rigid body
    void resolveCollision(RigidBody& other, const PhysicsVector& contactPoint, const PhysicsVector& contactNormal) {
        if (!active || !other.active) return;
        
        // Calculate relative velocity
        PhysicsVector relativeVelocity;
        relativeVelocity.x = other.linearVelocity.x - linearVelocity.x;
        relativeVelocity.y = other.linearVelocity.y - linearVelocity.y;
        relativeVelocity.angular = other.angularVelocity - angularVelocity;
        
        // Calculate impulse
        PhysicsVector impulse;
        impulse.x = contactNormal.x * (1.0f + restitution) * relativeVelocity.x;
        impulse.y = contactNormal.y * (1.0f + restitution) * relativeVelocity.y;
        
        // Apply impulses to both bodies
        applyImpulse(impulse, contactPoint);
        other.applyImpulse(PhysicsVector(-impulse.x, -impulse.y, 0.0f), contactPoint);
    }
};

// Collision information
struct CollisionInfo {
    RigidBody* bodyA;
    RigidBody* bodyB;
    PhysicsVector contactPoint;
    PhysicsVector contactNormal;
    float penetrationDepth;
    
    CollisionInfo() : bodyA(nullptr), bodyB(nullptr), penetrationDepth(0.0f) {}
};

// Physics system for simulation
class PhysicsSystem {
public:
    PhysicsSystem() = default;
    ~PhysicsSystem() = default;
    
    // Add a rigid body to the simulation
    void addRigidBody(std::shared_ptr<RigidBody> body) {
        bodies_.push_back(body);
    }
    
    // Remove a rigid body from the simulation
    void removeRigidBody(RigidBody* body) {
        bodies_.erase(
            std::remove_if(bodies_.begin(), bodies_.end(),
                [body](const std::shared_ptr<RigidBody>& b) { return b.get() == body; }),
            bodies_.end()
        );
    }
    
    // Update physics simulation
    void update(float dt) {
        // Detect collisions
        detectCollisions();
        
        // Update positions based on current velocities
        for (auto& body : bodies_) {
            body->updatePosition(dt);
        }
        
        // Apply gravity to non-static bodies
        applyGravity(dt);
        
        // Apply other environmental forces
        applyEnvironmentalForces(dt);
    }
    
    // Apply gravity to all bodies
    void applyGravity(float dt) {
        for (auto& body : bodies_) {
            if (!body->isStatic && body->active) {
                // Apply gravitational force
                float force = body->mass * GRAVITY;
                body->applyForce(0.0f, force * dt);
            }
        }
    }
    
    // Apply environmental forces (friction, wind, etc.)
    void applyEnvironmentalForces(float dt) {
        // Apply wind force if any bodies are in wind area
        applyWindForce(dt);
        
        // Apply ground friction
        applyGroundFriction(dt);
        
        // Apply air resistance
        applyAirResistance(dt);
    }
    
    // Apply wind force
    void applyWindForce(float dt) {
        float windForce = 0.5f; // Wind force constant
        for (auto& body : bodies_) {
            if (!body->isStatic && body->active) {
                // Apply wind force in +x direction (can be enhanced)
                body->applyForce(windForce * body->mass, 0.0f);
            }
        }
    }
    
    // Apply ground friction
    void applyGroundFriction(float dt) {
        for (auto& body : bodies_) {
            if (!body->isStatic && body->active) {
                // Simple ground friction when body is near ground (y < radius)
                if (body->y < body->radius) {
                    body->linearVelocity.x *= 0.95f;
                    body->linearVelocity.y *= 0.95f;
                }
            }
        }
    }
    
    // Apply air resistance
    void applyAirResistance(float dt) {
        for (auto& body : bodies_) {
            if (!body->isStatic && body->active) {
                float speed = body->linearVelocity.magnitude();
                if (speed > 0.0f) {
                    float dragForce = 0.5f * 1.225f * body->density * body->radius * speed * speed;
                    float dragRatio = dragForce / (body->mass * GRAVITY);
                    body->linearVelocity.x *= (1.0f - dragRatio);
                    body->linearVelocity.y *= (1.0f - dragRatio);
                }
            }
        }
    }
    
    // Detect collisions between all bodies
    void detectCollisions() {
        collisions_.clear();
        
        for (size_t i = 0; i < bodies_.size(); ++i) {
            for (size_t j = i + 1; j < bodies_.size(); ++j) {
                auto& bodyA = bodies_[i];
                auto& bodyB = bodies_[j];
                
                if (!bodyA->active || !bodyB->active) continue;
                
                PhysicsVector contactPoint, contactNormal;
                if (bodyA->checkCollision(*bodyB, contactPoint, contactNormal)) {
                    CollisionInfo collision;
                    collision.bodyA = bodyA.get();
                    collision.bodyB = bodyB.get();
                    collision.contactPoint = contactPoint;
                    collision.contactNormal = contactNormal;
                    collision.penetrationDepth = bodyA->radius + bodyB->radius - 
                        std::sqrt((bodyB->x - bodyA->x) * (bodyB->x - bodyA->x) + 
                                 (bodyB->y - bodyA->y) * (bodyB->y - bodyA->y));
                    collisions_.push_back(collision);
                }
            }
        }
    }
    
    // Resolve all detected collisions
    void resolveCollisions() {
        for (const auto& collision : collisions_) {
            collision.bodyA->resolveCollision(*collision.bodyB, 
                collision.contactPoint, collision.contactNormal);
        }
    }
    
    // Get all rigid bodies
    const std::vector<std::shared_ptr<RigidBody>>& getBodies() const {
        return bodies_;
    }
    
    // Clear all bodies
    void clear() {
        bodies_.clear();
        collisions_.clear();
    }
    
private:
    std::vector<std::shared_ptr<RigidBody>> bodies_;
    std::vector<CollisionInfo> collisions_;
};

} // namespace nlm