#include "StaticModelObject.h"

StaticModelObject::StaticModelObject(const glm::vec3 position, const glm::vec3 size,
									 const glm::vec3 velocity, const glm::vec4 color)
	: ModelObject(position, size, velocity, color)
{
	this->setupRigidBody();
}

StaticModelObject::StaticModelObject(Model* model, const glm::vec3 position, const glm::vec3 size,
									 const glm::vec3 velocity, const glm::vec4 color)
	: ModelObject(model, position, size, velocity, color)
{
	this->setupRigidBody();
}

StaticModelObject::~StaticModelObject()
{
	this->mRigidBody->release();
	this->mRigidBody = 0;
}

void StaticModelObject::Update(GLfloat dt)
{
	// Update the position and rotation
	PxTransform pose = this->mRigidBody->getGlobalPose();
	this->mPosition = glm::vec3(pose.p.x, pose.p.y, pose.p.z);
	this->mRotation = glm::quat(pose.q.w, pose.q.x, pose.q.y, pose.q.z);
}

void StaticModelObject::Draw(Renderer& renderer)
{
	renderer.DrawModel(this->mModel, this->mPosition, this->mRotation, this->mSize);
}

void StaticModelObject::setupRigidBody()
{
	PxTransform transform = PxTransform(PxVec3(this->mPosition.x, this->mPosition.y, this->mPosition.z),
		PxQuat(this->mRotation.w, PxVec3(this->mRotation.x, this->mRotation.y, this->mRotation.z)));
	this->mRigidBody = Physics::mPhysicsSDK->createRigidStatic(transform);

	for (int i = 0; i < this->mModel->mMeshCount; i++)
	{
		std::vector<PxVec3> convexVertices;
		std::vector<PxU32> convexIndices;

		for (int j = 0; j < this->mModel->mMeshes[i].mVertexCount; j++)
		{
			Vertex vertex = this->mModel->mMeshes[i].mVertices[j];
			int index = this->mModel->mMeshes[i].mIndices[j];
			convexVertices.push_back(PxVec3(vertex.Position.x, vertex.Position.y, vertex.Position.z));
			convexIndices.push_back(PxU32(index));
		}

		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = convexVertices.size();
		convexDesc.points.stride = sizeof(PxVec3);
		convexDesc.points.data = &convexVertices[0];
		convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		PxDefaultMemoryOutputStream buffer;
		PxConvexMeshCookingResult::Enum result;
		Physics::mCooking->cookConvexMesh(convexDesc, buffer, &result);
		PxDefaultMemoryInputData input(buffer.getData(), buffer.getSize());
		PxConvexMesh* convexMesh = Physics::mPhysicsSDK->createConvexMesh(input);

		PxConvexMeshGeometry convexGeometry(convexMesh);
		convexGeometry.scale = PxMeshScale(PxVec3(this->mSize.x, this->mSize.y, this->mSize.z), PxQuat(PxIdentity));
		this->mRigidBody->createShape(convexGeometry, *Physics::mMaterial);

		/*
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = convexVertices.size();
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = &convexVertices[0];

		meshDesc.triangles.count = convexIndices.size();
		meshDesc.triangles.stride = sizeof(PxU32);
		meshDesc.triangles.data = &convexIndices[0];

		Physics::mCooking->cookTriangleMesh(meshDesc, buffer);
		input = PxDefaultMemoryInputData(buffer.getData(), buffer.getSize());
		PxTriangleMesh* triangleMesh = Physics::mPhysicsSDK->createTriangleMesh(input);

		PxTriangleMeshGeometry triangleGeometry(triangleMesh);
		triangleGeometry.scale = PxMeshScale(PxVec3(this->mSize.x, this->mSize.y, this->mSize.z), PxQuat(0.0f, PxVec3(0.0f, 0.0f, 0.0f)));
		this->mRigidBody->createShape(triangleGeometry, *Physics::mMaterial);
		*/
	}

	Physics::mScene->addActor(*this->mRigidBody);
}