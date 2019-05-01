//internal includes
#include "common.h"
#include "ShaderProgram.h"
#include <unistd.h>

#include <SOIL/SOIL.h>
//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <random>
#include <ctime>

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


static const GLsizei WIDTH = 1400, HEIGHT = 1000; //размеры окна

//Создание переменной для хранения идентификатора VBO(см.далее)
GLuint g_vertexBufferObject;

	//Создание переменной для хранения идентификатора VAO(см.далее)
GLuint g_vertexArrayObject;
GLuint texture;	
float min1 = 100000, max1 = -100000; ///////////////////////////////////////////////////////////////////////убрать


struct Vert {
	glm::vec3 c; //координаты вершины
	glm::vec3 n; //нормаль
	glm::vec2 t;	//текстурные координаты(0, 0, так как у стола и вазы нет текстур)
	Vert(float v1, float v2, float v3, float n1, float n2, float n3, float t1 = 0.0, float t2 = 0.0) {
		c = glm::vec3(v1, v2, v3);
		n = glm::vec3(n1, n2, n3);
		t = glm::vec2(t1, t2);
	}
};

struct Material {
		float color[3];
		float diff[3];
		float ambient[3];
		float specular[3];
		float shininess;
		Material(float c1, float c2, float c3, float d1, float d2, float d3, float a1, float a2, float a3,
					float s1, float s2, float s3, float sh) {
			color[0] = c1;
			color[1] = c2;
			color[2] = c3;
			diff[0] = d1;
			diff[1] = d2;
			diff[2] = d3;
			ambient[0] = a1;
			ambient[1] = a2;
			ambient[2] = a3;
			specular[0] = s1;
			specular[1] = s2;
			specular[2] = s3;
			shininess = sh;
		}
};


void loadScene (std::string path, std::vector <Vert> & verts, std::vector <unsigned int> & indices)
//Создаем и загружаем геометрию поверхности
  //
{
		Assimp::Importer import;
		const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate);	
		//У vase - 1 потомок с 1 мешем, у table - 1 потомок с двумя мешами

		//У vase - 1 потомок с 1 мешем, у table - 1 потомок с двумя мешами
		//Сразу перейдем к потомку
		aiNode * root = scene -> mRootNode; 
		aiNode * child = root -> mChildren[0]; // только один потомок
		min1 = 100000, max1 = -100000;
		for(unsigned int i = 0; i < child-> mNumMeshes; i++)  {
			aiMesh *mesh = scene->mMeshes[child -> mMeshes[i]];
			for (int j = 0; j < mesh->mNumVertices; j++) {
				Vert curtVert(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
				verts.push_back(curtVert);		
				if (curtVert.c[1] < min1) {
					min1 = curtVert.c[1];
				}
				if (curtVert.c[1] > max1) {
					max1 = curtVert.c[1];
				}
			}
			for(unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for(unsigned int j = 0; j < face.mNumIndices; j++){
					indices.push_back(face.mIndices[j]);
				}

			}
		}

}
void load (std::vector <Vert> & verts, std::vector <unsigned int> & indices, const char * pict) {
		glGenTextures(1, &texture);	GL_CHECK_ERRORS;
		glBindTexture(GL_TEXTURE_2D, texture);	GL_CHECK_ERRORS;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	GL_CHECK_ERRORS;// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	GL_CHECK_ERRORS;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	GL_CHECK_ERRORS;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	GL_CHECK_ERRORS;

		int width, height;
		unsigned char* image = SOIL_load_image(pict, &width, &height, 0, SOIL_LOAD_RGB);	GL_CHECK_ERRORS;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);	GL_CHECK_ERRORS;
		glGenerateMipmap(GL_TEXTURE_2D);	GL_CHECK_ERRORS;
		SOIL_free_image_data(image);	GL_CHECK_ERRORS;
		glBindTexture(GL_TEXTURE_2D, 0);	GL_CHECK_ERRORS;

		g_vertexBufferObject = 0;
		GLuint vertexLocation = 0; // simple layout, assume have only positions at location = 0




		glGenBuffers(1, &g_vertexBufferObject);                                                        GL_CHECK_ERRORS;


		glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);             
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vert), &verts[0], GL_STATIC_DRAW);                                GL_CHECK_ERRORS;
	
	

		glGenVertexArrays(1, &g_vertexArrayObject);                                                    GL_CHECK_ERRORS;
		glBindVertexArray(g_vertexArrayObject);                                                        GL_CHECK_ERRORS;


		glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);                                           GL_CHECK_ERRORS;


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);
		
}

