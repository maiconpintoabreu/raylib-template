#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

// Custom uniforms
uniform float trailFadeExp = 2.0; // Controls how "sharp" the fade is

void main()
{
    // Use the X coordinate as the progress along the trail (0.0 to 1.0)
    float progress = fragTexCoord.y;

    float x = clamp(1.0 - progress, 0.0, 1.0);

    // For a "trailFadeExp" of 2.0:
    float fade = x*x*x*x*x*x*x*x*x*x;

    finalColor = vec4(fragColor.rgb, fragColor.a * fade);
}