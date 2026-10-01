// Factory Interface Implementation

#include "Factory.hpp"

namespace nlm {
namespace FactoryImpl {
    std::unique_ptr<Factory> currentFactory;
}

} // namespace nlm