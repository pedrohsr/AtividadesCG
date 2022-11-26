/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Jogos Digitais - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 09/03/2022
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"


// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// Prot�tipo da fun��o de callback de mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos);


// Prot�tipos das fun��es
int setupGeometry();
int setupMesh(string path);
vector <string> tokenize(string buffer, string delim);

void processInput(GLFWwindow* window);

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 800, HEIGHT = 600;


bool rotateX = false, rotateY = false, rotateZ = false;
int viewID = 1;

//Vari�veis globais para o controle da c�mera
glm::vec3 cameraPos, cameraFront, cameraUp;
bool firstMouse = true;
float lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0; //para calcular o quanto que o mouse deslocou
float yaw = -90.0, pitch = 0.0; //rota��o em x e y

//var temporaria enquanto n�o vira classe
int meshSize = 0;

// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	//Muita aten��o aqui: alguns ambientes n�o aceitam essas configura��es
	//Voc� deve adaptar para a vers�o do OpenGL suportada por sua placa
	//Sugest�o: comente essas linhas de c�digo para desobrir a vers�o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Cubo!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	//Dasabilitando o cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	Shader shader = Shader("../shaders/phong.vs", "../shaders/phong.fs");

	// Gerando um buffer simples, com a geometria de um tri�ngulo
	GLuint VAO = setupMesh("../models/subdividedSphere.obj");

	glUseProgram(shader.ID);

	//Inicializa��o dos par�metros da c�mera
	cameraPos = glm::vec3(0.0, 0.0, 3.0); //ponto
	cameraFront = glm::vec3(0.0, 0.0, -1.0); //vetor
	cameraUp = glm::vec3(0.0, 1.0, 0.0);

	//Propriedades do material dos objetos
	float ka, kd, ks, n;

	//Mandando para o shader as infos de ilumina��o
	GLint objectColorLoc = glGetUniformLocation(shader.ID, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(shader.ID, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(shader.ID, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(shader.ID, "viewPos");
	glUniform3f(objectColorLoc, 1.0f, 0.0f, 1.0f);
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLoc, 0.0, 5.0, 2.0);
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	// MATRIZ DE VIEW (olhando de frente)
	glm::mat4 view = glm::mat4(1); //matriz identidade;
	view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	//MATRIZ DE PROJE��O - tipo de proje��o: ortogr�fica ou perspectiva
	glm::mat4 projection = glm::mat4(1); //matriz identidade;
	projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	//projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -3.0f, 3.0f);
	GLint projectionLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, FALSE, glm::value_ptr(projection));

	GLint modelLoc = glGetUniformLocation(shader.ID, "model");

	glEnable(GL_DEPTH_TEST);


	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		processInput(window);

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(1);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		glm::mat4 view = glm::mat4(1); //matriz identidade;
		//Aqui atualizamos o view com a posi��o e orienta��o da c�mera atualizados
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));
		//Atualizar o shader com a posi��o da c�mera
		glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
		
		//MATRIZ DE MODELO
		 //matriz identidade;
		
		
			
		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES

		float xi = -1.5;
		float offsetx = 0.7;

		float yi = -0.5;
		float offsety = 0.7;

		shader.setFloat("ka", 0.2);
		shader.setFloat("kd", 0.5);
		shader.setFloat("ks", 0.25);

		float ns[5] = { 3.0, 5.0, 10.0, 27.0, 200.0 };
		float kss[3] = { 0.1, 0.25, 0.5 };

		glUniform3f(objectColorLoc, 1.0f, 0.0f, 1.0f);
		glBindVertexArray(VAO);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				shader.setFloat("ks", kss[i]);
				shader.setFloat("n", ns[j]);
				glm::mat4 model = glm::mat4(1);
				model = glm::translate(model, glm::vec3(xi + j * offsetx, yi + i*offsety, 0.0));
				model = glm::scale(model, glm::vec3(0.3, 0.3, 0.3));
				glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, meshSize);
			}
		}

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP
		//glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);
		//glDrawArrays(GL_LINE_LOOP, 0, meshSize);
		glBindVertexArray(0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = !rotateX;
	}

	else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateY = !rotateY;
	}

	else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateZ = !rotateZ;
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		viewID = 1;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		viewID = 2;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		viewID = 3;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		viewID = 4;
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		viewID = 5;
	}


}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//Na primeira vez o deslocamento ser� zero
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//Calculando o deslocamento do Mouse
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	//Amortizando, deixando o movimento mais suave
	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	//Atualizando os �ngulos de yaw e pitch
	yaw += xoffset;
	pitch += yoffset;

	//Colocando limite no �ngulo de pitch (nao � a menina do exorcista)
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	//Finalmente, usando os valiosos conhecimentos de Trigonometria para
	//atualizar o vetor front (Viva Pit�goras!)
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

}

