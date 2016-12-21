#include "Graphics/Window.h"
#include "Graphics/GraphicsEngine.h"
#include "Graphics/Mesh.h"
#include <Windows.h>
#include <vector>
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Camera.h"
#include "Graphics/ConstantBuffer.h"

static void CreateCube(std::vector<Graphics::Mesh::Vertex>& verts, std::vector<unsigned short>& indices)
{
	const float alpha = 1;
	verts.push_back(Graphics::Mesh::Vertex{ { 0.5f, 0.5f, 0.5f },{ 1, 1, 1, alpha } });
	verts.push_back(Graphics::Mesh::Vertex{ { 0.5f, 0.5f, -0.5f },{ 1, 1, 0, alpha } });
	verts.push_back(Graphics::Mesh::Vertex{ { 0.5f, -0.5f, 0.5f },{ 1, 0, 1, alpha } });
	verts.push_back(Graphics::Mesh::Vertex{ { 0.5f, -0.5f, -0.5f },{ 1, 0, 0, alpha } });
	verts.push_back(Graphics::Mesh::Vertex{ { -0.5f, 0.5f, 0.5f },{ 0, 1, 1, alpha } });
	verts.push_back(Graphics::Mesh::Vertex{ { -0.5f, 0.5f, -0.5f },{ 0, 1, 0, alpha } });
	verts.push_back(Graphics::Mesh::Vertex{ { -0.5f, -0.5f, 0.5f },{ 0, 0, 1, alpha } });
	verts.push_back(Graphics::Mesh::Vertex{ { -0.5f, -0.5f, -0.5f },{ 0, 0, 0, alpha } });

	unsigned short indexes[] = { 
		0, 4, 2, //back
		4, 6, 2,

		0, 3, 1,//right
		0, 2, 3,

		2, 6, 3,//bottom
		6, 7, 3,

		5, 6, 4,//left
		5, 7, 6,

		5, 4, 0,//top
		5, 0, 1,

		3, 5, 1,//front
		7, 5, 3 };

	indices.resize(_ARRAYSIZE(indexes));

	memcpy(&indices[0], indexes, sizeof(indexes));
}

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
	bool run = true;
	Graphics::Window win(L"Hello, World");
	Graphics::GraphicsEngine eng(win);

	Graphics::Shader ps(eng, "LambertPixel.cso", Graphics::ShaderType::Pixel);
	Graphics::Shader vs(eng, "LambertVertex.cso", Graphics::ShaderType::Vertex);
	Graphics::Camera cam(eng);

	Graphics::ConstantBuffer<DirectX::XMMATRIX> trans1(eng, Graphics::ShaderType::Vertex, 1);
	Graphics::ConstantBuffer<DirectX::XMMATRIX> trans2(eng, Graphics::ShaderType::Vertex, 1);
	Graphics::ConstantBuffer<DirectX::XMMATRIX> trans3(eng, Graphics::ShaderType::Vertex, 1);

	trans1.GetDataForWrite() = DirectX::XMMatrixIdentity();
	trans2.GetDataForWrite() = DirectX::XMMatrixAffineTransformation({ 1,1,1 }, { 0,0,0 }, { 0,0,0,0 }, { 1,0,0 });
	trans3.GetDataForWrite() = DirectX::XMMatrixAffineTransformation({ 10,0.5,10 }, { 0,0,0 }, { 0,0,0,0 }, { 0,-1,0 });

	if (!ps.Create())
		run = false;

	if (!vs.Create())
		run = false;

	std::vector<Graphics::Mesh::Vertex> verts;
	std::vector<unsigned short> indices;
	CreateCube(verts, indices);
	

	Graphics::Mesh cube(eng, verts, indices);

	verts.clear();
	verts.shrink_to_fit();
	indices.clear();
	indices.shrink_to_fit();

	Graphics::Mesh cube3 = cube.Copy();
	cube3.SetMutable(false);
	cube3.Create(vs);
	cube.Create(vs);
	Graphics::Mesh cube2 = cube.Copy();

	eng.VSync = Graphics::GraphicsEngine::VSyncType::On;

	while (run)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				run = false;
			cam.UpdateSize();
		}
		eng.ClearScreen(Graphics::Color(0,0,0));
		static float wow = 0;
		static float wow2 = 0;
		wow += 0.01f;
		wow2 += 0.05f;

		if (wow > 1)
			cube.SetMutable(true);

		static int wowzer = 0;

		if (wowzer++ > 30)
		{
			cube2 = cube.Copy();
			wowzer = 0;
		}

		cam.SetPosition(Graphics::Vector4(2 * sin(wow), 0.5f, 2 * cos(wow), 1));
		cam.SetLook(-cam.GetPosition());

		size_t ind = rand() % cube.GetVertices().size();
		Graphics::Mesh::Vertex vert = cube.GetVertices()[ind];
		for (int i = 0; i < 3; ++i)
			vert.Position[i] = static_cast<float>(static_cast<double>(rand()) / RAND_MAX - 0.5);
		cube.SetVertex(ind, vert);

		//always set because you don't know if the device changed
		vs.Use();
		ps.Use();
		cam.Use();

		trans3.Use();
		eng.Draw(cube3);
		trans1.Use();
		eng.Draw(cube);
		trans2.Use();
		eng.Draw(cube2);

		eng.Present();
	}
	return 0;
}