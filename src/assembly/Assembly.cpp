#include "Assembly.h"

#include <BRepBuilderAPI_Transform.hxx>
#include <BRep_Builder.hxx>
#include <Standard_Failure.hxx>

#include <algorithm>
#include <sstream>

namespace ms3d::assembly {

int Assembly::nextId()
{
    return m_nextId++;
}

void Assembly::clear()
{
    m_instances.clear();
    m_nextId = 1;
    m_nameSerial = 1;
}

int Assembly::addInstance(const TopoDS_Shape& shape, const gp_Trsf& transform,
                          const std::string& namePrefix)
{
    if (shape.IsNull()) {
        return -1;
    }
    ComponentInstance inst;
    inst.id = nextId();
    inst.shape = shape;
    inst.transform = transform;
    std::ostringstream name;
    name << (namePrefix.empty() ? "Component" : namePrefix) << m_nameSerial++;
    inst.name = name.str();
    m_instances.push_back(inst);
    return inst.id;
}

bool Assembly::removeInstance(int id)
{
    const auto it = std::remove_if(m_instances.begin(), m_instances.end(),
                                   [id](const ComponentInstance& c) { return c.id == id; });
    if (it == m_instances.end()) {
        return false;
    }
    m_instances.erase(it, m_instances.end());
    return true;
}

AssemblyResult Assembly::buildCompound() const
{
    AssemblyResult result;
    result.instanceCount = static_cast<int>(m_instances.size());
    if (m_instances.empty()) {
        result.error = "Assembly has no component instances";
        return result;
    }

    try {
        BRep_Builder builder;
        builder.MakeCompound(result.compound);
        for (const auto& inst : m_instances) {
            if (inst.shape.IsNull()) {
                result.error = "Assembly contains a null component shape: " + inst.name;
                return result;
            }
            BRepBuilderAPI_Transform xform(inst.shape, inst.transform, Standard_True);
            if (!xform.IsDone()) {
                result.error = "Failed to transform component: " + inst.name;
                return result;
            }
            builder.Add(result.compound, xform.Shape());
        }
        result.ok = true;
        return result;
    } catch (const Standard_Failure& ex) {
        result.error = ex.GetMessageString() ? ex.GetMessageString()
                                             : "OCCT failure building assembly compound";
        return result;
    } catch (...) {
        result.error = "Unknown exception building assembly compound";
        return result;
    }
}

} // namespace ms3d::assembly
