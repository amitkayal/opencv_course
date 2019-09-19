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

cv::Mat cloneImage;  // Source clone image
cv::Mat blemishImage;  // Blemish Image
cv::Point center; //center of the blemishes
cv::Rect roi; // Region of Interest for blemishes
unsigned int blemishSize = 30;  // Blemish size, by default

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

cv::Rect findBestSquare(cv::Mat img)
{
  float minVal = 1000;
  cv::Rect bestSquare, checkSquare;
  int nSquares = 3;
  int radius = blemishSize / 2;
  cv::Point startPosition (center.x - nSquares * radius,
      center.y - nSquares * radius);
  cv::Point newPosition(startPosition);

  // check all the squares in a nSquares x nSquares neighborhood around the required point
  for (int i = 0; i < nSquares; i++)
  {
    for (int j = 0; j < nSquares; j++)
    {
      // take a square from the neighborhood of the center
      newPosition.x = startPosition.x + i * 2 * radius;
      newPosition.y = startPosition.y + j * 2 * radius;

      // check for out of bounds
      if (newPosition.x + 2 * radius > img.cols || newPosition.x < 0 || 
          newPosition.y + 2 * radius > img.rows || newPosition.y < 0)
        continue;

      checkSquare = cv::Rect(newPosition.x, newPosition.y, 2 * radius,
          2 * radius );

      //find the gradient image
      Mat sobelX, SobelY;
      cv::Sobel(cloneImage(checkSquare), sobelX, CV_32F, 1, 0);
      cv::Sobel(cloneImage(checkSquare), SobelY, CV_32F, 0, 1);

      //find a measure of roughness of the square block
      float meanSobelX = cv::mean(cv::mean(abs(sobelX))).val[0];
      float meanSobelY = cv::mean(cv::mean(abs(SobelY))).val[0];

      //if it is smoother than previous ones update the best square
      if ((meanSobelX + meanSobelY) < minVal)
      {
        minVal = meanSobelX + meanSobelY;
        bestSquare = checkSquare;
      }
      else
        continue;
    }
  }
  return bestSquare;
}

// function which will be called on mouse input
void onMouse( int event, int x, int y, int flags, void* userdata)
{
  // get left click from the mouse
  if( event == EVENT_LBUTTONDOWN )  {

    center = Point(x,y);
    int radius = blemishSize / 2;
    roi = cv::Rect(center.x - radius, center.y - radius, 2 * radius, 2 * radius);

    // check if the selected region is on the boundaries
    if (roi.x + roi.width > cloneImage.cols || roi.x < 0 ||
        roi.y + roi.height > cloneImage.rows || roi.y < 0)
      return;

    blemishImage = cloneImage(roi).clone();

    // find the smoothest region around the marked point
    cv::Rect bestSquare = findBestSquare(cloneImage);

    cv::Mat smoothRegion = cloneImage(bestSquare);

    // Create a circular white mask of the same size as the smooth region
    cv::Mat mask = cv::Mat::zeros(blemishImage.rows, blemishImage.cols,
        blemishImage.depth());
    cv::circle(mask, cv::Point(radius, radius), radius,
        cv::Scalar(255, 255, 255), -1, 8);

    // Perform Seamless Cloning
    cv::seamlessClone(smoothRegion, cloneImage, mask,center, cloneImage,
        cv::NORMAL_CLONE);

    cv::imshow("Blemish Remover", cloneImage);
  }

  // added functionality for UNDO-ing the last modification
  if( event == EVENT_RBUTTONDOWN )
  {
    cv::Mat mask = 255 * cv::Mat::ones(blemishImage.rows, blemishImage.cols,
        blemishImage.depth());
    cv::seamlessClone(blemishImage, cloneImage, mask, center, cloneImage,
        cv::NORMAL_CLONE);
    cv::imshow("Blemish Remover",cloneImage);
  }
}

int main(int argc, char** argv)
{
  std::cout << "An utility to Remove Blemishes." << std::endl;
  std::cout << "Number of Input Arguments = " << argc << std::endl;
  if (argc == 1)
  {
    std::cout << "Get Help by blemishRemove -h" << std::endl;
    return 0;
  }
  
  if(cmdOptionExists(argv, argv + argc, "-h"))
  {
    std::cout << "blemishRemove -i inputFileName -s blemishSize ";
    std::cout <<  "-o outputFileName " << std::endl;    
    std::cout << "inputFileName -- Path of the file name where blemishes ";
    std::cout << "need to be removed, mandatory." << std::endl;
    std::cout << "blemishSize -- the blemishe size, optional." << std::endl;
    std::cout << "outputFileName -- Path of the file name where Blemishes ";
    std::cout << "removed is written, by pressing S key, optional.";
    std::cout << std::endl;    
    
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
     
    cmdOption = getCmdOption(argv, argv + argc, "-s");
        
    if (cmdOption.length() > 0)
    {
      blemishSize = std::stoi(cmdOption); 
    }
    
    std::cout << "Blemish Size = " << blemishSize << std::endl;
    
    cmdOption = getCmdOption(argv, argv + argc, "-o");
    std::string outputFilename = "";
    if (cmdOption.length() > 0)
    {
      outputFilename = cmdOption; 
    }
    std::cout << "Output File Name = " << outputFilename << std::endl;
    
  
    cv::Mat srcImage = cv::imread(inputFilename);

    if(srcImage.empty())
    {
      std::cout << "Input image is not a valid." << std::endl;
      return -1;
    }      
     
    //Create a window
    namedWindow("Blemish Remover", WINDOW_NORMAL); 
    
    cloneImage = srcImage.clone();

    setMouseCallback( "Blemish Remover", onMouse, &cloneImage);
    while(1)
    {
      cv::imshow("Blemish Remover", cloneImage);

      char k = waitKey(0);
      if ( k == 27)  //Escape key board event
        break;
        
      if ( k == 's' )
        cv::imwrite(outputFilename, cloneImage);
    }
    return 0;       
  }
}
