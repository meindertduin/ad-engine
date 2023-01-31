$input v_texcoord0

#include <bgfx_shader.sh>

uniform vec4 u_params[1];

#define u_k u_params[0].x

SAMPLER2D(s_cubeTex, 0);

void main()
{
    gl_FragColor = texture2D(s_cubeTex, v_texcoord0) * u_k;
}