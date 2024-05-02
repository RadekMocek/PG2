#include <iostream>
#include <string>

#include "App.hpp"

#define print(x) std::cout << x << "\n"

void App::Shoot()
{
	auto name = "obj_projectile_" + std::to_string(projectile_n);

	scene_opaque.find(name)->second->position = camera.position;

	projectile_directions[projectile_n] = camera.front;
	is_projectile_moving[projectile_n] = true;

	projectile_n = (projectile_n + 1) % N_PROJECTILES;
}

void App::UpdateProjectiles(float delta_time)
{
	for (int i = 0; i < N_PROJECTILES; i++) {
		if (is_projectile_moving[i]) {
			auto name = "obj_projectile_" + std::to_string(i);
			auto projectile = scene_opaque.find(name)->second;
			auto position = projectile->position;

			// Projectile movement
			projectile->position += projectile_speed * delta_time * projectile_directions[i];

			// Projectile collision check			
			for (const auto model : collisions) {
				
				if (glm::distance(position, model->position + model->coll_center) < model->coll_radius) {
					print("HIT " << model->name << " " << glm::distance(position, model->coll_center) << " " << model->coll_radius);
					is_projectile_moving[i] = false;
				}

			}
		}
	}
}
