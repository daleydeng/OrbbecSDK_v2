#pragma once
#include "openobsdk/h/ObTypes.h"
#include "IProperty.hpp"
#include "ISensor.hpp"
#include "IFilter.hpp"

namespace libobsensor {

struct DeviceInfo {
    // identifier of the device
    int         pid_ = 0;
    int         vid_ = 0;
    std::string uid_;  // Unique identifier of the port the device is connected to (pal specific)
    std::string name_;

    std::string connectionType_;  // Device connection type
    uint16_t    type_ = 0;        // 0: Monocular structured light; 1: Binocular structured light; 2: tof
    std::string fwVersion_;
    std::string hwVersion_;
    std::string supportedSdkVersion_;
    std::string asicName_;
    std::string deviceSn_;
};

typedef std::function<void(OBDeviceState state, const char *message)>                    DeviceStateChangedCallback;
typedef std::function<void(OBFwUpdateState state, const char *message, uint8_t percent)> DeviceFwUpdateCallback;

class IDevice {
public:
    typedef std::unique_lock<std::recursive_timed_mutex> ResourceLock;

    template <typename T> class ResourcePtr {
    public:
        ResourcePtr(std::shared_ptr<T> ptr, ResourceLock &&lock) : ptr_(ptr), lock_(std::move(lock)) {}

        // copy constructor and assignment operator are deleted to avoid accidental copies of the lock
        ResourcePtr(const ResourcePtr &other)            = delete;
        ResourcePtr &operator=(const ResourcePtr &other) = delete;

        // move constructor and assignment operator are deleted to avoid accidental moves of the lock
        ResourcePtr(ResourcePtr &&other)            = default;
        ResourcePtr &operator=(ResourcePtr &&other) = default;

        T *operator->() const {
            return ptr_.get();
        }

        T &operator*() const {
            return *ptr_;
        }

        operator bool() const {
            return ptr_ != nullptr;
        }

    private:
        std::shared_ptr<T> ptr_;
        ResourceLock       lock_;
    };

public:
    virtual ~IDevice() = default;

    virtual std::shared_ptr<const DeviceInfo> getInfo() const                              = 0;
    virtual const std::string                &getExtensionInfo(const std::string &infoKey) = 0;

    virtual ResourcePtr<IPropertyAccessor> getPropertyAccessor() = 0;

    virtual std::vector<OBSensorType>             getSensorTypeList() const                        = 0;
    virtual std::vector<std::shared_ptr<IFilter>> createRecommendedPostProcessingFilters(OBSensorType type) = 0;
    virtual ResourcePtr<ISensor>                  getSensor(OBSensorType type)                              = 0;

    virtual void enableHeadBeat(bool enable) = 0;

    virtual OBDeviceState getDeviceState()                                                       = 0;
    virtual int           registerDeviceStateChangeCallback(DeviceStateChangedCallback callback) = 0;
    virtual void          unregisterDeviceStateChangeCallback(int index)                         = 0;

    virtual void reboot()     = 0;
    virtual void deactivate() = 0;

    virtual void updateFirmware(const char *data, uint32_t dataSize, DeviceFwUpdateCallback updateCallback, bool async) = 0;

    // for debug and vendor specific purpose
    virtual const std::vector<uint8_t> &sendAndReceiveData(const std::vector<uint8_t> &data) = 0;
};

}  // namespace libobsensor

#ifdef __cplusplus
extern "C" {
#endif
struct ob_device_t {
    std::shared_ptr<libobsensor::IDevice> device;
};

struct ob_device_info_t {
    std::shared_ptr<const libobsensor::DeviceInfo> info;
};

#ifdef __cplusplus
}
#endif