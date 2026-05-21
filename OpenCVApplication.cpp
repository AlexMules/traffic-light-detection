// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>

#include <random>
#include <fstream>

wchar_t* projectPath;

/* Histogram display function - display a histogram using bars (simlilar to L3 / Image Processing)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}

void testOpenImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image",src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName)==0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName,"bmp");
	while(fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(),src);
		if (waitKey()==27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	_wchdir(projectPath);

	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]
		
		Mat src = imread(fname,IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height,width,CV_8UC1);
		// Accessing individual pixels in an 8 bits/pixel image
		// Inefficient way -> slow
		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				uchar val = src.at<uchar>(i,j);
				uchar neg = 255 - val;
				dst.at<uchar>(i,j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image",src);
		imshow("negative image",dst);
		waitKey();
	}
}

// Lab 1
// 3. Implementați o funcție care schimbă nivelele de gri cu un factor aditiv.
// factor aditiv
void testAdditiveFactorImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]

		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height, width, CV_8UC1);
		// Accessing individual pixels in an 8 bits/pixel image
		// Inefficient way -> slow
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				uchar val = src.at<uchar>(i, j);
				uchar new_val = min(val + 60, 255); // pentru a nu depasi valoarea maxima de 255 (evitare overflow)
				dst.at<uchar>(i, j) = new_val;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("additive factor image", dst);
		waitKey();
	}
}

// 4. Implementați o funcție care schimbă nivelele de gri cu un factor multiplicativ. Salvați imaginea rezultat.
// factor multiplicativ
void testMultiplicativeFactorImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]

		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height, width, CV_8UC1);
		// Accessing individual pixels in an 8 bits/pixel image
		// Inefficient way -> slow
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				uchar val = src.at<uchar>(i, j);
				uchar new_val = min(val * 3, 255); // pentru a nu depasi valoarea maxima de 255 (evitare overflow)
				dst.at<uchar>(i, j) = new_val;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("multiplicative image", dst);
		waitKey();
	}
}

// 5. Creați o imagine color de dimensiune 256 x 256. Împărțiți imaginea în 4 cadrane egale și colorați acestea, 
// din stânga-sus până în dreapta-jos, astfel: alb, roșu, verde, galben.
void createNewImage()
{
	Mat img(256, 256, CV_8UC3);
	int width = img.cols;
	int height = img.rows;

	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			Vec3b color;

			if (row < 128 && col < 128) 
			{
				color[0] = 255;
				color[1] = 255;
				color[2] = 255;
			}

			if (row < 128 && col >= 128)
			{
				color[0] = 0;
				color[1] = 0;
				color[2] = 255;
			}

			if (row >= 128 && col < 128)
			{
				color[0] = 0;
				color[1] = 255;
				color[2] = 0;
			}

			if (row >= 128 && col >= 128)
			{
				color[0] = 0;
				color[1] = 255;
				color[2] = 255;
			}

			img.at<Vec3b>(row, col) = color;
		}
	}

	imshow("colourful image", img);
	waitKey();
}

// cerc albastru in matrice
void drawBlueCircle()
{
	Mat img(256, 256, CV_8UC3);
	int width = img.rows;
	int height = img.cols;

	int cx = 127, cy = 127;
	int radius = 70;

	for (int row = 0; row < width; row++)
	{
		for (int col = 0; col < height; col++)
		{
			Vec3b color;

			if ((row - 127) * (row - 127) + (col - 127) * (col - 127) < radius * radius)
			{
				color[0] = 255;
				color[1] = 0;
				color[2] = 0;
				img.at<Vec3b>(row, col) = color;
				continue;
			}

			if (row < 128 && col < 128)
			{
				color[0] = 255;
				color[1] = 255;
				color[2] = 255;
			}

			if (row < 128 && col >= 128)
			{
				color[0] = 0;
				color[1] = 0;
				color[2] = 255;
			}

			if (row >= 128 && col < 128)
			{
				color[0] = 0;
				color[1] = 255;
				color[2] = 0;
			}

			if (row >= 128 && col >= 128)
			{
				color[0] = 0;
				color[1] = 255;
				color[2] = 255;
			}

			img.at<Vec3b>(row, col) = color;
		}
	}

	imshow("blue circle image", img);
	waitKey();
}

//lab 2
// problema 2.1
/* 1. Adăugați la framework o funcție care copiază canalele R, G, B ale unei imagini
RGB24 (tip CV_8UC3) în trei matrice de tip CV_8UC1. Afișați aceste matrice în 3 ferestre diferite. */
void colorChannels()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int width = src.rows;
		int height = src.cols;

		Mat blueChannelMatrix(width, height, CV_8UC1);
		Mat greenChannelMatrix(width, height, CV_8UC1);
		Mat redChannelMatrix(width, height, CV_8UC1);

		for (int row = 0; row < width; row++)
		{
			for (int col = 0; col < height; col++)
			{
				Vec3b color = src.at<Vec3b>(row, col);
				blueChannelMatrix.at<uchar>(row, col) = color[0];
				greenChannelMatrix.at<uchar>(row, col) = color[1];
				redChannelMatrix.at<uchar>(row, col) = color[2];
			}
		}

		imshow("blue channel", blueChannelMatrix);
		imshow("green channel", greenChannelMatrix);
		imshow("red channel", redChannelMatrix);
		waitKey();
	}
}


// problema 2.2
/*2. Adăugați la framework o funcție de conversie de la o imagine color RGB24 (tip CV_8UC3) la 
o imagine grayscale de tip (CV_8UC1) și afișați imaginea rezultat într-o fereastră destinație.*/
void colorToGrayScale()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int width = src.rows;
		int height = src.cols;

		Mat dst(width, height, CV_8UC1);

		for (int row = 0; row < width; row++)
		{
			for (int col = 0; col < height; col++)
			{
				Vec3b color = src.at<Vec3b>(row, col);
				uchar b = color[0];
				uchar g = color[1];
				uchar r = color[2];
				dst.at<uchar>(row, col) = (b + g + r) / 3;
			}
		}

		imshow("color to grayscale", dst);
		waitKey();
	}
}

// problema 2.3
/* 3. Adăugați la framework o funcție de procesare pentru conversia de la grayscale la alb-negru 
pentru imagini grayscale (CV_8UC1), folosind (2.2). Citiți valoarea pragului de la linia de 
comandă. Testați operația de binarizare folosind diverse imagini și diverse praguri. */
void grayscaleToBlackAndWhite()
{
	int threshold;
	std::cout << "Threshold: ";
	std::cin >> threshold;

	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int width = src.rows;
		int height = src.cols;

		Mat dst(width, height, CV_8UC1);
		
		for (int row = 0; row < src.rows; row++)
		{
			for (int col = 0; col < src.cols; col++)
			{
				if (src.at <uchar>(row, col) < threshold)
				{
					dst.at<uchar>(row, col) = 0;
				}
				else 
				{
					dst.at<uchar>(row, col) = 255;
				}
			}
		}

		imshow("black and white", dst);
		waitKey();
	}
}

Mat grayscaleToBlackAndWhite(const Mat& src, int threshold)
{
	int width = src.rows;
	int height = src.cols;

	Mat dst(width, height, CV_8UC1);

	for (int row = 0; row < src.rows; row++)
	{
		for (int col = 0; col < src.cols; col++)
		{
			if (src.at <uchar>(row, col) < threshold)
			{
				dst.at<uchar>(row, col) = 0;
			}
			else
			{
				dst.at<uchar>(row, col) = 255;
			}
		}
	}

	return dst;
}

// problema 2.5
/* 5. Implementați o funcție isInside(img, i, j) care verifică dacă poziția indicată de 
perechea (i,j) (rând, coloană) este înăuntrul imaginii img. */
bool isInside(Mat src, int i, int j)
{
	return (i >= 0 && i < src.rows && j >= 0 && j < src.cols);
}


// problema 2.4
/* 4. Adăugați la framework o funcție care convertește canalele R, G, B ale unei imagini RGB24 
(tip CV_8UC3) în componente H, S, V folosind ecuațiile din 2.6. Memorați componente H, S, V în 
câte o matrice de tip CV_8UC1 corespunzătoare canalelor H, S, V. Afișați aceste matrice în 
3 ferestre diferite. Verificați corectitudinea implementării prin comparație vizuală cu rezultatele 
de mai jos.*/
void RGBtoHSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		Mat Hue(height, width, CV_8UC1);
		Mat Saturation(height, width, CV_8UC1);
		Mat Value(height, width, CV_8UC1);

		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				Vec3b color = src.at<Vec3b>(row, col);
				float r = (float)color[2] / 255.0f;
				float g = (float)color[1] / 255.0f;
				float b = (float)color[0] / 255.0f;

				float M = max(r, max(g, b));
				float m = min(r, min(g, b));
				float C = M - m;

				float V = M;
				float S = 0.0f, H = 0.0f;

				if (V != 0.0f)
				{
					S = (float)C / V;
				}
				else
				{
					S = 0.0f;
				}

				if (C != 0.0f)
				{
					if (M == r) 
					{
						H = 60.0f * (g - b) / C;
					}
					else if (M == g)
					{
						H = 120.0f + 60.0f * (b - r) / C;
					}
					else if (M == b)
					{
						H = 240.0f + 60.0f * (r - g) / C;
					}
				}
				else
				{
					H = 0.0f;
				}

				if (H < 0.0f)
				{
					H += 360.0f;
				}

				Hue.at<uchar>(row, col) = H * 255 / 360;
				Saturation.at<uchar>(row, col) = S * 255;
				Value.at<uchar>(row, col) = V * 255;
				
			}
		}

		imshow("H", Hue);
		imshow("S", Saturation);
		imshow("V", Value);
		waitKey();
	}
}

// lab3
int* compute_histogram_naive(Mat source)
{
	int height = source.rows;
	int width = source.cols;

	int* histogram = (int*)calloc(256, sizeof(int));

	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			histogram[source.at<uchar>(row, col)]++;
		}
	}

	return histogram;
}

