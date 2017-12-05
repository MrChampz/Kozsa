#include "QuadtreeTerrainPatchTopology.h"
#include <algorithm>

using std::swap;

void QuadtreeTerrainPatchTopology::RotateIndices(unsigned int &x, unsigned int &y, const unsigned int rotation)
{
  // Change the indices to match the given rotation
  switch (rotation)
  {
    case NORTH:
    default:
      return;
    case EAST:
      swap(x, y);
      y = PATCH_SIZE - y;
      return;
    case SOUTH:
      x = PATCH_SIZE - x;
      y = PATCH_SIZE - y;
      return;
    case WEST:
      swap(x, y);
      x = PATCH_SIZE - x;
      return;
  }
}

QuadtreeTerrainPatchTopology::QuadtreeTerrainPatchTopology(const unsigned int detailNorth, const unsigned int detailEast, const unsigned int detailSouth, const unsigned int detailWest)
{
  TriangleFanList triangleFans;

  // Build the patch body (without edges)
  for (unsigned int x = 2; x < PATCH_VERTICES - 2; x += 2)
  {
    for (unsigned int y = 2; y < PATCH_VERTICES - 2; y += 2)
    {
      AddTriangleFan(triangleFans);
      AddTriangleFanVertex(triangleFans, x,     y);
      AddTriangleFanVertex(triangleFans, x + 1, y + 1);
      AddTriangleFanVertex(triangleFans, x,     y + 1);
      AddTriangleFanVertex(triangleFans, x - 1, y + 1);
      AddTriangleFanVertex(triangleFans, x - 1, y);
      AddTriangleFanVertex(triangleFans, x - 1, y - 1);
      AddTriangleFanVertex(triangleFans, x,     y - 1);
      AddTriangleFanVertex(triangleFans, x + 1, y - 1);
      AddTriangleFanVertex(triangleFans, x + 1, y);
      AddTriangleFanVertex(triangleFans, x + 1, y + 1);
    }
  }

  // Build all four edges
  BuildEdge(triangleFans, NORTH,  detailNorth);
  BuildEdge(triangleFans, EAST,   detailEast);
  BuildEdge(triangleFans, SOUTH,  detailSouth);
  BuildEdge(triangleFans, WEST,   detailWest);

  // Count the number of triangles and reserve three vertices per triangle
  int triangleCount = 0;
  for (unsigned int i = 0; i < triangleFans.size(); i++)
    if (triangleFans[i].size() > 2)
      triangleCount += triangleFans[i].size() - 2;
  vertexCount = 3 * triangleCount;

  // Create an index array
  GLushort *indexData = new GLushort[vertexCount];
  int n = 0;
  for (unsigned int i = 0; i < triangleFans.size(); i++)
  {
    for (unsigned int j = 2; j < triangleFans[i].size(); j++)
    {
      indexData[n++] = triangleFans[i][0];
      indexData[n++] = triangleFans[i][j-1];
      indexData[n++] = triangleFans[i][j];
    }
  }

  // Generate the index buffer object (IBO)
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * vertexCount, indexData, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Delete unnecessary data
  triangleFans.clear();
  delete[] indexData;
}

QuadtreeTerrainPatchTopology::~QuadtreeTerrainPatchTopology()
{
  glDeleteBuffers(1, &ibo);
}

// Creates and adds a triangle fan
void QuadtreeTerrainPatchTopology::AddTriangleFan(TriangleFanList &triangleFans)
{
  triangleFans.push_back(TriangleFan());
}

// Adds a vertex index to the most recently added triangle fan
void QuadtreeTerrainPatchTopology::AddTriangleFanVertex(TriangleFanList &triangleFans, unsigned int x, unsigned int y, const unsigned int rotation)
{
  // Rotate and use the x and y indices
  QuadtreeTerrainPatchTopology::RotateIndices(x, y, rotation);
  triangleFans.back().push_back(INDEX1D(x,y));
}

void QuadtreeTerrainPatchTopology::BuildEdge(TriangleFanList &triangleFans, const unsigned int side, const unsigned int detail)
{
  // If detail difference is 0 (default case) or too high
  if (detail == 0 || detail > MAX_DETAIL_DIFFERENCE)
  {
    for (unsigned int x = 0; x < PATCH_SIZE - 1; x += 2)
    {
      AddTriangleFan(triangleFans);
      AddTriangleFanVertex(triangleFans, x + 1, PATCH_SIZE - 1, side);
      for (int d = -1; d <= 1; d++)
        AddTriangleFanVertex(triangleFans, x + 1 - d, PATCH_SIZE, side);

      if (x > 0)
      {
        AddTriangleFan(triangleFans);
        AddTriangleFanVertex(triangleFans, x, PATCH_SIZE, side);
        for (int d = -1; d <= 1; d++)
          AddTriangleFanVertex(triangleFans, x + d, PATCH_SIZE - 1, side);
      }
    }
    return;
  }

  // Calculate step sizes; step = 2^detail
  const int step = 1 << detail;
  const int halfStep = step / 2;

  unsigned int x = 0;
  for (x = 0; x < PATCH_SIZE; x += step)
  {
    AddTriangleFan(triangleFans);
    AddTriangleFanVertex(triangleFans, x, PATCH_SIZE, side);
    AddTriangleFanVertex(triangleFans, x + halfStep, PATCH_SIZE - 1, side);
    AddTriangleFanVertex(triangleFans, x + step, PATCH_SIZE, side);

    if (x > 0)
    {
      AddTriangleFan(triangleFans);
      AddTriangleFanVertex(triangleFans, x, PATCH_SIZE, side);
      for (int d = -halfStep; d <= halfStep; d++)
        AddTriangleFanVertex(triangleFans, x + d, PATCH_SIZE - 1, side);
    }
  }

  if (step > 2)
  {
    AddTriangleFan(triangleFans);
    AddTriangleFanVertex(triangleFans, 0, PATCH_SIZE, side);
    for (int d = 1; d <= halfStep; d++)
      AddTriangleFanVertex(triangleFans, d, PATCH_SIZE - 1, side);

    AddTriangleFan(triangleFans);
    AddTriangleFanVertex(triangleFans, PATCH_SIZE, PATCH_SIZE, side);
    for (int d = -halfStep; d <= -1; d++)
      AddTriangleFanVertex(triangleFans, PATCH_SIZE + d, PATCH_SIZE - 1, side);
  }
}

GLsizei QuadtreeTerrainPatchTopology::GetVertexCount() const
{
  return vertexCount;
}

GLuint QuadtreeTerrainPatchTopology::GetIbo() const
{
  return ibo;
}
