uniform sampler2D uTexSource1;

varying lowp vec4 texCoordVarying;

void main()
{
    lowp vec4 color = texture2D(uTexSource1, texCoordVarying.st);
    gl_FragColor = color;
}
