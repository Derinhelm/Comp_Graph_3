//internal includes
#include "common.h"
#include "ShaderProgram.h"
#include <unistd.h>


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
GLuint g_vertexEBO;
float min1 = 100000, max1 = -100000; ///////////////////////////////////////////////////////////////////////убрать


struct Vert {
	glm::vec3 c; //координаты вершины
	glm::vec3 n; //нормаль
	Vert(float v1, float v2, float v3, float n1, float n2, float n3) {
		c = glm::vec3(v1, v2, v3);
		n = glm::vec3(n1, n2, n3);
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
		std:: cout << scene -> mNumMaterials << "\n";
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
			// каждого меша еще надо сделать материалы, но потом

			}  
		}

}
void load (std::vector <Vert> & verts, std::vector <unsigned int> & indices) {
    g_vertexBufferObject = 0;
    GLuint vertexLocation = 0; // simple layout, assume have only positions at location = 0

   /*Л. везде дальше GL_CHECK_ERRORS - полезный макрос для проверки ошибок в строчке, где он был записан  */


/*Л.   Следующие три строки работаем с Vertex Buffer Object (VBO) —  средством OpenGL, позволяющее загружать определенные данные в память GPU
 https://eax.me/opengl-vbo-vao-shaders/*/   


    glGenBuffers(1, &g_vertexBufferObject);                                                        GL_CHECK_ERRORS;
		// Копируем массив с вершинами в буфер OpenGL

		glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);             
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vert), &verts[0], GL_STATIC_DRAW);                                GL_CHECK_ERRORS;
	
	

/*Еще 2 строчек -  Vertex Arrays Object (VAO) — штука, которая говорит OpenGL, какую часть VBO следует использовать в последующих командах. 
 Представьте, что VAO представляет собой массив, в элементах которого хранится информация о том, какую часть некого VBO использовать, 
 и как эти данные нужно интерпретировать. Таким образом, один VAO по разным индексам может хранить координаты вершин, их цвета, нормали и прочие данные.
 Переключившись на нужный VAO мы можем эффективно обращаться к данным, на которые он «указывает», используя только индексы.
 https://eax.me/opengl-vbo-vao-shaders/*/   
    glGenVertexArrays(1, &g_vertexArrayObject);                                                    GL_CHECK_ERRORS;
    // Привязываем VAO
		glBindVertexArray(g_vertexArrayObject);                                                        GL_CHECK_ERRORS;


//очень возможно, следующая строка не нужна
    glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);                                           GL_CHECK_ERRORS;

	 //Устанавливаем указатели на вершинные атрибуты  
		glEnableVertexAttribArray(0);  /*здесь vertexlocation = 0*/                     //  GL_CHECK_ERRORS;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)0);
	//	glEnableVertexAttribArray(1);  /*здесь vertexlocation = 0*/                     //  GL_CHECK_ERRORS;
   // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, n));
