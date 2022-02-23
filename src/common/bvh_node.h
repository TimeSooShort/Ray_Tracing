#pragma once

#include "hittable.h"

#include <memory>
#include <vector>
#include <queue>
#include <algorithm>

enum class aabb_axi
{
	AXI_X, AXI_Y, AXI_Z
};

using aabb_axi_type = std::underlying_type<aabb_axi>::type;

class bvh_node : public hittable
{
public:
	bvh_node() = default;
	bvh_node(const bvh_node & node) : left(node.left), right(node.right), box(node.box) {}

	bvh_node(
		std::vector<std::shared_ptr<hittable>>& src_objects, double time0, double time1);

	bvh_node& operator=(const bvh_node& node)
	{
		left = node.left;
		right = node.right;
		box = node.box;
	}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override;

	std::shared_ptr<hittable> left;
	std::shared_ptr<hittable> right;
	aabb box;
};

bool bvh_node::bounding_box(double _time0, double _time1, aabb& output_box) const
{
	output_box = box;
	return true;
}

bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	if (!box.hit(r, t_min, t_max))
	{
		return false;
	}

	bool hit_result = left->hit(r, t_min, t_max, rec);
	// if (!hit_result && right) 这样是错的因为排序是x/y/z轴随机排的，并不是按离用户远近排的
	if (right)
	{
		// hit_result || right->hit(r, t_min, t_max, rec)  这样写当hit_result为true是就不会再计算后面了
		hit_result = right->hit(r, t_min, t_max, rec);
	}
	return hit_result;
}


//bvh_node build_tree(const std::vector<std::shared_ptr<hittable>>& src_objects, double time0, double time1)
//{
//	bvh_node node;
//	if (src_objects.size() == 1)
//	{
//		if (!src_objects.at(0)->bounding_box(time0, time1, node.box))
//		{
//			std::cerr << "No bounding box in bvh_node constructor.\n";
//		}
//		node.left = src_objects.at(0);
//		return node;
//	}
//	
//	auto axis = random_int(0, 2);
//	auto comparator = (axis == 0) ? box_x_compare :
//		(axis == 1) ? box_y_compare : box_z_compare;
//
//	std::sort(src_objects.cbegin(), src_objects.cend(), comparator);
//
//	auto min = src_objects.size() / 2;
//	std::shared_ptr<hittable> obj = src_objects.at(min);
//
//	std::vector<std::shared_ptr<hittable>> lower_half(src_objects.cbegin(), src_objects.cbegin()+ min);
//	std::vector<std::shared_ptr<hittable>> higher_half(src_objects.cbegin() + min, src_objects.cend());
//	bvh_node node_left = build_tree(lower_half, time0, time1);
//	bvh_node node_right = build_tree(higher_half, time0, time1);
//	node.left = std::make_shared<bvh_node>(node_left);
//	node.right = std::make_shared<bvh_node>(node_right);
//
//	node.box = surrounding_box(node_left.box, node_right.box);
//	return node;
//}

bvh_node::bvh_node(
	std::vector<std::shared_ptr<hittable>>& src_objects, double time0, double time1)
{
	if (src_objects.size() == 1)
	{
		if (!src_objects.at(0)->bounding_box(time0, time1, box))
		{
			std::cerr << "No bounding box in bvh_node constructor.\n";
		}
		left = src_objects.at(0);
		return;
	}

	auto axis = random_int(0, 2);

	auto axi_enum = (axis == 0) ? aabb_axi::AXI_X :
		(axis == 1) ? aabb_axi::AXI_Y : aabb_axi::AXI_Z;

	std::sort(src_objects.begin(), src_objects.end(), [&axi_enum](std::shared_ptr<hittable>& a, std::shared_ptr<hittable>& b) {
		aabb a_box, b_box;
		if (!a->bounding_box(0, 0, a_box) || !b->bounding_box(0, 0, b_box))
		{
			std::cerr << "No bounding box in bvh_node constructor.\n";
		}
		aabb_axi_type i = static_cast<aabb_axi_type>(axi_enum);
		return a_box.min().e[i] < b_box.min().e[i];
		});

	auto min = src_objects.size() / 2;
	std::vector<std::shared_ptr<hittable>> lower_half(src_objects.cbegin(), src_objects.cbegin() + min);
	std::vector<std::shared_ptr<hittable>> higher_half(src_objects.cbegin() + min, src_objects.cend());
	std::shared_ptr<bvh_node> node_left = std::make_shared<bvh_node>(lower_half, time0, time1);
	std::shared_ptr<bvh_node> node_right = std::make_shared<bvh_node>(higher_half, time0, time1);

	left = node_left;
	right = node_right;
	box = surrounding_box(node_left->box, node_right->box);
}

//inline std::ostream& operator<<(std::ostream& out, std::shared_ptr<bvh_node> bvh_node_tmp)
//{
//	int level = 1;
//	std::queue<std::shared_ptr<bvh_node>> q;
//	q.push(bvh_node_tmp);
//	std::shared_ptr<bvh_node> last = bvh_node_tmp;
//	std::shared_ptr<bvh_node> nLast;
//	out << "LEVEL " << level++ << ": ";
//	while (!q.empty())
//	{
//		bvh_node_tmp = q.front();
//		out << bvh_node_tmp->box.min() << " ; ";
//		if (bvh_node_tmp->left)
//		{
//			q.push(bvh_node_tmp->left);
//			nLast = bvh_node_tmp->left;
//		}
//		if (bvh_node_tmp->right)
//		{
//			q.push(bvh_node_tmp->right);
//			nLast = bvh_node_tmp->right;
//		}
//		if (node == last && !q.empty())
//		{
//			out << "\nLEVEL " << level++ << ": ";
//			last = nLast;
//		}
//	}
//}

