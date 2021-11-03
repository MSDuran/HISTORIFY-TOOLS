#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <cstdarg>
#include <array>

int main() {
	std::array<int, 9>interpolation_flags{ cv::InterpolationFlags::INTER_NEAREST,
											cv::InterpolationFlags::INTER_LINEAR,
											cv::InterpolationFlags::INTER_CUBIC,
											cv::InterpolationFlags::INTER_AREA,
											cv::InterpolationFlags::INTER_LANCZOS4,
											cv::InterpolationFlags::INTER_LINEAR_EXACT,
											cv::InterpolationFlags::INTER_NEAREST_EXACT,
											cv::InterpolationFlags::WARP_FILL_OUTLIERS,
											cv::InterpolationFlags::WARP_INVERSE_MAP,
	};
	std::array<std::string, 9> interpolation_strings{ "INTER_NEAREST",
													  "INTER_LINEAR",
													  "INTER_CUBIC",
													  "INTER_AREA",
													  "INTER_LANCZOS4",
													  "INTER_LINEAR_EXACT",
													  "INTER_NEAREST_EXACT",
													  "WARP_FILL_OUTLIERS",
													  "WARP_INVERSE_MAP"
	};
	cv::Mat input_img = cv::imread("C:/Users/90543/source/repos/opencvExample/my_img.jpg");
	cv::Mat resized_img;
	for (size_t i = 0; i < interpolation_flags.size(); i++)
	{
		cv::resize(input_img, resized_img, cv::Size(256, 256), interpolation_flags[i]);
		cv::imwrite("C:/Users/90543/source/repos/opencvExample/my_img_resized_" + interpolation_strings[i] + ".jpg", resized_img);
	}
	return 0;
}
