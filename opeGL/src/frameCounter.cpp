#include "frameCounter.h"
#include <print>

const float avgIntervalSec = 0.5f;
unsigned int numFrames = 0;
float accumulatedTime = 0.0f;
float currentFPS = 0.0f;

bool tick(float deltaTime, bool frameRendered) {
  if (frameRendered)
    numFrames++;
  accumulatedTime += deltaTime;

  if (accumulatedTime < avgIntervalSec)
    return false;
  
  currentFPS = static_cast<float>(numFrames / accumulatedTime);

  std::println("FPS {}", currentFPS);
  numFrames = 0;
  accumulatedTime = 0.0f;
  return true;
}

inline float getFPS() { return currentFPS; }
