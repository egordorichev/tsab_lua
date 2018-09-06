varying vec2 texCoord;

uniform sampler2D tex;
uniform vec4 color;
uniform float textured;

void main() {
    if (textured > 0.5) {
        gl_FragColor = texture2D(tex, texCoord) * color;
    } else {
        gl_FragColor = color;
    }
}