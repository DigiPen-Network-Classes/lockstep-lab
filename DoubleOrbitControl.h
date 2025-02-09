#pragma once


class DoubleOrbitControl
{
public:
	DoubleOrbitControl(float left_center_x, float left_center_y, float radius, float duration_secs);

	void Update();

	inline float GetCurrentX() const { return current_x_; }
	inline float GetCurrentY() const { return current_y_; }
	
private:
	void CalculateCurrentPosition();
	
	float left_center_x_;
	float left_center_y_;
	float right_center_x_;
	float right_center_y_; 
	float radius_;
	float duration_secs_;
	bool isOrbitingLeft_;
	float current_angle_;
	float current_x_;
	float current_y_;
};

