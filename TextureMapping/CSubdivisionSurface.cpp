
#include "CSubdivisionSurface.h"
#include "CApplication.h"

using namespace ion;


vec3f & SQuad::GetVertex(int const index)
{
	return *Vertices[index];
}

vec3f SQuad::GetCentroid()
{
	return (GetVertex(0) + GetVertex(1) + GetVertex(2) + GetVertex(3)) / 4.f;
}

SEdge & SQuad::GetEdge(int const index)
{
	return *Edges[index];
}

Scene::CSimpleMesh * SMesh::GenerateMesh(bool const PerVertexNormals)
{
	Scene::CSimpleMesh * Buffer = new ion::Scene::CSimpleMesh();
	int IndexCounter = 0;

	for (auto Quad : Quads)
	{
		ion::Scene::CSimpleMesh::SVertex Vertex;
		Vertex.Color = vec3f(1);

		Vertex.Position = Quad->GetVertex(0);
		Buffer->Vertices.push_back(Vertex);
		Vertex.Position = Quad->GetVertex(1);
		Buffer->Vertices.push_back(Vertex);
		Vertex.Position = Quad->GetVertex(2);
		Buffer->Vertices.push_back(Vertex);
		Vertex.Position = Quad->GetVertex(3);
		Buffer->Vertices.push_back(Vertex);

		Scene::CSimpleMesh::STriangle Triangle;
		Triangle.Indices[0] = IndexCounter + 0;
		Triangle.Indices[1] = IndexCounter + 2;
		Triangle.Indices[2] = IndexCounter + 1;
		Buffer->Triangles.push_back(Triangle);

		Triangle.Indices[0] = IndexCounter + 0;
		Triangle.Indices[1] = IndexCounter + 3;
		Triangle.Indices[2] = IndexCounter + 2;
		Buffer->Triangles.push_back(Triangle);

		IndexCounter += 4;
	}

	if (PerVertexNormals)
		Buffer->CalculateNormalsPerVertex(true, 0.00001f);
	else
		Buffer->CalculateNormalsPerFace();

	return Buffer;
}

