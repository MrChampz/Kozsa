#include "QuadtreeTerrainNode.h"
#include "QuadtreeTerrainPatch.h"
#include "QuadtreeTerrain.h"
#include <GL/gl.h>

#define NODE_MAX_DEPTH 13

using std::max;
using std::min;

double QuadtreeTerrainNode::splitDistanceScale = 2.0;

double QuadtreeTerrainNode::GetSplitDistanceScale()
{
  return splitDistanceScale;
}

void QuadtreeTerrainNode::SetSplitDistanceScale(const double value)
{
  // Make sure that the new split distance scale is reasonable
  splitDistanceScale = min(4.0, max(1.0, value));
}

QuadtreeTerrainNode::QuadtreeTerrainNode(QuadtreeTerrainFace *face, const QuadtreeTerrainNode *parent, const unsigned int quadrant)
  : face(face), parent(parent), quadrant(quadrant), depth(parent ? parent->depth + 1 : 0), children(), neighbors(), patch(0), neighborDetailDifferences(), visible(true)
{
}

QuadtreeTerrainNode::~QuadtreeTerrainNode()
{
  // Disconnect neighbors
  for (unsigned int i = 0; i < 4; i++)
    if (neighbors[i])
      neighbors[i]->SetNeighbor(MirrorSide(i), 0);

  // Delete children
  for (unsigned int i = 0; i < 4; i++)
    delete children[i];

  // Delete the patch
  delete patch;
}

// Returns the local center position of the node in the quadtree [-1,1]
Vector2<double> QuadtreeTerrainNode::GetPosition() const
{
  // Translate in the quadrant's direction (from the parent's position)
  return parent ? parent->GetPosition() + Vector2<double>(ADJACENT(EAST, quadrant) ? 1.0 : -1.0, ADJACENT(NORTH, quadrant) ? 1.0 : -1.0) * GetScale() : Vector2<double>(0.0, 0.0);
}

double QuadtreeTerrainNode::GetScale() const
{
  // 1.0 / (2^depth)
  return 1.0 / (1 << depth);
}

unsigned int QuadtreeTerrainNode::GetQuadrant() const
{
  return quadrant;
}

unsigned int QuadtreeTerrainNode::MirrorSide(const unsigned int side) const
{
  // If no neighbor; use default mirroring
  if (!neighbors[side])
    return MIRROR(side);

  // Get source and destination faces
  const unsigned int f0 = face->GetFaceType();
  const unsigned int f1 = neighbors[side]->face->GetFaceType();

  // If within the same face or faces with equal properties
  if (f0 == f1 || (f0 < 4 && f1 < 4))
    return MIRROR(side);

  // Source face
  switch (f0)
  {
    // Top face; always end up north
    case FACE_TOP:
      return NORTH;
    // Source bottom; always end up south
    case FACE_BOTTOM:
      return SOUTH;
  }

  // Destination face
  switch (f1)
  {
    // Top face; rotate to the source face
    case FACE_TOP:
      return MIRROR(f0);
    // Bottom face; rotate to the source face
    case FACE_BOTTOM:
      return (4 - f0) % 4;
  }

  return MIRROR(side);
}

