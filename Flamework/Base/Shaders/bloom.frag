
uniform sampler2D uTexSource1;

varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
//varying mediump vec3 normalVarying;     // normal in world space
//varying mediump vec3 tangentVarying;    // tangent in world space


void main()
{
    lowp vec4 color = texture2D(uTexSource1, texCoordVarying.st);
    if(color.r<=0.3 && color.g<=0.3 && color.b<=0.3){
        color = vec4(0.0,0.0,0.0,0.0);
    }else{
        color = vec4(1.0,1.0,1.0,0.0);
    }
    
    gl_FragColor =  color;//*sourceColor;
    
}
