#include "DrawHierarchy.hpp"
#include "imgui.h"
#include "SelectedEntity.hpp"
#include <rttr/registration>

namespace ES::Editor::System {

    void DrawComponents(ES::Engine::Core &core) {
        auto &selectedEntity = core.GetResource<ES::Editor::Resource::SelectedEntity>();
        if (selectedEntity.entity.has_value() && selectedEntity.entity.value() != entt::null) {
            auto ent = selectedEntity.entity.value();
            ImGui::Begin("Components:");
            
            for (auto [id, storage]: core.GetRegistry().storage()) {
                std::string name = std::to_string(id);
                rttr::type type = rttr::type::get_by_name(name);
                if (storage.contains(ent)) {
                    if (!type.is_valid()) {
                        ImGui::Text("Warning! This component is not registered: %s", std::string(storage.type().name()).c_str());
                        continue;
                    }
                    ImGui::Text("Component: %s", type.get_metadata("GUI_LABEL").is_valid() ? type.get_metadata("GUI_LABEL").to_string().c_str() : type.get_name().data() /* or fallback name*/);
                    ImGui::Text("Description: %s", type.get_metadata("GUI_DESCR").is_valid() ? type.get_metadata("GUI_DESCR").to_string().c_str() : "No description available.");
                    if (type.get_metadata("GUI_DRAW").is_valid()) {
                        if (type.get_metadata("GUI_DRAW").can_convert<std::function<void(ES::Engine::Core &, ES::Engine::Entity)>>()) {
                            auto func = type.get_metadata("GUI_DRAW").convert<std::function<void(ES::Engine::Core &, ES::Engine::Entity)>>();
                            func(core, ES::Engine::Entity(ent));
                        } else {
                            ImGui::Text("GUI_DRAW metadata is not a valid function.");
                        }
                    }
                    ImGui::Separator();
                }
            }

            ImGui::End();
        }
    }
    
}
    