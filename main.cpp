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

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

	cv::VideoCapture capVideo;

	cv::Mat imgFrame1;
	cv::Mat imgFrame2;

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

	while (capVideo.isOpened() && chCheckForEscKey != 27) {

		std::vector<Blob> blobs;

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

		cv:imshow("structuringElement9x9", structuringElement9x9);

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

		//Napravi novi crni frame iste velicine
		cv::Mat imgContours(imgThresh.size(), CV_8UC3, SCALAR_BLACK);

		//Crta konture na prethodno kreiranoj crnom frame-u, -1 je da popuni sve izmedju kontura
		cv::drawContours(imgContours, contours, -1, SCALAR_WHITE, -1);

		cv::imshow("imgContours", imgContours);

		std::vector<std::vector<cv::Point> > convexHulls(contours.size()); 

		for (unsigned int i = 0; i < contours.size(); i++) {
			cv::convexHull(contours[i], convexHulls[i]);
		}

		for (auto &convexHull : convexHulls) {
			Blob possibleBlob(convexHull);

			if (possibleBlob.boundingRect.area() > 400 &&
				possibleBlob.dblAspectRatio >= 0.2 &&
				possibleBlob.dblAspectRatio <= 4 &&
				possibleBlob.boundingRect.width > 30 &&
				possibleBlob.boundingRect.height > 30 &&
				possibleBlob.dblDiagonalSize > 60.0) {
				blobs.push_back(possibleBlob);
			}
			
		}

		cv::Mat imgConvexHulls(imgThresh.size(), CV_8UC3, SCALAR_BLACK);

		convexHulls.clear();

		for (auto &blob : blobs) {
			convexHulls.push_back(blob.contour);
		}

		cv::drawContours(imgConvexHulls, convexHulls, -1, SCALAR_WHITE, -1);

		cv::imshow("imgConvexHulls", imgConvexHulls);

		imgFrame2Copy = imgFrame2.clone();										//Ponovo pravimo kopiju drugog frame-a, da bismo na nju ucrtali oznake					

		for (auto &blob : blobs) {                                                 
			cv::rectangle(imgFrame2Copy, blob.boundingRect, SCALAR_RED, 2);        //Crtamo crveni bounding rectangle oko svakog bloba
			cv::circle(imgFrame2Copy, blob.centerPosition, 3, SCALAR_GREEN, -1);   //Crtamo zelenu tacku u centar svakog bounding rectangle-a
		}

		cv::imshow("imgFrame2Copy", imgFrame2Copy);

		imgFrame1 = imgFrame2.clone();											//Pripremamo frame-ove za sledecu iteraciju

		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {      
			capVideo.read(imgFrame2);                          
		}
		else {                                                 
			std::cout << "end of video\n";                     
			break;                                             
		}

		chCheckForEscKey = cv::waitKey(1);    

	}

	if (chCheckForEscKey != 27) {             
		cv::waitKey(0);                       
	}

	return(0);
}

