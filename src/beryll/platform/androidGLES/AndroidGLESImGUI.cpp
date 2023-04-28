#include "AndroidGLESImGUI.h"
#include "beryll/core/Window.h"
#include "beryll/utils/File.h"
#include "beryll/GUI/ButtonWithText.h"
#include "beryll/GUI/CheckBox.h"
#include "beryll/GUI/Text.h"
#include "beryll/GUI/Slider.h"

namespace Beryll
{
    AndroidGLESImGUI::AndroidGLESImGUI()
    {
        init();
    }

    AndroidGLESImGUI::~AndroidGLESImGUI()
    {
        destroy();
    }

    void AndroidGLESImGUI::init()
    {
        if(ImGui::GetCurrentContext())
        {
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(Window::getInstance()->getWindow(), Window::getInstance()->getGlContext());
        ImGui_ImplOpenGL3_Init(); // glsl_version = "#version 300 es" = default

        // complete initialization calling first frame
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(Window::getInstance()->getWindow());
        ImGui::NewFrame();
        // Rendering ImGUI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // load fonts
        uint32_t bufferSize = 0;
        char* buffer = nullptr;

        if(!m_defaultFontPath.empty())
        {
            buffer = Utils::File::readToBuffer(m_defaultFontPath.c_str(), &bufferSize);
            ImFont* fontDefault = io.Fonts->AddFontFromMemoryTTF(buffer, bufferSize, m_defaultFontHeight  * ImGui::GetIO().DisplaySize.y);
            BR_ASSERT((fontDefault != nullptr), "%s", "font nullptr");
            ImGui_ImplOpenGL3_CreateFontsTexture();
            ImGui::GetIO().FontDefault = fontDefault;
        }
        else
        {
            buffer = Utils::File::readToBuffer("fonts/roboto.ttf", &bufferSize); // load from ...../aseets/fonts/roboto.ttf
            ImFont* fontDefault = io.Fonts->AddFontFromMemoryTTF(buffer, bufferSize, m_defaultFontHeight  * ImGui::GetIO().DisplaySize.y);
            BR_ASSERT((fontDefault != nullptr), "%s", "font nullptr");
            ImGui_ImplOpenGL3_CreateFontsTexture();
            io.FontDefault = fontDefault;
        }

        if(!ButtonWithText::m_fontPath.empty() && ButtonWithText::m_fontHeight != 0.0f)
        {
            buffer = Utils::File::readToBuffer(ButtonWithText::m_fontPath.c_str(), &bufferSize);
            ImFont* f = io.Fonts->AddFontFromMemoryTTF(buffer, bufferSize, ButtonWithText::m_fontHeight * ImGui::GetIO().DisplaySize.y);
            BR_ASSERT((f != nullptr), "%s", "font nullptr");
            ImGui_ImplOpenGL3_CreateFontsTexture();
            ButtonWithText::m_font = f;
        }
        else
        {
            ButtonWithText::m_font = nullptr;
        }

        if(!CheckBox::m_fontPath.empty() && CheckBox::m_fontHeight != 0.0f)
        {
            buffer = Utils::File::readToBuffer(CheckBox::m_fontPath.c_str(), &bufferSize);
            ImFont* f = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, CheckBox::m_fontHeight * ImGui::GetIO().DisplaySize.y);
            BR_ASSERT((f != nullptr), "%s", "font nullptr");
            ImGui_ImplOpenGL3_CreateFontsTexture();
            CheckBox::m_font = f;
        }
        else
        {
            CheckBox::m_font = nullptr;
        }

        if(!Text::m_fontPath.empty() && Text::m_fontHeight != 0.0f)
        {
            buffer = Utils::File::readToBuffer(Text::m_fontPath.c_str(), &bufferSize);
            ImFont* f = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, Text::m_fontHeight * ImGui::GetIO().DisplaySize.y);
            BR_ASSERT((f != nullptr), "%s", "font nullptr");
            ImGui_ImplOpenGL3_CreateFontsTexture();
            Text::m_font = f;
        }
        else
        {
            Text::m_font = nullptr;
        }

