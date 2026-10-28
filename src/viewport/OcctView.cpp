#include "OcctView.h"

#include "geometry/SolidFactory.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_ViewCube.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Graphic3d_Camera.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Precision.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Failure.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

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

#include <cmath>

namespace ms3d::viewport {
namespace {

gp_Ax2 planeAx2(ms3d::sketch::PlaneId plane) {
    switch (plane) {
    case ms3d::sketch::PlaneId::XZ:
        return gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0), gp_Dir(1, 0, 0));
    case ms3d::sketch::PlaneId::YZ:
        return gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0));
    case ms3d::sketch::PlaneId::XY:
    default:
        return gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0));
    }
}

} // namespace

OcctView::OcctView(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(200, 200);
}

OcctView::~OcctView() = default;

bool OcctView::initializeViewer(QString* errorOut) {
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

void OcctView::ensureWindowMapped() {
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

bool OcctView::displaySolid(const TopoDS_Shape& shape, QString* errorOut) {
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

bool OcctView::displayBox(double dx, double dy, double dz, QString* errorOut) {
    const auto created = ms3d::geometry::makeBox(dx, dy, dz);
    if (!created.ok) {
        if (errorOut) {
            *errorOut = QString::fromStdString(created.error);
        }
        return false;
    }
    return displaySolid(created.solid, errorOut);
}

void OcctView::clearSolidDisplay() {
    if (m_context && !m_displayedAis.IsNull()) {
        m_context->Remove(m_displayedAis, Standard_False);
        m_displayedAis.Nullify();
        redraw();
    }
}

void OcctView::setSketch(ms3d::sketch::Sketch* sketch) {
    m_sketch = sketch;
    m_hasSketchAnchor = false;
    refreshSketchDisplay();
}

void OcctView::setSketchTool(SketchTool tool) {
    m_sketchTool = tool;
    m_hasSketchAnchor = false;
    if (tool != SketchTool::None) {
        clearSolidDisplay();
        refreshSketchDisplay();
    }
}

void OcctView::setSketchPlane(ms3d::sketch::PlaneId plane) {
    if (!m_sketch) {
        return;
    }
    m_sketch->setPlane(plane);
    m_hasSketchAnchor = false;
    refreshSketchDisplay();
    emit sketchChanged();
}

void OcctView::clearSketchAis() {
    if (!m_context) {
        return;
    }
    for (auto& ais : m_sketchAis) {
        if (!ais.IsNull()) {
            m_context->Remove(ais, Standard_False);
        }
    }
    m_sketchAis.clear();
    m_sketchAisEntityIds.clear();
}

void OcctView::refreshSketchDisplay() {
    if (!initializeViewer(nullptr) || !m_sketch || !m_context) {
        return;
    }

    clearSketchAis();

    TopoDS_Compound compound;
    std::string err;
    if (!m_sketch->buildDisplayCompound(compound, &err)) {
        emit statusMessage(QString::fromStdString(err));
        redraw();
        return;
    }

    // One AIS per entity for selection mapping
    for (const auto& entity : m_sketch->entities()) {
        ms3d::sketch::Sketch single(m_sketch->plane());
        if (entity.kind == ms3d::sketch::EntityKind::Line) {
            single.addLine(entity.p0, entity.p1);
        } else if (entity.kind == ms3d::sketch::EntityKind::Circle) {
            single.addCircle(entity.p0, entity.r);
        } else if (entity.kind == ms3d::sketch::EntityKind::Rectangle) {
            single.addRectangle(entity.p0, entity.w, entity.h);
        }
        // Re-assign known id by rebuilding compound from a temporary that only has geometry
        TopoDS_Compound edgeComp;
        if (!single.buildDisplayCompound(edgeComp, nullptr)) {
            continue;
        }
        Handle(AIS_Shape) ais = new AIS_Shape(edgeComp);
        ais->SetColor(Quantity_NOC_ORANGE);
        ais->SetDisplayMode(AIS_WireFrame);
        ais->SetWidth(2.0);
        m_context->Display(ais, Standard_False);
        m_context->Activate(ais, AIS_Shape::SelectionMode(TopAbs_SHAPE));
        m_sketchAis.push_back(ais);
        m_sketchAisEntityIds.push_back(entity.id);
    }

    redraw();
}

bool OcctView::pickSketchUv(int sx, int sy, ms3d::sketch::Vec2& uv) const {
    if (!m_view || !m_sketch) {
        return false;
    }

    Standard_Real X = 0, Y = 0, Z = 0;
    m_view->Convert(sx, sy, X, Y, Z);
    const gp_Pnt onView(X, Y, Z);
    const Handle(Graphic3d_Camera) cam = m_view->Camera();
    if (cam.IsNull()) {
        return false;
    }
    const gp_Pnt eye = cam->Eye();
    gp_Vec dirVec(eye, onView);
    if (dirVec.Magnitude() < Precision::Confusion()) {
        return false;
    }
    const gp_Lin ray(eye, gp_Dir(dirVec));
    const gp_Ax2 ax = planeAx2(m_sketch->plane());
    const gp_Pln pln(ax);
    IntAna_IntConicQuad inter(ray, pln, Precision::Angular(), Precision::Confusion());
    if (!inter.IsDone() || inter.NbPoints() < 1) {
        return false;
    }
    const gp_Pnt hit = inter.Point(1);
    const gp_Vec rel(ax.Location(), hit);
    uv.u = rel.Dot(gp_Vec(ax.XDirection()));
    uv.v = rel.Dot(gp_Vec(ax.YDirection()));
    return true;
}

void OcctView::selectSketchEntityAt(int sx, int sy) {
    if (!m_context || !m_view) {
        return;
    }
    m_context->MoveTo(sx, sy, m_view, Standard_True);
    m_context->Select(Standard_True);

    int selectedId = -1;
    for (m_context->InitSelected(); m_context->MoreSelected(); m_context->NextSelected()) {
        Handle(AIS_InteractiveObject) obj = m_context->SelectedInteractive();
        for (size_t i = 0; i < m_sketchAis.size(); ++i) {
            if (m_sketchAis[i] == obj) {
                selectedId = m_sketchAisEntityIds[i];
                break;
            }
        }
        if (selectedId >= 0) {
            break;
        }
    }
    emit sketchSelectionChanged(selectedId);
    redraw();
}

bool OcctView::handleSketchPress(QMouseEvent* event) {
    if (!m_sketch || m_sketchTool == SketchTool::None || event->button() != Qt::LeftButton) {
        return false;
    }
    if (event->modifiers() & (Qt::ShiftModifier | Qt::AltModifier)) {
        return false; // leave orbit/pan to camera handlers
    }

    if (m_sketchTool == SketchTool::Select) {
        selectSketchEntityAt(event->pos().x(), event->pos().y());
        return true;
    }

    ms3d::sketch::Vec2 uv;
    if (!pickSketchUv(event->pos().x(), event->pos().y(), uv)) {
        emit statusMessage(tr("Could not pick a point on the sketch plane"));
        return true;
    }

    if (!m_hasSketchAnchor) {
        m_sketchAnchor = uv;
        m_hasSketchAnchor = true;
        emit statusMessage(tr("Sketch: click second point to finish"));
        return true;
    }

    const ms3d::sketch::Vec2 a = m_sketchAnchor;
    const ms3d::sketch::Vec2 b = uv;
    m_hasSketchAnchor = false;

    int id = -1;
    if (m_sketchTool == SketchTool::Line) {
        id = m_sketch->addLine(a, b);
    } else if (m_sketchTool == SketchTool::Circle) {
        const double r = std::hypot(b.u - a.u, b.v - a.v);
        id = m_sketch->addCircle(a, r);
    } else if (m_sketchTool == SketchTool::Rectangle) {
        id = m_sketch->addRectangle(a, b.u - a.u, b.v - a.v);
    }

    if (id >= 0) {
        refreshSketchDisplay();
        emit sketchChanged();
        emit sketchSelectionChanged(id);
        emit statusMessage(tr("Sketch entity created (id %1)").arg(id));
    }
    return true;
}

void OcctView::fitAll() {
    if (m_view) {
        m_view->FitAll(0.1, Standard_False);
        m_view->ZFitAll();
        redraw();
    }
}

void OcctView::redraw() {
    if (m_view) {
        m_view->Redraw();
    }
}

void OcctView::paintEvent(QPaintEvent*) {
    redraw();
}

void OcctView::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (m_view) {
        m_view->MustBeResized();
        redraw();
    }
}

void OcctView::showEvent(QShowEvent* event) {
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

void OcctView::wheelEvent(QWheelEvent* event) {
    if (!m_view) {
        return;
    }
    const int delta = event->angleDelta().y();
    if (delta != 0) {
        m_view->SetZoom(delta > 0 ? 1.1 : (1.0 / 1.1), Standard_True);
    }
}

void OcctView::mousePressEvent(QMouseEvent* event) {
    m_lastButton = event->button();
    m_lastPos = event->pos();

    if (!m_context || !m_view) {
        return;
    }

    if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) {
        m_view->StartRotation(event->pos().x(), event->pos().y());
        return;
    }

    if (handleSketchPress(event)) {
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

void OcctView::mouseReleaseEvent(QMouseEvent*) {
    m_lastButton = Qt::NoButton;
}

void OcctView::mouseMoveEvent(QMouseEvent* event) {
    if (!m_view) {
        return;
    }

    const QPoint pos = event->pos();
    const int dx = pos.x() - m_lastPos.x();
    const int dy = pos.y() - m_lastPos.y();

    if (m_lastButton == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) {
        m_view->Rotation(pos.x(), pos.y());
    } else if (m_lastButton == Qt::MiddleButton ||
               (m_lastButton == Qt::LeftButton && (event->modifiers() & Qt::AltModifier))) {
        m_view->Pan(dx, -dy);
    } else if (m_lastButton == Qt::RightButton) {
        m_view->Zoom(m_lastPos.x(), m_lastPos.y(), pos.x(), pos.y());
    } else if (m_context && m_lastButton == Qt::NoButton) {
        m_context->MoveTo(pos.x(), pos.y(), m_view, Standard_True);
    }

    m_lastPos = pos;
}

} // namespace ms3d::viewport
