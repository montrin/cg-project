//#extension GL_EXT_shader_framebuffer_fetch : require
uniform sampler2D sceneTex; // 0
uniform sampler2D DiffuseMap;
uniform mediump float rt_w; // render target width
uniform mediump float rt_h; // render target height
uniform mediump float vx_offset;

mediump float offset[3];
mediump float weight[3];
varying lowp vec4 texCoordVarying;
//#define kBlendModeDifference 1
//#define kBlendModeOverlay    2
//#define BlendOverlay(a, b) ( (b<0.5) ? (2.0*b*a) : (1.0-2.0*(1.0-a)*(1.0-b)) )

void main()
{

    
    offset[0] = 0.0;
    offset[1] = 1.3846153846;
    offset[2] = 3.2307692308;
    
    weight[0] = 0.2270270270;
    weight[1] = 0.3162162162;
    weight[2] = 0.0702702703;

    lowp vec3 tc = vec3(1.0, 0.0, 0.0);
    if (texCoordVarying.x<(vx_offset-0.01))
    {
        lowp vec2 uv = texCoordVarying.xy;
        tc = texture2D(sceneTex, uv).rgb * weight[0];
        for (int i=1; i<3; i++)
        {
            tc += texture2D(sceneTex, uv + vec2(offset[i])/rt_w, 0.0).rgb * weight[i];
            tc += texture2D(sceneTex, uv - vec2(offset[i])/rt_w, 0.0).rgb * weight[i];
        }
    }
    else if (texCoordVarying.x>=(vx_offset+0.01))
    {
        tc = texture2D(sceneTex, texCoordVarying.xy).rgb;
  //      tc = texture2D(DiffuseMap, texCoordVarying.xy).rgb;
    }
    lowp vec4 sourceColor = vec4(tc, 1.0);
//    lowp vec4 destColor = gl_LastFragData[0];
    lowp vec4 destColor = vec4(1.0,0.0,0.0,1.0);
//    gl_FragColor.r = BlendOverlay(sourceColor.r, destColor.r);
//    gl_FragColor.g = BlendOverlay(sourceColor.g, destColor.g);
//    gl_FragColor.b = BlendOverlay(sourceColor.b, destColor.b);
//    gl_FragColor.a = sourceColor.a;
    gl_FragColor = sourceColor * destColor;

}