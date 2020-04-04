// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Tex coord input from vertex shader
in vec2 fragTexCoord;
in vec4 vertexColor;

// This corresponds to the output color to the color buffer
out vec4 outColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

void main()
{
	// Sample color from texture
	vec4 texColor = texture(uTexture,fragTexCoord);
	outColor = vec4(texColor.x+vertexColor.x/2,texColor.y+vertexColor.y/2,texColor.z+vertexColor.z/2,texColor.w*vertexColor.w);
}