float* compute_pdf(int* histogram, Mat source)
{
	int height = source.rows;
	int width = source.cols;
	int M = height * width;

	float* pdf = (float*)malloc(256 * sizeof(float));

	for (int i = 0; i < 256; i++)
	{
		pdf[i] = (float)histogram[i] / M;
	}

	return pdf;
}

int* compute_histogram_custom(Mat source, int histogram_bins)
{
	int height = source.rows;
	int width = source.cols;

	int* histo_bin = (int*)calloc(histogram_bins, sizeof(int));

	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			int grayscale_value = source.at<uchar>(row, col);
			int index = grayscale_value * histogram_bins / 256;

			if (index >= histogram_bins)
			{
				index = histogram_bins - 1;
			}

			histo_bin[index]++;
		}
	}

	return histo_bin;
}

typedef struct {
	uchar* grayscale_values; // vector cu nivelele de gri (maximele + 0 si 255)
	uchar count_grayscale_values; // numarul de nivele
} grayscale_mapping;

grayscale_mapping multi_level_threshold(int wh, float th, float* pdf)
{
	uchar* new_grayscale_values = (uchar*)malloc(256 * sizeof(uchar));
	new_grayscale_values[0] = 0;
	int count_grayscale_values = 1;

	for (int i = wh; i < 256 - wh; i++)
	{
		float mean = 0.0f;
		bool is_bigger = true;

		for (int k = -wh; k <= wh; k++)
		{
			mean += pdf[i + k];
			if (pdf[i] < pdf[i + k])
			{
				is_bigger = false;
			}
		}

		mean /= (2 * wh + 1);

		if (is_bigger && pdf[i] > mean + th)
		{
			new_grayscale_values[count_grayscale_values] = (uchar)i;
			count_grayscale_values++;
		}
	}

	new_grayscale_values[count_grayscale_values] = 255;
	count_grayscale_values++;

	uchar* final_grayscale_values = (uchar*)calloc(count_grayscale_values, sizeof(uchar));
	for (int i = 0; i < count_grayscale_values; i++)
	{
		final_grayscale_values[i] = new_grayscale_values[i];
	}

	free(new_grayscale_values);

	grayscale_mapping map = { final_grayscale_values, (uchar)count_grayscale_values };
	return map;
}

uchar find_closest_histogram_maximum(uchar old_pixel, grayscale_mapping gray_map)
{
	uchar* values = gray_map.grayscale_values;
	int n = gray_map.count_grayscale_values;

	uchar best_value = values[0];
	uchar best_dist = abs((int)old_pixel - (int)values[0]);

	for (int i = 1; i < n; i++)
	{
		int dist = abs((int)old_pixel - (int)values[i]);

		if (dist < best_dist)
		{
			best_dist = dist;
			best_value = values[i];
		}
		else 
		{
			// distanta a inceput sa creasca
			break;
		}
	}

	return best_value;
}

Mat draw_multi_thresholding(Mat source, grayscale_mapping grayscale_map)
{
	int height = source.rows;
	int width = source.cols;

	Mat dst(height, width, CV_8UC1);

	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			uchar old_pixel = source.at<uchar>(row, col);
			dst.at<uchar>(row, col) = find_closest_histogram_maximum(old_pixel, grayscale_map);
		}
	}

	return dst;
}

Mat floyd_steinberg_dithering(Mat source, grayscale_mapping gray_map)
{
	int rows = source.rows;
	int cols = source.cols;

	Mat work_img;
	source.convertTo(work_img, CV_32F);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			float old_pixel = work_img.at<float>(i, j);

			uchar clamped_val = saturate_cast<uchar>(old_pixel);
			uchar new_pixel = find_closest_histogram_maximum(clamped_val, gray_map);

			work_img.at<float>(i, j) = (float)new_pixel;
			float error = old_pixel - (float)new_pixel;

			if (j + 1 < cols)
				work_img.at<float>(i, j + 1) += error * 7 / 16.0f;

			if (i + 1 < rows && j - 1 >= 0)
				work_img.at<float>(i + 1, j - 1) += error * 3 / 16.0f;

			if (i + 1 < rows)
				work_img.at<float>(i + 1, j) += error * 5 / 16.0f;

			if (i + 1 < rows && j + 1 < cols)
				work_img.at<float>(i + 1, j + 1) += error * 1 / 16.0f;
		}
	}

	Mat dst;
	work_img.convertTo(dst, CV_8UC1);

	return dst;
}

void seeHistogram()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		if (src.empty()) 
			continue;

		int* histo = compute_histogram_naive(src);
		int* histo_bins = compute_histogram_custom(src, 32);
		float* pdf = compute_pdf(histo, src);
		grayscale_mapping map = multi_level_threshold(5, 0.0003f, pdf);
		Mat dst = draw_multi_thresholding(src, map);
		Mat fs_dst = floyd_steinberg_dithering(src, map);

		imshow("Original Image", src);
		showHistogram("Histogram", histo, 256, 256);
		showHistogram("Histogram_bins", histo_bins, 32, 256);
		imshow("Multi-threshold Image (Simple)", dst);
		imshow("Floyd-Steinberg Dithering", fs_dst);

		free(histo);
		free(histo_bins);
		free(pdf);
		free(map.grayscale_values);

		waitKey();
	}
}


// lab 4
Mat get_object_instance(const Mat& source, Vec3b color)
{
	Mat result(source.rows, source.cols, CV_8UC1, Scalar(0));

	for(int i = 0; i < source.rows; i++) {
		for (int j = 0; j < source.cols; j++) {
			if (source.at<Vec3b>(i, j) == color) {
				result.at<uchar>(i, j) = 255;
			}
		}
	}

	return result;
}

Point2d compute_center_of_mass(const Mat& src)
{
	int cx = 0, cy = 0;
	int area = 0;

	int rows = src.rows;
	int cols = src.cols;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (src.at<uchar>(i, j) == 255) {
				cx += j;
				cy += i;
				area++;
			}
		}
	}

	return Point2d((double)cx / area, (double)cy / area);
}

double compute_axis_of_elongation_angle(Point2d center, Mat& obj)
{
	double num = 0, dr2 = 0, dc2 = 0;

	for (int i = 0; i < obj.rows; i++) {
		for (int j = 0; j < obj.cols; j++) {
			if (obj.at<uchar>(i, j) == 255) {
				double dr = i - center.y;
				double dc = j - center.x;

				num += dr * dc;
				dr2 += dr * dr;
				dc2 += dc * dc;
			}
		}
	}

	return atan2(2 * num, (dc2 - dr2)) / 2;
}

void draw_elongation_axis(Mat& img, Point2d center, double phi)
{
	Mat display;
	if (img.channels() == 1) {
		cvtColor(img, display, COLOR_GRAY2BGR);
	}
	else {
		display = img.clone();
	}

	double length = 100.0;

	Point p1, p2;
	p1.x = cvRound(center.x + length * cos(phi));
	p1.y = cvRound(center.y + length * sin(phi));

	p2.x = cvRound(center.x - length * cos(phi));
	p2.y = cvRound(center.y - length * sin(phi));

	line(display, p1, p2, Scalar(0, 0, 255), 2);

	circle(display, center, 4, Scalar(0, 255, 0), -1);

	imshow("Elongation axis", display);
}

int compute_area(const Mat& src)
{
	int area = 0;

	int rows = src.rows;
	int cols = src.cols;
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			if (src.at<uchar>(row, col) == 255) {
				area++;
			}
		}
	}

	return area;
}

typedef struct {
	Mat contour;
	int length;
} contour_info;

contour_info compute_perimeter(const Mat& obj)
{
	contour_info res;

	res.length = 0;
	res.contour = Mat::zeros(obj.size(), CV_8UC1);

	int rows = obj.rows;
	int cols = obj.cols;

	int di[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
	int dj[] = { 0, 0, -1, 1, -1, 1, -1, 1 };

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (obj.at<uchar>(i, j) == 255) {
				bool isBoundary = false;

				for (int k = 0; k < 8; k++) {
					int ni = i + di[k];
					int nj = j + dj[k];

					if (!isInside(obj, ni, nj) || obj.at<uchar>(ni, nj) == 0) {
						isBoundary = true;
						break;
					}
				}

				if (isBoundary) {
					res.length++;
					res.contour.at<uchar>(i, j) = 255;
				}
			}
		}
	}

	return res;
}

double compute_thinness_ratio(int area, int perimeter)
{
	if (perimeter == 0) {
		return 0;
	}

	double ratio = (4.0 * CV_PI * area) / (double(perimeter) * perimeter);

	return ratio;
}

typedef struct {
	int c_min, c_max, r_min, r_max;
} rectangle_coord;

rectangle_coord compute_bounding_box(const Mat& src)
{
	rectangle_coord res;
	res.c_min = src.cols;
	res.c_max = 0;
	res.r_min = src.rows;
	res.r_max = 0;

	bool found = false;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<uchar>(i, j) == 255) {
				if (j < res.c_min) res.c_min = j;
				if (j > res.c_max) res.c_max = j;
				if (i < res.r_min) res.r_min = i;
				if (i > res.r_max) res.r_max = i;
				found = true;
			}
		}
	}

	if (!found) {
		res.c_min = res.c_max = res.r_min = res.r_max = 0;
	}

	return res;
}

double compute_aspect_ratio(rectangle_coord boundingBox)
{
	double width = (double)boundingBox.c_max - boundingBox.c_min + 1;
	double height = (double)boundingBox.r_max - boundingBox.r_min + 1;

	if (height == 0) {
		return 0.0;
	}

	return width / height;
}

Mat horizontal_projection(const Mat& src) {
	rectangle_coord boundingBox = compute_bounding_box(src);

	Mat proj = Mat::zeros(src.rows, src.cols, CV_8UC1);

	for (int i = boundingBox.r_min; i <= boundingBox.r_max; i++) {
		int count = 0;
		for (int j = boundingBox.c_min; j <= boundingBox.c_max; j++) {
			if (src.at<uchar>(i, j) == 255) {
				count++;
			}
		}
		line(proj, Point(0, i - boundingBox.r_min), Point(count, i - boundingBox.r_min), Scalar(255), 1);
	}
	return proj;
}

