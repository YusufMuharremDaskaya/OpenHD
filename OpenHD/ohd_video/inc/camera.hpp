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

#ifndef OPENHD_CAMERA_HPP
#define OPENHD_CAMERA_HPP

#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "camera_info.hpp"
#include "openhd_platform.h"

/**
 * NOTE: This file is copied into QOpenHD to populate the UI.
 */

// For development, always 'works' since fully emulated in SW.
static constexpr int X_CAM_TYPE_DUMMY_SW = 0;  // Dummy sw picture
// Manually feed camera data (encoded,rtp) to openhd. Bitrate control and more
// is not working in this mode, making it only valid for development and in
// extreme cases valid for users that want to use a specific ip camera.
static constexpr int X_CAM_TYPE_EXTERNAL = 2;
// For openhd, this is exactly the same as X_CAM_TYPE_EXTERNAL - only file
// start_ip_cam.txt is created Such that the ip cam service can start forwarding
// data to openhd core.
static constexpr int X_CAM_TYPE_EXTERNAL_IP = 3;
// For development, camera that reads input from a file, and then re-encodes it
// using the platform encoder
static constexpr int X_CAM_TYPE_DEVELOPMENT_FILESRC = 4;
// ... reserved for development / custom cameras

// OpenHD supports any usb camera outputting raw video (with sw encoding).
// H264 usb cameras are not supported, since in general, they do not support
// changing bitrate/ encoding parameters.
static constexpr int X_CAM_TYPE_USB_GENERIC = 10;
// 384x292@25 cam
static constexpr int X_CAM_TYPE_USB_INFIRAY = 11;
// 256x192@25 but only 0x0@0 works (urghs)
static constexpr int X_CAM_TYPE_USB_INFIRAY_T2 = 12;
static constexpr int X_CAM_TYPE_USB_INFIRAY_X2 = 13;
static constexpr int X_CAM_TYPE_USB_INFIRAY_P2_PRO = 14;
static constexpr int X_CAM_TYPE_USB_FLIR_VUE = 15;
static constexpr int X_CAM_TYPE_USB_FLIR_BOSON = 16;
// ... reserved for future (Thermal) USB cameras
//
// RPI Specific starts here
//
// As of now, we have mmal only for the geekworm hdmi to csi adapter
static constexpr int X_CAM_TYPE_RPI_MMAL_HDMI_TO_CSI = 20;
// ... 9 reserved for future use
// ...
// RPIF stands for RPI Foundation (aka original rpi foundation cameras)
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_RPIF_V1_OV5647 = 30;
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_RPIF_V2_IMX219 = 31;
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_RPIF_V3_IMX708 = 32;
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_RPIF_HQ_IMX477 = 33;
// .... 5 reserved for future use
// Now to all the rpi libcamera arducam cameras
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_SKYMASTERHDR_IMX708 = 40;
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_SKYVISIONPRO_IMX519 = 41;
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_IMX477M = 42;
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_IMX462 = 43;
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_IMX327 = 44;
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_IMX290 = 45;
static constexpr int X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_IMX462_LOWLIGHT_MINI = 46;
// ... 13 reserved for future use
static constexpr int X_CAM_TYPE_RPI_V4L2_VEYE_2MP = 60;
static constexpr int X_CAM_TYPE_RPI_V4L2_VEYE_CSIMX307 = 61;
static constexpr int X_CAM_TYPE_RPI_V4L2_VEYE_CSSC132 = 62;
static constexpr int X_CAM_TYPE_RPI_V4L2_VEYE_MVCAM = 63;
// ... 6 reserved for future use
//
// X20 Specific starts here
//
// Right now we only have one camera, but more (might) follow.
// Generic - camera(s) that don't support any IQ params or changing settings.
// For example the Foxeer cameras, or old runcam cameras
static constexpr int X_CAM_TYPE_X20_HDZERO_GENERIC = 70;
static constexpr int X_CAM_TYPE_X20_HDZERO_RUNCAM_V1 = 71;
static constexpr int X_CAM_TYPE_X20_HDZERO_RUNCAM_V2 = 72;
static constexpr int X_CAM_TYPE_X20_HDZERO_RUNCAM_V3 = 73;
static constexpr int X_CAM_TYPE_X20_HDZERO_RUNCAM_NANO_90 = 74;
static constexpr int X_CAM_TYPE_X20_OHD_Jaguar = 75;
static constexpr int X_CAM_TYPE_X21_OHD_Jaguar = 76;

