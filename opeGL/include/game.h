#pragma once

#include "camera.h"

void start(bool debug = false, bool vSync = false, bool fullscreen = false);
void run(CameraType cameraType, glm::vec3 cameraPosition, bool wireframe = false);
