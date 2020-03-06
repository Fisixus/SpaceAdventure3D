#version 150 


in vec4 vPosition;
in vec4 vNormal;
in vec4 AmbientProduct;
in vec4 DiffuseProduct;
in vec4 SpecularProduct;
out vec4 color;

uniform vec3 Theta;
uniform vec4 TranslatePlanets;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform float Shininess;

void main()
{
    // Transform vertex  position into eye coordinates
    vec3 pos = (ModelView * vPosition).xyz;
	
    vec3 L = normalize( (LightPosition).xyz - pos ); //assuming light position provided in camera coordinates
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( ModelView* vNormal ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    vec3 angles = radians( Theta );
    vec3 c = cos( angles );
    vec3 s = sin( angles );

    mat4 rX = mat4( 1.0,  0.0,  0.0, 0.0,
		    0.0,  c.x,  s.x, 0.0,
		    0.0, -s.x,  c.x, 0.0,
		    0.0,  0.0,  0.0, 1.0 );

    mat4 rY = mat4( c.y, 0.0, -s.y, 0.0,
		    0.0, 1.0,  0.0, 0.0,
		    s.y, 0.0,  c.y, 0.0,
		    0.0, 0.0,  0.0, 1.0 );
			
	mat4 rZ = mat4( c.z, s.z, 0.0, 0.0,
		-s.z,  c.z, 0.0, 0.0,
		0.0,  0.0, 1.0, 0.0,
		0.0,  0.0, 0.0, 1.0 );

    mat4 tXYZ = mat4(1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    TranslatePlanets.x, TranslatePlanets.y, TranslatePlanets.z, 1.0);
    
    gl_Position = Projection * ModelView * tXYZ * rX * rY * rZ * vPosition;

    color = ambient + diffuse + specular;
    color.a = 1.0;
}
