
#include "MantidQtSliceViewer/PhysicalSphericalPeak.h"

namespace MantidQt
{
  namespace SliceViewer
  {
      PhysicalSphericalPeak::PhysicalSphericalPeak(const Mantid::Kernel::V3D& origin, const double& radius):
      m_originalOrigin(origin),
      m_origin(origin),
      m_radius(radius),
      m_opacityMax(0.8),
      m_opacityMin(0.0),
      m_opacityAtDistance(0.0),
      m_radiusAtDistance(0.0)
      {
      }

      PhysicalSphericalPeak::~PhysicalSphericalPeak()
      {
      }

        //----------------------------------------------------------------------------------------------
        /** Set the distance between the plane and the center of the peak in md coordinates

        ASCII diagram below to demonstrate how dz (distance in z) is used to determine the radius of the sphere-plane intersection at that point,
        resloves both rx and ry. Also uses the distance to calculate the opacity to apply.

        @param z : position of the plane slice in the z dimension.

             /---------\
            /           \
        ---/---------rx--\---------------- plane
           |    dz|     /| peak
           |      |   /  |
           |      . /    |
           |             |
           \             /
            \           /
             \---------/
        */
      void PhysicalSphericalPeak::setSlicePoint(const double& z)
      {
        const double distanceSQ = (z - m_origin.Z()) * (z - m_origin.Z());
        const double distance = std::sqrt(distanceSQ);
        const double radSQ = m_radius * m_radius;

        if(distanceSQ < radSQ)
        {
          m_radiusAtDistance = std::sqrt( radSQ - distanceSQ );
        }
        else
        {
          m_radiusAtDistance = 0;
        }

        // Apply a linear transform to convert from a distance to an opacity between opacityMin and opacityMax.
        m_opacityAtDistance = ((m_opacityMin - m_opacityMax)/m_radius) * distance  + m_opacityMax;
        m_opacityAtDistance = m_opacityAtDistance >= m_opacityMin ? m_opacityAtDistance : m_opacityMin;
      }

      void PhysicalSphericalPeak::movePosition(PeakTransform_sptr peakTransform)
      {
        m_origin = peakTransform->transform(m_originalOrigin);
      }

      MantidQt::SliceViewer::SphericalPeakPrimitives PhysicalSphericalPeak::draw(const double& windowHeight, const double& windowWidth, const double& viewWidth, const double& viewHeight) const
      {
        // Scale factor for going from viewX to windowX
        const double scaleY = windowHeight/viewHeight;
        // Scale factor for going from viewY to windowY
        const double scaleX = windowWidth/viewWidth;

        const double innerRadiusX = scaleX * m_radiusAtDistance;
        const double innerRadiusY = scaleY * m_radiusAtDistance;

        double outerRadiusX = scaleX * m_radius;
        double outerRadiusY = scaleY * m_radius;

        const double lineWidthX = outerRadiusX - innerRadiusX;
        const double lineWidthY = outerRadiusY - innerRadiusY;
        outerRadiusX -= lineWidthX/2;
        outerRadiusY -= lineWidthY/2;

        // Create the return object.
        SphericalPeakPrimitives drawingObjects;
        drawingObjects.peakOuterRadiusX = outerRadiusX;
        drawingObjects.peakOuterRadiusY = outerRadiusY;
        drawingObjects.peakLineWidth = lineWidthX;
        drawingObjects.peakOpacityAtDistance = m_opacityAtDistance;
        drawingObjects.peakOrigin = m_origin;
        return drawingObjects;
      }
  }
}