// ... 9 reserved for future use
//
// ROCK 5 starts here
//
static constexpr int X_CAM_TYPE_ROCK_5_HDMI_IN = 80;
static constexpr int X_CAM_TYPE_ROCK_5_OV5647 = 81;
static constexpr int X_CAM_TYPE_ROCK_5_IMX219 = 82;
static constexpr int X_CAM_TYPE_ROCK_5_IMX708 = 83;
static constexpr int X_CAM_TYPE_ROCK_5_IMX462 = 84;
static constexpr int X_CAM_TYPE_ROCK_5_IMX415 = 85;
static constexpr int X_CAM_TYPE_ROCK_5_IMX477 = 86;
static constexpr int X_CAM_TYPE_ROCK_5_IMX519 = 87;
static constexpr int X_CAM_TYPE_ROCK_5_OHD_Jaguar = 88;
// ROCK 3 starts here
//
static constexpr int X_CAM_TYPE_ROCK_3_HDMI_IN = 90;
static constexpr int X_CAM_TYPE_ROCK_3_OV5647 = 91;
static constexpr int X_CAM_TYPE_ROCK_3_IMX219 = 92;
static constexpr int X_CAM_TYPE_ROCK_3_IMX708 = 93;
static constexpr int X_CAM_TYPE_ROCK_3_IMX462 = 94;
static constexpr int X_CAM_TYPE_ROCK_3_IMX519 = 95;
static constexpr int X_CAM_TYPE_ROCK_3_OHD_Jaguar = 96;
static constexpr int X_CAM_TYPE_ROCK_3_VEYE = 97;
//
// NVIDIA XAVIER specific starts here
static constexpr int X_CAM_TYPE_NVIDIA_XAVIER_IMX577 = 101;
// OpenIPC specific starts here
static constexpr int X_CAM_TYPE_OPENIPC_GENERIC = 110;
// Qualcomm specific starts here
static constexpr int X_CAM_TYPE_QC_IMX577 = 120;
static constexpr int X_CAM_TYPE_QC_OV9282 = 121;
// Projects are here
static constexpr int X_CAM_TYPE_WILLY_HORNET = 122;
static constexpr int X_CAM_TYPE_WILLY_JAGUAR = 123;
static constexpr int X_CAM_TYPE_WILLY_REKINDLE = 124;

//
// ... rest is reserved for future use
// no camera, only exists to have a default value for secondary camera (which is
// disabled by default). NOTE: The primary camera cannot be disabled !
static constexpr int X_CAM_TYPE_DISABLED = 255;  // Max for uint8_t

static std::string x_cam_type_to_string(int camera_type) {
  // Use CameraRegistry for lookup - encapsulates camera name information
  const auto& registry = CameraRegistry::instance();
  const auto* info = registry.get_camera_info(camera_type);
  if (info) {
    return info->get_name();
  }
  std::stringstream ss;
  ss << "UNKNOWN (" << camera_type << ")";
  return ss.str();
};

// ResolutionFramerate is now defined in camera_info.hpp

struct XCamera {
  int camera_type = X_CAM_TYPE_DUMMY_SW;
  // 0 for primary camera, 1 for secondary camera
  int index;
  // Only valid if camera is of type USB
  // For CSI camera(s) we in general 'know' from platform and cam type how to
  // tell the pipeline which cam/source to use.
  int usb_v4l2_device_number;

