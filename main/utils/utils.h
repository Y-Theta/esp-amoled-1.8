#ifndef USE_COMMON
#include "common.h"
#endif

#ifndef USE_UTILS
#define USE_UTILS
class utils {
private:
    utils();
    ~utils();
    volatile SemaphoreHandle_t lvgl_mux = NULL;

public:
    static utils &GetInstance() {
        static utils instance;
        return instance;
    }

    bool lvgl_lock(int ms);
    void lvgl_unlock();
};
#endif
