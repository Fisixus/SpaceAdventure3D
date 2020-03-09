#include <iostream>
#include "Angel.h"
#include <vector>
#include <Math.h>
#include <playsoundapi.h>

using namespace Angel;
using namespace std;
typedef vec4 point4;
typedef vec4 color4;
const float PI = 3.141592653589793238463;
enum { ViewMode1 = 0, ViewMode2 = 1, ViewMode3 = 2, ViewMode4 = 3 };
int ViewMode = ViewMode1;
#define LARGE_Y 300;

const GLint width = 1440;
const GLint height = 900;
const GLfloat material_shininess = 1000.0f;

point4 light_position(2.0, 3.0, 300.0, 1.0); //wrt camera
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

point4 at(105.0, 15.0, -2.0, 1.0);
point4 eye(105.0, 15.0, -1.0, 1.0);
vec4 up(0.0, 1.0, 0.0, 0.0);

//For planets and space station
GLint sectorCount = 100;
GLint stackCount = 100;
GLfloat spaceshipSpeed = 1.5f;
vec4 velocityVector(0.0, 0.0, -1.0, 0.0);
GLfloat velocityDirectionAngle = 0.0;

GLint radiusCircleIndex = 1000;

GLfloat angularSpeed = 2.0f;
GLfloat rotatingDegreeSpaceStation = 0.0;
bool isPaused = false;
bool framePerMove = false;

vector<point4> points;
vector<point4> normals;
vector<color4> ambient_products;
vector<color4> diffuse_products;
vector<color4> specular_products;

GLint planetIndex = 0;
GLint torusIndex = 0;
GLint spacestationIndex = 0;

point4 spaceship_coord = { 105.0, 15.0, 0.0, 1.0 };
point4 station_coord = { 100.0, 10.0, -10.0, 1.0 };
point4 station_front = { 0.5, 0.5, 0.5, 1.0 };

point4 planet_coords[8] = { 
{30, 30, -30, 1},
{30, 15, -170, 1},
{80, 25, -110, 1},
{70, 12, -60, 1},
{90, 13, -150, 1},
{120, 17, -80, 1},
{150, 15, -40, 1},
{160, 22, -170, 1} };

color4 ambient_color_catalogue[8] = { {0.633, 0.727811, 0.633, 1.0},
{0.30, 0.30, 0.30, 1.0},
{1.00, 0.00, 0.00, 1.0},
{0.00, 1.00, 0.00, 1.0},
{0.00, 0.00, 1.00, 1.0},
{1.00, 1.00, 0.00, 1.0},
{0.00, 1.00, 1.00, 1.0},
{1.00, 1.00, 1.00, 1.0} };

color4 diffuse_colors_catalogue[8] = { {0.07568, 0.61424, 0.07568, 1.0},
{0.30, 0.30, 0.30, 1.0},
{1.00, 0.00, 0.00, 1.0},
{1.0, 0.5, 0.0, 1.0},
{0.00, 0.00, 1.00, 1.0},
{1.00, 1.00, 0.00, 1.0},
{0.00, 1.00, 1.00, 1.0},
{1.00, 0.50, 1.00, 1.0} };

color4 specular_colors_catalogue[8] = { {0.0215, 0.1745, 0.0215, 1.0},
{0.30, 0.30, 0.30, 1.0},
{1.00, 0.00, 0.00, 1.0},
{1.00, 1.00, 1.00, 1.0},
{0.00, 0.00, 1.00, 1.0},
{1.00, 1.00, 0.00, 1.0},
{0.00, 1.00, 1.00, 1.0},
{0.00, 0.00, 0.00, 0.0} };

GLint ModelView, Projection;
GLuint vao;
GLuint translateObjectIndex;
GLuint  thetaIndex;
GLfloat  DefaultThetaValue[3] = { 0.0, 0.0, 0.0 };

