
uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mediump mat4 ProjectionMatrix;

uniform mediump mat3 NormalMatrix;

uniform mediump vec4 LightPos;
uniform mediump vec4 EyePos;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform lowp vec3 Ia;   // ambient light intensity
uniform lowp vec3 Id;   // diffuse light intensity
uniform lowp vec3 Is;   // specular light intensity

uniform sampler2D DiffuseMap;

varying lowp vec4 ambientVarying;
varying lowp vec4 diffuseVarying;
varying lowp vec4 specularVarying;
varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;       // pos in world space
varying mediump vec3 normalVarying;    // normal in world space

void main()
{
    // TODO: implement Phong Shading (per-fragment lighting)
    
    
    // TODO: calculate diffuse lighting
    mediump vec3 n = normalize(normalVarying) ;
    mediump vec3 l = normalize(vec3(LightPos - posVarying)) ;
    //
    lowp float intensity = max(0.0, dot(n,l)) ;
    lowp vec3 diffuse = Kd * clamp(intensity, 0.0, 1.0) * Id;
    
    // TODO: If vertex is lit, calculate specular term in view space using the Blinn-Phong model
    lowp vec4 diffusePhongShading = vec4(clamp(diffuse, 0.0, 1.0), 1.0);
    lowp vec4 specularPhonShading = vec4(0.0);
    
    if (intensity > 0.0)
    {
        //how to calculate eyeVec ?
        mediump vec3 eyeVec = normalize(vec3(EyePos - posVarying));
        mediump vec3 h = normalize(l+eyeVec);
        
        mediump vec3 specular = Ks * pow( dot( n, h ), Ns ) * Is;
        specularPhonShading = vec4(clamp(specular, 0.0, 1.0), 1.0);
    }
    
    lowp vec4 color = texture2D(DiffuseMap , vec2(texCoordVarying));
    gl_FragColor = (ambientVarying + diffusePhongShading) * color + specularPhonShading;
    gl_FrontFacing = false;
    
}
