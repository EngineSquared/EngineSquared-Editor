#include "AddSelectedEntity.hpp"
#include "SelectedEntity.hpp"

namespace ES::Editor::System {

    void AddSelectedEntity(ES::Engine::Core &core)
    {
        core.RegisterResource<ES::Editor::Resource::SelectedEntity>(ES::Editor::Resource::SelectedEntity());
    }
    
}
    