#include "TerrainObject.h"

#include <iostream>

#include <STB/stb_image.h>

TerrainObject::TerrainObject(const float width, const float height, const float heightScale, const glm::vec3 position, const glm::vec3 size)
	: GameObject(position, size), mWidth(width), mHeight(height), mHeightScale(heightScale), mHeightmap(0), mModel(0), mCells(0)
{
	this->loadPlaneHeightmap();
	this->setTerrainCoordinates();
	this->calculateNormals();
	this->buildTerrainModel();
	this->setupRigidBody();
	this->shutdownHeightMap();
	this->loadTerrainCells();
	this->shutdownTerrainModel();
}

TerrainObject::TerrainObject(std::string file, std::string blendMap, const float width, const float height, const float heightScale,
	const glm::vec3 position, const glm::vec3 size)
	: GameObject(position, size), mWidth(width), mHeight(height), mHeightScale(heightScale), mHeightmap(0), mModel(0), mCells(0)
{
	this->loadHeightmap(file);
	this->setTerrainCoordinates();
	this->calculateNormals();
	this->loadBlendMap(blendMap.c_str());
	this->buildTerrainModel();
	this->setupRigidBody();
	this->shutdownHeightMap();
	this->loadTerrainCells();
	this->shutdownTerrainModel();
}

TerrainObject::~TerrainObject()
{
}

void TerrainObject::Update(GLfloat dt)
{
}

void TerrainObject::Draw(Renderer& renderer, Frustum* frustum)
{
	// Prepare transformations
	renderer.mShader.Use();
	glm::mat4 model;

	// Translate, rotate and scale
	model = glm::translate(model, this->mPosition);
	model *= glm::toMat4(this->mRotation);
	model = glm::scale(model, this->mSize);

	// Input modelMatrix into shader
	renderer.mShader.SetMatrix4("model", model);

	// Bind appropriate textures
	int albedoNr = 1;
	int normalNr = 1;
	int roughnessNr = 1;
	int metallicNr = 1;
	int aoNr = 1;
	for (int i = 0; i < this->mTextures.size(); i++)
	{
		// Active proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + i);

		// Retrieve texture number (the N in diffuse_textureN)
		TextureType type = mTextures[i].mType;
		std::string number;
		std::string name;
		if (type == TEXTURE_TYPE_ALBEDO)
		{
			number = std::to_string(albedoNr++);
			name = "texture_albedo";
		}
		else if (type == TEXTURE_TYPE_NORMAL)
		{
			number = std::to_string(normalNr++);
			name = "texture_normal";
		}
		else if (type == TEXTURE_TYPE_ROUGHNESS)
		{
			number = std::to_string(roughnessNr++);
			name = "texture_roughness";
		}
		else if (type == TEXTURE_TYPE_METALLIC)
		{
			number = std::to_string(metallicNr++);
			name = "texture_metallic";
		}
		else if (type == TEXTURE_TYPE_AO)
		{
			number = std::to_string(aoNr++);
			name = "texture_ao";
		}

		// Now set the sampler to the correct texture unit
		renderer.mShader.SetInteger((name + number).c_str(), i);

		// And finally bind the texture
		this->mTextures[i].Bind();
	}

	// Draw terrain cells
	this->mCellsDrawnCount = 0;
	this->mCellsCulledCount = 0;
	for (int i = 0; i < this->mCellsCount; i++)
	{
		float maxWidth	= this->mCells[i].mMaxWidth;
		float maxHeight = this->mCells[i].mMaxHeight;
		float maxDepth	= this->mCells[i].mMaxDepth;
		float minWidth	= this->mCells[i].mMinWidth;
		float minHeight = this->mCells[i].mMinHeight;
		float minDepth	= this->mCells[i].mMinDepth;

		if (frustum->CheckRectangle2(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth))
		{
			// If it is visible then draw it
			this->mCells[i].Draw(renderer);

			// Increment the number of cells that were actually drawn
			this->mCellsDrawnCount++;
		}
		else
		{
			// Increment the number of cells that were culled
			this->mCellsCulledCount++;
		}
	}

	// Set everything back to defaults once configured
	for (int i = 0; i < this->mTextures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		this->mTextures[i].Unbind();
	}
}

