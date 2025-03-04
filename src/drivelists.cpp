//src/drivelists.cpp

#include "drivelists.hpp"

#include <napi.h>

#include <vector>
#include <functional>
#include <mutex>
#include <thread>
#include <chrono>
#include <unordered_map>



// Implementation of the PackDriveDescriptor function
Napi::Object PackDriveDescriptor(Napi::Env env, const DeviceDescriptor *device) {
  Napi::Object obj = Napi::Object::New(env);

  // Add basic properties
  obj.Set("device", Napi::String::New(env, device->device));
  obj.Set("devicePath", device->devicePathNull ? env.Null() : Napi::String::New(env, device->devicePath));
  obj.Set("raw", Napi::String::New(env, device->raw));
  obj.Set("description", Napi::String::New(env, device->description));
  obj.Set("size", Napi::Number::New(env, (double)device->size));
  obj.Set("blockSize", Napi::Number::New(env, device->blockSize));
  obj.Set("logicalBlockSize", Napi::Number::New(env, device->logicalBlockSize));
  obj.Set("busType", Napi::String::New(env, device->busType));
  obj.Set("busVersion", device->busVersionNull ? env.Null() : Napi::String::New(env, device->busVersion));
  obj.Set("error", Napi::String::New(env, device->error));
  obj.Set("partitionTableType", Napi::String::New(env, device->partitionTableType));

  // Add flags
  obj.Set("system", Napi::Boolean::New(env, device->isSystem));
  obj.Set("removable", Napi::Boolean::New(env, device->isRemovable));
  obj.Set("virtual", Napi::Boolean::New(env, device->isVirtual));
  obj.Set("card", Napi::Boolean::New(env, device->isCard));
  obj.Set("usb", Napi::Boolean::New(env, device->isUSB));
  obj.Set("readonly", Napi::Boolean::New(env, device->isReadOnly));

  // Create mountpoints array with objects containing path
  Napi::Array mountpoints = Napi::Array::New(env, device->mountpoints.size());
  for (size_t i = 0; i < device->mountpoints.size(); i++) {
    Napi::Object point = Napi::Object::New(env);
    point.Set("path", Napi::String::New(env, device->mountpoints[i]));

    // Add label if available
    if (i < device->mountpointLabels.size()) {
      point.Set("label", Napi::String::New(env, device->mountpointLabels[i]));
    } else {
      point.Set("label", env.Null());
    }

    mountpoints.Set(i, point);
  }
  obj.Set("mountpoints", mountpoints);

  // Add enumerator
  obj.Set("enumerator", Napi::String::New(env, device->enumerator));

  // Add advanced properties
  obj.Set("isUas", device->isUASNull ? env.Null() : Napi::Boolean::New(env, device->isUAS));
  obj.Set("isScsi", Napi::Boolean::New(env, device->isSCSI));

  // Protected property
  obj.Set("protected", Napi::Boolean::New(env, device->isSystem));

  return obj;
}

void RegisterDeviceChangeCallback(std::function<void(const std::vector<DeviceDescriptor>&)> callback) {
  std::lock_guard<std::mutex> lock(callbacksMutex);
  callbacks.push_back(callback);
}

void MonitorDevices() {
  while (isMonitoring) {
    std::vector<DeviceDescriptor> devices = ListStorageDevices();

    // Create a map of current devices
    std::unordered_map<std::string, DeviceDescriptor> currentDeviceMap;
    for (const auto& device : devices) {
      currentDeviceMap[device.raw] = device;
    }

    // Check for added or changed devices
    bool hasChanges = false;
    for (const auto& device : devices) {
      auto it = lastDeviceMap.find(device.raw);
      if (it == lastDeviceMap.end()) {
        // New device added
        hasChanges = true;
        break;
      }
    }

    // Check for removed devices
    for (const auto& [path, device] : lastDeviceMap) {
      if (currentDeviceMap.find(path) == currentDeviceMap.end()) {
        // Device removed
        hasChanges = true;
        break;
      }
    }

    if (hasChanges) {
      // Update last device map
      lastDeviceMap = currentDeviceMap;

      // Notify callbacks
      std::lock_guard<std::mutex> lock(callbacksMutex);
      for (const auto& callback : callbacks) {
        callback(devices);
      }
    }

    // Sleep for a short period before checking again
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}

void StartDeviceMonitoring() {
  if (!isMonitoring) {
    isMonitoring = true;

    // Initialize lastDeviceMap
    std::vector<DeviceDescriptor> devices = ListStorageDevices();
    for (const auto& device : devices) {
      lastDeviceMap[device.raw] = device;
    }

    monitorThread = std::thread(MonitorDevices);
  }
}

void StopDeviceMonitoring() {
  if (isMonitoring) {
    isMonitoring = false;
    if (monitorThread.joinable()) {
      monitorThread.join();
    }
  }
}

} // namespace Drivelists

class DriveListWorker : public Napi::AsyncWorker {
 public:
  explicit DriveListWorker(Napi::Function& callback)
      : Napi::AsyncWorker(callback), devices() {}

  virtual ~DriveListWorker() {}

  void Execute() { devices = Drivelist::ListStorageDevices(); }

  void OnOK() {
    Napi::HandleScope scope(Env());
    Napi::Object drives = Napi::Array::New(Env());

    uint32_t i;
    uint32_t size = (uint32_t)devices.size();

    for (i = 0; i < size; i++) {
      drives.Set(i, Drivelist::PackDriveDescriptor(Env(), &devices[i]));
    }

    Callback().Call({Env().Undefined(), drives});
  }

 private:
  std::vector<Drivelist::DeviceDescriptor> devices;
};

Napi::Value List(const Napi::CallbackInfo& info) {
  if (!info[0].IsFunction()) {
    Napi::TypeError::New(info.Env(), "Callback must be a function")
        .ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::Function callback = info[0].As<Napi::Function>();
  DriveListWorker* dlWorker = new DriveListWorker(callback);
  dlWorker->Queue();

  return info.Env().Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "list"), Napi::Function::New(env, List));
  return exports;
}

NODE_API_MODULE(DriveList, Init)
