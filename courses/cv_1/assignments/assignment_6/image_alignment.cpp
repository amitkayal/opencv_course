#include "includeLibraries.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "matplotlibcpp.h"
#include "displayImages.h"

using namespace std;
using namespace cv;
using namespace matplotlibcpp;


//Step 1: Read Image

// Read 8-bit color image.
// This is an image in which the three channels are
// concatenated vertically.
Mat img =  imread(DATA_PATH + "images/emir.jpg", IMREAD_GRAYSCALE);

// Find the width and height of the color image
Size sz = img.size();
int height = sz.height / 3;
int width = sz.width; 

cout << sz;


// Extract the three channels from the gray scale image
vector<Mat>channels; 
channels.push_back(img( Rect(0, 0,         width, height)));
channels.push_back(img( Rect(0, height,    width, height))); 
channels.push_back(img( Rect(0, 2*height,  width, height)));


Mat blue = channels[0];
Mat green = channels[1];
Mat red = channels[2];

plt::figure_size(1200,300);
plt::subplot(1,3,1);
plt::imshow(blue);
auto pltImg = displayImage(blue);
plt::subplot(1,3,2);
plt::imshow(green);
pltImg = displayImage(green);
plt::subplot(1,3,3);
plt::imshow(red);
pltImg = displayImage(red);
pltImg

//Step 2: Detect Features

///
int MAX_FEATURES = 650;
float GOOD_MATCH_PERCENT = 0.1055f;
///

///
// Variables to store keypoints and descriptors
std::vector<KeyPoint> keypointsBlue, keypointsGreen, keypointsRed;
Mat descriptorsBlue, descriptorsGreen, descriptorsRed;

// Detect ORB features and compute descriptors.
Ptr<Feature2D> orb = ORB::create(MAX_FEATURES);
orb->detectAndCompute(green, Mat(), keypointsGreen, descriptorsGreen);
orb->detectAndCompute(blue, Mat(), keypointsBlue, descriptorsBlue);
orb->detectAndCompute(red, Mat(), keypointsRed, descriptorsRed);
///

//Step 3: Match Features

plt::figure_size(1200,300);
Mat img2;
drawKeypoints(blue, keypointsBlue, img2, Scalar(255,0,0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
plt::subplot(1,3,1);
plt::imshow(img2);
pltImg = displayImage(img2);

drawKeypoints(green, keypointsGreen, img2, Scalar(0,255,0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
plt::subplot(1,3,2);
plt::imshow(img2);
pltImg = displayImage(img2);

drawKeypoints(red, keypointsRed, img2, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
plt::subplot(1,3,3);
plt::imshow(img2);
pltImg = displayImage(img2);

pltImg

///
std::vector<DMatch> matchesBlueGreen;
Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");

///

// Match features between blue and Green channels
///
matcher->match(descriptorsBlue, descriptorsGreen, matchesBlueGreen, Mat());
///

// Sort matches by score
std::sort(matchesBlueGreen.begin(), matchesBlueGreen.end());

// Remove not so good matches
int numGoodMatches = matchesBlueGreen.size() * GOOD_MATCH_PERCENT;
matchesBlueGreen.erase(matchesBlueGreen.begin()+numGoodMatches, matchesBlueGreen.end());

// Draw top matches
Mat imMatchesBlueGreen;
drawMatches(blue, keypointsBlue, green, keypointsGreen, matchesBlueGreen, imMatchesBlueGreen);

plt::figure_size(800,300);
plt::imshow(imMatchesBlueGreen);
pltImg = displayImage(imMatchesBlueGreen);
pltImg

// Match features between Red and Green channels
///
std::vector<DMatch> matchesRedGreen;
matcher->match(descriptorsRed, descriptorsGreen, matchesRedGreen, Mat());
///

// Sort matches by score
std::sort(matchesRedGreen.begin(), matchesRedGreen.end());

// Remove not so good matches
numGoodMatches = matchesRedGreen.size() * GOOD_MATCH_PERCENT;
matchesRedGreen.erase(matchesRedGreen.begin()+numGoodMatches, matchesRedGreen.end());

// Draw top matches
Mat imMatchesRedGreen;
drawMatches(red, keypointsRed, green, keypointsGreen, matchesRedGreen, imMatchesRedGreen);

plt::figure_size(800,300);
plt::imshow(imMatchesRedGreen);
pltImg = displayImage(imMatchesRedGreen);
pltImg

//Step 4: Calculate Homography

// Extract location of good matches
///
std::vector<Point2f> points1, points2;
for (size_t i = 0; i < matchesBlueGreen.size(); i++)
{
    points1.push_back(keypointsBlue[matchesBlueGreen[i].queryIdx].pt);    
    points2.push_back(keypointsGreen[matchesBlueGreen[i].trainIdx].pt);
}
///

// Find homography
///
Mat hBlueGreen = findHomography(points1, points2, RANSAC);
///

// Extract location of good matches
///
std::vector<Point2f> points3, points4;

for (size_t i = 0; i < matchesRedGreen.size(); i++)
{
    points3.push_back(keypointsRed[matchesRedGreen[i].queryIdx].pt);
    points4.push_back(keypointsGreen[matchesRedGreen[i].trainIdx].pt);
}
///

// Find homography
///
Mat hRedGreen = findHomography(points3, points4, RANSAC);
///

//Step 5: Warping Image
// Use homography to find blueWarped and RedWarped images
///

int imgHeight = blue.rows;
int imgWidth = blue.cols;

Mat blueWarped;
warpPerspective(blue, blueWarped, hBlueGreen, Size(imgWidth, imgHeight));

Mat redWarped;
warpPerspective(red, redWarped, hRedGreen, Size(imgWidth, imgHeight));

///

plt::figure_size(1200,500);
plt::subplot(1,2,1);
plt::imshow(blueWarped);
plt::title("Blue channel aligned w.r.t green channel");
pltImg = displayImage(blueWarped);
plt::subplot(1,2,2);
plt::imshow(redWarped);
plt::title("Red channel aligned w.r.t green channel");
pltImg = displayImage(redWarped);
pltImg


//Step 6: Merge Channels
Mat colorImage;
vector<Mat> colorImageChannels {blueWarped, green, redWarped};
merge(colorImageChannels,colorImage);
Mat originalImage;
merge(channels,originalImage);
plt::figure_size(1200,500);
plt::subplot(1,2,1);
plt::imshow(originalImage);
plt::title("Original Mis-aligned Image");
pltImg = displayImage(originalImage);
plt::subplot(1,2,2);
plt::imshow(colorImage);
plt::title("Aligned Image");
pltImg = displayImage(colorImage);
pltImg