  // Pipeline type checks - use CameraRegistry for encapsulated lookup
  bool requires_rpi_mmal_pipeline() const {
    const auto* info = CameraRegistry::instance().get_camera_info(camera_type);
    return info && info->requires_rpi_mmal_pipeline();
  }
  bool requires_rpi_libcamera_pipeline() const {
    const auto* info = CameraRegistry::instance().get_camera_info(camera_type);
    return info && info->requires_rpi_libcamera_pipeline();
  }
  bool requires_x20_cedar_pipeline() const {
    const auto* info = CameraRegistry::instance().get_camera_info(camera_type);
    return info && info->requires_x20_cedar_pipeline();
  }
  bool requires_rpi_veye_pipeline() const {
    const auto* info = CameraRegistry::instance().get_camera_info(camera_type);
    return info && info->requires_rpi_veye_pipeline();
  }
  bool x20_supports_basic_iq_params() const {
    const auto* info = CameraRegistry::instance().get_camera_info(camera_type);
    return info && info->requires_x20_cedar_pipeline() && info->supports_iq_params();
  }
  bool requires_rockchip5_mpp_pipeline() const {
    const auto* info = CameraRegistry::instance().get_camera_info(camera_type);
    return info && info->requires_rockchip5_mpp_pipeline();
  }
  bool requires_rockchip3_mpp_pipeline() const {
    const auto* info = CameraRegistry::instance().get_camera_info(camera_type);
    return info && info->requires_rockchip3_mpp_pipeline();
  }
  bool requires_willy_pipeline() const {
    const auto* info = CameraRegistry::instance().get_camera_info(camera_type);
    return info && info->requires_willy_pipeline();
  }
  std::string cam_type_as_verbose_string() const {
    return x_cam_type_to_string(camera_type);
  }
  bool is_camera_type_usb_infiray() const {
    return camera_type == X_CAM_TYPE_USB_INFIRAY ||
           camera_type == X_CAM_TYPE_USB_INFIRAY_T2 ||
           camera_type == X_CAM_TYPE_USB_INFIRAY_P2_PRO ||
           camera_type == X_CAM_TYPE_USB_FLIR_VUE ||
           camera_type == X_CAM_TYPE_USB_FLIR_BOSON ||
           camera_type == X_CAM_TYPE_USB_INFIRAY_X2;
  };
  // Returns a list of known supported resolution(s).
  // They should be ordered in ascending resolution / framerate
  // Must always return at least one resolution
  // Might not return all resolutions a camera supports per HW
  // (In qopenhd, we have the experiment checkbox, where the user can enter
  // anything he likes)
  std::vector<ResolutionFramerate> get_supported_resolutions() const {
    // Use CameraRegistry for lookup - encapsulates resolution information
    return CameraRegistry::instance().get_resolutions(camera_type);
  }
  // We default to the last supported resolution
  [[nodiscard]] ResolutionFramerate get_default_resolution_fps() const {
    auto supported_resolutions = get_supported_resolutions();
    return supported_resolutions.at(supported_resolutions.size() - 1);
  }
};

static bool is_rpi_csi_camera(int cam_type) {
  const auto* info = CameraRegistry::instance().get_camera_info(cam_type);
  return info && info->is_rpi_csi_camera();
}
static bool is_rock_csi_camera(int cam_type) {
  const auto* info = CameraRegistry::instance().get_camera_info(cam_type);
  return info && info->is_rock_csi_camera();
}
static bool is_willy_csi_camera(int cam_type) {
  const auto* info = CameraRegistry::instance().get_camera_info(cam_type);
  return info && info->get_family() == CameraFamily::WILLY;
}
static bool is_usb_camera(int cam_type) {
  const auto* info = CameraRegistry::instance().get_camera_info(cam_type);
  return info && info->is_usb_camera();
}