void TerrainObject::DrawDebugLines(Renderer & renderer)
{
	// Prepare transformations
	renderer.mShader.Use();
	glm::mat4 model;

	// Translate, rotate and scale
	model = glm::translate(model, this->mPosition);
	model *= glm::toMat4(this->mRotation);
	model = glm::scale(model, this->mSize);

	// Input modelMatrix into shader
	renderer.mShader.SetMatrix4("model", model);

	// Draw terrain cells lines
	for (int i = 0; i < this->mCellsCount; i++)
		this->mCells[i].DrawDebugLines(renderer);
}

void TerrainObject::loadPlaneHeightmap()
{
	int index;

	this->mHeightmap = new Heightmap[this->mWidth * this->mHeight];

	// Insert the image data into the height map array
	for (int j = 0; j < this->mHeight; j++)
	{
		for (int i = 0; i < this->mWidth; i++)
		{
			index = (this->mWidth * j) + i;

			// Store the height at this point in the height map array
			this->mHeightmap[index].y = (float)0.0f;
		}
	}
}

void TerrainObject::loadHeightmap(std::string file)
{
	FILE* filePtr;
	int error, index;
	unsigned long long imageSize, count;
	//unsigned short* rawImage;

	this->mHeightmap = new Heightmap[this->mWidth * this->mHeight];

	// Open the 16 bit raw height map file for reading in binary
	error = fopen_s(&filePtr, file.c_str(), "rb");
	if (error != 0)
		std::cout << "ERROR::TERRAIN: Error opening the heightmap file!" << std::endl;

	// Calculate the size of the raw image data
	imageSize = this->mWidth * this->mHeight;

	// Allocate memory for the raw image data
	rawImage = new unsigned short[imageSize];

	// Read in the raw image data
	count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	if (count != imageSize)
		std::cout << "ERROR::TERRAIN: Error reading the heightmap file!" << std::endl;

	// Close the file
	error = fclose(filePtr);
	if (error != 0)
		std::cout << "ERROR::TERRAIN: Error closing the heightmap file!" << std::endl;

	// Copy the image data into the height map 
	for (int j = 0; j < this->mHeight; j++)
	{
		for (int i = 0; i < this->mWidth; i++)
		{
			index = (this->mWidth * j) + i;

			// Store the height at this point in the height map array
			this->mHeightmap[index].y = (float)rawImage[index];
		}
	}

	// Release the bitmap image data
	//delete[] rawImage;
	//rawImage = 0;
}

void TerrainObject::loadTargaHeightmap(std::string file)
{
	this->mHeightmap = new Heightmap[this->mWidth * this->mHeight];

	// Load targa height map image
	int width, height, bpp;
	unsigned char* image = stbi_load(file.c_str(), &width, &height, &bpp, 0);

	// Read the image data into the height map portion of the height map structure.
	int index = 0;
	int k = 0;
	for (int j = 0; j < this->mHeight; j++)
	{
		for (int i = 0; i < this->mWidth; i++)
		{
			index = (this->mWidth * (this->mHeight - 1 - j)) + i;

			float h = (float)image[k];

			this->mHeightmap[index].y = h;

			k += bpp;
		}
	}

	// And finally free image data
	stbi_image_free(image);
}

void TerrainObject::shutdownHeightMap()
{
	if (this->mHeightmap)
	{
		delete[] this->mHeightmap;
		this->mHeightmap = 0;
	}
}

void TerrainObject::setTerrainCoordinates()
{
	int index;

	for (int j = 0; j < this->mHeight; j++)
	{
		for (int i = 0; i < this->mWidth; i++)
		{
			index = (this->mWidth * j) + i;

			this->mHeightmap[index].x = (float)i;
			this->mHeightmap[index].z = -(float)j;

			mHeightmap[index].z += (float)(this->mHeight - 1);

			mHeightmap[index].y *= this->mHeightScale;
		}
	}
}

