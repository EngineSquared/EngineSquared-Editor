#include "DrawHierarchy.hpp"
#include "imgui.h"
#include "SelectedEntity.hpp"

namespace ES::Editor::System {

    void DrawHierarchy(ES::Engine::Core &core)
    {
        ImGui::Begin("Hierarchy:");
        ImGui::Text("Entities:");

        auto &selectedEntity = core.GetResource<ES::Editor::Resource::SelectedEntity>();
        auto view = core.GetRegistry().view<entt::entity>();
        for (auto entity : view) {
            if (ImGui::Selectable(std::to_string((int)entity).c_str(), selectedEntity.entity.has_value() && selectedEntity.entity.value() == entity)) {
                selectedEntity.entity = entity;
            }
        }

        ImGui::End();
    }
    
}
    