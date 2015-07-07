
#ifndef VIRTUALVISTA_CAMERA_H
#define VIRTUALVISTA_CAMERA_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

namespace vv
{
  class Camera
  {
  public:
    Camera(Shader* shader);

    void update();
    void move(GLint key, double movement_speed);
    void rotate(double x, double y, double rotation_speed);

    void bindMatrices();

  private:
    Shader shader_;
    float pitch_angle_;
    float yaw_angle_;
    float max_pitch_angle_;
    float max_yaw_angle_;

    glm::vec3 position_delta_; // change in position from input
    glm::vec3 position_vec_;   // global coordinate point
    glm::vec3 direction_vec_;  // look at - position
    glm::vec3 look_at_vec_;    // position of observed point
    glm::vec3 up_vec_;         // global up direction

    glm::mat4 projection_mat_;
    glm::mat4 view_mat_;
  };
}

#endif // VIRTUALVISTA_CAMERA_H