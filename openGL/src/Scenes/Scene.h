#pragma once
#include "Model.h"

namespace scene {
	struct Object {
		Model model;
	};

	class Scene
	{
	public:
		Scene() {}
		virtual ~Scene() {}
	};
}
