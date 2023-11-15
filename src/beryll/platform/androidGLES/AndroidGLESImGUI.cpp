#include "AndroidGLESImGUI.h"
#include "beryll/core/Window.h"
#include "beryll/utils/File.h"

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

        // Setup Platform/Renderer backends.
        ImGui_ImplSDL2_InitForOpenGL(Window::getInstance()->getWindow(), Window::getInstance()->getGlContext());
        ImGui_ImplOpenGL3_Init(); // glsl_version = "#version 300 es" = default

        // Complete initialization by calling first frame.
        // Start the Dear ImGui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(Window::getInstance()->getWindow());
        ImGui::NewFrame();
        // Rendering ImGUI.
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Load default font.
        float fontHeight = 5.0f / 100.0f; // 5.0% of screen height

        uint32_t bufferSize = 0;
        char* buffer = Utils::File::readToBuffer("fonts/roboto.ttf", &bufferSize);
        ImFont* fontDefault = io.Fonts->AddFontFromMemoryTTF(buffer, bufferSize, fontHeight  * ImGui::GetIO().DisplaySize.y);
        BR_ASSERT((fontDefault != nullptr), "%s", "Font was not created.");
        m_loadedFonts.emplace_back(fontDefault, "fonts/roboto.ttf", 5.0f);
        ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui::GetIO().FontDefault = fontDefault;

        ImGui::GetCurrentContext()->Style.FramePadding = ImVec2{1.0f, 1.0f};

        BR_INFO("AndroidGLESImGUI created and initialized. width: %f, height: %f", ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
    }

    void AndroidGLESImGUI::destroy()
    {
        ImGui::GetIO().Fonts->Clear();
        m_loadedFonts.clear();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void AndroidGLESImGUI::reCreate()
    {
        // All GUI elements store pointers to theirs fonts and theirs positions on screen.
        // If screen change orientation PORTRAIT <-> LANDSCAPE all GUI elements must be destroyed and created again.
        BR_ASSERT(false, "%s", "Only if screen change orientation. All GUI elements must be destroyed and created again.");

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

    ImFont* AndroidGLESImGUI::createFont(const std::string& path, float heightInPercentOfScreen)
    {
        ImFont* storedFont = findStoredFont(path, heightInPercentOfScreen);
        if(storedFont)
        {
            BR_INFO("%s", "Font found. Return it.");
            return storedFont;
        }

        float fontHeight = heightInPercentOfScreen / 100.0f;

        uint32_t bufferSize = 0;
        char* buffer = Utils::File::readToBuffer(path.c_str(), &bufferSize);
        ImFont* f = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, fontHeight * ImGui::GetIO().DisplaySize.y);
        BR_ASSERT((f != nullptr), "%s", "Font was not created.");
        m_loadedFonts.emplace_back(f, path, heightInPercentOfScreen);
        ImGui_ImplOpenGL3_CreateFontsTexture();

        return f;
    }

    void AndroidGLESImGUI::deleteFont(ImFont* font)
    {
        // IMGUI does not support delete one font.
        // Only ImGui::GetIO().Fonts->Clear(); can be called to delete all fonts.
    }

    void AndroidGLESImGUI::setDefaultFont(const std::string& path, float heightInPercentOfScreen)
    {
        ImGui::GetIO().FontDefault = createFont(path, heightInPercentOfScreen);
    }
}
