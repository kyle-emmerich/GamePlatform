#pragma once

#include "Math/Vector3.h"
#include "Math/Plane.h"
#include "Math/AABB.h"
#include "Math/Sphere.h"
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <map>
#include <set>

template<typename T>
class Simplex {
public:
	union {
		struct {
			Vector3<T> A;
			Vector3<T> B;
			Vector3<T> C;
			Vector3<T> D;
		};
		Vector3<T> Vertices[4];
	};

	Simplex() : A(Vector3<T>::Zero), B(Vector3<T>::Zero), C(Vector3<T>::Zero), D(Vector3<T>::Zero) {}
	Simplex(Vector3<T> a, Vector3<T> b, Vector3<T> c, Vector3<T> d) : A(a), B(b), C(c), D(d) {}

	static T SignedVolume(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& c, const Vector3<T>& d) {
		return (b - a).Dot((c - a).Cross(d - a)) / static_cast<T>(6.0);
	}

	T Volume() const {
		return std::abs(SignedVolume(A, B, C, D));
	}

	T SurfaceArea() const {
		// Tetrahedron surface area: sum of the areas of faces ABC, ACD, ADB, BCD
		const Vector3<T> AB = B - A;
		const Vector3<T> AC = C - A;
		const Vector3<T> AD = D - A;
		const Vector3<T> BC = C - B;
		const Vector3<T> BD = D - B;

		const T areaABC = static_cast<T>(0.5) * AB.Cross(AC).Magnitude();
		const T areaACD = static_cast<T>(0.5) * AC.Cross(AD).Magnitude();
		const T areaADB = static_cast<T>(0.5) * AD.Cross(AB).Magnitude();
		const T areaBCD = static_cast<T>(0.5) * BC.Cross(BD).Magnitude();

		return areaABC + areaACD + areaADB + areaBCD;
	}

	bool ContainsPoint(const Vector3<T>& point, T eps = static_cast<T>(1e-6)) const {
		// Inside if signed volumes of the 4 tetrahedra formed with the point have the same sign (or zero)
		const T v1 = (B - point).Dot((C - point).Cross(D - point)); // 6 * vol(point, B, C, D)
		const T v2 = (A - point).Dot((D - point).Cross(C - point)); // 6 * vol(A, point, C, D)
		const T v3 = (A - point).Dot((B - point).Cross(D - point)); // 6 * vol(A, B, point, D)
		const T v4 = (A - point).Dot((C - point).Cross(B - point)); // 6 * vol(A, B, C, point)

		bool hasNeg = (v1 < -eps) || (v2 < -eps) || (v3 < -eps) || (v4 < -eps);
		bool hasPos = (v1 > eps) || (v2 > eps) || (v3 > eps) || (v4 > eps);
		return !(hasNeg && hasPos);
	}

	Sphere<T> BoundingSphere() {
		return Sphere<T>::FromPoints({ A, B, C, D });
	}

	bool Intersects(const Simplex<T>& other, T eps = static_cast<T>(1e-6)) const {
		
	}
};

template<typename T>
class ConvexHull {
public:
	std::vector<Simplex<T>> simplices;

	ConvexHull() {}
	ConvexHull(const std::vector<Vector3<T>>& inPoints) {
		BuildQuickHull(inPoints);
	}

	bool IsPointInside(const Vector3<T>& point, T eps = static_cast<T>(1e-6)) const {
		for (const auto& simplex : simplices) {
			if (simplex.ContainsPoint(point, eps)) {
				return true;
			}
		}
		return false;
	}
	bool IntersectsHull(const ConvexHull<T>& other, T eps = static_cast<T>(1e-6)) const {
		
	}

private:
	struct Face {
		int a, b, c; // indices into points
		Vector3<T> normal;
		T offset;

		Face() : a(-1), b(-1), c(-1), normal(Vector3<T>::Zero), offset(0) {}
		Face(int ia, int ib, int ic) : a(ia), b(ib), c(ic), normal(Vector3<T>::Zero), offset(0) {}

