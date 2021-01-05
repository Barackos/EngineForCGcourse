#define GLEW_STATIC
#include "GL/glew.h"
#include "scene.h"
#include <iostream>

static void printMat(const glm::mat4 mat)
{
	printf(" matrix: \n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			printf("%f ", mat[j][i]);
		printf("\n");
	}
}

Scene::Scene()
{
	glLineWidth(5);

	pickedShape = -1;
	depth = 0;

	isActive = true;
	isPicking = false;
}

void Scene::AddShapeFromFile(const std::string& fileName, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(fileName, mode));
}

void Scene::AddShape(int type, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(type, mode));
}

void Scene::AddShapeCopy(int indx, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(*shapes[indx], mode));
}

int Scene::AddShader(const std::string& fileName)
{
	shaders.push_back(new Shader(fileName));
	return (shaders.size() - 1);
}

int  Scene::AddTexture(const std::string& textureFileName, int dim)
{
	textures.push_back(new Texture(textureFileName, dim));
	return(textures.size() - 1);
}

int Scene::AddTexture(int width, int height, unsigned char* data, int mode)
{
	textures.push_back(new Texture(width, height));

	switch (mode)
	{
	case COLOR:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save memory.
		break;
	case DEPTH:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
		break;
	case STENCIL:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, data);
		break;
	default:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save memory.
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return(textures.size() - 1);
}

int Scene::AddMaterial(unsigned int texIndices[], unsigned int slots[], unsigned int size)
{
	materials.push_back(new Material(texIndices, slots, size));
	return (materials.size() - 1);
}

void Scene::Draw(int shaderIndx, const glm::mat4& MVP, int viewportIndx, Camera *c, unsigned int flags) 
{
	glm::mat4 Normal = MakeTrans();
	glm::mat4 MVPNT = c->GetViewProjection() * glm::inverse(glm::mat4(glm::mat3(c->MakeTrans())));

	int p = pickedShape;

	for (pickedShape = 0; pickedShape < shapes.size(); pickedShape++)
	{
		if (shapes[pickedShape]->Is2Render(viewportIndx))
		{
			glm::mat4 Model = Normal * shapes[pickedShape]->MakeTrans();
			if(pickedShape < 3) glDepthMask(GL_FALSE);
			if (shaderIndx > 0)
			{
				Update(pickedShape == 0 ? MVPNT : MVP, Model, shapes[pickedShape]->GetShader());
				BeforeDraw(1);
				shapes[pickedShape]->Draw(shaders[shapes[pickedShape]->GetShader()], false);
				AfterDraw(MVP);
			}
			else
			{ //picking
				Update(MVP, Model, 0);
				shapes[pickedShape]->Draw(shaders[0], true);
			}
			glDepthMask(GL_TRUE);
		}
	}
	pickedShape = p;
}

void Scene::ShapeTransformation(int type, float amt)
{
	if (glm::abs(amt) > 1e-5)
	{
		switch (type)
		{
		case xTranslate:
			shapes[pickedShape]->MyTranslate(glm::vec3(amt, 0, 0), 0);
			break;
		case yTranslate:
			shapes[pickedShape]->MyTranslate(glm::vec3(0, amt, 0), 0);
			break;
		case zTranslate:
			shapes[pickedShape]->MyTranslate(glm::vec3(0, 0, amt), 0);
			break;
		case xRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(1, 0, 0), 0);
			break;
		case yRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(0, 1, 0), 0);
			break;
		case zRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(0, 0, 1), 0);
			break;
		default:
			break;
		}
	}

}

bool Scene::Picking(unsigned char data[4])
{
		pickedShape = data[3]-1; //r 
		WhenPicked();
		return isPicking;
}
//return coordinates in global system for a tip of arm position is local system 
void Scene::MouseProccessing(int button, int xrel, int yrel)
{
	if(isPicking && pickedShape != -1){ 
		this->xrel = -xrel;
		this->yrel = yrel;
		if(button == 1)
			WhenTranslate();
		else
			WhenRotate();
	}
}

void Scene::ZeroShapesTrans()
{
	for (unsigned int i = 0; i < shapes.size(); i++)
	{
		shapes[i]->ZeroTrans();
	}
}

void Scene::ReadPixel()
{
	glReadPixels(1, 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
}

void Scene::AddShapeViewport(int shpIndx,int viewPortIndx)
{
	shapes[shpIndx]->AddViewport(viewPortIndx);
}

void Scene::RemoveShapeViewport(int shpIndx, int viewPortIndx)
{
	shapes[shpIndx]->RemoveViewport(viewPortIndx);
}

void Scene::BindMaterial(Shader* s, unsigned int materialIndx)
{

	for (size_t i = 0; i < materials[materialIndx]->GetNumOfTexs(); i++)
	{
		materials[materialIndx]->Bind(textures, i);
		s->SetUniform1i("sampler" + std::to_string(i + 1), materials[materialIndx]->GetSlot(i));
	}
}

Scene::~Scene(void)
{
	for (Shape* shp : shapes)
	{
		delete shp;
	}

	for (Shader* sdr : shaders)
	{
		delete sdr;
	}
	for (Texture* tex : textures)
	{
		delete tex;
	}
	for (Material* mat : materials)
	{
		delete mat;
	}

}



