// PCL lib Functions for processing point clouds 

#include "processPointClouds.h"
#include <unordered_set>

//constructor:
template<typename PointT>
ProcessPointClouds<PointT>::ProcessPointClouds() {}


//de-constructor:
template<typename PointT>
ProcessPointClouds<PointT>::~ProcessPointClouds() {}


template<typename PointT>
void ProcessPointClouds<PointT>::numPoints(typename pcl::PointCloud<PointT>::Ptr cloud)
{
    std::cout << cloud->points.size() << std::endl;
}


template<typename PointT>
typename pcl::PointCloud<PointT>::Ptr ProcessPointClouds<PointT>::FilterCloud(typename pcl::PointCloud<PointT>::Ptr cloud, float filterRes, Eigen::Vector4f minPoint, Eigen::Vector4f maxPoint)
{

    // Time filter process
    //auto startTime = std::chrono::steady_clock::now();

    // voxel grid point reduction and region based filtering
    pcl::VoxelGrid<PointT> vg;
    typename pcl::PointCloud<PointT>::Ptr cloudFiltered(new pcl::PointCloud<PointT>);
    vg.setInputCloud(cloud);
    vg.setLeafSize(filterRes, filterRes, filterRes);
    vg.filter(*cloudFiltered); 

    typename pcl::PointCloud<PointT>::Ptr cloudRegion (new pcl::PointCloud<PointT>);
    pcl::CropBox<PointT> region(true);
    region.setMin(minPoint);
    region.setMax(maxPoint);
    region.setInputCloud(cloudFiltered);
    region.filter(*cloudRegion);

    region.setMin(Eigen::Vector4f(-3.0, -3.0, -1, 1));
    region.setMax(Eigen::Vector4f(3, 3, 1, 1));
    region.setInputCloud(cloudRegion);
    region.setNegative(true);
    region.filter(*cloudRegion);

    //std::vector<int> indices;

    //pcl::CropBox<PointT> roof(true);
    //roof.setMin(Eigen::Vector4f(-1.5, -1.7, -1, 1));
    //roof.setMax(Eigen::Vector4f(2.6, 1.7, -.4, 1));
    //roof.setInputCloud(cloudRegion);
    //roof.filter(indices); 

    //pcl::PointIndices::Ptr inliers {new pcl::PointIndices};

    //for(int point: indices)
    //    inliers->indices.push_back(point);

    //pcl::ExtractIndices<PointT> extract;
    //extract.setInputCloud(cloudRegion);
    //extract.setIndices(inliers);
    //extract.setNegative(true);
    //extract.filter(*cloudRegion);


    //auto endTime = std::chrono::steady_clock::now();
    //auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    //std::cout << "filtering took " << elapsedTime.count() << " milliseconds" << std::endl;

    return cloudRegion;

}

template<typename PointT>
std::unordered_set<int> ProcessPointClouds<PointT>::MyRansac3D(const typename pcl::PointCloud<PointT>::Ptr& cloud, int maxIterations, float distanceTol)
{
	//auto startTime = std::chrono::steady_clock::now();

	std::unordered_set<int> inliersResult;
	srand(time(NULL));
	
	// For max iterations 
	while(maxIterations --> 0)
	{
		// Randomly sample subset and fit line
		std::unordered_set<int> inliers;
		while(inliers.size() < 3)
			inliers.insert(rand()%(cloud->points.size()));

		float x1, y1, z1, x2, y2, z2, x3, y3, z3;
		auto itr = inliers.begin();
		
		x1 = cloud->points[*itr].x;
		y1 = cloud->points[*itr].y;
		z1 = cloud->points[*itr].z;

		itr++;

		x2 = cloud->points[*itr].x;
		y2 = cloud->points[*itr].y;
		z2 = cloud->points[*itr].z;

		itr++;

		x3 = cloud->points[*itr].x;
		y3 = cloud->points[*itr].y;
		z3 = cloud->points[*itr].z;

		float a = (y2 - y1)*(z3- z1) - (z2 - z1)*(y3 - y1);
		float b = (z2 - z1)*(x3 - x1) - (x2 - x1)*(z3 - z1); 
		float c = (x2 - x1)*(y3 - y1) - (y2* - y1)*(x3 - x1);
		float d = -(a*x1 + b*y1 + c*z1);

		for(int index = 0; index < cloud->points.size(); index++)
		{
			if(inliers.count(index)>0)
				continue;
			
			PointT point = cloud->points[index];

			float x3 = point.x;
			float y3 = point.y;
			float z3 = point.z;

			float D = fabs(a*x3 + b*y3 + c*z3 + d)/sqrt(a*a + b*b + c*c);

			if(D <= distanceTol)
				inliers.insert(index);
		}

		if(inliers.size() > inliersResult.size())
			inliersResult = inliers;
	}

	//const auto endTime = std::chrono::steady_clock::now();
	//const auto elapsedTime = std::chrono::duration_cast<std::chrono::millisenconds>(endTime - startTime);

	//std::cout << "Ransac took " << elapsedTime.count() << " milliseconds" << std::endl;

	return inliersResult;

}


