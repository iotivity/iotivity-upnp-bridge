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

OCEntityHandlerResult UpnpGenericService::processGetRequest(string uri, OCRepPayload *payload, string resourceType)
{
    if (payload == NULL)
    {
        throw "payload is null";
    }

    if (UPNP_ACTION_RESOURCE == resourceType)
    {
        GUPnPServiceInfo *serviceInfo = GUPNP_SERVICE_INFO(m_proxy);
        if (GUPNP_IS_SERVICE_INFO(serviceInfo))
        {
            GUPnPServiceIntrospection *introspection = gupnp_service_info_get_introspection(serviceInfo, NULL);
            if (introspection != NULL)
            {
                size_t actionNamePos = uri.rfind("/");
                string actionName = uri.substr(actionNamePos+1);
                const GUPnPServiceActionInfo *actionInfo = gupnp_service_introspection_get_action(introspection, actionName.c_str());

                if (actionInfo != NULL)
                {
                    if (OCRepPayloadSetPropString(payload, ACTION_NAME.c_str(), actionInfo->name))
                    {
                        DEBUG_PRINT(m_uri << ": " << ACTION_NAME << ": " << actionInfo->name);
                    }
                    else
                    {
                        ERROR_PRINT("Failed to set " << m_uri << ": " << ACTION_NAME << " value in payload");
                    }

                    const GList *arguments = (GList *)actionInfo->arguments;
                    if (arguments != NULL)
                    {
                        vector<_genArg> inputArgs;
                        vector<_genArg> outputArgs;

                        const GList *argsList;
                        for (argsList = arguments; argsList != NULL; argsList = argsList->next)
                        {
                            const GUPnPServiceActionArgInfo *argInfo = (GUPnPServiceActionArgInfo *)argsList->data;
                            const char *argName = argInfo->name;
                            const GUPnPServiceActionArgDirection argDir = argInfo->direction;
                            const char *relatedVar = argInfo->related_state_variable;

                            // get the arg type from the related state variable
                            string argType;
                            const GUPnPServiceStateVariableInfo *stateVarInfo = gupnp_service_introspection_get_state_variable(introspection, relatedVar);
                            if (stateVarInfo != NULL)
                            {
                                const GType type = stateVarInfo->type;
                                const string gType = g_type_name(type);
                                argType = GTypeToUpnpTypeMap[gType];
                                if ((argType == nullptr) || (argType.empty()))
                                {
                                    ERROR_PRINT("No type found for GType " << gType);
                                }
                            }
                            else
                            {
                                ERROR_PRINT("No related state var info for " << m_uri << ": " << STATE_VAR_NAME << ": " << relatedVar);
                            }

                            _genArg genericArg;
                            genericArg.name = argName;
                            genericArg.type = argType;
                            if (GUPNP_SERVICE_ACTION_ARG_DIRECTION_IN == argDir)
                            {
                                inputArgs.push_back(genericArg);
                            }
                            else if (GUPNP_SERVICE_ACTION_ARG_DIRECTION_OUT == argDir)
                            {
                                outputArgs.push_back(genericArg);
                            }
                            else
                            {
                                ERROR_PRINT("Unknown arg direction for " << m_uri << ": " << argDir);
                            }
                        }

                        if (!inputArgs.empty())
                        {
                            DEBUG_PRINT("Setting input args for " << uri);
                            const OCRepPayload *args[inputArgs.size()];
                            size_t dimensions[MAX_REP_ARRAY_DEPTH] = {inputArgs.size(), 0, 0};
                            for (unsigned int i = 0; i < inputArgs.size(); ++i) {
                                DEBUG_PRINT(INPUT_ARGS << "[" << i << "]");
                                DEBUG_PRINT("\t" << GEN_NAME << "=" << inputArgs[i].name);
                                DEBUG_PRINT("\t" << GEN_TYPE << "=" << inputArgs[i].type);
                                DEBUG_PRINT("\t" << GEN_VALUE << "=" << inputArgs[i].value);
                                OCRepPayload *argPayload = OCRepPayloadCreate();
                                OCRepPayloadSetPropString(argPayload, GEN_NAME.c_str(), inputArgs[i].name.c_str());
                                OCRepPayloadSetPropString(argPayload, GEN_TYPE.c_str(), inputArgs[i].type.c_str());
                                OCRepPayloadSetPropString(argPayload, GEN_VALUE.c_str(), inputArgs[i].value.c_str());
                                args[i] = argPayload;
                            }
                            OCRepPayloadSetPropObjectArray(payload, INPUT_ARGS.c_str(), args, dimensions);
                        }

                        if (!outputArgs.empty())
                        {
                            DEBUG_PRINT("Setting output args for " << uri);
                            const OCRepPayload *args[outputArgs.size()];
                            size_t dimensions[MAX_REP_ARRAY_DEPTH] = {outputArgs.size(), 0, 0};
                            for (unsigned int i = 0; i < outputArgs.size(); ++i) {
                                DEBUG_PRINT(OUTPUT_ARGS << "[" << i << "]");
                                DEBUG_PRINT("\t" << GEN_NAME << "=" << outputArgs[i].name);
                                DEBUG_PRINT("\t" << GEN_TYPE << "=" << outputArgs[i].type);
                                DEBUG_PRINT("\t" << GEN_VALUE << "=" << outputArgs[i].value);
                                OCRepPayload *argPayload = OCRepPayloadCreate();
                                OCRepPayloadSetPropString(argPayload, GEN_NAME.c_str(), outputArgs[i].name.c_str());
                                OCRepPayloadSetPropString(argPayload, GEN_TYPE.c_str(), outputArgs[i].type.c_str());
                                OCRepPayloadSetPropString(argPayload, GEN_VALUE.c_str(), outputArgs[i].value.c_str());
                                args[i] = argPayload;
                            }
                            OCRepPayloadSetPropObjectArray(payload, OUTPUT_ARGS.c_str(), args, dimensions);
                        }
                    }
                }
                else
                {
                    ERROR_PRINT("No action info for " << m_uri << ": " << ACTION_NAME << ": " << actionName);
                }
                g_object_unref(introspection);
            }
        }
        else
        {
            ERROR_PRINT("GUPNP_SERVICE_INFO failed for proxy for " << m_uri);
        }
    }

    if (UPNP_STATE_VAR_RESOURCE == resourceType)
    {
        GUPnPServiceInfo *serviceInfo = GUPNP_SERVICE_INFO(m_proxy);
        if (GUPNP_IS_SERVICE_INFO(serviceInfo))
        {
            GUPnPServiceIntrospection *introspection = gupnp_service_info_get_introspection(serviceInfo, NULL);
            if (introspection != NULL)
            {
                size_t stateVarNamePos = uri.rfind("/");
                string stateVarName = uri.substr(stateVarNamePos+1);
                const GUPnPServiceStateVariableInfo *stateVarInfo = gupnp_service_introspection_get_state_variable(introspection, stateVarName.c_str());

                if (stateVarInfo != NULL)
                {
                    if (OCRepPayloadSetPropString(payload, STATE_VAR_NAME.c_str(), stateVarInfo->name))
                    {
                        DEBUG_PRINT(m_uri << ": " << STATE_VAR_NAME << ": " << stateVarInfo->name);
                    }
                    else
                    {
                        ERROR_PRINT("Failed to set " << m_uri << ": " << STATE_VAR_NAME << " value in payload");
                    }

                    const GType type = stateVarInfo->type;
                    const string gType = g_type_name(type);
                    const string upnpType = GTypeToUpnpTypeMap[gType];
                    if ((upnpType != nullptr) && (! upnpType.empty()))
                    {
                        if (OCRepPayloadSetPropString(payload, DATA_TYPE.c_str(), upnpType.c_str()))
                        {
                            DEBUG_PRINT(m_uri << ": " << DATA_TYPE << ": " << gType << "->" << upnpType);
                        }
                        else
                        {
                            ERROR_PRINT("Failed to set " << m_uri << ": " << DATA_TYPE << " value in payload");
                        }
                    }
                    else
                    {
                        ERROR_PRINT("No type found for GType " << gType);
                    }

                    const GValue defaultValue = stateVarInfo->default_value;
                    GValue strValue;
                    memset(&strValue, 0, sizeof (GValue));
                    g_value_init(&strValue, G_TYPE_STRING);
                    g_value_transform(&defaultValue, &strValue);

                    char *defaultValueAsString = g_value_dup_string(&strValue);
                    if (defaultValueAsString != NULL)
                    {
                        if (OCRepPayloadSetPropString(payload, DEFAULT_VALUE.c_str(), defaultValueAsString))
                        {
                            DEBUG_PRINT(m_uri << ": " << DEFAULT_VALUE << ": " << defaultValueAsString);
                        }
                        else
                        {
                            ERROR_PRINT("Failed to set " << m_uri << ": " << DEFAULT_VALUE << " value in payload");
                        }
                        g_free(defaultValueAsString);
                    }
                    g_value_unset(&strValue);

                    if (! stateVarInfo->is_numeric)
                    {
                        GList *allowedValues = (GList *)stateVarInfo->allowed_values;
                        if (allowedValues != NULL)
                        {
                            unsigned allowedValuesLength = g_list_length(allowedValues);
                            char **allowedValuesArray = (char **)OICCalloc(allowedValuesLength, sizeof(char *));
                            if (allowedValuesArray)
                            {
                                int allowedValuesArrayIndex = 0;
                                const GList *avList;
                                for (avList = allowedValues; avList != NULL; avList = avList->next)
                                {
                                    allowedValuesArray[allowedValuesArrayIndex++] = (char *)avList->data;
                                }

                                if (allowedValuesArrayIndex)
                                {
                                    DEBUG_PRINT("Setting allowed values for " << uri);
                                    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {allowedValuesLength, 0, 0};
                                    for (unsigned int i = 0; i < allowedValuesLength; ++i) {
                                        DEBUG_PRINT(ALLOWED_VALUE_LIST << "[" << i << "] = " << allowedValuesArray[i]);
                                    }
                                    OCRepPayloadSetStringArray(payload, ALLOWED_VALUE_LIST.c_str(), (const char **)allowedValuesArray, dimensions);
                                }

                                OICFree(allowedValuesArray);
                            }
                        }
                    }
                }
                else
                {
                    ERROR_PRINT("No state var info for " << m_uri << ": " << STATE_VAR_NAME << ": " << stateVarName);
                }
                g_object_unref(introspection);
            }

        }
        else
        {
            ERROR_PRINT("GUPNP_SERVICE_INFO failed for proxy for " << m_uri);
        }
    }

    return UpnpService::processGetRequest(uri, payload, resourceType);
}

OCEntityHandlerResult UpnpGenericService::processPutRequest(OCEntityHandlerRequest *ehRequest,
        string uri, string resourceType, OCRepPayload *payload)
{
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
    DEBUG_PRINT("TODO: process put request for " << uri << " resource type " << resourceType);

    return OC_EH_OK;
}
