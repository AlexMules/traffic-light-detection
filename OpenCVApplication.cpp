// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>

#include <random>
#include <fstream>

wchar_t* projectPath;

// problema 2.5
/* 5. Implementați o funcție isInside(img, i, j) care verifică dacă poziția indicată de 
perechea (i,j) (rând, coloană) este înăuntrul imaginii img. */
bool isInside(Mat src, int i, int j)
{
	return (i >= 0 && i < src.rows && j >= 0 && j < src.cols);
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

Mat opening(Mat src, int n_iterations, int connectivity) {
	Mat eroded = erode(src, n_iterations, connectivity);
	Mat opened = dilate(eroded, n_iterations, connectivity);

	return opened;
}

Mat closing(Mat src, int n_iterations, int connectivity) {
	Mat dilated = dilate(src, n_iterations, connectivity);
	Mat closed = erode(dilated, n_iterations, connectivity);

	return closed;
}

// PROJECT
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
					if (j < min_x[i]) {
						min_x[i] = j;
					}
					if (j > max_x[i]) {
						max_x[i] = j;
					}
					if (i < min_y[j]) {
						min_y[j] = i;
					}
					if (i > max_y[j]) {
						max_y[j] = i;
					}
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

		Mat validated_shapes_only = Mat::zeros(colored_labels.size(), CV_8UC3);

		int detected_shapes = 0;
		for (const auto& candidate : final_results) {
			if (candidate.is_circle) {
				detected_shapes++;

				Mat mask = get_obj_by_label(valid_labels.labels, candidate.id);
				
				for (int i = 0; i < colored_labels.rows; i++) {
					for (int j = 0; j < colored_labels.cols; j++) {
						if (mask.at<uchar>(i, j) == 255) {
							validated_shapes_only.at<Vec3b>(i, j) = colored_labels.at<Vec3b>(i, j);
						}
					}
				}

				Point center((int)candidate.centroid.x, (int)candidate.centroid.y);
				drawMarker(output_display, center, Scalar(255, 0, 255), MARKER_CROSS, 15, 2);
				circle(output_display, center, 20, Scalar(0, 255, 0), 2);
			}
		}

		imshow("Colored labels", colored_labels);
		imshow("Filtered valid shape(s)", validated_shapes_only);
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
		Mat final_validated_lights = Mat::zeros(colored_labels.size(), CV_8UC3);

		int confirmed_lights = 0;
		for (const auto& candidate : final_results) {
			if (candidate.is_circle) {
				Mat mask = get_obj_by_label(valid_labels.labels, candidate.id);

				int color = classify_candidate(mask, H, S, I);
				if (color != 0) {
					confirmed_lights++;
					for (int i = 0; i < colored_labels.rows; i++) {
						for (int j = 0; j < colored_labels.cols; j++) {
							if (mask.at<uchar>(i, j) == 255) {
								final_validated_lights.at<Vec3b>(i, j) = colored_labels.at<Vec3b>(i, j);
							}
						}
					}

					std::string label_text = get_color_name(color);
					Point center((int)candidate.centroid.x, (int)candidate.centroid.y);

					putText(output_display, label_text, Point(center.x + 15, center.y + 5),
						FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
				}
			}
		}

		imshow("Colored labels", colored_labels);
		imshow("Final validated light(s)", final_validated_lights);
		imshow("Detected traffic light(s)", output_display);

		waitKey(0);
		destroyAllWindows();
	}
}

// morphological operations applied for arrow detection
Mat apply_morphological_operations_arrow(const Mat& b_map)
{
	Mat inverted_bmap = 255 - b_map;

	Mat closed_map = closing(inverted_bmap, 5, 8);

	Mat opened_map = opening(closed_map, 2, 8);

	return opened_map;
}

