// www.ogre3d.org/tikiwiki/tiki-index.php?page=Faking+HDR#Fragment_program_for_pre-bloom_processing
uniform sampler2D sceneTex;

varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space

const lowp vec4 gloomStart = vec4(0.95,0.95,0.95,0.95);

lowp float sqr(lowp float x) { return x*x; }
lowp vec4 sqr(lowp vec4 x) { return x*x; }

lowp vec4 expand_Hdr(lowp vec4 color)
{
    return color*(sqr(color.a*2.0)+1.0);
}

void main(void)
{
    gl_FragColor = 0.5*(texture2D(sceneTex, texCoordVarying.st)-0.5);
//        gl_FragColor = clamp(texture2D(sceneTex, texCoordVarying.st),0.0,1.0);
}