// Brain_development.cpp implementation
// Contains:
// - Brain::getDevelopmentSystem() method (lines 1042-1044 from original Brain.cpp)
// - Brain::getDevelopmentalStage() method (lines 1046-1048 from original Brain.cpp)
// - Brain::setDevelopmentalStage() method (lines 1050-1052 from original Brain.cpp)
// - Any other development system methods

namespace nlm {

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}
}