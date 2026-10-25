#ifndef DEVELOPMENT_SYSTEM_H
#define DEVELOPMENT_SYSTEM_H

#include "development/DevelopmentSystem.hpp"
#include "brain/Brain.hpp"

namespace nlm {

class DevelopmentSystem {
public:
    virtual ~DevelopmentSystem() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(Brain* brain, TimestepDuration timestep) = 0;
    
    virtual DevelopmentalStage getCurrentStage() const = 0;
    virtual void setCurrentStage(DevelopmentalStage stage) = 0;
    
    virtual void applyDevelopmentalPlasticity(StructuralPlasticity* structuralPlasticity) = 0;
    virtual void updateDevelopmentalStage(float time) = 0;
    virtual bool isDevelopmentComplete() const = 0;
    
    virtual float getDevelopmentalAge() const = 0;
    virtual float getPlasticityRate() const = 0;
};

} // namespace nlm

#endif // DEVELOPMENT_SYSTEM_H