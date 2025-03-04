//src/drivelists.hpp

#ifndef SRC_DRIVELIST_HPP_
#define SRC_DRIVELIST_HPP_

#include <napi.h>
#include <string>
#include <vector>

namespace Drivelists {

struct MountPoint {
  std::string path;
};

struct DeviceDescriptor {
  std::string enumerator;
  std::string busType;
  std::string busVersion;
  bool busVersionNull;
  std::string device;
  std::string devicePath;
  bool devicePathNull;
  std::string raw;
  std::string description;
  std::string error;
  std::string partitionTableType;
  uint64_t size;
  uint32_t blockSize = 512;
  uint32_t logicalBlockSize = 512;
  std::vector<std::string> mountpoints;
  std::vector<std::string> mountpointLabels;
  bool isReadOnly;  // Device is read-only
  bool isSystem;  // Device is a system drive
  bool isVirtual;  // Device is a virtual storage device
  bool isRemovable;  // Device is removable from the running system
  bool isCard;  // Device is an SD-card
  bool isSCSI;  // Connected via the Small Computer System Interface (SCSI)
  bool isUSB;  // Connected via Universal Serial Bus (USB)
  bool isUAS;  // Connected via the USB Attached SCSI (UAS)
  bool isUASNull;
};

std::vector<DeviceDescriptor> ListStorageDevices();
Napi::Object PackDriveDescriptor(Napi::Env env, const DeviceDescriptor *instance);
void RegisterDeviceChangeCallback(std::function<void(const std::vector<DeviceDescriptor>&)> callback);
void StartDeviceMonitoring();
void StopDeviceMonitoring();

}  // namespace Drivelist

Napi::Value List(const Napi::CallbackInfo& info);
Napi::Value StartMonitoring(const Napi::CallbackInfo& info);
Napi::Value StopMonitoring(const Napi::CallbackInfo& info);
Napi::Object Init(Napi::Env env, Napi::Object exports);

#endif  // SRC_DRIVELIST_HPP_