Mat vertical_projection(const Mat& src) {
	rectangle_coord boundingBox = compute_bounding_box(src);

	Mat proj = Mat::zeros(src.rows, src.cols, CV_8UC1);

	for (int j = boundingBox.c_min; j <= boundingBox.c_max; j++) {
		int count = 0;
		for (int i = boundingBox.r_min; i <= boundingBox.r_max; i++) {
			if (src.at<uchar>(i, j) == 255) {
				count++;
			}
		}
		line(proj, Point(j - boundingBox.c_min, 0), Point(j - boundingBox.c_min, count), Scalar(255), 1);
	}
	return proj;
}

void geom_features(int event, int x, int y, int flags, void* param)
{
	Mat source = *(Mat*)param;

	if (event != EVENT_LBUTTONDOWN) {
		return;
	}

	Vec3b color = source.at<Vec3b>(y, x);
	Mat obj = get_object_instance(source, color);

	int area = compute_area(obj);
	contour_info p_info = compute_perimeter(obj);
	double thinness = compute_thinness_ratio(area, p_info.length);

	rectangle_coord bb = compute_bounding_box(obj);
	double aspect_ratio = compute_aspect_ratio(bb);

	printf("Area: %d\n", area);
	printf("Perimeter: %d\n", p_info.length);
	printf("Thinness Ratio: %.4f\n", thinness);
	printf("Aspect Ratio: %.4f\n", aspect_ratio);

	Point2d center = compute_center_of_mass(obj);
	draw_elongation_axis(obj, center, compute_axis_of_elongation_angle(center, obj));

	imshow("obj", obj);
	imshow("Contour", p_info.contour);
	imshow("Horizontal projection", horizontal_projection(obj));
	imshow("Vertical projection", vertical_projection(obj));
}

void geometric_features_ex()
{
	Mat src;
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		namedWindow("My Window", 1);

		setMouseCallback("My Window", geom_features, &src);

		//show the image
		imshow("My Window", src);

		// Wait until press some key
		waitKey(0);
	}
}

// lab 5
typedef struct {
	Mat labels;
	int no_labels;
} labels;


const Point previous_neighbors[4] = {
	Point(-1, 0), //stanga
	Point(-1, -1), // sus-stanga
	Point(0, -1), // sus
	Point(1, -1) // sus-dreapta
};

const Point neighborhood4[4] = {
	Point(0, -1), // Sus
	Point(0, 1),  // Jos
	Point(-1, 0), // Stanga
	Point(1, 0)   // Dreapta
};

const Point neighborhood8[8] = {
	Point(-1, -1), Point(0, -1), Point(1, -1), // Randul de sus
	Point(-1,  0),               Point(1,  0), // Stanga si Dreapta
	Point(-1,  1), Point(0,  1), Point(1,  1)  // Randul de jos
};

Mat color_labels(labels labels_str) {
	int height = labels_str.labels.rows;
	int width = labels_str.labels.cols;
	Mat color_img = Mat::zeros(height, width, CV_8UC3);

	std::vector<Vec3b> colors(labels_str.no_labels + 1, Vec3b(255, 255, 255));
	
	std::default_random_engine gen;
	std::uniform_int_distribution<int> d(0, 255);

	for (int i = 1; i <= labels_str.no_labels; i++) {
		colors[i] = Vec3b(d(gen), d(gen), d(gen));
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int label_val = labels_str.labels.at<int>(i, j);
			if (label_val != 0) {
				color_img.at<Vec3b>(i, j) = colors[label_val];
			}
		}
	}

	return color_img;
}

labels BFS_labeling(Mat source, const Point* neighborhood) {
	int height = source.rows;
	int width = source.cols;
	Mat labels_mat = Mat::zeros(height, width, CV_32SC1);
	int label = 0;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (source.at<uchar>(i, j) == 0 && labels_mat.at<int>(i, j) == 0) {
				label++;

				std::queue<Point> Q;
				labels_mat.at<int>(i, j) = label;
				Q.push(Point(j, i));

				while (!Q.empty()) {
					Point p = Q.front();
					Q.pop();

					for (int k = 0; k < 8; k++) {
						Point neighbor = p + neighborhood[k];

						if (isInside(source, neighbor.y, neighbor.x)) {

							if (source.at<uchar>(neighbor.y, neighbor.x) == 0 &&
								labels_mat.at<int>(neighbor.y, neighbor.x) == 0) {

								labels_mat.at<int>(neighbor.y, neighbor.x) = label;
								Q.push(neighbor);
							}
						}
					}
				}
			}
		}
	}

	labels result = { labels_mat, label };
	return result;
}

labels Two_pass_labelling(Mat source) {
	int height = source.rows;
	int width = source.cols;

	Mat labels_mat = Mat::zeros(height, width, CV_32SC1);
	int label = 0;

	std::vector<std::vector<int>> edges(1);

	// prima parcurgere
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (source.at<uchar>(i, j) == 0 && labels_mat.at<int>(i, j) == 0) {
				std::vector<int> L;

				for (int k = 0; k < 4; k++) {
					Point prevN = Point(j, i) + previous_neighbors[k];

					if (isInside(source, prevN.y, prevN.x)) {
						int val = labels_mat.at<int>(prevN.y, prevN.x);
						if (val > 0) {
							L.push_back(val);
						}
					}
				}

				if (L.empty()) {
					label++;
					labels_mat.at<int>(i, j) = label;
					edges.resize(label + 1);
				}
				else {
					int x = *std::min_element(L.begin(), L.end());
					labels_mat.at<int>(i, j) = x;

					for (int y : L) {
						if (y != x) {
							edges[x].push_back(y);
							edges[y].push_back(x);
						}
					}
				}
			}
		}
	}

	int newlabel = 0;
	std::vector<int> newlabels_map(label + 1, 0);

	// determinarea claselor de echivalenta folosind parcurgerea in latime pe graful etichetelor
	for (int i = 1; i <= label; i++) {
		if (newlabels_map[i] == 0) {
			newlabel++;
			std::queue<int> Q;
			newlabels_map[i] = newlabel;
			Q.push(i);

			while (!Q.empty()) {
				int u = Q.front();
				Q.pop();

				for (int v : edges[u]) {
					if (newlabels_map[v] == 0) {
						newlabels_map[v] = newlabel;
						Q.push(v);
					}
				}
			}
		}
	}

	// a doua parcurgere (inlocuirea etichetelor cu cele noi)
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int old_label = labels_mat.at<int>(i, j);
			if (old_label > 0) {
				labels_mat.at<int>(i, j) = newlabels_map[old_label];
			}
		}
	}

	labels result = { labels_mat, newlabel };
	return result;
}

void show_labels() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		labels labels_str = BFS_labeling(src, neighborhood8);
		Mat labeled_image = color_labels(labels_str);
		Mat two_pass_labeled_image = color_labels(Two_pass_labelling(src));
		imshow("Original Image", src);
		imshow("Labeled Image", labeled_image);
		imshow("Two-pass Labeled Image", two_pass_labeled_image);
		waitKey();
	}
}

// lab 6
typedef struct {
	std::vector<Point> border;
	std::vector<int> dir_vec;
} contour;

Point findP0(const Mat& src) 
{
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<uchar>(i, j) == 0) {
				return Point(j, i);
			}
		}
	}
}

int di[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
int dj[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };

contour extractC(const Mat& src) {
	Point p0 = findP0(src);

	std::vector<Point> border;
	std::vector<int> dir_vector;

	Point Pc = p0;
	int dir = 7;

	border.push_back(Pc);

	do {
		if(dir % 2 == 0) {
			dir = (dir + 7) % 8; // dir par
		}
		else {
			dir = (dir + 6) % 8; // dir impar
		}

		int foundDir = -1;

		for (int k = 0; k < 8; k++) {
			int currentDir = (dir + k) % 8;
			Point neighbor(Pc.x + dj[currentDir], Pc.y + di[currentDir]);

			if (isInside(src, neighbor.y, neighbor.x) && src.at<uchar>(neighbor.y, neighbor.x) == 0) {
				Pc = neighbor;        
				foundDir = currentDir;
				break;                
			}
		}

		if (foundDir == -1) {
			break;
		}

		dir = foundDir;
		border.push_back(Pc);
		dir_vector.push_back(dir);

	} while (!(border.size() > 2 && border[0] == border[border.size() - 2] &&
		border[1] == border[border.size() - 1]));

	contour result;
	result.border = border;
	result.dir_vec = dir_vector;

	return result;
}

void show_contour() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		contour c = extractC(src);
		Mat contour_img = Mat::zeros(src.size(), CV_8UC1);
		for (const Point& p : c.border) {
			contour_img.at<uchar>(p) = 255;
		}
		imshow("Original Image", src);
		imshow("Contour", contour_img);
		waitKey();
	}
}

void reconstructEx3() {
	char imgPath[MAX_PATH];

	if (!openFileDlg(imgPath)) {
		return;
	}

	Mat src = imread(imgPath, IMREAD_GRAYSCALE);
	std::ifstream fin("D:\\Alex\\UTCN\\An 3\\Semestrul 2\\Procesarea_imaginilor\\Laborator\\Imagini_lab6\\reconstruct.txt");

	int y0, x0, n;
	fin >> y0 >> x0 >> n;
	Point Pc(x0, y0);

	if (isInside(src, Pc.y, Pc.x)) {
		src.at<uchar>(Pc.y, Pc.x) = 0;
	}

	for (int i = 0; i < n; i++) {
		int code;
		fin >> code;

		Pc.y += di[code];
		Pc.x += dj[code];

		if (isInside(src, Pc.y, Pc.x)) {
			src.at<uchar>(Pc.y, Pc.x) = 0;
		}
	}

	fin.close();

	imshow("Reconstructed - EXCELLENT", src);
	waitKey();
}


