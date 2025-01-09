#pragma once
#include <glm/glm.hpp>

struct SceneData {
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewProjectionMatrix;

    void UpdateViewProjection() {
        ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
    }
};
