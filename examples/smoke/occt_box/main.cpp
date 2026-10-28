#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <iostream>

int main()
{
    try {
        BRepPrimAPI_MakeBox maker(10.0, 20.0, 30.0);
        const TopoDS_Solid solid = maker.Solid();

        BRepCheck_Analyzer analyzer(solid);
        if (!analyzer.IsValid()) {
            std::cerr << "OCCT smoke FAIL: solid is not valid\n";
            return 1;
        }

        int solidCount = 0;
        for (TopExp_Explorer exp(solid, TopAbs_SOLID); exp.More(); exp.Next()) {
            ++solidCount;
        }

        if (solidCount < 1) {
            std::cerr << "OCCT smoke FAIL: expected at least one solid\n";
            return 1;
        }

        std::cout << "MS3DMaker OCCT smoke OK: box solid count=" << solidCount << "\n";
        return 0;
    } catch (const Standard_Failure& ex) {
        std::cerr << "OCCT smoke FAIL: " << ex.GetMessageString() << "\n";
        return 2;
    } catch (...) {
        std::cerr << "OCCT smoke FAIL: unknown exception\n";
        return 3;
    }
}
