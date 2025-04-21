#include "ParticleDB.h"

void ParticleDB::OnStart()
{
	emit_angle_distribution = RandomEngine(emit_angle_min, emit_angle_max, 298);
	emit_radius_distribution = RandomEngine(emit_radius_min, emit_radius_max, 404);
	rotation_distribution = RandomEngine(rotation_min, rotation_max, 440);
	scale_distribution = RandomEngine(start_scale_min, start_scale_max, 494);
	speed_distribution = RandomEngine(start_speed_min, start_speed_max, 498);
	rotation_speed_distribution = RandomEngine(rotation_speed_min, rotation_speed_max, 305);
}

void ParticleDB::OnUpdate()
{
	//--- Emission ---//
	if (localFrameNumber % frames_between_bursts == 0 && emmissionAllowed)
	{
		GenerateNewParticles();
	}

	//--- Simulation ---//

	//--- Rendering ---//
	for (auto i = 0; i < x.size(); i++)
	{
		if (localParticleFrame[i] == duration_frames && doRender[i] == true)
		{
			freeSpace.push_back(i);
			doRender[i] = false;
		}
		if (doRender[i])
		{
			velocityX[i] += gravity_scale_x;
			velocityY[i] += gravity_scale_y;

			velocityX[i] *= drag_factor;
			velocityY[i] *= drag_factor;

			rotationSpeed[i] *= angular_drag_factor;

			x[i] += velocityX[i];
			y[i] += velocityY[i];

			rotation[i] += rotationSpeed[i];

			if (end_scale != -1.0f)
			{
				float lifetime_progression = static_cast<float>(localParticleFrame[i]) / duration_frames;
				scale[i] = glm::mix(initialScale[i], end_scale, lifetime_progression);
			}
			if (end_color_r != -1.0f)
			{
				float lifetime_progression = static_cast<float>(localParticleFrame[i]) / duration_frames;
				current_r[i] = glm::mix((initial_r[i]), end_color_r, lifetime_progression);
			}
			if (end_color_g != -1.0f)
			{
				float lifetime_progression = static_cast<float>(localParticleFrame[i]) / duration_frames;
				current_g[i] = glm::mix((initial_g[i]), end_color_g, lifetime_progression);
			}
			if (end_color_b != -1.0f)
			{
				float lifetime_progression = static_cast<float>(localParticleFrame[i]) / duration_frames;
				current_b[i] = glm::mix((initial_b[i]), end_color_b, lifetime_progression);
			}
			if (end_color_a != -1.0f)
			{
				float lifetime_progression = static_cast<float>(localParticleFrame[i]) / duration_frames;
				current_a[i] = glm::mix((initial_a[i]), end_color_a, lifetime_progression);
			}
			// Need to do culling here 
			pointerToEngine->ImageHandler.DrawImageEx(textureName, x[i], y[i], rotation[i], scale[i], scale[i], pivotPoint.x, pivotPoint.y, current_r[i], current_g[i], current_b[i], current_a[i], sortingOrder);
			
			localParticleFrame[i]++;
		}

	}

	localFrameNumber++;
}

void ParticleDB::GenerateNewParticles()
{
	for (auto i = 0; i < burst_quantity; i++)
	{
		float angle_radians = glm::radians(emit_angle_distribution.Sample());
		float radius = emit_radius_distribution.Sample();

		float cos_angle = glm::cos(angle_radians);
		float sin_angle = glm::sin(angle_radians);

		float speed = speed_distribution.Sample();


		if (freeSpace.empty())
		{
			x.push_back(startingX + cos_angle * radius);
			y.push_back(startingY + sin_angle * radius);

			velocityX.push_back(cos_angle * speed);
			velocityY.push_back(sin_angle * speed);

			scale.push_back(scale_distribution.Sample());
			initialScale.push_back(scale.back());
			rotation.push_back(rotation_distribution.Sample());

			rotationSpeed.push_back(rotation_speed_distribution.Sample());

			current_r.push_back(r);
			current_b.push_back(b);
			current_g.push_back(g);
			current_a.push_back(a);

			initial_r.push_back(r);
			initial_g.push_back(g);
			initial_b.push_back(b);
			initial_a.push_back(a);

			doRender.push_back(true);

			localParticleFrame.push_back(0);
		}
		else
		{
			x[freeSpace.front()] = (startingX + cos_angle * radius);
			y[freeSpace.front()] = (startingY + sin_angle * radius);

			velocityX[freeSpace.front()] = (cos_angle * speed);
			velocityY[freeSpace.front()] = (sin_angle * speed);

			scale[freeSpace.front()] = (scale_distribution.Sample());
			initialScale[freeSpace.front()] = (scale[freeSpace.front()]);
			rotation[freeSpace.front()] = (rotation_distribution.Sample());

			rotationSpeed[freeSpace.front()] = (rotation_speed_distribution.Sample());

			current_r[freeSpace.front()] = (r);
			current_b[freeSpace.front()] = (b);
			current_g[freeSpace.front()] = (g);
			current_a[freeSpace.front()] = (a);

			initial_r[freeSpace.front()] = (r);
			initial_g[freeSpace.front()] = (g);
			initial_b[freeSpace.front()] = (b);
			initial_a[freeSpace.front()] = (a);

			doRender[freeSpace.front()] = (true);

			localParticleFrame[freeSpace.front()] = 0;
			freeSpace.pop_front();
		}
	}
}

void ParticleDB::Stop()
{
	emmissionAllowed = false;
}

void ParticleDB::Play()
{
	emmissionAllowed = true;
}

void ParticleDB::Burst()
{
	GenerateNewParticles();
}