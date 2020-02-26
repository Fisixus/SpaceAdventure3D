// fragment shading of sphere model

#include "Angel.h"

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

enum { ViewMode1 = 0, ViewMode2 = 1, ViewMode3 = 2, ViewMode4 = 3};

int ViewMode = ViewMode1;

GLuint ww = 512;
GLuint wh = 512;

const int NumTimesToSubdivide = 5; //7; //5;
const int NumTriangles = 4096; //4 * 4 * 4096;  // (4 faces)^(NumTimesToSubdivide + 1)
const int NumVertices = 3 * NumTriangles;

point4 light_position(2.0, 3.0, 2.0, 1.0); //wrt camera
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

typedef struct {
	point4 location;
	color4 material_ambient;
	color4 material_diffuse;
	color4 material_specular;
	float radius;
} Planet;

point4 points[NumVertices];
vec3   normals[NumVertices];
point4 ambient_products[NumVertices];
point4 diffuse_products[NumVertices];
point4 specular_products[NumVertices];


// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

// Create a vertex array object
GLuint vao;

GLfloat planet_coords[][3] = { {30, 30, 30},
{30, 170, 15},
{80, 110, 25},
{70, 60, 12},
{90, 150, 13},
{120, 80, 17},
{150, 40, 15},
{160, 170, 22} };


color4 ambient_colors[8] = { {0.633, 0.727811, 0.633, 1.0},
{0.30, 0.30, 0.30, 1.0},
{1.00, 0.00, 0.00, 1.0},
{0.00, 1.00, 0.00, 1.0},
{0.00, 0.00, 1.00, 1.0},
{1.00, 1.00, 0.00, 1.0},
//{1.00, 0.00, 1.00, 1.0},
{0.00, 1.00, 1.00, 1.0},
{1.00, 1.00, 1.00, 1.0} };

color4 diffuse_colors[8] = { {0.07568, 0.61424, 0.07568, 1.0},
{0.30, 0.30, 0.30, 1.0},
{1.00, 0.00, 0.00, 1.0},
{0.00, 1.00, 0.00, 1.0},
{0.00, 0.00, 1.00, 1.0},
{1.00, 1.00, 0.00, 1.0},
//{1.00, 0.00, 1.00, 1.0},
{0.00, 1.00, 1.00, 1.0},
{1.00, 0.50, 1.00, 1.0} };

color4 specular_colors[8] = { {0.0215, 0.1745, 0.0215, 1.0},
{0.30, 0.30, 0.30, 1.0},
{1.00, 0.00, 0.00, 1.0},
{0.00, 1.00, 0.00, 1.0},
{0.00, 0.00, 1.00, 1.0},
{1.00, 1.00, 0.00, 1.0},
//{1.00, 0.00, 1.00, 1.0},
{0.00, 1.00, 1.00, 1.0},
{1.00, 1.00, 1.00, 1.0} };

//----------------------------------------------------------------------------

int Index = 0;

void triangle(const point4& a, const point4& b, const point4& c)
{
	/* flat */
   /* vec3  normal = normalize( cross(b - a, c - b) );

	 normals[Index] = normal;  points[Index] = a;  Index++;
	 normals[Index] = normal;  points[Index] = b;  Index++;
	 normals[Index] = normal;  points[Index] = c;  Index++;
   */
   /*Gauroud if vshader53 & fshader53 used*/
   /* Phong if vshader56 & fshader56 used */
	normals[Index] = vec3(a.x, a.y, a.z);  
	points[Index] = a; 
	ambient_products[Index] = light_ambient * ambient_colors[0];
	diffuse_products[Index] = light_diffuse * diffuse_colors[0];
	specular_products[Index] = light_specular * specular_colors[0];
	Index++;

	normals[Index] = vec3(b.x, b.y, b.z);  
	points[Index] = b;  
	ambient_products[Index] = light_ambient * ambient_colors[0];
	diffuse_products[Index] = light_diffuse * diffuse_colors[0];
	specular_products[Index] = light_specular * specular_colors[0];
	Index++;

	normals[Index] = vec3(c.x, c.y, c.z);  
	points[Index] = c;
	ambient_products[Index] = light_ambient * ambient_colors[0];
	diffuse_products[Index] = light_diffuse * diffuse_colors[0];
	specular_products[Index] = light_specular * specular_colors[0];
	Index++;


}

//----------------------------------------------------------------------------

point4 unit(const point4& p)
{
	float len = p.x*p.x + p.y*p.y + p.z*p.z;

	point4 t;
	if (len > DivideByZeroTolerance) {
		t = p / sqrt(len);
		t.w = 1.0;
	}

	return t;
}

