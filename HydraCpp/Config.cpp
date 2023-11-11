#include <iostream>

class Config
{
public:struct {
	std::string path = "C:/F/astro/matlab/m1test/";
	std::string parameterDir = "/parametersCPP/";
	std::string lightDir = "/lights/";
	std::string ext = ".png";
	float detectionThreshold = 0.55;
	int maxStars = 15;
	int topMatches = 6;
	std::string filter = "R";
	std::string align = "R";
} config;
};