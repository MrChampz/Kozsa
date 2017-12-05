/*******************************************************************************
* File:           RidgedMultifractalSphericalQuadtreeTerrain.h
* Author:         Christian Alfons
* Date created:   2011-02-10
* Date modified:  2011-02-10
* Description:    Ridged multifractal (RMF) spherical quadtree terrain class.
*                 Used to create spherical fractal landscapes, using the RMF
*                 algorithm presented in the book "Texturing & Modeling: A
*                 Procedural Approach" (third edition).
*******************************************************************************/

#ifndef RIDGEDMULTIFRACTALSPHERICALQUADTREETERRAIN_H_INCLUDED
#define RIDGEDMULTIFRACTALSPHERICALQUADTREETERRAIN_H_INCLUDED

#include "SphericalQuadtreeTerrain.h"

class RidgedMultifractalSphericalQuadtreeTerrain : public SphericalQuadtreeTerrain
{
  protected:
    // Instance variables
    double        heightScale;
    int           octaves;
    double        gain;
    double        lacunarity;
    double        offset;
    double        h;
    double        *exponentArray;
    double        oneDivExponentArraySum;

  public:
    // Constructor
    RidgedMultifractalSphericalQuadtreeTerrain(const double radius = 1.0);

    // Destructor
    ~RidgedMultifractalSphericalQuadtreeTerrain();

    // Instance methods
    void            GetRidgedMultifractalParameters(double &heightScale, int &octaves, double &gain, double &lacunarity, double &offset, double &h);
    void            SetRidgedMultifractalParameters(const double heightScale, const int octaves, const double gain, const double lacunarity, const double offset, const double h);
    virtual double  GetTerrainHeight(const Vector3<double> &point) const;
    virtual void    Update();
};

#endif
