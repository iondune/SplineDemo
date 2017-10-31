
#pragma once

#include <ionEngine.h>


struct SQuad;

struct SEdge
{
	ion::vec3f * Ends[2];
	SQuad * Faces[2];
	ion::vec3f * EdgePoint;
	SEdge * SubDivide[2];

	SQuad * NotThis(SQuad const * const Quad)
	{
		assert(Quad == Faces[0] || Quad == Faces[1]);
		return Quad == Faces[0] ? Faces[1] : Faces[0];
	}

	SEdge * PickSub(ion::vec3f const * const Point)
	{
		for (int i = 0; i < 2; ++ i)
			for (int j = 0; j < 2; ++ j)
				if (SubDivide[i]->Ends[j] == Point)
					return SubDivide[i];

		assert(false);
		return 0;
	}

	ion::vec3f const GetMidPoint()
	{
		return (*Ends[0] + *Ends[1]) / 2.f;
	}

	void GiveFace(SQuad * Face)
	{
		assert(! Faces[0] || ! Faces[1]);
		(Faces[0] ? Faces[1] : Faces[0]) = Face;
	}

	SEdge()
		: EdgePoint(0)
	{
		Ends[0] = Ends[1] = 0;
		Faces[0] = Faces[1] = 0;
		SubDivide[0] = SubDivide[1] = 0;
	}
};

struct SFacePoint
{
	ion::vec3f * Point;
	SQuad * Quad;
	SEdge * Edges[4];

	SFacePoint()
	{}

	SFacePoint(ion::vec3f * point, SQuad * quad)
		: Point(point), Quad(quad)
	{}
};

struct SQuad
{
	SEdge * Edges[4];
	ion::vec3f * Vertices[4];
	SQuad * SubDivide[4];

	SFacePoint FacePoint;

	ion::vec3f & GetVertex(int const index);
	ion::vec3f GetCentroid();
	SEdge & GetEdge(int const index);

	/*void BuildVertices()
	{
	/*assert(Edges[0]->Ends[0] == Edges[3]->Ends[1] ||
	Edges[0]->Ends[0] == Edges[3]->Ends[1]);
	assert(Edges[1]->Ends[0] == Edges[0]->Ends[1] ||
	Edges[1]->Ends[0] == Edges[0]->Ends[1]);
	assert(Edges[2]->Ends[0] == Edges[1]->Ends[1] ||
	Edges[2]->Ends[0] == Edges[1]->Ends[1]);
	assert(Edges[3]->Ends[0] == Edges[2]->Ends[1] ||
	Edges[3]->Ends[0] == Edges[2]->Ends[1]);*//*

	Vertices[0] = Edges[0]->Ends[0];
	Vertices[1] = Edges[1]->Ends[0];
	Vertices[2] = Edges[2]->Ends[0];
	Vertices[3] = Edges[3]->Ends[0];
	}*/

	void AddFaceToEdges()
	{
		for (int i = 0; i < 4; ++ i)
			Edges[i]->GiveFace(this);
	}

	SQuad()
	{
		Edges[0] = Edges[1] = Edges[2] = Edges[3] = 0;
		Vertices[0] = Vertices[1] = Vertices[2] = Vertices[3] = 0;
	}
};

struct SMesh
{
	std::vector<SQuad *> Quads;
	std::vector<SEdge *> Edges;

	ion::Scene::CSimpleMesh * GenerateMesh(bool const PerVertexNormals);
	void Subdivide();
};


class CSubdivisionSurface
{

public:

	static int const MaxSubDivide = 5;

	ion::Scene::CSimpleMesh * CubeMesh = nullptr;

	ion::Scene::CSimpleMeshSceneObject * MeshObject = nullptr;
	ion::Scene::CSimpleMesh * Meshes[MaxSubDivide][2] = { nullptr };

	void MakeCube();
	void ResetMesh();

	int NormalMode = 0;
	int SubDivLevel = 0;

};
