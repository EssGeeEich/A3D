#version 330 core

in vec2 TexCoord;
out vec4 fragColor;

uniform sampler2D uAccumColor;
uniform sampler2D uRevealage;

void main() {
    // Retrieve the accumulated weighted color and revealage
    vec4 colorAccum = texture(uAccumColor, TexCoord);
    float alphaAccum = texture(uRevealage, TexCoord).r;

    // Normalize the accumulated color. The small constant prevents division by zero.
    fragColor.rgb = colorAccum.rgb / max(alphaAccum, 0.0001);
    fragColor.a = alphaAccum; // This is our approximated transparency

	fragColor.r = TexCoord.x;
}