unsigned int QuadtreeTerrainNode::MirrorQuadrant(const unsigned int side, const unsigned int quadrant) const
{
  // If mirroring within the parent node
  if (!ADJACENT(side, quadrant))
    return REFLECT(side, quadrant);

  // If no parent or parent neighbor
  if (!parent || !parent->neighbors[side])
    return REFLECT(side, quadrant);

  // Get source and destination faces
  const unsigned int f0 = face->GetFaceType();
  const unsigned int f1 = parent->neighbors[side]->face->GetFaceType();

  // If within the same face or faces with equal properties
  if (f0 == f1 || (f0 < 4 && f1 < 4))
    return REFLECT(side, quadrant);

  // Source face
  switch (f0)
  {
    case FACE_NEAR:
      return REFLECT(side, quadrant);
    case FACE_LEFT:
      switch(quadrant)
      {
        case NORTH_EAST:
        case SOUTH_WEST:
          return SOUTH_WEST;
        case NORTH_WEST:
        case SOUTH_EAST:
          return NORTH_WEST;
      }
    case FACE_FAR:
      switch (quadrant)
      {
        case NORTH_EAST:
          return NORTH_WEST;
        case NORTH_WEST:
          return NORTH_EAST;
        case SOUTH_EAST:
          return SOUTH_WEST;
        case SOUTH_WEST:
          return SOUTH_EAST;
      }
    case FACE_RIGHT:
      switch (quadrant)
      {
        case NORTH_EAST:
        case SOUTH_WEST:
          return NORTH_EAST;
        case NORTH_WEST:
        case SOUTH_EAST:
          return SOUTH_EAST;
      }
    case FACE_TOP:
      switch (quadrant)
      {
        case NORTH_EAST:
        case SOUTH_WEST:
          return (side == NORTH || side == SOUTH) ? NORTH_WEST : NORTH_EAST;
        case NORTH_WEST:
        case SOUTH_EAST:
          return (side == NORTH || side == SOUTH) ? NORTH_EAST : NORTH_WEST;
      }
    case FACE_BOTTOM:
      switch (quadrant)
      {
        case NORTH_EAST:
        case SOUTH_WEST:
          return (side == NORTH || side == SOUTH) ? SOUTH_EAST : SOUTH_WEST;
        case NORTH_WEST:
        case SOUTH_EAST:
          return (side == NORTH || side == SOUTH) ? SOUTH_WEST : SOUTH_EAST;
      }
  }

  return REFLECT(side, quadrant);
}

bool QuadtreeTerrainNode::IsLeaf() const
{
  return !children[0];
}

const QuadtreeTerrainFace* QuadtreeTerrainNode::GetFace() const
{
  return face;
}

const QuadtreeTerrainNode* QuadtreeTerrainNode::GetParent() const
{
  return parent;
}

QuadtreeTerrainPatch* QuadtreeTerrainNode::GetPatch() const
{
  return patch;
}

void QuadtreeTerrainNode::SetNeighbor(const unsigned int side, QuadtreeTerrainNode *neighbor)
{
  // Connect the nodes and update neighbor detail differences
  neighbors[side] = neighbor;
  if (neighbor)
  {
    const unsigned int sideMirrored = MirrorSide(side);
    neighbor->neighbors[sideMirrored] = this;
    neighbor->UpdateNeighborDetailDifferences(sideMirrored);
  }
  UpdateNeighborDetailDifferences(side);
}

// Returns the neighbor of equal depth if it exists; otherwise its youngest ancestor
const QuadtreeTerrainNode* QuadtreeTerrainNode::GetEqualOrHigherNeighbor(const unsigned int side) const
{
  // Find the youngest ancestor with a neighbor in the given direction
  for (const QuadtreeTerrainNode *node = this; node != 0; node = node->parent)
    if (node->neighbors[side])
      return node->neighbors[side];
  return 0;
}

// Returns the neighbor detail (depth) difference [0,MAX_DETAIL_DIFFERENCE]
unsigned int QuadtreeTerrainNode::GetNeighborDetailDifference(const unsigned int side) const
{
  return neighborDetailDifferences[side];
}

void QuadtreeTerrainNode::FindNeighbor(const unsigned int side)
{
  // If the current node has no neighbor in the given direction, but its parent does
  if (!neighbors[side] && parent && parent->neighbors[side])
  {
    // If a valid neighbor is found (child of the parent's neighbor); use it
    if (QuadtreeTerrainNode *neighbor = parent->neighbors[side]->children[MirrorQuadrant(side, quadrant)])
      SetNeighbor(side, neighbor);
    else
      return;
  }

  // If no leaf node; find child node neighbors
  if (!IsLeaf())
    for (unsigned int i = 0; i < 4; i++)
      if (ADJACENT(side, i))
        children[i]->FindNeighbor(side);
}

