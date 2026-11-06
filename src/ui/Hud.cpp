#include "Hud.h"

#include <glew.h>

#include <algorithm>
#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace cgame
{
namespace
{
using Glyph = std::array<const char*, 7>;

const std::unordered_map<char, Glyph>& font()
{
	static const std::unordered_map<char, Glyph> glyphs = {
		{'A', {"01110", "10001", "10001", "11111", "10001", "10001", "10001"}},
		{'B', {"11110", "10001", "10001", "11110", "10001", "10001", "11110"}},
		{'C', {"01111", "10000", "10000", "10000", "10000", "10000", "01111"}},
		{'D', {"11110", "10001", "10001", "10001", "10001", "10001", "11110"}},
		{'E', {"11111", "10000", "10000", "11110", "10000", "10000", "11111"}},
		{'F', {"11111", "10000", "10000", "11110", "10000", "10000", "10000"}},
		{'G', {"01111", "10000", "10000", "10111", "10001", "10001", "01111"}},
		{'H', {"10001", "10001", "10001", "11111", "10001", "10001", "10001"}},
		{'I', {"11111", "00100", "00100", "00100", "00100", "00100", "11111"}},
		{'J', {"00111", "00010", "00010", "00010", "10010", "10010", "01100"}},
		{'K', {"10001", "10010", "10100", "11000", "10100", "10010", "10001"}},
		{'L', {"10000", "10000", "10000", "10000", "10000", "10000", "11111"}},
		{'M', {"10001", "11011", "10101", "10101", "10001", "10001", "10001"}},
		{'N', {"10001", "11001", "10101", "10011", "10001", "10001", "10001"}},
		{'O', {"01110", "10001", "10001", "10001", "10001", "10001", "01110"}},
		{'P', {"11110", "10001", "10001", "11110", "10000", "10000", "10000"}},
		{'Q', {"01110", "10001", "10001", "10001", "10101", "10010", "01101"}},
		{'R', {"11110", "10001", "10001", "11110", "10100", "10010", "10001"}},
		{'S', {"01111", "10000", "10000", "01110", "00001", "00001", "11110"}},
		{'T', {"11111", "00100", "00100", "00100", "00100", "00100", "00100"}},
		{'U', {"10001", "10001", "10001", "10001", "10001", "10001", "01110"}},
		{'V', {"10001", "10001", "10001", "10001", "10001", "01010", "00100"}},
		{'W', {"10001", "10001", "10001", "10101", "10101", "10101", "01010"}},
		{'X', {"10001", "10001", "01010", "00100", "01010", "10001", "10001"}},
		{'Y', {"10001", "10001", "01010", "00100", "00100", "00100", "00100"}},
		{'Z', {"11111", "00001", "00010", "00100", "01000", "10000", "11111"}},
		{'0', {"01110", "10001", "10011", "10101", "11001", "10001", "01110"}},
		{'1', {"00100", "01100", "00100", "00100", "00100", "00100", "01110"}},
		{'2', {"01110", "10001", "00001", "00010", "00100", "01000", "11111"}},
		{'3', {"11110", "00001", "00001", "01110", "00001", "00001", "11110"}},
		{'4', {"00010", "00110", "01010", "10010", "11111", "00010", "00010"}},
		{':', {"00000", "00100", "00100", "00000", "00100", "00100", "00000"}},
		{'-', {"00000", "00000", "00000", "11111", "00000", "00000", "00000"}}
	};
	return glyphs;
}

GLuint compileShader(GLenum type, const char* source)
{
	const GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	GLint compiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_TRUE)
		return shader;
	glDeleteShader(shader);
	return 0;
}

void addRectangle(std::vector<float>& vertices, float left, float top,
	float right, float bottom)
{
	const float quad[] = {
		left, top, right, top, right, bottom,
		left, top, right, bottom, left, bottom
	};
	vertices.insert(vertices.end(), std::begin(quad), std::end(quad));
}

void addText(std::vector<float>& vertices, const std::string& text,
	float x, float y, float cell)
{
	const float advance = cell * 6.0f;
	for (char character : text)
	{
		if (character == ' ')
		{
			x += advance;
			continue;
		}
		const auto found = font().find(character);
		if (found != font().end())
		{
			for (int row = 0; row < 7; ++row)
			{
				for (int column = 0; column < 5; ++column)
				{
					if (found->second[static_cast<std::size_t>(row)][column] == '1')
					{
						const float left = x + column * cell;
						const float top = y + row * cell;
						addRectangle(vertices, left, top,
							left + cell - 0.5f, top + cell - 0.5f);
					}
				}
			}
		}
		x += advance;
	}
}
}