// function for identifying and classifying arrows direction
int classify_arrow_direction(const Mat& mask)
{
	// computing bounding box
	rectangle_coord bb = compute_bounding_box(mask);
	int w = bb.c_max - bb.c_min + 1;
	int h = bb.r_max - bb.r_min + 1;

	// rejecting objects that are too big or too small
	if (w < 20 || h < 20 || w > 200 || h > 200) {
		return 0;
	}

	// computing the area of the object
	// we define a center point (cx, cy) and we count how the pixels are distributed arount the center point
	// L = left, R = right, D = down, U = UP
	int area = 0;
	int cx = bb.c_min + w / 2;
	int cy = bb.r_min + h / 2;
	int L = 0, R = 0, U = 0, D = 0;

	for (int i = bb.r_min; i <= bb.r_max; i++) {
		for (int j = bb.c_min; j <= bb.c_max; j++) {
			if (mask.at<uchar>(i, j) == 255) {
				area++;
				if (j < cx) {
					L++; // left pixel
				}
				else if (j > cx) {
					R++; // right pixel
				}

				if (i < cy) {
					U++; // up pixel
				}
				else if (i > cy) {
					D++; // down pixel
				}
			}
		}
	}

	// if object is too small, it's not an arrow	
	if (area < 200) {
		return 0;
	}

	// a filled circle/square has a ratio close to 1.0.
	// an arrow has a lower fill ratio, so this blocks solid objects from being detected as arrows
	double fill_ratio = (double)area / (w * h);
	if (fill_ratio < 0.35 || fill_ratio > 0.72) {
		return 0;
	}

	// sym_LR (Left-Right symmetry) compares the density of both sides
	// sym_UD (Up-Down symmetry) compares the density of both sides
	// a perfect rectangle has a ratio of 1.0; for an arrow, the value is lower
	double sym_LR = (double)min(L, R) / (max(L, R) + 1);
	double sym_UD = (double)min(U, D) / (max(U, D) + 1);
	double aspect_ratio = (double)w / h;

	// horizontal arrow
	bool is_horizontal = (sym_LR < 0.80 && sym_UD > 0.60 && aspect_ratio > 0.65);

	// vertical arrow
	bool is_vertical = (sym_UD < 0.80 && sym_LR > 0.60 && aspect_ratio < 1.40);

	// if it's not horiozntal nor vertical, it is NOT an arrow
	if (!is_horizontal && !is_vertical) {
		return 0;
	}

	// final decision
	if (is_horizontal) {
		if (L > R) {
			return 2; // left arrow
		}
		else {
			return 3; // right arrow
		}
	}
	else {
		if (U > D) {
			return 1; // up arrow
		}
	}

	return 0; // NOT an arrow
}

// test arrow detection
void test_traffic_light_arrow_detection()
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

		Mat morphed_map_arrow = apply_morphological_operations_arrow(b_map);

		Mat final_candidates_arrow_map;
		labels valid_labels_arrow = process_candidate_regions(morphed_map_arrow, final_candidates_arrow_map);
		std::vector<TrafficLightCandidate> results_arrow = filter_by_shape(valid_labels_arrow);
		Mat colored_labels_arrow = color_labels(valid_labels_arrow);

		Mat final_validated_arrows = Mat::zeros(src.size(), CV_8UC3);

		for (const auto& candidate : results_arrow) {
			if (!candidate.is_circle) {
				Mat mask = get_obj_by_label(valid_labels_arrow.labels, candidate.id);
				int arrow_dir = classify_arrow_direction(mask);

				if (arrow_dir != 0) {
					int color = classify_candidate(mask, H, S, I);
					if (color != 0) {
						for (int i = 0; i < colored_labels_arrow.rows; i++) {
							for (int j = 0; j < colored_labels_arrow.cols; j++) {
								if (mask.at<uchar>(i, j) == 255) {
									final_validated_arrows.at<Vec3b>(i, j) = colored_labels_arrow.at<Vec3b>(i, j);
								}
							}
						}

						std::string direction_name = (arrow_dir == 1) ? "UP" : (arrow_dir == 2) ? "LEFT" : "RIGHT";
						std::string color_name = get_color_name(color);
						std::string final_label = color_name + " ARROW " + direction_name;

						Point center((int)candidate.centroid.x, (int)candidate.centroid.y);

						drawMarker(output_display, center, Scalar(255, 0, 255), MARKER_CROSS, 15, 2);
						putText(output_display, final_label, Point(center.x + 15, center.y + 5),
							FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
					}
				}
			}
		}

		imshow("Colored labeled candidates", colored_labels_arrow);
		imshow("Final validated arrows", final_validated_arrows);
		imshow("Detected arrows", output_display);

		waitKey(0);
		destroyAllWindows();
	}
}

 // function for detecting and classifying arrow based on projections
