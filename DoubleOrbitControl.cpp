#include "framework.h"
#include "DoubleOrbitControl.h"


const float kTwoPi = 6.283185f;


DoubleOrbitControl::DoubleOrbitControl(const float left_center_x, const float left_center_y, const float radius,
                                       const float duration_secs)
	: left_center_x_(left_center_x),
	  left_center_y_(left_center_y),
	  radius_(radius),
	  duration_secs_(duration_secs),
	  isOrbitingLeft_(true),
	  current_angle_(0.0f)
{
	right_center_x_ = left_center_x_ + (2.0f * radius_);
	right_center_y_ = left_center_y_;
	current_x_ = current_y_ = 0.0f;
	CalculateCurrentPosition();
}


void DoubleOrbitControl::Update()
{
	//TODO: is GetDt deterministic?
	current_angle_ += kTwoPi * CP_System_GetDt() / duration_secs_;
	if (current_angle_ > kTwoPi)
	{
		isOrbitingLeft_ = !isOrbitingLeft_;
		current_angle_ = fmod(current_angle_, kTwoPi);
	}

	CalculateCurrentPosition();
}


void DoubleOrbitControl::CalculateCurrentPosition()
{
	if (isOrbitingLeft_)
	{
		current_x_ = left_center_x_ + (cos(current_angle_) * radius_);
		current_y_ = left_center_y_ + (sin(current_angle_) * radius_);
	}
	else
	{
		current_x_ = right_center_x_ - (cos(current_angle_) * radius_);
		current_y_ = right_center_y_ + (sin(current_angle_) * radius_);
	}
}