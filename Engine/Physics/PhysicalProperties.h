#pragma once

struct PhysicalProperties {
public:
    [[summary("The coefficient of friction for the object. Higher values result in more friction.")]]
    double Friction = 0.5; // Coefficient of friction
    [[summary("The coefficient of restitution for the object. Higher values result in more restitution.")]]
    double Restitution = 0.5; // Bounciness
    [[summary("The density of the object. Higher values result in more dense objects.")]]
    double Density = 1.0;
};