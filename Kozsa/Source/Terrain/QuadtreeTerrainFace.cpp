#include "QuadtreeTerrainFace.h"
#include "QuadtreeTerrain.h"

// Maximum number of node splits per face and update (set to -1 for unlimited)
#define MAX_NODE_SPLITS_PER_UPDATE 2

QuadtreeTerrainFace::QuadtreeTerrainFace(const QuadtreeTerrain *terrain, const unsigned int faceType, const double positionZ)
  : terrain(terrain), faceType(faceType), positionZ(positionZ), topNode(new QuadtreeTerrainNode(this)), splitsRemaining(0)
{
  // Set the orientation to match the face direction
  if (faceType < 4)
    orientation = glm::rotate(orientation, ((double)faceType * -0.5 * M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
  else
    orientation = glm::rotate(orientation, ((faceType == FACE_BOTTOM ? 0.5 : -0.5) * M_PI), glm::vec3(1.0f, 0.0f, 0.0f));
}

QuadtreeTerrainFace::~QuadtreeTerrainFace()
{
  delete topNode;
}

void QuadtreeTerrainFace::Rebuild()
{
  delete topNode;
  topNode = new QuadtreeTerrainNode(this);
}

void QuadtreeTerrainFace::Connect(const unsigned int side, QuadtreeTerrainFace *face)
{
  // Connect the face top nodes
  if (topNode && face->topNode)
    topNode->SetNeighbor(side, face->topNode);
}

const QuadtreeTerrain* QuadtreeTerrainFace::GetTerrain() const
{
  return terrain;
}

unsigned int QuadtreeTerrainFace::GetFaceType() const
{
  return faceType;
}

void QuadtreeTerrainFace::Render() const
{
  // Render the quadtree, starting at the top node
  if (topNode)
    topNode->Render();
}

// Returns true if the face is allowed to split more nodes (during the current update)
// Nodes call this function when they want to split; each call consumes one split
// Once the remaining split count reaches 0, nodes can no longer split
bool QuadtreeTerrainFace::HasSplitsLeft()
{
  // If no splits remain; return false
  if (splitsRemaining == 0)
    return false;

  // If positive count; decrease (negative means unlimited)
  if (splitsRemaining > 0)
    splitsRemaining--;

  return true;
}

void QuadtreeTerrainFace::Update()
{
  // Reset the split count
  splitsRemaining = MAX_NODE_SPLITS_PER_UPDATE;

  // Update the quadtree, starting at the top node
  if (topNode)
    topNode->Update();
}

glm::vec3 QuadtreeTerrainFace::GetWorldPosition(const glm::vec2 &p) const
{
  return orientation * glm::vec3(p.x, p.y, positionZ);
}
