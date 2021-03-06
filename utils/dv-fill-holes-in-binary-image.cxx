// Boost
#include <boost/program_options.hpp>

namespace po = boost::program_options;

// Custom
#include <dvFillHolesInBinaryImage.h>
#include <dvReadImageIOBase.h>

int
main(int argc, char** argv)
{

  // Declare the supported options.
  po::options_description description("Allowed options");
  description.add_options()("help", "Print usage information.")(
    "input-image",
    po::value<std::string>()->required(),
    "Filename of input image.")("output-image",
                                po::value<std::string>()->required(),
                                "Filename of output image.");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, description), vm);

  if (vm.count("help") || 1 == argc) {
    std::cout << description << '\n';
    return EXIT_SUCCESS;
  }

  po::notify(vm);

  const std::string IImage = vm["input-image"].as<std::string>();
  const std::string OImage = vm["output-image"].as<std::string>();

  switch (dv::ReadImageIOBase(IImage)->GetComponentType()) {
    case itk::ImageIOBase::UCHAR:
      dv::FillHolesInBinaryImage<3, unsigned char>(IImage, OImage);
      break;
    case itk::ImageIOBase::CHAR:
      dv::FillHolesInBinaryImage<3, char>(IImage, OImage);
      break;
    case itk::ImageIOBase::USHORT:
      dv::FillHolesInBinaryImage<3, unsigned short>(IImage, OImage);
      break;
    case itk::ImageIOBase::SHORT:
      dv::FillHolesInBinaryImage<3, short>(IImage, OImage);
      break;
    case itk::ImageIOBase::UINT:
      dv::FillHolesInBinaryImage<3, unsigned int>(IImage, OImage);
      break;
    case itk::ImageIOBase::INT:
      dv::FillHolesInBinaryImage<3, int>(IImage, OImage);
      break;
    case itk::ImageIOBase::ULONG:
      dv::FillHolesInBinaryImage<3, unsigned long>(IImage, OImage);
      break;
    case itk::ImageIOBase::LONG:
      dv::FillHolesInBinaryImage<3, long>(IImage, OImage);
      break;
    case itk::ImageIOBase::FLOAT:
      dv::FillHolesInBinaryImage<3, float>(IImage, OImage);
      break;
    case itk::ImageIOBase::DOUBLE:
      dv::FillHolesInBinaryImage<3, double>(IImage, OImage);
      break;
    default:
      std::cerr << "ERROR: Unrecognized pixel type." << std::endl;
      return EXIT_FAILURE;
      break;
  }

  return EXIT_SUCCESS;
}
