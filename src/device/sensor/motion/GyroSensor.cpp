#include "GyroSensor.hpp"
#include "IDevice.hpp"
#include "property/InternalProperty.hpp"
#include "stream/StreamProfileFactory.hpp"

namespace libobsensor {

#pragma pack(1)
typedef struct {
    uint32_t         num;
    OBGyroSampleRate odr[16];
} GyroSampleRateList;

typedef struct {
    uint32_t             num;
    OBGyroFullScaleRange fs[16];
} GyroFullScaleRangeList;
#pragma pack()

GyroSensor::GyroSensor(const std::shared_ptr<IDevice> &owner, const std::shared_ptr<ISourcePort> &backend, const std::shared_ptr<MotionStreamer> &streamer)
    : SensorBase(owner, OB_SENSOR_GYRO, backend), streamer_(streamer) {
    auto propAccessor = owner->getPropertyAccessor();

    auto gyroSampleRateList = propAccessor->getFirmwareDataT<GyroSampleRateList>(OB_STRUCT_GET_GYRO_PRESETS_ODR_LIST);
    auto gyroFullScaleRangeList = propAccessor->getFirmwareDataT<GyroFullScaleRangeList>(OB_STRUCT_GET_GYRO_PRESETS_FULL_SCALE_LIST);

    auto lazySensor = std::make_shared<LazySensor>(owner, OB_SENSOR_GYRO);
    for(uint32_t i = 0; i < gyroSampleRateList.num; i++) {
        for(uint32_t j = 0; j < gyroFullScaleRangeList.num; j++) {
            auto gyro_odr = gyroSampleRateList.odr[i];
            auto gyro_fs  = gyroFullScaleRangeList.fs[j];
            if(gyro_odr == 0 || gyro_fs == 0) {
                continue;
            }

            auto profile = StreamProfileFactory::createGyroStreamProfile(lazySensor, gyro_fs, gyro_odr);
            streamProfileList_.emplace_back(profile);
        }
    }
}

GyroSensor::~GyroSensor() noexcept {
    if(isStreamActivated()) {
        TRY_EXECUTE(stop());
    }
    LOG_DEBUG("GyroSensor is destroyed");
};

void GyroSensor::start(std::shared_ptr<const StreamProfile> sp, FrameCallback callback) {
    activatedStreamProfile_ = sp;
    frameCallback_          = callback;
    updateStreamState(STREAM_STATE_STARTING);

    auto owner        = getOwner();
    auto propAccessor = owner->getPropertyAccessor();

    auto gyroSp = sp->as<GyroStreamProfile>();
    propAccessor->setPropertyValueT(OB_PROP_GYRO_ODR_INT, static_cast<int>( gyroSp->getSampleRate()));
    propAccessor->setPropertyValueT(OB_PROP_GYRO_FULL_SCALE_INT,   static_cast<int>(gyroSp->getFullScaleRange()) );
    propAccessor->setPropertyValueT(OB_PROP_GYRO_SWITCH_BOOL, true);

    streamer_->start(sp, [this](std::shared_ptr<const Frame> frame) {
        updateStreamState(STREAM_STATE_STREAMING);
        if(frameCallback_) {
            frameCallback_(frame);
        }
    });
}

void GyroSensor::stop() {
    updateStreamState(STREAM_STATE_STOPPING);
    streamer_->stop(activatedStreamProfile_);
    updateStreamState(STREAM_STATE_STOPED);
}

}  // namespace libobsensor