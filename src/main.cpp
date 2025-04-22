#include "Core.hpp"
#include "OpenGL.hpp"
#include "RenderingPipeline.hpp"
#include "APlugin.hpp"
#include "Startup.hpp"
#include "Startup.hpp"
#include "system/WindowSystem.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <rttr/registration>

namespace UI {
class SelectedEntity {
public:
    std::optional<entt::entity> entity = std::nullopt;
};
}

class OpenGLIMGUI : public ES::Engine::APlugin {
    public:
      explicit OpenGLIMGUI(ES::Engine::Core &core)
          : ES::Engine::APlugin(core){
                // empty
            };
      ~OpenGLIMGUI() = default;
  
      void Bind() final
      {
        RequirePlugins<ES::Plugin::OpenGL::Plugin>();
        RegisterSystems<ES::Plugin::RenderingPipeline::Init>([](ES::Engine::Core &core){
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
        });
        RegisterSystems<ES::Plugin::RenderingPipeline::Setup>(
            [](ES::Engine::Core &core){
                ImGuiIO& io = ImGui::GetIO(); (void)io;
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

                // Setup Dear ImGui style
                ImGui::StyleColorsDark();

                // Setup Platform/Renderer backends
                ImGui_ImplGlfw_InitForOpenGL(core.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow(), true);
                ImGui_ImplOpenGL3_Init("#version 130");
            }
        );
        RegisterSystems<ES::Engine::Scheduler::Startup>([](ES::Engine::Core &core){
            core.RegisterResource<UI::SelectedEntity>(UI::SelectedEntity());
        });

        RegisterSystems<ES::Plugin::RenderingPipeline::PreUpdate>([](ES::Engine::Core &core){
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        });

        RegisterSystems<ES::Engine::Scheduler::Update>(
            // Hierarchy
            [](ES::Engine::Core &core){
                auto &selectedEntity = core.GetResource<UI::SelectedEntity>();

                ImGui::Begin("Hierarchy:");
                    ImGui::Text("Entities:");
                    auto view = core.GetRegistry().view<entt::entity>();
                    for (auto entity : view) {
                        if (ImGui::Selectable(std::to_string((int)entity).c_str(), selectedEntity.entity.has_value() && selectedEntity.entity.value() == entity)) {
                            selectedEntity.entity = entity;
                        }
                    }
                ImGui::End();
            },
            // Components
            [](ES::Engine::Core &core) {
                auto &selectedEntity = core.GetResource<UI::SelectedEntity>();
                if (selectedEntity.entity.has_value() && selectedEntity.entity.value() != entt::null) {
                    auto ent = selectedEntity.entity.value();
                    ImGui::Begin("Components:");
                    
                    for (auto [id, storage]: core.GetRegistry().storage()) {
                        std::string name = std::to_string(id);
                        rttr::type type = rttr::type::get_by_name(name);
                        if (type.is_valid() && storage.contains(ent)) {
                            ImGui::Text("Component: %s", type.get_metadata("GUI_LABEL").is_valid() ? type.get_metadata("GUI_LABEL").to_string().c_str() : type.get_name().data() /* or fallback name*/);
                            ImGui::Text("Description: %s", type.get_metadata("GUI_DESCR").is_valid() ? type.get_metadata("GUI_DESCR").to_string().c_str() : "No description available.");
                            if (!type.get_properties().empty()) {
                                if (type.get_metadata("GUI_DRAW").is_valid()) {
                                    if (type.get_metadata("GUI_DRAW").can_convert<std::function<void(ES::Engine::Core &, ES::Engine::Entity)>>()) {
                                        auto func = type.get_metadata("GUI_DRAW").convert<std::function<void(ES::Engine::Core &, ES::Engine::Entity)>>();
                                        func(core, ES::Engine::Entity(ent));
                                    } else {
                                        ImGui::Text("GUI_DRAW metadata is not a valid function.");
                                    }
                                } else {
                                    ImGui::Text("Properties:");
                                    for (const auto& prop : type.get_properties()) {
                                        if (prop.get_metadata("GUI_LABEL").is_valid()) {
                                            ImGui::Text("  %s", prop.get_metadata("GUI_LABEL").to_string().c_str());
                                        } else {
                                            ImGui::Text("  %s", prop.get_type().get_name().data());
                                        }
                                        if (prop.get_metadata("GUI_DESCR").is_valid()) {
                                            ImGui::Text("  Description: %s", prop.get_metadata("GUI_DESCR").to_string().c_str());
                                        }
                                    }
                                }
                            }
                        } else {
                            ImGui::Text("Component (not valid): %s ", std::string(storage.type().name()).c_str());
                        }
                        ImGui::Separator();
                    }

                    ImGui::End();
                }
            }
        );

        RegisterSystems<ES::Plugin::RenderingPipeline::ToGPU>([](ES::Engine::Core &core){
            ImGui::Render();
        });

        RegisterSystems<ES::Plugin::RenderingPipeline::ToGPU>([](ES::Engine::Core &core){
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        });
    }
};

