#version 100
precision mediump float;

// In version 100, we use 'varying' instead of 'in'
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Custom uniforms
uniform float trailFadeExp; // Note: default values aren't supported in GLSL 100 headers

void main()
{
    // Use the Y coordinate as the progress along the trail (0.0 to 1.0)
    float progress = fragTexCoord.y;
    
    float x = clamp(1.0 - progress, 0.0, 1.0);

    float fade = x*x*x*x*x*x*x*x*x*x;
    
    // In version 100, we assign directly to the built-in gl_FragColor
    gl_FragColor = vec4(fragColor.rgb, fragColor.a * fade);
}