// ImageSubtractionCpp.sln
// main.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<conio.h>           // it may be necessary to change or remove this line if not using Windows

#include "Blob.h"

using namespace std;

// global variables ///////////////////////////////////////////////////////////////////////////////
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

// function prototypes ////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex);
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
double distanceBetweenPoints(cv::Point point1, cv::Point point2);
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

	cv::VideoCapture capVideo;

	cv::Mat imgFrame1;
	cv::Mat imgFrame2;

	std::vector<Blob> blobs;

	capVideo.open("SnimakAutoputa.mp4");

	if (!capVideo.isOpened()) {                                                 // if unable to open video file
		std::cout << "\nerror reading video file" << std::endl << std::endl;      // show error message
		_getch();                    // it may be necessary to change or remove this line if not using Windows
		return(0);                                                              // and exit program
	}

	if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
		std::cout << "\nerror: video file must have at least two frames";
		_getch();
		return(0);
	}

	capVideo.read(imgFrame1);
	capVideo.read(imgFrame2);

	char chCheckForEscKey = 0;

	bool blnFirstFrame = true;

	int frameCount = 2;

	while (capVideo.isOpened() && chCheckForEscKey != 27) {

		std::vector<Blob> currentFrameBlobs;

		cv::Mat imgFrame1Copy = imgFrame1.clone();
		cv::Mat imgFrame2Copy = imgFrame2.clone();

		cv::Mat imgDifference;
		cv::Mat imgThresh;

		//Prebacivanje iz RGB u GRAY
		cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
		cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

		//Gausova funkcija zamuti ostrinu slike, treci parametar predstavlja susede na koje se primenje funkcija
		cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
		cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

		//Razlika izmedju dva frejma
		cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

		//cv::imshow("IMG COPY 1",imgFrame1Copy);
		//cv::imshow("IMG COPY 2", imgFrame2Copy);
		//cv::imshow("IMG COPY DIFF", imgDifference);

		//Segmentacija slike, iz grayscale u binarnu (crno - belo)
		cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

		cv::imshow("imgThresh", imgThresh);

		cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
		cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
		cv::Mat structuringElement9x9 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));

		//cv:imshow("structuringElement9x9", structuringElement9x9);

		//Dilate - uklanja noise, vrsi izolaciju individualnih elemenata na slici, "siri" trenutne elemente, upotpunjuje ih
		cv::dilate(imgThresh, imgThresh, structuringElement5x5);
		cv::dilate(imgThresh, imgThresh, structuringElement5x5);
		//Erode - radi u kombinaciji sa dilate, radi kontra efekat uredjivanja "smanjivanja" elemenata na frame-u
		cv::erode(imgThresh, imgThresh, structuringElement5x5);

		//cv::imshow("IMG TRESH AFTER DILATE", imgThresh);

		cv::Mat imgThreshCopy = imgThresh.clone();

		std::vector<std::vector<cv::Point> > contours;

		//Pronalazi spoljasnje konture i veze ih u jedan oblik
		cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		

		//Crta konture na prethodno kreiranoj crnom frame-u, -1 je da popuni sve izmedju kontura
		drawAndShowContours(imgThresh.size(), contours, "imgContours");

		std::vector<std::vector<cv::Point> > convexHulls(contours.size());

		for (unsigned int i = 0; i < contours.size(); i++) {
			cv::convexHull(contours[i], convexHulls[i]);
		}

		drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");

		for (auto &convexHull : convexHulls) {
			Blob possibleBlob(convexHull);

			if (possibleBlob.currentBoundingRect.area() > 400 &&
				possibleBlob.dblCurrentAspectRatio > 0.2 &&
				possibleBlob.dblCurrentAspectRatio < 4.0 &&
				possibleBlob.currentBoundingRect.width > 30 &&
				possibleBlob.currentBoundingRect.height > 30 &&
				possibleBlob.dblCurrentDiagonalSize > 60.0 &&
				(cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
				currentFrameBlobs.push_back(possibleBlob);
			}
		}

		drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

		if (blnFirstFrame == true) {
			for (auto &currentFrameBlob : currentFrameBlobs) {
				blobs.push_back(currentFrameBlob);
			}
		}
		else {
			matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
		}

		drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

		imgFrame2Copy = imgFrame2.clone();         

		drawBlobInfoOnImage(blobs, imgFrame2Copy);

		cv::imshow("imgFrame2Copy", imgFrame2Copy);

		//cv::waitKey(0);                 //frame po frame debugging

		//priprema za sledecu iteraciju

		currentFrameBlobs.clear();

		imgFrame1 = imgFrame2.clone();          

		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
			capVideo.read(imgFrame2);
		}
		else {
			std::cout << "end of video\n";
			break;
		}

		blnFirstFrame = false;
		frameCount++;
		chCheckForEscKey = cv::waitKey(1);
	}

	if (chCheckForEscKey != 27) {           
		cv::waitKey(0);                     
	}

	return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {

	for (auto &existingBlob : existingBlobs) {

		existingBlob.blnCurrentMatchFoundOrNewBlob = false;

		existingBlob.predictNextPosition();
	}

	for (auto &currentFrameBlob : currentFrameBlobs) {

		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;

		for (unsigned int i = 0; i < existingBlobs.size(); i++) {
			if (existingBlobs[i].blnStillBeingTracked == true) {
				double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

				if (dblDistance < dblLeastDistance) {
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}

		if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 1.15) {
			addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
		}
		else {
			addNewBlob(currentFrameBlob, existingBlobs);
		}

	}

	for (auto &existingBlob : existingBlobs) {

		if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
			existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
		}

		if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
			existingBlob.blnStillBeingTracked = false;
		}

	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {

	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

	existingBlobs[intIndex].blnStillBeingTracked = true;
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {

	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;

	existingBlobs.push_back(currentFrameBlob);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double distanceBetweenPoints(cv::Point point1, cv::Point point2) {

	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {
	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

	cv::imshow(strImageName, image);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {

	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	std::vector<std::vector<cv::Point> > contours;

	for (auto &blob : blobs) {
		if (blob.blnStillBeingTracked == true) {
			contours.push_back(blob.currentContour);
		}
	}

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

	cv::imshow(strImageName, image);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy) {

	for (unsigned int i = 0; i < blobs.size(); i++) {

		if (blobs[i].blnStillBeingTracked == true) {
			cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);

			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0;
			int intFontThickness = (int)std::round(dblFontScale * 1.0);

			cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
		}
	}
}

