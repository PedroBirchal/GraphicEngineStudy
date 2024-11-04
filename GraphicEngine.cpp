#include <iostream>
#include "olcConsoleGameEngine.h"

struct vec3 {
    float x, y, z;
};

struct triangle {
    vec3 vertex[3];
};

struct mesh {
    vector<triangle> tris;
};

struct mat4x4 {
    float m[4][4] = { 0 };
};

class olcEngine3D : public olcConsoleGameEngine {
public:
    olcEngine3D() {
        m_sAppName = L"3D Demo";
    }

private:
    mesh cube;
    mat4x4 projMat;
    float fTheta;

    void mulVec(vec3& i, vec3& o, mat4x4& m) {
        o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
        o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
        o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
        float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

        if (w != 0) {
            o.x /= w;
            o.y /= w;
            o.z /= w;
        }
    }

public:
    bool OnUserCreate() override {
        
        // GEOMETRY:

        cube.tris = {
            // UP
            {0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f},

            // FRONT
            {0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f},

            // LEFT
            {0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f},

            // BACK
            {1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f},

            // RIGHT
            {1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f},

            // DOWN
            {1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f},
        };

        // PROJECTION MATRIX:

        float nearClip = 0.1f;
        float farClip = 1000.0f;
        float fov = 90.0f;
        float aspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
        float fovTangent = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);

        projMat.m[0][0] = aspectRatio * fovTangent;
        projMat.m[1][1] = fovTangent;
        projMat.m[2][2] = farClip / (farClip - nearClip);
        projMat.m[3][2] = (-farClip * nearClip) / (farClip - nearClip);
        projMat.m[2][3] = 1.0f;
        projMat.m[3][3] = 0.0f;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {

        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

        mat4x4 rotationMatZ, rotationMatX;
        fTheta += 1.0f * fElapsedTime;

        // Rotation Z

        rotationMatZ.m[0][0] = cosf(fTheta);
        rotationMatZ.m[0][1] = sinf(fTheta);
        rotationMatZ.m[1][0] = -sinf(fTheta);
        rotationMatZ.m[1][1] = cosf(fTheta);
        rotationMatZ.m[2][2] = 1;
        rotationMatZ.m[3][3] = 1;

        // Rotation X

        rotationMatX.m[0][0] = 1;
        rotationMatX.m[1][1] = cosf(fTheta * 0.5f);
        rotationMatX.m[1][2] = sinf(fTheta * 0.5f);
        rotationMatX.m[2][1] = -sinf(fTheta * 0.5f);
        rotationMatX.m[2][2] = cosf(fTheta * 0.5f);
        rotationMatX.m[3][3] = 1;
        

        for (auto tri : cube.tris) {
            triangle projTri, transTri, rotTriXZ, rotTriZ;

            // Rotation Z

            for (int i = 0; i < 3; i++) {

                mulVec(tri.vertex[i], rotTriZ.vertex[i], rotationMatZ);

            }

            // Rotation X

            for (int i = 0; i < 3; i++) {
                mulVec(rotTriZ.vertex[i], rotTriXZ.vertex[i], rotationMatX);
            }

            // Translation
            transTri = rotTriXZ;
            for (int i = 0; i < 3; i++) {
                transTri.vertex[i].z = rotTriXZ.vertex[i].z + 3.0f;
            }

            mulVec(transTri.vertex[0], projTri.vertex[0], projMat);
            mulVec(transTri.vertex[1], projTri.vertex[1], projMat);
            mulVec(transTri.vertex[2], projTri.vertex[2], projMat);

            projTri.vertex[0].x += 1.0f; projTri.vertex[0].y += 1.0f;
            projTri.vertex[1].x += 1.0f; projTri.vertex[1].y += 1.0f;
            projTri.vertex[2].x += 1.0f; projTri.vertex[2].y += 1.0f;

            projTri.vertex[0].x *= 0.5f * (float)ScreenWidth();
            projTri.vertex[1].x *= 0.5f * (float)ScreenWidth();
            projTri.vertex[2].x *= 0.5f * (float)ScreenWidth();

            projTri.vertex[0].y *= 0.5f * (float)ScreenHeight();
            projTri.vertex[1].y *= 0.5f * (float)ScreenHeight();
            projTri.vertex[2].y *= 0.5f * (float)ScreenHeight();

            DrawTriangle(
                projTri.vertex[0].x, projTri.vertex[0].y,
                projTri.vertex[1].x, projTri.vertex[1].y,
                projTri.vertex[2].x, projTri.vertex[2].y,
                PIXEL_SOLID, FG_CYAN
            );

        }

        return true;
    }

};

int main()
{
    olcEngine3D demo;
    if (demo.ConstructConsole(256, 240, 4, 4))
        demo.Start();

    return 0;
}