void loadMaterial(GLuint progrNum, Material mat) {
		GLint lightPosLoc = glGetUniformLocation(progrNum, "g_objectColor");
		glUniform3f(lightPosLoc, mat.color[0], mat.color[1], mat.color[2]); 
		GLint diffPosLoc = glGetUniformLocation(progrNum, "g_diffuseStrength");
		glUniform3f(diffPosLoc, mat.diff[0], mat.diff[1], mat.diff[2]); 
		GLint ambPosLoc = glGetUniformLocation(progrNum, "g_ambient");
		glUniform3f(ambPosLoc, mat.ambient[0], mat.ambient[1], mat.ambient[2]); 
		GLint specPosLoc = glGetUniformLocation(progrNum, "g_specularStrength");
		glUniform3f(specPosLoc, mat.specular[0], mat.specular[1], mat.specular[2]); 
		GLint shinPosLoc = glGetUniformLocation(progrNum, "g_shininess");
		glUniform1f(shinPosLoc, mat.shininess); 

}

void draw(ShaderProgram program, GLFWwindow*  window, unsigned int len1, unsigned int len2, 
		std::vector <unsigned int> ind1, std::vector <unsigned int> ind2, glm::mat4 view, glm::mat4 proj,
		glm::mat4 trans1,	glm::mat4 trans2, glm::mat4 trans3, 
		std::vector <Material> maters, float * lightPos, float * camPos) {
		glfwPollEvents();

		program.StartUseShader();                           GL_CHECK_ERRORS;


		glViewport  (0, 0, WIDTH, HEIGHT);
		glEnable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);




		glBindVertexArray(g_vertexArrayObject); GL_CHECK_ERRORS;

		GLint lightPosLoc = glGetUniformLocation(program.GetProgram(), "g_lightPos");
		glUniform3f(lightPosLoc, lightPos[0], lightPos[1], lightPos[2]); 
		
		GLint camPosLoc = glGetUniformLocation(program.GetProgram(), "g_camPos");
		glUniform3f(camPosLoc, camPos[0], camPos[1], camPos[2]); 


		GLuint transformLoc1 = glGetUniformLocation(program.GetProgram(), "g_matrixView");   GL_CHECK_ERRORS;
		glUniformMatrix4fv(transformLoc1, 1, GL_FALSE, glm::value_ptr(view)); GL_CHECK_ERRORS;

		GLuint transformLoc3 = glGetUniformLocation(program.GetProgram(), "g_matrixProj");   GL_CHECK_ERRORS;
		glUniformMatrix4fv(transformLoc3, 1, GL_FALSE, glm::value_ptr(proj)); GL_CHECK_ERRORS;



		GLuint transformLoc2 = glGetUniformLocation(program.GetProgram(), "g_matrixScale");   GL_CHECK_ERRORS;
		glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(trans1)); GL_CHECK_ERRORS;

		glBindTexture(GL_TEXTURE_2D, texture);	GL_CHECK_ERRORS;

		loadMaterial(program.GetProgram(), maters[0]);
	
		GLuint hasTextureLoc2 = glGetUniformLocation(program.GetProgram(), "hasTexture");   GL_CHECK_ERRORS;
		glUniform1i(hasTextureLoc2, 0); GL_CHECK_ERRORS;
		unsigned int points[3] = {0, 1, 2};
		
		for (int i = 0; 3 * i + 2 < ind1.size(); i++) {
			for (int j = 0; j < 3; j++) {
				points[j] = ind1[3 * i + j];
			}
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, points);
		}


		transformLoc2 = glGetUniformLocation(program.GetProgram(), "g_matrixScale");   GL_CHECK_ERRORS;
		glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(trans2)); GL_CHECK_ERRORS;

		loadMaterial(program.GetProgram(), maters[1]);


		for (int i = 0; 3 * i + 2 < ind2.size(); i++) {
			for (int j = 0; j < 3; j++) {
				points[j] = len1 + ind2[3 * i + j];
			}
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, points);
		}

		transformLoc2 = glGetUniformLocation(program.GetProgram(), "g_matrixScale");   GL_CHECK_ERRORS;
		glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(trans3)); GL_CHECK_ERRORS;

		loadMaterial(program.GetProgram(), maters[2]);

		hasTextureLoc2 = glGetUniformLocation(program.GetProgram(), "hasTexture");   GL_CHECK_ERRORS;
		glUniform1i(hasTextureLoc2, 1); GL_CHECK_ERRORS;
		
		glDrawArrays(GL_TRIANGLE_FAN, len1 + len2, 4);
		

		glBindVertexArray(0);

		program.StopUseShader();

		glfwSwapBuffers(window); 

}

template<typename T>
void vectorMerge(std::vector <T> &v1, std::vector<T> &v2, std::vector<T> &res) {
	for (auto i = v1.begin(); i < v1.end(); i++) {
		res.push_back(*i);
	}

	for (auto i = v2.begin(); i < v2.end(); i++) {
		res.push_back(*i);
	}
}

int initGL()
{
		int res = 0;
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize OpenGL context" << std::endl;
			return -1;
		}

		std::cout << "Vendor: "   << glGetString(GL_VENDOR) << std::endl;
		std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "Version: "  << glGetString(GL_VERSION) << std::endl;
		std::cout << "GLSL: "     << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		return 0;
}


