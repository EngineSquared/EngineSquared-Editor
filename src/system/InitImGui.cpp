#include "InitImGui.hpp"

#include "imgui.h"

namespace ES::Editor::System {

    void InitImGui(const ES::Engine::Core &)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
    }
    
}
    