		void ComputePlane(const std::vector<Vector3<T>>& points, T eps) const {
			const Vector3<T>& A = points[f.a];
			const Vector3<T>& B = points[f.b];
			const Vector3<T>& C = points[f.c];
			Vector3<T> n = (B - A).Cross(C - A);
			T len = n.Magnitude();
			if (len <= eps) {
				f.normal = Vector3<T>::Zero;
				f.offset = static_cast<T>(0);
				return;
			}
			normal = n / len;
			offset = normal.Dot(A);
		}

		void OrientOutward(Face& f, const std::vector<Vector3<T>>& pts, const Vector3<T>& insidePoint, T eps) const {
			f.ComputePlane(pts, eps);
			if (f.normal == Vector3<T>::Zero) return;

			const T side = f.DistanceToPoint(insidePoint);
			// If the inside point is on the positive side of the face, normal points inward -> flip
			if (side > static_cast<T>(0)) {
				std::swap(f.b, f.c);
				f.ComputePlane(pts, eps);
			}
		}

		T DistanceToPoint(const Vector3<T>& point) const {
			return normal.Dot(point) - offset;
		}
	};

	

	void AddFace(std::vector<Face>& faces, int a, int b, int c, const std::vector<Vector3<T>>& pts, const Vector3<T>& insidePoint, T eps) const {
		Face f(a, b, c);
		f.OrientOutward(pts, insidePoint, eps);
		// Only add valid non-degenerate face
		if (f.normal != Vector3<T>::Zero) {
			faces.push_back(f);
		}
	}

	bool BuildInitialTetrahedron(const std::vector<Vector3<T>>& pts, int& i0, int& i1, int& i2, int& i3, T eps) const {
		const int n = static_cast<int>(pts.size());
		// i0: min x, i1: max x
		i0 = 0; i1 = 0;
		for (int i = 1; i < n; ++i) {
			if (pts[i].x < pts[i0].x) i0 = i;
			if (pts[i].x > pts[i1].x) i1 = i;
		}
		if (i0 == i1) return false;

		// i2: farthest from line i0-i1
		const Vector3<T> A = pts[i0];
		const Vector3<T> B = pts[i1];
		const Vector3<T> AB = B - A;
		const T abLen = AB.Magnitude();
		if (abLen <= eps) return false;

		i2 = -1;
		T bestDist = -static_cast<T>(1);
		for (int i = 0; i < n; ++i) {
			if (i == i0 || i == i1) continue;
			T dist = (pts[i] - A).Cross(AB).Magnitude() / abLen;
			if (dist > bestDist) {
				bestDist = dist;
				i2 = i;
			}
		}
		if (i2 < 0 || bestDist <= eps) return false;

		// i3: farthest from plane (i0, i1, i2)
		Vector3<T> N = (pts[i1] - pts[i0]).Cross(pts[i2] - pts[i0]);
		T nLen = N.Magnitude();
		if (nLen <= eps) return false;
		Vector3<T> unitN = N / nLen;
		T d = unitN.Dot(pts[i0]);

		i3 = -1;
		T bestAbs = -static_cast<T>(1);
		for (int i = 0; i < n; ++i) {
			if (i == i0 || i == i1 || i == i2) continue;
			const T dist = unitN.Dot(pts[i]) - d;
			if (std::abs(dist) > bestAbs) {
				bestAbs = std::abs(dist);
				i3 = i;
			}
		}
		if (i3 < 0 || bestAbs <= eps) return false;

		return true;
	}