// Отвязываем VAO
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
		std::vector <Material> maters, float * lightPos, float * camPos
  /*, float ** col*/) {
			    /* Л. glfwPollEvents  обрабатывает только те события, которые уже находятся в очереди событий, а затем сразу же возвращается. 
    Обработка событий вызовет окно и входные обратные вызовы, связанные с этими событиями. */
		glfwPollEvents();



/* Л. program.StartUseShader() запускаем шейдеры*/
    program.StartUseShader();                           GL_CHECK_ERRORS;


    // очистка и заполнение экрана цветом
    //
		    /* Л.  glViewport определяет аффинное преобразование координат x и y из нормализованных координат устройства в координаты окна.  
    https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glViewport.xml*/
    glViewport  (0, 0, WIDTH, HEIGHT);
		glEnable(GL_DEPTH_TEST);  		    /*Л. следующие две строчки - см.ранее*/
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // draw call
    //
		    /*Л. рисуем примитивы, которые загрузили ранее
      https://eax.me/opengl-vbo-vao-shaders/*/   


    glBindVertexArray(g_vertexArrayObject); GL_CHECK_ERRORS;

		GLint lightPosLoc = glGetUniformLocation(program.GetProgram(), "g_lightPos");
		glUniform3f(lightPosLoc, lightPos[0], lightPos[1], lightPos[2]); 
		
		GLint camPosLoc = glGetUniformLocation(program.GetProgram(), "g_camPos");
		glUniform3f(camPosLoc, camPos[0], camPos[1], camPos[2]); 


		GLuint transformLoc1 = glGetUniformLocation(program.GetProgram(), "g_matrixView");   GL_CHECK_ERRORS;
	//	std::cout << "view" << transformLoc1 << "\n";
		glUniformMatrix4fv(transformLoc1, 1, GL_FALSE, glm::value_ptr(view)); GL_CHECK_ERRORS;

		GLuint transformLoc3 = glGetUniformLocation(program.GetProgram(), "g_matrixProj");   GL_CHECK_ERRORS;
	//	std::cout << "proj" << transformLoc3 << "\n";
		glUniformMatrix4fv(transformLoc3, 1, GL_FALSE, glm::value_ptr(proj)); GL_CHECK_ERRORS;



		GLuint transformLoc2 = glGetUniformLocation(program.GetProgram(), "g_matrixScale");   GL_CHECK_ERRORS;
	//	std::cout << "trans" << transformLoc2 << "\n";
		glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(trans1)); GL_CHECK_ERRORS;


		loadMaterial(program.GetProgram(), maters[0]);
	
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


		
    glDrawArrays(GL_TRIANGLE_FAN, len1 + len2, 4);
		

    glBindVertexArray(0);

    program.StopUseShader();

//Л. glfwSwapBuffers меняет местами передний и задний буферы указанного окна. 
//Если интервал подкачки больше нуля, драйвер GPU ожидает указанное количество обновлений экрана перед заменой буферов.
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
	//грузим функции opengl через glad
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
	 /* glfwInit инициализирует библиотеку GLFW. Перед использованием большинства функций GLFW необходимо инициализировать GLFW,
   а перед завершением работы приложения необходимо завершить GLFW, чтобы освободить ресурсы, выделенные во время или после инициализации. */
	if(!glfwInit())
    return -1;

	//запрашиваем контекст opengl версии 3.3
	//делаем начальные настройки Л.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

  GLFWwindow*  window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);
  //Эта функция создает окно и связанный с ним контекст OpenGL или OpenGL ES.
	// Большинство параметров, управляющих созданием окна и его контекста, задаются с помощью подсказок окна(window hints). Л.

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	/* Л. glfwMakeContextCurrent делает контекст OpenGL или OpenGL ES указанного окна текущим в вызывающем потоке. 
Контекст можно сделать текущим только в одном потоке за один раз, и каждый поток может иметь только один текущий контекст за один раз. */
	glfwMakeContextCurrent(window); 


	/* Л.
glfwSetInputMode устанавливает режим ввода для указанного окна. Режим должен быть одним из GLFW_CURSOR, GLFW_STICKY_KEYS или GLFW_STICKY_MOUSE_BUTTONS.
*/

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

//функция, см. выше Л.
	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	//создание шейдерной программы из двух файлов с исходниками шейдеров
	//используется класс-обертка ShaderProgram
	std::unordered_map<GLenum, std::string> shaders;
	shaders[GL_VERTEX_SHADER]   = "vertex.glsl";
	shaders[GL_FRAGMENT_SHADER] = "fragment.glsl";
	ShaderProgram program(shaders); GL_CHECK_ERRORS;



