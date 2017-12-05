#version 330 core

in float Transp;

out vec4 FragColor;

uniform sampler2D texture;

void main()
{	
	FragColor = texture(texture, gl_PointCoord);
	FragColor.a *= Transp;
}