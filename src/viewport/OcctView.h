#pragma once

#include <QWidget>
#include <Standard_Handle.hxx>
#include <TopoDS_Shape.hxx>

class AIS_InteractiveContext;
class AIS_Shape;
class AIS_Trihedron;
class AIS_ViewCube;
class Aspect_DisplayConnection;
class Aspect_Window;
class OpenGl_GraphicDriver;
class V3d_View;
class V3d_Viewer;

namespace ms3d::viewport {

class OcctView : public QWidget {
    Q_OBJECT
public:
    explicit OcctView(QWidget* parent = nullptr);
    ~OcctView() override;

    bool initializeViewer(QString* errorOut = nullptr);
    bool displaySolid(const TopoDS_Shape& shape, QString* errorOut = nullptr);
    bool displayBox(double dx, double dy, double dz, QString* errorOut = nullptr);
    void fitAll();

    Handle(AIS_InteractiveContext) context() const { return m_context; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    QPaintEngine* paintEngine() const override { return nullptr; }

private:
    void redraw();
    void ensureWindowMapped();

    Handle(Aspect_DisplayConnection) m_displayConnection;
    Handle(OpenGl_GraphicDriver) m_graphicDriver;
    Handle(V3d_Viewer) m_viewer;
    Handle(V3d_View) m_view;
    Handle(AIS_InteractiveContext) m_context;
    Handle(Aspect_Window) m_window;
    Handle(AIS_Trihedron) m_trihedron;
    Handle(AIS_ViewCube) m_viewCube;
    Handle(AIS_Shape) m_displayedAis;

    bool m_initialized = false;
    Qt::MouseButton m_lastButton = Qt::NoButton;
    QPoint m_lastPos;
};

} // namespace ms3d::viewport
