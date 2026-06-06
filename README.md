# Vision-Computador-JNI
# Computer Vision Practice 2: Real-Time JNI Processing & Medical Imaging

**Author:** Juan Alberto Guillén Albarracín  and Ariel David Solano Rivera
**Program:** Computer Science Engineering  
**Institution:** Universidad Politécnica Salesiana  

## About The Project

This repository contains the implementation of a comprehensive Computer Vision project divided into three main stages. It showcases advanced image and video processing techniques using **OpenCV**, bridging real-time mobile processing in Android via **C++ (JNI)** and medical image analysis using **Python (Jupyter Notebooks)**.

### Part 1-A: Channel Manipulation & Alpha Blending (Android / C++)
This module focuses on the foundational manipulation of image matrices in a native Android environment. 
* **Struss Effect (Channel Separation):** Real-time extraction and independent manipulation of RGB/BGR color channels directly from the device's camera stream.
* **Alpha Blending:** Implementation of a weighted sum arithmetic operation (`cv::addWeighted`) to create smooth, dynamic transitions between the live camera feed and a static background image, controlled via the Android UI.

### Part 1-B: Background Segmentation & Spatial Filtering (Android / C++)
This section implements a robust, real-time video processing pipeline to isolate subjects and handle image noise under hardware constraints.
* **Chroma Key Segmentation:** Conversion of camera frames to the HSV color space to generate accurate binary masks (`cv::inRange`), successfully replacing solid backgrounds (Blue/Green screens) with custom images.
* **Synthetic Noise Injection:** Stochastic generation of Gaussian noise applied exclusively to the foreground subject, simulating sensor degradation.
* **Spatial Filtering & Performance:** Application of Median Blur filters with dynamically adjustable kernel sizes (via UI sliders) to attenuate the injected noise. It includes a custom, native FPS (Frames Per Second) counter rendered on the image matrix to analyze the computational cost of large convolution kernels on mobile hardware.

### Part 2: Medical Imaging & Morphological Operations (Python / Jupyter)
The final stage shifts to a Python-based environment to solve clinical image analysis problems.
* **Noise Reduction in Medical Images:** Application of specific spatial filters (e.g., Gaussian, Median) tailored to clean noise from medical scans without destroying critical clinical details.
* **Morphological Transformations:** Strategic use of morphological operations (Erosion, Dilation, Opening, and Closing) with carefully selected kernel sizes to highlight, clean, and segment anatomical structures of interest.

## Technologies Used
* **Languages:** C++, Java, Python
* **Frameworks/Libraries:** OpenCV, Android NDK (JNI), NumPy, Matplotlib
* **Tools:** Android Studio, CMake, Jupyter Notebook
