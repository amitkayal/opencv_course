#include <math.h>

#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

using namespace cv;


// Source image
cv::Mat sourceImage;
//Region of Interest or Bounding Box points
cv::Point startPoint, endPoint;

bool isCrop;
bool isLeftDown;

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
  return std::find(begin, end, option) != end;
}

char* getCmdOption(char** begin, char** end, const std::string& option)
{
  char** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)
  {
      return *itr;
  }
  return "";
}

// function which will be called on mouse input
void onMouse(int action, int x, int y, int flags, void *userdata)
{
  // Action to be taken when left mouse button is pressed
  if (action == cv::EVENT_LBUTTONDOWN)
  {
    isLeftDown = true;
    startPoint = Point(x, y);    
  }
  // Action to be taken when left mouse button is released
  else if (action == cv::EVENT_LBUTTONUP)
  {
    endPoint = Point(x, y);    
    isCrop = true;    
  } else if (action == cv::EVENT_MOUSEMOVE)
  {
    if (isLeftDown)
    {
      endPoint = Point(x, y);        
      Mat boundImage = sourceImage.clone();
      cv::rectangle(boundImage, startPoint, endPoint, cv::Scalar(0,255,0), 2);
      cv::imshow("Window", boundImage);      
    }
  }  
}

void imageCrop(const std::string& inFile, const std::string& outFile,
    int imageType)
{
  std::cout << "imageCrop function is called. " <<  std::endl;
  sourceImage = cv::imread(inFile, imageType);
  
  //Create a Window
  cv::namedWindow("Window");
  cv::imshow("Window", sourceImage);
  
  // highgui function called when mouse events occur
  cv::setMouseCallback("Window", onMouse);
  
  isCrop = false;
  isLeftDown = false;
  
  // loop until the image the cropped. 
  while(!isCrop)
  {    
    cv::waitKey(20);    
  }
  
  cv::destroyWindow("Window"); 
  
  //Create a Region of Interest or Bounding Box.
  cv::Rect box;
  box.width = std::abs(endPoint.x - startPoint.x);
	box.height = std::abs(endPoint.y - startPoint.y);
	box.x = std::min(startPoint.x, endPoint.x);
	box.y = std::min(startPoint.y, endPoint.y);
	Mat cropImage(sourceImage,box); 
  
  //Write the Cropped image in the Output file 
  cv::imwrite(outFile, cropImage);  
}  
      

int main(int argc, char** argv)
{
  std::cout << "An Utility to crop the Image." << std::endl;
  std::cout << "Number of Input Arguments = " << argc << std::endl;
  if (argc == 1)
  {
    std::cout << "Get Help by image_crop -h" << std::endl;
  }
  
  if(cmdOptionExists(argv, argv + argc, "-h"))
  {
    std::cout << "image_crop -i inputFileName -type imageType " ;
    std::cout << "-o outputFileName \n" << std::endl;
    std::cout << "inputFileName -- Path of the file name which needs to be ";
    std::cout << "cropped, is mandatory." << std::endl;
    std::cout << "imageType -- An integer value, 1 for color, 0 for gray; ";
    std::cout << "default is color, is optional. " << std::endl;
    std::cout << "outputFileName -- Path of the file name which is ";
    std::cout << "cropped, is mandatory. " << std::endl;
  }
  
  std::string cmdOption = getCmdOption(argv, argv + argc, "-i");
  
  std::string inputFilename = " ";  
  if (cmdOption.length() > 0)
  {
    inputFilename = cmdOption; 
  }
  std::cout << "Input File Name = " << inputFilename << std::endl;
  
  cmdOption = getCmdOption(argv, argv + argc, "-o");
  std::string outputFilename = "";
  if (cmdOption.length() > 0)
  {
    outputFilename = cmdOption; 
  }
  std::cout << "Output File Name = " << outputFilename << std::endl;
  
  int imageType = 1;
  cmdOption = getCmdOption(argv, argv + argc, "-type");
  if (cmdOption.length() > 0)
  {
    imageType = std::stoi(cmdOption);
  }
  
  std::cout << "Image Type = " << imageType << std::endl;
  
  std::ifstream inFile(inputFilename);
  if (inFile.good())
  {
    std::cout << "Input File is valid. " << std::endl;
    imageCrop(inputFilename, outputFilename, imageType);    
  } else
  {
    std::cout << "Input File, " <<  inputFilename << " does not exist.";
    std::cout << std::endl;
  }
  
  return 0;
}
