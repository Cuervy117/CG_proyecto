#pragma once

#include <string>

namespace cgame
{
class Hud
{
public:
	~Hud();

	bool initialize();
	void draw(int width, int height, const std::string& mode,
		const std::string& location, const std::string& action);
	void shutdown();

private:
	unsigned int m_program = 0;
	unsigned int m_vertexArray = 0;
	unsigned int m_vertexBuffer = 0;
	int m_viewportLocation = -1;
	int m_colorLocation = -1;
};
}
