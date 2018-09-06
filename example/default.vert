attribute vec3 v_Vertex;
attribute vec2 v_TexCoord;
attribute vec4 v_Color;
uniform mat4 v_ProjectionMatrix;

varying vec4 color;
varying vec2 texCoord;

void main(void) {
    color = v_Color;
    texCoord = vec2(v_TexCoord);
    gl_Position = v_ProjectionMatrix * vec4(v_Vertex, 1.0);
}