	void BuildQuickHull(const std::vector<Vector3<T>>& pts) {
		const T eps = static_cast<T>(1e-6);
		simplices.clear();
		if (pts.size() < 4) return;

		// Step 1: Build initial tetrahedron
		int i0, i1, i2, i3;
		if (!BuildInitialTetrahedron(pts, i0, i1, i2, i3, eps)) {
			return; // Degenerate input (colinear/coplanar)
		}

		// Inside reference point: centroid of initial tetrahedron
		Vector3<T> center = (pts[i0] + pts[i1] + pts[i2] + pts[i3]) / static_cast<T>(4.0);

		// Faces of the initial hull (oriented outward)
		std::vector<Face> faces;
		faces.reserve(64);
		addFace(faces, i0, i1, i2, pts, center, eps);
		addFace(faces, i0, i2, i3, pts, center, eps);
		addFace(faces, i0, i3, i1, pts, center, eps);
		addFace(faces, i1, i3, i2, pts, center, eps);

		// Remaining points (candidates)
		std::vector<int> candidates;
		candidates.reserve(pts.size());
		for (int i = 0; i < static_cast<int>(pts.size()); ++i) {
			if (i == i0 || i == i1 || i == i2 || i == i3) continue;
			candidates.push_back(i);
		}

		// QuickHull loop
		std::set<int> processed;
		while (true) {
			int bestFace = -1;
			int bestPoint = -1;
			T bestDist = eps;

			// Find the point farthest outside any face
			for (int pi : candidates) {
				if (processed.count(pi)) continue;
				for (int fi = 0; fi < static_cast<int>(faces.size()); ++fi) {
					const Face& f = faces[fi];
					const T dist = pointPlaneDistance(pts[pi], f.normal, f.offset);
					if (dist > bestDist) {
						bestDist = dist;
						bestFace = fi;
						bestPoint = pi;
					}
				}
			}

			if (bestFace == -1) break; // No point lies outside -> hull complete

			const int pIdx = bestPoint;

			// Determine all faces visible from pIdx
			std::vector<int> visible;
			visible.reserve(faces.size());
			for (int fi = 0; fi < static_cast<int>(faces.size()); ++fi) {
				const Face& f = faces[fi];
				if (pointPlaneDistance(pts[pIdx], f.normal, f.offset) > eps) {
					visible.push_back(fi);
				}
			}

			// Build map of undirected edge -> count within visible set to find horizon
			struct EdgeKey { int u, v; };
			auto makeUndirected = [](int u, int v) -> std::pair<int, int> {
				return (u < v) ? std::make_pair(u, v) : std::make_pair(v, u);
			};
			std::map<std::pair<int, int>, int> edgeCount;
			std::vector<EdgeKey> orientedEdges; // oriented as in visible faces
			orientedEdges.reserve(visible.size() * 3);

			for (int fi : visible) {
				const Face& f = faces[fi];
				int va[3] = { f.a, f.b, f.c };
				for (int e = 0; e < 3; ++e) {
					const int u = va[e];
					const int v = va[(e + 1) % 3];
					edgeCount[makeUndirected(u, v)]++;
					orientedEdges.push_back({ u, v });
				}
			}

			// Horizon edges are those appearing only once in visible faces
			std::vector<EdgeKey> horizon;
			horizon.reserve(orientedEdges.size());
			for (const auto& e : orientedEdges) {
				if (edgeCount[makeUndirected(e.u, e.v)] == 1) {
					horizon.push_back(e);
				}
			}

			// Remove visible faces
			std::vector<Face> newFaces;
			newFaces.reserve(faces.size() + horizon.size());
			{
				std::set<int> visSet(visible.begin(), visible.end());
				for (int fi = 0; fi < static_cast<int>(faces.size()); ++fi) {
					if (!visSet.count(fi)) {
						newFaces.push_back(faces[fi]);
					}
				}
			}

			// Create new faces from horizon to the point; reverse orientation of edge to maintain outward normal, then fix using inside point
			for (const auto& e : horizon) {
				addFace(newFaces, e.v, e.u, pIdx, pts, center, eps);
			}

			faces.swap(newFaces);
			processed.insert(pIdx);
		}

		// Compute inside point as centroid of unique hull vertices to ensure it lies inside
		{
			std::set<int> hullVerts;
			for (const auto& f : faces) {
				hullVerts.insert(f.a);
				hullVerts.insert(f.b);
				hullVerts.insert(f.c);
			}
			if (!hullVerts.empty()) {
				Vector3<T> sum = Vector3<T>::Zero;
				for (int idx : hullVerts) sum = sum + pts[idx];
				center = sum / static_cast<T>(hullVerts.size());
			}
		}

		// Triangulate hull volume into tetrahedra using the inside point
		for (const auto& f : faces) {
			// Ensure face is outward; orient if needed before creating simplex
			Face tmp = f;
			orientFaceOutward(tmp, pts, center, eps);
			simplices.emplace_back(pts[tmp.a], pts[tmp.b], pts[tmp.c], center);
		}
	}
};