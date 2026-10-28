#pragma once

#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>

#include <string>
#include <vector>

namespace ms3d::assembly {

struct ComponentInstance {
    int id = 0;
    std::string name;
    TopoDS_Shape shape; // source BREP (copied by value handle semantics of TopoDS)
    gp_Trsf transform;  // millimetre translation/rotation in model space
};

struct AssemblyResult {
    bool ok = false;
    TopoDS_Compound compound;
    std::string error;
    int instanceCount = 0;
};

/// In-memory multi-body assembly (session capability; not .ms3d v1).
class Assembly {
public:
    const std::vector<ComponentInstance>& instances() const { return m_instances; }

    int addInstance(const TopoDS_Shape& shape, const gp_Trsf& transform,
                    const std::string& namePrefix = "Component");
    bool removeInstance(int id);
    void clear();

    AssemblyResult buildCompound() const;

private:
    int nextId();
    std::vector<ComponentInstance> m_instances;
    int m_nextId = 1;
    int m_nameSerial = 1;
};

} // namespace ms3d::assembly