void drawPlanet(GLfloat radius, const color4 &ambient_color,
			 const color4 &diffuse_color, const color4 &specular_color) {
	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;       
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           

			float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			float y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			point4 point = { x,y,z,1.0 };
			points.push_back(point);
			vec4 normalized = normalize(vec4(x, y, z, 0.0));
			normalized.w = 0.0;
			normals.push_back(normalized);
			ambient_products.push_back(light_ambient * ambient_color);
			diffuse_products.push_back(light_diffuse * diffuse_color);
			specular_products.push_back(light_specular * specular_color);
			planetIndex++;
		}
	}

	GLfloat angle;
	for (int i = 0; i < radiusCircleIndex/2; i++)
	{
		angle = 2 * PI * (i + 1) / 499;
		points.push_back(point4(cos(angle) *  3.5, 0.0 , sin(angle) * 3.5, 1.0));
		normals.push_back(vec4(cos(angle) *  3.5, 0.0, sin(angle) * 3.5, 0.0));
		ambient_products.push_back(light_ambient*vec4(1.0, 1, 1, 1.0));
		diffuse_products.push_back(light_diffuse*vec4(1.0, 1, 1, 1.0));
		specular_products.push_back(light_specular*vec4(1, 1, 1, 1.0));
	}

	for (int i = 0; i < radiusCircleIndex / 2; i++)
	{
		angle = 2 * PI * (i + 1) / 499;
		points.push_back(point4(cos(angle) *  4.3, 0.0, sin(angle) * 4.3, 1.0));
		normals.push_back(vec4(cos(angle) *  4.3, 0.0, sin(angle) * 4.3, 0.0));
		ambient_products.push_back(light_ambient*vec4(1.0, 1, 1, 1.0));
		diffuse_products.push_back(light_diffuse*vec4(1.0, 1, 1, 1.0));
		specular_products.push_back(light_specular*vec4(1, 1, 1, 1.0));
	}


}

void drawSpaceStation(GLfloat radius, const color4 &ambient_color,
	const color4 &diffuse_color, const color4 &specular_color) {

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;

			float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			float y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			point4 point = { x,y,z,1.0 };
			points.push_back(point);
			vec4 normalized = normalize(vec4(x, y, z, 0.0));
			normalized.w = 0.0;
			normals.push_back(normalized);

			ambient_products.push_back(light_ambient * ambient_color);
			diffuse_products.push_back(light_diffuse * diffuse_color);
			specular_products.push_back(light_specular * specular_color);

			spacestationIndex++;
		}
	}

}

void pushTetraHedronandFrontColor() {
	ambient_products.push_back(light_ambient*vec4(1.0, 0.0, 0.0, 1.0));
	diffuse_products.push_back(light_diffuse*vec4(1.0, 0.0, 0.7, 1.0));
	specular_products.push_back(light_specular*vec4(0.0215, 0.1745, 0.0215, 1.0));
}

void triangle(const point4 &a, const point4 &b, const point4 &c) {
	points.push_back(a);
	pushTetraHedronandFrontColor();
	vec4 norm = normalize(cross(b - a, c - a));
	norm.w = 0.0;
	normals.push_back(norm);
	points.push_back(b);
	norm = normalize(cross(a - c, b - c));
	norm.w = 0.0;
	normals.push_back(norm);
	pushTetraHedronandFrontColor();
	points.push_back(c);
	norm = normalize(cross(a - b, c - b));
	norm.w = 0.0;
	normals.push_back(norm);
	pushTetraHedronandFrontColor();
}


void drawFront() {
	triangle(point4(-0.4, 0.4,-4, 1.0), point4(-0.4, -0.4, -4, 1.0), point4(0.4, -0.4, -4, 1.0));
	triangle(point4(0.4, -0.4, -4, 1.0), point4(0.4, 0.4, -4, 1.0), point4(-0.4, 0.4, -4, 1.0));
}

