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

const GLint width = 512;
const GLint height = 512;
const GLfloat material_shininess = 1000.0f;

point4 light_position(2.0, 3.0, 2.0, 1.0); //wrt camera
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

//For planets and space station
GLint sectorCount = 100;
GLint stackCount = 100;

vector<point4> locations;
vector<point4> points;
vector<point4> normals;
vector<color4> ambient_products;
vector<color4> diffuse_products;
vector<color4> specular_products;

GLint planetIndex = 0;
GLint torusIndex = 0;
GLint tetrahedronIndex = 0;

point4 planet_coords[10] = { {0, 0, -5, 1},
{2, 3, -5, 1},
{30, 30, 30, 1},
{30, 170, 15, 1},
{80, 110, 25, 1},
{70, 60, 12, 1},
{90, 150, 13, 1},
{120, 80, 17, 1},
{150, 40, 15, 1},
{160, 170, 22, 1} };

color4 ambient_color_catalogue[8] = { {0.633, 0.727811, 0.633, 1.0},
{0.30, 0.30, 0.30, 1.0},
{1.00, 0.00, 0.00, 1.0},
{0.00, 1.00, 0.00, 1.0},
{0.00, 0.00, 1.00, 1.0},
{1.00, 1.00, 0.00, 1.0},
//{1.00, 0.00, 1.00, 1.0},
{0.00, 1.00, 1.00, 1.0},
{1.00, 1.00, 1.00, 1.0} };

color4 diffuse_colors_catalogue[8] = { {0.07568, 0.61424, 0.07568, 1.0},
{0.30, 0.30, 0.30, 1.0},
{1.00, 0.00, 0.00, 1.0},
{0.00, 1.00, 0.00, 1.0},
{0.00, 0.00, 1.00, 1.0},
{1.00, 1.00, 0.00, 1.0},
//{1.00, 0.00, 1.00, 1.0},
{0.00, 1.00, 1.00, 1.0},
{1.00, 0.50, 1.00, 1.0} };

color4 specular_colors_catalogue[8] = { {0.0215, 0.1745, 0.0215, 1.0},
{0.30, 0.30, 0.30, 1.0},
{1.00, 0.00, 0.00, 1.0},
{0.00, 1.00, 0.00, 1.0},
{0.00, 0.00, 1.00, 1.0},
{1.00, 1.00, 0.00, 1.0},
//{1.00, 0.00, 1.00, 1.0},
{0.00, 1.00, 1.00, 1.0},
{1.00, 1.00, 1.00, 1.0} };

GLint ModelView, Projection;



