#include <Debug.h>

namespace Debug {
	
	void SetViewProj(glm::mat4& viewProj) 
	{ 
		vp = viewProj;
	}

	void DrawLine(glm::vec3 start, glm::vec3 end)
	{
		VertexArray va;
		Shader shader("res/shaders/debug/debugv.vert", "res/shaders/debug/debugf.frag");
		glm::vec3 lineVerts[] = { start, end };
		VertexBuffer lineVB(lineVerts, (unsigned int)sizeof(lineVerts));
		VertexBufferLayout lineLayout;
		
		lineLayout.Push<float>(3);

		va.AddBuffer(lineVB, lineLayout);

		shader.Bind();
		va.Bind();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mvp = vp * model;
		shader.SetUniformMatrix4fv("mvp", mvp);

		glDrawArrays(GL_LINES, 0, 2);
	}
}