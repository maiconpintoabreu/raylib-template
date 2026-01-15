#version 330

// Input vertex attributes (from Raylib)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output color
out vec4 finalColor;

// Uniforms
uniform float seconds;

// Simple hash function to get a random value based on a coordinate
float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

void main() {
    // 1. Setup coordinates (0.0 to 1.0)
    vec2 uv = fragTexCoord;
    
    // 2. Adjust for movement
    // We add 'seconds' to Y to make stars fall. 
    // Multiplied by 0.5 for a steady drift.
    vec2 movingUv = uv;
    movingUv.y += seconds * 0.2; 

    // 3. Create a grid system for stars
    // Scaling determines star density (higher = more stars)
    vec2 gridUv = movingUv * 50.0; 
    vec2 id = floor(gridUv);   // Unique ID for each grid cell
    vec2 fuv = fract(gridUv);  // Coordinate inside the cell (0 to 1)

    // 4. Generate a star in the cell
    // Use the cell ID to get a random position for a star inside this cell
    float h = hash(id);
    float star = 0.0;

    // Only draw a star if the hash is above a threshold (density control)
    if (h > 0.95) {
        // Create a circular shape using distance from a random center point
        // Shift the center slightly using the hash so they aren't perfectly centered
        vec2 center = vec2(h, fract(h * 10.0)); 
        float d = length(fuv - center);
        
        // Sharpen the point into a glowing star
        star = smoothstep(0.1, 0.0, d); 
        
        // Add a simple flicker effect based on time
        star *= 0.5 + 0.5 * sin(seconds * 3.0 + h * 10.0);
    }

    finalColor = vec4(vec3(star), 1.0);
}