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


struct Vert {
	glm::vec3 c; //координаты вершины
	glm::vec3 n; //нормаль
	public: 
	Vert(float v1, float v2, float v3, float n1, float n2, float n3) {
		c = glm::vec3(v1, v2, v3);
		n = glm::vec3(n1, n2, n3);
	}
};


void fun1(const aiScene * scene, std::vector <Vert> & v, std::vector <unsigned int> & ind) {
		//У vase - 1 потомок с 1 мешем, у table - 1 потомок с двумя мешами
		//Сразу перейдем к потомку
		aiNode * root = scene -> mRootNode; 
		aiNode * child = root -> mChildren[0]; // только один потомок

		for(unsigned int i = 0; i < child-> mNumMeshes; i++)  {
			aiMesh *mesh = scene->mMeshes[child -> mMeshes[i]];
			for (int j = 0; j < mesh->mNumVertices; j++) {
				Vert curtVert(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
				v.push_back(curtVert);		
			}
			for(unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
    		aiFace face = mesh->mFaces[i];
    		for(unsigned int j = 0; j < face.mNumIndices; j++){
        		ind.push_back(face.mIndices[j]);
				}
			// каждого меша еще надо сделать материалы, но потом

			}  
		}


}

void load (std::string path, std::vector <Vert> & verts, std::vector <unsigned int> & indices)
//Создаем и загружаем геометрию поверхности
  //
{
    Assimp::Importer import;
		const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate);	
			//У vase - 1 потомок с 1 мешем, у table - 1 потомок с двумя мешами

		fun1(scene, verts, indices);
	
    g_vertexBufferObject = 0;
    GLuint vertexLocation = 0; // simple layout, assume have only positions at location = 0

   /*Л. везде дальше GL_CHECK_ERRORS - полезный макрос для проверки ошибок в строчке, где он был записан  */


/*Л.   Следующие три строки работаем с Vertex Buffer Object (VBO) —  средством OpenGL, позволяющее загружать определенные данные в память GPU
 https://eax.me/opengl-vbo-vao-shaders/*/   


    glGenBuffers(1, &g_vertexBufferObject);                                                        GL_CHECK_ERRORS;
		// Копируем массив с вершинами в буфер OpenGL

		glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);             
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vert), &verts[0], GL_STATIC_DRAW);                                GL_CHECK_ERRORS;
	
	
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_vertexEBO);             
  //  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


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



void draw(ShaderProgram program, GLFWwindow*  window, std::vector <Vert> & verts, std::vector <unsigned int> & indices,
	 glm::mat4 trans, glm::mat4 view, glm::mat4 proj  /*, float ** col*/) {
			    /* Л. glfwPollEvents  обрабатывает только те события, которые уже находятся в очереди событий, а затем сразу же возвращается. 
    Обработка событий вызовет окно и входные обратные вызовы, связанные с этими событиями. */
		glfwPollEvents();


//Л. glClearColor задает красные, зеленые, синие и Альфа-значения, используемые glClear для очистки цветовых буферов.
		//очищаем экран каждый кадр
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);               GL_CHECK_ERRORS;

		/*Л. glClear устанавливает область окна к ранее выбранным значениям по glClearColor, glClearIndex, glClearDepth, glClearStencil, и glClearAccum.
  Несколько цветовых буферов можно очистить одновременно, выбрав более чем один буфер за раз, используя glDrawBuffer.    */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