        if(!Slider::m_fontPath.empty() && Slider::m_fontHeight != 0.0f)
        {
            buffer = Utils::File::readToBuffer(Slider::m_fontPath.c_str(), &bufferSize);
            ImFont* f = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, Slider::m_fontHeight * ImGui::GetIO().DisplaySize.y);
            BR_ASSERT((f != nullptr), "%s", "font nullptr");
            ImGui_ImplOpenGL3_CreateFontsTexture();
            Slider::m_font = f;
        }
        else
        {
            Slider::m_font = nullptr;
        }
    }

    void AndroidGLESImGUI::destroy()
    {
        ButtonWithText::m_font = nullptr;
        CheckBox::m_font = nullptr;
        Text::m_font = nullptr;
        Slider::m_font = nullptr;

        ImGui::GetIO().Fonts->Clear();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void AndroidGLESImGUI::reCreate()
    {
        destroy();
        init();
    }

    void AndroidGLESImGUI::beginFrame()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(Window::getInstance()->getWindow());
        ImGui::NewFrame();
    }

    void AndroidGLESImGUI::endFrame()
    {
        // Rendering ImGUI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    float AndroidGLESImGUI::getGUIWidth()
    {
        return ImGui::GetIO().DisplaySize.x;
    }

    float AndroidGLESImGUI::getGUIHeight()
    {
        return ImGui::GetIO().DisplaySize.y;
    }

    void AndroidGLESImGUI::setDefaultFont(const char* path, float heightInPercent)
    {
        m_defaultFontPath = path;
        m_defaultFontHeight = heightInPercent / 100.0f;

        uint32_t bufferSize = 0;
        char *buffer = Utils::File::readToBuffer(m_defaultFontPath.c_str(), &bufferSize);
        ImFont* fontDefault = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, m_defaultFontHeight  * ImGui::GetIO().DisplaySize.y);
        BR_ASSERT((fontDefault != nullptr), "%s", "font nullptr");
        ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui::GetIO().FontDefault = fontDefault;
    }

    void AndroidGLESImGUI::setDefaultFontHeight(float heightInPercent)
    {
        m_defaultFontHeight = heightInPercent / 100.0f;

        uint32_t bufferSize = 0;
        char* buffer = Utils::File::readToBuffer("fonts/roboto.ttf", &bufferSize); // load from ...../aseets/fonts/ folder
        ImFont* fontDefault = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, m_defaultFontHeight  * ImGui::GetIO().DisplaySize.y);
        BR_ASSERT((fontDefault != nullptr), "%s", "font nullptr");
        ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui::GetIO().FontDefault = fontDefault;
    }

    void AndroidGLESImGUI::setButtonsFont(const char* path, float heightInPercent)
    {
        ButtonWithText::m_fontPath = path;
        ButtonWithText::m_fontHeight = heightInPercent / 100.0f;

        uint32_t bufferSize = 0;
        char *buffer = Utils::File::readToBuffer(ButtonWithText::m_fontPath.c_str(), &bufferSize);
        ImFont* f = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, ButtonWithText::m_fontHeight * ImGui::GetIO().DisplaySize.y);
        BR_ASSERT((f != nullptr), "%s", "font nullptr");
        ImGui_ImplOpenGL3_CreateFontsTexture();
        ButtonWithText::m_font = f;
    }

    void AndroidGLESImGUI::setCheckBoxesFont(const char* path, float heightInPercent)
    {
        CheckBox::m_fontPath = path;
        CheckBox::m_fontHeight = heightInPercent / 100.0f;

        uint32_t bufferSize = 0;
        char *buffer = Utils::File::readToBuffer(CheckBox::m_fontPath.c_str(), &bufferSize);
        ImFont* f = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, CheckBox::m_fontHeight * ImGui::GetIO().DisplaySize.y);
        BR_ASSERT((f != nullptr), "%s", "font nullptr");
        ImGui_ImplOpenGL3_CreateFontsTexture();
        CheckBox::m_font = f;
    }

    void AndroidGLESImGUI::setTextsFont(const char* path, float heightInPercent)
    {
        Text::m_fontPath = path;
        Text::m_fontHeight = heightInPercent / 100.0f;

        uint32_t bufferSize = 0;
        char *buffer = Utils::File::readToBuffer(Text::m_fontPath.c_str(), &bufferSize);
        ImFont* f = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, Text::m_fontHeight * ImGui::GetIO().DisplaySize.y);
        BR_ASSERT((f != nullptr), "%s", "font nullptr");
        ImGui_ImplOpenGL3_CreateFontsTexture();
        Text::m_font = f;
    }

    void AndroidGLESImGUI::setSlidersFont(const char* path, float heightInPercent)
    {
        Slider::m_fontPath = path;
        Slider::m_fontHeight = heightInPercent / 100.0f;

        uint32_t bufferSize = 0;
        char *buffer = Utils::File::readToBuffer(Slider::m_fontPath.c_str(), &bufferSize);
        ImFont* f = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, Slider::m_fontHeight * ImGui::GetIO().DisplaySize.y);
        BR_ASSERT((f != nullptr), "%s", "font nullptr");
        ImGui_ImplOpenGL3_CreateFontsTexture();
        Slider::m_font = f;
    }
}