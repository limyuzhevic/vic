// BrainDevelopment.hpp - Development system access
#pragma once

#include "../core/Types/Types.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

class BrainDevelopment {
public:
    BrainDevelopment(Brain* brain);
    ~BrainDevelopment();
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
private:
    Brain* brain_;
};

} // namespace nlm
