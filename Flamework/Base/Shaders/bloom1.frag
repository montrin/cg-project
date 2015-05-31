
uniform sampler2D uTexSource1;

varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
varying mediump vec3 normalVarying;     // normal in world space
varying mediump vec3 tangentVarying;    // tangent in world space


void main()
{
    lowp vec4 scolor = vec4(1.0,1.0,0.0,1.0);
    gl_FragColor =  scolor;//*sourceColor;
    
}
