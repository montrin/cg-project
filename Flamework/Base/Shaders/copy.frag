uniform sampler2D texAfterBloom;

varying lowp vec4 texCoordVarying;

void main()
{
    lowp vec4 color = texture2D(texAfterBloom, texCoordVarying.st);
    gl_FragColor = color;
}
