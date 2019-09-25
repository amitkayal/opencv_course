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

cv::Point startPoint, endPoint;
bool isPatch;


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
  if (!isPatch)
  {
    // Event to be taken when left mouse button is pressed
    if (event == cv::EVENT_LBUTTONDOWN)
    {  
      startPoint = Point(x, y);        
    }
    // Event to be taken when left mouse button is released
    else if (event == cv::EVENT_LBUTTONUP)
    {    
      endPoint = Point(x, y);    
      isPatch = true;            
    } 
  }
}

int main(int argc, char** argv)
{
  std::cout << "An utility for Chroma Keying." << std::endl;
  std::cout << "Number of Input Arguments = " << argc << std::endl;
  if (argc == 1)
  {
    std::cout << "Get Help by chromaKeying -h" << std::endl;
    return 0;
  }
  
  if(cmdOptionExists(argv, argv + argc, "-h"))
  {
    std::cout << "chromaKeying -i inputFileName -b backgroundImage ";
    std::cout <<  std::endl;    
    std::cout << "inputFileName -- Path of the file name for Chroma Keying, ";
    std::cout << "mandatory." << std::endl;
    std::cout << "backgroundImage -- the background Image, "; 
    std::cout << "mandatory." << std::endl;
    
    return 0;
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
    std::string backgroundImage = "";
    if (cmdOption.length() > 0)
    {
      backgroundImage = cmdOption; 
    }
    std::cout << "Output File Name = " << backgroundImage << std::endl;  
      

    cv::VideoCapture videoCap(inputFilename);
    // Check if camera opened successfully and read a frame from the object
    // videoCap
    if (!videoCap.isOpened())
    {
      std::cout << "Error opening video stream or file" << std::endl;
      return -1;
    }
  
    //Create a window
    cv::namedWindow("Chroma Keying", WINDOW_NORMAL);     

    cv::setMouseCallback("Chroma Keying", onMouse, NULL);
    
    isPatch = false;
    cv::Mat videoFrame;
    while(1)
    { 
      
      // Capture frame-by-frame
      videoCap >> videoFrame;
      
      // If the frame is empty, break immediately
      if (videoFrame.empty())
        break;
    
      if (isPatch)
        break;      
      
      // Display the resulting frame
      cv::imshow("Chroma Keying", videoFrame);   
      
      // Wait for sometime.
      char c = (char)waitKey(25);
    }
    
    // When everything done, release the video capture object
    videoCap.release();
    if (isPatch)
    {
      while (1)
      {
        cv::rectangle(videoFrame, startPoint, endPoint, cv::Scalar(0,0,255), 5);
        cv::imshow("Chroma Keying", videoFrame);  
        // Press ESC on keyboard to exit
        char c = (char)waitKey(25);
        if (c == 27)  //Escape key board event
          break; 
      }
    }
    // Closes all the frames
    destroyAllWindows();
  
    return 0;       
  }
}    