template<typename PointT>
void ProcessPointClouds<PointT>::MyClusterHelper(int indice, std::vector<std::vector<float>> points, std::vector<int>& cluster, std::vector<bool>& processed, KdTree* tree,  float distanceTol)
{
	processed[indice] = true;
	cluster.push_back(indice);

	std::vector<int>nearest = tree->search(points[indice], distanceTol);

	for(int id: nearest)
	{
		if(!processed[id])
			MyClusterHelper(id, points, cluster, processed, tree, distanceTol);
	}

}

template<typename PointT>
std::vector<std::vector<int>> ProcessPointClouds<PointT>::MyEuclideanCluster(const std::vector<std::vector<float>>& points, KdTree* tree, float distanceTol, float minSize, float maxSize)
{

	std::vector<std::vector<int>> clusters;
	std::vector<bool> processed(points.size(), false);

	int i = 0;

	while(i  < points.size())
	{
		if (processed[i])
		{
			i++;
			continue;
		}

		std::vector<int> cluster;
		MyClusterHelper(i, points, cluster, processed, tree, distanceTol);
        if (cluster.size() >= minSize && cluster.size() <= maxSize)
		    clusters.push_back(cluster);
		i++;
	}

 
	return clusters;

}


template<typename PointT>
std::pair<typename pcl::PointCloud<PointT>::Ptr, typename pcl::PointCloud<PointT>::Ptr> ProcessPointClouds<PointT>::SeparateClouds(pcl::PointIndices::Ptr inliers, typename pcl::PointCloud<PointT>::Ptr cloud) 
{
  // TODO: Create two new point clouds, one cloud with obstacles and other with segmented plane
    typename pcl::PointCloud<PointT>::Ptr obsCloud (new pcl::PointCloud<PointT>());
    typename pcl::PointCloud<PointT>::Ptr planeCloud (new pcl::PointCloud<PointT>());

    for(int index:inliers->indices)
        planeCloud->points.push_back(cloud->points[index]);

    
    pcl::ExtractIndices<PointT> extract;

    extract.setInputCloud(cloud);
    extract.setIndices(inliers);
    extract.setNegative(true);
    extract.filter(*obsCloud);
  
    std::pair<typename pcl::PointCloud<PointT>::Ptr, typename pcl::PointCloud<PointT>::Ptr> segResult(obsCloud, planeCloud);
    return segResult;
}

// My Implementation
template<typename PointT>
std::pair<typename pcl::PointCloud<PointT>::Ptr, typename pcl::PointCloud<PointT>::Ptr> ProcessPointClouds<PointT>::MySegmentPlane(const typename pcl::PointCloud<PointT>::Ptr& cloud, int maxIterations, float distanceThreshold)
{
    std::unordered_set<int> inliers = MyRansac3D(cloud, maxIterations, distanceThreshold);

	typename pcl::PointCloud<PointT>::Ptr obsCloud(new pcl::PointCloud<PointT>());
	typename pcl::PointCloud<PointT>::Ptr planeCloud(new pcl::PointCloud<PointT>());

	for(int index = 0; index < cloud->points.size(); index++)
	{
		PointT point = cloud->points[index];
		if(inliers.count(index))
			obsCloud->points.push_back(point);
		else
			planeCloud->points.push_back(point);
	}

    
    if (obsCloud->points.size() == 0)
    {
        exit(-1);
    }
      
    std::pair<typename pcl::PointCloud<PointT>::Ptr, typename pcl::PointCloud<PointT>::Ptr> segResult(planeCloud, obsCloud);
    return segResult;
}


template<typename PointT>
std::pair<typename pcl::PointCloud<PointT>::Ptr, typename pcl::PointCloud<PointT>::Ptr> ProcessPointClouds<PointT>::SegmentPlane(typename pcl::PointCloud<PointT>::Ptr cloud, int maxIterations, float distanceThreshold)
{
    // Time segmentation process
    auto startTime = std::chrono::steady_clock::now();
	
    // TODO:: Fill in this function to find inliers for the cloud.
    pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients ());
    pcl::PointIndices::Ptr inliers (new pcl::PointIndices ());
    // Create the segmentation object
    pcl::SACSegmentation<PointT> seg;
    // Optional
    seg.setOptimizeCoefficients (true);
    // Mandatory
    seg.setModelType (pcl::SACMODEL_PLANE);
    seg.setMethodType (pcl::SAC_RANSAC);
    seg.setMaxIterations (maxIterations);
    seg.setDistanceThreshold (distanceThreshold);
  
    seg.setInputCloud (cloud);
    seg.segment (*inliers, *coefficients);
    if (inliers->indices.size () == 0)
    {
      std::cerr << "Could not estimate a planar model for the given dataset." << std::endl;
    }
  

    auto endTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "plane segmentation took " << elapsedTime.count() << " milliseconds" << std::endl;

    std::pair<typename pcl::PointCloud<PointT>::Ptr, typename pcl::PointCloud<PointT>::Ptr> segResult = SeparateClouds(inliers,cloud);
    return segResult;
}

