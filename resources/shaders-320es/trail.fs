#version 300 es

// Precision must be defined for ES versions
precision mediump float;

// 'in' attributes are the same, but the calling convention 
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

// Custom uniforms
uniform float trailFadeExp;

void main()
{
    // Progress along the trail (0.0 at start, 1.0 at end)
    float progress = fragTexCoord.y;
    
    float x = clamp(1.0 - progress, 0.0, 1.0);

    float fade = x*x*x*x*x*x*x*x*x*x;
    
    finalColor = vec4(fragColor.rgb, fragColor.a * fade);
}