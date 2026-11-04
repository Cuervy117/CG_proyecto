#include "Create.hpp"


void CreateObjects()
{
	// [0] cuadro unitario en el plano XY: de -0.5 a 0.5, UV de 0 a 1, normal +Z
	unsigned int cuadroIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	GLfloat cuadroVertices[] = {
		//	x		y		z		u		v		nx		ny		nz
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f
	};

	// [1] llama de los motores: cuadro con la textura fuego.png (se dibuja con transparencia)
	unsigned int llamaIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	GLfloat llamaVertices[] = {
		//	x		y		z		u		v		nx		ny		nz
		-0.45f, -0.45f, 0.0f,	0.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		 0.45f, -0.45f, 0.0f,	1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		 0.45f,  0.45f, 0.0f,	1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		-0.45f,  0.45f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, -1.0f
	};

	// [2] cubo unitario (de -0.5 a 0.5), como el dado de la práctica: 4 vértices por cara para que
	// cada cara tenga su propia normal. Las normales van invertidas igual que en el dado.
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
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
		22, 23, 20
	};

	GLfloat cubo_vertices[] = {
		// front
		// x		y		z		S		T		NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		0.0f,  0.0f, -1.0f,	//0
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f,  0.0f, -1.0f,	//1
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		0.0f,  0.0f, -1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		0.0f,  0.0f, -1.0f,	//3
		// right
		 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		-1.0f, 0.0f,  0.0f,	//4
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		-1.0f, 0.0f,  0.0f,	//5
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		-1.0f, 0.0f,  0.0f,	//6
		 0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		-1.0f, 0.0f,  0.0f,	//7
		// back
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		0.0f,  0.0f,  1.0f,	//8
		 0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f,  0.0f,  1.0f,	//9
		 0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f,  0.0f,  1.0f,	//10
		-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		0.0f,  0.0f,  1.0f,	//11
		// left
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		1.0f,  0.0f,  0.0f,	//12
		-0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		1.0f,  0.0f,  0.0f,	//13
		-0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		1.0f,  0.0f,  0.0f,	//14
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		1.0f,  0.0f,  0.0f,	//15
		// bottom
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		0.0f,  1.0f,  0.0f,	//16
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f,  1.0f,  0.0f,	//17
		 0.5f, -0.5f, -0.5f,	1.0f, 1.0f,		0.0f,  1.0f,  0.0f,	//18
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		0.0f,  1.0f,  0.0f,	//19
		// top
		-0.5f,  0.5f,  0.5f,	0.0f, 0.0f,		0.0f, -1.0f,  0.0f,	//20
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		0.0f, -1.0f,  0.0f,	//21
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		0.0f, -1.0f,  0.0f,	//22
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f, -1.0f,  0.0f	//23
	};

	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(cuadroVertices, cuadroIndices, 32, 6);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(llamaVertices, llamaIndices, 32, 6);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(obj3);
}
