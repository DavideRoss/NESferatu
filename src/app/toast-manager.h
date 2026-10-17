#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include <imgui.h>

class ToastManager
{
public:
    void Show(const std::string& message, float duration = 2.0f);
    void Render();

private:
    struct Toast
    {
        std::string message;
        float remaining;
    };

    std::vector<Toast> toasts_;
};