//TRIANGLE_STRIP
void drawTorusY() {

	float innerR = 0.1;
	float outerR = 0.8;
	int i, j, k;
	float outerAngle, innerAngle;
	int numc = 100;
	int numt = 100;
	float twopi = 2 * PI;
	for (i = 0; i < numc; i++) {
		for (j = 0; j <= numt; j++) {
			for (k = 1; k >= 0; k--) {
				outerAngle = (i + k) % numc + 0.5;
				innerAngle = j % numt;

				float x = (outerR + innerR * cos(outerAngle*twopi / numc))*cos(innerAngle*twopi / numt);
				float y = (outerR + innerR * cos(outerAngle*twopi / numc))*sin(innerAngle*twopi / numt);
				float z = innerR * sin(outerAngle * twopi / numc);

				//For outerCircle
				float tangentX = -sin(innerAngle*twopi / numt);
				float tangentY = cos(innerAngle*twopi / numt);
				float tangentZ = 0;

				//For innerCircle
				float sx = cos(innerAngle*twopi / numt)*(-sin(outerAngle*twopi / numc));
				float sy = sin(innerAngle*twopi / numt)*(-sin(outerAngle*twopi / numc));
				float sz = cos(outerAngle*twopi / numc);

				/* normal is cross-product of tangents */
				float nx = tangentY * sz - tangentZ * sy;
				float ny = tangentZ * sx - tangentX * sz;
				float nz = tangentX * sy - tangentY * sx;



				point4 point = { x,y,z,1.0 };
				points.push_back(point);
				vec3 normalized = normalize(vec3(nx, ny, nz));
				normals.push_back(vec4(normalized, 0.0));
				ambient_products.push_back(light_ambient*vec4(1.0, 0.0, 0.0, 1.0));
				diffuse_products.push_back(light_diffuse*vec4(1.0, 0.61424, 0.07568, 1.0));
				specular_products.push_back(light_specular*vec4(0.0215, 0.1745, 0.0215, 1.0));
				torusIndex++;
			}
		}
	}

}

void drawTorusZ() {

	float innerR = 0.1;
	float outerR = 1.0;
	int i, j, k;
	float outerAngle, innerAngle;
	int numc = 100;
	int numt = 100;
	float twopi = 2 * PI;
	for (i = 0; i < numc; i++) {
		for (j = 0; j <= numt; j++) {
			for (k = 1; k >= 0; k--) {
				outerAngle = (i + k) % numc + 0.5;
				innerAngle = j % numt;

				float x = (outerR + innerR * cos(outerAngle*twopi / numc))*cos(innerAngle*twopi / numt);
				float y = (outerR + innerR * cos(outerAngle*twopi / numc))*sin(innerAngle*twopi / numt);
				float z = innerR * sin(outerAngle * twopi / numc);

				//For outerCircle
				float tangentX = -sin(innerAngle*twopi / numt);
				float tangentY = cos(innerAngle*twopi / numt);
				float tangentZ = 0;

				//For innerCircle
				float sx = cos(innerAngle*twopi / numt)*(-sin(outerAngle*twopi / numc));
				float sy = sin(innerAngle*twopi / numt)*(-sin(outerAngle*twopi / numc));
				float sz = cos(outerAngle*twopi / numc);

				/* normal is cross-product of tangents */
				float nx = tangentY * sz - tangentZ * sy;
				float ny = tangentZ * sx - tangentX * sz;
				float nz = tangentX * sy - tangentY * sx;

				mat4 rY = mat4(cos(90 * PI / 180), 0.0, sin(90 * PI / 180), 0.0,
					0.0, 1.0, 0.0, 0.0,
					-sin(90 * PI / 180), 0.0, cos(90 * PI / 180), 0.0,
					0.0, 0.0, 0.0, 1.0);
				point4 point = rY *  point4(x,y,z,1.0) ;
				points.push_back(point);
				vec4 normalized = normalize(rY * vec4(nx, ny, nz, 0.0));
				normals.push_back(normalized);
				ambient_products.push_back(light_ambient*vec4(1.0, 0.0, 0.0, 1.0));
				diffuse_products.push_back(light_diffuse*vec4(1.0, 0.61424, 0.07568, 1.0));
				specular_products.push_back(light_specular*vec4(0.0215, 0.1745, 0.0215, 1.0));
			}
		}
	}

}


//TRIANGLE_STRIP
void drawTetraHedron() {
	triangle(point4(-0.1, 0.0, -3.2, 1.0),point4(0.0, 0.0, -2.7, 1.0), point4(0.1, 0.0, -3.2, 1.0));
	triangle(point4(0.1, 0.0, -3.2, 1.0), point4(0.0, 0.2, -2.9, 1.0), point4(0.0, 0.0, -2.7, 1.0));
	triangle(point4(0.0, 0.2, -2.9, 1.0), point4(-0.1, 0.0, -3.2, 1.0), point4(0.0, 0.0, -3.7, 1.0));
	triangle(point4(0.0, 0.2, -2.9, 1.0), point4(-0.1, 0.0, -3.2, 1.0), point4(0.1, 0.0, -3.2, 1.0));
}

