
uniform mediump mat4 ModelMatrix;

attribute vec4 Position;

attribute vec4 TexCoord;

varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
//varying mediump vec3 normalVarying;     // normal in world space
//varying mediump vec3 tangentVarying;    // tangent in world space

void main()
{
    posVarying = ModelMatrix * Position;
    
    texCoordVarying = TexCoord;
    
    gl_Position =   posVarying;
}
