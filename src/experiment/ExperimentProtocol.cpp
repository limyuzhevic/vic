#include "ExperimentProtocol.hpp"

namespace nlm {

bool ExperimentProtocol::addSubProtocol(std::shared_ptr<ExperimentProtocol> subProtocol) {
    return false;
}

bool ExperimentProtocol::removeSubProtocol(const std::string& name) {
    return false;
}

std::vector<std::shared_ptr<ExperimentProtocol>> ExperimentProtocol::getSubProtocols() const {
    return std::vector<std::shared_ptr<ExperimentProtocol>>();
}

} // namespace nlm