// lab 7
// vecinatati
int di4[] = { -1, 1, 0, 0 };
int dj4[] = { 0, 0, -1, 1 };

int di8[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
int dj8[] = { 0, 0, -1, 1, -1, 1, -1, 1 };

Mat dilate(Mat src, int n_iterations, int connectivity) {
	Mat currentImage = src.clone();

	int n_neighbors = (connectivity == 4) ? 4 : 8;
	int* di = (connectivity == 4) ? di4 : di8;
	int* dj = (connectivity == 4) ? dj4 : dj8;

	for (int iter = 0; iter < n_iterations; iter++) {
		Mat resultImage(currentImage.size(), CV_8UC1, Scalar(255));

		for (int i = 0; i < currentImage.rows; i++) {
			for (int j = 0; j < currentImage.cols; j++) {
				if (currentImage.at<uchar>(i, j) == 0) {

					resultImage.at<uchar>(i, j) = 0; 

					for (int k = 0; k < n_neighbors; k++) {
						int ni = i + di[k];
						int nj = j + dj[k];

						if (isInside(currentImage, ni, nj)) {
							resultImage.at<uchar>(ni, nj) = 0;
						}
					}
				}
			}
		}
		currentImage = resultImage.clone();
	}
	return currentImage;
}

Mat erode(Mat src, int n_iterations, int connectivity) {
	Mat currentImage = src.clone();

	int n_neighbors = (connectivity == 4) ? 4 : 8;
	int* di = (connectivity == 4) ? di4 : di8;
	int* dj = (connectivity == 4) ? dj4 : dj8;

	for (int iter = 0; iter < n_iterations; iter++) {
		Mat resultImage(currentImage.size(), CV_8UC1, Scalar(255));

		for (int i = 0; i < currentImage.rows; i++) {
			for (int j = 0; j < currentImage.cols; j++) {

				if (currentImage.at<uchar>(i, j) == 0) {
					bool coversOnlyObject = true;
					for (int k = 0; k < n_neighbors; k++) {
						int ni = i + di[k];
						int nj = j + dj[k];

						if (isInside(currentImage, ni, nj) && currentImage.at<uchar>(ni, nj) == 255) {
							coversOnlyObject = false;
							break;
						}
					}

					if (coversOnlyObject) {
						resultImage.at<uchar>(i, j) = 0;
					}
				}
			}
		}
		currentImage = resultImage.clone();
	}
	return currentImage;
}

void testDilate() {
	char fname[MAX_PATH];

	while (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		if (src.empty()) {
			continue;
		}

		int n, connectivity;
		printf("Number of iterations (n): "); 
		std::cin >> n;

		printf("Connectivity (4 or 8): "); 
		std::cin >> connectivity;

		Mat dst = dilate(src, n, connectivity);

		imshow("original image", src);
		imshow("dilated image", dst);
		waitKey();
	}
}

void testErode() {
	char fname[MAX_PATH];

	while (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		if (src.empty()) {
			continue;
		}

		int n, connectivity;
		printf("Number of iterations (n): ");
		std::cin >> n;

		printf("Connectivity (4 or 8): ");
		std::cin >> connectivity;

		Mat dst = erode(src, n, connectivity);

		imshow("original image", src);
		imshow("eroded image", dst);
		waitKey();
	}
}

Mat opening(Mat src, int n_iterations, int connectivity) {
	Mat eroded = erode(src, n_iterations, connectivity);
	Mat opened = dilate(eroded, n_iterations, connectivity);

	return opened;
}

void testOpening() {
	char fname[MAX_PATH];

	while (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		if (src.empty()) {
			continue;
		}

		int n, connectivity;
		printf("Number of iterations (n): "); 
		scanf("%d", &n);
		printf("Connectivity (4 or 8): "); 
		scanf("%d", &connectivity);

		Mat dst = opening(src, n, connectivity);

		imshow("original image", src);
		imshow("opened image", dst);
		waitKey();
	}
}

Mat closing(Mat src, int n_iterations, int connectivity) {
	Mat dilated = dilate(src, n_iterations, connectivity);
	Mat closed = erode(dilated, n_iterations, connectivity);

	return closed;
}

void testClosing() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		if (src.empty()) {
			continue;
		}

		int n, connectivity;
		printf("Number of iterations (n): "); 
		scanf("%d", &n);
		printf("Connectivity (4 or 8): "); 
		scanf("%d", &connectivity);

		Mat dst = closing(src, n, connectivity);

		imshow("original image", src);
		imshow("closed image", dst);
		waitKey();
	}
}

Mat boundaryExtraction(Mat src, int connectivity) {
	Mat eroded = erode(src, 1, connectivity);

	Mat boundary(src.size(), CV_8UC1, Scalar(255)); 

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {

			uchar originalPixel = src.at<uchar>(i, j);
			uchar erodedPixel = eroded.at<uchar>(i, j);

			if (originalPixel == 0 && erodedPixel == 255) {
				boundary.at<uchar>(i, j) = 0; 
			}
		}
	}

	return boundary;
}

void testBoundaryExtraction() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		if (src.empty()) {
			continue;
		}

		int connectivity;
		printf("Connectivity (4 or 8): "); 
		scanf("%d", &connectivity);

		Mat dst = boundaryExtraction(src, connectivity);

		imshow("original image", src);
		imshow("boundary", dst);
		waitKey();
	}
}

Mat regionFill(Mat boundaryImg, int connectivity) {
	Mat A_comp(boundaryImg.size(), CV_8UC1);
	for (int i = 0; i < boundaryImg.rows; i++) {
		for (int j = 0; j < boundaryImg.cols; j++) {
			A_comp.at<uchar>(i, j) = (boundaryImg.at<uchar>(i, j) == 0) ? 255 : 0;
		}
	}

	Mat X_prev(boundaryImg.size(), CV_8UC1, Scalar(255));
	int centerX = boundaryImg.cols / 2;
	int centerY = boundaryImg.rows / 2;

	if (isInside(X_prev, centerY, centerX)) {
		X_prev.at<uchar>(centerY, centerX) = 0;
	}

	Mat X_curr;
	bool changed = true;

	while (changed) {
		Mat dilated = dilate(X_prev, 1, connectivity);
		X_curr = Mat(boundaryImg.size(), CV_8UC1, Scalar(255));

		changed = false;
		for (int i = 0; i < dilated.rows; i++) {
			for (int j = 0; j < dilated.cols; j++) {
				if (dilated.at<uchar>(i, j) == 0 && A_comp.at<uchar>(i, j) == 0) {
					X_curr.at<uchar>(i, j) = 0;
				}

				if (X_curr.at<uchar>(i, j) != X_prev.at<uchar>(i, j)) {
					changed = true;
				}
			}
		}
		X_prev = X_curr.clone();
	}


	Mat result = boundaryImg.clone();
	for (int i = 0; i < boundaryImg.rows; i++) {
		for (int j = 0; j < boundaryImg.cols; j++) {
			if (X_prev.at<uchar>(i, j) == 0) {
				result.at<uchar>(i, j) = 0;
			}
		}
	}

	return result;
}

void testRegionFill() {
	char fname[MAX_PATH];

	while (openFileDlg(fname)) {
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		if (src.empty()) {
			return;
		}

		int connectivity;
		printf("Connectivity (4 or 8): ");
		scanf("%d", &connectivity);

		Mat filled = regionFill(src, connectivity);

		imshow("original image", src);
		imshow("filled region image", filled);
		waitKey();
	}
}

// lab 8
float compute_mean(const Mat& src) 
{
	int* histogram = compute_histogram_naive(src);

	int sum = 0;

	for (int i = 0; i < 256; i++) {
		sum += histogram[i] * i;
	}

	return (float)sum / (src.rows * src.cols);
}

float compute_standard_deviation(float mean, int* histogram, float* pdf)
{
	float sum = 0;
	for (int i = 0; i < 256; i++) {
		float term = (float)(i - mean) * (i - mean) * pdf[i];
		sum += term;
	}

	return sqrt(sum);
}

float* compute_cumulative_histogram(int* histogram, float* pdf)
{
	float* cpdf = (float*)malloc(256 * sizeof(float));
	cpdf[0] = pdf[0];
	for (int i = 1; i < 256; i++) {
		cpdf[i] = cpdf[i - 1] + pdf[i];
	}

	return cpdf;
}

void test_ex1_lab8()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int* histogram = compute_histogram_naive(src);
		float* pdf = compute_pdf(histogram, src);
		float mean = compute_mean(src);
		float stddev = compute_standard_deviation(mean, histogram, pdf);
		float* cpdf = compute_cumulative_histogram(histogram, pdf);

		imshow("Original Image", src);
		printf("Mean: %.2f\n", mean);
		printf("Standard Deviation: %.2f\n", stddev);
		showHistogram("Histogram", histogram, 256, 256);
		
		free(histogram);
		free(pdf);
		free(cpdf);
		waitKey();
	}
}

typedef struct {
	int threshold;
	Mat dst;
} global_binarized_image;

global_binarized_image global_automated_binarization(const Mat& src)
{
	int* histogram = compute_histogram_naive(src);
	int imin = 0, imax = 255;

	for (int i = 0; i < 256; i++) {
		if (histogram[i] > 0) {
			imin = i;
			break;
		}
	}

	for (int i = 255; i >= 0; i--) {
		if (histogram[i] > 0) {
			imax = i;
			break;
		}
	}

	float T = (float)(imin + imax) / 2.0f;
	float old_T;
	float error = 0.1f;

	do {
		old_T = T;

		double sum1 = 0, count1 = 0;
		double sum2 = 0, count2 = 0;

		for (int i = imin; i <= imax; i++) {
			if (i <= T) {
				sum1 += (double)i * histogram[i];
				count1 += histogram[i];
			}
			else {
				sum2 += (double)i * histogram[i];
				count2 += histogram[i];
			}
		}

		float ug1 = (count1 > 0) ? (float)(sum1 / count1) : 0;
		float ug2 = (count2 > 0) ? (float)(sum2 / count2) : 0;

		T = (ug1 + ug2) / 2.0f;

	} while (fabs(T - old_T) > error);

	Mat dst = src.clone();
	int final_threshold = (int)T;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<uchar>(i, j) <= final_threshold)
				dst.at<uchar>(i, j) = 0;
			else
				dst.at<uchar>(i, j) = 255;
		}
	}

	free(histogram);

	return { (int)T, dst };
}

