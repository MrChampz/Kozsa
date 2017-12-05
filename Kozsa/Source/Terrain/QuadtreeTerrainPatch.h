/*******************************************************************************
* File:           QuadtreeTerrainPatch.h
* Author:         Christian Alfons
* Date created:   2010-06-31
* Date modified:  2010-10-24
* Description:    Quadtree chunked level-of-detail terrain patch class. This
*                 class is used to give a quadtree node an actual appearance.
*                 Primarily, this class creates the heightmap (and normalmap)
*                 using FBOs, and also renders the node geometry.
*******************************************************************************/

#ifndef QUADTREETERRAINPATCH_H_INCLUDED
#define QUADTREETERRAINPATCH_H_INCLUDED

#include "QuadtreeTerrainPatchTopology.h"

#define NUM_FBOS 2

class QuadtreeTerrainNode;

template<typename Real>
struct Vertex
{
  Vector3<Real> position;
  Vector3<Real> normal;
  Vector2<Real> texCoord;
  Vertex() {}
  Vertex(const Vector3<Real> &position, const Vector3<Real> &normal, const Vector2<Real> &texCoord) : position(position), normal(normal), texCoord(texCoord) {}
};

class QuadtreeTerrainPatch
{
  private:
    // Class variables
    static unsigned int instanceCount;
    static QuadtreeTerrainPatchTopology* topologies[MAX_DETAIL_DIFFERENCE + 1][MAX_DETAIL_DIFFERENCE + 1][MAX_DETAIL_DIFFERENCE + 1][MAX_DETAIL_DIFFERENCE + 1];
    static GLuint fbos[NUM_FBOS];
    static GLuint texPositionmap;

    // Class methods
    static void GenerateTopologies();
    static void DeleteTopologies();
    static void RenderFullscreenSquare(const Vector3<double> patchCorners[4]);

    // Instance variables
    const QuadtreeTerrainNode *node;
    GLuint                    vbo;
    Vertex<float>             verts[PATCH_VERTICES_TOTAL];
    GLuint                    texHeightmap;
    GLuint                    texNormalmap;

    // Instance methods
    const QuadtreeTerrainPatchTopology* GetTopology() const;
    void RenderTextures(const QuadtreeTerrainNode *node);

  public:
    // Constructor
    QuadtreeTerrainPatch(const QuadtreeTerrainNode *node);

    // Destructor
    ~QuadtreeTerrainPatch();

    // Instance methods
    void Render() const;
    void GetBoundingSphere(Vector3<double> &boundingSphereCenter, double &boundingSphereRadius) const;
};

#endif
