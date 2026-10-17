#include "toast-manager.h"

void ToastManager::Show(const std::string &message, float duration)
{
    toasts_.push_back({
        message,
        duration
    });
}

void ToastManager::Render()
{
    const ImGuiIO& io = ImGui::GetIO();

    for (Toast& toast : toasts_)
    {
        toast.remaining -= io.DeltaTime;
    }

    std::erase_if(toasts_, [](const Toast& toast)
    {
        return toast.remaining <= 0.0f;
    });

    if (toasts_.empty()) return;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    constexpr float MARGIN = 20.0f;
    constexpr float SPACING = 8.0f;

    float y = viewport->WorkPos.y + viewport->WorkSize.y - MARGIN;
    for (std::size_t i = 0; i < toasts_.size(); i++)
    {
        Toast& toast = toasts_[i];

        ImGui::SetNextWindowPos(
            ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - MARGIN, y),
            ImGuiCond_Always,
            ImVec2(1.0f, 1.0f)
        );

        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove;

        const std::string id = "##Toast" + std::to_string(i);
        if (ImGui::Begin(id.c_str(), nullptr, flags))
        {
            ImGui::TextUnformatted(toast.message.c_str());
            y -= ImGui::GetWindowSize().y + SPACING;
        }

        ImGui::End();
    }
}
