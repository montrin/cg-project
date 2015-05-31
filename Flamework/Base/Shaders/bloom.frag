uniform sampler2D uTexSource1;

uniform sampler2D uTexSource2;

varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
varying mediump vec3 normalVarying;     // normal in world space
varying mediump vec3 tangentVarying;    // tangent in world space


void main()
{
    lowp vec4 color = texture2D(uTexSource2, texCoordVarying.st);
    
    lowp vec4 sourceColor = vec4(1.0,1.0,0.0,0.5);
    
    gl_FragColor =  color;//*sourceColor;
    
}
