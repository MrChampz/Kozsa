#include "SphericalQuadtreeTerrain.h"

// Projects a point on the unit sphere surface (the original point should be on the surface of the cube [-1,1])
Vector3<double> SphericalQuadtreeTerrain::CubeToSphere(const Vector3<double> &point)
{
  // Map from the cube [-1,1] (x, y, z) to the unit sphere
  // http://mathproofs.blogspot.com/2005/07/mapping-cube-to-sphere.html
  const double x2 = point.x * point.x;
  const double y2 = point.y * point.y;
  const double z2 = point.z * point.z;
  return Vector3<double>(point.x * sqrt(1.0 - (y2 + z2) * 0.5 + y2 * z2 * 0.33333333333333333333),
                         point.y * sqrt(1.0 - (z2 + x2) * 0.5 + z2 * x2 * 0.33333333333333333333),
                         point.z * sqrt(1.0 - (x2 + y2) * 0.5 + x2 * y2 * 0.33333333333333333333));
}

SphericalQuadtreeTerrain::SphericalQuadtreeTerrain(const double radius)
  : radius(radius)
{
  // Create the planet quadtree faces (use 1.0 as z position to form the cube [-1,1])
  for (unsigned int i = 0; i < 6; i++)
   faces.push_back(new QuadtreeTerrainFace(this, i, 1.0));
  ConnectFaces();
}

void SphericalQuadtreeTerrain::Rebuild()
{
  QuadtreeTerrain::Rebuild();
  ConnectFaces();
}

void SphericalQuadtreeTerrain::ConnectFaces()
{
  // Connect the faces to form a cube
  faces[FACE_NEAR ]->Connect(NORTH, faces[FACE_TOP]);
  faces[FACE_NEAR ]->Connect( WEST, faces[FACE_LEFT]);
  faces[FACE_NEAR ]->Connect(SOUTH, faces[FACE_BOTTOM]);
  faces[FACE_LEFT ]->Connect(NORTH, faces[FACE_TOP]);
  faces[FACE_LEFT ]->Connect( WEST, faces[FACE_FAR]);
  faces[FACE_LEFT ]->Connect(SOUTH, faces[FACE_BOTTOM]);
  faces[FACE_FAR  ]->Connect(NORTH, faces[FACE_TOP]);
  faces[FACE_FAR  ]->Connect( WEST, faces[FACE_RIGHT]);
  faces[FACE_FAR  ]->Connect(SOUTH, faces[FACE_BOTTOM]);
  faces[FACE_RIGHT]->Connect(NORTH, faces[FACE_TOP]);
  faces[FACE_RIGHT]->Connect( WEST, faces[FACE_NEAR]);
  faces[FACE_RIGHT]->Connect(SOUTH, faces[FACE_BOTTOM]);
}

// Calculates the terrain height in a given direction from the center
double SphericalQuadtreeTerrain::GetTerrainHeight(const Vector3<double> &point) const
{
  return radius;
}

// Projects a point on the surface (the original point should be on the surface of the cube [-1,1])
Vector3<double> SphericalQuadtreeTerrain::ProjectOnSurface(const Vector3<double> &point) const
{
  // Project the point from the cube [-1,1] on the unit sphere to get a direction vector
  Vector3<double> surfacePoint = CubeToSphere(point);

  // Scale the vector by the terrain height to get the surface point
  surfacePoint *= GetTerrainHeight(surfacePoint);

  return surfacePoint;
}

double SphericalQuadtreeTerrain::GetRadius() const
{
  return radius;
}
