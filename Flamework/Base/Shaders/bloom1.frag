
uniform sampler2D uTexSource1;

varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
//varying mediump vec3 normalVarying;     // normal in world space
//varying mediump vec3 tangentVarying;    // tangent in world space


void main()
{
//    lowp vec2 xy = texCoordVarying.st;
//    xy.x = texCoordVarying.x*4.0;
//    xy.y = texCoordVarying.y*4.0;
    lowp vec4 color = texture2D(uTexSource1, texCoordVarying.st);
    
    if(color.r<=0.3 && color.g<=0.3 && color.b<=0.3){
        color = vec4(0.0,0.0,0.0,1.0);
    }else{
        color = color + vec4(vec3(0.3),1.0);
    }
    
    gl_FragColor =  color;//*sourceColor;
    
}
