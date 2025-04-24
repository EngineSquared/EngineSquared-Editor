#include "Core.hpp"
#include "OpenGL.hpp"
#include "RenderingPipeline.hpp"
#include "APlugin.hpp"
#include "Startup.hpp"
#include "Startup.hpp"
#include "system/WindowSystem.hpp"
#include <numbers>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <rttr/registration>

#include "Types.hpp"

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
                ImGui::ShowDemoWindow();
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
    ES::Editor::Types::RegisterTypes();

    core.RegisterSystem<ES::Engine::Scheduler::Startup>(AddQuad);

    core.RunCore();
    return 0;
}