void SMesh::Subdivide()
{
	std::map<vec3f *, std::vector<vec3f>> FValues;
	std::map<vec3f *, std::vector<vec3f>> RValues;

	// Create Face Points
	for (auto Quad : Quads)
	{
		Quad->FacePoint = SFacePoint(new vec3f(Quad->GetCentroid()), Quad);
		FValues[Quad->Vertices[0]].push_back(*Quad->FacePoint.Point);
		FValues[Quad->Vertices[1]].push_back(*Quad->FacePoint.Point);
		FValues[Quad->Vertices[2]].push_back(*Quad->FacePoint.Point);
		FValues[Quad->Vertices[3]].push_back(*Quad->FacePoint.Point);
	}

	// Create Edge Points
	for (auto Edge : Edges)
	{
		Edge->EdgePoint = new vec3f((
			*Edge->Ends[0] + *Edge->Ends[1] + 
			Edge->Faces[0]->GetCentroid() + Edge->Faces[1]->GetCentroid()) / 4.f);
	}

	// Create New Edges
	std::vector<SEdge *> NewEdges;
	for (auto Quad : Quads)
	{
		SFacePoint & FacePoint = Quad->FacePoint;
		for (int i = 0; i < 4; ++ i)
		{
			NewEdges.push_back(new SEdge);
			NewEdges.back()->Ends[0] = FacePoint.Point;
			NewEdges.back()->Ends[1] = FacePoint.Quad->Edges[i]->EdgePoint;
			FacePoint.Edges[i] = NewEdges.back();
		}
	}

	// Subdivide Existing Edges
	for (auto Edge : Edges)
	{	
		NewEdges.push_back(new SEdge);
		NewEdges.back()->Ends[0] = Edge->Ends[0];
		NewEdges.back()->Ends[1] = Edge->EdgePoint;
		Edge->SubDivide[0] = NewEdges.back();
		RValues[Edge->Ends[0]].push_back(Edge->GetMidPoint());

		NewEdges.push_back(new SEdge);
		NewEdges.back()->Ends[0] = Edge->EdgePoint;
		NewEdges.back()->Ends[1] = Edge->Ends[1];
		Edge->SubDivide[1] = NewEdges.back();
		RValues[Edge->Ends[1]].push_back(Edge->GetMidPoint());
	}

	// Subdivide Faces
	std::vector<SQuad *> NewQuads;
	for (auto Quad : Quads)
	{
		SQuad * NewQuad = new SQuad;
		NewQuad->Edges[0] = Quad->Edges[0]->PickSub(Quad->Vertices[0]);
		NewQuad->Edges[1] = Quad->FacePoint.Edges[0];
		NewQuad->Edges[2] = Quad->FacePoint.Edges[3];
		NewQuad->Edges[3] = Quad->Edges[3]->PickSub(Quad->Vertices[0]);
		NewQuad->AddFaceToEdges();
		//NewQuad->BuildVertices();
		NewQuad->Vertices[0] = Quad->Vertices[0];
		NewQuad->Vertices[1] = Quad->Edges[0]->EdgePoint;
		NewQuad->Vertices[2] = Quad->FacePoint.Point;
		NewQuad->Vertices[3] = Quad->Edges[3]->EdgePoint;
		Quad->SubDivide[0] = NewQuad;
		NewQuads.push_back(NewQuad);

		NewQuad = new SQuad;
		NewQuad->Edges[0] = Quad->Edges[0]->PickSub(Quad->Vertices[1]);
		NewQuad->Edges[1] = Quad->Edges[1]->PickSub(Quad->Vertices[1]);
		NewQuad->Edges[2] = Quad->FacePoint.Edges[1];
		NewQuad->Edges[3] = Quad->FacePoint.Edges[0];
		NewQuad->AddFaceToEdges();
		//NewQuad->BuildVertices();
		NewQuad->Vertices[0] = Quad->Edges[0]->EdgePoint;
		NewQuad->Vertices[1] = Quad->Vertices[1];
		NewQuad->Vertices[2] = Quad->Edges[1]->EdgePoint;
		NewQuad->Vertices[3] = Quad->FacePoint.Point;
		Quad->SubDivide[1] = NewQuad;
		NewQuads.push_back(NewQuad);

		NewQuad = new SQuad;
		NewQuad->Edges[0] = Quad->FacePoint.Edges[1];
		NewQuad->Edges[1] = Quad->Edges[1]->PickSub(Quad->Vertices[2]);
		NewQuad->Edges[2] = Quad->Edges[2]->PickSub(Quad->Vertices[2]);
		NewQuad->Edges[3] = Quad->FacePoint.Edges[2];
		NewQuad->AddFaceToEdges();
		//NewQuad->BuildVertices();
		NewQuad->Vertices[0] = Quad->FacePoint.Point;
		NewQuad->Vertices[1] = Quad->Edges[1]->EdgePoint;
		NewQuad->Vertices[2] = Quad->Vertices[2];
		NewQuad->Vertices[3] = Quad->Edges[2]->EdgePoint;
		Quad->SubDivide[2] = NewQuad;
		NewQuads.push_back(NewQuad);

		NewQuad = new SQuad;
		NewQuad->Edges[0] = Quad->FacePoint.Edges[3];
		NewQuad->Edges[1] = Quad->FacePoint.Edges[2];
		NewQuad->Edges[2] = Quad->Edges[2]->PickSub(Quad->Vertices[3]);
		NewQuad->Edges[3] = Quad->Edges[3]->PickSub(Quad->Vertices[3]);
		NewQuad->AddFaceToEdges();
		//NewQuad->BuildVertices();
		NewQuad->Vertices[0] = Quad->Edges[3]->EdgePoint;
		NewQuad->Vertices[1] = Quad->FacePoint.Point;
		NewQuad->Vertices[2] = Quad->Edges[2]->EdgePoint;
		NewQuad->Vertices[3] = Quad->Vertices[3];
		Quad->SubDivide[3] = NewQuad;
		NewQuads.push_back(NewQuad);
	}

	for (auto Quad : Quads)
	{
		for (int i = 0; i < 4; ++ i)
		{
			assert(FValues[Quad->Vertices[i]].size() == RValues[Quad->Vertices[i]].size());
			vec3f P = *Quad->Vertices[i];
			float n = (float) FValues[Quad->Vertices[i]].size();

			vec3f F;
			for (auto Value : FValues[Quad->Vertices[i]])
				F += Value;
			F /= n;

			vec3f R; 
			for (auto Value : RValues[Quad->Vertices[i]])
				R+= Value;
			R /= n;

			*Quad->Vertices[i] = (F + R * 2 + P * (n - 3)) / n;
		}
	}

	Quads = NewQuads;
	Edges = NewEdges;
}

