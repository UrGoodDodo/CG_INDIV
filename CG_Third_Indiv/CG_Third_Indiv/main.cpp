#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h> 
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <list>

#include "model.h"
#include "shader.h"

glm::mat4 projection = glm::perspective(glm::radians(45.0f), 900.0f / 900.0f, 0.1f, 100.0f);

//камера
glm::vec3 cameraPos = glm::vec3(0.1f, 38.2f, 55.7f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.5f, -0.8f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

GLuint snowVAO;
GLuint snowVBO;
GLuint snowTexture;

// ID шейдерной программы
GLuint Program;

//позиция света
glm::vec3 lightPos(0.0f, 2.0f, 10.0f);
glm::vec3 lightDirection(10.5f, -12.5f, -18.5f);
glm::vec3 lightness(1.0f, 1.0f, 1.0f);
float conus = 12.5f;

vector<Model> models;

//позиция дирижабля
glm::vec3 airshipPos(0.0f, 10.0f, 15.0f);
glm::vec3 baloon_threshold(-2.0f, 10.0f, 0.0f);
float updown = 0.005f;
int numb_clouds = 12;
vector<glm::vec3> position_of_clouds;
vector<float> scale_of_clouds;

void checkOpenGLerror()
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "OpenGL error " << err << std::endl;
	}
}

void ShaderLog(unsigned int shader)
{
	int infologLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	GLint vertex_compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &vertex_compiled);

	if (infologLen > 1)
	{
		int charsWritten = 0;
		std::vector<char> infoLog(infologLen);
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
		std::cout << "InfoLog: " << infoLog.data() << std::endl;
	}

	if (vertex_compiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(shader, 1024, &log_length, message);
		std::cout << "InfoLog2: " << message << std::endl;
	}

}

void InitShader()
{
	GLuint vShaderFong = glCreateShader(GL_VERTEX_SHADER); 
	// Передаем исходный код
	glShaderSource(vShaderFong, 1, &VertexShader, NULL); 
	// Компилируем шейдер
	glCompileShader(vShaderFong);
	std::cout << "vertex shader f\n";
	// Функция печати лога шейдера
	ShaderLog(vShaderFong); 

	//-----------------------
	
	// Создаем фрагментный шейдер
	GLuint fShaderFong = glCreateShader(GL_FRAGMENT_SHADER);
	// Передаем исходный код
	glShaderSource(fShaderFong, 1, &FragShader, NULL);
	// Компилируем шейдер
	glCompileShader(fShaderFong);
	std::cout << "fragment shader f\n";
	// Функция печати лога шейдера
	ShaderLog(fShaderFong);


	// Проверяем статус сборки
	int link_ok;
	Program = glCreateProgram();
	glAttachShader(Program, vShaderFong);
	glAttachShader(Program, fShaderFong);

	// Линкуем шейдерную программу
	glLinkProgram(Program);

	// Проверяем статус сборки
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);

	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		return;
	}

	checkOpenGLerror();
}

