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
#include "InputManager.hpp"
#include "Logger.hpp"

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
        RegisterSystems<ES::Plugin::RenderingPipeline::Setup>(ES::Editor::System::SetupImGui,
        [](ES::Engine::Core &){
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            
        },[](ES::Engine::Core &core) {
            auto &inputManager = core.GetResource<ES::Plugin::Input::Resource::InputManager>();
            inputManager.RegisterCursorEnterCallback(
                [](ES::Engine::Core &c, int entered){
                    ImGui_ImplGlfw_CursorEnterCallback(c.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow(), entered);
                });
            inputManager.RegisterCursorPosCallback(
                [](ES::Engine::Core &c, double x, double y){
                    ImGui_ImplGlfw_CursorPosCallback(c.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow(), x, y);
                });
            inputManager.RegisterMouseButtonCallback(
                [](ES::Engine::Core &c, int button, int action, int mods) {
                    ImGui_ImplGlfw_MouseButtonCallback(c.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow(), button, action, mods);
                });
            inputManager.RegisterScrollCallback(
                [](ES::Engine::Core &c, double xoffset, double yoffset){
                    ImGui_ImplGlfw_ScrollCallback(c.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow(), xoffset, yoffset);
                });
            inputManager.RegisterKeyCallback(
                [](ES::Engine::Core &c, int key, int scancode, int action, int mods){
                    ImGui_ImplGlfw_KeyCallback(c.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow(), key, scancode, action, mods);
                });
            inputManager.RegisterCharCallback(
                [](ES::Engine::Core &c, unsigned int ch){
                    ImGui_ImplGlfw_CharCallback(c.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow(), ch);
                });
        });
        RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Editor::System::AddSelectedEntity);
        RegisterSystems<ES::Engine::Scheduler::Update>(
            [](ES::Engine::Core &core){
                core.GetResource<ES::Plugin::OpenGL::Utils::Framebuffer>().UnBind();
            },
            ES::Editor::System::NewFrame,
            [](ES::Engine::Core &core){
                static bool docking_enabled = true;
                static bool opt_fullscreen = true;
                static bool opt_padding = false;
                static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

                // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
                // because it would be confusing to have two docking targets within each others.
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
                if (opt_fullscreen)
                {
                    const ImGuiViewport* viewport = ImGui::GetMainViewport();
                    ImGui::SetNextWindowPos(viewport->WorkPos);
                    ImGui::SetNextWindowSize(viewport->WorkSize);
                    ImGui::SetNextWindowViewport(viewport->ID);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
                }
                else
                {
                    dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
                }

                // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
                // and handle the pass-thru hole, so we ask Begin() to not render a background.
                if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                    window_flags |= ImGuiWindowFlags_NoBackground;

                // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
                // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
                // all active windows docked into it will lose their parent and become undocked.
                // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
                // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
                if (!opt_padding)
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::Begin("DockSpace Demo", &docking_enabled, window_flags);
                if (!opt_padding)
                    ImGui::PopStyleVar();

                if (opt_fullscreen)
                    ImGui::PopStyleVar(2);

                // Submit the DockSpace
                ImGuiIO& io = ImGui::GetIO();
                if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
                {
                    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
                }

                if (ImGui::BeginMenuBar())
                {
                    if (ImGui::BeginMenu("Options"))
                    {
                        if (ImGui::MenuItem("Close"))
                            core.Stop();
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenuBar();
                }
                ImGui::End();      
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::Begin("Viewport");
                // tmp
                static glm::vec2 _viewportSize = {0.0f, 0.0f};
                auto viewportPanelSize = ImGui::GetContentRegionAvail();
                if (viewportPanelSize.x != _viewportSize.x || viewportPanelSize.y != _viewportSize.y) {
                    _viewportSize = {viewportPanelSize.x, viewportPanelSize.y};
                    core.GetResource<ES::Plugin::OpenGL::Utils::Framebuffer>().Resize(_viewportSize);
                    core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().size = {viewportPanelSize.x, viewportPanelSize.y};
                    core.GetResource<ES::Plugin::OpenGL::Resource::Camera>().viewer.setAspectRatio(viewportPanelSize.x / viewportPanelSize.y);
                }
                ImGui::Image(core.GetResource<ES::Plugin::OpenGL::Utils::Framebuffer>().GetColorAttachment(), {_viewportSize.x, _viewportSize.y}, ImVec2(0, 1), ImVec2(1, 0));
                ImGui::End();
                ImGui::PopStyleVar();
            },
            // Utily call to show the demo window
            [](ES::Engine::Core &){
                ImGui::ShowDemoWindow();
            },
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