#include <iostream>
#include "Angel.h"
#include <vector>
#include <Math.h>

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
GLfloat spaceshipSpeed = 2.0f;
vec4 velocityVector(0.0, 0.0, -1.0, 0.0);

GLfloat rotateAngularSpeed = 2.0f;

GLfloat rotatingDegreeSpaceStation = 0.0;

vector<point4> points;
vector<point4> normals;
vector<color4> ambient_products;
vector<color4> diffuse_products;
vector<color4> specular_products;

GLint planetIndex = 0;
GLint torusIndex = 0;
GLint tetrahedronIndex = 0;
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
// Create a vertex array object
GLuint vao;
GLuint translatePlanetIndex;
GLuint  thetaIndex;
GLfloat  ThetaValue[3] = { 0.0, 0.0, 0.0 };
GLfloat  DefaultThetaValue[3] = { 0.0, 0.0, 0.0 };


//TRIANGLE_FAN
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

}
//TODO Remark station's front
//TRIANGLE_FAN
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

void pushTetraHedronColor() {
	ambient_products.push_back(light_ambient*vec4(1.0, 0.0, 0.0, 1.0));
	diffuse_products.push_back(light_diffuse*vec4(0.9, 0.9, 0.7, 1.0));
	specular_products.push_back(light_specular*vec4(0.0215, 0.1745, 0.0215, 1.0));
}

void triangle(const point4 &a, const point4 &b, const point4 &c) {
	points.push_back(a);
	pushTetraHedronColor();
	vec4 norm = normalize(cross(b - a, c - a));
	norm.w = 0.0;
	normals.push_back(norm);
	tetrahedronIndex++;
	points.push_back(b);
	norm = normalize(cross(a - c, b - c));
	norm.w = 0.0;
	normals.push_back(norm);
	pushTetraHedronColor();
	tetrahedronIndex++;
	points.push_back(c);
	tetrahedronIndex++;
	/*
	norm = normalize(cross(a - b, c - b));
	norm.w = 0.0;
	normals.push_back(norm);
	pushTetraHedronColor();
	*/
}

//TRIANGLE_STRIP
void drawTetraHedron() {
	triangle(point4(-0.1, 0.0, -2.2, 1.0),point4(0.0, 0.0, -1.7, 1.0), point4(0.1, 0.0, -2.2, 1.0));
	triangle(point4(0.1, 0.0, -2.2, 1.0), point4(0.0, 0.2, -1.9, 1.0), point4(0.0, 0.0, -1.7, 1.0));
	triangle(point4(0.0, 0.2, -1.9, 1.0), point4(-0.1, 0.0, -2.2, 1.0), point4(0.0, 0.0, -1.7, 1.0));
	triangle(point4(0.0, 0.2, -1.9, 1.0), point4(-0.1, 0.0, -2.2, 1.0), point4(0.1, 0.0, -2.2, 1.0));
}

void drawPlanets() {
	///GREEN
	drawPlanet(3, ambient_color_catalogue[0], diffuse_colors_catalogue[0], specular_colors_catalogue[0]);
	///ORANGE
	drawPlanet(3, ambient_color_catalogue[7], diffuse_colors_catalogue[3], specular_colors_catalogue[3]);
	///PINK
	drawPlanet(3, ambient_color_catalogue[2], diffuse_colors_catalogue[7], specular_colors_catalogue[1]);
	///YELLOW
	drawPlanet(3, ambient_color_catalogue[6], diffuse_colors_catalogue[5], specular_colors_catalogue[4]);
	///LIGHT BLUE
	drawPlanet(3, ambient_color_catalogue[0], diffuse_colors_catalogue[6], specular_colors_catalogue[5]);
	///BLUE
	drawPlanet(3, ambient_color_catalogue[0], diffuse_colors_catalogue[4], specular_colors_catalogue[5]);
	///RED
	drawPlanet(3, ambient_color_catalogue[1], diffuse_colors_catalogue[2], specular_colors_catalogue[0]);
	///PURPLE
	drawPlanet(3, ambient_color_catalogue[1], diffuse_colors_catalogue[7], specular_colors_catalogue[1]);
}

void drawSpaceship() {
	drawTorusY();
	drawTorusZ();
	drawTetraHedron();
}

