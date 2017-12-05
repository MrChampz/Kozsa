#include "QuadtreeTerrain.h"

QuadtreeTerrain::QuadtreeTerrain()
  : cameraPosition(0.0, 0.0, 0.0)
{
}

QuadtreeTerrain::~QuadtreeTerrain()
{
  for (unsigned int i = 0; i < faces.size(); i++)
    delete faces[i];
}

void QuadtreeTerrain::Rebuild()
{
  for (unsigned int i = 0; i < faces.size(); i++)
    faces[i]->Rebuild();
}

glm::vec3 QuadtreeTerrain::GetCameraPositionLocal() const
{
  return cameraPosition;
}

void QuadtreeTerrain::SetCameraPosition(const glm::vec3 &cameraPosition)
{
  this->cameraPosition = cameraPosition;
}

void QuadtreeTerrain::Render() const
{
  for (unsigned int i = 0; i < faces.size(); i++)
    faces[i]->Render();
}

void QuadtreeTerrain::Update()
{
  for (unsigned int i = 0; i < faces.size(); i++)
    faces[i]->Update();
}
