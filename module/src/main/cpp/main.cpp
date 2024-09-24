#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include "zygisk.hpp"
#include <thread>
#include <cstring>
#include <fstream>
#include <string>

#define TARGET_PACKAGE "com.kiarygames.tinyroom"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

class MyModule : public zygisk::ModuleBase {
public:

    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        const char *data_dir = env->GetStringUTFChars(args->app_data_dir, nullptr);
        preSpecialize(process, data_dir);
        env->ReleaseStringUTFChars(args->nice_name, process);
        env->ReleaseStringUTFChars(args->app_data_dir, data_dir);
    }

    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (start_module) {
            std::thread module_thread(&MyModule::runModule, this, target_data_dir);
            module_thread.detach();
        }
    }

private:
    Api *api;
    JNIEnv *env;
    bool start_module;
    std::string target_data_dir;
    void preSpecialize(const char* process, const char* data_dir) {
        if (strcmp(process, TARGET_PACKAGE) == 0) {
            start_module = true;
            target_data_dir = data_dir;
        }
    }

    void runModule(const std::string& target_data_dir) {
        std::string log_path = target_data_dir + "/dump.txt";
        std::ofstream log(log_path, std::ios_base::app);
        log << "Hello World!" << std::endl;
        log.close();
    }
};

// Register our module class
REGISTER_ZYGISK_MODULE(MyModule)