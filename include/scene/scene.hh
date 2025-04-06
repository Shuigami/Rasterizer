#pragma once

#include "object/object.hh"
#include "light/light.hh"
#include "scene/camera.hh"

#include <list>

class Scene {
public:
    Scene(std::list<Object*> objects, std::list<Light*> lights, Camera camera);

    std::list<Object*> getObjects();
    std::list<Light*> getLights();
    const Camera& getCamera();

private:
    std::list<Object*> objects_;
    std::list<Light*> lights_;
    Camera camera_;
};