void test_global_automated_binarization()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		global_binarized_image result = global_automated_binarization(src);
		imshow("Original Image", src);
		imshow("Binarized Image", result.dst);
		printf("Computed Threshold: %d\n", result.threshold);
		waitKey();
	}
}

Mat change_contrast(const Mat& src, int goutMin, int goutMax)
{
	int rows = src.rows;
	int cols = src.cols;
	Mat dst = Mat(rows, cols, CV_8UC1);

	uchar ginMin = 255;
	uchar ginMax = 0;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			uchar val = src.at<uchar>(i, j);
			if (val < ginMin) ginMin = val;
			if (val > ginMax) ginMax = val;
		}
	}

	if (ginMax == ginMin) {
		return src.clone();
	}

	uchar lut[256];
	float ratio = (float)(goutMax - goutMin) / (ginMax - ginMin);

	for (int gin = 0; gin < 256; gin++) {
		float val = goutMin + (gin - ginMin) * ratio;

		if (val < 0) {
			val = 0;
		}
		if (val > 255) {
			val = 255;
		}

		lut[gin] = (uchar)val;
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			dst.at<uchar>(i, j) = lut[src.at<uchar>(i, j)];
		}
	}

	return dst;
}

void test_change_contrast()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int goutMin, goutMax;
		printf("Enter desired output range (goutMin goutMax): ");
		scanf("%d %d", &goutMin, &goutMax);
		Mat dst = change_contrast(src, goutMin, goutMax);
		imshow("Original Image", src);
		imshow("Contrast Changed Image", dst);
		showHistogram("Original Histogram", compute_histogram_naive(src), 256, 256);
		showHistogram("Contrast Changed Histogram", compute_histogram_naive(dst), 256, 256);
		waitKey();
	}
}

Mat gamma_correction(const Mat& src, float gamma)
{
	int rows = src.rows;
	int cols = src.cols;
	Mat dst = Mat(rows, cols, CV_8UC1);

	uchar lut[256];

	for (int i = 0; i < 256; i++) {
		float gin_normalized = (float)i / 255.0f;
		float gout = 255.0f * pow(gin_normalized, gamma);

		if (gout > 255.0f) {
			lut[i] = 255;
		}
		else if (gout < 0.0f) {
			lut[i] = 0;
		}
		else {
			lut[i] = (uchar)gout;
		}
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			dst.at<uchar>(i, j) = lut[src.at<uchar>(i, j)];
		}
	}

	return dst;
}

void test_gamma_correction()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		float gamma;
		printf("Enter gamma value: ");
		scanf("%f", &gamma);
		Mat dst = gamma_correction(src, gamma);
		imshow("Original Image", src);
		imshow("Gamma Corrected Image", dst);
		showHistogram("Original Histogram", compute_histogram_naive(src), 256, 256);
		showHistogram("Gamma Corrected Histogram", compute_histogram_naive(dst), 256, 256);
		waitKey();
	}
}

Mat equalize_histogram(const Mat& src)
{
	int rows = src.rows;
	int cols = src.cols;
	int M = rows * cols;
	Mat dst = Mat(rows, cols, CV_8UC1);

	int* h = compute_histogram_naive(src);

	uchar tab[256];
	float sum = 0;

	for (int i = 0; i < 256; i++) {
		sum += (float)h[i] / M;

		float gout = 255.0f * sum;

		if (gout > 255) 
		{
			gout = 255;
		}
		tab[i] = (uchar)(gout + 0.5f);
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			dst.at<uchar>(i, j) = tab[src.at<uchar>(i, j)];
		}
	}

	free(h);

	return dst;
}

void test_equalize_histogram()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		Mat dst = equalize_histogram(src);
		imshow("Original Image", src);
		imshow("Equalized Image", dst);
		showHistogram("Original Histogram", compute_histogram_naive(src), 256, 256);
		showHistogram("Equalized Histogram", compute_histogram_naive(dst), 256, 256);
		waitKey();
	}
}


// lab 9
Mat apply_convolution(const Mat& src, const int* conv_weights, int w)
{
	int k = w / 2;
	int L = 255;

	// Calculam informatii despre nucleu
	int sum_kernel = 0;
	int S_pos = 0;
	int S_neg = 0;
	bool has_negative = false;

	for (int u = 0; u < w; u++) {
		for (int v = 0; v < w; v++) {
			int weight = conv_weights[u * w + v];
			sum_kernel += weight;

			if (weight < 0) {
				has_negative = true;
				S_neg += -weight;
			}
			else {
				S_pos += weight;
			}
		}
	}

	float scale = 1.0f;
	float offset = 0.0f;

	bool is_low_pass = !has_negative;

	if (is_low_pass) {
		if (sum_kernel != 0) {
			scale = 1.0f / sum_kernel;
		}
		offset = 0.0f;
	} 
	else {
		// Filtre trece-sus: scalare conform ideii din ec 9.20
		int S = max(S_pos, S_neg);

		if (S != 0) {
			scale = 1.0f / (2.0f * S);
		}
		else {
			scale = 1.0f;
		}

		offset = L / 2.0f;
	}

	Mat dst = Mat::zeros(src.size(), CV_8UC1);

	for(int i = k; i < src.rows - k; i++) {
		for(int j = k; j < src.cols - k; j++) {
			int conv_sum = 0;

			for(int u = -k; u <= k; u++) {
				for(int v = -k; v <= k; v++) {
					int x = i + u;
					int y = j + v;

					uchar pixel_value = src.at<uchar>(x, y);

					int kernel_row = u + k;
					int kernel_col = v + k;

					int weight = conv_weights[kernel_row * w + kernel_col];
					conv_sum += pixel_value * weight;
				}
			}

			float g_out;
			if (sum_kernel > 0) {
				g_out = (conv_sum * scale) + offset;
			}
			else {
				float manual_scale = 1.0f;
				g_out = abs(conv_sum) * manual_scale;
			}

			dst.at<uchar>(i, j) = saturate_cast<uchar>(g_out);
		}
	}

	return dst;
}

void test_convolution()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);

		int arithmetic_mean_filter[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 }; // arithmetic mean filter 3x3
		Mat dst_arithmetic_mean = apply_convolution(src, arithmetic_mean_filter, 3);

		int gaussian_filter[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 }; // gaussian filter 3x3
		Mat dst_gaussian = apply_convolution(src, gaussian_filter, 3);

		int laplace_filter[9] = { -1, -1, -1, -1, 8, -1, -1, -1, -1 }; // laplace filter 3x3
		Mat dst_laplace = apply_convolution(src, laplace_filter, 3);

		imshow("Original Image", src);
		imshow("Arithmetic Mean Filter Image", dst_arithmetic_mean);
		imshow("Gaussian Filter Image", dst_gaussian);
		imshow("Laplace Filter Image", dst_laplace);
		waitKey();
	}
}

// lab 10
Mat ordered_filtering(Mat source, int kernel_size, std::string filter_type)
{
	Mat dst = source.clone(); 
	int k = kernel_size / 2;

	for (int i = k; i < source.rows - k; i++) {
		for (int j = k; j < source.cols - k; j++) {

			std::vector<uchar> window;
			window.reserve(kernel_size * kernel_size);

			for (int u = -k; u <= k; u++) {
				for (int v = -k; v <= k; v++) {
					window.push_back(source.at<uchar>(i + u, j + v));
				}
			}

			std::sort(window.begin(), window.end());

			if (filter_type == "median") {
				dst.at<uchar>(i, j) = window[window.size() / 2];
			}
			else if (filter_type == "min") {
				dst.at<uchar>(i, j) = window.front(); // firt value (the smallest)
			}
			else if (filter_type == "max") {
				dst.at<uchar>(i, j) = window.back(); // last value (the largest)
			}
		}
	}

	return dst;
}

void test_ordered_filtering()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int kernel_size;
		printf("Enter kernel size (odd integer): ");
		scanf("%d", &kernel_size);
		Mat dst_median = ordered_filtering(src, kernel_size, "median");
		Mat dst_min = ordered_filtering(src, kernel_size, "min");
		Mat dst_max = ordered_filtering(src, kernel_size, "max");
		imshow("Original Image", src);
		imshow("Median Filtered Image", dst_median);
		imshow("Min Filtered Image", dst_min);
		imshow("Max Filtered Image", dst_max);
		waitKey();
	}
}

Mat compute_kernel_2D(int kernel_size)
{
	Mat kernel = Mat::zeros(kernel_size, kernel_size, CV_32FC1);
    int half_size = kernel_size / 2;
	float sigma = kernel_size / 6.0f;

    float constant = 1.0f / (2.0f * (float)CV_PI * sigma * sigma);
    float two_sigma_sq = 2.0f * sigma * sigma;

	for (int i = 0; i < kernel_size; i++) {
		for (int j = 0; j < kernel_size; j++) {
			float x = i - half_size;
			float y = j - half_size;
			float value = exp(-(x * x + y * y) / two_sigma_sq) * constant;
			kernel.at<float>(i, j) = value;
		}
	}

    return kernel;
}



