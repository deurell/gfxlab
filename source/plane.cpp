//
// Created by Mikael Deurell on 2021-05-05.
//

#include "plane.h"
#include <GLFW/glfw3.h>
#include <memory>

DL::Plane::Plane(std::string_view vertexShader, std::string_view fragShader, std::string_view glslVersion, DL::Camera& camera) :
  mCamera(camera)
{
  mShader = std::make_unique<DL::Shader>(vertexShader, fragShader, glslVersion);
  float vertices[] = {
      // positions        // colors         // texture coords
      1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };

  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  glGenVertexArrays(1, &mVAO);
  glGenBuffers(1, &mVBO);
  glGenBuffers(1, &mEBO);

  glBindVertexArray(mVAO);

  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));


  glEnableVertexAttribArray(2);
  glBindVertexArray(0);
}
void DL::Plane::render(float /* delta */) const{
  mShader->use();
  mShader->setFloat("iTime", (float)glfwGetTime());
  glm::mat4 transform = glm::mat4(1.0f);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::scale(model, mScale);
  model = glm::translate(model, mPosition);
  mShader->setMat4f("model", model);
  glm::mat4 view = mCamera.getViewMatrix();
  mShader->setMat4f("view", view);
  glm::mat4 projectionMatrix = mCamera.getPerspectiveTransform(
      45.0, mCamera.mScreenSize.x / mCamera.mScreenSize.y);
  mShader->setMat4f("projection", projectionMatrix);

  glBindVertexArray(mVAO);
  glDrawElements(GL_TRIANGLES,6, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
