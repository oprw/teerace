#version 330

layout (location = 0) in vec2 inVertex;

uniform mat4x2 Pos;

uniform vec2 Offset;
uniform vec2 Dir;
uniform int JumpIndex;

void main()
{
	vec4 VertPos = vec4(inVertex, 0.0, 1.0);
	int XCount = gl_InstanceID - (int(gl_InstanceID/JumpIndex) * JumpIndex);
	int YCount = (int(gl_InstanceID/JumpIndex));
	VertPos.x += Offset.x + Dir.x * XCount;
	VertPos.y += Offset.y + Dir.y * YCount;
		
	gl_Position = vec4(Pos * VertPos, 0.0, 1.0);
}
