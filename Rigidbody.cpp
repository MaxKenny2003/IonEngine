#include "Rigidbody.h"
#include <iostream>

b2Vec2 Rigidbody::GetPosition()
{
	if (body == nullptr)
	{
		return b2Vec2(x, y);
	}
	return body->GetPosition();
}

float Rigidbody::GetRotation()
{
	return body->GetAngle() * (180.0f / b2_pi);
}

void Rigidbody::defineShapeAndFixture()
{
	if (!hasCollider && !has_trigger)
	{
		b2PolygonShape phantom_shape;
		phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);

		b2FixtureDef phantom_fixture_def;
		phantom_fixture_def.shape = &phantom_shape;
		phantom_fixture_def.density = density;

		phantom_fixture_def.isSensor = true;
		phantom_fixture_def.userData.pointer = reinterpret_cast<uintptr_t> (associatedActor);
		phantom_fixture_def.filter.categoryBits = 0b0100;
		phantom_fixture_def.filter.maskBits = ~0b1011;
		body->CreateFixture(&phantom_fixture_def);
	}
	if (hasCollider)
	{
		b2PolygonShape my_shape;
		b2CircleShape my_circle;
		b2FixtureDef fixture;
		fixture.isSensor = false;
		
		if (colliderType == "circle")
		{
			my_circle.m_radius = radius;
			fixture.shape = &my_circle;
			fixture.density = density;
			fixture.friction = friction;
			fixture.restitution = bounciness;
		}
		else
		{
			my_shape.SetAsBox(width * 0.5f, height * 0.5f);
			fixture.shape = &my_shape;
			fixture.density = density;
			fixture.friction = friction;
			fixture.restitution = bounciness;
		}
		fixture.filter.categoryBits = ColliderCategory;
		fixture.filter.maskBits = ~SensorCategory;
		fixture.userData.pointer = reinterpret_cast<uintptr_t>(associatedActor);
		body->CreateFixture(&fixture);
	}
	if (has_trigger)
	{
		b2PolygonShape my_shape;
		b2CircleShape my_circle;
		b2FixtureDef fixture;
		fixture.isSensor = true;

		if (triggerType == "circle")
		{
			my_circle.m_radius = triggerRadius;
			fixture.shape = &my_circle;
			fixture.density = density;
			fixture.friction = friction;
			fixture.restitution = bounciness;
		}
		else
		{
			my_shape.SetAsBox(triggerWidth * 0.5f, triggerHeight * 0.5f);
			fixture.shape = &my_shape;
			fixture.density = density;
			fixture.friction = friction;
			fixture.restitution = bounciness;
		}
		fixture.filter.categoryBits = SensorCategory;
		fixture.filter.maskBits = ~ColliderCategory;
		fixture.userData.pointer = reinterpret_cast<uintptr_t>(associatedActor);
		body->CreateFixture(&fixture);
	}
}

void Rigidbody::addForce(b2Vec2 force)
{
	body->ApplyForceToCenter(force, true);
}

void Rigidbody::setVelocity(b2Vec2 velocity)
{
	body->SetLinearVelocity(velocity);
}

void Rigidbody::setPosition(b2Vec2 position)
{
	if (body == nullptr)
	{
		x = position.x;
		y = position.y;
	}
	else
	{
		float currentRotation = body->GetAngle();
		body->SetTransform(position, rotation);
	}
}

void Rigidbody::setRotation(float newRotation)
{
	// Set rotation in degrees clockwise
	float rotationRadians = (newRotation * (b2_pi / 180.0f));
	body->SetTransform(body->GetPosition(), rotationRadians);
}

void Rigidbody::setAngularVelocity(float degreesClockwise)
{
	float angularVelocityRadians = (degreesClockwise * (b2_pi / 180.0f));
	body->SetAngularVelocity(angularVelocityRadians);
}

void Rigidbody::setGravityScale(float newGravityScale)
{
	body->SetGravityScale(newGravityScale);
}

void Rigidbody::setUpDirection(b2Vec2 direction)
{
	direction.Normalize();
	float newAngleRadians = glm::atan(direction.x, -direction.y);
	body->SetTransform(body->GetPosition(), newAngleRadians);

}

void Rigidbody::setRightDirection(b2Vec2 direction)
{
	direction.Normalize();
	float newAngleRadians = glm::atan(direction.x, -direction.y);
	newAngleRadians -= b2_pi / 2.0f;
	body->SetTransform(body->GetPosition(), newAngleRadians);
}

b2Vec2 Rigidbody::getVelocity()
{
	return body->GetLinearVelocity();
}

float Rigidbody::getAngularVelocity()
{
	return body->GetAngularVelocity() * (180.0f / b2_pi);
}

float Rigidbody::getGravityScale()
{
	return body->GetGravityScale();
}

b2Vec2 Rigidbody::getUpDirection()
{
	float angle = body->GetAngle();
	b2Vec2 result = b2Vec2(glm::sin(angle), -glm::cos(angle));
	result.Normalize();
	return result;
}

b2Vec2 Rigidbody::getRightDirection()
{
	float angle = body->GetAngle();
	b2Vec2 result = b2Vec2(glm::sin(angle + (b2_pi/2.0f)), -glm::cos(angle + (b2_pi/2.0f)));
	result.Normalize();
	return result;
}

void Rigidbody::OnDestroy()
{
	Game->physics_world->DestroyBody(body);
}

void Rigidbody::OnStart()
{
	Game->defineBody(*this);
	defineShapeAndFixture();
}