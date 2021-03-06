// ITK
#include <itkQuadEdgeMesh.h>
#include <itkRegularSphereMeshSource.h>
#include <itkModifiedButterflyTriangleCellSubdivisionQuadEdgeMeshFilter.h>
#include <itkLinearTriangleCellSubdivisionQuadEdgeMeshFilter.h>
#include <itkLoopTriangleCellSubdivisionQuadEdgeMeshFilter.h>
#include <itkSquareThreeTriangleCellSubdivisionQuadEdgeMeshFilter.h>
#include <itkTriangleHelper.h>
#include <itkIterativeTriangleCellSubdivisionQuadEdgeMeshFilter.h>

// VTK
#include <vtkPolyData.h>

// Custom
#include <dvITKMeshToVTKPolyData.h>
#include <dvQuickViewMultiplePolyData.h>

template< typename SubdivisionType >
void
SubdivisionTestHelper(const bool uniform) {

  // Typedefs
  const unsigned int Dimension = 3;
  using TCoordinate = float;
  using TQEMesh = itk::QuadEdgeMesh<TCoordinate, Dimension>;
  using TSource = itk::RegularSphereMeshSource< TQEMesh >;
  using TTriangleHelper = itk::TriangleHelper< typename TQEMesh::PointType >;
  using TSubdivision = SubdivisionType;
  using TSubdivisionIt = itk::IterativeTriangleCellSubdivisionQuadEdgeMeshFilter< TQEMesh, TSubdivision >;

  // Generate some input mesh data
  const auto sphere = TSource::New();
  sphere->Update();

  const auto i_mesh = TQEMesh::New();
  i_mesh->Graft( sphere->GetOutput() );
  i_mesh->DisconnectPipeline();

  typename TSubdivisionIt::SubdivisionCellContainer cells_to_subdivide;

  // Assign cell data (different for each octant).
  for (auto it = i_mesh->GetCells()->Begin();
    it != i_mesh->GetCells()->End();
    ++it) {
    const auto cell = it.Value();

    const auto centroid = TTriangleHelper::ComputeGravityCenter(
      i_mesh->GetPoint(cell->GetPointIds()[0]),
      i_mesh->GetPoint(cell->GetPointIds()[1]),
      i_mesh->GetPoint(cell->GetPointIds()[2])
    );

    unsigned int data = 0;
    if (centroid[0] < 0 && centroid[1] < 0 && centroid[2] < 0) {
      data = 1;
    } else if (centroid[0] < 0 && centroid[1] < 0 && centroid[2] >= 0) {
      data = 2;
    } else if (centroid[0] < 0 && centroid[1] >= 0 && centroid[2] < 0) {
      data = 3;
    } else if (centroid[0] < 0 && centroid[1] >= 0 && centroid[2] >= 0) {
      data = 4;
      cells_to_subdivide.push_back(it.Index());
    } else if (centroid[0] >= 0 && centroid[1] < 0 && centroid[2] < 0) {
      data = 5;
    } else if (centroid[0] >= 0 && centroid[1] < 0 && centroid[2] >= 0) {
      data = 6;
    } else if (centroid[0] >= 0 && centroid[1] >= 0 && centroid[2] < 0) {
      data = 7;
    } else if (centroid[0] >= 0 && centroid[1] >= 0 && centroid[2] >= 0) {
      data = 8;
    }

    i_mesh->SetCellData( it.Index(), data );
  }

  // Assert one CellData entry for each Cell
  const auto i_cell = i_mesh->GetNumberOfCells();
  const auto i_data = i_mesh->GetCellData()->Size();
  itkAssertOrThrowMacro(i_cell == i_data,
    "Incorrect number of entries in input cell data array.");

  const size_t resolution = 3;

  const auto subdivide = TSubdivisionIt::New();
  subdivide->SetResolutionLevels( resolution );
  if (!uniform) {
    subdivide->SetCellsToBeSubdivided( cells_to_subdivide );
  }
  subdivide->SetInput( i_mesh );
  subdivide->Update();

  const auto o_mesh = TQEMesh::New();
  o_mesh->Graft( subdivide->GetOutput() );
  o_mesh->DisconnectPipeline();

  // Assert one CellData entry for each Cell
  const auto o_cell = o_mesh->GetNumberOfCells();
  const auto o_data = o_mesh->GetCellData()->Size();
  itkAssertOrThrowMacro(o_cell == o_data,
    "Incorrect number of entries in output cell data array.");

  // Visualize
  const auto i_polydata = dv::ITKMeshToVTKPolyData<TQEMesh>( i_mesh );
  const auto o_polydata = dv::ITKMeshToVTKPolyData<TQEMesh>( o_mesh );

  std::vector<vtkPolyData*> poly_data_vector;
  poly_data_vector.emplace_back(i_polydata);
  poly_data_vector.emplace_back(o_polydata);
  dv::QuickViewMultiplePolyData(poly_data_vector);

}

int main() {

  const unsigned int Dimension = 3;
  using TCoordinate = float;
  using TQEMesh = itk::QuadEdgeMesh<TCoordinate, Dimension>;

  using TButterfly = itk::ModifiedButterflyTriangleCellSubdivisionQuadEdgeMeshFilter< TQEMesh, TQEMesh >;
  using TLinear = itk::LinearTriangleCellSubdivisionQuadEdgeMeshFilter< TQEMesh, TQEMesh >;
  using TLoop = itk::LoopTriangleCellSubdivisionQuadEdgeMeshFilter< TQEMesh, TQEMesh >;
  using TSquare = itk::SquareThreeTriangleCellSubdivisionQuadEdgeMeshFilter< TQEMesh, TQEMesh >;

  SubdivisionTestHelper< TButterfly >(true);
  SubdivisionTestHelper< TLinear >(true);
  SubdivisionTestHelper< TLoop >(true);
  SubdivisionTestHelper< TSquare >(true);

  SubdivisionTestHelper< TButterfly >(false);
  SubdivisionTestHelper< TLinear >(false);
  SubdivisionTestHelper< TLoop >(false);
  SubdivisionTestHelper< TSquare >(false);

  return EXIT_SUCCESS;

}
