
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
uniform sampler2D SpecularMap;
uniform sampler2D NormalMap;

varying lowp vec4 ambientVarying;
varying lowp vec4 diffuseVarying;
varying lowp vec4 specularVarying;
varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
varying mediump vec3 normalVarying;     // normal in world space
varying mediump vec3 tangentVarying;    // tangent in world space

void main()
{
    lowp vec4 ambientResult = vec4(Ka * Ia, 1.0);
    
    mediump vec4 pos = posVarying;
    mediump vec3 n = normalize(normalVarying);
    
    // TODO: calculate tbn matrix, ensure it's orthonormal
    mediump vec3 t = normalize(tangentVarying);
    mediump vec3 t_o = normalize((t - n * dot(n, t)));
    mediump vec3 b = normalize(cross(n,t_o));
//    mediump vec3 b = normalize(cross(n,t));

    mediump mat3 tbn = (mat3(t_o,b,n));
    mediump vec3 TextureNormal_tangentspace = normalize(texture2D( NormalMap, texCoordVarying.st ).xyz*2.0 - 1.0);
    n = normalize(tbn * TextureNormal_tangentspace);
    
    // TODO: read and correctly transform normals from normal map, then use them for lighting
//    pos = vec4(n,0) * pos;
//    mediump vec4 pos2 = LightPos;        // pos in world space
//    pos2 = vec4(n,0) * pos2;

    mediump vec3 l = normalize((LightPos - pos).xyz);
    lowp float intensity = dot(n, l);
    lowp vec3 diffuse = Kd * clamp(intensity, 0.0, 1.0) * Id;
    lowp vec4 diffuseResult = vec4(clamp(diffuse, 0.0, 1.0), 1.0);
    
    // If vertex is lit, calculate specular term in view space using the Blinn-Phong model
    lowp vec4 specularResult = vec4(0.0);
    if (intensity > 0.0)
    {
        mediump vec4 pos3 = EyePos;        // pos in world space
        pos3 = vec4(n,0) * pos3;

        mediump vec3 eyeVec = normalize(EyePos.xyz - pos.xyz);
        mediump vec3 h = normalize(l + eyeVec);
        mediump float specIntensity = pow(max(dot(h,n), 0.0), Ns);
        mediump vec3 specular = Ks * clamp(specIntensity, 0.0, 1.0) * Is;
        specularResult = vec4(clamp(specular, 0.0, 1.0), 1.0);

    }

    lowp vec4 color = texture2D(DiffuseMap, texCoordVarying.st);
    specularResult = specularResult * texture2D(SpecularMap, texCoordVarying.st);
    gl_FragColor = (ambientResult + diffuseResult + specularResult) * color;

}
