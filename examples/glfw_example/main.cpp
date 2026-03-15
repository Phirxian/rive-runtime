/**
 * Minimal Rive + GLFW example.
 * Usage: glfw_example [path/to/file.riv]
 * Opens a window and plays the first animation from the .riv file.
 */
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "rive/artboard.hpp"
#include "rive/file.hpp"
#include "rive/static_scene.hpp"
#include "rive/animation/linear_animation_instance.hpp"
#include "rive/math/mat2d.hpp"
#include "rive/renderer/rive_renderer.hpp"
#include "rive/renderer/gl/render_context_gl_impl.hpp"
#include "rive/renderer/gl/render_target_gl.hpp"
#include "rive/refcnt.hpp"

#ifdef RIVE_DESKTOP_GL
#include "glad/glad_custom.h"
#endif

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

using namespace rive;
using namespace rive::gpu;

static std::unique_ptr<RenderContext> g_renderContext;
static rcp<RenderTargetGL> g_renderTarget;
static std::unique_ptr<RiveRenderer> g_renderer;
static rcp<File> g_rivFile;
static std::unique_ptr<ArtboardInstance> g_artboard;
static std::unique_ptr<Scene> g_scene;
static int g_width = 800, g_height = 600;

static void loadRivFile(const char* path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        std::cerr << "Failed to open: " << path << std::endl;
        return;
    }
    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(f)), {});
    f.close();
    if (bytes.empty()) {
        std::cerr << "Empty file: " << path << std::endl;
        return;
    }
    g_rivFile = File::import(bytes, g_renderContext.get());
    if (!g_rivFile) {
        std::cerr << "Failed to import .riv file" << std::endl;
        return;
    }
    g_artboard = g_rivFile->artboardDefault();
    if (!g_artboard) {
        std::cerr << "No default artboard" << std::endl;
        return;
    }
    g_scene = g_artboard->defaultScene();
    if (!g_scene) {
        auto anim = g_artboard->animationAt(0);
        if (anim) {
            g_scene = std::move(anim);
        } else {
            g_scene = std::make_unique<StaticScene>(g_artboard.get());
        }
    }
    std::cout << "Loaded: " << path << std::endl;
}

static void onFramebufferSize(GLFWwindow* window, int width, int height) {
    g_width = width > 0 ? width : 1;
    g_height = height > 0 ? height : 1;
    g_renderTarget = make_rcp<FramebufferRenderTargetGL>(
        static_cast<uint32_t>(g_width),
        static_cast<uint32_t>(g_height),
        0,
        0
    );
    glViewport(0, 0, g_width, g_height);
}

int main(int argc, char* argv[]) {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(g_width, g_height, "Rive GLFW Example", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

#ifdef RIVE_DESKTOP_GL
    if (!gladLoadCustomLoader(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
#endif

    g_renderContext = RenderContextGLImpl::MakeContext({});
    if (!g_renderContext) {
        std::cerr << "Failed to create render context" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    g_renderTarget = make_rcp<FramebufferRenderTargetGL>(
        static_cast<uint32_t>(g_width),
        static_cast<uint32_t>(g_height),
        0,
        0
    );
    g_renderer = std::make_unique<RiveRenderer>(g_renderContext.get());

    const char* rivPath = (argc > 1) ? argv[1] : nullptr;
    if (rivPath) {
        loadRivFile(rivPath);
    } else {
        std::cout << "Usage: glfw_example <file.riv>" << std::endl;
    }

    glfwSetFramebufferSizeCallback(window, onFramebufferSize);

    auto lastTime = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (g_renderContext && g_renderTarget && g_renderer) {
            auto* glImpl = g_renderContext->static_impl_cast<RenderContextGLImpl>();
            glImpl->invalidateGLState();
            g_renderContext->beginFrame({
                .renderTargetWidth = static_cast<uint32_t>(g_width),
                .renderTargetHeight = static_cast<uint32_t>(g_height),
                .clearColor = 0xff303030,
                .msaaSampleCount = 0,
            });

            if (g_scene && g_artboard) {
                auto now = std::chrono::steady_clock::now();
                float dt = std::chrono::duration<float>(now - lastTime).count();
                lastTime = now;
                g_scene->advanceAndApply(dt);

                Mat2D m = computeAlignment(
                    Fit::contain,
                    Alignment::center,
                    AABB(0, 0, static_cast<float>(g_width), static_cast<float>(g_height)),
                    g_artboard->bounds()
                );
                g_renderer->save();
                g_renderer->transform(m);
                g_scene->draw(g_renderer.get());
                g_renderer->restore();
            }

            g_renderContext->flush({
                .renderTarget = g_renderTarget.get(),
            });
            glImpl->unbindGLInternalResources();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        glfwSwapBuffers(window);
    }

    g_scene.reset();
    g_artboard.reset();
    g_rivFile.reset();
    g_renderer.reset();
    g_renderTarget.reset();
    g_renderContext.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
