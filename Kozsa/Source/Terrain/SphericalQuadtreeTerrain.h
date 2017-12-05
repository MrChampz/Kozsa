/*******************************************************************************
* File:           SphericalQuadtreeTerrain.h
* Author:         Christian Alfons
* Date created:   2010-07-16
* Date modified:  2011-01-24
* Description:    Spherical quadtree terrain class. Uses six faces to form a
*                 cube, which is then transformed into a sphere. Points on the
*                 sphere can be translated along their normal vectors to shape
*                 the terrain.
*******************************************************************************/

#ifndef SPHERICALQUADTREETERRAIN_H_INCLUDED
#define SPHERICALQUADTREETERRAIN_H_INCLUDED

#include "../../Shaders/ShaderManager.h"
#include "QuadtreeTerrain.h"

class SphericalQuadtreeTerrain : public QuadtreeTerrain
{
  private:
    // Class methods
    static Vector3<double> CubeToSphere(const Vector3<double> &point);

  protected:
    // Instance variables
    const double radius;

  public:
    // Constructor
    SphericalQuadtreeTerrain(const double radius = 1.0);

    // Instance methods
    virtual void            Rebuild();
    void                    ConnectFaces();
    virtual double          GetTerrainHeight(const Vector3<double> &point) const;
    virtual Vector3<double> ProjectOnSurface(const Vector3<double> &point) const;
    double                  GetRadius() const;
};

#endif
