#include <iostream>
#include "Angel.h"
#include <vector>
#include <Math.h>

using namespace Angel;
using namespace std;
typedef vec4 point4;
typedef vec4 color4;
const GLint width = 500;
const GLint height = 500;
vector<point4> sphere;
vector<point4> normal;
point4 color = { 1.0,0.0,0.0,1.0 };
const double PI = 3.141592653589793238463;

point4 light_position(2.0, 3.0, 2.0, 1.0); //wrt camera
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

vector<point4> ambient_products;
vector<point4> diffuse_products;
vector<point4> specular_products;

GLint ModelView, Projection;

void pushColor() {
	ambient_products.push_back(light_ambient*vec4(1.0, 0.0, 0.0, 1.0));
	diffuse_products.push_back(light_diffuse*vec4(0.9, 0.9, 0.7, 1.0));
	specular_products.push_back(light_specular*vec4(0.0215, 0.1745, 0.0215, 1.0));
}

//TRIANGLE_FAN
void fillSpherePoints() {
	/*
	for (int i = 0; i < 360; i = i + 5) {
		int k = i;
		for (int j = 0; j < 180; j = j + 1) {
			k += 30;
			float lat = PI / 180 * k;
			float lon = PI / 180 * j;
			float x = cos(lat)*sin(lon);
			float y = cos(lon);
			float z = sin(lon)*sin(lat);
			point4 point = { x,y,z,1.0 };
			vec3 normalized = normalize(vec3(x, y, z));
			normal.push_back(vec4(normalized, 0.0));
			ambient_products.push_back(light_ambient*vec4(1.0, 0.0, 0.0, 1.0));
			diffuse_products.push_back(light_diffuse*vec4(1.0, 0.61424, 0.07568, 1.0));
			specular_products.push_back(light_specular*vec4(0.0215, 0.1745, 0.0215, 1.0));
			sphere.push_back(point);
		}
	}
	*/
	/*
	float sectorCount = 100;
	float stackCount = 100;
	float radius = 1.0f;
	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);	// r * cos(u);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			float y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			point4 point = { x,y,z,1.0 };
			sphere.push_back(point);
			vec3 normalized = normalize(vec3(x, y, z));
			normal.push_back(vec4(normalized, 0.0));
			ambient_products.push_back(light_ambient*vec4(1.0, 0.0, 0.0, 1.0));
			diffuse_products.push_back(light_diffuse*vec4(1.0, 0.61424, 0.07568, 1.0));
			specular_products.push_back(light_specular*vec4(0.0215, 0.1745, 0.0215, 1.0));
		}
	}
	*/


}
//TRIANGLE_STRIP
void drawTorus() {

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
				float sz = cos(outerAngle*twopi / numc);				/* normal is cross-product of tangents */
				float nx = tangentY * sz - tangentZ * sy;
				float ny = tangentZ * sx - tangentX * sz;
				float nz = tangentX * sy - tangentY * sx;


				point4 point = { x,y,z,1.0 };
				sphere.push_back(point);
				vec3 normalized = normalize(vec3(nx, ny, nz));
				normal.push_back(vec4(normalized, 0.0));
				ambient_products.push_back(light_ambient*vec4(1.0, 0.0, 0.0, 1.0));
				diffuse_products.push_back(light_diffuse*vec4(1.0, 0.61424, 0.07568, 1.0));
				specular_products.push_back(light_specular*vec4(0.0215, 0.1745, 0.0215, 1.0));
			}
		}
	}

}

