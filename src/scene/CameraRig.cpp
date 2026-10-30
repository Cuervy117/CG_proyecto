#include "CameraRig.h"

#include <gtc/matrix_transform.hpp>

glm::mat4 CameraRig::projection(float aspect) const
{
	return glm::perspective(glm::radians(fov), aspect, nearZ, farZ);
}