// Esta fun��o est� bastante harcoded - objetivo � criar os buffers que armazenam a 
// geometria de um tri�ngulo
// Apenas atributo coordenada nos v�rtices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A fun��o retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {

		//Base do cubo: 2 tri�ngulos
		//x    y    z    r    g    b
		-0.5, -0.5, -0.5, 0.88, 0.15, 0.07,
		-0.5, -0.5,  0.5, 0.88, 0.15, 0.07,
		 0.5, -0.5, -0.5, 0.88, 0.15, 0.07,

		 -0.5, -0.5, 0.5,  0.88, 0.15, 0.07,
		  0.5, -0.5,  0.5, 0.88, 0.15, 0.07,
		  0.5, -0.5, -0.5, 0.88, 0.15, 0.07,

		  //Face de cima: 2 tri�ngulos
		  -0.5, 0.5,  0.5, 1.0, 0.41, 0.03,
		   0.5, 0.5,  0.5, 1.0, 0.41, 0.03,
		  -0.5, 0.5, -0.5, 1.0, 0.41, 0.03,

		  0.5, 0.5,  0.5, 1.0, 0.41, 0.03,
		  0.5, 0.5, -0.5, 1.0, 0.41, 0.03,
		 -0.5, 0.5, -0.5, 1.0, 0.41, 0.03,

		 //Face de frente: 2 tri�ngulos
		 -0.5, -0.5, -0.5, 0.94, 1.0, 0.03,
		 -0.5,  0.5, -0.5, 0.94, 1.0, 0.03,
		  0.5, -0.5, -0.5, 0.94, 1.0, 0.03,

		  0.5, -0.5, -0.5, 0.94, 1.0, 0.03,
		 -0.5,  0.5, -0.5, 0.94, 1.0, 0.03,
		  0.5,  0.5, -0.5, 0.94, 1.0, 0.03,

		  //Face de tr�s: 2 tri�ngulos
		  -0.5,  0.5,  0.5, 0.09, 0.49, 0.12,
		  -0.5, -0.5,  0.5, 0.09, 0.49, 0.12,
		   0.5,  0.5,  0.5, 0.09, 0.49, 0.12,

		   0.5,  0.5,  0.5, 0.09, 0.49, 0.12,
		  -0.5, -0.5,  0.5, 0.09, 0.49, 0.12,
		   0.5, -0.5,  0.5, 0.09, 0.49, 0.12,

		   //Face da esquerda: 2 tri�ngulos
		  -0.5,  0.5, -0.5, 0.28, 0.28, 1.0,
		  -0.5, -0.5, -0.5, 0.28, 0.28, 1.0,
		  -0.5, -0.5,  0.5, 0.28, 0.28, 1.0,

		  -0.5, -0.5,  0.5, 0.28, 0.28, 1.0,
		  -0.5,  0.5,  0.5, 0.28, 0.28, 1.0,
		  -0.5,  0.5, -0.5, 0.28, 0.28, 1.0,

		  //Face da direita: 2 tri�ngulos
		   0.5,  0.5,  0.5, 0.47, 0.18, 0.54,
		   0.5, -0.5,  0.5, 0.47, 0.18, 0.54,
		   0.5, -0.5, -0.5, 0.47, 0.18, 0.54,

		   0.5, -0.5, -0.5, 0.47, 0.18, 0.54,
		   0.5,  0.5, -0.5, 0.47, 0.18, 0.54,
		   0.5,  0.5,  0.5, 0.47, 0.18, 0.54,

		   //Ch�o: 2 tri�ngulos
		   -1.0, -0.5, -1.0, 0.5, 0.5, 0.5,
		   -1.0, -0.5,  1.0, 0.5, 0.5, 0.5,
			1.0, -0.5, -1.0, 0.5, 0.5, 0.5,

		   -1.0, -0.5,  1.0,  0.5, 0.5, 0.5,
			1.0, -0.5,  1.0, 0.5, 0.5, 0.5,
			1.0, -0.5, -1.0, 0.5, 0.5, 0.5,

	};

	GLuint VBO, VAO;

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posi��o (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);



	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

