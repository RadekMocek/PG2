#include "App.hpp"

void App::Shoot()
{
	auto name = "obj_projectile_" + projectile_n;

	scene_opaque.find(name)->second.position = camera.position;

	projectile_n = (projectile_n + 1) % N_PROJECTILES;
}
