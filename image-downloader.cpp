//link regex     : http:\/\/www.cs.ucf.edu\/~aroshan\/index_files\/Dataset_PitOrlManh\/images\/[0-9]{6}_[0-9].jpg
//filename regex : >[0-9]{6}_[0-9].jpg

#include <iostream> 
#include <curl/curl.h> 
#include <cstdio>
#include <string>
#include <regex>
#include <vector>
#include <boost/timer/timer.hpp>
#include <algorithm>
#include <thread>
#include <fstream>
#include <execution>
#include <mutex>
#include "hs/hs.h"
#include <algorithm>
#include <cstdlib>
#include <random>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <atomic>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <array>
#include <libzippp/libzippp.h>

#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "advapi32.lib")
#pragma comment (lib, "Crypt32.lib")

#define NUMBER_OF_FILES 62'058
#define CONTENT_LENGTH 12'660'695
#define SEPARATOR "\\"
std::string readBuffer;
std::string fileNameRegex = ">[0-9]{6}_[0-9].jpg";

std::vector<std::string> fileNames;
std::vector<std::string> fileNames_down;

std::mutex my_mutex;

unsigned int down_img = 0;

CURL* curl_obj;
CURLcode curl_code;
FILE* fp;
char url[100] = "http://www.cs.ucf.edu/~aroshan/index_files/Dataset_PitOrlManh/images/";
void down_image(const std::string& fileName) {
	CURL* curl_obj_;
	CURLcode curl_code_;
	FILE* fp_;
	char url_[100] = "http://www.cs.ucf.edu/~aroshan/index_files/Dataset_PitOrlManh/images/";

	curl_obj_ = curl_easy_init();
	fp_ = fopen(fileName.c_str(), "wb");
	if (fp_ == NULL) {
		std::cout << "Error\n";
	}
	strcpy(url_, "http://www.cs.ucf.edu/~aroshan/index_files/Dataset_PitOrlManh/images/");
	strcat(url_, fileName.c_str());
	curl_easy_setopt(curl_obj_, CURLOPT_CONNECTTIMEOUT, 10L);
	curl_easy_setopt(curl_obj_, CURLOPT_URL, url_);
	curl_easy_setopt(curl_obj_, CURLOPT_WRITEDATA, fp_);
	curl_code_ = curl_easy_perform(curl_obj_);
	{
		std::lock_guard<std::mutex> guard(my_mutex);
		std::cout << url_ << "\n";
	}
	if (curl_code_) {
		std::cout << "Error: " << curl_code_ << "\n";
	}
	fflush(fp_);
	fclose(fp_);
	//std::this_thread::sleep_for(std::chrono::milliseconds(250));
}
void resize_images() {
	static std::array<int, 9>interpolation_flags{ cv::InterpolationFlags::INTER_NEAREST,
												  cv::InterpolationFlags::INTER_LINEAR,
												  cv::InterpolationFlags::INTER_CUBIC,
												  cv::InterpolationFlags::INTER_AREA,
												  cv::InterpolationFlags::INTER_LANCZOS4,
												  cv::InterpolationFlags::INTER_LINEAR_EXACT,
												  cv::InterpolationFlags::INTER_NEAREST_EXACT,
												  cv::InterpolationFlags::WARP_FILL_OUTLIERS,
												  cv::InterpolationFlags::WARP_INVERSE_MAP,
	};
	static std::array<std::string, 9> interpolation_strings{ "INTER_NEAREST",
															 "INTER_LINEAR",
															 "INTER_CUBIC",
															 "INTER_AREA",
															 "INTER_LANCZOS4",
															 "INTER_LINEAR_EXACT",
															 "INTER_NEAREST_EXACT",
															 "WARP_FILL_OUTLIERS",
															 "WARP_INVERSE_MAP"
	};

	//std::cout <<"Size: "<< fileNames_down.size();
	for (size_t i = 0; i < down_img; i++)
	{
		if (std::filesystem::exists(std::filesystem::current_path().string() + SEPARATOR + fileNames_down[i]))
		{
			if (auto var = std::filesystem::path((std::filesystem::current_path().string() + SEPARATOR + fileNames_down[i])); std::filesystem::file_size(var) > 0)
			{
				cv::Mat input_img = cv::imread(std::filesystem::current_path().string() + SEPARATOR + fileNames_down[i]);
				cv::Mat resized_img;
				cv::resize(input_img, resized_img, cv::Size(256, 256), interpolation_flags[0]);
				cv::imwrite(std::filesystem::current_path().string() + SEPARATOR + fileNames_down[i].substr(0, 8) + interpolation_strings[0] + ".jpg", resized_img);
			}
			std::remove((std::filesystem::current_path().string() + SEPARATOR + fileNames_down[i]).c_str());
		}
	}
	//zip images here
	libzippp::ZipArchive zf("archive.zip");
	zf.open(libzippp::ZipArchive::Write);
	//const char* textData = "Hello,World!";
	//zf.addData("helloworld.txt", textData, 12);
	for (size_t i = 0; i < down_img; i++)
	{
		//std::cout << fileNames[i] << " " << std::filesystem::current_path().string() + SEPARATOR + fileNames[i].substr(0, 8) + interpolation_strings[0] + ".jpg";
		std::cout << "adding " << fileNames_down[i] << " to the zip file" << "\n";
		if (std::filesystem::exists(std::filesystem::current_path().string() + SEPARATOR + fileNames_down[i].substr(0, 8) + interpolation_strings[0] + ".jpg"))
		{
			zf.addFile(fileNames_down[i], std::filesystem::current_path().string() + SEPARATOR + fileNames_down[i].substr(0, 8) + interpolation_strings[0] + ".jpg");
		}
	}
	zf.close();
	for (size_t i = 0; i < fileNames_down.size(); i++)
	{
		if (std::filesystem::exists(std::filesystem::current_path().string() + SEPARATOR + fileNames_down[i].substr(0, 8) + interpolation_strings[0] + ".jpg"))
		{
			std::remove((std::filesystem::current_path().string() + SEPARATOR + fileNames_down[i].substr(0, 8) + interpolation_strings[0] + ".jpg").c_str());
		}
	}
}
size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
	readBuffer.append(ptr, size * nmemb);
	return size * nmemb;
}

