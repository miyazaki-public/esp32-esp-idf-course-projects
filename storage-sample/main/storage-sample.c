#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "nvs_flash.h"
#include "nvs.h"

#include "esp_littlefs.h"

static const char *TAG = "storage-sample";

void app_main(void)
{
    // ========================================================================
    // NVSの動作確認
    // ========================================================================

    // Initialize NVS
    esp_err_t err = nvs_flash_init_partition("mynvs");
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase_partition("mynvs");
        err = nvs_flash_init_partition("mynvs");
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error nvs_flash_init_partition %s", esp_err_to_name(err));
        }
    }

    nvs_handle_t handle;
    err = nvs_open_from_partition("mynvs", "storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error nvs_open_from_partition %s", esp_err_to_name(err));
    }

    int32_t boot_count = 0;
    err = nvs_get_i32(handle, "boot_count", &boot_count);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Key 'boot_count' found, current value = %ld", boot_count);

        // ブートカウントを更新
        boot_count += 1;
        nvs_set_i32(handle, "boot_count", boot_count);
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Key 'boot_count' does not exist initialize count with 0");
        nvs_set_i32(handle, "boot_count", 0);
    } else {
        ESP_LOGE(TAG, "Error reading key: %s", esp_err_to_name(err));
    }
    
    char buf[32];
    size_t size = 32;
    ESP_LOGI(TAG, "\nReading string from NVS...");

    // sizeでは取得に使用するバッファサイズを設定した変数を渡す
    // nvs_get_str の呼び出しにより実際の文字列の長さが設定されている
    // 格納されている文字列の長さを取得したい場合は バッファにNULL、サイズを0として
    // nvs_get_strを呼び出すことで必要な長さ(NULL終端を含む)が取得できる
    err = nvs_get_str(handle, "my_str", buf, &size);
    char write_buf[32];
    memset(write_buf, 0, 32);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Key 'my_str' found, my_str: %s", buf);

        // boot_countの偶数か奇数かに合わせて my_str の文字列を更新
        if (boot_count % 2) {
            strcpy(write_buf, "boot_count is odd !");
        } else {
            strcpy(write_buf, "boot_count is even !");
        }
        nvs_set_str(handle, "my_str", write_buf);

    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Key 'my_str' does not exist initialize count with 'zero'");
        strcpy(write_buf, "boot_count is zero !");
        nvs_set_str(handle, "my_str", write_buf);

    } else {
        ESP_LOGE(TAG, "Error reading key: %s", esp_err_to_name(err));
    }

    // commit
    ESP_LOGI(TAG, "\nCommitting updates in NVS...");
    err = nvs_commit(handle);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "NVS commited.");
    } else {
        ESP_LOGE(TAG, "Failed to commit NVS changes!");
    }

    // Close
    nvs_close(handle);
    ESP_LOGI(TAG, "NVS handle closed.");

    // ========================================================================
    // LittleFSの動作確認
    // ========================================================================
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",
        .partition_label = NULL,
        .format_if_mount_failed = true,
        .dont_mount = false
    };

    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount LittleFS (%s)", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "LittleFS mounted");

    // logsフォルダの存在チェック
    struct stat st;
    if (stat("/littlefs/logs", &st) != 0) {
        ESP_LOGI(TAG, "logs folder does not exist. Creating...");
        ret = mkdir("/littlefs/logs", 0777);
        if (ret != 0) {
            ESP_LOGE(TAG, "Failed to create logs folder");
            return;
        }
    } else {
        ESP_LOGI(TAG, "logs folder exists");
    }

    // boot.log の読み出し
    FILE *fp = fopen("/littlefs/logs/boot.log", "r");
    if (fp != NULL) {
        ESP_LOGI(TAG, "Reading boot.log:");
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            ESP_LOGI(TAG, "%s", line);
        }
        fclose(fp);
    } else {
        ESP_LOGI(TAG, "boot.log does not exist. It will be created.");
    }

    // boot.logへの追記
    fp = fopen("/littlefs/logs/boot.log", "a");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to open boot.log for appending");
        return;
    }

    char new_line[256];
    snprintf(new_line, 256, "boot_count:%ld, my_str: %s\n", boot_count, write_buf);
    fwrite(new_line, 1, strlen(new_line), fp);
    fclose(fp);

    esp_vfs_littlefs_unregister(conf.partition_label);
    ESP_LOGI(TAG, "LittleFS unmounted");
}
