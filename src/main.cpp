#include "Core.hpp"
#include "OpenGL.hpp"
#include "APlugin.hpp"
#include "Startup.hpp"
#include "Startup.hpp"
#include "system/WindowSystem.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <rttr/registration>

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

struct Test
{
    int a = 0;
    float b = 0.0f;
    std::string c = "Hello World!";
};

auto main(int, char**) -> int {
    ES::Engine::Core core;

    core.AddPlugins<OpenGLIMGUI>();

    rttr::registration::class_<Test>(std::to_string(entt::type_hash<Test>::value()))
        (
            rttr::metadata("GUI_LABEL", "Test Class"),
            rttr::metadata("GUI_DESCR", "This is a test class.")
        )
        .constructor<>()
        .property("a", &Test::a)
        (
            rttr::metadata("GUI_LABEL", "A."),
            rttr::metadata("GUI_DESCR", "The value of A.")
        )
        .property("b", &Test::b)
        (
            rttr::metadata("GUI_LABEL", "B."),
            rttr::metadata("GUI_DESCR", "The value of B.")
        )
        .property("c", &Test::c)
        (
            rttr::metadata("GUI_LABEL", "C."),
            rttr::metadata("GUI_DESCR", "The value of C.")
        );

    core.CreateEntity().AddComponent<Test>(core);
    core.CreateEntity().AddComponent<Test>(core);
    for (auto [id, storage]: core.GetRegistry().storage()) {
        std::string name = std::to_string(id);
        rttr::type type = rttr::type::get_by_name(name);
        if (type.is_valid()) {
            std::cout << "Type: " << type.get_name() << std::endl;
            for (const auto& prop : type.get_properties()) {
                std::cout << "Property: " << prop.get_name() << std::endl;
                std::cout << "  Type: " << prop.get_type().get_name() << std::endl;
                if (prop.get_metadata("GUI_LABEL").is_valid()) {
                    std::cout << "  GUI Label: " << prop.get_metadata("GUI_LABEL").to_string() << std::endl;
                }
                if (prop.get_metadata("GUI_DESCR").is_valid()) {
                    std::cout << "  GUI Description: " << prop.get_metadata("GUI_DESCR").to_string() << std::endl;
                }
            }
        } else {
            std::cout << "Type not found for ID: " << id << " with name: " << name << std::endl;
        }
    }

    core.RunCore();
    return 0;
}