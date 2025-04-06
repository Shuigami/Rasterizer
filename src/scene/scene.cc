#include "scene/scene.hh"
#include "scene/camera.hh"

Scene::Scene(std::list<Object*> objects, std::list<Light*> lights, Camera camera)
: objects_(objects), lights_(lights), camera_(camera)
{}

std::list<Object*> Scene::getObjects()
{
    return objects_;
}

std::list<Light*> Scene::getLights()
{
    return lights_;
}

const Camera& Scene::getCamera()
{
    return camera_;
}
