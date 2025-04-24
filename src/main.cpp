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

#include "InitImGui.hpp"
#include "SetupImGui.hpp"
#include "AddSelectedEntity.hpp"
#include "NewFrame.hpp"
#include "DrawHierarchy.hpp"
#include "DrawComponents.hpp"
#include "RenderFrame.hpp"

#include "SelectedEntity.hpp"

#include "Types.hpp"

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
        RegisterSystems<ES::Plugin::RenderingPipeline::Init>(ES::Editor::System::InitImGui);
        RegisterSystems<ES::Plugin::RenderingPipeline::Setup>(ES::Editor::System::SetupImGui);
        RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Editor::System::AddSelectedEntity);
        RegisterSystems<ES::Plugin::RenderingPipeline::PreUpdate>(ES::Editor::System::NewFrame);
        RegisterSystems<ES::Engine::Scheduler::Update>(
            // Utily call to show the demo window
            // [](ES::Engine::Core &core){
            //     ImGui::ShowDemoWindow();
            // },
            ES::Editor::System::DrawHierarchy,
            ES::Editor::System::DrawComponents
        );
        RegisterSystems<ES::Plugin::RenderingPipeline::ToGPU>(
            ES::Editor::System::RenderFrame
        );
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