int setupMesh(string path)
{
	vector <glm::vec3> vertices, normals;
	vector <glm::vec2> texCoords;
	vector <glm::vec3> vertIndices, normIndices, texIndices; //indices s�o ints, mas s�o 3

	ifstream inputFile;
	inputFile.open(path);
	if (inputFile.is_open())
	{
		char line[100];
		string sline;
		int cont = 1;
		while (!inputFile.eof())
		{
			inputFile.getline(line, 100);
			if (!inputFile.eof())
			{
				//cout << cont << " --> *" << line << "*" << endl;
				cont++;
				sline = line;
				string word;
				istringstream ssline(line);
				ssline >> word;
				if (word == "v" || word == "vn" || word == "vt" || word == "f")
				{
					glm::vec3 v3;
					if (word == "v")
					{
						ssline >> v3.x >> v3.y >> v3.z;
						vertices.push_back(v3);
					}
					if (word == "vn")
					{
						ssline >> v3.x >> v3.y >> v3.z;
						normals.push_back(v3);
					}
					if (word == "vt")
					{
						glm::vec2 v2;
						ssline >> v2.x >> v2.y;
						texCoords.push_back(v2);
					}
					if (word == "f")
					{
						string i1, i2, i3;
						glm::vec3 iv, ivn, ivt;
						ssline >> i1 >> i2 >> i3;
						vector <string> tokens;

						//Pegando os primeiros �ndices (considerando que tem todos)
						tokens = tokenize(i1, "/");
						iv[0] = atof(tokens[0].c_str());
						ivt[0] = atof(tokens[1].c_str());
						ivn[0] = atof(tokens[2].c_str());

						//Pegando os segundos �ndices (considerando que tem todos)
						tokens = tokenize(i2, "/");
						iv[1] = atof(tokens[0].c_str());
						ivt[1] = atof(tokens[1].c_str());
						ivn[1] = atof(tokens[2].c_str());

						//Pegando os terceiros �ndices (considerando que tem todos)
						tokens = tokenize(i3, "/");
						iv[2] = atof(tokens[0].c_str());
						ivt[2] = atof(tokens[1].c_str());
						ivn[2] = atof(tokens[2].c_str());

						vertIndices.push_back(iv);
						normIndices.push_back(ivn);
						texIndices.push_back(ivt);
					}

				}
				else
				{
					//ignorando o resto por enquanto
				}
			}//if (!inputFile.eof())
			else
			{
				cout << "Foi at� o fim!" << endl;
			}
		}
		inputFile.close();

		vector <float> geometryBuffer;

		meshSize = vertIndices.size() * 3;

		for (int i = 0; i < vertIndices.size(); i++)
		{
			for (int j = 0; j < 3; j++) //triangulo
			{
				//recupera x, y, z (posicao)
				float x = vertices[(int)vertIndices[i][j]-1].x;
				float y = vertices[(int)vertIndices[i][j]-1].y;
				float z = vertices[(int)vertIndices[i][j]-1].z;
				geometryBuffer.push_back(x);
				geometryBuffer.push_back(y);
				geometryBuffer.push_back(z);

				//recupera nx, ny, nz (normal)
				float nx = normals[(int)normIndices[i][j]-1].x;
				float ny = normals[(int)normIndices[i][j]-1].y;
				float nz = normals[(int)normIndices[i][j]-1].z;
				geometryBuffer.push_back(nx);
				geometryBuffer.push_back(ny);
				geometryBuffer.push_back(nz);

				//recupera s, t (coord textura)
				/*float s = texCoords[(int)vertIndices[i][j]].x;
				float t = texCoords[(int)vertIndices[i][j]].y;
				geometryBuffer.push_back(s);
				geometryBuffer.push_back(t);*/
			}
		}


		GLuint VBO, VAO;

		//Gera��o do identificador do VBO
		glGenBuffers(1, &VBO);

		//Faz a conex�o (vincula) do buffer como um buffer de array
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		//Envia os dados do array de floats para o buffer da OpenGl
		glBufferData(GL_ARRAY_BUFFER, geometryBuffer.size() * sizeof(GLfloat), geometryBuffer.data(), GL_STATIC_DRAW);

		//Gera��o do identificador do VAO (Vertex Array Object)
		glGenVertexArrays(1, &VAO);

		// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
		// e os ponteiros para os atributos 
		glBindVertexArray(VAO);

		//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
		// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
		// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
		// Tipo do dado
		// Se est� normalizado (entre zero e um)
		// Tamanho em bytes 
		// Deslocamento a partir do byte zero 

		//Atributo posi��o (x, y, z)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		//Atributo normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
		// atualmente vinculado - para que depois possamos desvincular com seguran�a
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
		glBindVertexArray(0);

		return VAO;
	}
	else
	{
		cout << "Erro ao ler o arquivo!" << endl;
		return -1;
	}


	return 0;
}

void processInput(GLFWwindow* window)
{
	glfwPollEvents(); //verificar as callbacks de input

	float cameraSpeed = 0.05;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//Movimenta para frente
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		//Movimenta para tr�s
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		//Movimenta para esquerda - precisa calcular o vetor Right
		//Calcula-se pelo produto vetorial do Up e do Front
		//O vetor Right precisa ser unit�rio
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//Movimenta para direita
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

vector <string> tokenize(string buffer, string delim) {
	vector <string> tokens;
	string strbuffer(buffer);
	string token;
	int pos;

	do {
		pos = strbuffer.find(delim);
		if (pos > 0) {
			token = strbuffer.substr(0, pos);
			strbuffer = strbuffer.substr(pos + delim.size());
			tokens.push_back(token);
		}
		else tokens.push_back(strbuffer);
	} while (pos > 0);

	return tokens;
}
