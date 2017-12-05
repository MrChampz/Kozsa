#include "RidgedMultifractalSphericalQuadtreeTerrain.h"
#include "../../Math/PerlinNoise.h"

#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

RidgedMultifractalSphericalQuadtreeTerrain::RidgedMultifractalSphericalQuadtreeTerrain(const double radius)
  : SphericalQuadtreeTerrain(radius), exponentArray(0), oneDivExponentArraySum(0.0)
{
  SetRidgedMultifractalParameters(0.05, 20, 2.0, 2.01, 1.0, 1.0);
}

RidgedMultifractalSphericalQuadtreeTerrain::~RidgedMultifractalSphericalQuadtreeTerrain()
{
  delete[] exponentArray;
}

void RidgedMultifractalSphericalQuadtreeTerrain::SetRidgedMultifractalParameters(const double heightScale, const int octaves, const double gain, const double lacunarity, const double offset, const double h)
{
  // Set multifractal parameters
  this->heightScale = CLAMP(heightScale,   0.0,   0.2);
  this->octaves     = CLAMP(octaves,         0,    20);
  this->gain        = CLAMP(gain,          0.0,   5.0);
  this->lacunarity  = CLAMP(lacunarity,    1.5,   3.0);
  this->offset      = CLAMP(offset,        0.7,   1.3);
  this->h           = CLAMP(h,             0.7,   1.3);

  // Precalculate spectral weights
  if (exponentArray)
    delete[] exponentArray;
  double frequency = 1.0;
  exponentArray = new double[this->octaves + 1];
  double exponentArraySum = 0.0;
  for (int i = 0; i <= this->octaves; i++)
  {
    exponentArray[i] = pow(frequency, -this->h);
    frequency *= this->lacunarity;
    exponentArraySum += exponentArray[i];
  }

  // Calculate scale for algorithm output
  oneDivExponentArraySum = exponentArraySum > 0.0 ? (1.0 / exponentArraySum) : 1.0;
}

void RidgedMultifractalSphericalQuadtreeTerrain::GetRidgedMultifractalParameters(double &heightScale, int &octaves, double &gain, double &lacunarity, double &offset, double &h)
{
  heightScale = this->heightScale;
  octaves     = this->octaves;
  gain        = this->gain;
  lacunarity  = this->lacunarity;
  offset      = this->offset;
  h           = this->h;
}

// Calculates the terrain height in a given direction from the planet's center (ridged multifractal)
double RidgedMultifractalSphericalQuadtreeTerrain::GetTerrainHeight(const Vector3<double> &point) const
{
  Vector3<double> dir = point;
  double signal, weight;

  // Get the base signal (absolute value to create the ridges; square for sharper ridges)
  signal = offset - fabs(PerlinNoise<double>::GetValue(dir.x, dir.y, dir.z));
  signal *= signal;
  double result = signal;

  for (int i = 1; i < octaves; i++)
  {
    // Increase the frequency
    dir *= lacunarity;

    // Calculate weight
    weight = CLAMP(signal * gain, 0.0, 1.0);

    // Get the next "octave" (only true octave if lacunarity = 2.0, right?)
    signal = offset - fabs(PerlinNoise<double>::GetValue(dir.x, dir.y, dir.z));
    signal *= signal;
    signal *= weight;

    result += signal * exponentArray[i];
  }

  // Scale the result to [-1,1], which allows the terrain to go "under sea level"
  // This scaling isn't 100 % accurate when offset != 1.0
  result *= oneDivExponentArraySum;
  result = 2.0 * result - 1.0;

  return radius + result * heightScale * radius;
}

void RidgedMultifractalSphericalQuadtreeTerrain::Update()
{
  ShaderManager::UseShaderProgram(PATCH_POSITIONMAP);
  static const GLint uniformHeightScale = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PATCH_POSITIONMAP), "heightScale");
  glUniform1f(uniformHeightScale, heightScale);
  static const GLint uniformOctaves = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PATCH_POSITIONMAP), "octaves");
  glUniform1i(uniformOctaves, octaves);
  static const GLint uniformGain = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PATCH_POSITIONMAP), "gain");
  glUniform1f(uniformGain, gain);
  static const GLint uniformLacunarity = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PATCH_POSITIONMAP), "lacunarity");
  glUniform1f(uniformLacunarity, lacunarity);
  static const GLint uniformOffset = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PATCH_POSITIONMAP), "offset");
  glUniform1f(uniformOffset, offset);
  static const GLint uniformH = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PATCH_POSITIONMAP), "h");
  glUniform1f(uniformH, h);
  static const GLint uniformRadius = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PATCH_POSITIONMAP), "radius");
  glUniform1f(uniformRadius, radius);
  glUseProgram(0);

  SphericalQuadtreeTerrain::Update();
}
