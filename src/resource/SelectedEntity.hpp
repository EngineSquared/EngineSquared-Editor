#pragma once

#include <optional>
#include "Engine.hpp"

namespace ES::Editor::Resource {
class SelectedEntity {
public:
    std::optional<entt::entity> entity = std::nullopt;
};
}