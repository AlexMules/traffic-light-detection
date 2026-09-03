# 🚦 Traffic Light Detection and Classification

This project is an automatic image processing pipeline developed in C++ and OpenCV for detecting and classifying traffic lights in static images. Designed as a foundational component for Intelligent Transportation Systems (ITS) and autonomous driving assistance, this system identifies classic circular traffic lights as well as directional arrow signals.

The pipeline avoids deep learning for the core classification, relying instead on a robust, analytical approach using color space transformations, morphological filtering, and geometric spatial analysis.

<br>

## 📌 Table of Contents
1. [System Architecture & Pipeline](#-system-architecture--pipeline)
2. [Algorithmic Implementation Details](#-algorithmic-implementation-details)
3. [User Manual & Installation](#-user-manual--installation)
4. [Results & Limitations](#-results--limitations)
5. [Future Work](#-future-work)

<br>

## 🏗️ System Architecture & Pipeline

The recognition system operates in two main phases: **Detection** (isolating regions of interest) and **Classification** (validating shapes and colors). 

<div align="center">
<img alt="image" src="https://github.com/user-attachments/assets/211b3562-53dc-4daf-9c92-f814a5e46ea2" />
</div>

<br>

### 1. Pre-processing & Color Segmentation
*   **BGR to HSI Conversion:** To build invariance against environmental illumination (shadows, glaring sun, nighttime), the image is converted from the BGR color space to HSI (Hue, Saturation, Intensity). This decouples pure color information from brightness.
*   **Foreground Masking:** Strict mathematical thresholds are applied to the H, S, and I channels to isolate pixels matching active Red, Yellow, or Green lights.

> **📸 IMAGE PLACEHOLDER:** Insert an image showing the HSI conversion or color screening.
> *Recomandare:* Decupează colajul cu ferestrele "Hue (H)", "Saturation (S)" și "Intensity (I)" de la pagina 11.
> *Markdown:* `![HSI Conversion](docs/images/hsi_conversion.png)`

### 2. Morphological Filtering
*   **Noise Cleaning:** A sequence of *Closing* (dilation followed by erosion) and *Opening* (erosion followed by dilation) operations is applied. 
*   **Structural Merging:** By increasing the iteration count, the effective size of the structuring element is expanded. This successfully overcomes the mathematical idempotency of the operations, allowing the algorithm to physically "weld" separated LED clusters together while erasing isolated background noise.

### 3. Connected Component Labeling
*   **Two-Pass Labeling:** The binary map is scanned to group connected pixels into distinct objects.
*   **Spatial Filtering:** Objects with a pixel area below `MIN_AREA_THRESHOLD` (500 pixels for circles, 200 for arrows) are discarded as residual noise.

---

## 🧠 Algorithmic Implementation Details

### Circular Traffic Light Validation
To differentiate a standard traffic light from random background objects, the algorithm analyzes the geometry of each candidate region.
1.  **Compactness:** Computes the bounding box and aspect ratio. The object must have a width-to-height ratio between `0.5` and `1.8`.
2.  **Centroid Calculation:** Extracts the outer boundary points and computes the geometric centroid (center of mass).
3.  **Circularity Variance:** Calculates the Euclidean distance from the centroid to all outer border points to find $r_{min}$ and $r_{max}$. If the shape variance ratio $(r_{max} - r_{min}) / r_{max}$ is $\le 0.45$, the object is validated as a circle.

> **📸 IMAGE PLACEHOLDER:** Insert the shape validation visualization.
> *Recomandare:* Folosește imaginea de la pagina 17 unde apar ferestrele "Filtered valid shape(s)" alături de semaforul pe care sunt desenate cercurile verzi și crucile pe centru.
> *Markdown:* `![Shape Validation](docs/images/shape_validation.png)`

### Directional Arrow Detection (Two Methods)
Arrows are structurally fragile and prone to fragmentation. The pipeline applies a heavy *Closing* operation (5 iterations) specifically for arrow candidates to prevent the "tail" from detaching from the "tip". The direction is then evaluated using one of two implemented methods:

#### Method A: Spatial Distribution (Quadrants)
*   The bounding box is divided into 4 quadrants (Left, Right, Up, Down) relative to the geometric center.
*   The algorithm computes the *Fill Ratio* (must be between 0.35 and 0.72) and evaluates horizontal/vertical symmetry.
*   The direction is assigned to the quadrant containing the highest pixel density.

#### Method B: 1D Projections (Histogram Tip Analysis)
*   Instead of static quadrants, this method generates 1D histograms (horizontal and vertical projections) by summing the white pixels along the axes.
*   **Tip Analysis:** The algorithm isolates the first 20% and the last 20% of the shape's length. Because an arrow's tip concentrates significantly more mass than its tail, the algorithm compares these extremities. If the density of one end exceeds the other by a safety factor of `1.1x`, the direction is validated.

> **📸 IMAGE PLACEHOLDER:** Insert the arrow detection visualization.
> *Recomandare:* Folosește imaginea cu săgeata verde înspre sus ("GREEN ARROW UP") alături de forma sa extrasă (mov) de la pagina 21 sau 23.
> *Markdown:* `![Arrow Detection](docs/images/arrow_detection.png)`

### Color Classification
Once a shape is validated, the algorithm re-evaluates the pixel density strictly inside the validated mask using highly restrictive HSI thresholds to prevent color overlapping. A majority rule is applied: the dominant color must cover $> 50\%$ of the candidate's area to be accepted.

---

## 🚀 User Manual & Installation

### Prerequisites
*   **OS:** Windows
*   **IDE:** Microsoft Visual Studio (Desktop development with C++)
*   **Libraries:** OpenCV 4.x configured in the system environment.

### Build Instructions
1. Clone the repository:
   ```bash
   git clone [https://github.com/AlexMules/traffic-light-detection.git](https://github.com/AlexMules/traffic-light-detection.git)
