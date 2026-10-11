#include "src/brain/Brain.hpp"
#include "src/core/Types/Types.hpp"

int main() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    return 0;
}
