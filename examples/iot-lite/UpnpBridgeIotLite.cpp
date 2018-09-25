//******************************************************************
//
// Copyright 2018 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "UpnpConnector.h"

#define OC_SERVER

#include <oc_api.h>

#include <pthread.h>
#include <signal.h>

static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
static int quit = 0;

static UpnpConnector *connector;

static int
app_init(void)
{
    int ret = oc_init_platform("Intel", NULL, NULL);
    ret |= oc_add_device("/oic/d", "oic.d.bridge", "UPnP Bridge", "ocf.1.0.0",
                         "ocf.res.1.0.0,ocf.sh.1.0.0", NULL, NULL);
    return ret;
}

static void
signal_event_loop(void)
{
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mutex);
}

static void
register_resources(void)
{
    connector = new UpnpConnector();
    connector->connect();
}

static void
handle_signal(int signal)
{
    (void)signal;
    signal_event_loop();
    quit = 1;
}

int main()
{
    int init;
    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);

    static const oc_handler_t handler = {.init = app_init,
                                         .signal_event_loop = signal_event_loop,
                                         .register_resources = register_resources};

    oc_clock_time_t next_event;

    init = oc_main_init(&handler);
    if (init < 0)
      return init;

    while (! quit) {
      next_event = oc_main_poll();
      pthread_mutex_lock(&mutex);
      if (next_event == 0) {
        pthread_cond_wait(&cv, &mutex);
      } else {
        ts.tv_sec = (next_event / OC_CLOCK_SECOND);
        ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
        pthread_cond_timedwait(&cv, &mutex, &ts);
      }
      pthread_mutex_unlock(&mutex);
    }

    if (connector) {
        connector->disconnect();
        delete connector;
    }

    oc_main_shutdown();

    return 0;
}
