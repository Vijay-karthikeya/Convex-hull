#include "Con_algo.h"

vector<Side> convexHull2d(vector<Point2d> v) {
    // the reference point is the point with the lowest y value
    Point2d rf(INT_MAX, INT_MAX);
    for (const auto& p : v) if (p.y < rf.y) rf = p;
    // using this we calculate the ref side which says that there is no point under this side, using which we can calculate angles which gives our point priority order
    Side rf_s(rf, Point2d(rf.x + 100, rf.y));
    map<Point2d, double> m;
    for (auto& p : v) {
        if (p == rf) {
            m[rf] = 0;
            continue;
        }
        Side temp(rf, p);
        m[p] = Side::angle(rf_s, temp);
    }
    sort(v.begin(), v.end(), [&](Point2d& A, Point2d& B) {return m[A] < m[B];});
    vector<Side> ans;
    Point2d p1 = rf, p2 = v[1];
    Side curr(p1, p2);
    ans.push_back(curr);
    curr = Side(p2, v[2]);
    ans.push_back(curr);
    p1 = p2;
    p2 = v[2];
    for (int i = 3;i < v.size();i++) {
        Side temp(p1, v[i]);
        int c = curr.cross(temp);
        if (c > 0) {
            curr = Side(p2, v[i]);
            ans.push_back(curr);
            p1 = p2;
            p2 = v[i];
        }
        else {
            ans.pop_back();
            curr = Side(p1, v[i]);
            ans.push_back(curr);
            p2 = v[i];
        }
    }
    curr = Side(rf, ans.back().p2);
    ans.push_back(curr);
    return ans;
}





vector<face> convexhull3d(vector<Point3d> p) {
    int n = p.size();
    vector<face> f;

    // Consider an edge (a->b) visible if it is not a CCW edge of some current face
    // If an edge is alive but not its reverse, this is an exposed edge.
    // We should add new faces on the exposed edges.
    vector<vector<bool>> visible(n, vector<bool>(n, true));

    auto add_face = [&](int a, int b, int c) {
        f.push_back({ a, b, c, (p[b] - p[a]).cross(p[c] - p[a]) });
        visible[a][b] = visible[b][c] = visible[c][a] = false;
    };


    // Initialize the convex hull of the first 3 points as a
    // triangular disk with two faces of opposite orientation
    // We initialise the convex hull by making a triangle and setting the faces as not visible

    for (int i = 2;i < n;i++) {
        if (area(p[0], p[1], p[i]) != 0) {
            add_face(0, 1, i);
            add_face(i, 1, 0);
            break;
        }
    }

    for (int i = 2;i < n;i++) {
        // f2 will be the list of faces invisible to the added point p[i]
        vector<face> f2;
        for (auto& F : f) {
            if ((p[i] - p[F.a]).dot(F.norm) > 0) {
                // this face is visible to the new point, so mark its edges as visible
                visible[F.a][F.b] = visible[F.b][F.c] = visible[F.c][F.a] = true;
            }
            else {
                f2.push_back(F);
            }
        }
        // Add a new face for each exposed edge.
        // Only check edges of alive faces for being exposed.
        f.clear();
        for (auto& F : f2) {
            int arr[3] = { F.a, F.b, F.c };
            for (int j = 0;j < 3; j++) {
                int a = arr[j], b = arr[(j + 1) % 3]; //By doing this the values go as (1,2);(2,3);(3,1) making it anti-clockwise cyclic
                if (visible[b][a]) {
                    add_face(b, a, i);
                }
            }
            f.push_back(F);
        }
    }
    return f;
}