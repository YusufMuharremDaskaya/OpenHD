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

#include "camera_info.hpp"
#include "camera.hpp"

// Helper to create resolution vectors
static std::vector<ResolutionFramerate> make_resolutions(
    std::initializer_list<ResolutionFramerate> list) {
  return std::vector<ResolutionFramerate>(list);
}

std::vector<ResolutionFramerate> CameraRegistry::get_resolutions(
    int camera_type) const {
  const auto* info = get_camera_info(camera_type);
  if (info) {
    return info->get_resolutions();
  }
  // Default fallback
  return {{640, 480, 30}};
}

CameraRegistry::CameraRegistry() {
  // ============================================================
  // Development/Debug Cameras
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_DUMMY_SW, "DUMMY", CameraFamily::DEVELOPMENT,
      CameraPipelineType::DUMMY_SW,
      make_resolutions({{640, 480, 30}, {1280, 720, 30}, {1280, 720, 60}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_EXTERNAL, "EXTERNAL", CameraFamily::DEVELOPMENT,
      CameraPipelineType::EXTERNAL, make_resolutions({{640, 480, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_EXTERNAL_IP, "EXTERNAL_IP", CameraFamily::DEVELOPMENT,
      CameraPipelineType::EXTERNAL, make_resolutions({{640, 480, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_DEVELOPMENT_FILESRC, "DEV_FILESRC", CameraFamily::DEVELOPMENT,
      CameraPipelineType::DUMMY_SW,
      make_resolutions({{848, 480, 60}, {1280, 720, 60}, {1920, 1080, 60}})));

  // ============================================================
  // USB Cameras
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_USB_GENERIC, "USB", CameraFamily::USB,
      CameraPipelineType::USB_GENERIC,
      make_resolutions({{640, 480, 30}, {0, 0, 0}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_USB_INFIRAY, "INFIRAY", CameraFamily::USB,
      CameraPipelineType::USB_GENERIC, make_resolutions({{384, 292, 25}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_USB_INFIRAY_T2, "INFIRAY_T2", CameraFamily::USB,
      CameraPipelineType::USB_GENERIC,
      make_resolutions({{256, 192, 25}, {0, 0, 0}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_USB_INFIRAY_X2, "INFIRAY_X2", CameraFamily::USB,
      CameraPipelineType::USB_GENERIC, make_resolutions({{384, 292, 50}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_USB_INFIRAY_P2_PRO, "INFIRAY_P2_PRO", CameraFamily::USB,
      CameraPipelineType::USB_GENERIC, make_resolutions({{256, 192, 25}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_USB_FLIR_VUE, "FLIR VUE", CameraFamily::USB,
      CameraPipelineType::USB_GENERIC, make_resolutions({{640, 512, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_USB_FLIR_BOSON, "FLIR BOSON", CameraFamily::USB,
      CameraPipelineType::USB_GENERIC, make_resolutions({{640, 512, 60}})));

  // ============================================================
  // RPI CSI Cameras - MMAL
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_MMAL_HDMI_TO_CSI, "MMAL_HDMI", CameraFamily::RPI_CSI,
      CameraPipelineType::RPI_MMAL,
      make_resolutions({{1280, 720, 30}, {1920, 1080, 25}, {1280, 720, 60}})));

  // ============================================================
  // RPI CSI Cameras - Libcamera (RPI Foundation)
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_RPIF_V1_OV5647, "RPIF_V1_OV5647",
      CameraFamily::RPI_CSI, CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions({{1280, 720, 30}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_RPIF_V2_IMX219, "RPIF_V2_IMX219",
      CameraFamily::RPI_CSI, CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions(
          {{640, 480, 47}, {896, 504, 47}, {1280, 720, 47}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_RPIF_V3_IMX708, "RPIF_V3_IMX708",
      CameraFamily::RPI_CSI, CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions(
          {{640, 480, 60}, {896, 504, 60}, {1280, 720, 60}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_RPIF_HQ_IMX477, "RPIF_HQ_IMX477",
      CameraFamily::RPI_CSI, CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions(
          {{640, 480, 50}, {896, 504, 50}, {1280, 720, 50}, {1920, 1080, 30}})));

  // ============================================================
  // RPI CSI Cameras - Libcamera (Arducam)
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_SKYMASTERHDR_IMX708,
      "ARDUCAM_SKYMASTERHDR", CameraFamily::RPI_CSI,
      CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions(
          {{640, 480, 60}, {896, 504, 60}, {1280, 720, 60}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_SKYVISIONPRO_IMX519,
      "ARDUCAM_SKYVISIONPRO", CameraFamily::RPI_CSI,
      CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions(
          {{640, 480, 60}, {896, 504, 60}, {1280, 720, 60}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_IMX477M, "ARDUCAM_IMX477M",
      CameraFamily::RPI_CSI, CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions(
          {{640, 480, 50}, {896, 504, 50}, {1280, 720, 50}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_IMX462, "ARDUCAM_IMX462",
      CameraFamily::RPI_CSI, CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions({{640, 480, 60},
                        {896, 504, 30},
                        {896, 504, 60},
                        {1280, 720, 30},
                        {1280, 720, 60},
                        {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_IMX327, "ARDUCAM_IMX327",
      CameraFamily::RPI_CSI, CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions(
          {{640, 480, 60}, {896, 504, 60}, {1280, 720, 60}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_IMX290, "ARDUCAM_IMX290",
      CameraFamily::RPI_CSI, CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions({{1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_LIBCAMERA_ARDUCAM_IMX462_LOWLIGHT_MINI,
      "ARDUCAM_IMX462_LOWLIGHT_MINI", CameraFamily::RPI_CSI,
      CameraPipelineType::RPI_LIBCAMERA,
      make_resolutions({{640, 480, 60},
                        {896, 504, 30},
                        {896, 504, 60},
                        {1280, 720, 30},
                        {1280, 720, 60},
                        {1920, 1080, 30}})));

  // ============================================================
  // RPI CSI Cameras - VEYE
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_V4L2_VEYE_2MP, "VEYE_2MP", CameraFamily::RPI_CSI,
      CameraPipelineType::RPI_VEYE, make_resolutions({{1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_V4L2_VEYE_CSIMX307, "VEYE_IMX307", CameraFamily::RPI_CSI,
      CameraPipelineType::RPI_VEYE,
      make_resolutions({{1280, 720, 60}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_V4L2_VEYE_CSSC132, "VEYE_CSSC132", CameraFamily::RPI_CSI,
      CameraPipelineType::RPI_VEYE, make_resolutions({{1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_RPI_V4L2_VEYE_MVCAM, "VEYE_MVCAM", CameraFamily::RPI_CSI,
      CameraPipelineType::RPI_VEYE, make_resolutions({{1920, 1080, 30}})));

  // ============================================================
  // X20 Cameras
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_X20_HDZERO_GENERIC, "X20_HDZERO_GENERIC", CameraFamily::X20,
      CameraPipelineType::X20_CEDAR, make_resolutions({{1280, 720, 60}}),
      false));  // No IQ params

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_X20_HDZERO_RUNCAM_V1, "X20_HDZERO_RUNCAM_V1", CameraFamily::X20,
      CameraPipelineType::X20_CEDAR, make_resolutions({{1280, 720, 60}}),
      true));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_X20_HDZERO_RUNCAM_V2, "X20_HDZERO_RUNCAM_V2", CameraFamily::X20,
      CameraPipelineType::X20_CEDAR, make_resolutions({{1280, 720, 60}}),
      true));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_X20_HDZERO_RUNCAM_V3, "X20_HDZERO_RUNCAM_V3", CameraFamily::X20,
      CameraPipelineType::X20_CEDAR, make_resolutions({{1280, 720, 60}}),
      true));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_X20_HDZERO_RUNCAM_NANO_90, "X20_HDZERO_RUNCAM_NANO",
      CameraFamily::X20, CameraPipelineType::X20_CEDAR,
      make_resolutions({{1280, 720, 60}}), true));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_X20_OHD_Jaguar, "X20_OHD_Jaguar", CameraFamily::X20,
      CameraPipelineType::X20_CEDAR, make_resolutions({{1280, 720, 60}}),
      true));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_X21_OHD_Jaguar, "X21_OHD_Jaguar", CameraFamily::X20,
      CameraPipelineType::X20_CEDAR, make_resolutions({{1280, 720, 60}}),
      true));

  // ============================================================
  // Rockchip 5 Cameras
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_5_HDMI_IN, "ROCK_5_HDMI_IN", CameraFamily::ROCKCHIP_5,
      CameraPipelineType::ROCKCHIP_5_MPP,
      make_resolutions({{1280, 720, 30},
                        {1280, 720, 60},
                        {1920, 1080, 30},
                        {1920, 1080, 60},
                        {3840, 2160, 30},
                        {3840, 2160, 60}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_5_OV5647, "ROCK_5_OV5647", CameraFamily::ROCKCHIP_5,
      CameraPipelineType::ROCKCHIP_5_MPP,
      make_resolutions({{640, 480, 60}, {1280, 720, 30}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_5_IMX219, "ROCK_5_IMX219", CameraFamily::ROCKCHIP_5,
      CameraPipelineType::ROCKCHIP_5_MPP,
      make_resolutions(
          {{640, 480, 30}, {848, 480, 30}, {1280, 720, 30}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_5_IMX708, "ROCK_5_IMX708", CameraFamily::ROCKCHIP_5,
      CameraPipelineType::ROCKCHIP_5_MPP,
      make_resolutions({{1280, 720, 30},
                        {1280, 720, 60},
                        {1920, 1080, 60},
                        {4608, 2592, 14}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_5_IMX462, "ROCK_5_IMX462", CameraFamily::ROCKCHIP_5,
      CameraPipelineType::ROCKCHIP_5_MPP,
      make_resolutions({{1280, 720, 30},
                        {1280, 720, 60},
                        {1920, 1080, 30},
                        {1920, 1080, 60}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_5_IMX415, "ROCK_5_IMX415", CameraFamily::ROCKCHIP_5,
      CameraPipelineType::ROCKCHIP_5_MPP,
      make_resolutions({{1280, 720, 30},
                        {1280, 720, 30},
                        {1920, 1080, 30},
                        {3864, 2192, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_5_IMX477, "ROCK_5_IMX477", CameraFamily::ROCKCHIP_5,
      CameraPipelineType::ROCKCHIP_5_MPP,
      make_resolutions({{1280, 720, 60},
                        {1920, 1080, 60},
                        {3840, 2160, 20},
                        {4056, 3040, 10}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_5_IMX519, "ROCK_5_IMX519", CameraFamily::ROCKCHIP_5,
      CameraPipelineType::ROCKCHIP_5_MPP,
      make_resolutions({{1280, 720, 60},
                        {1920, 1080, 60},
                        {3840, 2160, 20},
                        {4056, 3040, 9}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_5_OHD_Jaguar, "ROCK_5_OHD_Jaguar", CameraFamily::ROCKCHIP_5,
      CameraPipelineType::ROCKCHIP_5_MPP,
      make_resolutions(
          {{1280, 720, 120}, {1920, 1080, 100}, {3840, 2160, 90}})));

  // ============================================================
  // Rockchip 3 Cameras
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_3_HDMI_IN, "ROCK_3_HDMI_IN", CameraFamily::ROCKCHIP_3,
      CameraPipelineType::ROCKCHIP_3_MPP,
      make_resolutions({{1280, 720, 30}, {1280, 720, 60}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_3_OV5647, "ROCK_3_OV5647", CameraFamily::ROCKCHIP_3,
      CameraPipelineType::ROCKCHIP_3_MPP,
      make_resolutions({{640, 480, 60}, {1280, 720, 30}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_3_IMX219, "ROCK_3_IMX219", CameraFamily::ROCKCHIP_3,
      CameraPipelineType::ROCKCHIP_3_MPP,
      make_resolutions(
          {{640, 480, 30}, {848, 480, 30}, {1280, 720, 30}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_3_IMX708, "ROCK_3_IMX708", CameraFamily::ROCKCHIP_3,
      CameraPipelineType::ROCKCHIP_3_MPP,
      make_resolutions({{1280, 720, 30}, {1280, 720, 60}, {1920, 1080, 60}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_3_IMX462, "ROCK_3_IMX462", CameraFamily::ROCKCHIP_3,
      CameraPipelineType::ROCKCHIP_3_MPP,
      make_resolutions({{1280, 720, 60},
                        {1280, 720, 30},
                        {1920, 1080, 30},
                        {1920, 1080, 60}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_3_IMX519, "ROCK_3_IMX519", CameraFamily::ROCKCHIP_3,
      CameraPipelineType::ROCKCHIP_3_MPP,
      make_resolutions({{1280, 720, 60},
                        {1280, 720, 30},
                        {1920, 1080, 30},
                        {1920, 1080, 60}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_3_OHD_Jaguar, "ROCK_3_OHD_Jaguar", CameraFamily::ROCKCHIP_3,
      CameraPipelineType::ROCKCHIP_3_MPP,
      make_resolutions({{1280, 720, 60},
                        {1280, 720, 120},
                        {1920, 1080, 60},
                        {1920, 1080, 100}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_ROCK_3_VEYE, "ROCK_3_VEYE", CameraFamily::ROCKCHIP_3,
      CameraPipelineType::ROCKCHIP_3_MPP,
      make_resolutions({{1920, 1080, 30}})));

  // ============================================================
  // NVIDIA Xavier Cameras
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_NVIDIA_XAVIER_IMX577, "XAVIER_IMX577", CameraFamily::NVIDIA,
      CameraPipelineType::NVIDIA_XAVIER,
      make_resolutions({{1280, 720, 60}, {1920, 1080, 60}})));

  // ============================================================
  // OpenIPC Cameras
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_OPENIPC_GENERIC, "OPENIPC_X", CameraFamily::OPENIPC,
      CameraPipelineType::OPENIPC, make_resolutions({{640, 480, 30}})));

  // ============================================================
  // Qualcomm Cameras
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_QC_IMX577, "CORETRONIC IMX577", CameraFamily::QUALCOMM,
      CameraPipelineType::QUALCOMM,
      make_resolutions({{1280, 720, 30}, {1920, 1080, 30}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_QC_OV9282, "CORETRONIC OV9282", CameraFamily::QUALCOMM,
      CameraPipelineType::QUALCOMM, make_resolutions({{1280, 720, 30}})));

  // ============================================================
  // Willy Cameras
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_WILLY_HORNET, "WILLY_HORNET", CameraFamily::WILLY,
      CameraPipelineType::WILLY, make_resolutions({{960, 720, 120}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_WILLY_JAGUAR, "WILLY_JAGUAR", CameraFamily::WILLY,
      CameraPipelineType::WILLY, make_resolutions({{1280, 720, 120}})));

  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_WILLY_REKINDLE, "WILLY_REKINDLE", CameraFamily::WILLY,
      CameraPipelineType::WILLY, make_resolutions({{1280, 720, 60}})));

  // ============================================================
  // Disabled Camera
  // ============================================================
  register_camera(std::make_unique<CameraInfo>(
      X_CAM_TYPE_DISABLED, "DISABLED", CameraFamily::DEVELOPMENT,
      CameraPipelineType::UNKNOWN, make_resolutions({})));
}