void divide_triangle(const point4& a, const point4& b,
	const point4& c, int count)
{
	if (count > 0) {
		point4 v1 = unit(a + b);
		point4 v2 = unit(a + c);
		point4 v3 = unit(b + c);
		divide_triangle(a, v1, v2, count - 1);
		divide_triangle(c, v2, v3, count - 1);
		divide_triangle(b, v3, v1, count - 1);
		divide_triangle(v1, v3, v2, count - 1);
	}
	else {
		triangle(a, b, c);
	}
}

void tetrahedron(int count)
{
	point4 v[4] = {
	vec4(0.0, 0.0, 1.0, 1.0),
	vec4(0.0, 0.942809, -0.333333, 1.0),
	vec4(-0.816497, -0.471405, -0.333333, 1.0),
	vec4(0.816497, -0.471405, -0.333333, 1.0)
	};

	divide_triangle(v[0], v[1], v[2], count);
	divide_triangle(v[3], v[2], v[1], count);
	divide_triangle(v[0], v[3], v[1], count);
	divide_triangle(v[0], v[2], v[3], count);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void init()
{
	// Subdivide a tetrahedron into a sphere
	tetrahedron(NumTimesToSubdivide);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) + sizeof(ambient_products) + sizeof(diffuse_products) + sizeof(specular_products),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points),
		sizeof(normals), normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals),
		sizeof(ambient_products), ambient_products);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) +sizeof(ambient_products),
		sizeof(diffuse_products), diffuse_products);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) + sizeof(ambient_products) + sizeof(diffuse_products),
		sizeof(specular_products), specular_products);

	// Load shaders and use the resulting shader program
   //GLuint program = InitShader( "example6_vs.glsl", "example6_fs.glsl" );
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	GLuint AmbientProduct = glGetAttribLocation(program, "AmbientProduct");
	glEnableVertexAttribArray(AmbientProduct);
	glVertexAttribPointer(AmbientProduct, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points) + sizeof(normals)));

	GLuint DiffuseProduct = glGetAttribLocation(program, "DiffuseProduct");
	glEnableVertexAttribArray(DiffuseProduct);
	glVertexAttribPointer(DiffuseProduct, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points) + sizeof(normals) + sizeof(ambient_products)));

	GLuint SpecularProduct = glGetAttribLocation(program, "SpecularProduct");
	glEnableVertexAttribArray(SpecularProduct);
	glVertexAttribPointer(SpecularProduct, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points) + sizeof(normals) + sizeof(ambient_products) + sizeof(diffuse_products)));
	// Initialize shader lighting parameters
  //  point4 light_position( 0.0, 0.0, 2.0, 0.0 );

	/*

	//orange
    color4 material_ambient( 1.0, 1.0, 1.0, 1.0 );
	color4 material_diffuse( 1.0, 0.5, 0.0, 1.0 );
	color4 material_specular( 1.0, 1.0, 1.0, 1.0 );


	//emerald
	color4 material_ambient(0.633, 0.727811, 0.633, 1.0);
	color4 material_diffuse(0.07568, 0.61424, 0.07568, 1.0);
	color4 material_specular(0.0215, 0.1745, 0.0215, 1.0);
	
	//	vec3 Ks = vec3(0.0215, 0.1745, 0.0215); 
	//vec3 Kd = vec3(0.07568, 0.61424, 0.07568); 
	//vec3 Ka = vec3(0.633, 0.727811, 0.633); 
	//float specular_exponent = 76.8; 


	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);


	*/
	float  material_shininess = 960.8;
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0); /* black background */
}

//----------------------------------------------------------------------------

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	point4 at(0.0, 0.0, 0.0, 1.0);
	point4 eye(0.0, 0.0, 3.0, 1.0);
	vec4   up(0.0, 1.0, 0.0, 0.0);

	mat4 model_view = LookAt(eye, at, up);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------

void reshape(GLsizei  w, GLsizei  h)
{
	glViewport(0, 0, w, h);

	GLfloat aspect = GLfloat(w) / h;

	mat4  projection = Perspective(60.0, aspect, 0.5, 20.0);

	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);


}

//----------------------------------------------------------------------------

int main(int argc, char **argv)
{

	glutInit(&argc, argv);

#ifdef __APPLE__
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
#endif

	glutInitWindowSize(ww, wh);
	//    glutInitContextVersion( 3, 2 );
	//    glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow("Space Adventure 3D");
	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}
