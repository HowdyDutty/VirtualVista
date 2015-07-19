
#include <iostream>

#include <glm/glm.hpp>

#include "Application.h"
#include "Input.h"
#include "Mesh.h"
#include "Settings.h"

namespace vv
{
  App::App() :
    first_run_(true),
    first_input_(true),
    app_init_(false),
    last_x_(0),
    last_y_(0),
    global_time_(0)
  {
  }


  bool App::init()
  {
    if (!app_init_) // prevent multiple calls
    {
      if (!glfwInit())
      {
        std::cerr << "ERROR: GLFW failed to initialize.\n";
        return EXIT_FAILURE;
      }

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
      glfwWindowHint(GLFW_SAMPLES, 4);

      int x, y, width, height;
      Settings::instance()->getViewport(x, y, width, height);
      window_ = glfwCreateWindow(width, height, "Virtual Vista", nullptr, nullptr);

      if (!window_)
      {
        std::cerr << "ERROR: Window context failed to initialize.\n";
        return false; // Maybe have a try catch here
      }

      glfwMakeContextCurrent(window_);
      glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      // Needs to be called after window_ creation else seg fault.
      glewExperimental = GL_TRUE;
      glewInit();

      app_init_ = true;
    }

    return true;
  }


  void App::handleInput(Camera* cam, double delta_time)
  {
    double movement_speed = Settings::instance()->getMovementSpeed() * delta_time;
    double rotation_speed = Settings::instance()->getRotationSpeed() * delta_time;

    if (Input::instance()->keyIsPressed(GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(window_, GL_TRUE);

    // Camera movements
    if (Input::instance()->keyIsPressed(GLFW_KEY_W))
      cam->move(GLFW_KEY_W, movement_speed);

    if (Input::instance()->keyIsPressed(GLFW_KEY_A))
      cam->move(GLFW_KEY_A, movement_speed);

    if (Input::instance()->keyIsPressed(GLFW_KEY_S))
      cam->move(GLFW_KEY_S, movement_speed);

    if (Input::instance()->keyIsPressed(GLFW_KEY_D))
      cam->move(GLFW_KEY_D, movement_speed);

    // Camera rotations
    double curr_x, curr_y;
    Input::instance()->getMouseValues(curr_x, curr_y);
    if (first_input_ && (curr_x != 0 && curr_y != 0))
    {
      last_x_ = curr_x;
      last_y_ = curr_y;
      first_input_ = false;
    }

    double delta_x = (curr_x - last_x_);
    double delta_y = (last_y_ - curr_y);
    last_x_ = curr_x;
    last_y_ = curr_y;

    cam->rotate(delta_x, delta_y, rotation_speed);
  }


  void App::run()
  {
    // create program
    std::string path = Settings::instance()->getShaderLocation();
    Shader shader(path + "vertex.glsl", path + "fragment.glsl");

    Camera camera(&shader);

    GLfloat vertices[] = {
      -0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f,  0.5f, -0.5f,
      0.5f,  0.5f, -0.5f,
      -0.5f,  0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,

      -0.5f, -0.5f,  0.5f,
      0.5f, -0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,
      -0.5f,  0.5f,  0.5f,
      -0.5f, -0.5f,  0.5f,

      -0.5f,  0.5f,  0.5f,
      -0.5f,  0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,
      -0.5f, -0.5f,  0.5f,
      -0.5f,  0.5f,  0.5f,

      0.5f,  0.5f,  0.5f,
      0.5f,  0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,

      -0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f,  0.5f,
      0.5f, -0.5f,  0.5f,
      -0.5f, -0.5f,  0.5f,
      -0.5f, -0.5f, -0.5f,

      -0.5f,  0.5f, -0.5f,
      0.5f,  0.5f, -0.5f,
      0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,
      -0.5f,  0.5f,  0.5f,
      -0.5f,  0.5f, -0.5f
    };

    // convert to array of structures
    std::vector<Vertex> vertices_vec;
    for (int i = 0; i < 36; ++i)
    {
      Vertex v;
      v.position = glm::vec3(vertices[3*i], vertices[3*i+1], vertices[3*i+2]);
      vertices_vec.push_back(v);
    }

    Mesh mesh(vertices_vec);

    glm::vec3 cube_positions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),
      glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),
      glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),
      glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    glEnable(GL_DEPTH_TEST);

    // main loop
    while (!glfwWindowShouldClose(window_))
    {
      // reset framebuffer
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // manage time
      double curr_time = glfwGetTime();
      if (first_run_)
      {
        global_time_ = curr_time;
        first_run_ = false;
      }
      double delta_time = curr_time - global_time_;
      global_time_ = curr_time;

      // handle input
      glfwPollEvents();
      handleInput(&camera, delta_time);
      camera.update();

      shader.useProgram();

      // update uniforms
      camera.bindMatrices();

      GLfloat a = 1.0f * float(delta_time);
      mesh.rotate(a, glm::vec3(1.0f, 0.3f, 0.5f));
      mesh.rotate(a, glm::vec3(0.0f, 1.0f, 0.0f));
      mesh.bindUniforms(&shader);

      // render
      mesh.render();

      glfwSwapBuffers(window_);
    }
    glDisable(GL_DEPTH_TEST);

    // clean up
    glfwDestroyWindow(window_);
    glfwTerminate();
  }


  GLFWwindow* App::getWindow()
  {
    return window_;
  }
} // namespace vv