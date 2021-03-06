#include "cheadlines.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include "glutils.h"
#include "vec3.h"
#include "vec2.h"
#include "glm.h"

CHeadLines::CHeadLines()
{

}

void CHeadLines::resetHead()
{
    calcList.clear();
    hdLine.clear();
}

bool CHeadLines::isPointInHeadArea(Vec3 testPointv2)
{
    if (calcList.size() < 3) return false;
    int j = hdLine.size() - 1;
    bool oddNodes = false;

    //test against the constant and multiples list the test point
    for (int i = 0; i < hdLine.size(); j = i++)
    {
        if ((hdLine[i].northing < testPointv2.northing && hdLine[j].northing >= testPointv2.northing)
        || (hdLine[j].northing < testPointv2.northing && hdLine[i].northing >= testPointv2.northing))
        {
            oddNodes ^= ((testPointv2.northing * calcList[i].northing) + calcList[i].easting < testPointv2.easting);
        }
    }
    return oddNodes; //true means inside.
}

bool CHeadLines::isPointInHeadArea(Vec2 testPointv2)
{
    if (calcList.size() < 3) return false;
    int j = hdLine.size() - 1;
    bool oddNodes = false;

    //test against the constant and multiples list the test point
    for (int i = 0; i < hdLine.size(); j = i++)
    {
        if ((hdLine[i].northing < testPointv2.northing && hdLine[j].northing >= testPointv2.northing)
        || (hdLine[j].northing < testPointv2.northing && hdLine[i].northing >= testPointv2.northing))
        {
            oddNodes ^= ((testPointv2.northing * calcList[i].northing) + calcList[i].easting < testPointv2.easting);
        }
    }
    return oddNodes; //true means inside.
}

void CHeadLines::drawHeadLine(QOpenGLFunctions *gl, const QMatrix4x4 &mvp, int linewidth)
{
    if (hdLine.size() < 2) return;
    int ptCount = hdLine.size();
    int cntr = 0;
    if (ptCount > 1)
    {
        GLHelperOneColor gldraw;

        gl->glLineWidth(linewidth);

        QColor color = QColor::fromRgbF(0.960f, 0.96232f, 0.30f);
        //GL.PointSize(2);

        while (cntr < ptCount)
        {
            if (isDrawList[cntr])
            {
                gldraw.clear();
                if (cntr > 0)
                    gldraw.append(QVector3D(hdLine[cntr - 1].easting, hdLine[cntr - 1].northing, 0));
                else
                    gldraw.append(QVector3D(hdLine[hdLine.size() - 1].easting, hdLine[hdLine.size() - 1].northing, 0));


                for (int i = cntr; i < ptCount; i++)
                {
                    cntr++;
                    if (!isDrawList[i]) break;
                    gldraw.append(QVector3D(hdLine[i].easting, hdLine[i].northing, 0));
                }
                if (cntr < ptCount - 1)
                gldraw.append(QVector3D(hdLine[cntr+1].easting, hdLine[cntr+1].northing, 0));

                gldraw.draw(gl, mvp, color, GL_LINE_STRIP, linewidth);
            }
            else
            {
                cntr++;
            }
        }
    }
}

void CHeadLines::drawHeadLineBackBuffer(QOpenGLFunctions *gl, const QMatrix4x4 &mvp)
{
    if (hdLine.count() < 2) return;
    int ptCount = hdLine.count();
    int cntr = 0;
    if (ptCount > 1)
    {
        GLHelperOneColor gldraw;
        gl->glLineWidth(3);

        QColor color = QColor::fromRgbF(0.960f, 0.96232f, 0.30f);

        while (cntr < ptCount)
        {
            if (isDrawList[cntr])
            {
                gldraw.clear();

                if (cntr > 0) gldraw.append(QVector3D(hdLine[cntr - 1].easting, hdLine[cntr - 1].northing, 0));
                else gldraw.append(QVector3D(hdLine[hdLine.count() - 1].easting, hdLine[hdLine.count() - 1].northing, 0));


                for (int i = cntr; i < ptCount; i++)
                {
                    cntr++;
                    if (!isDrawList[i]) break;
                    gldraw.append(QVector3D(hdLine[i].easting, hdLine[i].northing, 0));
                }
                if (cntr < ptCount - 1)
                    gldraw.append(QVector3D(hdLine[cntr + 1].easting, hdLine[cntr + 1].northing, 0));

                gldraw.draw(gl, mvp, color, GL_LINE_STRIP, 3.0);
            }
            else
            {
                cntr++;
            }
        }
    }
}

void CHeadLines::preCalcHeadLines()
{
    int j = hdLine.count() - 1;
    //clear the list, constant is easting, multiple is northing
    calcList.clear();
    Vec2 constantMultiple(0, 0);

    for (int i = 0; i < hdLine.count(); j = i++)
    {
        //check for divide by zero
        if (fabs(hdLine[i].northing - hdLine[j].northing) < glm::DOUBLE_EPSILON)
        {
            constantMultiple.easting = hdLine[i].easting;
            constantMultiple.northing = 0;
            calcList.append(constantMultiple);
        }
        else
        {
            //determine constant and multiple and add to list
            constantMultiple.easting = hdLine[i].easting - ((hdLine[i].northing * hdLine[j].easting)
                            / (hdLine[j].northing - hdLine[i].northing)) + ((hdLine[i].northing * hdLine[i].easting)
                                / (hdLine[j].northing - hdLine[i].northing));
            constantMultiple.northing = (hdLine[j].easting - hdLine[i].easting) / (hdLine[j].northing - hdLine[i].northing);
            calcList.append(constantMultiple);
        }
    }
}

