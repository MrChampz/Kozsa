/*******************************************************************************
* File:           QuadtreeTerrainPatchTopology.h
* Author:         Christian Alfons
* Date created:   2010-10-04
* Date modified:  2011-01-18
* Description:    Quadtree chunked level-of-detail terrain patch topology
*                 class. There can be only a certain number of different node
*                 patch topologies (read: triangle structures), based on the
*                 maximum detail difference allowed between neighboring nodes.
*                 Hence, the topologies can be precalculated and shared among
*                 the patches (which in turn hold the geometry).
*******************************************************************************/

#ifndef QUADTREETERRAINPATCHTOPOLOGY_H_INCLUDED
#define QUADTREETERRAINPATCHTOPOLOGY_H_INCLUDED

#include <vector>
#include "../../GL/GLee.h"
#include <GL/gl.h>
#include "../../Math/VectorMath.h"

#define MAX_DETAIL_DIFFERENCE           4
#define PATCH_SIZE                      (1 << MAX_DETAIL_DIFFERENCE) // 2^MAX_DETAIL_DIFFERENCE
#define PATCH_SIZE_HALF                 (PATCH_SIZE / 2)
#define PATCH_VERTICES                  (PATCH_SIZE + 1)
#define PATCH_VERTICES_TOTAL            (PATCH_VERTICES * PATCH_VERTICES)
#define NORTH                           0
#define EAST                            1
#define SOUTH                           2
#define WEST                            3
#define NORTH_WEST                      0
#define NORTH_EAST                      1
#define SOUTH_EAST                      2
#define SOUTH_WEST                      3
#define CENTER                          4
#define MIRROR(s)                       (((s) + 2) % 4)
#define ADJACENT(s, q)                  ((4 + (q) - (s)) % 4 <= 1)
#define REFLECT(s, q)                   ((s) % 2 ? ((q) % 2 ? (q) - 1 : (q) + 1) : 3 - (q))
#define INDEX1D(x, y)                   ((x) + (y) * PATCH_VERTICES)
#define INDEX2D(i)                      ((i) % PATCH_VERTICES), ((i) / PATCH_VERTICES)

using std::vector;

typedef vector<GLushort>    TriangleFan;
typedef vector<TriangleFan> TriangleFanList;

class QuadtreeTerrainPatchTopology
{
  private:
    // Class methods
    static void RotateIndices(unsigned int &x, unsigned int &y, const unsigned int rotation);
    static void AddTriangleFan(TriangleFanList &triangleFans);
    static void AddTriangleFanVertex(TriangleFanList &triangleFans, unsigned int x, unsigned int y, const unsigned int rotation = 0);
    static void BuildEdge(TriangleFanList &triangleFans, const unsigned int side, const unsigned int detail = 0);

    // Instance variables
    GLsizei vertexCount;
    GLuint  ibo;

  public:
    // Constructor
    QuadtreeTerrainPatchTopology(const unsigned int detailNorth, const unsigned int detailEast, const unsigned int detailSouth, const unsigned int detailWest);

    // Destructor
    ~QuadtreeTerrainPatchTopology();

    // Instance methods
    GLsizei GetVertexCount() const;
    GLuint  GetIbo() const;
};

#endif