Mat apply_gaussian_blur_color(const Mat& src)
{
	Mat dst = src.clone();

	int kernel[3][3] = {
		{1, 2, 1},
		{2, 4, 2},
		{1, 2, 1}
	};
	int kernel_sum = 16;

	for (int i = 1; i < src.rows - 1; i++) {
		for (int j = 1; j < src.cols - 1; j++) {

			int sum_b = 0, sum_g = 0, sum_r = 0;

			for (int u = -1; u <= 1; u++) {
				for (int v = -1; v <= 1; v++) {
					Vec3b pixel = src.at<Vec3b>(i + u, j + v);
					int weight = kernel[u + 1][v + 1];

					sum_b += pixel[0] * weight;
					sum_g += pixel[1] * weight;
					sum_r += pixel[2] * weight;
				}
			}

			dst.at<Vec3b>(i, j)[0] = saturate_cast<uchar>(sum_b / kernel_sum);
			dst.at<Vec3b>(i, j)[1] = saturate_cast<uchar>(sum_g / kernel_sum);
			dst.at<Vec3b>(i, j)[2] = saturate_cast<uchar>(sum_r / kernel_sum);
		}
	}

	return dst;
}

void test_traffic_light_step1()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);
		if (src.empty())
		{
			continue;
		}

		if (src.cols > 800) {
			float scale = 800.0f / src.cols;
			cv::resize(src, src, Size(), scale, scale);
		}
		Mat blurred = apply_gaussian_blur_color(src);

		imshow("Imagine Originala", src);
		imshow("Pas 1 - Gaussian Blur Manual", blurred);

		waitKey(0);
		destroyAllWindows();
	}
}

// ============================================================
// Pasul 2: Conversia din BGR în HSV (manuala)
// ============================================================

void compute_HSV_manual(const Mat& src, Mat& Hue, Mat& Saturation, Mat& Value)
{
	int height = src.rows;
	int width = src.cols;

	Hue = Mat::zeros(height, width, CV_8UC1);
	Saturation = Mat::zeros(height, width, CV_8UC1);
	Value = Mat::zeros(height, width, CV_8UC1);

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			Vec3b color = src.at<Vec3b>(row, col);
			float b = (float)color[0] / 255.0f;
			float g = (float)color[1] / 255.0f;
			float r = (float)color[2] / 255.0f;

			float M = max(r, max(g, b));
			float m = min(r, min(g, b));
			float C = M - m;

			float V = M;
			float S = 0.0f, H = 0.0f;

			if (V != 0.0f)
				S = C / V;

			if (C != 0.0f) {
				if (M == r)
					H = 60.0f * (g - b) / C;
				else if (M == g)
					H = 120.0f + 60.0f * (b - r) / C;
				else if (M == b)
					H = 240.0f + 60.0f * (r - g) / C;
			}

			if (H < 0.0f)
				H += 360.0f;

			Hue.at<uchar>(row, col) = (uchar)(H / 2.0f);
			Saturation.at<uchar>(row, col) = (uchar)(S * 255.0f);
			Value.at<uchar>(row, col) = (uchar)(V * 255.0f);
		}
	}
}

// ============================================================
// PASUL 3: Creare masti de culoare pe baza pragurilor HSV
//
// FIX-URI APLICATE PENTRU GALBEN:
//   1. Intervalul H extins la [15, 35] pentru a prinde mai bine galbenul
//   2. Doua cazuri de detectie:
//      a) Periferia becului: S > 60 (galben saturat normal)
//      b) Centrul becului supraluminat: S <= 60 dar V > 200 si R,G puternice
//         (centrul aprins apare aproape alb => S scazuta)
// ============================================================

void extract_color_masks(const Mat& src_blurred,
	const Mat& H, const Mat& S, const Mat& V,
	Mat& maskRed, Mat& maskYellow, Mat& maskGreen)
{
	int height = src_blurred.rows;
	int width = src_blurred.cols;

	maskRed = Mat::zeros(height, width, CV_8UC1);
	maskYellow = Mat::zeros(height, width, CV_8UC1);
	maskGreen = Mat::zeros(height, width, CV_8UC1);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			uchar h = H.at<uchar>(i, j);
			uchar s = S.at<uchar>(i, j);
			uchar v = V.at<uchar>(i, j);

			Vec3b color = src_blurred.at<Vec3b>(i, j);
			uchar b = color[0];
			uchar g = color[1];
			uchar r = color[2];

			// --- FILTRUL PENTRU ROSU ---
			// Rosul are H aproape de 0 sau aproape de 180 (capetele cercului HSV)
			if (s > 60 && v > 100) {
				if (h <= 10 || h >= 160) {
					// R sa domine clar G
					if (r > g + 30) {
						maskRed.at<uchar>(i, j) = 255;
					}
				}
			}

			// --- FILTRUL PENTRU GALBEN (FIX) ---
			// H intre 15-35 (galben in OpenCV scale 0-179)
			if (h >= 15 && h <= 35) {
				// Cazul 1: Periferia becului - saturat normal
				bool periphery = (s > 40 && v > 80
					&& r > 100 && g > 90
					&& abs((int)r - (int)g) <= 60
					&& b < 140);

				// Cazul 2: Centrul supraluminat - aproape alb, S mica dar V foarte mare
				// Centrul unui bec galben aprins intens are S scazuta pentru ca e prea luminos
				bool center = (s <= 60 && v > 200
					&& r > 180 && g > 160
					&& b < 130);

				if (periphery || center) {
					maskYellow.at<uchar>(i, j) = 255;
				}
			}

			// --- FILTRUL PENTRU VERDE ---
			if (s > 60 && v > 100) {
				if (h >= 40 && h <= 90) {
					// G sa domine clar R
					if (g > r + 20) {
						maskGreen.at<uchar>(i, j) = 255;
					}
				}
			}
		}
	}
}

// ============================================================
// PASUL 4: Morfologie matematica
//
// FIX-URI APLICATE:
//   - Pentru GALBEN: ordinea s-a inversat -> CLOSING primul, OPENING al doilea
//     Motivul: closing umple mai intai gaura din centrul supraluminat,
//     apoi opening elimina zgomotul exterior pe masca deja intreaga.
//   - Dimensiunea opening pentru galben redusa (8->3) pentru a nu distruge
//     blob-ul deja fragmentat inainte de filling.
//   - Dimensiunea closing pentru galben marita (8->12) pentru a acoperi
//     gaura centrala care poate fi destul de larga.
// ============================================================

void test_traffic_light_segmentation()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);
		if (src.empty()) continue;

		// Redimensionare pentru baza constanta de comparatie
		if (src.cols > 800) {
			float scale = 800.0f / src.cols;
			cv::resize(src, src, Size(), scale, scale);
		}

		// PAS 1: Preprocesare (Blur Gaussian color)
		Mat blurred = apply_gaussian_blur_color(src);

		// PAS 2: Conversie HSV manuala
		Mat H, S, V;
		compute_HSV_manual(blurred, H, S, V);

		// PAS 3: Segmentare culori (masti brute cu validare HSV + RGB)
		Mat maskRed, maskYellow, maskGreen;
		extract_color_masks(blurred, H, S, V, maskRed, maskYellow, maskGreen);

		// PAS 4: Morfologie matematica

		// --- ROSU si VERDE: pipeline original (opening -> closing -> fill) ---
		// Becurile rosu si verde nu au problema cu centrul supraluminat
		// la aceeasi intensitate ca galbenul, deci ordinea originala e OK.
		Mat opRed = opening(maskRed, 1, 8);
		Mat clRed = closing(opRed, 1, 8);
		Mat finalRed = regionFill(clRed, 8);

		// GREEN
		Mat opGreen = opening(maskGreen, 1, 8);
		Mat clGreen = closing(opGreen, 1, 8);
		Mat finalGreen = regionFill(clGreen, 8);

		// YELLOW
		Mat opYellow = opening(maskYellow, 1, 8);
		Mat clYellow = closing(opYellow, 1, 8);
		Mat finalYellow = regionFill(clYellow, 8);

		// --- AFISARE REZULTATE COMPARATIVE ---
		imshow("Imagine Originala", src);

		imshow("ROSU   (Brut - Pas 3)", maskRed);
		imshow("ROSU   (Final - Pas 4)", finalRed);

		imshow("GALBEN (Brut - Pas 3)", maskYellow);
		imshow("GALBEN (Final - Pas 4)", finalYellow);

		imshow("VERDE  (Brut - Pas 3)", maskGreen);
		imshow("VERDE  (Final - Pas 4)", finalGreen);

		if (waitKey(0) == 27) break; // ESC pentru a iesi
	}
	destroyAllWindows();
}








// first step - BGR -> HSI conversion
void convert_BGR_to_HSI(const Mat& src, Mat& Hue, Mat& Saturation, Mat& Intensity)
{
	int height = src.rows;
	int width = src.cols;

	Hue = Mat::zeros(height, width, CV_32FC1);
	Saturation = Mat::zeros(height, width, CV_32FC1);
	Intensity = Mat::zeros(height, width, CV_32FC1);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vec3b bgr = src.at<Vec3b>(i, j);
			float b = (float)bgr[0];
			float g = (float)bgr[1];
			float r = (float)bgr[2];

			// intensity (I)
			float I = (r + g + b) / 3.0f;

			// saturation (S)
			float min_val = min(r, min(g, b));
			float S = 0.0f;
			if (I > 0.0f) {
				S = 1.0f - (min_val / I);
			}

			// Hue (H)
			float H = 0.0f;
			float numerator = 0.5f * ((r - g) + (r - b));
			float denominator = sqrt((r - g) * (r - g) + (r - b) * (g - b));

			if (denominator != 0.0f) {
				float theta = acos(numerator / denominator);
				H = theta * 180.0f / CV_PI;    

				if (b > g) {
					H = 360.0f - H;
				}
			}

			Hue.at<float>(i, j) = H;
			Saturation.at<float>(i, j) = S;
			Intensity.at<float>(i, j) = I;
		}
	}
}

// helper function for resize
void resize_for_display(Mat& src, int max_dim = 800)
{
	if (src.cols > max_dim || src.rows > max_dim) {
		float scale_w = (float)max_dim / src.cols;
		float scale_h = (float)max_dim / src.rows;

		float scale = min(scale_w, scale_h);

		cv::resize(src, src, Size(), scale, scale);
	}
}

