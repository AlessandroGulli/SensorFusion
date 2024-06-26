
#include "render/render.h"


// Structure to represent node of kd tree (3D)
struct Node
{
	std::vector<float> point;
	int id;
	Node* left;
	Node* right;

	Node(std::vector<float> arr, int setId)
	:	point(arr), id(setId), left(NULL), right(NULL)
	{}
};

struct KdTree
{
	Node* root;

	KdTree()
	: root(NULL)
	{}

	void insertHelper(Node** node, uint depth, std::vector<float>point, int id)
	{
		if(*node == NULL)
		{
			*node = new Node(point, id);
		}
		else
		{
			uint cd = depth%3;

			if (point[cd] < ((*node)->point[cd]))
				insertHelper(&((*node)->left), depth+1, point, id);
			else
				insertHelper(&((*node)->right), depth+1, point, id);
		}
	}

	void insert(std::vector<float> point, int id)
	{ 
		insertHelper(&root, 0, point, id);
	}

	float CalDistToTarget(std::vector<float> point, std::vector<float> target)
	{
		uint dim = point.size();
		float sum_squared = 0;
		for (size_t i = 0; i < dim; i++)
		{
			sum_squared += (point[i] - target[i])*(point[i] - target[i]);
		}
		return sqrt(sum_squared);
	}
	
	void searchHelper(const std::vector<float>& target, Node* &node, int depth, float distanceTol, std::vector<int>& ids)
	{
		if(node!= NULL)
		{
			if( (node->point[0] >= (target[0] - distanceTol) ) && (node->point[0] <= (target[0] + distanceTol) ) && ( node->point[1]>= (target[1] -distanceTol) ) && ( node->point[1]<= (target[1] + distanceTol)) && ( node->point[2]>= (target[2] -distanceTol) ) && ( node->point[2]<= (target[2] + distanceTol)))
			{
				float distance = CalDistToTarget(node->point, target);
				if (distance <= distanceTol)
				{
					ids.push_back(node->id);
				}
			}

			if((target[depth%3]- distanceTol) < node->point[depth%3])
				searchHelper(target, node->left, depth+1,distanceTol, ids);
			if ((target[depth%3]+ distanceTol) > node->point[depth%3])
				searchHelper(target, node->right, depth+1,distanceTol, ids);

		}
	}

	// return a list of point ids in the tree that are within distance of target
	std::vector<int> search(const std::vector<float>& target, float distanceTol)
	{
		std::vector<int> ids;

		searchHelper(target, root, 0, distanceTol, ids);

		return ids;
	}

};




