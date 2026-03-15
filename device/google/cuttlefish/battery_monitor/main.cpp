#include <iostream>
#include <thread>
#include <chrono>
#include <android/binder_manager.h>
#include <aidl/android/hardware/health/IHealth.h>
#include <aidl/android/hardware/health/HealthInfo.h> // Required for the info struct
#include <android-base/logging.h>

using aidl::android::hardware::health::IHealth;
using aidl::android::hardware::health::HealthInfo;

int main() {
    const std::string instance = std::string(IHealth::descriptor) + "/default";
    std::shared_ptr<IHealth> healthService = IHealth::fromBinder(
        ndk::SpAIBinder(AServiceManager_waitForService(instance.c_str())));

    if (!healthService) {
        LOG(ERROR) << "Failed to get Health HAL AIDL service";
        return 1;
    }

    LOG(INFO) << "Battery Monitor Service Started on Baklava!";

    while (true) {
        HealthInfo info;
        // In Baklava/AIDL, we get the whole info struct at once
        auto ndk_status = healthService->getHealthInfo(&info);

        if (ndk_status.isOk()) {
            // Access batteryStatus inside the HealthInfo struct
            LOG(INFO) << "Current Battery Status: " << (int)info.batteryStatus;
            LOG(INFO) << "Battery Level: " << info.batteryLevel << "%";
        } else {
            LOG(ERROR) << "Failed to read health info: " << ndk_status.getMessage();
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}