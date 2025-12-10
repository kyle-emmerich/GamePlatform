#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include "Core/Event.h"
#include "Core/LogSystem.h"
#include "Rendering/IRenderable.h"
#include "ClientShared/Export.h"
#include "Input/Input.h"
#include "Input/KeyCode.h"

class Engine;
class CLIENT_API Console : public IRenderable {
public:
    struct Message {
        std::string text;
        LogSystem::Level level;
    };

    Console(Engine* engine);
    virtual ~Console();

    void OnAttached(Viewport* viewport) override;
    void OnDetached(Viewport* viewport) override;
    void OnRendered(Viewport* viewport) override;

    void SetEnabled(bool _enabled);
    bool IsEnabled() const;

    MulticastEvent<std::string> TextEntered;

    /// @brief Set to KeyCode::None to disable keybind toggling of console.
    KeyCode ToggleKey = KeyCode::GraveAccent; // `/~

    int GetScrollPosition() const { return windowY; }
    void ScrollTo(int line);
    void ScrollToBottom();
    void Clear();
private:
    Engine* engine = nullptr;

    bool enabled = false;
    bool enabledChangedFlag = false;

    std::string inputBuffer;
    size_t inputCursorPosition = 0;

    std::deque<std::string> inputHistory;
    int maxInputHistoryEntries = 100;
    int inputHistoryIndex = -1;

    std::deque<Message> messages;
    const size_t maxLines = 1024;
    int windowY = 0;
    const int windowLines = 20;

    int caretFlashTimer = 0;
    const int caretFlashInterval = 30; //frames

    MulticastEvent<std::string, LogSystem::Level>::Listener* logMessageListener = nullptr;
    MulticastEvent<std::string>::Listener* windowTextReceivedListener = nullptr;
    MulticastEvent<class Input*>::Listener* windowKeyInputListener = nullptr;
};