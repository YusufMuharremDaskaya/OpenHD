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

#include "openhd_platform.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <unordered_map>

#include "openhd_spdlog.h"
#include "openhd_util.h"
#include "openhd_util_filesystem.h"

// Constants
static constexpr auto NVIDIA_XAVIER_BOARDID_PATH =
    "/proc/device-tree/nvidia,dtsfilename";
static constexpr auto DEVICE_TREE_COMPATIBLE_PATH =
    "/proc/device-tree/compatible";
static constexpr auto ALLWINNER_BOARDID_PATH = "/dev/cedar_dev";
static constexpr auto SIGMASTAR_BOARDID_PATH = "/dev/mstar_ive0";
static constexpr auto QUALCOMM_BOARDID_PATH = "/proc/device-tree/model";

static int internal_discover_platform() {
  openhd::log::get_default()->warn("OpenHD Platform Discovery started!");

  if (OHDFilesystemUtil::exists("/proc/device-tree/model")) {
    const std::string model_content =
        OHDFilesystemUtil::read_file("/proc/device-tree/model");
    if (OHDUtil::contains_after_uppercase(model_content, "ORQA")) {
      openhd::log::get_default()->warn("Detected Willy platform.");
      return X_PLATFORM_TYPE_WILLY;
    }
  }
  if (OHDFilesystemUtil::exists(ALLWINNER_BOARDID_PATH)) {
    openhd::log::get_default()->warn("Detected Allwinner platform (X20).");
    return X_PLATFORM_TYPE_ALWINNER_X20;
  }

  if (OHDFilesystemUtil::exists("/boot/config.txt")) {
    openhd::log::get_default()->warn(
        "Detected potential Raspberry Pi platform.");
    const auto filename_proc_cpuinfo = "/proc/cpuinfo";
    const auto proc_cpuinfo_opt =
        OHDFilesystemUtil::opt_read_file("/proc/cpuinfo");

    if (!proc_cpuinfo_opt.has_value()) {
      openhd::log::get_default()->warn(
          "File {} does not exist. Unable to complete Raspberry Pi detection.",
          filename_proc_cpuinfo);
      return X_PLATFORM_TYPE_RPI_OLD;
    }

    openhd::log::get_default()->warn("Checking Raspberry Pi hardware...");
    if (OHDUtil::contains(proc_cpuinfo_opt.value(), "BCM2711")) {
      openhd::log::get_default()->warn("Raspberry Pi 4 detected.");
      return X_PLATFORM_TYPE_RPI_4;
    }

    openhd::log::get_default()->warn("Detected an older Raspberry Pi (<=3).");
    return X_PLATFORM_TYPE_RPI_OLD;
  }

  if (OHDFilesystemUtil::exists(SIGMASTAR_BOARDID_PATH)) {
    openhd::log::get_default()->warn("Detected SigmaStar platform.");
    return X_PLATFORM_TYPE_OPENIPC_SIGMASTAR_UNDEFINED;
  }

  if (OHDFilesystemUtil::exists(DEVICE_TREE_COMPATIBLE_PATH)) {
    openhd::log::get_default()->warn("Checking for Rockchip platforms...");

    const std::string compatible_content =
        OHDFilesystemUtil::read_file(DEVICE_TREE_COMPATIBLE_PATH);
    const std::string device_tree_model =
        OHDFilesystemUtil::read_file("/proc/device-tree/model");
    std::regex r("rockchip,(r[kv][0-9]+)");
    std::smatch sm;

    // Simple chip to platform type mapping
    static const std::unordered_map<std::string, int> kSimpleChipMapping = {
        {"rv1126", X_PLATFORM_TYPE_ROCKCHIP_RV1126},
        {"rv1103", X_PLATFORM_TYPE_ROCKCHIP_RV1103},
        {"rv1106", X_PLATFORM_TYPE_ROCKCHIP_RV1106},
        {"rk3506", X_PLATFORM_TYPE_LUCKFOX_LYRA},
    };

    if (regex_search(compatible_content, sm, r)) {
      const std::string chip = sm[1];
      openhd::log::get_default()->warn("Rockchip chip identified: {}", chip);

      // Check simple mappings first
      auto simple_it = kSimpleChipMapping.find(chip);
      if (simple_it != kSimpleChipMapping.end()) {
        openhd::log::get_default()->warn("Detected Rockchip {}.",
                                         x_platform_type_to_string(simple_it->second));
        return simple_it->second;
      }

      // Handle RK3588 variants
      if (chip == "rk3588") {
        if (OHDUtil::contains_after_uppercase(device_tree_model,
                                              "Radxa ROCK 5A")) {
          openhd::log::get_default()->warn(
              "Detected Rockchip RK3588 (Radxa ROCK 5A).");
          return X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_A;
        }
        openhd::log::get_default()->warn(
            "Detected Rockchip RK3588 (Radxa ROCK 5B).");
        return X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_B;
      }

      // Handle RK3566 variants
      if (chip == "rk3566") {
        if (OHDUtil::contains_after_uppercase(device_tree_model,
                                              "Radxa CM3 RPI CM4 IO")) {
          openhd::log::get_default()->warn(
              "Detected Rockchip RK3566 (Radxa CM3).");
          return X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_CM3;
        }
        // Default to ZERO3W for other RK3566 boards
        openhd::log::get_default()->warn(
            "Detected Rockchip RK3566 (default Radxa ZERO3W).");
        return X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_ZERO3W;
      }
    }

    openhd::log::get_default()->warn("No specific Rockchip match found.");
    return X_PLATFORM_TYPE_UNKNOWN;
  }

  if (OHDFilesystemUtil::exists(NVIDIA_XAVIER_BOARDID_PATH)) {
    openhd::log::get_default()->warn("Detected NVIDIA Xavier platform.");
    return X_PLATFORM_TYPE_NVIDIA_XAVIER;
  }

  if (OHDFilesystemUtil::exists(QUALCOMM_BOARDID_PATH)) {
    openhd::log::get_default()->warn("Checking for Qualcomm platforms...");
    const std::string qualcomm_board_id_content =
        OHDFilesystemUtil::read_file(QUALCOMM_BOARDID_PATH);

    // Qualcomm chip to platform type mapping
    static const std::unordered_map<std::string, int> kQualcommChipMapping = {
        {"qcs405", X_PLATFORM_TYPE_QUALCOMM_QCS405},
        {"qrb5165", X_PLATFORM_TYPE_QUALCOMM_QRB5165},
    };

    std::regex qualcomm_regex("(qcs405|qrb5165)");
    std::smatch match;

    if (std::regex_search(qualcomm_board_id_content, match, qualcomm_regex)) {
      const std::string chip = match[1];
      auto it = kQualcommChipMapping.find(chip);
      if (it != kQualcommChipMapping.end()) {
        openhd::log::get_default()->warn("Detected Qualcomm {}.", chip);
        return it->second;
      }
    }

    openhd::log::get_default()->warn("No specific Qualcomm match found.");
    return X_PLATFORM_TYPE_UNKNOWN;
  }

  const auto arch_opt = OHDUtil::run_command_out("arch");

  if (!arch_opt.has_value()) {
    openhd::log::get_default()->warn("Could not determine architecture.");
    return X_PLATFORM_TYPE_UNKNOWN;
  }

  const auto arch = arch_opt.value();
  openhd::log::get_default()->warn("Architecture detected: {}", arch);

  if (std::regex_search(arch, std::regex{"x86_64"})) {
    openhd::log::get_default()->warn("Detected x86 platform.");
    return X_PLATFORM_TYPE_X86;
  }

  openhd::log::get_default()->warn("Unknown platform.");
  return X_PLATFORM_TYPE_UNKNOWN;
}

