//******************************************************************
//
// Copyright 2017 Intel Corporation All Rights Reserved.
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

#include "UpnpGenericService.h"

static const string MODULE = "UpnpGenericService";

vector <UpnpAttributeInfo> UpnpGenericService::Attributes =
{
};

OCEntityHandlerResult UpnpGenericService::processGetRequest(OCRepPayload *payload, string resourceType)
{
    if (payload == NULL)
    {
        throw "payload is null";
    }
    (void) resourceType;

    //TODO use async version with callback
    DEBUG_PRINT("TODO:");
    // TODO use introspection to get actions and state variables
    // TODO create action resource if it doesn't exist
    // TODO create state valiable resource if it doesn't exist
    // TODO add action and state variable links to payload

    return UpnpService::processGetRequest(payload, resourceType);
}

OCEntityHandlerResult UpnpGenericService::processPutRequest(OCEntityHandlerRequest *ehRequest,
        string uri, string resourceType, OCRepPayload *payload)
{
    (void) uri;
    (void) resourceType;
    (void) payload;
    if (!ehRequest || !ehRequest->payload ||
            ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        throw "Incoming payload is NULL or not a representation";
    }

    OCRepPayload *input = reinterpret_cast<OCRepPayload *>(ehRequest->payload);
    if (!input)
    {
        throw "PUT payload is null";
    }

    //TODO use async version with callback
    DEBUG_PRINT("TODO:");

    return OC_EH_OK;
}
