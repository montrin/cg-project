varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
uniform mediump vec4 LightPos;
uniform mediump vec4 LightDir;
uniform mediump vec4 EyePos;

void main()
{
    lowp vec4 color = vec4(1.0,1.0,1.0,0.8);
    mediump vec4 pos = posVarying;
    mediump vec3 eyeVec = normalize(EyePos.xyz - pos.xyz);
    
    //TODO: Adjust brightness / glow if distance between view and fragment gets smaller
//    if (eyeVec.z < (0.1)) {
//        color = vec4(1.0,0.5,0.5,1.0);
//    }
    gl_FragColor =  color;// + vec4(0.3);
    
}
