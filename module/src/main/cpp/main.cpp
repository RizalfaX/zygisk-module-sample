#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include "zygisk.hpp"
#include <thread>
#include <cstring>
#include "module.h"

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
        // Use JNI to fetch our process name
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        const char *data_dir = env->GetStringUTFChars(args->app_data_dir, nullptr);
        preSpecialize(process, data_dir);
        env->ReleaseStringUTFChars(args->nice_name, process);
        env->ReleaseStringUTFChars(args->app_data_dir, data_dir);
    }

    void postAppSpecialize(const AppSpecializeArgs *) override
    {
        if (start_module) 
        {
            std::thread module_thread(module, target_data_dir);
            module_thread.detach();
        }
    }
private:
    Api *api;
    JNIEnv *env;
    char* target_data_dir;
    bool start_module;

    void preSpecialize(const char* process, const char* data_dir) 
    {
        if rcmp(proce(process, TARGET_PACKAGE) == 0) 
        {
            start_module = true;
            target_data_dir = new char[strlen(data_dir) + 1];
            strcpy(target_data_dir, data_dir);
        }
    }

};

// Register our module class
REGISTER_ZYGISK_MODULE(MyModule)