/* Л. program.StartUseShader() запускаем шейдеры*/
    program.StartUseShader();                           GL_CHECK_ERRORS;


    // очистка и заполнение экрана цветом
    //
		    /* Л.  glViewport определяет аффинное преобразование координат x и y из нормализованных координат устройства в координаты окна.  
    https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glViewport.xml*/
    glViewport  (0, 0, WIDTH, HEIGHT);

		    /*Л. следующие две строчки - см.ранее*/
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // draw call
    //
		    /*Л. рисуем примитивы, которые загрузили ранее
      https://eax.me/opengl-vbo-vao-shaders/*/   
    glBindVertexArray(g_vertexArrayObject); GL_CHECK_ERRORS;
		
		GLuint transformLoc2 = glGetUniformLocation(program.GetProgram(), "g_matrixScale");   GL_CHECK_ERRORS;
		std::cout << "trans" << transformLoc2 << "\n";
		glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(trans)); GL_CHECK_ERRORS;

		GLuint transformLoc1 = glGetUniformLocation(program.GetProgram(), "g_matrixView");   GL_CHECK_ERRORS;
		std::cout << "view" << transformLoc1 << "\n";
		glUniformMatrix4fv(transformLoc1, 1, GL_FALSE, glm::value_ptr(view)); GL_CHECK_ERRORS;

		GLuint transformLoc3 = glGetUniformLocation(program.GetProgram(), "g_matrixProj");   GL_CHECK_ERRORS;
		std::cout << "proj" << transformLoc3 << "\n";
		glUniformMatrix4fv(transformLoc3, 1, GL_FALSE, glm::value_ptr(proj)); GL_CHECK_ERRORS;

		unsigned int points[3] = {0, 1, 2};
		//174079
		for (int i = 0; i < 174079; i++) {//3 * i + 2 < indices.size(); i++) {
			for (int j = 0; j < 3; j++) {
				points[j] = indices[3 * i + j];
			}
		//	for (int k = 0; k < 3; k++) {
		//		glm::vec4 v1 = proj * view *  trans * glm::vec4(verts[points[k]].c, 1.0); 
		//		std::cout << v1[0] << " " << v1[1] << " " << v1[2] << "\n";
		//	}
		//	std::cout << verts[points[0]].c[0] << " "<< verts[points[0]].c[1] << " " << verts[points[0]].c[2]  << "\n";
	//		std::cout << verts[points[1]].c[0] << " "<< verts[points[1]].c[1] << " " << verts[points[1]].c[2]  << "\n";
	//		std::cout << verts[points[2]].c[0] << " "<< verts[points[2]].c[1] << " " << verts[points[2]].c[2]  << "\n";
	//		std ::cout << "////////////////////" << "\n";
    	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, points);
		}
    glBindVertexArray(0);

    program.StopUseShader();

//Л. glfwSwapBuffers меняет местами передний и задний буферы указанного окна. 
//Если интервал подкачки больше нуля, драйвер GPU ожидает указанное количество обновлений экрана перед заменой буферов.
		glfwSwapBuffers(window); 

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
	time_t prev = time(0);

	double ms_per_update =  4.999f;//0.115f;
	
	std::vector <Vert> verts;
  std::vector <unsigned int> indices;


	glm::mat4 trans(1.0f), view(1.0f), project(1.0f);  GL_CHECK_ERRORS;
	trans = glm::scale(trans, glm::vec3(0.3, 0.5, 0.5));  GL_CHECK_ERRORS;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	float aspect = WIDTH / HEIGHT;
	project = glm::ortho(-1.0f, 1.0f, -1.0f * aspect, 1.0f * aspect, 0.1f, 100.0f );
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
				std::cout << trans[i][j] << " ";
		}
		std::cout << "\n";
	}
	load("/media/derin/DATA/Computer_Graph/3/prob1/objects/vase.obj", verts, indices); GL_CHECK_ERRORS

	while (!glfwWindowShouldClose(window))
	{
		time_t begin = time(0);  	
		
	//	std::cout << "----------------------------" << "\n";
  	//load("/media/derin/DATA/Computer_Graph/3/prob1/objects/table_simple.obj");
		
		draw(program, window, verts, indices, trans, view, project); GL_CHECK_ERRORS
		sleep(ms_per_update - difftime(time(0), begin));
		break;
	}

	//очищаем vbo и vao перед закрытием программы
  //
	glDeleteVertexArrays(1, &g_vertexArrayObject);
  glDeleteBuffers(1, &g_vertexBufferObject);

	glfwTerminate();
	std::cout << indices.size()<<"\n";
	return 0;
}
