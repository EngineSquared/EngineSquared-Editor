#include "ObjectTypes.hpp"
#include <rttr/registration>
#include "imgui.h"
#include "Object.hpp"
#include <numbers>
#include "Engine.hpp"

namespace ES::Editor::Types::Object {

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
    RegisterType<ES::Plugin::Object::Component::Transform>("Transform", "The transform component.", std::function<void(ES::Engine::Core &, ES::Engine::Entity)>([](ES::Engine::Core &c, ES::Engine::Entity ent) {
        auto &transform = ent.GetComponents<ES::Plugin::Object::Component::Transform>(c);
        ImGui::DragFloat3("Position", &transform.position[0], 0.1f);
        ImGui::SameLine();
        if (ImGui::Button("Copy##0")) {
            ImGui::SetClipboardText(fmt::format("{{{:.2f}, {:.2f}, {:.2f}}}", transform.position.x, transform.position.y, transform.position.z).c_str());
        }
        glm::vec3 rotation = glm::eulerAngles(transform.rotation);
        ImGui::SliderFloat3("Rotation", &rotation[0], -std::numbers::pi_v<float> / 2.f, std::numbers::pi_v<float> / 2.f);
        ImGui::SameLine();
        if (ImGui::Button("Copy##1")) {
            ImGui::SetClipboardText(fmt::format("{{{:.2f}, {:.2f}, {:.2f}}}", rotation.x, rotation.y, rotation.z).c_str());
        }
        transform.rotation = glm::quat(rotation);
        ImGui::Text("Rotation (Quaternion): %.2f, %.2f, %.2f, %.2f", transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
        ImGui::SameLine();
        if (ImGui::Button("Copy##2")) {
            ImGui::SetClipboardText(fmt::format("{{{:.2f}, {:.2f}, {:.2f}, {:.2f}}}", transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w).c_str());
        }
        ImGui::DragFloat3("Scale", &transform.scale[0]);
        ImGui::SameLine();
        if (ImGui::Button("Copy##3")) {
            ImGui::SetClipboardText(fmt::format("{{{:.2f}, {:.2f}, {:.2f}}}", transform.scale.x, transform.scale.y, transform.scale.z).c_str());
        }
    }));
    RegisterType<ES::Plugin::Object::Component::Mesh>("Mesh", "The mesh component.", std::function<void(ES::Engine::Core &, ES::Engine::Entity)>([](ES::Engine::Core &c, ES::Engine::Entity ent) {
        const auto &mesh = ent.GetComponents<ES::Plugin::Object::Component::Mesh>(c);
        ImGui::Text("Vertices: %zu", mesh.vertices.size());
        ImGui::Text("Normals: %zu", mesh.normals.size());
        ImGui::Text("Indices: %zu", mesh.indices.size());
    }));
}
}