int classify_arrow_direction_projections(const Mat& mask)
{
	// compute bounding box
	rectangle_coord bb = compute_bounding_box(mask);
	int w = bb.c_max - bb.c_min + 1;
	int h = bb.r_max - bb.r_min + 1;

	// too small or too large objects are not arrows
	if (w < 20 || h < 20 || w > 200 || h > 200) {
		return 0;
	}

	// computing area and horizontal and vertical projections
	int area = 0;
	std::vector<int> proj_h(w, 0); // horizontal projection
	std::vector<int> proj_v(h, 0); // vertical projection

	for (int i = bb.r_min; i <= bb.r_max; i++) {
		for (int j = bb.c_min; j <= bb.c_max; j++) {
			if (mask.at<uchar>(i, j) == 255) {
				area++;
				proj_h[j - bb.c_min]++;
				proj_v[i - bb.r_min]++;
			}
		}
	}

	// if object is too small, it's not an arrow	
	if (area < 200) {
		return 0;
	}

	// a filled circle/square has a ratio close to 1.0.
	// an arrow has a lower fill ratio, so this blocks solid objects from being detected as arrows
	double fill_ratio = (double)area / (w * h);
	if (fill_ratio < 0.35 || fill_ratio > 0.72) {
		return 0;
	}

	// we define a center point (cx, cy) and we count how the pixels are distributed arount the center point
	// L = left, R = right, D = down, U = UP
	int cx = w / 2;
	int cy = h / 2;
	int L = 0, R = 0, U = 0, D = 0;
	for (int j = 0; j < w; j++) { 
		if (j < cx) {
			L += proj_h[j]; // left pixel
		}
		else {
			R += proj_h[j]; // right pixel
		}
	}
	for (int i = 0; i < h; i++) { 
		if (i < cy) {
			U += proj_v[i]; // up pixel
		}
		else {
			D += proj_v[i]; // down pixel
		}
	}

	// computing assymetry
	// an arrow is highly asymmetrical in the direction it points
	double asym_h = (double)abs(L - R) / area;
	double asym_v = (double)abs(U - D) / area;

	// computing aspect ratio
	double aspect_ratio = (double)w / h;

	// horizontal arrow
	bool is_horizontal = (aspect_ratio > 0.75) && (asym_h > 0.12) && (asym_v < 0.35);

	// vertical arrow
	bool is_vertical = (aspect_ratio < 1.35) && (asym_v > 0.12) && (asym_h < 0.35);

	// if it's not horiozntal nor vertical, it is NOT an arrow
	if (!is_horizontal && !is_vertical) {
		return 0;
	}

	/* tip analysis
	we check the density of pixels at the tip; if the tip is significantly denser than the tail, we have found a directional indicator */
	// look at the first 20% of the shape
	auto get_avg_tip = [&](const std::vector<int>& proj, int len) {
		int sample_size = len / 5;
		int sum = 0;
		for (int i = 0; i < sample_size; i++) {
			sum += proj[i];
		}

		return (double)sum / sample_size;
	};

	// look at the last 20% of the shape
	auto get_avg_tip_end = [&](const std::vector<int>& proj, int len) {
		int sample_size = len / 5;
		int sum = 0;
		for (int i = len - sample_size; i < len; i++) {
			sum += proj[i];
		}

		return (double)sum / sample_size;
	};

	// final direction decision
	if (is_horizontal) {
		double tip_left = get_avg_tip(proj_h, w);
		double tip_right = get_avg_tip_end(proj_h, w);

		if (tip_left > tip_right * 1.1) { // more "mass" on the left => Left arrow
			return 2;
		}
		if (tip_right > tip_left * 1.1) { // more "mass" on the right => Right arrow
			return 3;
		}
	}
	else if (is_vertical) {
		double tip_up = get_avg_tip(proj_v, h);
		double tip_down = get_avg_tip_end(proj_v, h);

		if (tip_up > tip_down * 1.1) { // more "mass" on top => Up arrow
			return 1;
		}
	}

	return 0;
}

