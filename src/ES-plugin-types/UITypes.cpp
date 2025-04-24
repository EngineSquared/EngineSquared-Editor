#include "UITypes.hpp"
#include <rttr/registration>
#include "imgui.h"
#include <numbers>
#include "Engine.hpp"
#include "UI.hpp"

namespace ES::Editor::Types::UI {

template<typename TComponent>
static void RegisterType(const std::string &label, const std::string &descr, const std::function<void(ES::Engine::Core &, ES::Engine::Entity)> &drawFunc = nullptr) {
    rttr::registration::class_<TComponent>(std::to_string(entt::type_hash<TComponent>::value()))
    (
        rttr::metadata("GUI_LABEL", label),
        rttr::metadata("GUI_DESCR", descr)
    );
    if (drawFunc) {
        rttr::registration::class_<TComponent>(std::to_string(entt::type_hash<TComponent>::value()))
        (
            rttr::metadata("GUI_DRAW", drawFunc)
        );
    }
};

void RegisterTypes(void) {
    RegisterType<ES::Plugin::UI::Component::Text>(
        "Text",
        "The text component."
    );
}
}