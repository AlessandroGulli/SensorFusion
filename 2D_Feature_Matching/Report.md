
# Midterm Project - Camera Based 2D Feature Tracking

## MP.1 Data Buffer Optimization
- Implementation of a 'Queue-like' data structure to manage image data efficiently.
- Whenever the overall size is 3 or above, the front element of the vector is removed.

## MP.2 Keypoint Detection
- Description of the implementation of keypoint detection detectors in `matching2D_student.cpp`.

## MP.3 Keypoint Removal
- Method for removing keypoints that are not within a specified bounding box. Otherwise, they are removed from the keypoint
vector.

## MP.4 Keypoint Descriptors
- Implementation details of keypoint descriptors in `matching2D_student.cpp`.

## MP.5 Descriptor Matching
- Details on the implementation of FLANN and k-nearest neighbour approaches in `matching2D_student.cpp`.

## MP.6 Descriptor Distance Ratio
- Information about the implementation in `matching2D_student.cpp`.

## MP.7, MP.8, MP.9 Performance Evaluation
- Instructions to look at a spreadsheet for a summary of data regarding performance evaluations.

## Top 3 Recommendations

### 1. SHI-TOMASI Detector (with BRIEF Descriptor)
- **Key Points Detected:**
  - Many keypoints are located on the rear of the vehicle, notably around the carplate area.
  - Key points are uniformly spread, aiding in accurate distance ratio and Time To Collision calculations.
- **Performance:**
  - Very fast.

### 2. BRISK Detector (with BRIEF Descriptor)
- **Key Points Detected:**
  - Detects the highest number of keypoints.
- **Performance:**
  - Very fast.

### 3. SIFT Detector (with BRIEF Descriptor)
- **Key Points Detected:**
  - Points are rather uniformly spread on the rear of the preceding vehicle.
- **Performance:**
  - Very fast.