void InitPlane()
{
	float snow[] = {
		 40.0f, -0.5f,  40.0f,  0.0f, 1.0f, 0.0f,  5.0f,  0.0f,
		-40.0f, -0.5f,  40.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-40.0f, -0.5f, -40.0f,  0.0f, 1.0f, 0.0f,   0.0f, 5.0f,

		 40.0f, -0.5f,  40.0f,  0.0f, 1.0f, 0.0f,  5.0f,  0.0f,
		-40.0f, -0.5f, -40.0f,  0.0f, 1.0f, 0.0f,   0.0f, 5.0f,
		 40.0f, -0.5f, -40.0f,  0.0f, 1.0f, 0.0f,  5.0f, 5.0f
	};

	//объявляем массив атрибутов и буфер

	glGenVertexArrays(1, &snowVAO);
	glGenBuffers(1, &snowVBO);
	glBindVertexArray(snowVAO);
	// передаем вершины в буфер
	glBindBuffer(GL_ARRAY_BUFFER, snowVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(snow), snow, GL_STATIC_DRAW);

	// Подключаем массив аттрибутов с указанием на каких местах кто находится
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindVertexArray(0);

	// создаем текстуру
	glGenTextures(1, &snowTexture);

	// связываем с типом текступы
	glBindTexture(GL_TEXTURE_2D, snowTexture);

	// настроки отображения текстуры при выходе за пределы диапазона текстуры
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// настройки отображения текстуры в зависимости от удаления или приближения обьекта
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// грузим картинку
	int width, height;
	unsigned char* image = SOIL_load_image("snow.jpg", &width, &height, 0, SOIL_LOAD_RGB);

	//создаем текстуру
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	//отключаем привязку к текстуре
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Init()
{
	// Шейдеры
	InitShader();

	//включаем тест глубины
	glEnable(GL_DEPTH_TEST);
}

void InitUniforms() 
{
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(glm::radians(45.0f), 900.0f / 900.0f, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(glGetUniformLocation(Program, "airshipPos"), airshipPos.x, airshipPos.y, airshipPos.z);
	glUniform3f(glGetUniformLocation(Program, "position"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(Program, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(Program, "ambient"), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(Program, "diffuse"), 0.9f, 0.9f, 0.9f);
	glUniform3f(glGetUniformLocation(Program, "specular"), 1.0f, 1.0f, 1.0f);
	glUniform1i(glGetUniformLocation(Program, "shininess"), 16);
	glUniform3f(glGetUniformLocation(Program, "direction"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(Program, "direction_spot"), 0.0f, -28.0f, -43.0f);
	glUniform1f(glGetUniformLocation(Program, "constant_atten"), 1.0f);
	glUniform1f(glGetUniformLocation(Program, "linear_atten"), 0.045f);
	glUniform1f(glGetUniformLocation(Program, "quadratic_atten"), 0.0075f);
	glUniform1f(glGetUniformLocation(Program, "cutOff"), glm::cos(glm::radians(conus)));
	glUniform1f(glGetUniformLocation(Program, "outerCutOff"), glm::cos(glm::radians(conus * 1.4f)));
	glUniform1i(glGetUniformLocation(Program, "shininess"), 2);
}

void Draw(sf::Clock clock)
{
	glm::mat4 model = glm::mat4(1.0f);

	glUseProgram(Program); // Устанавливаем шейдерную программу текущей
	InitUniforms();
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, snowTexture);
	glUniform1i(glGetUniformLocation(Program, "tex"), 0);
	glBindVertexArray(snowVAO);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	//minecraft supa dupa ship mega omega пауа
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-9.0f, 0.0f, 20.0f));
	model = glm::translate(model, glm::vec3(airshipPos.x, airshipPos.y, airshipPos.z));
	model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

	glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	models[0].Draw(Program, 1);


	//Дома
	float angle = 30;
	float offset = 0;
	bool flag = false;
	for (size_t i = 0; i < 4; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(24.0f - offset, 0.0f, -20.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(-180.0f + angle), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		models[1].Draw(Program, 1);
		offset += 16;
		if (!flag)
			angle *= -1;

	}

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-24.0f, 0.0f, 12.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	models[1].Draw(Program, 1);


	//Ель
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 6.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	models[2].Draw(Program, 1);

	//speedy gonshik run forest around tree
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, clock.getElapsedTime().asSeconds() * glm::radians(-20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(5.5f, 0.0f, 0.0f));
	

	glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	models[3].Draw(Program, 1);


	//облака
	for (int i = 0; i < numb_clouds; i++)
	{
		model = glm::mat4(1.0f);
		position_of_clouds[i] += glm::vec3(sin(clock.getElapsedTime().asSeconds()) * 0.1f , 0.0f, 0.0f);
		model = glm::translate(model, position_of_clouds[i]);
		model = glm::scale(model, glm::vec3(18.0f + scale_of_clouds[i], 18.0f + scale_of_clouds[i], 18.0f + scale_of_clouds[i]));
		model = glm::rotate(model, clock.getElapsedTime().asSeconds() * glm::radians(scale_of_clouds[i] * 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		models[4].Draw(Program, 1);
	}

	//Шарик

	if (baloon_threshold.y > 18.0f || baloon_threshold.y < 15.0f)
	{
		updown *= -1.0f;
	}

	baloon_threshold += glm::vec3(0.0f, updown, 0.0f);

	model = glm::mat4(1.0f);

	model = glm::translate(model, baloon_threshold);
	model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
	model = glm::rotate(model, clock.getElapsedTime().asSeconds() * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	models[5].Draw(Program, 1);

	//снеговик
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(20.0f, 6.3f, 10.0f));
	model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
	model = glm::rotate(model, glm::radians(-115.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	models[6].Draw(Program, 1);

	//Пряничный человечек
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-17.0f, -0.8f, -2.0f));
	model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	models[7].Draw(Program, 1);

	float prianik_offset = 0;
	float prianik_angle = 30;
	bool prianik_flag = false;
	for (size_t i = 0; i < 7; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-4.0f + prianik_offset, 0.0f, 17.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, glm::radians(prianik_angle), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		models[7].Draw(Program, 1);
		prianik_offset += 2;
		if (!flag)
			prianik_angle *= -1;
		prianik_flag = !prianik_flag;
	}

	glUseProgram(0); // Отключаем шейдерную программу

	checkOpenGLerror();
}


// Освобождение буфера
void ReleaseVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Освобождение шейдеров
void ReleaseShader()
{
	// Передавая ноль, мы отключаем шейдерную программу
	glUseProgram(0);
	// Удаляем шейдерные программы
	glDeleteProgram(Program);
}


void Release()
{
	// Шейдеры
	ReleaseShader();
	// Вершинный буфер
	ReleaseVBO();
}

void Calculate_Clouds() 
{
	srand(time(0));
	// rand() % (end - (start) + 1) для промежутков
	for (int i = 0; i < numb_clouds; i++)
	{
		float x = (rand() % (25 - (-25) + 1) + (-25)) * 1.0f;

		float z = (rand() % (15 - (-25) + 1) + (-25)) * 1.0f;

		float y = (rand() % (25 - 24 + 1) + 24) * 1.0f;

		position_of_clouds.push_back(glm::vec3(x, y, z));

		y = (rand() % (-5 - 5 + 1) + 5) * 1.0f;

		scale_of_clouds.push_back(y);

	}
}

int main()
{
	std::setlocale(LC_ALL, "Russian");

	sf::Window window(sf::VideoMode(900, 900), "Happy New Year", sf::Style::Default, sf::ContextSettings(24));
	window.setVerticalSyncEnabled(true);
	window.setActive(true);
	
	glewInit();
	glGetError();
	Calculate_Clouds();

	sf::Clock clock;
		
	Model airship("ship/Mineways2Skfb.obj");
	models.push_back(airship);

	Model house("domik/Snow covered CottageOBJ.obj"); 
	models.push_back(house);
	
	Model tree("tree/Model.obj");
	models.push_back(tree);

	Model sani("sani/Horse Drawn Sleigh 02.obj");
	models.push_back(sani);

	Model cloud("cloud/cloud.obj");
	models.push_back(cloud);

	Model airbaloon("baloon/smiley.obj");
	models.push_back(airbaloon);
	
	Model snowman("decor/model.obj");
	models.push_back(snowman);

	Model kust("decor/CHARACTER_Gingerbread Man.obj");
	models.push_back(kust);


	Init();
	InitPlane();
	float t = 0.0;
	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{

			float camera_speed = 0.5f;

			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
					break;
				}
				if (event.key.code == sf::Keyboard::W)
				{
					airshipPos -= glm::vec3(0.0f, 0.0f, 0.5f);
				}
				if (event.key.code == sf::Keyboard::S)
				{
					airshipPos += glm::vec3(0.0f, 0.0f, 0.5f);
				}
				if (event.key.code == sf::Keyboard::A)
				{
					airshipPos -= glm::vec3(0.5f, 0.0f, 0.0f);
				}
				if (event.key.code == sf::Keyboard::D)
				{
					airshipPos += glm::vec3(0.5f, 0.0f, 0.0f);
				}
				else if (event.type == sf::Event::Resized)
					glViewport(0, 0, event.size.width, event.size.height);
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
		Draw(clock);

		window.display();
	}

	Release();
	return 0;
}