void AddQuad(ES::Engine::Core &core)
{
    using namespace glm;

    auto quad = ES::Engine::Entity(core.GetRegistry().create());

    ES::Plugin::Object::Component::Mesh mesh;

    mesh.vertices = {
        glm::vec3(-1, 1, 0),
        glm::vec3(1, 1, 0),
        glm::vec3(-1, -1, 0),
        glm::vec3(1, -1, 0)
    };

    mesh.normals = {
        glm::vec3(0, 0, -1),
        glm::vec3(0, 0, -1),
        glm::vec3(0, 0, -1),
        glm::vec3(0, 0, -1)
    };

    mesh.indices = {2, 0, 1, 2, 1, 3};

    quad.AddComponent<ES::Plugin::Object::Component::Mesh>(core, mesh);
    auto &transform = quad.AddComponent<ES::Plugin::Object::Component::Transform>(core);

    transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
    transform.rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    transform.scale = glm::vec3(10.0f, 10.0f, 10.0f);

    quad.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, ES::Plugin::OpenGL::Component::ShaderHandle("default"));
    quad.AddComponent<ES::Plugin::OpenGL::Component::MaterialHandle>(core, ES::Plugin::OpenGL::Component::MaterialHandle("default"));
    quad.AddComponent<ES::Plugin::OpenGL::Component::ModelHandle>(core, ES::Plugin::OpenGL::Component::ModelHandle("floor"));
}

auto main(int, char**) -> int {
    ES::Engine::Core core;

    core.AddPlugins<OpenGLIMGUI>();

    rttr::registration::class_<ES::Plugin::Object::Component::Transform>(std::to_string(entt::type_hash<ES::Plugin::Object::Component::Transform>::value()))
        .property("position", &ES::Plugin::Object::Component::Transform::position)
        .property("rotation", &ES::Plugin::Object::Component::Transform::rotation)
        .property("scale", &ES::Plugin::Object::Component::Transform::scale);
        
    rttr::registration::class_<ES::Plugin::Object::Component::Transform>(std::to_string(entt::type_hash<ES::Plugin::Object::Component::Transform>::value()))
        (
            rttr::metadata("GUI_LABEL", "Transform"),
            rttr::metadata("GUI_DESCR", "The transform component."),
            rttr::metadata("GUI_DRAW", std::function<void(ES::Engine::Core &, ES::Engine::Entity)>([](ES::Engine::Core &core, ES::Engine::Entity ent) {
                ImGui::Text("Transform Component");
                ImGui::InputFloat3("Position", &core.GetRegistry().get<ES::Plugin::Object::Component::Transform>(ent).position[0]);
                glm::vec3 rotation = glm::eulerAngles(core.GetRegistry().get<ES::Plugin::Object::Component::Transform>(ent).rotation);
                ImGui::SliderFloat3("Rotation", &rotation[0], -3.14f / 2.f, 3.14f / 2.f);
                core.GetRegistry().get<ES::Plugin::Object::Component::Transform>(ent).rotation = glm::quat(rotation);
                ImGui::InputFloat3("Scale", &core.GetRegistry().get<ES::Plugin::Object::Component::Transform>(ent).scale[0]);
            }))
        );
    
        

    rttr::registration::class_<ES::Plugin::Object::Component::Mesh>(std::string{entt::type_id<ES::Plugin::Object::Component::Mesh>().name()})
        (
            rttr::metadata("GUI_LABEL", "Mesh"),
            rttr::metadata("GUI_DESCR", "The mesh component.")
        );
    rttr::registration::class_<ES::Plugin::OpenGL::Component::ShaderHandle>(std::to_string(entt::type_hash<ES::Plugin::OpenGL::Component::ShaderHandle>::value()))
        (
            rttr::metadata("GUI_LABEL", "ShaderHandle"),
            rttr::metadata("GUI_DESCR", "The shader handle component.")
        );
    rttr::registration::class_<ES::Plugin::OpenGL::Component::MaterialHandle>(std::to_string(entt::type_hash<ES::Plugin::OpenGL::Component::MaterialHandle>::value()))
        (
            rttr::metadata("GUI_LABEL", "MaterialHandle"),
            rttr::metadata("GUI_DESCR", "The material handle component.")
        );
    rttr::registration::class_<ES::Plugin::OpenGL::Component::ModelHandle>(std::to_string(entt::type_hash<ES::Plugin::OpenGL::Component::ModelHandle>::value()))
        (
            rttr::metadata("GUI_LABEL", "ModelHandle"),
            rttr::metadata("GUI_DESCR", "The model handle component.")
        );

    rttr::registration::class_<ES::Plugin::OpenGL::Component::FontHandle>(std::to_string(entt::type_hash<ES::Plugin::OpenGL::Component::FontHandle>::value()))
        (
            rttr::metadata("GUI_LABEL", "FontHandle"),
            rttr::metadata("GUI_DESCR", "The font handle component.")
        );
    rttr::registration::class_<ES::Plugin::OpenGL::Component::TextHandle>(std::to_string(entt::type_hash<ES::Plugin::OpenGL::Component::TextHandle>::value()))
        (
            rttr::metadata("GUI_LABEL", "TextHandle"),
            rttr::metadata("GUI_DESCR", "The text handle component.")
        );
    rttr::registration::class_<ES::Plugin::UI::Component::Text>(std::to_string(entt::type_hash<ES::Plugin::UI::Component::Text>::value()))
        (
            rttr::metadata("GUI_LABEL", "Text"),
            rttr::metadata("GUI_DESCR", "The text component.")
        )
        .property("text", &ES::Plugin::UI::Component::Text::text)
        (
            rttr::metadata("GUI_LABEL", "Text."),
            rttr::metadata("GUI_DESCR", "The text to display.")
        )
        .property("color", &ES::Plugin::UI::Component::Text::color)
        (
            rttr::metadata("GUI_LABEL", "Color."),
            rttr::metadata("GUI_DESCR", "The color of the text.")
        );



    core.RegisterSystem<ES::Engine::Scheduler::Startup>(AddQuad);

    core.RunCore();
    return 0;
}