int main(int argc, char** argv)
{
		if(!glfwInit())
			return -1;

	//запрашиваем контекст opengl версии 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

		GLFWwindow*  window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);

		if (window == nullptr)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return -1;
		}
	
		glfwMakeContextCurrent(window); 



		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


		if(initGL() != 0) 
			return -1;
	
  //Reset any OpenGL errors which could be present for some reason
		GLenum gl_error = glGetError();
		while (gl_error != GL_NO_ERROR)
			gl_error = glGetError();


		std::unordered_map<GLenum, std::string> shaders;
		shaders[GL_VERTEX_SHADER]   = "vertex.glsl";
		shaders[GL_FRAGMENT_SHADER] = "fragment.glsl";
		ShaderProgram program(shaders); GL_CHECK_ERRORS;



		glfwSwapInterval(1); // force 60 frames per second

	
		std::vector <Vert> verts1, verts2, vertsQuad, resVerts1, resVerts;
		std::vector <unsigned int> indices1, indices2, resInd;
		std::vector <Material> maters;
		float lightPos[] = {0.3f, (0.341855 + 1.0813) * 0.5 + 0.4, 0.3f};
		float camPos[] = {0.0f, 1.4, 3.0f};//1.8, 2.8};////0.9f, 2.8f};

		maters.push_back(Material(1.0f, 0.0f, 0.0f, 0.9, 0.0, 0.0, 0.5, 0.4, 0.4, 0.7, 0.04, 0.04, 0.078125 * 128));
		maters.push_back(Material(0.7f, 0.5f, 0.0f, 0.25,	0.20725, 0.20725,	1, 0.829, 0.829, 0.296648, 0.296648,0.296648, 0.088 * 128));
		maters.push_back(Material(1.0f, 1.0f, 1.0f,  0.1,0.18725, 0.1745,  0.396, 0.74151, 0.69102,  0.297254, 0.30829, 0.306678, 0.1 * 128));

		glm::mat4 view(1.0f), project(1.0f);
		glm::mat4 trans1(1.0f);  GL_CHECK_ERRORS;
		float shiftVase = (0.341855 + 1.0813) * 0.5 + 0.937952 * 0.2;
		glm::mat4 b1(1.0f), b2(1.0f);
	
		trans1 = glm::translate(b1, glm::vec3(0.0f, shiftVase, 0.0f)) * glm::scale(b2, glm::vec3(0.2, 0.3, 0.3));
		view = glm::translate(view, glm::vec3(-camPos[0], -camPos[1], -camPos[2]));
		float aspect = WIDTH / HEIGHT;
		project = glm::perspective( 45.0f, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
		loadScene("../objects/vase.obj", verts1, indices1); GL_CHECK_ERRORS
		//минимальныая координата вазы -0.937952, максимальная 1.78299


		glm::mat4 trans2(1.0f);  GL_CHECK_ERRORS;
	
		glm::mat4 a1(1.0f), a2(1.0f);
		trans2 = glm::translate(a1, glm::vec3(0.0f, 0.341855 * 0.5, 0.0f)) * glm::scale(a2, glm::vec3(0.3, 0.5, 0.5));

		loadScene("../objects/table_simple.obj", verts2, indices2); GL_CHECK_ERRORS;
		//минимальныая координата стола -0.341855, максимальная 1.0813

		vertsQuad.push_back(Vert(-2, 2, 0, 0, 0, 1, 0.0, 0.0));
		vertsQuad.push_back(Vert(-2, -2, 0, 0, 0, 1, 0.0, 10.0));
		vertsQuad.push_back(Vert(2, -2, 0, 0, 0, 1, 10.0, 10.0));
		vertsQuad.push_back(Vert(2, 2, 0, 0, 0, 1, 10.0, 0.0));

		glm::mat4 trans3(1.0f);  GL_CHECK_ERRORS;
		trans3 = glm::rotate(trans3, 90.0f, glm::vec3(1.0, 0.0, 0.0));  GL_CHECK_ERRORS;

		vectorMerge(verts1, verts2, resVerts1);
		vectorMerge(resVerts1, vertsQuad, resVerts);
		vectorMerge(indices1, indices2, resInd);



		load(resVerts, resInd, "../floor3.jpg");
		float t = 0;

		double ms_per_update = 0.09f;//0.115f;
		while (!glfwWindowShouldClose(window))
		{
			time_t begin = time(0);  	
		
			trans1 = glm::translate(b1, glm::vec3(0.6f * sin(t), shiftVase, 0.0f)) * glm::scale(b2, glm::vec3(0.1, 0.2, 0.2));
			t += 0.03f;
			draw(program, window, verts1.size(), verts2.size(), indices1, indices2, view, project,
				trans1, trans2, trans3, 
				maters, lightPos, camPos); GL_CHECK_ERRORS
			sleep(ms_per_update - difftime(time(0), begin));
		}

	//очищаем vbo и vao перед закрытием программы
		glDeleteVertexArrays(1, &g_vertexArrayObject);
		glDeleteBuffers(1, &g_vertexBufferObject);

		glfwTerminate();
		return 0;
}