// test BGR -> HSI conversion
void test_HSI_conversion()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);
		if (src.empty()) {
			continue;
		}

		resize_for_display(src);

		int height = src.rows;
		int width = src.cols;

		Mat H, S, I;
		convert_BGR_to_HSI(src, H, S, I);

		Mat H_display = Mat::zeros(height, width, CV_8UC1);
		Mat S_display = Mat::zeros(height, width, CV_8UC1);
		Mat I_display = Mat::zeros(height, width, CV_8UC1);

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				float h_val = H.at<float>(i, j);
				H_display.at<uchar>(i, j) = (uchar)((h_val * 255.0f) / 360.0f);

				float s_val = S.at<float>(i, j);
				S_display.at<uchar>(i, j) = (uchar)(s_val * 255.0f);

				float i_val = I.at<float>(i, j);
				I_display.at<uchar>(i, j) = (uchar)(i_val);
			}
		}

		imshow("Original Image", src);
		imshow("Hue (H)", H_display);
		imshow("Saturation (S)", S_display);
		imshow("Intensity (I)", I_display);

		waitKey(0);
		destroyAllWindows();
	}
}

// second step - creating the binary foreground map, using HSI thresholds
void compute_foreground_map(const Mat& H, const Mat& S, const Mat& I, Mat& b_map)
{
	int height = H.rows;
	int width = H.cols;

	b_map = Mat::zeros(height, width, CV_8UC1);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			float hue = H.at<float>(i, j);
			float saturation = S.at<float>(i, j);
			float intensity = I.at<float>(i, j);

			// red detection
			bool isRed = ((hue >= 0.0f && hue <= 20.0f) || (hue >= 340.0f && hue <= 360.0f)) &&
				(saturation >= 0.13f && saturation <= 1.0f) &&
				(intensity >= 80.0f && intensity <= 255.0f);

			// yellow detection
			bool isYellow = (hue >= 25.0f && hue <= 60.0f) &&
				(saturation >= 0.15f && saturation <= 1.0f) &&
				(intensity >= 120.0f && intensity <= 255.0f);

			// green detection
			bool isGreen = (hue >= 120.0f && hue <= 180.0f) &&
				(saturation >= 0.12f && saturation <= 1.0f) &&
				(intensity >= 110.0f && intensity <= 255.0f);

			if (isRed || isYellow || isGreen) {
				b_map.at<uchar>(i, j) = 255;
			}
		}
	}
}

// test binary foreground map creation
void test_foreground_map()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);
		if (src.empty()) {
			continue;
		}

		resize_for_display(src);

		Mat H, S, I;
		convert_BGR_to_HSI(src, H, S, I);

		// color screening
		Mat b_map;
		compute_foreground_map(H, S, I, b_map);

		// visualization matrix
		int height = src.rows;
		int width = src.cols;
		Mat visualization(height, width, CV_8UC3);

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (b_map.at<uchar>(i, j) == 255) {
					visualization.at<Vec3b>(i, j) = src.at<Vec3b>(i, j);
				}
				else {
					visualization.at<Vec3b>(i, j) = Vec3b(255, 255, 255); // white
				}
			}
		}

		imshow("Original Image", src);
		imshow("Binary Map", b_map);
		imshow("Color Screening", visualization);

		waitKey(0);
		destroyAllWindows();
	}
}

// third step - apply morphological operations (closing + opening)
Mat apply_morphological_operations(const Mat& b_map)
{
	Mat inverted_bmap = 255 - b_map;

	Mat closed_map = closing(inverted_bmap, 1, 8);
	Mat opened_map = opening(closed_map, 1, 8);

	return opened_map;
}

// test morphological operations effect
void test_morphological_operations()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);
		if (src.empty()) {
			continue;
		}

		resize_for_display(src);

		Mat H, S, I, b_map;
		convert_BGR_to_HSI(src, H, S, I);
		compute_foreground_map(H, S, I, b_map);

		Mat morphed_map = apply_morphological_operations(b_map);
		Mat display_morphed = 255 - morphed_map;

		imshow("Map before", b_map);
		imshow("Map after closing + opening", display_morphed);

		waitKey(0);
		destroyAllWindows();
	}
}

// fourth step - label and process candidate regions by area
labels process_candidate_regions(const Mat& morphed_map, Mat& final_candidates_map)
{
	labels labeled_data = Two_pass_labelling(morphed_map);
	int rows = labeled_data.labels.rows;
	int cols = labeled_data.labels.cols;
	Mat labels_matrix = labeled_data.labels;

	std::vector<int> areas(labeled_data.no_labels + 1, 0);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			int lbl = labels_matrix.at<int>(i, j);
			if (lbl > 0) {
				areas[lbl]++;
			}
		}
	}

	final_candidates_map = Mat::zeros(morphed_map.size(), CV_8UC1);
	Mat filtered_labels_mat = Mat::zeros(morphed_map.size(), CV_32SC1);

	int valid_candidates = 0;
	std::vector<int> valid_label_map(labeled_data.no_labels + 1, 0);

	int MIN_AREA_THRESHOLD = 500;

	// filtering labeled regions by area
	for (int i = 1; i <= labeled_data.no_labels; i++) {
		if (areas[i] > MIN_AREA_THRESHOLD) {
			valid_candidates++;
			valid_label_map[i] = valid_candidates;
		}
	}

	// final binary map and valid labels matrix
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			int old_lbl = labels_matrix.at<int>(i, j);

			if (old_lbl > 0) {
				int new_lbl = valid_label_map[old_lbl];

				if (new_lbl > 0) {
					final_candidates_map.at<uchar>(i, j) = 255;
					filtered_labels_mat.at<int>(i, j) = new_lbl;
				}
			}
		}
	}

	labels result_labels = { filtered_labels_mat, valid_candidates };
	return result_labels;
}

// test candidate regions selection
void test_candidate_regions()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);
		if (src.empty()) {
			continue;
		}

		resize_for_display(src);

		Mat H, S, I;
		convert_BGR_to_HSI(src, H, S, I);

		Mat b_map;
		compute_foreground_map(H, S, I, b_map);

		Mat morphed_map = apply_morphological_operations(b_map);

		Mat final_candidates_map;
		labels valid_labels = process_candidate_regions(morphed_map, final_candidates_map);

		Mat colored_labels = color_labels(valid_labels);

		imshow("Original image", src);
		imshow("Map before processing candidate regions", 255 - morphed_map);
		imshow("Candidate regions", final_candidates_map);
		imshow("Labeled candidate regions", colored_labels);

		waitKey(0);
		destroyAllWindows();
	}
}

// struct for a traffic light candidate region
struct TrafficLightCandidate {
	int id;
	Point2d centroid;
	bool is_circle;
};


// helper function for getting object mask based on its label
Mat get_obj_by_label(const Mat& label_mat, int label_id) {
	Mat mask = Mat::zeros(label_mat.size(), CV_8UC1);
	for (int i = 0; i < label_mat.rows; i++) {
		for (int j = 0; j < label_mat.cols; j++) {
			if (label_mat.at<int>(i, j) == label_id) {
				mask.at<uchar>(i, j) = 255;
			}
		}
	}
	return mask;
}

// fifth step - filtering candidate regions based on their shape (circle + ellipse)
std::vector<TrafficLightCandidate> filter_by_shape(const labels& valid_labels)
{
	std::vector<TrafficLightCandidate> results;
	int height = valid_labels.labels.rows;
	int width = valid_labels.labels.cols;

	for (int current_label = 1; current_label <= valid_labels.no_labels; current_label++) {
		Mat obj = get_obj_by_label(valid_labels.labels, current_label);

		// computing bounding box and aspect ratio
		rectangle_coord bb = compute_bounding_box(obj);
		double aspect_ratio = compute_aspect_ratio(bb);

		// check object's compactness
		bool is_compact = (aspect_ratio > 0.5 && aspect_ratio < 1.8);

		std::vector<Point> region_pixels;
		std::vector<int> min_x(height, width + 1);
		std::vector<int> max_x(height, -1);
		std::vector<int> min_y(width, height + 1);
		std::vector<int> max_y(width, -1);

		// computing object's outer boundary
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (obj.at<uchar>(i, j) == 255) {
					region_pixels.push_back(Point(j, i));
					if (j < min_x[i]) min_x[i] = j;
					if (j > max_x[i]) max_x[i] = j;
					if (i < min_y[j]) min_y[j] = i;
					if (i > max_y[j]) max_y[j] = i;
				}
			}
		}

		if (region_pixels.empty()) {
			continue;
		}

		// computing centroid
		double sum_x = 0, sum_y = 0;
		for (Point p : region_pixels) {
			sum_x += p.x;
			sum_y += p.y;
		}
		double X_i = sum_x / region_pixels.size();
		double Y_i = sum_y / region_pixels.size();

		// outer border points
		std::vector<Point> outer_border;
		for (int i = 0; i < height; i++) {
			if (max_x[i] != -1) {
				outer_border.push_back(Point(min_x[i], i));
				outer_border.push_back(Point(max_x[i], i));
			}
		}
		for (int j = 0; j < width; j++) {
			if (max_y[j] != -1) {
				outer_border.push_back(Point(j, min_y[j]));
				outer_border.push_back(Point(j, max_y[j]));
			}
		}

		// computing radii
		double r_max = -1.0;
		double r_min = 99999999.0;

		for (Point p : outer_border) {
			double r = std::sqrt((p.x - X_i) * (p.x - X_i) + (p.y - Y_i) * (p.y - Y_i));
			if (r > r_max) {
				r_max = r;
			}
			if (r < r_min) {
				r_min = r;
			}
		}

		// final decision (based on the geometry of the object)
		double shape_variance_ratio = (r_max - r_min) / r_max;
		bool is_circle = (shape_variance_ratio <= 0.45); // 45% tolerance threshold

		bool is_valid = is_circle && is_compact && (r_max > 5.0);

		results.push_back({ current_label, Point2d(X_i, Y_i), is_valid });
	}

	return results;
}