// My Implementation
template<typename PointT>
std::vector<typename pcl::PointCloud<PointT>::Ptr> ProcessPointClouds<PointT>::MyClustering(const typename pcl::PointCloud<PointT>::Ptr& cloud, float clusterTolerance, float minSize, float maxSize)
{

    std::vector<typename pcl::PointCloud<PointT>::Ptr> clusters;

    std::vector<std::vector<float>> points(cloud->points.size());

    KdTree* tree = new KdTree;
  
    for (int i=0; i< cloud->points.size(); i++) 
    {
        std::vector<float> point = {cloud->points[i].x, cloud->points[i].y, cloud->points[i].z};
    	tree->insert(point,i); 
        points[i] = point;
  	}
          
    std::vector<std::vector<int>> clustersIndices = MyEuclideanCluster(points, tree, clusterTolerance, minSize, maxSize);

  	for(std::vector<int> cluster : clustersIndices)
  	{
  		typename pcl::PointCloud<PointT>::Ptr cloudCluster(new pcl::PointCloud<PointT>());
  		for(int index: cluster)
  			cloudCluster->points.push_back(cloud->points[index]);
  		cloudCluster->width = cloudCluster->points.size();
        cloudCluster->height = 1;
        cloudCluster->is_dense = true;

        clusters.push_back(cloudCluster);
  	}

    return clusters;
}

template<typename PointT>
std::vector<typename pcl::PointCloud<PointT>::Ptr> ProcessPointClouds<PointT>::Clustering(typename pcl::PointCloud<PointT>::Ptr cloud, float clusterTolerance, int minSize, int maxSize)
{

    // Time clustering process
    auto startTime = std::chrono::steady_clock::now();

    std::vector<typename pcl::PointCloud<PointT>::Ptr> clusters;

    // TODO:: Fill in the function to perform euclidean clustering to group detected obstacles
    typename pcl::search::KdTree<PointT>::Ptr tree (new pcl::search::KdTree<PointT>);
    tree-> setInputCloud(cloud);

    std::vector<pcl::PointIndices> cluster_indices;
    pcl::EuclideanClusterExtraction<PointT> ec;
    ec.setClusterTolerance(clusterTolerance);
    ec.setMinClusterSize(minSize);
    ec.setMaxClusterSize(maxSize);
    ec.setSearchMethod(tree);
    ec.setInputCloud(cloud);
    ec.extract(cluster_indices);

    for(pcl::PointIndices getIndices: cluster_indices)
    {
        typename pcl::PointCloud<PointT>::Ptr cloudCluster (new pcl::PointCloud<PointT>);

        for(int index:getIndices.indices)
            cloudCluster->points.push_back(cloud->points[index]);

        cloudCluster->width = cloudCluster->points.size();
        cloudCluster->height = 1;
        cloudCluster->is_dense = true;

        clusters.push_back(cloudCluster);

    }

    auto endTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "clustering took " << elapsedTime.count() << " milliseconds and found " << clusters.size() << " clusters" << std::endl;

    return clusters;
}


template<typename PointT>
Box ProcessPointClouds<PointT>::BoundingBox(typename pcl::PointCloud<PointT>::Ptr cluster)
{

    // Find bounding box for one of the clusters
    PointT minPoint, maxPoint;
    pcl::getMinMax3D(*cluster, minPoint, maxPoint);

    Box box;
    box.x_min = minPoint.x;
    box.y_min = minPoint.y;
    box.z_min = minPoint.z;
    box.x_max = maxPoint.x;
    box.y_max = maxPoint.y;
    box.z_max = maxPoint.z;

    return box;
}


template<typename PointT>
void ProcessPointClouds<PointT>::savePcd(typename pcl::PointCloud<PointT>::Ptr cloud, std::string file)
{
    pcl::io::savePCDFileASCII (file, *cloud);
    std::cerr << "Saved " << cloud->points.size () << " data points to "+file << std::endl;
}


template<typename PointT>
typename pcl::PointCloud<PointT>::Ptr ProcessPointClouds<PointT>::loadPcd(std::string file)
{

    typename pcl::PointCloud<PointT>::Ptr cloud (new pcl::PointCloud<PointT>);

    if (pcl::io::loadPCDFile<PointT> (file, *cloud) == -1) //* load the file
    {
        PCL_ERROR ("Couldn't read file \n");
    }
    std::cerr << "Loaded " << cloud->points.size () << " data points from "+file << std::endl;

    return cloud;
}


template<typename PointT>
std::vector<boost::filesystem::path> ProcessPointClouds<PointT>::streamPcd(std::string dataPath)
{

    std::vector<boost::filesystem::path> paths(boost::filesystem::directory_iterator{dataPath}, boost::filesystem::directory_iterator{});

    // sort files in accending order so playback is chronological
    sort(paths.begin(), paths.end());

    return paths;

}