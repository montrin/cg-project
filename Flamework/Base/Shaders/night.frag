
uniform sampler2D uTexSource1;

varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
//varying mediump vec3 normalVarying;     // normal in world space
//varying mediump vec3 tangentVarying;    // tangent in world space


void main()
{//
    lowp vec4 color = texture2D(uTexSource1, texCoordVarying.st);

    // choose monochrome value
    mediump float v = max(max(color.r, color.g), color.b);
    
    // adjust contrast
    mediump float uContrast = 1.0;
    
    v = clamp(uContrast * (v - 0.5) + 0.5, 0.0, 1.0);
    
    // choose green or greyscale
    mediump vec4 monochrome;
        monochrome = vec4(0.0, v, 0.0, color.a);
    gl_FragColor = monochrome;
    
}
