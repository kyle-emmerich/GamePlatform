
#include "Core/Engine.h"
#include "ClientShared/Console.h"
#include "Platform/Viewport.h"
#include "Platform/PlatformWindow.h"
#include "Input/InputSystem.h"
#include "bgfx/bgfx.h"
#include <iostream>
#include <algorithm>

uint8_t colors[(int)LogSystem::Level::Max] = {
    0x0f, // Verbose - white
    0x8f, // Info - light gray
    0x0e, // Warning - yellow
    0x0c  // Error - red
};

Console::Console(Engine* engine)
    : engine(engine) {

    logMessageListener = &engine->GetSystem<LogSystem>()->MessageLogged.Connect(
        [this](const std::string& message, LogSystem::Level level) {
            bool isScrolledToBottom = (windowY + windowLines) >= (int)messages.size();

            messages.push_back({ message, level });
            while (messages.size() > maxLines) {
                messages.pop_front();
            }
            if (isScrolledToBottom) {
                ScrollToBottom();
            }
        }
    );
}
Console::~Console() {

}
void Console::OnAttached(Viewport* viewport) {

    windowTextReceivedListener = &viewport->GetAttachedWindow()->ReceivedTextInput.Connect(
        [this](const std::string& text) {
            if (!IsEnabled()) {
                return;
            }
            if (enabledChangedFlag) {
                enabledChangedFlag = false;
                return;
            }
            inputBuffer.insert(inputCursorPosition, text);
            inputCursorPosition += text.size();
        }
    );

    Engine* engine = viewport->GetAttachedWindow()->GetEngine();
    if (!engine) {
        return;
    }

    InputSystem* inputSystem = engine->GetSystem<InputSystem>();
    if (!inputSystem) {
        return;
    }

    windowKeyInputListener = &inputSystem->InputBegan.Connect(
        [this](Input* input) {
            KeyCode key = input->Key;
            InputState state = input->State;
            if (state == InputState::Begin) {
                if (key == ToggleKey) {
                    enabledChangedFlag = true;
                    SetEnabled(!IsEnabled());
                    return;
                }
                if (!IsEnabled()) {
                    return;
                }

                if (key == KeyCode::Return) {
                    messages.push_back({ " > " + inputBuffer, LogSystem::Level::Info });
                    TextEntered.Fire(inputBuffer);
                    
                    if (inputHistory.empty() || inputHistory.back() != inputBuffer) {
                        inputHistory.push_back(inputBuffer);
                        while (inputHistory.size() > maxInputHistoryEntries) {
                            inputHistory.pop_front();
                        }
                    }
                    inputHistoryIndex = -1;

                    inputBuffer.clear();
                    inputCursorPosition = 0;
                    ScrollToBottom();
                } else if (key == KeyCode::Backspace) {
                    if (!inputBuffer.empty()) {
                        if (inputCursorPosition > 0) {
                            inputBuffer.erase(inputCursorPosition - 1, 1);
                            inputCursorPosition--;
                        }
                    }
                } else if (key == KeyCode::UpArrow) {
                    if (!inputHistory.empty()) {
                        if (inputHistoryIndex == -1) {
                            inputHistoryIndex = (int)inputHistory.size() - 1;
                        } else if (inputHistoryIndex > 0) {
                            inputHistoryIndex--;
                        }
                        inputBuffer = inputHistory[inputHistoryIndex];
                        inputCursorPosition = inputBuffer.size();
                    }
                } else if (key == KeyCode::DownArrow) {
                    if (!inputHistory.empty()) {
                        if (inputHistoryIndex == -1) {
                            inputHistoryIndex = -1;
                            inputBuffer.clear();
                        } else if (inputHistoryIndex < (int)inputHistory.size() - 1) {
                            inputHistoryIndex++;
                            inputBuffer = inputHistory[inputHistoryIndex];
                            inputCursorPosition = inputBuffer.size();
                        }
                    }
                } else if (key == KeyCode::LeftArrow) {
                    if (inputCursorPosition > 0) {
                        inputCursorPosition--;
                    }
                } else if (key == KeyCode::RightArrow) {
                    if (inputCursorPosition < (int)inputBuffer.size()) {
                        inputCursorPosition++;
                    }
                } else if (key == KeyCode::DeleteForward) {
                    if (inputCursorPosition < (int)inputBuffer.size()) {
                        inputBuffer.erase(inputCursorPosition, 1);
                    }
                } else if (key == KeyCode::Home) {
                    inputCursorPosition = 0;
                } else if (key == KeyCode::End) {
                    inputCursorPosition = (int)inputBuffer.size();
                } else if (key == KeyCode::PageDown) {
                    ScrollTo(windowY + windowLines);
                } else if (key == KeyCode::PageUp) {
                    ScrollTo(windowY - windowLines);
                } else if (key == KeyCode::Tab) {
                    inputBuffer += "\t";
                }
            } else if (state == InputState::End) {

            }
        }
    );
}
void Console::OnDetached(Viewport* viewport) {

}
void Console::OnRendered(Viewport* viewport) {

    int x = 2;
    int y = 2;
    bgfx::dbgTextClear();
    if (enabled) {
        bgfx::setDebug(BGFX_DEBUG_TEXT);
        for (int i = windowY; i < std::min((int)messages.size(), windowY + windowLines); ++i) {
            const std::string& line = messages[i].text;
            LogSystem::Level level = messages[i].level;
            bgfx::dbgTextPrintf(x, y, colors[(int)level], "%s", line.c_str());
            y += 1;
        }
        y += 1;
        bgfx::dbgTextPrintf(x, y, 0x0f, " > %s", inputBuffer.c_str());
        caretFlashTimer = (caretFlashTimer + 1) % (caretFlashInterval * 2);
        //draw the caret at the cursor position
        if (caretFlashTimer < caretFlashInterval) {
            bgfx::dbgTextPrintf(x + inputCursorPosition + 3, y, 0xf0, " ");
        }
    } else {
        bgfx::setDebug(0);
    }
}

void Console::SetEnabled(bool enabled) {
    this->enabled = enabled;
}
bool Console::IsEnabled() const {
    return enabled;
}

void Console::ScrollTo(int line) {
    if (line < 0) {
        line = 0;
    }
    if (line > (int)messages.size() - windowLines) {
        line = std::max(0, (int)messages.size() - windowLines);
    }
    windowY = line;
}
void Console::ScrollToBottom() {
    windowY = std::max(0, (int)messages.size() - windowLines);
}
void Console::Clear() {
    messages.clear();
    windowY = 0;
}