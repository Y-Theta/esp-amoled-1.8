#ifndef USE_COMMON
#include "common.h"
#endif

#ifndef USE_FS
#define USE_FS

class fsmanager {
private:
    esp_vfs_littlefs_conf_t *conf;
    int read_buf_size = 128;

public:
    fsmanager();
    ~fsmanager();

    cJSON *read_json(char *path);
    void save_config(COMMON::global_config config, char *path);
};

#endif