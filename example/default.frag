varying vec2 v_texCoord;

uniform sampler2D tex;
uniform vec4 color;

void main(void) {
    gl_FragColor = texture2D(tex, v_texCoord) * color;
}