// test arrow detection method based on projections
void test_traffic_light_arrow_projections()
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

		Mat morphed_map_arrow = apply_morphological_operations_arrow(b_map);

		Mat final_candidates_arrow_map;
		labels valid_labels_arrow = process_candidate_regions(morphed_map_arrow, final_candidates_arrow_map);
		std::vector<TrafficLightCandidate> results_arrow = filter_by_shape(valid_labels_arrow);
		Mat colored_labels_arrow = color_labels(valid_labels_arrow);

		Mat final_validated_arrows = Mat::zeros(src.size(), CV_8UC3);

		for (const auto& candidate : results_arrow) {
			if (!candidate.is_circle) {
				Mat mask = get_obj_by_label(valid_labels_arrow.labels, candidate.id);
				int arrow_dir = classify_arrow_direction_projections(mask);

				if (arrow_dir != 0) {
					int color = classify_candidate(mask, H, S, I);
					if (color != 0) {
						for (int i = 0; i < colored_labels_arrow.rows; i++) {
							for (int j = 0; j < colored_labels_arrow.cols; j++) {
								if (mask.at<uchar>(i, j) == 255) {
									final_validated_arrows.at<Vec3b>(i, j) = colored_labels_arrow.at<Vec3b>(i, j);
								}
							}
						}

						std::string direction_name = (arrow_dir == 1) ? "UP" : (arrow_dir == 2) ? "LEFT" : "RIGHT";
						std::string color_name = get_color_name(color);
						std::string final_label = color_name + " ARROW " + direction_name;

						Point center((int)candidate.centroid.x, (int)candidate.centroid.y);

						drawMarker(output_display, center, Scalar(255, 0, 255), MARKER_CROSS, 15, 2);
						putText(output_display, final_label, Point(center.x + 15, center.y + 5),
							FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
					}
				}
			}
		}

		imshow("Colored labeled candidates", colored_labels_arrow);
		imshow("Final validated arrows", final_validated_arrows);
		imshow("Detected arrows", output_display);

		waitKey(0);
		destroyAllWindows();
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
		printf(" 1 - HSI conversion\n");
		printf(" 2 - Binary foreground map for traffic light detection\n");
		printf(" 3 - Apply morphological operations\n");
		printf(" 4 - Labeled candidate regions for traffic light detection\n");
		printf(" 5 - Candidate regions for traffic light detection (shape-based filtering)\n");
		printf(" 6 - Final traffic light detection\n");
		printf(" 7 - Test traffic light arrow detection\n");
		printf(" 8 - Test traffic light arrow detection (Projections Method)\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d",&op);
		switch (op)
		{
			case 1:
				test_HSI_conversion();
				break;
			case 2:
				test_foreground_map();
				break;
			case 3:
				test_morphological_operations();
				break;
			case 4:
				test_candidate_regions();
				break;
			case 5:
				test_traffic_light_shape_detection();
				break;
			case 6:
				test_traffic_light_final_detection();
				break;
			case 7:
				test_traffic_light_arrow_detection();
				break;
			case 8:
				test_traffic_light_arrow_projections();
				break;
			default:
				break;
		}
	}
	while (op!=0);
	return 0;
}