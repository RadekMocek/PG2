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
			bool hit = false;

			// - Objects collision check
			for (const auto model : collisions) {
				if (model->Coll_CheckPoint(position)) {
					print("PROJECTILE HIT " << model->name);
					hit = true;
				}
			}

			// - Heightmap collision check
			if (position.y < GetHeightmapY(position.x, position.z)) {
				print("PROJECTILE HIT ground");
				hit = true;
			}

			// - Hide if hit
			if (hit) {
				is_projectile_moving[i] = false;
				projectile->position = glm::vec3(0.0f, -10.0f, 0.0f);
			}
		}
	}
}
