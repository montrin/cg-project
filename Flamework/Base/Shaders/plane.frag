varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
varying mediump vec3 normalVarying;     // normal in world space
varying mediump vec4 viewSpace;        //

uniform mediump vec4 LightPos;
uniform mediump vec4 LightDir;
uniform mediump vec4 EyePos;

uniform mediump float opacity;

//fog constants
const mediump vec3 DiffuseLight = vec3(0.15, 0.05, 0.0);
const mediump vec3 RimColor = vec3(0.2, 0.2, 0.2);
const mediump vec3 fogColor = vec3(1.0, 1.0,1.0);
const mediump float FogDensity = 0.5;
const int depthFog=1;
const int fogSelector=2;
mediump float opacityFactor=0.1;
mediump float o;

const mediump float sampleDist = 1.0;
const mediump float sampleStrength = 4.2;
varying lowp vec2 uv;

mediump vec4 fog() {
    lowp vec4 tex1 = vec4(.95,.95,.95,1.0);
    mediump vec4 pos = posVarying;
    mediump vec3 eyeVec = normalize(EyePos.xyz - pos.xyz);
    
    //get light an view directions
    mediump vec3 L = normalize( LightPos.xyz - pos.xyz);
    mediump vec3 V = normalize( EyePos.xyz - pos.xyz);
    
    //diffuse lighting
    mediump vec3 diffuse = DiffuseLight * max(0.0, dot(L,normalVarying));
    
    //rim lighting
    mediump float rim = 1.0 - max(dot(V, normalVarying), 0.0);
    rim = smoothstep(0.6, 1.0, rim);
    mediump vec3 finalRim = RimColor * vec3(rim, rim, rim);
    //get all lights and texture
    mediump vec3 lightColor = finalRim + diffuse + tex1.xyz;
    
    mediump vec3 finalColor = vec3(0, 0, 0);
    
    //distance
    mediump float dist = 0.0;
    mediump float fogFactor = 0.0;
    
    //compute distance used in fog equations
    if(depthFog == 0)//select plane based vs range based
    {
        //plane based
        dist = abs(viewSpace.z);
        //dist = (gl_FragCoord.z / gl_FragCoord.w);
    }
    else
    {
        //range based
        dist = length(viewSpace);
    }
    
    if(fogSelector == 0)//linear fog
    {
        // 20 - fog starts; 80 - fog ends
        fogFactor = (80.0 - dist)/(80.0 - 20.0);
        fogFactor = clamp( fogFactor, 0.0, 1.0 );
        
        //if you inverse color in glsl mix function you have to
        //put 1.0 - fogFactor
        finalColor = mix(fogColor, lightColor, fogFactor);
    }
    else if( fogSelector == 1)// exponential fog
    {
        fogFactor = 1.0 /exp(dist * FogDensity);
        fogFactor = clamp( fogFactor, 0.0, 1.0 );
        
        // mix function fogColor⋅(1−fogFactor) + lightColor⋅fogFactor
        finalColor = mix(fogColor, lightColor, fogFactor);
    }
    else if( fogSelector == 2)
    {
        fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
        fogFactor = clamp( fogFactor, 0.0, 1.0 );
        
        finalColor = mix(fogColor, lightColor, fogFactor);
    }
    
    //scale opacity in relation to distance
    mediump float clamp_dist = clamp(dist, 0.0,20.0);
    o = clamp_dist/20.0*0.98;
    o = clamp(o,0.0,1.0);
    
    return vec4(finalColor,o);
}
void main()
{
//    gl_FragColor =  vec4(finalColor, o);
    gl_FragColor =  fog();
    
}
