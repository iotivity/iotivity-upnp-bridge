//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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
//

#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include <assert.h>
#include <pluginServer.h>
#include "logger.h"
#include "UpnpConnector.h"
#include "UpnpResource.h"
#include "UpnpBridgeDevice.h"

#define TAG "UPNP_PLUGIN"
static std::string MODULE = "UPNP_PLUGIN MODULE";

#define DLL_PUBLIC __attribute__((__visibility__("default")))

MPMPluginCtx *g_plugin_ctx = NULL;
static char CRED_FILE[] = "./oic_svr_db_upnp.dat";

FILE *sec_file(const char *, const char *mode)
{
    return fopen(CRED_FILE, mode);
}

static UpnpConnector *s_upnpConnector;
static UpnpBridgeDevice *s_bridge;
std::vector< UpnpResource::Ptr > m_vecResources;

int connectorDiscoveryCb(UpnpResource::Ptr pUpnpResource)
{
    DEBUG_PRINT("UpnpResource URI " << pUpnpResource->m_uri);
    m_vecResources.push_back(pUpnpResource);
    if (s_bridge != nullptr)
    {
        s_bridge->addResource(pUpnpResource);
    }
    else {
        ERROR_PRINT("Failed to add resource: " << pUpnpResource->m_uri);
    }

    s_upnpConnector->onAdd(pUpnpResource->m_uri); // add now, don't wait for scan

    return 0;
}

void connectorLostCb(UpnpResource::Ptr pUpnpResource)
{
    DEBUG_PRINT("UpnpResource URI " << pUpnpResource->m_uri);
    std::vector< UpnpResource::Ptr >::iterator itor;
    itor = std::find(m_vecResources.begin(), m_vecResources.end(), pUpnpResource);
    if (itor != m_vecResources.end())
    {
        m_vecResources.erase(itor);
    }

    if (s_bridge != nullptr)
    {
        s_bridge->removeResource(pUpnpResource->m_uri);
    }

    s_upnpConnector->onRemove(pUpnpResource->m_uri);
}

extern "C" DLL_PUBLIC MPMResult pluginCreate(MPMPluginCtx **plugin_specific_ctx)
{
    printf("***********************************************\n");
    printf("UPNP pluginCreate\n");
    printf("***********************************************\n");
    MPMPluginCtx *ctx = (MPMPluginCtx *) calloc(1, sizeof(MPMPluginCtx));
    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG, "Allocation of plugin context failed");
        return MPM_RESULT_INTERNAL_ERROR;
    }
    *plugin_specific_ctx = ctx;
    g_plugin_ctx = ctx;
    ctx->device_name = "UPnP plugin";
    ctx->resource_type = "oic.r.upnp.device";
    ctx->open = sec_file;
    return MPM_RESULT_OK;
}

extern "C" DLL_PUBLIC MPMResult pluginStart(MPMPluginCtx *ctx)
{
    printf("***********************************************\n");
    printf("UPNP pluginStart\n");
    printf("***********************************************\n");
    ctx->stay_in_process_loop = true;
    OIC_LOG(INFO, TAG, "Plugin start called!");

    s_bridge = new UpnpBridgeDevice();

    UpnpConnector::DiscoveryCallback discoveryCb = std::bind(&connectorDiscoveryCb, std::placeholders::_1);
    UpnpConnector::LostCallback lostCb = std::bind(&connectorLostCb, std::placeholders::_1);

    s_upnpConnector = new UpnpConnector(discoveryCb, lostCb);
    s_upnpConnector->connect();

    return MPM_RESULT_OK;
}

extern "C" DLL_PUBLIC MPMResult pluginScan(MPMPluginCtx *, MPMPipeMessage *message)
{
    printf("***********************************************\n");
    printf("UPNP pluginScan\n");
    printf("***********************************************\n");
    (void) message;
    OIC_LOG(INFO, TAG, "Scan called!");
    // Send back scan response to the client.
    s_upnpConnector->onScan();
    return MPM_RESULT_OK;
}

extern "C" DLL_PUBLIC MPMResult pluginAdd(MPMPluginCtx *, MPMPipeMessage *message)
{
    printf("***********************************************\n");
    printf("UPNP pluginAdd\n");
    printf("***********************************************\n");
    OIC_LOG(INFO, TAG, "Add called! Create Iotivity resources here based on what the client says");

    if (message->payloadSize <= 0 && message->payload == NULL)
    {
        OIC_LOG(ERROR, TAG, "No payload received, failed to add device");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    std::string uri = reinterpret_cast<const char *>(message->payload);

    s_upnpConnector->onAdd(uri);

    return MPM_RESULT_OK;
}

extern "C" DLL_PUBLIC MPMResult pluginRemove(MPMPluginCtx *, MPMPipeMessage *message)
{
    printf("***********************************************\n");
    printf("UPNP pluginRemove\n");
    printf("***********************************************\n");
    OIC_LOG(INFO, TAG, "Remove called! Remove iotivity resources here based on what the client says");

    if (message->payloadSize <= 0 && message->payload == NULL)
    {
        OIC_LOG(ERROR, TAG, "No payload received, failed to remove device");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    std::string uri = reinterpret_cast<const char *>(message->payload);

    s_upnpConnector->onRemove(uri);
    s_bridge->removeResource(uri);

    return MPM_RESULT_OK;
}

extern "C" DLL_PUBLIC MPMResult pluginReconnect(MPMPluginCtx *, MPMPipeMessage *message)
{
    printf("***********************************************\n");
    printf("UPNP pluginReconnect\n");
    printf("***********************************************\n");
    (void) message;
    OIC_LOG(INFO, TAG, "Reconnect called! Reconnect to devices, create "
                       "resources from the message/cloud/db/file.");
    // TODO implement the pluginReconnect function
    return MPM_RESULT_OK;
}

extern "C" DLL_PUBLIC MPMResult pluginStop(MPMPluginCtx *)
{
    printf("***********************************************\n");
    printf("UPNP pluginStop\n");
    printf("***********************************************\n");
    OIC_LOG(INFO, TAG, "Stop called !");

    if (s_upnpConnector != nullptr)
    {
        s_upnpConnector->disconnect();
    }
    delete s_upnpConnector;

    delete s_bridge;

    return MPM_RESULT_OK;
}


extern "C" DLL_PUBLIC MPMResult pluginDestroy(MPMPluginCtx *pluginSpecificCtx)
{
    printf("***********************************************\n");
    printf("UPNP pluginDestroy\n");
    printf("***********************************************\n");
    OIC_LOG(INFO, TAG, "Destroy called");
    if (pluginSpecificCtx != NULL && g_plugin_ctx != NULL)
    {
        if (pluginSpecificCtx->started)
        {
            pluginStop(pluginSpecificCtx);
        }
        // freeing the resource allocated in create
        free(pluginSpecificCtx);
        g_plugin_ctx = NULL;
    }

    return (MPM_RESULT_OK);
}
