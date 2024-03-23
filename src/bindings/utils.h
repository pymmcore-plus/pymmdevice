#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace util {

/**
 * Converts a buffer to a NumPy array.
 *
 * @param buffer The buffer to convert.
 * @param height The height of the array.
 * @param width The width of the array.
 * @param bytesPerPixel The number of bytes per pixel.
 * @return The NumPy array representing the buffer.
 * @throws std::runtime_error if the bytes per pixel is unsupported.
 */
py::array bufferToNumpy(const unsigned char *buffer, unsigned int height, unsigned int width,
                        unsigned int bytesPerPixel) {
  py::dtype dtype;
  if (bytesPerPixel == 1) {
    dtype = py::dtype::of<uint8_t>();
  } else if (bytesPerPixel == 2) {
    dtype = py::dtype::of<uint16_t>();
  } else if (bytesPerPixel == 4) {
    dtype = py::dtype::of<uint32_t>();
  } else if (bytesPerPixel == 8) {
    dtype = py::dtype::of<uint64_t>();
  } else {
    throw std::runtime_error("Unsupported bytes per pixel: " + std::to_string(bytesPerPixel));
  }
  // Ensure shape and strides are explicitly defined as vectors
  std::vector<ssize_t> shape = {static_cast<ssize_t>(height), static_cast<ssize_t>(width)};
  std::vector<ssize_t> strides = {static_cast<ssize_t>(width * bytesPerPixel),
                                  static_cast<ssize_t>(bytesPerPixel)};

  return py::array(dtype, shape, strides, buffer);
}

/**
 * Resolves the absolute path of a given file or directory.
 *
 * @param path A handle to the file or directory path.
 * @return The absolute path as a std::string.
 */
inline std::string resolvePath(py::handle path) {
  py::object os_path = py::module::import("os.path");
  path = os_path.attr("realpath")(os_path.attr("expanduser")(path));
  std::string path_str = path.cast<std::string>();
  return path_str;
}

}  // namespace util