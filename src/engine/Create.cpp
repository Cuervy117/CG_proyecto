#include "Create.hpp"

#include <glm.hpp>

Geometria geometria;

static void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	geometria.meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	geometria.meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	geometria.meshListModel.push_back(obj3);

	MeshModel *obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	geometria.meshListModel.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	geometria.pisoMesh = obj3;
	geometria.vegetacionMesh = obj4;
}


void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};
	GLfloat cubo_vertices[] = {
		//x			y			z			S			T			NX		NY		NZ
		-0.500000f,	-0.500000f,	0.500000f,	0.034461f,	0.323295f,	0.000000f,	0.000000f,	1.000000f,	//0
		0.500000f,	-0.500000f,	0.500000f,	0.346941f,	0.322110f,	0.000000f,	0.000000f,	1.000000f,	//1
		0.500000f,	0.500000f,	0.500000f,	0.345634f,	0.613796f,	0.000000f,	0.000000f,	1.000000f,	//2
		-0.500000f,	0.500000f,	0.500000f,	0.031019f,	0.618704f,	0.000000f,	0.000000f,	1.000000f,	//3

		0.500000f,	-0.500000f,	0.500000f,	0.383404f,	0.648398f,	1.000000f,	0.000000f,	0.000000f,	//4
		0.500000f,	-0.500000f,	-0.500000f,	0.660242f,	0.647082f,	1.000000f,	0.000000f,	0.000000f,	//5
		0.500000f,	0.500000f,	-0.500000f,	0.656243f,	0.920243f,	1.000000f,	0.000000f,	0.000000f,	//6
		0.500000f,	0.500000f,	0.500000f,	0.382544f,	0.918346f,	1.000000f,	0.000000f,	0.000000f,	//7

		-0.500000f,	-0.500000f,	-0.500000f,	0.635690f,	0.333037f,	0.000000f,	0.000000f,	-1.000000f,	//8
		0.500000f,	-0.500000f,	-0.500000f,	0.354521f,	0.335013f,	0.000000f,	0.000000f,	-1.000000f,	//9
		0.500000f,	0.500000f,	-0.500000f,	0.356498f,	0.616182f,	0.000000f,	0.000000f,	-1.000000f,	//10
		-0.500000f,	0.500000f,	-0.500000f,	0.637667f,	0.614205f,	0.000000f,	0.000000f,	-1.000000f,	//11

		-0.500000f,	-0.500000f,	-0.500000f,	0.043326f,	0.632830f,	-1.000000f,	0.000000f,	0.000000f,	//12
		-0.500000f,	-0.500000f,	0.500000f,	0.381357f,	0.636845f,	-1.000000f,	0.000000f,	0.000000f,	//13
		-0.500000f,	0.500000f,	0.500000f,	0.375929f,	0.926307f,	-1.000000f,	0.000000f,	0.000000f,	//14
		-0.500000f,	0.500000f,	-0.500000f,	0.037898f,	0.922291f,	-1.000000f,	0.000000f,	0.000000f,	//15

		-0.500000f,	-0.500000f,	0.500000f,	0.941874f,	0.316593f,	0.000000f,	-1.000000f,	0.000000f,	//16
		0.500000f,	-0.500000f,	0.500000f,	0.648864f,	0.313120f,	0.000000f,	-1.000000f,	0.000000f,	//17
		0.500000f,	-0.500000f,	-0.500000f,	0.650521f,	0.622226f,	0.000000f,	-1.000000f,	0.000000f,	//18
		-0.500000f,	-0.500000f,	-0.500000f,	0.941943f,	0.625014f,	0.000000f,	-1.000000f,	0.000000f,	//19

		-0.500000f,	0.500000f,	0.500000f,	0.993109f,	0.931856f,	0.000000f,	1.000000f,	0.000000f,	//20
		0.500000f,	0.500000f,	0.500000f,	0.679358f,	0.934219f,	0.000000f,	1.000000f,	0.000000f,	//21
		0.500000f,	0.500000f,	-0.500000f,	0.670953f,	0.642038f,	0.000000f,	1.000000f,	0.000000f,	//22
		-0.500000f,	0.500000f,	-0.500000f,	0.993109f,	0.642038f,	0.000000f,	1.000000f,	0.000000f,	//23
	};

	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	geometria.meshListModel.push_back(dado);

	geometria.dadoMesh = dado;

}