void drawPlanets() {
	///GREEN
	drawPlanet(3, ambient_color_catalogue[0], diffuse_colors_catalogue[0], specular_colors_catalogue[0]);
	///RED
	drawPlanet(3, ambient_color_catalogue[1], diffuse_colors_catalogue[2], specular_colors_catalogue[0]);
	///PINK
	drawPlanet(3, ambient_color_catalogue[2], diffuse_colors_catalogue[7], specular_colors_catalogue[1]);
	///YELLOW
	drawPlanet(3, ambient_color_catalogue[6], diffuse_colors_catalogue[5], specular_colors_catalogue[4]);
	///LIGHT BLUE
	drawPlanet(3, ambient_color_catalogue[0], diffuse_colors_catalogue[6], specular_colors_catalogue[5]);
	///BLUE
	drawPlanet(3, ambient_color_catalogue[0], diffuse_colors_catalogue[4], specular_colors_catalogue[5]);
	///PURPLE
	drawPlanet(3, ambient_color_catalogue[1], diffuse_colors_catalogue[7], specular_colors_catalogue[1]);
	///ORANGE
	drawPlanet(3, ambient_color_catalogue[7], diffuse_colors_catalogue[3], specular_colors_catalogue[3]);
}

void drawSpaceship() {
	drawTorusY();
	drawTorusZ();
	drawTetraHedron();
}


void init() {
	drawPlanets();
	drawSpaceStation(4.0, ambient_color_catalogue[7], diffuse_colors_catalogue[1], specular_colors_catalogue[1]);
	drawFront();
	drawSpaceship();
	PlaySound(TEXT("2001_A_Space_Odyssey.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*points.size() + sizeof(point4)*normals.size() + sizeof(point4)*ambient_products.size()
		+ sizeof(point4)*diffuse_products.size() + sizeof(point4)*specular_products.size(), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*points.size(), points.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*points.size(), sizeof(point4)*normals.size(), normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*points.size() + sizeof(point4)*normals.size(), sizeof(point4)*ambient_products.size(), ambient_products.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*points.size() + sizeof(point4)*normals.size() + sizeof(point4)*ambient_products.size()
		, sizeof(point4)*diffuse_products.size(), diffuse_products.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*points.size() + sizeof(point4)*normals.size() + sizeof(point4)*ambient_products.size()
		+ sizeof(point4)*diffuse_products.size(), sizeof(point4)*specular_products.size(), specular_products.data());

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4)*points.size()));

	GLuint AmbientProduct = glGetAttribLocation(program, "AmbientProduct");
	glEnableVertexAttribArray(AmbientProduct);
	glVertexAttribPointer(AmbientProduct, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4)*points.size() + sizeof(point4)*normals.size()));

	GLuint DiffuseProduct = glGetAttribLocation(program, "DiffuseProduct");
	glEnableVertexAttribArray(DiffuseProduct);
	glVertexAttribPointer(DiffuseProduct, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4)*points.size() + sizeof(point4)*normals.size() + sizeof(point4)*ambient_products.size()));

	GLuint SpecularProduct = glGetAttribLocation(program, "SpecularProduct");
	glEnableVertexAttribArray(SpecularProduct);
	glVertexAttribPointer(SpecularProduct, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4)*points.size() + sizeof(point4)*normals.size() + sizeof(point4)*ambient_products.size() + sizeof(point4)*diffuse_products.size()));

	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);

	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	translateObjectIndex = glGetUniformLocation(program, "TranslateObjects");
	thetaIndex = glGetUniformLocation(program, "Theta");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);
	mat4 model_view = LookAt(eye, at, up);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);


	for (int i = 0; i < 8; i++) {
		glUniform4fv(translateObjectIndex, 1, planet_coords[i]);
		glUniform3fv(thetaIndex, 1, DefaultThetaValue);
		glDrawArrays(GL_TRIANGLE_FAN, i * (planetIndex/8) + radiusCircleIndex / 2 * 2 * i, planetIndex / 8);
		glUniform3fv(thetaIndex, 1, vec3(0.0, 0.0, 45.0));
		glDrawArrays(GL_LINE_LOOP, (i+1)*(planetIndex / 8) + (radiusCircleIndex / 2) * 2 * i, radiusCircleIndex/2);
		glDrawArrays(GL_LINE_LOOP, (i+1)*(planetIndex / 8) + (radiusCircleIndex / 2) * (2 * i + 1) , radiusCircleIndex / 2);
	}

	glUniform3fv(thetaIndex, 1, vec3(0.0, rotatingDegreeSpaceStation, 0.0));
	glUniform4fv(translateObjectIndex, 1, station_coord);
	glDrawArrays(GL_TRIANGLE_FAN, planetIndex + radiusCircleIndex * 8, spacestationIndex);
	glDrawArrays(GL_TRIANGLE_FAN, planetIndex + spacestationIndex + radiusCircleIndex * 8, 6);

	glUniform3fv(thetaIndex, 1, vec3(0.0,velocityDirectionAngle,0.0));
	glUniform4fv(translateObjectIndex, 1, spaceship_coord);
	glDrawArrays(GL_TRIANGLE_STRIP, planetIndex + spacestationIndex + 6 + radiusCircleIndex * 8, torusIndex);
	glUniform4fv(translateObjectIndex, 1, spaceship_coord);
	glDrawArrays(GL_TRIANGLE_STRIP, planetIndex + spacestationIndex + 6 + torusIndex + radiusCircleIndex * 8, torusIndex);

	glUniform3fv(thetaIndex, 1, vec3(0.0, 0.0, velocityDirectionAngle));
	glUniform4fv(translateObjectIndex, 1, spaceship_coord);
	glDrawArrays(GL_TRIANGLES, planetIndex + spacestationIndex + 6 +  2 * torusIndex + radiusCircleIndex * 8, 12);
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
	glViewport(0, 0, w, h);
	GLfloat aspect = GLfloat(w) / h;
	mat4 projection = Perspective(90.0, aspect, 0.5, 2000.0);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

