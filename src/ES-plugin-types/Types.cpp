#include "ObjectTypes.hpp"
#include "OpenGLTypes.hpp"
#include "UITypes.hpp"

namespace ES::Editor::Types {
    void RegisterTypes(void)
    {
        ES::Editor::Types::Object::RegisterTypes();
        ES::Editor::Types::OpenGL::RegisterTypes();
        ES::Editor::Types::UI::RegisterTypes();
    }
}