//TRIANGLE_FAN
void drawPlanet(GLfloat radius, const point4 &location, const color4 &ambient_color,
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
			locations.push_back(location);
			planetIndex++;
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
				/*
				mat4 rX = mat4(1.0, 0.0, 0.0, 0.0,
					0.0, cos(90*PI/180), -sin(90 * PI / 180), 0.0,
					0.0, sin(90 * PI / 180), cos(90 * PI / 180), 0.0,
					0.0, 0.0, 0.0, 1.0);

				mat4 rZ = mat4(cos(90 * PI / 180), -sin(90 * PI / 180), 0.0, 0.0,
					sin(90 * PI / 180), cos(90 * PI / 180), 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0);
*/
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

void pushColor() {
	ambient_products.push_back(light_ambient*vec4(1.0, 0.0, 0.0, 1.0));
	diffuse_products.push_back(light_diffuse*vec4(0.9, 0.9, 0.7, 1.0));
	specular_products.push_back(light_specular*vec4(0.0215, 0.1745, 0.0215, 1.0));
}

void triangle(const point4 &a, const point4 &b, const point4 &c) {
	points.push_back(a);
	pushColor();
	vec4 norm = normalize(cross(b - a, c - a));
	norm.w = 0.0;
	normals.push_back(norm);
	tetrahedronIndex++;
	points.push_back(b);
	norm = normalize(cross(a - c, b - c));
	norm.w = 0.0;
	normals.push_back(norm);
	pushColor();
	tetrahedronIndex++;
	points.push_back(c);
	norm = normalize(cross(a - b, c - b));
	norm.w = 0.0;
	normals.push_back(norm);
	pushColor();
	tetrahedronIndex++;
}

//TRIANGLE_STRIP
void drawTetraHedron() {
	triangle(point4(-0.4 / 3, 0.0 / 3, 1.3, 1.0), point4(0.0, 0.0, 1.8, 1.0), point4(0.4 / 3, 0.0, 1.3, 1.0));
	triangle(point4(0.4 / 3, 0.0 / 3, 1.3, 1.0), point4(0.1 / 3, 0.5 / 3, 1.6, 1.0), point4(0.0, 0.0, 1.8, 1.0));
	triangle(point4(0.1 / 3, 0.5 / 3, 1.6, 1.0), point4(-0.4 / 3, 0.0, 1.3, 1.0), point4(0.0, 0.0, 1.8, 1.0));
	triangle(point4(0.1 / 3, 0.5 / 3, 1.6, 1.0), point4(-0.4 / 3, 0.0, 1.3, 1.0), point4(0.4 / 3, 0.0, 1.3, 1.0));
}


void drawPlanets() {
	drawPlanet(1.0, planet_coords[0], ambient_color_catalogue[0], diffuse_colors_catalogue[0], specular_colors_catalogue[0]);
	drawPlanet(1.5, planet_coords[1], ambient_color_catalogue[1], diffuse_colors_catalogue[2], specular_colors_catalogue[0]);
}

void drawSpaceship() {
	drawTorusY();
	drawTorusZ();
	drawTetraHedron();
}

void init() {
	drawPlanets();
	drawSpaceship();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*points.size() + sizeof(point4)*normals.size() + sizeof(point4)*ambient_products.size()
		+ sizeof(point4)*diffuse_products.size() + sizeof(point4)*specular_products.size() + sizeof(point4) * locations.size(), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*points.size(), points.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*points.size(), sizeof(point4)*normals.size(), normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*points.size() + sizeof(point4)*normals.size(), sizeof(point4)*ambient_products.size(), ambient_products.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*points.size() + sizeof(point4)*normals.size() + sizeof(point4)*ambient_products.size()
		, sizeof(point4)*diffuse_products.size(), diffuse_products.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*points.size() + sizeof(point4)*normals.size() + sizeof(point4)*ambient_products.size()
		+ sizeof(point4)*diffuse_products.size(), sizeof(point4)*specular_products.size(), specular_products.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*points.size() + sizeof(point4)*normals.size() + sizeof(point4)*ambient_products.size()
		+ sizeof(point4)*diffuse_products.size() + sizeof(point4)*specular_products.size(), sizeof(point4)*locations.size(),locations.data());

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

	GLuint transformLoc = glGetAttribLocation(program, "transformLoc");
	glEnableVertexAttribArray(transformLoc);
	glVertexAttribPointer(transformLoc, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4)*points.size() + sizeof(point4)*normals.size() + sizeof(point4)*ambient_products.size() + sizeof(point4)*diffuse_products.size() + sizeof(point4)*specular_products.size()));
	
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);

	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	point4 at(0.0, 0.0, 0.0, 1.0);
	point4 eye(0.0, 0.0, 3.0, 1.0);
	vec4 up(0.0, 1.0, 0.0, 0.0);

	mat4 model_view = LookAt(eye, at, up);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glDrawArrays(GL_TRIANGLE_FAN, 0, planetIndex/2);
	glDrawArrays(GL_TRIANGLE_FAN, planetIndex / 2, planetIndex / 2);
	glDrawArrays(GL_TRIANGLE_STRIP, planetIndex, torusIndex);
	glDrawArrays(GL_TRIANGLE_STRIP, planetIndex + torusIndex, torusIndex);
	glDrawArrays(GL_TRIANGLE_STRIP, planetIndex + torusIndex + torusIndex, 12);

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
	glViewport(0, 0, w, h);
	GLfloat aspect = GLfloat(w) / h;
	mat4 projection = Perspective(60.0, aspect, 0.5, 20.0);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'c': case 'C':
		ViewMode = ViewMode1;
		break;
	case 's': case 'S':
		ViewMode = ViewMode2;
		break;
	case 't': case 'T':
		ViewMode = ViewMode3;
		break;
	case 'w': case 'W':
		ViewMode = ViewMode4;
		break;
	}
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
	glutMainLoop();
	return 0;
}