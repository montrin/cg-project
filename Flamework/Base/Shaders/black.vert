uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mediump mat4 ProjectionMatrix;

attribute vec4 Position;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * (ModelMatrix * Position);
}
