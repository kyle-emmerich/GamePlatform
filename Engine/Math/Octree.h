#pragma once

template<typename T>
class Octree {
public:
    Octree(const Math::Vector3<double>& center, double halfSize)
        : root(new OctreeNode{ center, halfSize }) {}

    ~Octree() {
        delete root;
    }

    void Insert(const T& item, const Math::Vector3<double>& position) {
        insert(root, item, position);
    }

    std::vector<T> QueryRange(const Math::Vector3<double>& center, double halfSize) {
        std::vector<T> results;
        root->queryRange(root, center, halfSize, results);
        return results;
    }
private:
    struct OctreeNode {
        Math::Vector3<double> center;
        double halfSize;
        std::vector<T> items;
        OctreeNode* children[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

        ~OctreeNode() {
            for (int i = 0; i < 8; ++i) {
                delete children[i];
            }
        }

        bool isLeaf() const {
            for (int i = 0; i < 8; ++i) {
                if (children[i] != nullptr) {
                    return false;
                }
            }
            return true;
        }

        void subdivide() {
            for (int i = 0; i < 8; ++i) {
                children[i] = new OctreeNode();
            }
        }
    };

    OctreeNode* root;

    void insert(OctreeNode* node, const T& item, const Math::Vector3<double>& position) {
        if (node->halfSize <= 1.0) {
            node->items.push_back(item);
            return;
        }

        if (node->isLeaf()) {
            node->subdivide();
        }

        int index = 0;
        if (position.X >= node->center.X) index |= 4;
        if (position.Y >= node->center.Y) index |= 2;
        if (position.Z >= node->center.Z) index |= 1;

        Math::Vector3<double> newCenter = node->center;
        double offset = node->halfSize / 2.0;
        newCenter.X += (index & 4) ? offset : -offset;
        newCenter.Y += (index & 2) ? offset : -offset;
        newCenter.Z += (index & 1) ? offset : -offset;

        insert(node->children[index], item, position);
    }

    void queryRange(OctreeNode* node, const Math::Vector3<double>& center, double halfSize, std::vector<T>& results) {
        if (!intersects(node->center, node->halfSize, center, halfSize)) {
            return;
        }

        for (const T& item : node->items) {
            results.push_back(item);
        }

        if (node->isLeaf()) {
            return;
        }

        for (int i = 0; i < 8; ++i) {
            queryRange(node->children[i], center, halfSize, results);
        }
    }
};