/*******************************************************************************
* File:           QuadtreeTerrainNode.h
* Author:         Christian Alfons
* Date created:   2010-06-31
* Date modified:  2010-10-24
* Description:    Quadtree chunked level-of-detail terrain node class.
*                 Represents a node in the quadtree structure. Each node has
*                 0 or 4 children, up to 4 neighbors and possibly 1 parent.
*                 The quadrant tells us what kind of child this node is
*                 (as seen from its parent).
*                 A node is only a sort of abstract organizational item, and
*                 the actual geometry is handled by its patch.
*                 Performs view frustum culling using patch bounding volumes.
*******************************************************************************/

#ifndef QUADTREETERRAINNODE_H_INCLUDED
#define QUADTREETERRAINNODE_H_INCLUDED

#include "QuadtreeTerrainPatch.h"
#include "../Frustum.h"

class QuadtreeTerrainFace;

class QuadtreeTerrainNode
{
  private:
    // Class variables
    static double splitDistanceScale;

    // Instance variables
    QuadtreeTerrainFace         *face;
    const QuadtreeTerrainNode   *parent;
    const unsigned int          quadrant;
    const unsigned int          depth;
    QuadtreeTerrainNode         *children[4];
    QuadtreeTerrainNode         *neighbors[4];
    QuadtreeTerrainPatch        *patch;
    unsigned int                neighborDetailDifferences[4];
    Vector3<double>             boundingSphereCenter;
    double                      boundingSphereRadius;
    bool                        visible;

    // Instance methods
    unsigned int                MirrorQuadrant(const unsigned int side, const unsigned int quadrant) const;
    bool                        IsLeaf() const;
    bool                        Split();
    bool                        Merge();
    void                        FindNeighbor(const unsigned int side);
    void                        UpdateNeighborDetailDifferences(const unsigned int side);

  public:
    // Class methods
    static double GetSplitDistanceScale();
    static void   SetSplitDistanceScale(const double value);

    // Constructor
    QuadtreeTerrainNode(QuadtreeTerrainFace *face, const QuadtreeTerrainNode *parent = 0, const unsigned int quadrant = 0);

    // Destructor
    ~QuadtreeTerrainNode();

    // Instance methods
    unsigned int                MirrorSide(const unsigned int side) const;
    const QuadtreeTerrainNode*  GetEqualOrHigherNeighbor(const unsigned int side) const;
    unsigned int                GetNeighborDetailDifference(const unsigned int side) const;
    Vector2<double>             GetPosition() const;
    double                      GetScale() const;
    unsigned int                GetQuadrant() const;
    const QuadtreeTerrainFace*  GetFace() const;
    const QuadtreeTerrainNode*  GetParent() const;
    QuadtreeTerrainPatch*       GetPatch() const;
    void                        SetNeighbor(const unsigned int side, QuadtreeTerrainNode *neighbor);
    void                        Render() const;
    void                        Update();
};

#endif
