uniform mediump mat4 ModelMatrix;
uniform mediump vec4 LightPos;

attribute vec4 Position;


attribute vec4 TexCoord;

varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
varying mediump vec3 normalVarying;     // normal in world space
varying mediump vec3 tangentVarying;    // tangent in world space
varying mediump vec4 lightPosVarying;

void main()
{
    posVarying = ModelMatrix * Position;
    
    texCoordVarying = TexCoord;
    lightPosVarying = LightPos;
    
    gl_Position =   posVarying;
}