
uniform sampler2D texBlackWhite;

varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
varying mediump vec3 normalVarying;     // normal in world space
varying mediump vec3 tangentVarying;    // tangent in world space

uniform mediump float exposure;
uniform mediump float decay;
uniform mediump float density;
uniform mediump float weight;
varying mediump vec4 lightPosVarying;
const int NUM_SAMPLES = 100;

void main()
{
    //    exposure = 0.0034;
    //    decay = 1.0;
    //    density = 0.84;
    //    weight = 5.65;
    //calculate vector from pixel to light source
    mediump vec2 deltaTexCoord = vec2( texCoordVarying.st - lightPosVarying.xy );
    //divide by # of samples and scale by control factor
    deltaTexCoord *= 1.0 / float(NUM_SAMPLES/2) * 0.84;
    //store initial sample
    mediump vec4 initialSample = texture2D(texBlackWhite, texCoordVarying.st);
    //set up illumination decay factor
    mediump float illuminationDecay = 1.0;
    //copy textureCoordinates for manipulation
    mediump vec2 textCoo = texCoordVarying.st;
    //evauluate summation in NUM_SAMPLES iterations
    for(int i=0; i < NUM_SAMPLES ; i++)
    {
        //step sample location along ray
        textCoo -= deltaTexCoord;
        //retrieve sample at new location
        mediump vec4 newSample = texture2D(texBlackWhite, textCoo);
        //apply sample attenuation scale/decay factors
        newSample *= illuminationDecay * 5.65;
            //acumulate combined color
        gl_FragColor += newSample;

        //update exponential decay factor
        illuminationDecay *= 4.0;
    }
    //output final color with a further scale control factor
  //gl_FragColor = mix(texture2D(texBlackWhite, texCoordVarying.st),gl_FragColor,1.0);
//    gl_FragColor *= 0.0024;
    gl_FragColor = clamp(gl_FragColor,0.0,1.0);
    gl_FragColor *= 0.09;
//    gl_FragColor = clamp(gl_FragColor,0.0,1.0);

}