void init() {
	drawPlanets();
	drawSpaceStation(4.0, ambient_color_catalogue[7], diffuse_colors_catalogue[1], specular_colors_catalogue[1]);
	drawSpaceship();

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
	translatePlanetIndex = glGetUniformLocation(program, "TranslatePlanets");
	thetaIndex = glGetUniformLocation(program, "Theta");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);
	mat4 model_view = LookAt(eye, at, up);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);

	glUniform3fv(thetaIndex, 1, DefaultThetaValue);
	glUniform4fv(translatePlanetIndex, 1, planet_coords[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, planetIndex / 8);
	glUniform4fv(translatePlanetIndex, 1, planet_coords[1]);
	glDrawArrays(GL_TRIANGLE_FAN, planetIndex / 8, planetIndex / 8);
	glUniform4fv(translatePlanetIndex, 1, planet_coords[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 2 * planetIndex / 8, planetIndex / 8);
	glUniform4fv(translatePlanetIndex, 1, planet_coords[3]);
	glDrawArrays(GL_TRIANGLE_FAN, 3 * planetIndex / 8, planetIndex / 8);
	glUniform4fv(translatePlanetIndex, 1, planet_coords[4]);
	glDrawArrays(GL_TRIANGLE_FAN, 4 * planetIndex / 8, planetIndex / 8);
	glUniform4fv(translatePlanetIndex, 1, planet_coords[5]);
	glDrawArrays(GL_TRIANGLE_FAN, 5 * planetIndex / 8, planetIndex / 8);
	glUniform4fv(translatePlanetIndex, 1, planet_coords[6]);
	glDrawArrays(GL_TRIANGLE_FAN, 6 * planetIndex / 8, planetIndex / 8);
	glUniform4fv(translatePlanetIndex, 1, planet_coords[7]);
	glDrawArrays(GL_TRIANGLE_FAN, 7 * planetIndex / 8, planetIndex / 8);

	glUniform3fv(thetaIndex, 1, ThetaValue);
	glUniform4fv(translatePlanetIndex, 1, station_coord);
	glDrawArrays(GL_TRIANGLE_FAN, planetIndex, spacestationIndex);


	glUniform3fv(thetaIndex, 1, DefaultThetaValue);
	glUniform4fv(translatePlanetIndex, 1, spaceship_coord);
	glDrawArrays(GL_TRIANGLE_STRIP, planetIndex + spacestationIndex, torusIndex);
	glUniform4fv(translatePlanetIndex, 1, spaceship_coord);
	glDrawArrays(GL_TRIANGLE_STRIP, planetIndex + spacestationIndex + torusIndex, torusIndex);

	//TODO Only Z rotation and translate not cause distortion, why?
	glUniform3fv(thetaIndex, 1, DefaultThetaValue);
	//point4 sa(spaceship_coord.x, spaceship_coord.y, spaceship_coord.z, spaceship_coord.w);
	glUniform4fv(translatePlanetIndex, 1, spaceship_coord);
	glDrawArrays(GL_TRIANGLE_STRIP, planetIndex + spacestationIndex +  2 * torusIndex, tetrahedronIndex);
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
	glViewport(0, 0, w, h);
	GLfloat aspect = GLfloat(w) / h;
	mat4 projection = Perspective(90.0, aspect, 0.5, 500.0);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'j': case 'J':
		rotateAngularSpeed += 2.0;
		if (rotateAngularSpeed >= 20) rotateAngularSpeed = 20; //Clamping
		break;
	case 'k': case 'K':
		rotateAngularSpeed -= 2.0;
		if (rotateAngularSpeed <= 0) rotateAngularSpeed = 2; //Clamping
		break;
	case 'a': case 'A':
		spaceshipSpeed -= 0.25;
		break;
	case 'd': case 'D':
		spaceshipSpeed += 0.25;
		break;
	case 'c': case 'C':
		ViewMode = ViewMode1;
		eye.y = spaceship_coord.y;
		eye.z = spaceship_coord.z - 1.0;
		at.y = spaceship_coord.y;
		at.z = eye.z - 1;
		break;
	case 's': case 'S':
		ViewMode = ViewMode2;
		break;
	case 't': case 'T':
		ViewMode = ViewMode3;
		eye.y = spaceship_coord.y + 2.0;
		eye.z = spaceship_coord.z + 4;
		at.y = spaceship_coord.y + 2.0;
		at.z = eye.z - 1;
		break;
	case 'w': case 'W':
		ViewMode = ViewMode4;
		eye.y = LARGE_Y;
		eye.z = -200;
		break;
	}
	glutPostRedisplay();
}

void moveSpaceship(int id)
{
	spaceship_coord[2] += spaceshipSpeed * -1;
	if (ViewMode != ViewMode4) {
		eye.z += spaceshipSpeed * -1;
		at.z += spaceshipSpeed * -1;
	}

	glutPostRedisplay();
	glutTimerFunc(200, moveSpaceship, 0);
}

void rotateSpaceStation(int id)
{
	if (rotatingDegreeSpaceStation > 360) rotatingDegreeSpaceStation = 0;

	//Rotate
	ThetaValue[0] = 0.0;
	ThetaValue[1] = rotatingDegreeSpaceStation;
	ThetaValue[2] = 0.0;
	rotatingDegreeSpaceStation += rotateAngularSpeed;

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
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, moveSpaceship, 0);
	glutTimerFunc(10, rotateSpaceStation, 1);
	glutMainLoop();
	return 0;
}