static void write_platform_manifest(const OHDPlatform& ohdPlatform) {
  static constexpr auto PLATFORM_MANIFEST_FILENAME =
      "/tmp/platform_manifest.txt";
  OHDFilesystemUtil::write_file(PLATFORM_MANIFEST_FILENAME,
                                ohdPlatform.to_string());
}

static OHDPlatform discover_and_write_manifest() {
  auto platform_int = internal_discover_platform();
  auto platform = OHDPlatform(platform_int);
  write_platform_manifest(platform);
  return platform;
}

// Lookup table for platform type names
static const std::unordered_map<int, std::string> kPlatformTypeNames = {
    {X_PLATFORM_TYPE_UNKNOWN, "UNKNOWN"},
    {X_PLATFORM_TYPE_X86, "X86"},
    {X_PLATFORM_TYPE_RPI_OLD, "RPI<=3"},
    {X_PLATFORM_TYPE_RPI_4, "RPI 4"},
    {X_PLATFORM_TYPE_RPI_5, "RPI 5"},
    {X_PLATFORM_TYPE_RPI_CM4, "RPI CM4"},
    {X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_ZERO3W, "RADXA ZERO3W"},
    {X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_CM3, "RADXA CM3"},
    {X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_A, "RADXA RK3588S"},
    {X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_B, "RADXA RK3588"},
    {X_PLATFORM_TYPE_ROCKCHIP_RV1126, "RV1126"},
    {X_PLATFORM_TYPE_ROCKCHIP_RV1103, "RV1103"},
    {X_PLATFORM_TYPE_ROCKCHIP_RV1106, "RV1106"},
    {X_PLATFORM_TYPE_LUCKFOX_LYRA, "LUCKFOX LYRA"},
    {X_PLATFORM_TYPE_WILLY, "Willy"},
    {X_PLATFORM_TYPE_ALWINNER_X20, "X20"},
    {X_PLATFORM_TYPE_OPENIPC_SIGMASTAR_UNDEFINED, "OPENIPC SIGMASTAR"},
    {X_PLATFORM_TYPE_NVIDIA_XAVIER, "NVIDIA_XAVIER"},
    {X_PLATFORM_TYPE_QUALCOMM_QCS405, "QUALCOMM_QCS405"},
    {X_PLATFORM_TYPE_QUALCOMM_QRB5165, "QUALCOMM_QRB5165"},
};