// test traffic light detection by shape
void test_traffic_light_shape_detection()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);
		if (src.empty()) {
			continue;
		}

		resize_for_display(src);

		Mat output_display = src.clone();

		Mat H, S, I;
		convert_BGR_to_HSI(src, H, S, I);

		Mat b_map;
		compute_foreground_map(H, S, I, b_map);

		Mat morphed_map = apply_morphological_operations(b_map);

		Mat final_candidates_map;
		labels valid_labels = process_candidate_regions(morphed_map, final_candidates_map);
		Mat colored_labels = color_labels(valid_labels);

		std::vector<TrafficLightCandidate> final_results = filter_by_shape(valid_labels);

		int detected_shapes = 0;
		for (const auto& candidate : final_results) {
			if (candidate.is_circle) {
				detected_shapes++;

				Point center((int)candidate.centroid.x, (int)candidate.centroid.y);
				drawMarker(output_display, center, Scalar(255, 0, 255), MARKER_CROSS, 15, 2);
				circle(output_display, center, 20, Scalar(0, 255, 0), 2);
			}
		}

		imshow("Colored labels", colored_labels);
		imshow("Shape detection", output_display);

		waitKey(0);
		destroyAllWindows();
	}
}

// helper function to get color's name
std::string get_color_name(int color_code) {
	if (color_code == 1) {
		return "RED";
	}

	if (color_code == 2) {
		return "YELLOW";
	}

	if (color_code == 3) {
		return "GREEN";
	}
	return "";
}

// sixth step - classify region's color
int classify_candidate(Mat obj_mask, Mat H, Mat S, Mat I)
{
	int red_count = 0, yellow_count = 0, green_count = 0, total_pixels = 0;

	for (int i = 0; i < obj_mask.rows; i++) {
		for (int j = 0; j < obj_mask.cols; j++) {
			if (obj_mask.at<uchar>(i, j) == 255) {
				total_pixels++;
				float hue = H.at<float>(i, j);
				float sat = S.at<float>(i, j);
				float inten = I.at<float>(i, j);

				// new color thresholds for classifying, avoiding overlapping
				if (((hue >= 0.0f && hue <= 20.0f) || (hue >= 340.0f && hue <= 360.0f)) &&
					(sat >= 0.13f) && (inten >= 80.0f)) {
					red_count++;
				}
				else if ((hue >= 25.0f && hue <= 60.0f) &&
					(sat >= 0.15f) && (inten >= 120.0f)) {
					yellow_count++;
				}
				else if ((hue >= 100.0f && hue <= 190.0f) &&
					(sat >= 0.05f) && (inten >= 100.0f)) {
					green_count++;
				}
			}
		}
	}

	if (total_pixels == 0) {
		return 0;
	}

	// compute color density percentages
	float p_red = (float)red_count / total_pixels;
	float p_yellow = (float)yellow_count / total_pixels;
	float p_green = (float)green_count / total_pixels;

	// TH_c = 0.5 (the dominant color must cover more than 50% of the candidate region)
	if (p_red > 0.5f) {
		return 1;
	}
	if (p_yellow > 0.5f) {
		return 2;
	}
	if (p_green > 0.5f) {
		return 3;
	}

	return 0; // unknown color
}

// test color classification
void test_traffic_light_final_detection()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_COLOR);
		if (src.empty()) {
			continue;
		}

		resize_for_display(src);

		Mat output_display = src.clone();

		Mat H, S, I;
		convert_BGR_to_HSI(src, H, S, I);

		Mat b_map;
		compute_foreground_map(H, S, I, b_map);

		Mat morphed_map = apply_morphological_operations(b_map);

		Mat final_candidates_map;
		labels valid_labels = process_candidate_regions(morphed_map, final_candidates_map);
		Mat colored_labels = color_labels(valid_labels);

		std::vector<TrafficLightCandidate> final_results = filter_by_shape(valid_labels);

		int confirmed_lights = 0;
		for (const auto& candidate : final_results) {
			if (candidate.is_circle) {
				Mat mask = get_obj_by_label(valid_labels.labels, candidate.id);

				int color = classify_candidate(mask, H, S, I);
				if (color != 0) {
					confirmed_lights++;
					std::string label_text = get_color_name(color);
					Point center((int)candidate.centroid.x, (int)candidate.centroid.y);

					Scalar text_color;
					if (color == 1) {
						text_color = Scalar(0, 0, 255); // RED
					}
					else if (color == 2) {
						text_color = Scalar(0, 255, 255); // YELLOW
					}
					else {
						text_color = Scalar(0, 255, 0); // GREEN
					}

					putText(output_display, label_text, Point(center.x + 15, center.y + 5),
						FONT_HERSHEY_SIMPLEX, 0.7, text_color, 2);
				}
			}
		}

		imshow("Colored labels", colored_labels);
		imshow("Detected traffic light(s)", output_display);

		waitKey(0);
		destroyAllWindows();
	}
}

// !!!!! FA ALGORITMUL PENTRU RECUNOASTERE SAGEATA INTERMITENT LA SEMAFOR






void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == EVENT_LBUTTONDOWN)
		{
			printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
				x, y,
				(int)(*src).at<Vec3b>(y, x)[2],
				(int)(*src).at<Vec3b>(y, x)[1],
				(int)(*src).at<Vec3b>(y, x)[0]);
		}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

int main() 
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
    projectPath = _wgetcwd(0, 0);

	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Open image\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Image negative\n");
		printf(" 4 - Image negative (fast)\n");
		printf(" 5 - BGR->Gray\n");
		printf(" 6 - BGR->Gray (fast, save result to disk) \n");
		printf(" 7 - BGR->HSV\n");
		printf(" 8 - Resize image\n");
		printf(" 9 - Canny edge detection\n");
		printf(" 10 - Edges in a video sequence\n");
		printf(" 11 - Snap frame from live video\n");
		printf(" 12 - Mouse callback demo\n");
		printf(" 13 - Additive Factor\n");
		printf(" 14 - Multiplicative Factor\n");
		printf(" 15 - Show Colourful Image\n");
		printf(" 16 - Draw Blue Circle Over Colourful Image\n");
		printf(" 17 - Draw Color Channels Matrices\n");
		printf(" 18 - Convert from Color to Grayscale\n");
		printf(" 19 - Convert from grayscale to black and white\n");
		printf(" 20 - RGB TO HSV\n");
		printf(" 21 - Show histogram\n");
		printf(" 22 - Geometric features ex\n");
		printf(" 23 - Show labeled image\n");
		printf(" 24 - Show contour\n");
		printf(" 25 - Reconstruct contour from file\n");
		printf(" 26 - Dilated image\n");
		printf(" 27 - Eroded image\n");
		printf(" 28 - Opening image\n");
		printf(" 29 - Closing image\n");
		printf(" 30 - Boundary extraction\n");
		printf(" 31 - Region filling\n");
		printf(" 32 - Mean, standard deviation, histogram and cumulative histogram\n");
		printf(" 33 - Global automated binarization\n");
		printf(" 34 - Change contrast\n");
		printf(" 35 - Gamma correction\n");
		printf(" 36 - Histogram equalization\n");
		printf(" 37 - Apply convolution\n");
		printf(" 38 - Ordered filtering (median, min, max)\n");
		printf(" 39 - Traffic light detection - Step 1 (Gaussian Blur)\n");
		printf(" 40 - Traffic light detection - Step 2 (BGR to HSV conversion)\n");
		printf(" 41 - HSI conversion\n");
		printf(" 42 - Foreground map for traffic light detection\n");
		printf(" 43 - Apply morphological operations\n");
		printf(" 44 - Candidate regions for traffic light detection\n");
		printf(" 45 - Test candidate regions for traffic light detection (shape-based filtering)\n");
		printf(" 46 - Test final sistem de detectie semafoare (Toate etapele)\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d",&op);
		switch (op)
		{
			case 1:
				testOpenImage();
				break;
			case 2:
				testOpenImagesFld();
				break;
			case 3:
				testNegativeImage();
				break;
			case 6:
				testImageOpenAndSave();
				break;
			case 12:
				testMouseClick();
				break;
			case 13:
				testAdditiveFactorImage();
				break;
			case 14:
				testMultiplicativeFactorImage();
				break;
			case 15:
				createNewImage();
				break;
			case 16:
				drawBlueCircle();
				break;
			case 17:
				colorChannels();
				break;
			case 18:
				colorToGrayScale();
				break;
			case 19:
				grayscaleToBlackAndWhite();
				break;
			case 20:
				RGBtoHSV();
				break;
			case 21:
				seeHistogram();
				break;
			case 22:
				geometric_features_ex();
				break;
			case 23:
				show_labels();
				break;
			case 24:
				show_contour();
				break;
			case 25:
				reconstructEx3();
				break;
			case 26:
				testDilate();
				break;
			case 27:
				testErode();
				break;
			case 28:
				testOpening();
				break;
			case 29:
				testClosing();
				break;
			case 30:
				testBoundaryExtraction();
				break;
			case 31:
				testRegionFill();
				break;
			case 32:
				test_ex1_lab8();
				break;
			case 33:
				test_global_automated_binarization();
				break;
			case 34:
				test_change_contrast();
				break;
			case 35:
				test_gamma_correction();
				break;
			case 36:
				test_equalize_histogram();
				break;
			case 37:
				test_convolution();
				break;
			case 38:
				test_ordered_filtering();
				break;
			case 39:
				test_traffic_light_step1();
				break;
			case 40:
				test_traffic_light_segmentation();
				break;
			case 41:
				test_HSI_conversion();
				break;
			case 42:
				test_foreground_map();
				break;
			case 43:
				test_morphological_operations();
				break;
			case 44:
				test_candidate_regions();
				break;
			case 45:
				test_traffic_light_shape_detection();
				break;
			case 46:
				test_traffic_light_final_detection();
				break;
			default:
				break;
		}
	}
	while (op!=0);
	return 0;
}