Hud::~Hud()
{
	shutdown();
}

bool Hud::initialize()
{
	static const char* vertexSource =
		"#version 330 core\n"
		"layout(location=0) in vec2 position;\n"
		"uniform vec2 viewport;\n"
		"void main(){ vec2 p=vec2(position.x/viewport.x*2.0-1.0, 1.0-position.y/viewport.y*2.0); gl_Position=vec4(p,0.0,1.0); }\n";
	static const char* fragmentSource =
		"#version 330 core\n"
		"uniform vec4 color;\n"
		"out vec4 outColor;\n"
		"void main(){ outColor=color; }\n";

	const GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
	const GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
	if (vertex == 0 || fragment == 0)
	{
		if (vertex != 0) glDeleteShader(vertex);
		if (fragment != 0) glDeleteShader(fragment);
		return false;
	}

	m_program = glCreateProgram();
	glAttachShader(m_program, vertex);
	glAttachShader(m_program, fragment);
	glLinkProgram(m_program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	GLint linked = GL_FALSE;
	glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE)
	{
		shutdown();
		return false;
	}

	m_viewportLocation = glGetUniformLocation(m_program, "viewport");
	m_colorLocation = glGetUniformLocation(m_program, "color");
	glGenVertexArrays(1, &m_vertexArray);
	glGenBuffers(1, &m_vertexBuffer);
	glBindVertexArray(m_vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glBindVertexArray(0);
	return m_vertexArray != 0 && m_vertexBuffer != 0;
}

void Hud::draw(int width, int height, const std::string& cameraView,
	const std::string& location, const std::string& action)
{
	if (m_program == 0 || width <= 0 || height <= 0)
		return;

	constexpr float cell = 2.0f;
	const float x = 26.0f;
	const float panelHeight = 58.0f;
	const float panelWidth = static_cast<float>(std::max(0, std::min(width - 32, 340)));
	const float panelTop = static_cast<float>(height) - panelHeight - 14.0f;
	const float y = panelTop + 10.0f;
	std::vector<float> background;
	std::vector<float> text;
	addRectangle(background, 16.0f, panelTop, 16.0f + panelWidth,
		panelTop + panelHeight);
	addText(text, "CAMARA: " + cameraView, x, y, cell);
	addText(text, "LUGAR: " + location, x, y + 16.0f, cell);
	addText(text, "ESTADO: " + action, x, y + 32.0f, cell);

	const GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
	GLboolean depthMaskWasEnabled = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMaskWasEnabled);
	const GLboolean blendWasEnabled = glIsEnabled(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(m_program);
	glUniform2f(m_viewportLocation, static_cast<float>(width),
		static_cast<float>(height));
	glBindVertexArray(m_vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, background.size() * sizeof(float),
		background.data(), GL_STREAM_DRAW);
	glUniform4f(m_colorLocation, 0.025f, 0.055f, 0.09f, 0.88f);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(background.size() / 2));
	glBufferData(GL_ARRAY_BUFFER, text.size() * sizeof(float), text.data(),
		GL_STREAM_DRAW);
	glUniform4f(m_colorLocation, 0.35f, 0.95f, 1.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(text.size() / 2));
	glBindVertexArray(0);
	glUseProgram(0);
	if (blendWasEnabled) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (depthMaskWasEnabled) glDepthMask(GL_TRUE); else glDepthMask(GL_FALSE);
	if (depthWasEnabled) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
}

void Hud::shutdown()
{
	if (m_vertexBuffer != 0) glDeleteBuffers(1, &m_vertexBuffer);
	if (m_vertexArray != 0) glDeleteVertexArrays(1, &m_vertexArray);
	if (m_program != 0) glDeleteProgram(m_program);
	m_vertexBuffer = 0;
	m_vertexArray = 0;
	m_program = 0;
}
}
