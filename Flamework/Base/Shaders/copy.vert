uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mediump mat4 ProjectionMatrix;

uniform mediump mat3 NormalMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;

varying lowp vec4 texCoordVarying;
varying mediump vec4 posVarying;        // pos in world space

void main()
{
    posVarying = ModelMatrix * Position;
    texCoordVarying = TexCoord;
    
    gl_Position = posVarying;
}