std::string x_platform_type_to_string(int platform_type) {
  auto it = kPlatformTypeNames.find(platform_type);
  if (it != kPlatformTypeNames.end()) {
    return it->second;
  }
  std::stringstream ss;
  ss << "ERR-UNDEFINED{" << platform_type << "}";
  return ss.str();
}

// Lookup table for FEC max block size per platform type
static const std::unordered_map<int, int> kFecMaxBlockSizeByPlatform = {
    {X_PLATFORM_TYPE_RPI_4, 50},
    {X_PLATFORM_TYPE_RPI_CM4, 50},
    {X_PLATFORM_TYPE_RPI_OLD, 30},
    {X_PLATFORM_TYPE_X86, 80},
    {X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_ZERO3W, 20},
    {X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_CM3, 20},
    {X_PLATFORM_TYPE_ROCKCHIP_RV1103, 20},
    {X_PLATFORM_TYPE_ROCKCHIP_RV1106, 20},
    {X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_A, 20},
    {X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_B, 20},
    {X_PLATFORM_TYPE_ALWINNER_X20, 20},
    {X_PLATFORM_TYPE_NVIDIA_XAVIER, 50},
    {X_PLATFORM_TYPE_WILLY, 50},
    {X_PLATFORM_TYPE_QUALCOMM_QRB5165, 50},
    {X_PLATFORM_TYPE_QUALCOMM_QCS405, 50},
};

static constexpr int kDefaultFecMaxBlockSize = 20;

int get_fec_max_block_size_for_platform() {
  const auto platform_type = OHDPlatform::instance().platform_type;
  const auto it = kFecMaxBlockSizeByPlatform.find(platform_type);
  if (it != kFecMaxBlockSizeByPlatform.end()) {
    return it->second;
  }
  return kDefaultFecMaxBlockSize;
}

// OHDPlatform methods
const OHDPlatform& OHDPlatform::instance() {
  static OHDPlatform instance = discover_and_write_manifest();
  return instance;
}

std::string OHDPlatform::to_string() const {
  std::stringstream ss;
  ss << "OHDPlatform:[" << x_platform_type_to_string(platform_type) << "]";
  return ss.str();
}

// Platform type sets for clearer categorization (replacing magic number ranges)
namespace platform_categories {
static const std::set<int> kRpiPlatforms = {
    X_PLATFORM_TYPE_RPI_OLD,
    X_PLATFORM_TYPE_RPI_4,
    X_PLATFORM_TYPE_RPI_CM4,
    X_PLATFORM_TYPE_RPI_5,
};

static const std::set<int> kRockchipPlatforms = {
    X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_ZERO3W,
    X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_A,
    X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_B,
    X_PLATFORM_TYPE_ROCKCHIP_RV1126,
    X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_CM3,
    X_PLATFORM_TYPE_ROCKCHIP_RV1106,
    X_PLATFORM_TYPE_ROCKCHIP_RV1103,
    X_PLATFORM_TYPE_LUCKFOX_LYRA,
};
}  // namespace platform_categories

bool OHDPlatform::is_rpi() const {
  return platform_categories::kRpiPlatforms.count(platform_type) > 0;
}

bool OHDPlatform::is_rock() const {
  return platform_categories::kRockchipPlatforms.count(platform_type) > 0;
}

bool OHDPlatform::is_rpi_or_x86() const {
  return is_rpi() || platform_type == X_PLATFORM_TYPE_X86;
}

bool OHDPlatform::is_x20() const {
  return platform_type == X_PLATFORM_TYPE_ALWINNER_X20;
}

bool OHDPlatform::is_willy() const {
  return platform_type == X_PLATFORM_TYPE_WILLY;
}

bool OHDPlatform::is_zero3w() const {
  return platform_type == X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_ZERO3W;
}

bool OHDPlatform::is_radxa_cm3() const {
  return platform_type == X_PLATFORM_TYPE_ROCKCHIP_RK3566_RADXA_CM3;
}

bool OHDPlatform::is_rock5_a() const {
  return platform_type == X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_A;
}

bool OHDPlatform::is_rock5_b() const {
  return platform_type == X_PLATFORM_TYPE_ROCKCHIP_RK3588_RADXA_ROCK5_B;
}

bool OHDPlatform::is_rock5_a_b() const { return is_rock5_a() || is_rock5_b(); }

bool OHDPlatform::is_qcs405() const {
  return platform_type == X_PLATFORM_TYPE_QUALCOMM_QCS405;
}

bool OHDPlatform::is_qrb5165() const {
  return platform_type == X_PLATFORM_TYPE_QUALCOMM_QRB5165;
}