// Л. glfwSwapInterval устанавливает интервал подкачки для текущего контекста OpenGL или OpenGL ES,
// т. е. количество обновлений экрана, ожидающих с момента вызова glfwSwapBuffers до подкачки буферов и возврата.
  glfwSwapInterval(1); // force 60 frames per second

	
	std::vector <Vert> verts1, verts2, vertsQuad, resVerts1, resVerts;
  std::vector <unsigned int> indices1, indices2, resInd;
	std::vector <Material> maters;
	float lightPos[] = {0.2f, (0.341855 + 1.0813) * 0.5 + 0.05, 0.0f};
	float camPos[] = {0.0f, 0.8, 2.8f};//1.8, 2.8};////0.9f, 2.8f};

	maters.push_back(Material(1.0f, 0.0f, 0.0f,  0.1,0.18725, 0.1745,  0.396, 0.74151, 0.69102,  0.297254, 0.30829, 0.306678, 0.1 * 128));
	maters.push_back(Material(0.0f, 1.0f, 0.0f,  0.1,0.18725, 0.1745,  0.396, 0.74151, 0.69102,  0.297254, 0.30829, 0.306678, 0.1 * 128));
	maters.push_back(Material(0.0f, 0.0f, 1.0f,  0.1,0.18725, 0.1745,  0.396, 0.74151, 0.69102,  0.297254, 0.30829, 0.306678, 0.1 * 128));

	glm::mat4 view(1.0f), project(1.0f);
	glm::mat4 trans1(1.0f);  GL_CHECK_ERRORS;
	float shiftVase = (0.341855 + 1.0813) * 0.5 + 0.937952 * 0.2;
	glm::mat4 b1(1.0f), b2(1.0f);
	
	trans1 = glm::translate(b1, glm::vec3(0.0f, shiftVase, 0.0f)) * glm::scale(b2, glm::vec3(0.1, 0.2, 0.2));
	view = glm::translate(view, glm::vec3(-camPos[0], -camPos[1], -camPos[2]));
	float aspect = WIDTH / HEIGHT;
	project = glm::perspective( 45.0f, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
	loadScene("/media/derin/DATA/Computer_Graph/3/prob1/objects/vase.obj", verts1, indices1); GL_CHECK_ERRORS
	//минимальныая координата вазы -0.937952, максимальная 1.78299

	std::cout << min1 << "\n" << max1 << "\n";
	std::cout << "---------------------------------" << "\n";

	glm::mat4 trans2(1.0f);  GL_CHECK_ERRORS;
	
	glm::mat4 a1(1.0f), a2(1.0f);
	trans2 = glm::translate(a1, glm::vec3(0.0f, 0.341855 * 0.5, 0.0f)) * glm::scale(a2, glm::vec3(0.3, 0.5, 0.5));

	loadScene("/media/derin/DATA/Computer_Graph/3/prob1/objects/table_simple.obj", verts2, indices2); GL_CHECK_ERRORS;
		//минимальныая координата стола -0.341855, максимальная 1.0813
	std::cout << min1 << "\n" << max1 << "\n";
	std::cout << "---------------------------------" << "\n";

	vertsQuad.push_back(Vert(-100, 100, 0, 0, 0, 1));
	vertsQuad.push_back(Vert(-100, -100, 0, 0, 0, 1));
	vertsQuad.push_back(Vert(100, -100, 0, 0, 0, 1));
	vertsQuad.push_back(Vert(100, 100, 0, 0, 0, 1));

	glm::mat4 trans3(1.0f);  GL_CHECK_ERRORS;
	trans3 = glm::rotate(trans3, 90.0f, glm::vec3(1.0, 0.0, 0.0));  GL_CHECK_ERRORS;
//	trans3 = glm::translate(trans3, glm::vec3(0.0f, 0.0f, 0.0f)); // -10

	vectorMerge(verts1, verts2, resVerts1);
	vectorMerge(resVerts1, vertsQuad, resVerts);
	vectorMerge(indices1, indices2, resInd);

	load(resVerts, resInd);
	float t = 0;

	double ms_per_update = 0.09f;//0.115f;
	while (!glfwWindowShouldClose(window))
	{
		time_t begin = time(0);  	
		
		trans1 = glm::translate(b1, glm::vec3(0.6f * sin(t), shiftVase, 0.0f)) * glm::scale(b2, glm::vec3(0.1, 0.2, 0.2));
		//trans1 = glm::translate(scale1, glm::vec3(1.0f * sin(t), 0.3f, -2.0f));
		t += 0.03f;
		draw(program, window, verts1.size(), verts2.size(), indices1, indices2, view, project,
				trans1, trans2, trans3, 
				maters, lightPos, camPos); GL_CHECK_ERRORS
		sleep(ms_per_update - difftime(time(0), begin));
	}

	//очищаем vbo и vao перед закрытием программы
  //
	glDeleteVertexArrays(1, &g_vertexArrayObject);
  glDeleteBuffers(1, &g_vertexBufferObject);

	glfwTerminate();
	return 0;
}
