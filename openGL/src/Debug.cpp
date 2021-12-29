#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Camera.h"
#include "Debug.h"

namespace Debug {
	
	void SetViewProj(glm::mat4& viewProj) 
	{ 
		vp = viewProj;
	}

	void DrawLine(glm::vec3 start, glm::vec3 end)
	{
		VertexArray va;
		Shader shader("res/shaders/debug/debugv.vert", "res/shaders/debug/debugLinef.frag");
		glm::vec3 lineVerts[2] = { start, end };
		VertexBuffer lineVB(lineVerts, (unsigned int)sizeof(lineVerts));
		VertexBufferLayout lineLayout;
		
		lineLayout.Push<float>(3);
		
		va.AddBuffer(lineVB, lineLayout);
		
		shader.Bind();
		va.Bind();
		
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mvp = vp * model;
		shader.SetUniformMatrix4fv("mvp", mvp);
		
		glLineWidth(3);
		glDrawArrays(GL_LINES, 0, 2);

		glDeleteVertexArrays(1, va.GetID());
	}

	void DrawTri(glm::vec3 one, glm::vec3 two, glm::vec3 three)
	{
		VertexArray va;
		Shader shader("res/shaders/debug/debugv.vert", "res/shaders/debug/debugf.frag");
		glm::vec3 triVerts[] = { one, two, three };
		VertexBuffer triVB(triVerts, (unsigned int)sizeof(triVerts));
		VertexBufferLayout triLayout;

		triLayout.Push<float>(3);

		va.AddBuffer(triVB, triLayout);

		shader.Bind();
		va.Bind();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mvp = vp * model;
		shader.SetUniformMatrix4fv("mvp", mvp);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDeleteVertexArrays(1, va.GetID());
	}

	void DrawOrthProj(glm::mat4& viewProj)
	{
		glm::mat4 inv = glm::inverse(viewProj);

		glm::vec4 ndc[8] =
		{
			// near face
			glm::vec4(1, 1, -1, 1.f),
			glm::vec4(-1, 1, -1, 1.f),
			glm::vec4(1, -1, -1, 1.f),
			glm::vec4(-1, -1, -1, 1.f),

			// far face
			glm::vec4(1, 1, 1, 1.f),
			glm::vec4(-1, 1, 1, 1.f),
			glm::vec4(1, -1, 1, 1.f),
			glm::vec4(-1, -1, 1, 1.f),
		};

		glm::vec3 v[8];
		for (int i = 0; i < 8; i++)
		{
			glm::vec4 wc = inv * ndc[i];
			v[i].x = wc.x / wc.w;
			v[i].y = wc.y / wc.w;
			v[i].z = wc.z / wc.w;
		}

		DrawTri(v[0], v[1], v[2]);
		DrawTri(v[1], v[2], v[3]);

		DrawTri(v[0], v[1], v[4]);
		DrawTri(v[1], v[4], v[5]);
		
		DrawTri(v[2], v[3], v[6]);
		DrawTri(v[3], v[6], v[7]);

		DrawTri(v[4], v[5], v[6]);
		DrawTri(v[5], v[6], v[7]);

		DrawTri(v[1], v[3], v[5]);
		DrawTri(v[3], v[5], v[7]);

		DrawTri(v[0], v[2], v[4]);
		DrawTri(v[2], v[4], v[6]);

		DrawLine(v[0], v[1]);
		DrawLine(v[0], v[2]);
		DrawLine(v[3], v[1]);
		DrawLine(v[3], v[2]);
		
		DrawLine(v[4], v[5]);
		DrawLine(v[4], v[6]);
		DrawLine(v[7], v[5]);
		DrawLine(v[7], v[6]);
		
		DrawLine(v[0], v[4]);
		DrawLine(v[1], v[5]);
		DrawLine(v[3], v[7]);
		DrawLine(v[2], v[6]);
	}
}