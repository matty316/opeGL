#include "level.hpp"

#include <tmxlite/Map.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/TileLayer.hpp>

#include "opegl.hpp"

OpeLevel::OpeLevel(std::string path, uint32_t wallTexture,
                   uint32_t floorTexture, uint32_t ceilingTexture,
                   size_t maxHeight)
    : wallTexture(wallTexture), floorTexture(floorTexture),
      ceilingTexture(ceilingTexture), maxHeight(maxHeight) {
  tmx::Map map;
  if (map.load(path)) {
    const auto &layers = map.getLayers();
    for (const auto &layer : layers) {
      if (layer->getName() == "walls") {
        const auto &tiles = layer->getLayerAs<tmx::TileLayer>().getTiles();
        depth = layer->getSize().y;
        width = layer->getSize().x;
        walls.resize(depth);
        for (size_t z = 0; z < depth; z++) {
          walls[z].resize(width);
          for (size_t x = 0; x < width; x++) {
            walls[z][x] = tiles[z * width + x].ID;
          }
        }
      } else if (layer->getName() == "player") {
        const auto &position = layer->getLayerAs<tmx::ObjectGroup>()
                                   .getObjects()
                                   .front()
                                   .getPosition();
        playerPos.x = position.x / map.getTileSize().x;
        playerPos.y = position.y / map.getTileSize().y;
      } else if (layer->getName() == "floors") {
        const auto &tiles = layer->getLayerAs<tmx::TileLayer>().getTiles();
        depth = layer->getSize().y;
        width = layer->getSize().x;
        floors.resize(depth);
        for (size_t z = 0; z < depth; z++) {
          floors[z].resize(width);
          for (size_t x = 0; x < width; x++) {
            floors[z][x] = tiles[z * width + x].ID;
          }
        }
      } else if (layer->getName() == "ceiling") {
        const auto &tiles = layer->getLayerAs<tmx::TileLayer>().getTiles();
        depth = layer->getSize().y;
        width = layer->getSize().x;
        ceilings.resize(depth);
        for (size_t z = 0; z < depth; z++) {
          ceilings[z].resize(width);
          for (size_t x = 0; x < width; x++) {
            ceilings[z][x] = tiles[z * width + x].ID;
          }
        }
      } else if (layer->getName() == "point lights") {
        for (const auto &object :
             layer->getLayerAs<tmx::ObjectGroup>().getObjects()) {
          const auto &position = object.getPosition();
          auto x = position.x / map.getTileSize().x;
          auto y = static_cast<float>(maxHeight) - 0.1f;
          auto z = position.y / map.getTileSize().y;
          PointLight pointLight(glm::vec3(x, y, z));
          pointLights.push_back(pointLight);
        }
      }
    }
  } else {
    std::runtime_error("failed to load level");
  }
}

void OpeLevel::loadLevel(OpeGL &app) {
  app.setPlayerPos(playerPos);
  for (auto &light : pointLights)
    app.addPointLight(light);
  for (size_t z = 0; z < depth; z++) {
    for (size_t x = 0; x < width; x++) {
      auto wall = walls[z][x];
      if (wall == 1) {
        if (z != depth - 1 && walls[z + 1][x] == 0) {
          for (size_t height = 0; height < maxHeight; height++) {
            app.addQuad(
                glm::vec3(0.0f + x, static_cast<float>(height), 0.0f + z), 0.0f,
                glm::vec3(1.0f), 1.0f, wallTexture);
          }
        }
        if (x != width - 1 && walls[z][x + 1] == 0) {
          for (size_t height = 0; height < maxHeight; height++) {
            app.addQuad(
                glm::vec3(0.5f + x, static_cast<float>(height), -0.5f + z),
                90.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, wallTexture);
          }
        }
        if (x != 0 && walls[z][x - 1] == 0) {
          for (size_t height = 0; height < maxHeight; height++) {
            app.addQuad(
                glm::vec3(-0.5f + x, static_cast<float>(height), -0.5f + z),
                270.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, wallTexture);
          }
        }
        if (z != 0 && walls[z - 1][x] == 0) {
          for (size_t height = 0; height < maxHeight; height++) {
            app.addQuad(
                glm::vec3(0.0f + x, static_cast<float>(height), -1.0f + z),
                180.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, wallTexture);
          }
        }
      }

      auto floor = floors[z][x];
      if (floor == 1 && wall != 1)
        app.addQuad(glm::vec3(x, 0.0f, z), 270.0f, glm::vec3(1.0f, 0.0f, 0.0f),
                    1.0f, floorTexture);

      auto ceiling = ceilings[z][x];
      if (ceiling == 1 && wall != 1)
        app.addQuad(glm::vec3(x, static_cast<float>(maxHeight), -1.0f + z),
                    90.0f, glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, ceilingTexture);
    }
  }
}
