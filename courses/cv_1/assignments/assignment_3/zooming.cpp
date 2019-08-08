#include <iostream>
#include <string>
#include <fstream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int maxScaleUp = 100;
int scaleFactor = 1;
int scaleType = 0;
int maxType = 1;
cv::Mat sourceImage;

string windowName = "Resize Image";
string trackbarValue = "Scale";
string trackbarType = "Type: \n 0: Scale Up \n 1: Scale Down";

void scaleImage(int, void*);

int main(int argc, char** argv)
{
  if (argc > 1)
  {
    std::string inputFilename = argv[1];
    
    std::ifstream inFile(inputFilename);
    if (inFile.good())
    {
      std::cout << "Input File is valid. " << std::endl;
      
      // load an image
      sourceImage = cv::imread(inputFilename);

      // Create a window to display results
      cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

      cv::createTrackbar(trackbarValue, windowName, &scaleFactor, maxScaleUp,
          scaleImage);
      cv::createTrackbar(trackbarType, windowName, &scaleType, maxType,
          scaleImage);

      scaleImage(25,0);

      while (true)
      {
        int c;
        c = cv::waitKey(20);
        if (static_cast<char>(c) == 27)
          break;
      }
      cv::destroyWindow(windowName); 
    } else
    {
      std::cout << "Input File, " <<  inputFilename << " does not exist.";
      std::cout << std::endl;
    }    
  } else
  {
    std::cout << "In Sufficient arguments." << std::endl;
    std::cout << "zooming file_name is the working model." << std::endl;
  }
  return 0;
}

// Callback functions
void scaleImage(int, void*)
{
	double scaleFactorDouble;
  if (scaleType == 0)
  {  
    scaleFactorDouble = 1 + scaleFactor/100.0;    
  } else
  {
    scaleFactorDouble = 1 - scaleFactor/100.0;    
  }  
  
  if ((scaleFactorDouble == 0) || (scaleFactorDouble == 1))
  {
    scaleFactorDouble = 1;
  } 
  	
	cv::Mat scaledImage;
	cv::resize(sourceImage, scaledImage, cv::Size(), scaleFactorDouble,
      scaleFactorDouble, cv::INTER_LINEAR);  
	cv::imshow(windowName, scaledImage);
}