bool QuadtreeTerrainNode::Split()
{
  // Make sure that the node is allowed to split:
  // 1. The node must be a leaf node (i.e. not have children already)
  // 2. The node must not be at max depth already
  // 3. The node's face must have splits left
  if (!IsLeaf() || depth >= NODE_MAX_DEPTH || !face->HasSplitsLeft())
    return false;

  // Create the four child nodes
  children[NORTH_WEST] = new QuadtreeTerrainNode(face, this, NORTH_WEST);
  children[NORTH_EAST] = new QuadtreeTerrainNode(face, this, NORTH_EAST);
  children[SOUTH_EAST] = new QuadtreeTerrainNode(face, this, SOUTH_EAST);
  children[SOUTH_WEST] = new QuadtreeTerrainNode(face, this, SOUTH_WEST);

  // Connect the children
  children[NORTH_WEST]->SetNeighbor(EAST,   children[NORTH_EAST]);
  children[NORTH_EAST]->SetNeighbor(SOUTH,  children[SOUTH_EAST]);
  children[SOUTH_EAST]->SetNeighbor(WEST,   children[SOUTH_WEST]);
  children[SOUTH_WEST]->SetNeighbor(NORTH,  children[NORTH_WEST]);

  // Connect neighbors
  for (int i = 0; i < 4; i++)
    if (neighbors[i] && !neighbors[i]->IsLeaf())
      neighbors[i]->FindNeighbor(MirrorSide(i));

  return true;
}

bool QuadtreeTerrainNode::Merge()
{
  // If leaf node; cannot merge
  if (IsLeaf())
    return false;

  // Delete all child nodes
  for (unsigned int i = 0; i < 4; i++)
  {
    delete children[i];
    children[i] = 0;
  }

  return true;
}

void QuadtreeTerrainNode::UpdateNeighborDetailDifferences(const unsigned int side)
{
  // Update neighbor detail differences
  for (unsigned int i = 0; i < 4; i++)
    if (const QuadtreeTerrainNode *neighbor = GetEqualOrHigherNeighbor(i))
      neighborDetailDifferences[i] = min(depth - neighbor->depth, (unsigned int)MAX_DETAIL_DIFFERENCE);

  // Force child nodes on the updated side to redraw
  if (!IsLeaf())
    for (unsigned int i = 0; i < 4; i++)
      if (ADJACENT(side, i))
        children[i]->UpdateNeighborDetailDifferences(side);
}

void QuadtreeTerrainNode::Render() const
{
  // If not visible; return
  if (!face->GetTerrain()->GetFrustum().ContainsSphere(boundingSphereCenter, boundingSphereRadius))
    return;

  // If not leaf node; render the child nodes
  if (!IsLeaf())
  {
    for (unsigned int i = 0; i < 4; i++)
      children[i]->Render();
    return;
  }

  if (patch)
    patch->Render();
}

void QuadtreeTerrainNode::Update()
{
  // If no patch
  if (!patch)
  {
    // Create a patch and get its bounding sphere
    patch = new QuadtreeTerrainPatch(this);
    patch->GetBoundingSphere(boundingSphereCenter, boundingSphereRadius);
  }

  // Use the distance from the camera to the bounding sphere to calculate the split distance
  const double distanceToCamera = (face->GetTerrain()->GetCameraPositionLocal() - boundingSphereCenter).GetLength();
  const bool split = distanceToCamera < boundingSphereRadius * splitDistanceScale;

  // If the node shouldn't be split
  if (!split)
  {
    // If not a leaf node; merge
    if (!IsLeaf())
      Merge();
    return;
  }

  // If the node has children, or if children are successfully created; update the children
  if (!IsLeaf() || Split())
    for (unsigned int i = 0; i < 4; i++)
      children[i]->Update();
}
