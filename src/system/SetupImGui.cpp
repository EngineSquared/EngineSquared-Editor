#include "SetupImGui.hpp"
#include "Window.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace ES::Editor::System {

    void SetupImGui(ES::Engine::Core &core)
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(core.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 130");
    }
    
}
    