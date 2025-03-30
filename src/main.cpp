#include "Core.hpp"
#include "OpenGL.hpp"
#include "APlugin.hpp"
#include "Startup.hpp"
#include "Startup.hpp"
#include "system/WindowSystem.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class OpenGLIMGUI : public ES::Engine::APlugin {
    public:
      explicit OpenGLIMGUI(ES::Engine::Core &core)
          : ES::Engine::APlugin(core){
                // empty
            };
      ~OpenGLIMGUI() = default;
  
      void Bind() final {
        RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::InitGLFW);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::SetupGLFWHints);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::CreateWindowSystem);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::LinkGLFWContextToGL);

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::InitGLEW);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::CheckGLEWVersion);

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::EnableVSync);

    RegisterSystems<ES::Engine::Scheduler::Startup>([](ES::Engine::Core &core){
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(core.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 130");
    });

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::LoadButtons);

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadFontManager);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadMaterialCache);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadShaderManager);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadDefaultShader);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadDefaultTextShader);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::CreateCamera);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::SetupShaderUniforms);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::SetupTextShaderUniforms);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadGLMeshBufferManager);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadGLTextBufferManager);

    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::PollEvents);

    RegisterSystems<ES::Engine::Scheduler::Update>([](ES::Engine::Core &core){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        static bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);
        ImGui::Render();
    });

    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::UpdateKey);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::UpdatePosCursor);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::UpdateButton);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::SaveLastMousePos);

    RegisterSystems<ES::Engine::Scheduler::Update>([](ES::Engine::Core &core){
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });

    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::SwapBuffers);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::StopSystems);

    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::MouseDragging);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::UpdateMatrices);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLClearColor);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLClearDepth);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLEnableDepth);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLEnableCullFace);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::SetupCamera);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::SetupLights);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::LoadGLMeshBuffer);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::LoadGLTextBuffer);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::RenderMeshes);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::RenderText);
    }
};

auto main(int, char**) -> int {
    ES::Engine::Core core;

    core.AddPlugins<OpenGLIMGUI>();



    core.RunCore();
    return 0;
}