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
int toleranceFactor = 128;
int toleranceMax = 255;

Mat toleranceImage;
cv::Point blueColorTolerance;
cv::Point greenColorTolerance;
cv::Point redColorTolerance;

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

void UpdateTolerances()
{
  cv::Rect box;
  box.width = std::abs(endPoint.x - startPoint.x);
  box.height = std::abs(endPoint.y - startPoint.y);
  box.x = std::min(startPoint.x, endPoint.x);
  box.y = std::min(startPoint.y, endPoint.y);
  Mat colorPatch(toleranceImage, box); 
  
  //Find maximum and minimum colors for each channel
  cv::Mat colorChannels[3];
  cv::split(colorPatch, colorChannels);
  double min, max;
  cv::minMaxIdx(colorChannels[0], &min, &max);  
  blueColorTolerance = 
      cv::Point(static_cast<int>(min),static_cast<int>(max));
  cv::minMaxIdx(colorChannels[1], &min, &max);  
  greenColorTolerance = 
      cv::Point(static_cast<int>(min),static_cast<int>(max));
  cv::minMaxIdx(colorChannels[2], &min, &max);  
  redColorTolerance = 
      cv::Point(static_cast<int>(min),static_cast<int>(max));        
}

void toleranceUpdate(int, void*)
{
  std::cout << "Tolerance Value = " << toleranceFactor << std::endl;
  for (int y = 0; y < toleranceImage.cols; y++)
  {
    for (int x = 0; x < toleranceImage.rows; x++)
    {
      cv::Vec3b pixel = toleranceImage.at<Vec3b>(x, y);
      cv::Vec3b newPixel;      
      newPixel.val[1] = pixel.val[1] + (toleranceFactor - 128);      
      toleranceImage.at<Vec3b>(x, y) = newPixel;
    }
  }
  UpdateTolerances();
  cv::imshow("Chroma Keying", toleranceImage);
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
      
    if ((inputFilename.length() > 0) && (backgroundImage.length() > 0))
    {  
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
    
      // Release the video capture object
      videoCap.release();
      
      if (isPatch)
      {
        toleranceImage = videoFrame.clone();
        
        cv::createTrackbar("Tolerance", "Chroma Keying", &toleranceFactor, toleranceMax, toleranceUpdate);             
              
        while (1)
        {
          cv::rectangle(toleranceImage, startPoint, endPoint,
            cv::Scalar(0, 0, 255), 5);
          cv::imshow("Chroma Keying", toleranceImage);  
          // Press ESC on keyboard to exit
          char c = (char)waitKey(25);
          if (c == 27)  //Escape key board event
            break; 
            
          if ((c == 114) || (c == 82)) //R or r key board event
            break; 
        }
      
        std::cout << "Get the mask and dispaly it. " << std::endl;
        cv::Scalar lower_threshold(0, 100, 0);  //RGB
        cv::Scalar upper_threshold(redColorTolerance.y, 
          greenColorTolerance.y, blueColorTolerance.y);
        cv::Mat maskedFrame;
        cv::inRange(toleranceImage, lower_threshold, upper_threshold,
          maskedFrame);
          
        cv::VideoCapture videoCapBack(inputFilename);
        
        while (1)
        {
          // Capture frame-by-frame
          videoCap >> videoFrame;
        
          cv::imshow("Chroma Keying", maskedFrame);  
          // Press ESC on keyboard to exit
          char c = (char)waitKey(25);
          if (c == 27)  //Escape key board event
            break;         
        }
        videoCapBack.release();
      }
      // Closes all the frames
      
      destroyAllWindows();
    
      return 0;       
    } else 
    {
      std::cout << "Not valid path images path." << std::endl;
    }
  } 
  return 0;
}    