static bool is_valid_primary_cam_type(int cam_type) {
  if (cam_type >= 0 && cam_type < X_CAM_TYPE_DISABLED) return true;
  return false;
}
static bool is_valid_secondary_cam_type(int cam_type) {
  if (is_usb_camera(cam_type)) return true;
  if (cam_type == X_CAM_TYPE_DUMMY_SW || cam_type == X_CAM_TYPE_EXTERNAL ||
      cam_type == X_CAM_TYPE_EXTERNAL_IP || cam_type == X_CAM_TYPE_DISABLED) {
    return true;
  }
  return false;
}
// Takes a string in the from {width}x{height}@{framerate}
// e.g. 1280x720@30
static std::optional<ResolutionFramerate> parse_video_format(
    const std::string& videoFormat) {
  // 0x0@0 is a valid resolution (omit resolution / fps in the pipeline)
  if (videoFormat == "0x0@0") return ResolutionFramerate{0, 0, 0};
  // Otherwise, we need at least 6 characters (0x0@0 is 5 characters)
  if (videoFormat.size() <= 5) {
    return std::nullopt;
  }
  ResolutionFramerate tmp_video_format{0, 0, 0};
  const std::regex reg{R"((\d*)x(\d*)\@(\d*))"};
  std::smatch result;
  if (std::regex_search(videoFormat, result, reg)) {
    if (result.size() == 4) {
      // openhd::log::get_default()->debug("result[0]=["+result[0].str()+"]");
      tmp_video_format.width_px = atoi(result[1].str().c_str());
      tmp_video_format.height_px = atoi(result[2].str().c_str());
      tmp_video_format.fps = atoi(result[3].str().c_str());
      return tmp_video_format;
    }
  }
  return std::nullopt;
}

//
// Used in QOpenHD UI
//
// Lookup table for resolution names (width, height) -> display name
namespace resolution_names {
struct ResolutionKey {
  int width;
  int height;
  bool operator==(const ResolutionKey& other) const {
    return width == other.width && height == other.height;
  }
};

struct ResolutionKeyHash {
  std::size_t operator()(const ResolutionKey& key) const {
    return std::hash<int>()(key.width) ^ (std::hash<int>()(key.height) << 1);
  }
};

static const std::unordered_map<ResolutionKey, std::string, ResolutionKeyHash>
    kResolutionNames = {
        {{640, 480}, "VGA 4:3"},   {{848, 480}, "VGA 16:9"},
        {{896, 504}, "SD 16:9"},   {{1280, 720}, "HD 16:9"},
        {{1920, 1080}, "FHD 16:9"}, {{2560, 1440}, "2K 16:9"},
};
}  // namespace resolution_names

static std::string get_verbose_string_of_resolution(
    const ResolutionFramerate& resolution_framerate) {
  if (resolution_framerate.width_px == 0 &&
      resolution_framerate.height_px == 0 && resolution_framerate.fps == 0) {
    return "AUTO";
  }
  std::stringstream ss;
  const resolution_names::ResolutionKey key{resolution_framerate.width_px,
                                             resolution_framerate.height_px};
  auto it = resolution_names::kResolutionNames.find(key);
  if (it != resolution_names::kResolutionNames.end()) {
    ss << it->second;
  } else {
    ss << resolution_framerate.width_px << "x"
       << resolution_framerate.height_px;
  }
  ss << "\n" << resolution_framerate.fps << "fps";
  return ss.str();
}

static std::string get_v4l2_device_name_string(int value) {
  std::stringstream ss;
  ss << "/dev/video" << value;
  return ss.str();
}

/**
 * On platforms with many cameras (e.g. rpi) we need a differentiation by
 * manufacturer to make a nice UI - otherwise, the choices are overwhelming.
 * Manufacturer is not really the right name for all categories that result
 * here, but it is 'okay' for the UI in qopenhd.
 */
struct CameraNameAndType {
  std::string name;
  int type;
};
struct ManufacturerForPlatform {
  std::string manufacturer_name;
  std::vector<CameraNameAndType> cameras;
};

