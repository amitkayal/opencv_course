#include <math.h>

#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

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
void onMouse(int event, int x, int y, int flags, void* userdata)
{
  
}

int main(int argc, char** argv)
{
  std::cout << "An utility for Document Scanner." << std::endl;
  std::cout << "Number of Input Arguments = " << argc << std::endl;
  if (argc == 1)
  {
    std::cout << "Get Help by document Scanner -h" << std::endl;
    return 0;
  }
  
  if(cmdOptionExists(argv, argv + argc, "-h"))
  {
    std::cout << "chromaKeying -i inputFileName -o outputFileName";
    std::cout <<  std::endl;    
    std::cout << "inputFileName -- Path of the file name for Chroma Keying, ";
    std::cout << "mandatory." << std::endl;
    std::cout << "outputFileName -- the scanned image path, "; 
    std::cout << "mandatory." << std::endl;    
  } else 
  {  
    std::string cmdOption = getCmdOption(argv, argv + argc, "-i");
    
    std::string inputFilename = " ";  
    if (cmdOption.length() > 0)
    {
      inputFilename = cmdOption; 
    }
    std::cout << "Input File Name = " << inputFilename << std::endl;  
        
    
    cmdOption = getCmdOption(argv, argv + argc, "-b");
    std::string outputFilename = "";
    if (cmdOption.length() > 0)
    {
      outputFilename = cmdOption; 
    }
    std::cout << "Output File Name = " << outputFilename << std::endl;

    cv::Mat inputImage = imread(inputFilename);
        
    int width = inputImage.cols;
    int height = inputImage.rows;       
            
    std::cout << "Size of Input Image = " << width << "x" << height
      << std::endl;  
    
    //Create a window
    cv::namedWindow("Document Scanner", WINDOW_NORMAL); 

    while (1)
    {      
      cv::imshow("Chroma Keying", inputImage);  
      // Press ESC on keyboard to exit
      char c = (char)waitKey(25);
      if (c == 27)  //Escape key board event
        break;       
    }  
  }
  
  return 0;
}

