#include "game.h"
#include <iostream>
#include "GL/glew.h"

static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

Game::Game() : Scene()
{
}

//Game::Game(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Game::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0};
	unsigned int slots[3] = { 0 , 1, 0 };
	
	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/basicShader2");
	//AddShader("../res/shaders/basicShader");
	
	AddTexture("../res/textures/box0.bmp",2);
	//AddTexture("../res/textures/grass.bmp", 2);
	TextureDesine(800, 800);

	AddMaterial(texIDs,slots, 2);
	AddMaterial(texIDs+1,slots+1, 2);
	//AddShape(Cube, -1, TRIANGLES);
	for(int i = 0; i < 27; i++){
        AddShape(Cube, -1, TRIANGLES);
        SetShapeShader(i, 1);
        pickedShape = i;
        SetShapeMaterial(i, 0);
        glm::vec3 walls = glm::vec3(i % 3 - 1, (i % 9) / 3 - 1, i / 9 - 1);
        ShapeTransformation(xTranslate, -2*(walls.x));
        ShapeTransformation(yTranslate, -2*(walls.y));
        ShapeTransformation(zTranslate, -2*(walls.z));
    }
	//AddShape(Cube, -1, TRIANGLES);
	//AddShape(Plane, -1, TRIANGLES);
	//AddShapeViewport(2, 1);
	//RemoveShapeViewport(2, 0);
	//SetShapeShader(2, 2);

	pickedShape = 0;
	//SetShapeMaterial(0, 1);
	//ShapeTransformation(xTranslate, -1);

	pickedShape = 1;
	//ShapeTransformation(xTranslate, 1);
	//SetShapeMaterial(1, 1);
	pickedShape = -1;

	pickedShape = 2;
	//SetShapeMaterial(2, 2);
	pickedShape = -1;
	
	//ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Game::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	//textures[0]->Bind(0);
	s->Bind();
	if (shaderIndx != 2)
	{
		s->SetUniformMat4f("MVP", MVP);
		s->SetUniformMat4f("View", MVP);
		s->SetUniformMat4f("Normal", Model);
		s->SetUniformMat4f("Model", Model);
	}
	else
	{
		s->SetUniformMat4f("MVP", glm::mat4(1));
		s->SetUniformMat4f("Normal", glm::mat4(1));
	}
	s->SetUniform1i("sampler", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	if(shaderIndx!=2)
		s->SetUniform1i("sampler", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));

	s->Unbind();
}

void Game::WhenRotate()
{
}

void Game::WhenTranslate()
{
}

void Game::Motion()
{
	if(isActive)
	{
	}
}

unsigned int Game::TextureDesine(int width, int height)
{
	unsigned char* data = new unsigned char[width * height * 4];
	for (size_t i = 0; i < width; i++)
	{
		for (size_t j = 0; j < height; j++)
		{
			data[(i * height + j) * 4] = (i + j) % 256;
			data[(i * height + j) * 4 + 1] = (i + j * 2) % 256;
			data[(i * height + j) * 4 + 2] = (i * 2 + j) % 256;
			data[(i * height + j) * 4 + 3] = (i * 3 + j) % 256;
		}
	}
	textures.push_back(new Texture(width, height));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save 
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
	return(textures.size() - 1);
}

Game::~Game(void)
{

}
