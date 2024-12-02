#version 330

in vec2 fragBary;

uniform vec3 wireColor = vec3(0.0, 0.7, 1.0);
uniform vec3 faceColor = vec3(0.2);
uniform float wireWidth = 1.0;

out vec4 finalColor;

float edgeFactor() {
    vec3 bary = vec3(fragBary.x, fragBary.y, 1.0 - fragBary.x - fragBary.y);
    vec3 deltas = fwidth(bary) * wireWidth;
    vec3 smoothing = deltas;

    vec3 edges = smoothstep(deltas, vec3(0.0), bary);
    return max(max(edges.x, edges.y), edges.z);
}

void main() {
    float edge = edgeFactor();
    finalColor = vec4(mix(faceColor, wireColor, edge), 1.0);
}