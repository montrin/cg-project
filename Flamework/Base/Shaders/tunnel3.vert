//
//uniform mat4 ModelViewMatrix;
//uniform mat4 ViewMatrix;
//uniform mat4 ProjectionMatrix;
//
//attribute vec4 Position;
//attribute vec4 Normal;
//attribute vec4 TexCoord;
//
//// TODO: create varying variable to hand over color values to the fragment shader
//
//void main()
//{
//    vec4 pos = ModelViewMatrix * Position;  // vertex position in eye coordinates
//
//    // TODO: uncomment to assign normal value to color varying variable
//    // colorVarying = vec4(vec3(0.5) + Normal.xyz * 0.5, 1.0);
//
//    gl_Position = ProjectionMatrix * pos;
//}


uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mediump mat4 ProjectionMatrix;

uniform mediump mat3 NormalMatrix;

uniform mediump vec4 LightPos;
uniform mediump vec4 EyePos;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform lowp vec3 Ia;   // ambient light intensity
uniform lowp vec3 Id;   // diffuse light intensity
uniform lowp vec3 Is;   // specular light intensity

attribute vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

varying lowp vec4 ambientVarying;
varying lowp vec4 diffuseVarying;
varying lowp vec4 specularVarying;
varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;       // pos in world space
varying mediump vec3 normalVarying;    // normal in world space

void main()
{
    posVarying = ModelMatrix * Position;
    normalVarying = NormalMatrix * Normal;
    ambientVarying = vec4(Ka * Ia, 1.0);
    texCoordVarying = TexCoord;
    
    
    gl_Position = ProjectionMatrix * ViewMatrix * posVarying;
}
