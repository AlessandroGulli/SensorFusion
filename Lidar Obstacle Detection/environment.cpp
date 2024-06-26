
// Create simple 3d highway enviroment using PCL
// for exploring self-driving car sensors
#include "sensors/lidar.h"
#include "render/render.h"
#include "processPointClouds.h"
// using templates for processPointClouds so also include .cpp to help linker
#include "processPointClouds.cpp"

std::vector<Car> initHighway(bool renderScene, pcl::visualization::PCLVisualizer::Ptr& viewer)
{

    Car egoCar( Vect3(0,0,0), Vect3(4,2,2), Color(0,1,0), "egoCar");
    Car car1( Vect3(15,0,0), Vect3(4,2,2), Color(0,0,1), "car1");
    Car car2( Vect3(8,-4,0), Vect3(4,2,2), Color(0,0,1), "car2");	
    Car car3( Vect3(-12,4,0), Vect3(4,2,2), Color(0,0,1), "car3");
  
    std::vector<Car> cars;
    cars.push_back(egoCar);
    cars.push_back(car1);
    cars.push_back(car2);
    cars.push_back(car3);

    if(renderScene)
    {
        renderHighway(viewer);
        egoCar.render(viewer);
        car1.render(viewer);
        car2.render(viewer);
        car3.render(viewer);
    }

    return cars;
}

void cityBlock(pcl::visualization::PCLVisualizer::Ptr& viewer, ProcessPointClouds<pcl::PointXYZI> pointProcessorI, pcl::PointCloud<pcl::PointXYZI>::Ptr inputCloud)
{
    // Hyperparameters 
    float voxelFilterResolution = 0.2;
    Eigen::Vector4f boxFilterMin = {-10, -6, -5, 1};//{-10, -6, -5, 1};
    Eigen::Vector4f boxFilterMax = {35, 7, 5, 1};
    int segmentationMaxIteration = 20;
    float segmentationDistThresh = 0.3;
    float clusteTolerance = 0.35;
    float clusterMinSize = 15;
    float clusterMaxSize = 250;

    inputCloud = pointProcessorI.FilterCloud(inputCloud, voxelFilterResolution, boxFilterMin, boxFilterMax);

    std::pair< pcl::PointCloud<pcl::PointXYZI>::Ptr, pcl::PointCloud<pcl::PointXYZI>::Ptr> segmentCloud = pointProcessorI.MySegmentPlane(inputCloud, segmentationMaxIteration, segmentationDistThresh); 
    
    std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> cloudClusters = pointProcessorI.MyClustering(segmentCloud.first, clusteTolerance, clusterMinSize, clusterMaxSize); 

    renderPointCloud(viewer, segmentCloud.second, "planeCloud", Color(0,1,0));

    int clusterId = 0;
    std::vector<Color> colors = {Color(1,0,0), Color(1,1,0), Color(0,0,1)};
    for(pcl::PointCloud<pcl::PointXYZI>::Ptr cluster:cloudClusters)
    {
        std::cout << "cluster size ";
        pointProcessorI.numPoints(cluster);
        renderPointCloud(viewer, cluster, "obsCloud"+std::to_string(clusterId), colors[clusterId%colors.size()]);       

        Box box = pointProcessorI.BoundingBox(cluster);
        renderBox(viewer, box, clusterId);

        ++clusterId;
    }

}  

void simpleHighway(pcl::visualization::PCLVisualizer::Ptr& viewer)
{
    // ----------------------------------------------------
    // -----Open 3D viewer and display simple highway -----
    // ----------------------------------------------------
    
    // RENDER OPTIONS
    bool renderScene = false;
    std::vector<Car> cars = initHighway(renderScene, viewer);
    
    
    Lidar* lidar = new Lidar(cars, 0);
    pcl::PointCloud<pcl::PointXYZ>::Ptr input_cloud = lidar->scan();
    //renderRays(viewer, lidar->position, input_cloud);
    //renderPointCloud(viewer, input_cloud, "PointCloud");

    //ProcessPointClouds<plc::PointXYZ> pointProcessor;
    ProcessPointClouds<pcl::PointXYZ> pointProcessor;
    std::pair< pcl::PointCloud<pcl::PointXYZ>::Ptr, pcl::PointCloud<pcl::PointXYZ>::Ptr> segmentCloud = pointProcessor.MySegmentPlane(input_cloud, 100, 0.2);
    renderPointCloud(viewer, segmentCloud.first, "obsCloud", Color(1,0,0));
    renderPointCloud(viewer, segmentCloud.second, "planeCloud", Color(0,1,0));

    std::vector<pcl::PointCloud<pcl::PointXYZ>::Ptr> cloudClusters = pointProcessor.MyClustering(segmentCloud.first, 1.0, 3, 30);

    int clusterId = 0;
    std::vector<Color> colors = {Color(1,0,0), Color(1,1,0), Color(0,0,1)};
    for(pcl::PointCloud<pcl::PointXYZ>::Ptr cluster:cloudClusters)
    {
        std::cout << "cluster size ";
        pointProcessor.numPoints(cluster);
        renderPointCloud(viewer, cluster, "obsCloud"+std::to_string(clusterId), colors[clusterId%colors.size()]);

        Box box = pointProcessor.BoundingBox(cluster);
        renderBox(viewer, box, clusterId);

        ++clusterId;
    }

    delete lidar;
}


//setAngle: SWITCH CAMERA ANGLE {XY, TopDown, Side, FPS}
void initCamera(CameraAngle setAngle, pcl::visualization::PCLVisualizer::Ptr& viewer)
{

    viewer->setBackgroundColor (0, 0, 0);
    
    // set camera position and angle
    viewer->initCameraParameters();
    // distance away in meters
    int distance = 16;
    
    switch(setAngle)
    {
        case XY : viewer->setCameraPosition(-distance, -distance, distance, 1, 1, 0); break;
        case TopDown : viewer->setCameraPosition(0, 0, distance, 1, 0, 1); break;
        case Side : viewer->setCameraPosition(0, -distance, 0, 0, 0, 1); break;
        case FPS : viewer->setCameraPosition(-10, 0, 0, 0, 0, 1);
    }

    if(setAngle!=FPS)
        viewer->addCoordinateSystem (1.0);
}


int main (int argc, char** argv)
{
    std::cout << "starting enviroment" << std::endl;

    pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
    CameraAngle setAngle = XY;
    initCamera(setAngle, viewer);
    //simpleHighway(viewer);
    //cityBlock(viewer);

    ProcessPointClouds<pcl::PointXYZI> pointProcessorI;
    std::vector<boost::filesystem::path> stream = pointProcessorI.streamPcd("../src/sensors/data/pcd/data_1");
    auto streamIterator = stream.begin();

    pcl::PointCloud<pcl::PointXYZI>::Ptr inputCloudI;

    while (!viewer->wasStopped ())
    {
        viewer->removeAllPointClouds();
        viewer->removeAllShapes();

        inputCloudI = pointProcessorI.loadPcd((*streamIterator).string());
        cityBlock(viewer, pointProcessorI, inputCloudI);

        streamIterator++;
        if(streamIterator == stream.end())
            streamIterator = stream.begin();
        
        viewer->spinOnce ();
    } 
}