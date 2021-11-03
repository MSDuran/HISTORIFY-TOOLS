#include <iostream>
#include <filesystem>
#include <libzippp/libzippp.h>
#include <fstream>
#include<opencv2/opencv.hpp>

int main() {

	if (std::filesystem::exists(std::string("segmented_images.zip")))
	{
		std::cout << "segmented_images.zip exists!\n";
	}
	else {
		std::cout << "segmented_images.zip does not exist!\nPlease Download it.\nLink:https://drive.google.com/file/d/1VF5yfWZ4CuEwUN6O72h8uadcnd4bCNAI/view?usp=sharing\n";
	}
	std::filesystem::create_directory("segmented_images");
	libzippp::ZipArchive zf{ "segmented_images.zip" };
	if (!zf.open(libzippp::ZipArchive::ReadOnly)) {
		std::cout << "Unable to open the zip!\n";
	}

	std::vector<libzippp::ZipEntry> entries = zf.getEntries();
	std::vector<libzippp::ZipEntry>::iterator it;
	libzippp::ZipEntry entry;
	for (it = entries.begin() + 1; it != entries.end(); ++it) {
		libzippp::ZipEntry entry = *it;
		std::string name = entry.getName();
		int size = entry.getSize();
		std::string binaryData = entry.readAsText();
		//std::cout << name.substr(name.find("/") + 1, name.length() - name.find("/"));
		std::ofstream myfile("segmented_images/" + name.substr(name.find("/") + 1, name.length() - name.find("/")), std::ofstream::binary);
		//myfile.open(name);
		myfile << binaryData;
		myfile.close();
	}
	zf.close();

	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("segmented_images"))
	{
		cv::Mat image1 = cv::imread(dirEntry.path().string());
		cv::Size size = image1.size();
		std::cout << size.width << "X" << size.height << "\n" << dirEntry.path().string() << " ";
		cv::Rect crop_region(size.width / 2, 0, size.width / 2, size.height);
		cv::Mat image2 = image1(crop_region);
		cv::imwrite(dirEntry.path().string(), image2);
	}
	return 0;
}
