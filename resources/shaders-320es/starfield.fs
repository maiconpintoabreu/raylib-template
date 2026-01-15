#version 320 es
precision mediump float;

// Input from vertex shader
in vec2 fragTexCoord;
in vec4 fragColor;

// Output color
out vec4 finalColor;

// Uniforms
uniform float seconds;

float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

void main() {
    vec2 uv = fragTexCoord;
    
    vec2 movingUv = uv;
    movingUv.y += seconds * 0.2; 

    vec2 gridUv = movingUv * 50.0; 
    vec2 id = floor(gridUv);
    vec2 fuv = fract(gridUv);

    float h = hash(id);
    float star = 0.0;

    if (h > 0.95) {
        vec2 center = vec2(h, fract(h * 10.0)); 
        float d = length(fuv - center);
        
        star = smoothstep(0.1, 0.0, d); 
        star *= 0.5 + 0.5 * sin(seconds * 3.0 + h * 10.0);
    }

    finalColor = vec4(vec3(star), 1.0);
}