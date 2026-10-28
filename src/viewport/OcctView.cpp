#include "OcctView.h"

#include "geometry/SolidFactory.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_ViewCube.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <Geom_Axis2Placement.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Failure.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <gp.hxx>

#ifdef _WIN32
#include <WNT_Window.hxx>
#else
#include <Xw_Window.hxx>
#endif

#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QString>
#include <QWheelEvent>
#include <QtGlobal>

namespace ms3d::viewport {

OcctView::OcctView(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(200, 200);
}

OcctView::~OcctView() = default;

bool OcctView::initializeViewer(QString* errorOut)
{
    if (m_initialized) {
        return true;
    }

    try {
        m_displayConnection = new Aspect_DisplayConnection();
        m_graphicDriver = new OpenGl_GraphicDriver(m_displayConnection);

        m_viewer = new V3d_Viewer(m_graphicDriver);
        m_viewer->SetDefaultLights();
        m_viewer->SetLightOn();

        m_view = m_viewer->CreateView();
        m_view->SetBackgroundColor(Quantity_Color(0.18, 0.18, 0.20, Quantity_TOC_RGB));

        m_context = new AIS_InteractiveContext(m_viewer);

        ensureWindowMapped();
        m_view->MustBeResized();
        m_view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.08, V3d_ZBUFFER);

        Handle(Geom_Axis2Placement) axis = new Geom_Axis2Placement(gp::XOY());
        m_trihedron = new AIS_Trihedron(axis);
        m_trihedron->SetSize(30.0);
        m_context->Display(m_trihedron, Standard_False);

        m_viewCube = new AIS_ViewCube();
        m_viewCube->SetSize(60.0);
        m_viewCube->SetBoxColor(Quantity_NOC_GRAY75);
        m_context->Display(m_viewCube, Standard_False);

        m_view->Redraw();
        m_initialized = true;
        return true;
    } catch (const Standard_Failure& ex) {
        if (errorOut) {
            *errorOut = QString::fromUtf8(ex.GetMessageString() ? ex.GetMessageString()
                                                                : "OCCT viewer init failed");
        }
        return false;
    } catch (...) {
        if (errorOut) {
            *errorOut = QStringLiteral("Unknown error initializing OCCT viewer");
        }
        return false;
    }
}

void OcctView::ensureWindowMapped()
{
    if (!m_view) {
        return;
    }

#ifdef _WIN32
    const Aspect_Handle hwnd = (Aspect_Handle)winId();
    m_window = new WNT_Window(hwnd);
#else
    const Aspect_Handle drawable = (Aspect_Handle)winId();
    m_window = new Xw_Window(m_displayConnection, drawable);
#endif
    m_view->SetWindow(m_window);
    if (!m_window->IsMapped()) {
        m_window->Map();
    }
}

bool OcctView::displaySolid(const TopoDS_Shape& shape, QString* errorOut)
{
    if (!initializeViewer(errorOut)) {
        return false;
    }
    if (shape.IsNull()) {
        if (errorOut) {
            *errorOut = QStringLiteral("Cannot display a null shape");
        }
        return false;
    }

    try {
        if (!m_displayedAis.IsNull()) {
            m_context->Remove(m_displayedAis, Standard_False);
            m_displayedAis.Nullify();
        }

        m_displayedAis = new AIS_Shape(shape);
        m_displayedAis->SetColor(Quantity_NOC_DEEPSKYBLUE3);
        m_displayedAis->SetDisplayMode(AIS_Shaded);
        m_context->Display(m_displayedAis, Standard_False);
        m_context->Activate(m_displayedAis, AIS_Shape::SelectionMode(TopAbs_SHAPE));
        fitAll();
        return true;
    } catch (const Standard_Failure& ex) {
        if (errorOut) {
            *errorOut = QString::fromUtf8(ex.GetMessageString() ? ex.GetMessageString()
                                                                : "Failed to display solid");
        }
        return false;
    }
}

bool OcctView::displayBox(double dx, double dy, double dz, QString* errorOut)
{
    const auto created = ms3d::geometry::makeBox(dx, dy, dz);
    if (!created.ok) {
        if (errorOut) {
            *errorOut = QString::fromStdString(created.error);
        }
        return false;
    }
    return displaySolid(created.solid, errorOut);
}

void OcctView::fitAll()
{
    if (m_view) {
        m_view->FitAll(0.1, Standard_False);
        m_view->ZFitAll();
        redraw();
    }
}

void OcctView::redraw()
{
    if (m_view) {
        m_view->Redraw();
    }
}

void OcctView::paintEvent(QPaintEvent*)
{
    redraw();
}

void OcctView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_view) {
        m_view->MustBeResized();
        redraw();
    }
}

void OcctView::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!m_initialized) {
        QString err;
        if (!initializeViewer(&err)) {
            qWarning("OcctView init: %s", qUtf8Printable(err));
        }
    }
    if (m_view) {
        ensureWindowMapped();
        m_view->MustBeResized();
        redraw();
    }
}

void OcctView::wheelEvent(QWheelEvent* event)
{
    if (!m_view) {
        return;
    }
    const int delta = event->angleDelta().y();
    if (delta != 0) {
        m_view->SetZoom(delta > 0 ? 1.1 : (1.0 / 1.1), Standard_True);
    }
}

void OcctView::mousePressEvent(QMouseEvent* event)
{
    m_lastButton = event->button();
    m_lastPos = event->pos();

    if (!m_context || !m_view) {
        return;
    }

    if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) {
        m_view->StartRotation(event->pos().x(), event->pos().y());
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_context->MoveTo(event->pos().x(), event->pos().y(), m_view, Standard_True);
        if (event->modifiers() & Qt::ControlModifier) {
            m_context->ShiftSelect(Standard_True);
        } else {
            m_context->Select(Standard_True);
        }
        redraw();
    }
}

void OcctView::mouseReleaseEvent(QMouseEvent*)
{
    m_lastButton = Qt::NoButton;
}

void OcctView::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_view) {
        return;
    }

    const QPoint pos = event->pos();
    const int dx = pos.x() - m_lastPos.x();
    const int dy = pos.y() - m_lastPos.y();

    if (m_lastButton == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) {
        m_view->Rotation(pos.x(), pos.y());
    } else if (m_lastButton == Qt::MiddleButton
               || (m_lastButton == Qt::LeftButton && (event->modifiers() & Qt::AltModifier))) {
        m_view->Pan(dx, -dy);
    } else if (m_lastButton == Qt::RightButton) {
        m_view->Zoom(m_lastPos.x(), m_lastPos.y(), pos.x(), pos.y());
    } else if (m_context && m_lastButton == Qt::NoButton) {
        m_context->MoveTo(pos.x(), pos.y(), m_view, Standard_True);
    }

    m_lastPos = pos;
}

} // namespace ms3d::viewport