void specialKeyInputFunc(int key, int x, int y)
{
	if (velocityDirectionAngle == 360 || velocityDirectionAngle == -360) velocityDirectionAngle = 0;
	if (key == GLUT_KEY_LEFT)
	{
		if(ViewMode != ViewMode2)
		{
			velocityDirectionAngle += 5;
			velocityVector.x = -1 * sin(velocityDirectionAngle*DegreesToRadians);
			velocityVector.z = -1 * cos(velocityDirectionAngle*DegreesToRadians);
		}
	}

	if (key == GLUT_KEY_RIGHT)
	{
		if(ViewMode != ViewMode2)
		{
			velocityDirectionAngle -= 5;
			velocityVector.x = -1 * sin(velocityDirectionAngle*DegreesToRadians);
			velocityVector.z = -1 * cos(velocityDirectionAngle*DegreesToRadians);
		}
	}

	if (key == GLUT_KEY_UP)
	{
		if(spaceship_coord.y <= 20.0)
		{
			if (ViewMode != ViewMode2)
			{
				if (velocityVector.y < 0.0)
					velocityVector.y = 0.0;
				else
					velocityVector.y = 0.25;
			}
		}
	}

	if (key == GLUT_KEY_DOWN)
	{
		if(spaceship_coord.y >= -20.0)
		{
			if (ViewMode != ViewMode2)
			{
				if (velocityVector.y > 0.0)
					velocityVector.y = 0.0;
				else
					velocityVector.y = -0.25;
			}
		}
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'j': case 'J':
		if (isPaused) return;
		angularSpeed += 2.0;
		if (angularSpeed >= 20) angularSpeed = 20; //Clamping
		break;
	case 'k': case 'K':
		if (isPaused) return;
		angularSpeed -= 2.0;
		if (angularSpeed <= 0) angularSpeed = 2; //Clamping
		break;
	case 'a': case 'A':
		if (isPaused) return;
		if(ViewMode != ViewMode2)
			spaceshipSpeed -= 0.25;
		if (spaceshipSpeed <= 0) spaceshipSpeed = 0.25;
		break;
	case 'd': case 'D':
		if (isPaused) return;
		if(ViewMode != ViewMode2)
			spaceshipSpeed += 0.25;
		break;
	case 'c': case 'C':
		ViewMode = ViewMode1;
		eye.z = spaceship_coord.z - 1.0;
		eye.y = spaceship_coord.y;
		eye.x = spaceship_coord.x; // If you commit this you can see the real torin
		at.z = eye.z - 1;
		at.y = spaceship_coord.y;
		at.x = spaceship_coord.x;
		break;
	case 's': case 'S':
		ViewMode = ViewMode2;
		eye.z = station_coord.z - 5; // 5 equals to r of space station
		eye.y = station_coord.y;
		eye.x = station_coord.x;
		at.z = eye.z - 1;
		at.y = station_coord.y;
		at.x = station_coord.x;
		break;
	case 't': case 'T':
		ViewMode = ViewMode3;
		eye.z = spaceship_coord.z + 4;
		eye.y = spaceship_coord.y + 2.0;
		eye.x = spaceship_coord.x; // If you commit this you can see the real torin
		at.y = spaceship_coord.y + 2.0;
		at.z = eye.z - 1;
		at.x = spaceship_coord.x;
		break;
	case 'w': case 'W':
		ViewMode = ViewMode4;
		eye.y = LARGE_Y;
		eye.z = -200;
		at.z = eye.z - 1;
		at.x = 90;
		break;
	case 'p': case 'P':
		isPaused = !isPaused;
		if(isPaused) PlaySound(NULL, 0, 0);
		else if(!isPaused) PlaySound(TEXT("2001_A_Space_Odyssey.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		break;
	}
	glutPostRedisplay();
}

void debugInformation() {
	system("CLS");
	printf("Spaceship Location(x,y,z): %f,%f,%f\n", spaceship_coord.x, spaceship_coord.y, spaceship_coord.z);
	printf("Spaceship Velocity Vector: %f,%f,%f\n", velocityVector.x, velocityVector.y, velocityVector.z);
	printf("Spaceship Speed: %f\n", spaceshipSpeed);
	printf("Space Station Angular Speed: %f\n", angularSpeed);
}

void mouseFunc(int btn, int state, int x, int y)
{
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		PlaySound(NULL, 0, 0);
		if (!isPaused) isPaused = true;
		else
		{
			debugInformation();
			framePerMove = true;
		}

	}
	if (state == GLUT_UP)
	{
		framePerMove = false;
	}
}

void moveSpaceship(int id)
{
	if (!isPaused || framePerMove) {
		spaceship_coord += velocityVector*spaceshipSpeed;
		if (ViewMode != ViewMode4 && ViewMode != ViewMode2) {
			eye += spaceshipSpeed * velocityVector;
			at += spaceshipSpeed * velocityVector;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(200, moveSpaceship, 0);
}

void rotateSpaceStation(int id)
{
	if (rotatingDegreeSpaceStation > 360) rotatingDegreeSpaceStation = 0;
	if (!isPaused || framePerMove) {
		rotatingDegreeSpaceStation += angularSpeed;

		if (ViewMode == ViewMode2) {

			mat4 rY = mat4(cos(-rotatingDegreeSpaceStation * DegreesToRadians), 0.0, sin(-rotatingDegreeSpaceStation * DegreesToRadians), 0.0,
				0.0, 1.0, 0.0, 0.0,
				-sin(-rotatingDegreeSpaceStation * DegreesToRadians), 0.0, cos(-rotatingDegreeSpaceStation * DegreesToRadians), 0.0,
				0.0, 0.0, 0.0, 1.0);
			point4 rotatingEye = rY * point4(-0.4, 0.4, -10.0, 1.0);
			point4 rotatingAt = rY * point4(0.0, 0.0, -1, 1.0);

			eye.z = station_coord.z - 5; // 5 equals to r of space station
			eye.y = station_coord.y;
			eye.x = station_coord.x;
			at.z = eye.z - 1;
			at.y = station_coord.y;
			at.x = station_coord.x;

			eye.x += rotatingEye.x;
			eye.y += rotatingEye.y;
			eye.z += rotatingEye.z;
			at = eye;
			at.x += rotatingAt.x;
			at.y += rotatingAt.y;
			at.z += rotatingAt.z;

		}
	}

	glutPostRedisplay();
	glutTimerFunc(75, rotateSpaceStation, 1);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Space Adventure 3D");
	glewExperimental = GL_TRUE;
	glewInit();
	init();
	glutMouseFunc(mouseFunc);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyInputFunc);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, moveSpaceship, 0);
	glutTimerFunc(10, rotateSpaceStation, 1);
	glutMainLoop();
	return 0;
}