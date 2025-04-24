#include "OpenGLTypes.hpp"
#include <rttr/registration>
#include "imgui.h"
#include "OpenGL.hpp"
#include <numbers>
#include "Engine.hpp"

namespace ES::Editor::Types::OpenGL {

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
    RegisterType<ES::Plugin::OpenGL::Component::MaterialHandle>(
        "MaterialHandle",
        "The material handle component.",
        std::function<void(ES::Engine::Core &, ES::Engine::Entity)>([](ES::Engine::Core &c, ES::Engine::Entity ent) {
            const auto &materialHandle = ent.GetComponents<ES::Plugin::OpenGL::Component::MaterialHandle>(c);
            ImGui::Text("Material name: %s", materialHandle.name.c_str());
        })
    );
    RegisterType<ES::Plugin::OpenGL::Component::ShaderHandle>(
        "ShaderHandle",
        "The shader handle component.",
        std::function<void(ES::Engine::Core &, ES::Engine::Entity)>([](ES::Engine::Core &c, ES::Engine::Entity ent) {
            const auto &shaderHandle = ent.GetComponents<ES::Plugin::OpenGL::Component::ShaderHandle>(c);
            ImGui::Text("Shader name: %s", shaderHandle.name.c_str());
        })
    );
    RegisterType<ES::Plugin::OpenGL::Component::ModelHandle>(
        "ModelHandle",
        "The model handle component."
    );
    RegisterType<ES::Plugin::OpenGL::Component::FontHandle>(
        "FontHandle",
        "The font handle component."
    );
    RegisterType<ES::Plugin::OpenGL::Component::TextHandle>(
        "TextHandle",
        "The text handle component."
    );
}
}