int callback(unsigned int id, unsigned long long from, unsigned long long to,
	unsigned int flags, void* context)
{
	fileNames.emplace_back("" + readBuffer.substr(from + 1, to - from - 1));
	return 0;
}


int main() {
	fileNames.reserve(NUMBER_OF_FILES);
	readBuffer.reserve(CONTENT_LENGTH);
	curl_global_init(CURL_GLOBAL_ALL);

	boost::timer::auto_cpu_timer t(3, "\nWhole processes Took %w seconds\n");
	CURL* curl_obj;
	CURLcode curl_code;
	{
		boost::timer::auto_cpu_timer t(3, "took %w seconds\n");
		std::cout << "Connecting to the dataset...\n";
		curl_obj = curl_easy_init();
		if (curl_obj) {
			curl_easy_setopt(curl_obj, CURLOPT_URL, "http://www.cs.ucf.edu/~aroshan/index_files/Dataset_PitOrlManh/images/");
			curl_easy_setopt(curl_obj, CURLOPT_WRITEFUNCTION, write_callback);
			curl_code = curl_easy_perform(curl_obj);
			if (curl_code) {
				std::cout << "Cannot grab the curl_obj!\n";
			}
		}
	}
	{
		boost::timer::auto_cpu_timer t(3, "took %w seconds\n");
		std::cout << "Parsing the dataset...\n";
		hs_database_t* db;
		hs_compile_error_t* cerr;
		hs_scratch_t* scratch = NULL;
		hs_compile(">[0-9]{6}_[0-9].jpg", HS_FLAG_SOM_LEFTMOST, HS_MODE_BLOCK, NULL, &db, &cerr);
		hs_alloc_scratch(db, &scratch);
		hs_scan(db, readBuffer.c_str(), readBuffer.length(), 0, scratch, callback, NULL);
	}
	std::cout << "Number of pictures in the dataset: " << fileNames.size() << "\n";
	std::cout << "how many images do you want to be downloaded?\n";
	std::cin >> down_img;
	std::cout << "Downloading " << down_img << " randomly chosen pictures from the dataset\n";
	std::sample(fileNames.begin(), fileNames.end(), std::back_inserter(fileNames_down), down_img, std::mt19937{ std::random_device{}() });
	{
		boost::timer::auto_cpu_timer t(3, "took %w seconds\n");
		std::cout << "Downloading...\n";
		boost::asio::thread_pool pool(std::thread::hardware_concurrency());
		for (size_t i = 0; i < down_img; i++)
		{
			boost::asio::post(pool, std::bind(down_image, fileNames_down[i]));
		}
		pool.join();
		curl_easy_cleanup(curl_obj);
	}
	resize_images();
	curl_global_cleanup();
	return 0;
}