void TerrainObject::calculateNormals()
{
	int index, index1, index2, index3;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	Vector* normals;

	// Create a temporary array to hold the face normal vectors
	normals = new Vector[(this->mWidth - 1) * (this->mHeight - 1)];

	// Go through all the faces in the mesh and calculate their normals
	for (int j = 0; j < (this->mHeight - 1); j++)
	{
		for (int i = 0; i < (this->mHeight - 1); i++)
		{
			index1 = ((j + 1) * this->mWidth) + i;			// Bottom left vertex
			index2 = ((j + 1) * this->mWidth) + (i + 1);	// Bottom right vertex
			index3 = (j * this->mWidth) + i;				// Upper left vertex

			// Get three vertices from the face
			vertex1[0] = this->mHeightmap[index1].x;
			vertex1[1] = this->mHeightmap[index1].y;
			vertex1[2] = this->mHeightmap[index1].z;

			vertex2[0] = this->mHeightmap[index2].x;
			vertex2[1] = this->mHeightmap[index2].y;
			vertex2[2] = this->mHeightmap[index2].z;

			vertex3[0] = this->mHeightmap[index3].x;
			vertex3[1] = this->mHeightmap[index3].y;
			vertex3[2] = this->mHeightmap[index3].z;

			// Calculate the two vectors for this face
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (this->mWidth - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// Calculate the length
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				                 (normals[index].z * normals[index].z));

			// Normalize the final value for this face using the length
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);
		}
	}

	// Now go through all the vertices and take a sum of the face normals that touch this vertex
	for (int j = 0; j < this->mHeight; j++)
	{
		for (int i = 0; i < this->mWidth; i++)
		{
			// Initialize the sum
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (this->mWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face
			if ((i < (this->mWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (this->mWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face
			if (((i - 1) >= 0) && (j < (this->mHeight - 1)))
			{
				index = (j * (this->mWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face
			if ((i < (this->mWidth - 1)) && (j < (this->mHeight - 1)))
			{
				index = (j * (this->mWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Calculate the length of this normal
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array
			index = (j * this->mWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array
			this->mHeightmap[index].nx = (sum[0] / length);
			this->mHeightmap[index].ny = (sum[1] / length);
			this->mHeightmap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals
	delete[] normals;
	normals = 0;
}

void TerrainObject::loadBlendMap(std::string path)
{
	// Load blend map image
	int width, height, bpp;
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &bpp, 3);

	// Read the image data into the blend map portion of the height map structure.
	int index = 0;
	int k = 0;
	for (int j = 0; j < this->mHeight; j++)
	{
		for (int i = 0; i < this->mWidth; i++)
		{
			index = (this->mWidth * (this->mHeight - 1 - j)) + i;

			this->mHeightmap[index].r = (float)image[k + 0]	/ 255.0f;
			this->mHeightmap[index].g = (float)image[k + 1] / 255.0f;
			this->mHeightmap[index].b = (float)image[k + 2] / 255.0f;

			k += 3;
		}
	}

	// And finally free image data
	stbi_image_free(image);
}

void TerrainObject::buildTerrainModel()
{
	int index, index1, index2, index3, index4;

	this->mVertexCount = (this->mHeight - 1) * (this->mWidth - 1) * 6;

	this->mModel = new TerrainModel[this->mVertexCount];

	index = 0;
	for (int j = 0; j < (this->mHeight - 1); j++)
	{
		for (int i = 0; i < (this->mWidth - 1); i++)
		{
			index1 = (this->mWidth * j) + i;
			index2 = (this->mWidth * j) + (i + 1);
			index3 = (this->mWidth * (j + 1)) + i;
			index4 = (this->mWidth * (j + 1)) + (i + 1);

			this->mModel[index].x = this->mHeightmap[index1].x;
			this->mModel[index].y = this->mHeightmap[index1].y;
			this->mModel[index].z = this->mHeightmap[index1].z;
			this->mModel[index].tu = 0.0f;
			this->mModel[index].tv = 0.0f;
			this->mModel[index].nx = this->mHeightmap[index1].nx;
			this->mModel[index].ny = this->mHeightmap[index1].ny;
			this->mModel[index].nz = this->mHeightmap[index1].nz;
			this->mModel[index].r = this->mHeightmap[index1].r;
			this->mModel[index].g = this->mHeightmap[index1].g;
			this->mModel[index].b = this->mHeightmap[index1].b;
			index++;

			this->mModel[index].x = this->mHeightmap[index2].x;
			this->mModel[index].y = this->mHeightmap[index2].y;
			this->mModel[index].z = this->mHeightmap[index2].z;
			this->mModel[index].tu = 1.0f;
			this->mModel[index].tv = 0.0f;
			this->mModel[index].nx = this->mHeightmap[index2].nx;
			this->mModel[index].ny = this->mHeightmap[index2].ny;
			this->mModel[index].nz = this->mHeightmap[index2].nz;
			this->mModel[index].r = this->mHeightmap[index2].r;
			this->mModel[index].g = this->mHeightmap[index2].g;
			this->mModel[index].b = this->mHeightmap[index2].b;
			index++;

			this->mModel[index].x = this->mHeightmap[index3].x;
			this->mModel[index].y = this->mHeightmap[index3].y;
			this->mModel[index].z = this->mHeightmap[index3].z;
			this->mModel[index].tu = 0.0f;
			this->mModel[index].tv = 1.0f;
			this->mModel[index].nx = this->mHeightmap[index3].nx;
			this->mModel[index].ny = this->mHeightmap[index3].ny;
			this->mModel[index].nz = this->mHeightmap[index3].nz;
			this->mModel[index].r = this->mHeightmap[index3].r;
			this->mModel[index].g = this->mHeightmap[index3].g;
			this->mModel[index].b = this->mHeightmap[index3].b;
			index++;

			this->mModel[index].x = this->mHeightmap[index3].x;
			this->mModel[index].y = this->mHeightmap[index3].y;
			this->mModel[index].z = this->mHeightmap[index3].z;
			this->mModel[index].tu = 0.0f;
			this->mModel[index].tv = 1.0f;
			this->mModel[index].nx = this->mHeightmap[index3].nx;
			this->mModel[index].ny = this->mHeightmap[index3].ny;
			this->mModel[index].nz = this->mHeightmap[index3].nz;
			this->mModel[index].r = this->mHeightmap[index3].r;
			this->mModel[index].g = this->mHeightmap[index3].g;
			this->mModel[index].b = this->mHeightmap[index3].b;
			index++;

			this->mModel[index].x = this->mHeightmap[index2].x;
			this->mModel[index].y = this->mHeightmap[index2].y;
			this->mModel[index].z = this->mHeightmap[index2].z;
			this->mModel[index].tu = 1.0f;
			this->mModel[index].tv = 0.0f;
			this->mModel[index].nx = this->mHeightmap[index2].nx;
			this->mModel[index].ny = this->mHeightmap[index2].ny;
			this->mModel[index].nz = this->mHeightmap[index2].nz;
			this->mModel[index].r = this->mHeightmap[index2].r;
			this->mModel[index].g = this->mHeightmap[index2].g;
			this->mModel[index].b = this->mHeightmap[index2].b;
			index++;

			this->mModel[index].x = this->mHeightmap[index4].x;
			this->mModel[index].y = this->mHeightmap[index4].y;
			this->mModel[index].z = this->mHeightmap[index4].z;
			this->mModel[index].tu = 1.0f;
			this->mModel[index].tv = 1.0f;
			this->mModel[index].nx = this->mHeightmap[index4].nx;
			this->mModel[index].ny = this->mHeightmap[index4].ny;
			this->mModel[index].nz = this->mHeightmap[index4].nz;
			this->mModel[index].r = this->mHeightmap[index4].r;
			this->mModel[index].g = this->mHeightmap[index4].g;
			this->mModel[index].b = this->mHeightmap[index4].b;
			index++;
		}
	}
}

void TerrainObject::shutdownTerrainModel()
{
	if (this->mModel)
	{
		delete[] this->mModel;
		this->mModel = 0;
	}
}

void TerrainObject::calculateTerrainVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	Vector tangent, binormal;


	// Calculate the number of faces in the terrain model.
	faceCount = this->mVertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the terrain model.
		vertex1.x = this->mModel[index].x;
		vertex1.y = this->mModel[index].y;
		vertex1.z = this->mModel[index].z;
		vertex1.tu = this->mModel[index].tu;
		vertex1.tv = this->mModel[index].tv;
		vertex1.nx = this->mModel[index].nx;
		vertex1.ny = this->mModel[index].ny;
		vertex1.nz = this->mModel[index].nz;
		index++;

		vertex2.x = this->mModel[index].x;
		vertex2.y = this->mModel[index].y;
		vertex2.z = this->mModel[index].z;
		vertex2.tu = this->mModel[index].tu;
		vertex2.tv = this->mModel[index].tv;
		vertex2.nx = this->mModel[index].nx;
		vertex2.ny = this->mModel[index].ny;
		vertex2.nz = this->mModel[index].nz;
		index++;

		vertex3.x = this->mModel[index].x;
		vertex3.y = this->mModel[index].y;
		vertex3.z = this->mModel[index].z;
		vertex3.tu = this->mModel[index].tu;
		vertex3.tv = this->mModel[index].tv;
		vertex3.nx = this->mModel[index].nx;
		vertex3.ny = this->mModel[index].ny;
		vertex3.nz = this->mModel[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		this->calculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the tangent and binormal for this face back in the model structure.
		this->mModel[index - 1].tx = tangent.x;
		this->mModel[index - 1].ty = tangent.y;
		this->mModel[index - 1].tz = tangent.z;
		this->mModel[index - 1].bx = binormal.x;
		this->mModel[index - 1].by = binormal.y;
		this->mModel[index - 1].bz = binormal.z;

		this->mModel[index - 2].tx = tangent.x;
		this->mModel[index - 2].ty = tangent.y;
		this->mModel[index - 2].tz = tangent.z;
		this->mModel[index - 2].bx = binormal.x;
		this->mModel[index - 2].by = binormal.y;
		this->mModel[index - 2].bz = binormal.z;
		this->mModel[index - 3].by = binormal.y;
		this->mModel[index - 3].bz = binormal.z;

		this->mModel[index - 3].tx = tangent.x;
		this->mModel[index - 3].ty = tangent.y;
		this->mModel[index - 3].tz = tangent.z;
		this->mModel[index - 3].bx = binormal.x;
	}

	return;
}

void TerrainObject::calculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, 
											 Vector& tangent, Vector& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of the tangent.
	length = (float)sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the tangent and then store it.
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of the binormal.
	length = (float)sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the binormal and then store it.
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void TerrainObject::loadTerrainCells()
{
	int cellWidth, cellHeight, cellRowCount, index, modelIndex;

	// Width and height of each cell
	cellWidth = 33;
	cellHeight = 33;

	// Calculate the cell count
	cellRowCount = (this->mWidth - 1) / (cellWidth - 1);
	this->mCellsCount = cellRowCount * cellRowCount;

	// Initialize the cells array
	this->mCells = new TerrainCell[this->mCellsCount];

	// Loop over cells, and initialize them
	for (int j = 0; j < cellRowCount; j++)
	{
		for (int i = 0; i < cellRowCount; i++)
		{
			modelIndex = ((i * (cellWidth - 1)) + (j * (cellHeight - 1) * (this->mWidth - 1))) * 6;
			index = 0;

			// Calculate the vertex and index count for this cell
			int vertexCount = (cellHeight - 1) * ((cellWidth - 1) * 6);
			int indexCount = (cellHeight - 1) * ((cellWidth - 1) * 6);

			// Initialize vertex and index arrays
			TerrainVertex* vertices = new TerrainVertex[vertexCount];
			GLuint* indices = new GLuint[indexCount];

			// Loop over each vertex/index on this cell
			for (int h = 0; h < (cellHeight - 1); h++)
			{
				for (int g = 0; g < ((cellWidth - 1) * 6); g++)
				{
					TerrainVertex vertex;
					vertex.Position		= glm::vec3(this->mModel[modelIndex].x, this->mModel[modelIndex].y, this->mModel[modelIndex].z);
					vertex.Normal		= glm::vec3(this->mModel[modelIndex].nx, this->mModel[modelIndex].ny, this->mModel[modelIndex].nz);
					vertex.Tangent		= glm::vec3(this->mModel[modelIndex].tx, this->mModel[modelIndex].ty, this->mModel[modelIndex].tz);
					vertex.Binormal		= glm::vec3(this->mModel[modelIndex].bx, this->mModel[modelIndex].by, this->mModel[modelIndex].bz);
					vertex.TexCoords	= glm::vec2(this->mModel[modelIndex].tu, this->mModel[modelIndex].tv);
					vertex.Color		= glm::vec3(this->mModel[modelIndex].r, this->mModel[modelIndex].g, this->mModel[modelIndex].b);

					vertices[index] = vertex;
					indices[index] = index;
					modelIndex++;
					index++;
				}

				modelIndex += (this->mWidth * 6) - (cellWidth * 6);
			}

			// Create cell with vertex and index data
			index = (cellRowCount * j) + i;
			TerrainCell* cell = new TerrainCell(vertices, indices, vertexCount, indexCount, cellWidth, cellHeight, this->mWidth);
			this->mCells[index] = *cell;
		}
	}
}

void TerrainObject::setupRigidBody()
{
	PxTransform transform = PxTransform(PxVec3(this->mPosition.x, this->mPosition.y + 109.78f, this->mPosition.z),
		PxQuat(this->mRotation.w, PxVec3(this->mRotation.x, this->mRotation.y, this->mRotation.z)));
	this->mRigidBody = Physics::mPhysicsSDK->createRigidStatic(transform);

	int numRows = this->mHeight;
	int numCols = this->mWidth;
	PxU32* samples = (PxU32*)malloc(sizeof(PxU32) * numCols * numRows);

	PxHeightFieldDesc heightFieldDesc;
	heightFieldDesc.nbColumns = numCols;
	heightFieldDesc.nbRows = numRows;
	heightFieldDesc.samples.data = samples;
	heightFieldDesc.samples.stride = sizeof(PxU32);
	heightFieldDesc.convexEdgeThreshold = 0;

	PxU8* currentByte = (PxU8*)heightFieldDesc.samples.data;
	PxU32 texturePitch = 2;


	int index = 0;
	for (PxU32 j = 0; j < numRows; j++)
	{
		for (PxU32 i = 0; i < numCols; i++)
		{
			index = (numRows) * (numCols) - ((i + 1) * numCols) + j;

			PxHeightFieldSample* currentSample = (PxHeightFieldSample*)currentByte;
			currentSample->height = this->rawImage[index] - (SHRT_MAX + 1);
			currentSample->materialIndex0 = 0;
			currentSample->materialIndex1 = 0;
			currentSample->setTessFlag();

			currentByte += heightFieldDesc.samples.stride;
		}
	}

	PxHeightField* heightField = Physics::mCooking->createHeightField(heightFieldDesc, Physics::mPhysicsSDK->getPhysicsInsertionCallback());
	PxHeightFieldGeometry heightFieldGeometry(heightField, PxMeshGeometryFlags(), this->mHeightScale, this->mSize.x, this->mSize.z);
	PxShape* hfShape = this->mRigidBody->createShape(heightFieldGeometry, *Physics::mMaterial);
	hfShape->setFlag(PxShapeFlag::ePARTICLE_DRAIN, false);
	hfShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	hfShape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	PxActor* hfActor = hfShape->getActor();
	Physics::mScene->addActor(*hfActor);

	delete[] samples;
}