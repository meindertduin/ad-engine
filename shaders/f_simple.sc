$input v_color0, v_texCoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_cubeTex, 0);

void main()
{
    gl_FragColor = texture2D(s_cubeTex, v_texCoord0);
}