void CrearHolocron()
{
	unsigned int holocron_indices[] = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23,

		24, 25, 26,

		27, 28, 29,

		30, 31, 32,

		33, 34, 35,

		36, 37, 38,

		39, 40, 41,

		42, 43, 44,

		45, 46, 47
	};

	GLfloat holocron_vertices[] = {
		//x			y			z			S			T			NX		NY		NZ
		-0.140696f,	-2.601934f,	2.558561f,	0.242541f,	0.212349f,	-0.0224f,	0.0036f,	0.9997f,	//0
		2.593003f,	-0.085222f,	2.505755f,	0.247165f,	0.402678f,	-0.0224f,	0.0036f,	0.9997f,	//1
		0.051152f,	2.399781f,	2.545043f,	0.083854f,	0.398037f,	-0.0224f,	0.0036f,	0.9997f,	//2
		-2.646786f,	-0.139047f,	2.493757f,	0.081088f,	0.208232f,	-0.0224f,	0.0036f,	0.9997f,	//3

		0.049649f,	2.440128f,	-2.417217f,	0.811394f,	0.401339f,	-0.0114f,	0.9999f,	0.0081f,	//4
		-2.451372f,	2.421759f,	0.077538f,	0.810699f,	0.209005f,	-0.0114f,	0.9999f,	0.0081f,	//5
		0.051152f,	2.399781f,	2.545043f,	1.002047f,	0.209892f,	-0.0114f,	0.9999f,	0.0081f,	//6
		2.521206f,	2.448241f,	0.063388f,	1.002047f,	0.400547f,	-0.0114f,	0.9999f,	0.0081f,	//7

		2.632201f,	-2.553892f,	0.007226f,	0.427512f,	0.204151f,	0.9997f,	0.0223f,	-0.0063f,	//8
		2.534944f,	-0.061146f,	-2.580280f,	0.432958f,	0.396288f,	0.9997f,	0.0223f,	-0.0063f,	//9
		2.521206f,	2.448241f,	0.063388f,	0.242450f,	0.392942f,	0.9997f,	0.0223f,	-0.0063f,	//10
		2.593003f,	-0.085222f,	2.505755f,	0.243904f,	0.204723f,	0.9997f,	0.0223f,	-0.0063f,	//11

		-0.140696f,	-2.601934f,	2.558561f,	0.035624f,	0.404892f,	0.0010f,	-0.9998f,	-0.0178f,	//12
		-2.517274f,	-2.579095f,	0.002115f,	0.220434f,	0.403956f,	0.0010f,	-0.9998f,	-0.0178f,	//13
		0.081432f,	-2.513204f,	-2.426005f,	0.221888f,	0.592288f,	0.0010f,	-0.9998f,	-0.0178f,	//14
		2.632201f,	-2.553892f,	0.007226f,	0.035624f,	0.604413f,	0.0010f,	-0.9998f,	-0.0178f,	//15

		-2.453980f,	-0.063388f,	-2.477451f,	0.620970f,	0.402874f,	-0.9985f,	0.0402f,	-0.0381f,	//16
		-2.517274f,	-2.579095f,	0.002115f,	0.621797f,	0.208151f,	-0.9985f,	0.0402f,	-0.0381f,	//17
		-2.646786f,	-0.139047f,	2.493757f,	0.814150f,	0.208151f,	-0.9985f,	0.0402f,	-0.0381f,	//18
		-2.451372f,	2.421759f,	0.077538f,	0.817425f,	0.402479f,	-0.9985f,	0.0402f,	-0.0381f,	//19

		0.081432f,	-2.513204f,	-2.426005f,	0.423064f,	0.203328f,	-0.0641f,	0.0014f,	-0.9979f,	//20
		-2.453980f,	-0.063388f,	-2.477451f,	0.630670f,	0.199640f,	-0.0641f,	0.0014f,	-0.9979f,	//21
		0.049649f,	2.440128f,	-2.417217f,	0.630933f,	0.408171f,	-0.0641f,	0.0014f,	-0.9979f,	//22
		2.534944f,	-0.061146f,	-2.580280f,	0.423064f,	0.407801f,	-0.0641f,	0.0014f,	-0.9979f,	//23

		2.521206f,	2.448241f,	0.063388f,	0.784456f,	1.003642f,	0.5661f,	0.5994f,	-0.5660f,	//24
		2.534944f,	-0.061146f,	-2.580280f,	0.670671f,	0.793745f,	0.5661f,	0.5994f,	-0.5660f,	//25
		0.049649f,	2.440128f,	-2.417217f,	0.883446f,	0.796365f,	0.5661f,	0.5994f,	-0.5660f,	//26

		-2.451372f,	2.421759f,	0.077538f,	0.395322f,	0.994495f,	-0.5649f,	0.5887f,	0.5782f,	//27
		-2.646786f,	-0.139047f,	2.493757f,	0.283893f,	0.802262f,	-0.5649f,	0.5887f,	0.5782f,	//28
		0.051152f,	2.399781f,	2.545043f,	0.496727f,	0.802657f,	-0.5649f,	0.5887f,	0.5782f,	//29

		0.049649f,	2.440128f,	-2.417217f,	0.183152f,	1.009891f,	-0.5737f,	0.5874f,	-0.5708f,	//30
		-2.453980f,	-0.063388f,	-2.477451f,	0.047649f,	0.800203f,	-0.5737f,	0.5874f,	-0.5708f,	//31
		-2.451372f,	2.421759f,	0.077538f,	0.317974f,	0.801380f,	-0.5737f,	0.5874f,	-0.5708f,	//32

		0.051152f,	2.399781f,	2.545043f,	0.590604f,	1.005405f,	0.5732f,	0.5771f,	0.5818f,	//33
		2.593003f,	-0.085222f,	2.505755f,	0.466767f,	0.796166f,	0.5732f,	0.5771f,	0.5818f,	//34
		2.521206f,	2.448241f,	0.063388f,	0.710721f,	0.799752f,	0.5732f,	0.5771f,	0.5818f,	//35

		-2.453980f,	-0.063388f,	-2.477451f,	0.772987f,	0.794828f,	-0.5482f,	-0.5800f,	-0.6025f,	//36
		0.081432f,	-2.513204f,	-2.426005f,	0.650003f,	0.598553f,	-0.5482f,	-0.5800f,	-0.6025f,	//37
		-2.517274f,	-2.579095f,	0.002115f,	0.889787f,	0.597754f,	-0.5482f,	-0.5800f,	-0.6025f,	//38

		2.593003f,	-0.085222f,	2.505755f,	0.355529f,	0.799010f,	0.5530f,	-0.5883f,	0.5900f,	//39
		-0.140696f,	-2.601934f,	2.558561f,	0.246429f,	0.604556f,	0.5530f,	-0.5883f,	0.5900f,	//40
		2.632201f,	-2.553892f,	0.007226f,	0.468927f,	0.605599f,	0.5530f,	-0.5883f,	0.5900f,	//41

		2.534944f,	-0.061146f,	-2.580280f,	0.561937f,	0.802150f,	0.5524f,	-0.5898f,	-0.5890f,	//42
		2.632201f,	-2.553892f,	0.007226f,	0.446122f,	0.602217f,	0.5524f,	-0.5898f,	-0.5890f,	//43
		0.081432f,	-2.513204f,	-2.426005f,	0.686198f,	0.603793f,	0.5524f,	-0.5898f,	-0.5890f,	//44

		-2.646786f,	-0.139047f,	2.493757f,	0.160505f,	0.801074f,	-0.5940f,	-0.5900f,	0.5469f,	//45
		-2.517274f,	-2.579095f,	0.002115f,	0.046338f,	0.601751f,	-0.5940f,	-0.5900f,	0.5469f,	//46
		-0.140696f,	-2.601934f,	2.558561f,	0.265788f,	0.603836f,	-0.5940f,	-0.5900f,	0.5469f,	//47
	};

	MeshModel* holocron = new MeshModel();
	holocron->CreateMeshModel(holocron_vertices, holocron_indices, 384, 60);
	geometria.meshListModel.push_back(holocron);

	geometria.holocronMesh = holocron;

}