// Platform camera choices registry - encapsulates camera availability per platform
namespace platform_camera_choices {

// Common camera sets used across multiple platforms
inline std::vector<CameraNameAndType> get_usb_cameras() {
  return {
      {"INFIRAY USB", X_CAM_TYPE_USB_INFIRAY},
      {"INFIRAY USB T2", X_CAM_TYPE_USB_INFIRAY_T2},
      {"INFIRAY USB P2 Pro", X_CAM_TYPE_USB_INFIRAY_P2_PRO},
      {"INFIRAY USB X2", X_CAM_TYPE_USB_INFIRAY_X2},
      {"FLIR_VUE", X_CAM_TYPE_USB_FLIR_VUE},
      {"FLIR_BOSON", X_CAM_TYPE_USB_FLIR_BOSON},
      {"EXP USB GENERIC", X_CAM_TYPE_USB_GENERIC}};
}

inline ManufacturerForPlatform get_manufacturer_usb() {
  return {"USB", get_usb_cameras()};
}

inline ManufacturerForPlatform get_manufacturer_debug() {
  return {"DEV/DEBUG", {{"Dummy (debug)", 0},
                        {"External (DEV)", 2},
                        {"DEV Filecamera", 4}}};
}

inline ManufacturerForPlatform get_manufacturer_disable() {
  return {"DISABLE", {{"DISABLE", X_CAM_TYPE_DISABLED}}};
}

// Platform-specific camera manufacturers
inline std::vector<ManufacturerForPlatform> get_rpi_cameras() {
  return {
      {"ARDUCAM", {{"SKYMASTERHDR", 40}, {"SKYVISIONPRO", 41}, {"IMX477m", 42},
                   {"IMX462", 43}, {"IMX327", 44}, {"IMX290", 45},
                   {"IMX462_LOWLIGHT_MINI", 46}}},
      {"VEYE", {{"2MP", 60}, {"CSIMX307", 61}, {"CSSC132", 62}, {"MVCAM", 63}}},
      {"RPI FOUNDATION", {{"V1 OV5647", 30}, {"V2 IMX219", 31},
                          {"V3 IMX708", 32}, {"HQ IMX477", 33}}},
      {"HDMI TO CSI", {{"GENERIC HDMI to CSI", 20}}},
      get_manufacturer_usb(),
      get_manufacturer_debug()};
}

inline std::vector<ManufacturerForPlatform> get_x20_cameras() {
  return {
      {"HDZERO", {{"GENERIC", X_CAM_TYPE_X20_HDZERO_GENERIC}}},
      {"RUNCAM", {{"RUNCAM V1", X_CAM_TYPE_X20_HDZERO_RUNCAM_V1},
                  {"RUNCAM V2", X_CAM_TYPE_X20_HDZERO_RUNCAM_V2},
                  {"RUNCAM V3", X_CAM_TYPE_X20_HDZERO_RUNCAM_V3},
                  {"RUNCAM NANO 90", X_CAM_TYPE_X20_HDZERO_RUNCAM_NANO_90},
                  {"OpenHD Jaguar", X_CAM_TYPE_X20_OHD_Jaguar}}}};
}

inline std::vector<ManufacturerForPlatform> get_rock3_cameras() {
  return {
      {"Arducam", {{"IMX462", X_CAM_TYPE_ROCK_3_IMX462},
                   {"IMX519", X_CAM_TYPE_ROCK_3_IMX519},
                   {"IMX708", X_CAM_TYPE_ROCK_3_IMX708},
                   {"OpenHD Jaguar", X_CAM_TYPE_ROCK_3_OHD_Jaguar}}},
      get_manufacturer_usb(),
      get_manufacturer_debug()};
}

inline std::vector<ManufacturerForPlatform> get_rock5a_cameras() {
  return {
      {"Generic", {{"IMX415", X_CAM_TYPE_ROCK_5_IMX415},
                   {"IMX462", X_CAM_TYPE_ROCK_5_IMX462},
                   {"IMX477", X_CAM_TYPE_ROCK_5_IMX477},
                   {"IMX519", X_CAM_TYPE_ROCK_5_IMX519},
                   {"OV5647", X_CAM_TYPE_ROCK_5_OV5647},
                   {"IMX219", X_CAM_TYPE_ROCK_5_IMX219},
                   {"IMX708", X_CAM_TYPE_ROCK_5_IMX708},
                   {"OpenHD Jaguar", X_CAM_TYPE_ROCK_5_OHD_Jaguar}}},
      get_manufacturer_usb(),
      get_manufacturer_debug()};
}

inline std::vector<ManufacturerForPlatform> get_rock5b_cameras() {
  return {
      {"Generic", {{"HDMI IN", X_CAM_TYPE_ROCK_5_HDMI_IN},
                   {"IMX415", X_CAM_TYPE_ROCK_5_IMX415},
                   {"IMX462", X_CAM_TYPE_ROCK_5_IMX462},
                   {"IMX477", X_CAM_TYPE_ROCK_5_IMX477},
                   {"IMX519", X_CAM_TYPE_ROCK_5_IMX519},
                   {"OV5647", X_CAM_TYPE_ROCK_5_OV5647},
                   {"IMX219", X_CAM_TYPE_ROCK_5_IMX219},
                   {"IMX708", X_CAM_TYPE_ROCK_5_IMX708},
                   {"OpenHD Jaguar", X_CAM_TYPE_ROCK_5_OHD_Jaguar}}},
      get_manufacturer_usb(),
      get_manufacturer_debug()};
}

inline std::vector<ManufacturerForPlatform> get_nvidia_cameras() {
  return {
      {"LEOPARD", {{"IMX577", X_CAM_TYPE_NVIDIA_XAVIER_IMX577}}},
      get_manufacturer_usb(),
      get_manufacturer_debug()};
}

inline std::vector<ManufacturerForPlatform> get_willy_cameras() {
  return {
      {"WILLY", {{"HORNET", X_CAM_TYPE_WILLY_HORNET},
                 {"JAGUAR", X_CAM_TYPE_WILLY_JAGUAR},
                 {"REKINDLE", X_CAM_TYPE_WILLY_REKINDLE}}},
      get_manufacturer_usb(),
      get_manufacturer_debug()};
}

inline std::vector<ManufacturerForPlatform> get_x86_cameras() {
  return {get_manufacturer_usb(), get_manufacturer_debug()};
}

inline std::vector<ManufacturerForPlatform> get_default_cameras() {
  return {get_manufacturer_debug()};
}

// Set of platform types that share RPI camera choices
inline bool is_rpi_platform(int platform_type) {
  return platform_type == X_PLATFORM_TYPE_RPI_OLD ||
         platform_type == X_PLATFORM_TYPE_RPI_4 ||
         platform_type == X_PLATFORM_TYPE_RPI_CM4;
}

// Set of platform types that share Rock3 camera choices
inline bool is_rock3_platform(int platform_type) {
  return platform_type == X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_ZERO3W ||
         platform_type == X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_CM3;
}

}  // namespace platform_camera_choices

