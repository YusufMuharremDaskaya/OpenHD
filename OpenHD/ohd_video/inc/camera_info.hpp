/******************************************************************************
 * OpenHD
 *
 * Licensed under the GNU General Public License (GPL) Version 3.
 *
 * This software is provided "as-is," without warranty of any kind, express or
 * implied, including but not limited to the warranties of merchantability,
 * fitness for a particular purpose, and non-infringement. For details, see the
 * full license in the LICENSE file provided with this source code.
 *
 * Non-Military Use Only:
 * This software and its associated components are explicitly intended for
 * civilian and non-military purposes. Use in any military or defense
 * applications is strictly prohibited unless explicitly and individually
 * licensed otherwise by the OpenHD Team.
 *
 * Contributors:
 * A full list of contributors can be found at the OpenHD GitHub repository:
 * https://github.com/OpenHD
 *
 * © OpenHD, All Rights Reserved.
 ******************************************************************************/

#ifndef OPENHD_CAMERA_INFO_HPP
#define OPENHD_CAMERA_INFO_HPP

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * Resolution and framerate structure - moved here to avoid circular dependency
 */
struct ResolutionFramerate {
  int width_px;
  int height_px;
  int fps;
  std::string as_string() const {
    std::stringstream ss;
    ss << width_px << "x" << height_px << "@" << fps;
    return ss.str();
  }
};

/**
 * Enum for camera pipeline types - determines which pipeline to use for
 * streaming
 */
enum class CameraPipelineType {
  UNKNOWN,
  DUMMY_SW,           // Software dummy for testing
  EXTERNAL,           // External feed
  USB_GENERIC,        // Generic USB camera
  RPI_MMAL,           // Raspberry Pi MMAL pipeline
  RPI_LIBCAMERA,      // Raspberry Pi libcamera pipeline
  RPI_VEYE,           // Raspberry Pi V4L2 VEYE pipeline
  X20_CEDAR,          // Allwinner X20 cedar pipeline
  ROCKCHIP_5_MPP,     // Rockchip 5 MPP pipeline
  ROCKCHIP_3_MPP,     // Rockchip 3 MPP pipeline
  NVIDIA_XAVIER,      // NVIDIA Xavier pipeline
  QUALCOMM,           // Qualcomm pipeline
  WILLY,              // Willy pipeline
  OPENIPC             // OpenIPC pipeline
};

/**
 * Enum for camera family - groups cameras by platform/manufacturer
 */
enum class CameraFamily {
  DEVELOPMENT,   // Dummy, external, debug cameras
  USB,           // USB cameras (thermal, generic)
  RPI_CSI,       // Raspberry Pi CSI cameras
  X20,           // Allwinner X20 cameras
  ROCKCHIP_5,    // Rockchip 5 cameras
  ROCKCHIP_3,    // Rockchip 3 cameras
  NVIDIA,        // NVIDIA cameras
  QUALCOMM,      // Qualcomm cameras
  WILLY,         // Willy project cameras
  OPENIPC        // OpenIPC cameras
};

/**
 * Base class containing all camera information.
 * Each camera type is represented by an instance of this class with its
 * properties encapsulated.
 */
class CameraInfo {
 public:
  CameraInfo(int type, const std::string& name, CameraFamily family,
             CameraPipelineType pipeline,
             const std::vector<ResolutionFramerate>& resolutions,
             bool supports_iq_params = false)
      : m_type(type),
        m_name(name),
        m_family(family),
        m_pipeline(pipeline),
        m_resolutions(resolutions),
        m_supports_iq_params(supports_iq_params) {}

  virtual ~CameraInfo() = default;

  // Getters
  [[nodiscard]] int get_type() const { return m_type; }
  [[nodiscard]] const std::string& get_name() const { return m_name; }
  [[nodiscard]] CameraFamily get_family() const { return m_family; }
  [[nodiscard]] CameraPipelineType get_pipeline() const { return m_pipeline; }
  [[nodiscard]] const std::vector<ResolutionFramerate>& get_resolutions() const {
    return m_resolutions;
  }
  [[nodiscard]] bool supports_iq_params() const { return m_supports_iq_params; }

  // Pipeline type checks - derived from pipeline type
  [[nodiscard]] bool requires_rpi_mmal_pipeline() const {
    return m_pipeline == CameraPipelineType::RPI_MMAL;
  }
  [[nodiscard]] bool requires_rpi_libcamera_pipeline() const {
    return m_pipeline == CameraPipelineType::RPI_LIBCAMERA;
  }
  [[nodiscard]] bool requires_rpi_veye_pipeline() const {
    return m_pipeline == CameraPipelineType::RPI_VEYE;
  }
  [[nodiscard]] bool requires_x20_cedar_pipeline() const {
    return m_pipeline == CameraPipelineType::X20_CEDAR;
  }
  [[nodiscard]] bool requires_rockchip5_mpp_pipeline() const {
    return m_pipeline == CameraPipelineType::ROCKCHIP_5_MPP;
  }
  [[nodiscard]] bool requires_rockchip3_mpp_pipeline() const {
    return m_pipeline == CameraPipelineType::ROCKCHIP_3_MPP;
  }
  [[nodiscard]] bool requires_willy_pipeline() const {
    return m_pipeline == CameraPipelineType::WILLY;
  }

  // Family checks
  [[nodiscard]] bool is_usb_camera() const {
    return m_family == CameraFamily::USB;
  }
  [[nodiscard]] bool is_rpi_csi_camera() const {
    return m_family == CameraFamily::RPI_CSI;
  }
  [[nodiscard]] bool is_rock_csi_camera() const {
    return m_family == CameraFamily::ROCKCHIP_5 ||
           m_family == CameraFamily::ROCKCHIP_3;
  }

 protected:
  int m_type;
  std::string m_name;
  CameraFamily m_family;
  CameraPipelineType m_pipeline;
  std::vector<ResolutionFramerate> m_resolutions;
  bool m_supports_iq_params;
};

/**
 * Camera registry - singleton that stores all camera definitions and provides
 * lookup functions.
 */
class CameraRegistry {
 public:
  static CameraRegistry& instance() {
    static CameraRegistry instance;
    return instance;
  }

  // Get camera info by type, returns nullptr if not found
  [[nodiscard]] const CameraInfo* get_camera_info(int camera_type) const {
    auto it = m_cameras.find(camera_type);
    if (it != m_cameras.end()) {
      return it->second.get();
    }
    return nullptr;
  }

  // Get camera name by type
  [[nodiscard]] std::string get_camera_name(int camera_type) const {
    const auto* info = get_camera_info(camera_type);
    if (info) {
      return info->get_name();
    }
    return "UNKNOWN";
  }

  // Get supported resolutions for a camera type
  [[nodiscard]] std::vector<ResolutionFramerate> get_resolutions(
      int camera_type) const;

  // Register a camera - called during initialization
  void register_camera(std::unique_ptr<CameraInfo> camera) {
    m_cameras[camera->get_type()] = std::move(camera);
  }

 private:
  CameraRegistry();  // Private constructor, populates registry
  std::unordered_map<int, std::unique_ptr<CameraInfo>> m_cameras;
};

#endif  // OPENHD_CAMERA_INFO_HPP
