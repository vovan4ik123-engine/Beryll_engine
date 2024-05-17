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
        ImGui_ImplOpenGL3_Init(); // glsl_version = "#version 300 es" = default.

        // Complete initialization by calling first frame.
        // Start the ImGui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(Window::getInstance()->getWindow());
        ImGui::NewFrame();
        // Rendering ImGUI.
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Load default font.
        float fontHeight = 0.05f; // 5.0% of screen height.

        uint32_t bufferSize = 0;
        char* buffer = BeryllUtils::File::readToBuffer("fonts/roboto.ttf", &bufferSize);
        ImFont* fontDefault = io.Fonts->AddFontFromMemoryTTF(buffer, bufferSize, fontHeight  * ImGui::GetIO().DisplaySize.y);
        BR_ASSERT((fontDefault != nullptr), "%s", "Font was not created.");
        m_loadedFonts.emplace_back(fontDefault, "fonts/roboto.ttf", fontHeight);
        ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui::GetIO().FontDefault = fontDefault;

        ImGui::GetCurrentContext()->Style.FramePadding = ImVec2{0.0f, 0.0f};

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
        // Start the Dear ImGui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(Window::getInstance()->getWindow());
        ImGui::NewFrame();
    }

    void AndroidGLESImGUI::endFrame()
    {
        // Rendering ImGUI.
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

    float AndroidGLESImGUI::getGUIScreenAspectRation()
    {
        if(ImGui::GetIO().DisplaySize.x > ImGui::GetIO().DisplaySize.y)
            return ImGui::GetIO().DisplaySize.x / ImGui::GetIO().DisplaySize.y;
        else
            return ImGui::GetIO().DisplaySize.y / ImGui::GetIO().DisplaySize.x;
    }

    ImFont* AndroidGLESImGUI::createFont(const std::string& path, float fontHeight)
    {
        ImFont* storedFont = findStoredFont(path, fontHeight);
        if(storedFont)
        {
            BR_INFO("%s", "Font found. Return it.");
            return storedFont;
        }

        uint32_t bufferSize = 0;
        char* buffer = BeryllUtils::File::readToBuffer(path.c_str(), &bufferSize);
        ImFont* f = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buffer, bufferSize, fontHeight * ImGui::GetIO().DisplaySize.y);
        BR_ASSERT((f != nullptr), "%s", "Font was not created.");
        m_loadedFonts.emplace_back(f, path, fontHeight);
        ImGui_ImplOpenGL3_CreateFontsTexture();

        return f;
    }

    void AndroidGLESImGUI::deleteFont(ImFont* font)
    {
        BR_ASSERT(false, "%s", "ImGUI does not support delete one font. Only ImGui::GetIO().Fonts->Clear(); can be called to delete all fonts.");
    }

    void AndroidGLESImGUI::setDefaultFont(const std::string& path, float fontHeight)
    {
        ImGui::GetIO().FontDefault = createFont(path, fontHeight);
    }
}
