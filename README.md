# HydraSTAQ_Cpp
Porting of HydraSTAQ to C++

HydraSTAQ is an implementation of the following algorithms:

FOCAS Automatic Catalog Matching Algorithms, https://doi.org/10.1086/133667

Pattern Matching with Differential Voting and Median Transformation Derivation, https://doi.org/10.1007/1-4020-4179-9_146

HydraSTAQ is intended to be a lightweight stacker for aligning and taking the median of thousands of short exposures captured by a monochrome camera, but it can also be used for smaller datasets. 
The program takes 8 and 16 bit images as input in *.png or *.tif format. Full calibration with darks, flats and flat darks for all filters is done. If HydraSTAQ does not detect a given type of 
calibration files, it will proceed by setting the master dark and flatdark frames to zero and the master flatframe to unity.

Put your lights and calibration frames (or folders with such frames) as in the following structure and select the basepath from the GUI.

./
├── lights
│   ├── L (Luminance)
│   ├── R (Red)
│   ├── G (Green)
│   ├── B (Blue)
│   └── H (Ha)
│   └── O (OIII)
├── darks
│   ├── L (Luminance)
│   ├── R (Red)
│   ├── G (Green)
│   ├── B (Blue)
│   ├── H (Ha)
│   ├── O (OIII)
│   ├── RGB (same set of darks for R/G/B)
│   └── Narrowband (same set of darks for all narrowband filters)
├── flatdarks (use the same subdirectory structure as for darks)
└── flats
    ├── L (Luminance)
    ├── R (Red)
    ├── G (Green)
    ├── B (Blue)
    ├── H (Ha)
    └── O (OIII)
