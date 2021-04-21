// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"

#include "lwip/err.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/xlogging.h"

#include "lwip/apps/sntp.h"

#include "agenttime_esp.h"

static unsigned sntp_initialized = 0;

void initialize_sntp(void)
{
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_setservername(1, "pool.ntp.org");
	sntp_setservername(2, "pool.ntp.org");
	sntp_setservername(3, "pool.ntp.org");
	sntp_init();
}

void finalize_sntp(void)
{
	sntp_stop();
}

static void obtain_time(void)
{
    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;

    while(timeinfo.tm_year < (2021 - 1900) ) {
		ESP_LOGI("AGENT_TIME","Time is not set yet. timeinfo.tm_year:%d",timeinfo.tm_year);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
}

time_t sntp_get_current_timestamp()
{
    time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	if (timeinfo.tm_year < (2021 - 1900)) {
		obtain_time();
		time(&now);
	}
	localtime_r(&now, &timeinfo);
	return now;
}
