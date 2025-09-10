#pragma once

#include "camera.h"

void start(bool debug = false, bool fullscreen = false, bool vSync = false);
void run(CameraType cameraType, glm::vec3 cameraPosition,
         bool wireframe = false);