/**
 * Return: a list of categories for this platform.
 * Each category has a list of valid camera types (for this platform).
 * @param platform_type unique platform type
 * @param is_secondary selection is different for secondary cam,most notably, we
 * only support usb, develop and a 'disabled' type.
 */
static std::vector<ManufacturerForPlatform> get_camera_choices_for_platform(
    int platform_type, bool is_secondary) {
  using namespace platform_camera_choices;

  // Secondary camera only supports USB and debug cameras
  if (is_secondary) {
    return {get_manufacturer_disable(), get_manufacturer_usb(),
            get_manufacturer_debug()};
  }

  // Platform-specific camera choices
  if (is_rpi_platform(platform_type)) {
    return get_rpi_cameras();
  }
  if (platform_type == X_PLATFORM_TYPE_ALWINNER_X20) {
    return get_x20_cameras();
  }
  if (is_rock3_platform(platform_type)) {
    return get_rock3_cameras();
  }
  if (platform_type == X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_A) {
    return get_rock5a_cameras();
  }
  if (platform_type == X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_B) {
    return get_rock5b_cameras();
  }
  if (platform_type == X_PLATFORM_TYPE_X86) {
    return get_x86_cameras();
  }
  if (platform_type == X_PLATFORM_TYPE_NVIDIA_XAVIER) {
    return get_nvidia_cameras();
  }
  if (platform_type == X_PLATFORM_TYPE_WILLY) {
    return get_willy_cameras();
  }

  return get_default_cameras();
}

#endif  // OPENHD_CAMERA_HPP