void drawTorus2() {

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
				float sz = cos(outerAngle*twopi / numc);				/* normal is cross-product of tangents */
				float nx = tangentY * sz - tangentZ * sy;
				float ny = tangentZ * sx - tangentX * sz;
				float nz = tangentX * sy - tangentY * sx;				/*
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
				sphere.push_back(point);
				vec4 normalized = normalize(rY * vec4(nx, ny, nz, 0.0));
				normal.push_back(normalized);
				ambient_products.push_back(light_ambient*vec4(1.0, 0.0, 0.0, 1.0));
				diffuse_products.push_back(light_diffuse*vec4(1.0, 0.61424, 0.07568, 1.0));
				specular_products.push_back(light_specular*vec4(0.0215, 0.1745, 0.0215, 1.0));
			}
		}
	}

}

void triangle(const point4 &a, const point4 &b, const point4 &c) {
	sphere.push_back(a);
	pushColor();
	vec4 norm = normalize(cross(b - a, c - a));
	norm.w = 0.0;
	normal.push_back(norm);
	sphere.push_back(b);
	norm = normalize(cross(a - c, b - c));
	norm.w = 0.0;
	normal.push_back(norm);
	pushColor();
	sphere.push_back(c);
	norm = normalize(cross(a - b, c - b));
	norm.w = 0.0;
	normal.push_back(norm);
	pushColor();
}
//TRIANGLE_STRIP
void drawTetraHedron() {
	triangle(point4(-0.4 / 3, 0.0 / 3, 1.3, 1.0), point4(0.0, 0.0, 1.8, 1.0), point4(0.4 / 3, 0.0, 1.3, 1.0));
	triangle(point4(0.4 / 3, 0.0 / 3, 1.3, 1.0), point4(0.1 / 3, 0.5 / 3, 1.6, 1.0), point4(0.0, 0.0, 1.8, 1.0));
	triangle(point4(0.1 / 3, 0.5 / 3, 1.6, 1.0), point4(-0.4 / 3, 0.0, 1.3, 1.0), point4(0.0, 0.0, 1.8, 1.0));
	triangle(point4(0.1 / 3, 0.5 / 3, 1.6, 1.0), point4(-0.4 / 3, 0.0, 1.3, 1.0), point4(0.4 / 3, 0.0, 1.3, 1.0));
}

void init() {
	drawTorus();
	drawTetraHedron();
	drawTorus2();
	//fillSpherePoints();
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*sphere.size() + sizeof(point4)*normal.size() + sizeof(point4)*ambient_products.size()
		+ sizeof(point4)*diffuse_products.size() + sizeof(point4)*specular_products.size(), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*sphere.size(), sphere.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*sphere.size(), sizeof(point4)*normal.size(), normal.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*sphere.size() + sizeof(point4)*normal.size(), sizeof(point4)*ambient_products.size(), ambient_products.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*sphere.size() + sizeof(point4)*normal.size() + sizeof(point4)*ambient_products.size()
		, sizeof(point4)*diffuse_products.size(), diffuse_products.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*sphere.size() + sizeof(point4)*normal.size() + sizeof(point4)*ambient_products.size()
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
		BUFFER_OFFSET(sizeof(point4)*sphere.size()));

	GLuint AmbientProduct = glGetAttribLocation(program, "AmbientProduct");
	glEnableVertexAttribArray(AmbientProduct);
	glVertexAttribPointer(AmbientProduct, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4)*sphere.size() + sizeof(point4)*normal.size()));

	GLuint DiffuseProduct = glGetAttribLocation(program, "DiffuseProduct");
	glEnableVertexAttribArray(DiffuseProduct);
	glVertexAttribPointer(DiffuseProduct, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4)*sphere.size() + sizeof(point4)*normal.size() + sizeof(point4)*ambient_products.size()));

	GLuint SpecularProduct = glGetAttribLocation(program, "SpecularProduct");
	glEnableVertexAttribArray(SpecularProduct);
	glVertexAttribPointer(SpecularProduct, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(point4)*sphere.size() + sizeof(point4)*normal.size() + sizeof(point4)*ambient_products.size() + sizeof(point4)*diffuse_products.size()));

	float material_shininess = 960.8;
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);

	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	point4 at(0.0, 0.0, 0.0, 1.0);
	point4 eye(0.0, 0.0, 3.0, 1.0);
	vec4 up(0.0, 1.0, 0.0, 0.0);

	mat4 model_view = LookAt(eye, at, up);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere.size());
	glutSwapBuffers();
}
void myReshape(GLsizei w, GLsizei h) {
	glViewport(0, 0, w, h);
	GLfloat aspect = GLfloat(w) / h;
	mat4 projection = Perspective(60.0, aspect, 0.5, 20.0);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}
int main(int argc, char **argv) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Sphere");
	glewExperimental = GL_TRUE;
	glewInit();
	init();
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutMainLoop();
	return 0;
}