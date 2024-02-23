#pragma once

#include <Logger.hpp>
#include <GLInclude.hpp>
#include <GLFWInclude.hpp>
#include <Input.hpp>
#include <glm/glm.hpp>
#include <VisualizerScene.hpp>
#include <LIDARFrameGrabber.hpp>

#include <memory>
#include <inttypes.h>

namespace em {
    struct AppParams
    {
        uint8_t msaaSamples = 2;
        uint16_t width = 800;
        uint16_t height = 600;
        uint16_t minWidth = 480;
        uint16_t minHeight = 320;
        bool fullscreen = false;
        bool vsync = true;
        bool resizable = true;
    };

    class VisualizerApp 
    {
    public:
        VisualizerApp();

        bool start(AppParams& settings);
        bool shouldClose();
        bool runLoop();
        bool terminate();

        void setFullscreen(bool fullscreen);
        glm::ivec2 getWindowSize();

        AppParams getParams();
        const Input& getInput();
        GLFWwindow* getWindowHandle();
        LIDARFrameGrabber* getLIDARFrameGrabber();

        static VisualizerApp& getInstance();
    private:
        Logger m_logger;
        AppParams m_params;
        GLFWwindow* m_window;
        std::unique_ptr<Input> m_input;
        bool m_fullscreen;
        bool m_shouldClose;
        bool m_initialized;
        double m_lastFrameTime;
        glm::ivec2 m_windowSize;

        VisualizerScene m_scene;

        std::unique_ptr<LIDARFrameGrabber> m_frameGrabber;

        void genUI();

        static void onWindowResize(GLFWwindow* window, int width, int height);
    };
}