void CSubdivisionSurface::MakeCube()
{
	// Make Cube
	SMesh Mesh;
	std::vector<vec3f *> Vertices;
	Vertices.push_back(new vec3f(0, 0, 0));
	Vertices.push_back(new vec3f(1, 0, 0));
	Vertices.push_back(new vec3f(1, 1, 0));
	Vertices.push_back(new vec3f(0, 1, 0));
	Vertices.push_back(new vec3f(0, 0, 1));
	Vertices.push_back(new vec3f(1, 0, 1));
	Vertices.push_back(new vec3f(1, 1, 1));
	Vertices.push_back(new vec3f(0, 1, 1));

	std::vector<SEdge *> Edges;
	for (int i = 0; i < 12; ++ i)
		Edges.push_back(new SEdge);

	Edges[0]->Ends[0] = Vertices[0];
	Edges[0]->Ends[1] = Vertices[1];

	Edges[1]->Ends[0] = Vertices[1];
	Edges[1]->Ends[1] = Vertices[2];

	Edges[2]->Ends[0] = Vertices[2];
	Edges[2]->Ends[1] = Vertices[3];

	Edges[3]->Ends[0] = Vertices[3];
	Edges[3]->Ends[1] = Vertices[0];

	Edges[4]->Ends[0] = Vertices[4];
	Edges[4]->Ends[1] = Vertices[5];

	Edges[5]->Ends[0] = Vertices[5];
	Edges[5]->Ends[1] = Vertices[6];

	Edges[6]->Ends[0] = Vertices[6];
	Edges[6]->Ends[1] = Vertices[7];

	Edges[7]->Ends[0] = Vertices[7];
	Edges[7]->Ends[1] = Vertices[4];

	Edges[8]->Ends[0] = Vertices[1];
	Edges[8]->Ends[1] = Vertices[5];

	Edges[9]->Ends[0] = Vertices[2];
	Edges[9]->Ends[1] = Vertices[6];

	Edges[10]->Ends[0] = Vertices[0];
	Edges[10]->Ends[1] = Vertices[4];

	Edges[11]->Ends[0] = Vertices[3];
	Edges[11]->Ends[1] = Vertices[7];

	SQuad * Quads[6];
	for (int i = 0; i < 6; ++ i)
		Quads[i] = new SQuad;

	Quads[0]->Edges[0] = Edges[0];
	Quads[0]->Edges[1] = Edges[1];
	Quads[0]->Edges[2] = Edges[2];
	Quads[0]->Edges[3] = Edges[3];
	Quads[0]->Vertices[0] = Vertices[0];
	Quads[0]->Vertices[1] = Vertices[1];
	Quads[0]->Vertices[2] = Vertices[2];
	Quads[0]->Vertices[3] = Vertices[3];
	//Quads[0]->BuildVertices();

	Quads[1]->Edges[0] = Edges[8];
	Quads[1]->Edges[1] = Edges[5];
	Quads[1]->Edges[2] = Edges[9];
	Quads[1]->Edges[3] = Edges[1];
	Quads[1]->Vertices[0] = Vertices[1];
	Quads[1]->Vertices[1] = Vertices[5];
	Quads[1]->Vertices[2] = Vertices[6];
	Quads[1]->Vertices[3] = Vertices[2];
	//Quads[1]->BuildVertices();

	Quads[2]->Edges[0] = Edges[4];
	Quads[2]->Edges[1] = Edges[7];
	Quads[2]->Edges[2] = Edges[6];
	Quads[2]->Edges[3] = Edges[5];
	Quads[2]->Vertices[0] = Vertices[5];
	Quads[2]->Vertices[1] = Vertices[4];
	Quads[2]->Vertices[2] = Vertices[7];
	Quads[2]->Vertices[3] = Vertices[6];
	//Quads[2]->BuildVertices();

	Quads[3]->Edges[0] = Edges[10];
	Quads[3]->Edges[1] = Edges[3];
	Quads[3]->Edges[2] = Edges[11];
	Quads[3]->Edges[3] = Edges[7];
	Quads[3]->Vertices[0] = Vertices[4];
	Quads[3]->Vertices[1] = Vertices[0];
	Quads[3]->Vertices[2] = Vertices[3];
	Quads[3]->Vertices[3] = Vertices[7];
	//Quads[3]->BuildVertices();

	Quads[4]->Edges[0] = Edges[4];
	Quads[4]->Edges[1] = Edges[8];
	Quads[4]->Edges[2] = Edges[0];
	Quads[4]->Edges[3] = Edges[10];
	Quads[4]->Vertices[0] = Vertices[4];
	Quads[4]->Vertices[1] = Vertices[5];
	Quads[4]->Vertices[2] = Vertices[1];
	Quads[4]->Vertices[3] = Vertices[0];
	//Quads[4]->BuildVertices();

	Quads[5]->Edges[0] = Edges[2];
	Quads[5]->Edges[1] = Edges[9];
	Quads[5]->Edges[2] = Edges[6];
	Quads[5]->Edges[3] = Edges[11];
	Quads[5]->Vertices[0] = Vertices[3];
	Quads[5]->Vertices[1] = Vertices[2];
	Quads[5]->Vertices[2] = Vertices[6];
	Quads[5]->Vertices[3] = Vertices[7];
	//Quads[5]->BuildVertices();

	Edges[0]->Faces[0] = Quads[0];
	Edges[0]->Faces[1] = Quads[4];

	Edges[1]->Faces[0] = Quads[0];
	Edges[1]->Faces[1] = Quads[1];

	Edges[2]->Faces[0] = Quads[0];
	Edges[2]->Faces[1] = Quads[5];

	Edges[3]->Faces[0] = Quads[0];
	Edges[3]->Faces[1] = Quads[3];

	Edges[4]->Faces[0] = Quads[2];
	Edges[4]->Faces[1] = Quads[4];

	Edges[5]->Faces[0] = Quads[2];
	Edges[5]->Faces[1] = Quads[1];

	Edges[6]->Faces[0] = Quads[2];
	Edges[6]->Faces[1] = Quads[5];

	Edges[7]->Faces[0] = Quads[2];
	Edges[7]->Faces[1] = Quads[3];

	Edges[8]->Faces[0] = Quads[4];
	Edges[8]->Faces[1] = Quads[1];

	Edges[9]->Faces[0] = Quads[1];
	Edges[9]->Faces[1] = Quads[5];

	Edges[10]->Faces[0] = Quads[4];
	Edges[10]->Faces[1] = Quads[3];

	Edges[11]->Faces[0] = Quads[3];
	Edges[11]->Faces[1] = Quads[5];

	for (int i = 0; i < 6; ++ i)
		Mesh.Quads.push_back(Quads[i]);

	Mesh.Edges = Edges;

	Meshes[0][0] = Mesh.GenerateMesh(false);
	Meshes[0][1] = Mesh.GenerateMesh(true);

	for (int i = 1; i < MaxSubDivide; ++ i)
	{
		ion::CStopWatch sw;
		sw.Start();

		Log::Info("Generating subdivision %d...", i);
		Mesh.Subdivide();
		Meshes[i][0] = Mesh.GenerateMesh(false);
		Meshes[i][1] = Mesh.GenerateMesh(true);
		Log::Info("Done in %.2fs.", sw.Stop());
	}

	ResetMesh();
}

void CSubdivisionSurface::ResetMesh()
{
	SingletonPointer<CApplication> Application;

	if (! MeshObject)
	{
		MeshObject = new Scene::CSimpleMeshSceneObject();
		MeshObject->SetShader(Application->DiffuseShader);
		MeshObject->SetPosition(vec3f(3, 3, 0));
		MeshObject->SetUniform("uColor", Graphics::CUniform<color3f>(Color::Hex(0x367DE0)));
		Application->RenderPass->AddSceneObject(MeshObject);
	}

	MeshObject->SetMesh(Meshes[SubDivLevel][NormalMode]);
}