void CreateScene()
{
	CreateObjects();   // triangulos, piso y vegetacion  
	CrearDado();       // cubo con los logos               
	CrearHolocron();   // holocron                         
}
void CrearQuad()
{
	// quad unitario en el plano XY: de -0.5 a 0.5, UV de 0 a 1, normal +Z
	float quadVertices[] = {
		//	x	 y	  z			u	  v		 nx   ny   nz
		-0.5f, -0.5f, 0.0f,	0.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,	1.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,	1.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,	0.0f, 1.0f,	0.0f, 0.0f, 1.0f
	};

	unsigned int quadIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	geometria.quadMesh = new MeshModel();
	geometria.quadMesh->CreateMeshModel(quadVertices, quadIndices, 32, 6);
	geometria.meshListModel.push_back(geometria.quadMesh);
}

void CrearLlamas()
{
	
	float llamaVertices[] = {
		-0.45f, -0.45f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		 0.45f, -0.45f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		 0.45f,  0.45f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		-0.45f,  0.45f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f
	};

	unsigned int llamaIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	geometria.llamaMesh = new MeshModel();
	geometria.llamaMesh->CreateMeshModel(llamaVertices, llamaIndices, 4 * 8, 6);
	geometria.meshListModel.push_back(geometria.llamaMesh);
}