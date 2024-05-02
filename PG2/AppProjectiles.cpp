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
					const auto& hit_name = model->name;
					print("PROJECTILE HIT " << hit_name);
					hit = true;

					// Projectile hit glass cube – destroy it & play sound
					if (hit_name.substr(0, 15) == "obj_glass_cube_") {
						// Remove cube from possible collisions vector
						collisions.erase(std::remove(collisions.begin(), collisions.end(), model), collisions.end());
						// Remove cube from the scene
						scene_transparent.erase(hit_name);
						// Remove cube from helper vector for sorting transparent objects (by clearing the whole thing and regenerating it)
						scene_transparent_pairs.clear();
						for (auto i = scene_transparent.begin(); i != scene_transparent.end(); i++) {
							scene_transparent_pairs.push_back(&*i);
						}
					}
					// Projectile hit jukebox – on/off light+music
					else if (hit_name == "obj_jukebox") {
						is_jukebox_on = (is_jukebox_on + 1) % 2;
					}

					break;
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
