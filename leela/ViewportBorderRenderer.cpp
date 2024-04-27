#include "ViewportBorderRenderer.h"
#include "ViewportSceneObject.h"
#include "Utils.h"


#define VERTEX_STRIDE_IN_VBO        7


void ViewportBorderRenderer::init()
{
	constructBorderVertices();
}

void ViewportBorderRenderer::constructBorderVertices()
{
	std::vector<float>* v = new std::vector<float>();

	vector_push_back_7(*v, -_viewport->_w / 2.0, -_viewport->_h / 2.0, 0.0f, 1.0f, 1.0f, 1.0f, 0.3f);
	vector_push_back_7(*v, +_viewport->_w / 2.0, -_viewport->_h / 2.0, 0.0f, 1.0f, 1.0f, 1.0f, 0.3f);
	
	vector_push_back_7(*v, +_viewport->_w / 2.0, -_viewport->_h / 2.0, 0.0f, 1.0f, 1.0f, 1.0f, 0.3f);
	vector_push_back_7(*v, +_viewport->_w / 2.0, +_viewport->_h / 2.0, 0.0f, 1.0f, 1.0f, 1.0f, 0.3f);

	vector_push_back_7(*v, +_viewport->_w / 2.0, +_viewport->_h / 2.0, 0.0f, 1.0f, 1.0f, 1.0f, 0.3f);
	vector_push_back_7(*v, -_viewport->_w / 2.0, +_viewport->_h / 2.0, 0.0f, 1.0f, 1.0f, 1.0f, 0.3f);
	
	vector_push_back_7(*v, -_viewport->_w / 2.0, +_viewport->_h / 2.0, 0.0f, 1.0f, 1.0f, 1.0f, 0.3f);
	vector_push_back_7(*v, -_viewport->_w / 2.0, -_viewport->_h / 2.0, 0.0f, 1.0f, 1.0f, 1.0f, 0.3f);

	//--------------------------------------

	if (_borderVbo != 0) {
		glDeleteBuffers(1, &_borderVbo);
	}
	if (_borderVao != 0) {
		glDeleteVertexArrays(1, &_borderVao);
	}

	glGenVertexArrays(1, &_borderVao);
	glBindVertexArray(_borderVao);
	glGenBuffers(1, &_borderVbo);
	glBindBuffer(GL_ARRAY_BUFFER, _borderVbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float) * v->size(),
		v->data(),
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	numBorderVertices = v->size() / VERTEX_STRIDE_IN_VBO;

	_viewport->bChanged = false;

	//spdlog::info("Sending viewport border to GPU");

	delete v;
}



void ViewportBorderRenderer::parentChanged()
{
	_viewport = dynamic_cast<ViewportSceneObject*>(_sceneParent);
}

void ViewportBorderRenderer::render(ViewportType viewportType, RenderStage renderStage, GlslProgram& glslProgram)
{
	if (viewportType == _viewport->_viewportType) {
		if (renderStage == RenderStage::Final) {
			if (glslProgram.type() == GlslProgramType::SimpleOrtho) {

				if (_viewport->bChanged)
					constructBorderVertices();

				renderBorder(glslProgram);
			}
		}
	}
}

void ViewportBorderRenderer::renderBorder(GlslProgram& glslProgram)
{
	glm::mat4 projection = glm::ortho(float(( - _viewport->_w - 2) / 2),
		                              float(( + _viewport->_w + 2) / 2),
		                              float(( - _viewport->_h - 2) / 2),
		                              float(( + _viewport->_h + 2) / 2),
		                              -1.0f,
		                              1.0f);
	glslProgram.setMat4("proj", glm::value_ptr(projection));

	glEnable(GL_BLEND);

	glBindVertexArray(_borderVao);
	glDrawArrays(GL_LINES, 0, (GLsizei)numBorderVertices);
	
	glDisable(GL_BLEND);

}
