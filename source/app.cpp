#include "app.h"
#include "c64scene.h"
#include "demoscene.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

void renderLoopCallback(void *arg) { static_cast<App *>(arg)->render(); }

void App::init() {
  glfwInit();
  basisInit();
#ifdef Emscripten
  mGlslVersionString = "#version 300 es\n";
#else
  mGlslVersionString = "#version 330 core\n";
#endif
  mScene = std::make_unique<C64Scene>(mGlslVersionString, *mCodebook);
}

int App::run() {
  init();

#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#endif

  mWindow = glfwCreateWindow(screen_width, screen_height, windows_title,
                             nullptr, nullptr);
  if (mWindow == nullptr) {
    std::cout << "window create failed";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(mWindow);
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "glad init failed";
    return -1;
  }

  glViewport(0, 0, screen_width, screen_height);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
  ImGui_ImplOpenGL3_Init(mGlslVersionString.c_str());

  mScene->init();
  mScene->onScreenSizeChanged({screen_width, screen_height});

#ifdef Emscripten
  emscripten_set_main_loop_arg(&renderLoopCallback, this, -1, 1);
#else
  while (!glfwWindowShouldClose(mWindow)) {
    render();
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(mWindow);
#endif

  glfwTerminate();
  return 0;
}

void App::render() {
  float currentFrame = glfwGetTime();
  mDeltaTime = currentFrame - mLastFrame;
  mLastFrame = currentFrame;

  mScene->render(mDeltaTime);
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  processInput(mWindow);

  int frameWidth, frameHeight;
  glfwGetFramebufferSize(mWindow, &frameWidth, &frameHeight);
  mScene->onScreenSizeChanged({frameWidth, frameHeight});
  glViewport(0, 0, frameWidth, frameHeight);
  glfwSwapBuffers(mWindow);
  glfwPollEvents();
}

void App::processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  for (int i = GLFW_KEY_A; i <= GLFW_KEY_Z; i++) {
    if (glfwGetKey(window, i) == GLFW_PRESS) {
      mScene->onKey(i);
    }
  }
}

void App::basisInit() {
  basist::basisu_transcoder_init();
  mCodebook = std::make_unique<basist::etc1_global_selector_codebook>(
      basist::g_global_selector_cb_size, basist::g_global_selector_cb);
}
