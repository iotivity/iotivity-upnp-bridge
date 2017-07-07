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
        ERROR_PRINT("payload is null for " << uri);
        return OC_EH_ERROR;
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
                        DEBUG_PRINT(uri << ": " << ACTION_NAME << ": " << actionInfo->name);
                    }
                    else
                    {
                        ERROR_PRINT("Failed to set " << uri << ": " << ACTION_NAME << " value in payload");
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
                                ERROR_PRINT("No related state var info for " << uri << ": " << STATE_VAR_NAME << ": " << relatedVar);
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
                                ERROR_PRINT("Unknown arg direction for " << uri << ": " << argDir);
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
                    ERROR_PRINT("No action info for " << uri << ": " << ACTION_NAME << ": " << actionName);
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
                        DEBUG_PRINT(uri << ": " << STATE_VAR_NAME << ": " << stateVarInfo->name);
                    }
                    else
                    {
                        ERROR_PRINT("Failed to set " << uri << ": " << STATE_VAR_NAME << " value in payload");
                    }

                    const GType type = stateVarInfo->type;
                    const string gType = g_type_name(type);
                    const string upnpType = GTypeToUpnpTypeMap[gType];
                    if ((upnpType != nullptr) && (! upnpType.empty()))
                    {
                        if (OCRepPayloadSetPropString(payload, DATA_TYPE.c_str(), upnpType.c_str()))
                        {
                            DEBUG_PRINT(uri << ": " << DATA_TYPE << ": " << gType << "->" << upnpType);
                        }
                        else
                        {
                            ERROR_PRINT("Failed to set " << uri << ": " << DATA_TYPE << " value in payload");
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
                            DEBUG_PRINT(uri << ": " << DEFAULT_VALUE << ": " << defaultValueAsString);
                        }
                        else
                        {
                            ERROR_PRINT("Failed to set " << uri << ": " << DEFAULT_VALUE << " value in payload");
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
                    ERROR_PRINT("No state var info for " << uri << ": " << STATE_VAR_NAME << ": " << stateVarName);
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
    OCEntityHandlerResult result = OC_EH_ERROR;

    if (!ehRequest || !ehRequest->payload ||
            ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        ERROR_PRINT("Incoming payload is NULL or not a representation for " << uri);
        return result;
    }

    OCRepPayload *putReqPayload = reinterpret_cast<OCRepPayload *>(ehRequest->payload);
    if (!putReqPayload)
    {
        ERROR_PRINT("PUT payload is null for " << uri);
        return result;
    }

    if (UPNP_ACTION_RESOURCE == resourceType)
    {
        char *actionName = NULL;
        if (OCRepPayloadGetPropString(putReqPayload, ACTION_NAME.c_str(), &actionName))
        {
            DEBUG_PRINT(uri << ": " << ACTION_NAME << ": " << actionName);

            // Construct request input arg lists
            GList *inNames = NULL;
            GList *inValues = NULL;

            OCRepPayload **inputArgs = NULL;
            size_t dimensionsIn[MAX_REP_ARRAY_DEPTH] = {0};
            if (OCRepPayloadGetPropObjectArray(putReqPayload, INPUT_ARGS.c_str(), &inputArgs, dimensionsIn))
            {
                for (size_t i = 0; i < dimensionsIn[0]; ++i) {
                    char *name = NULL;
                    char *type = NULL;
                    char *value = NULL;
                    OCRepPayloadGetPropString(inputArgs[i], GEN_NAME.c_str(), &name);
                    OCRepPayloadGetPropString(inputArgs[i], GEN_TYPE.c_str(), &type);
                    OCRepPayloadGetPropString(inputArgs[i], GEN_VALUE.c_str(), &value);
                    DEBUG_PRINT("--> " << INPUT_ARGS << "[" << i << "]");
                    DEBUG_PRINT("\t" << GEN_NAME << "=" << name);
                    DEBUG_PRINT("\t" << GEN_TYPE << "=" << type);
                    DEBUG_PRINT("\t" << GEN_VALUE << "=" << value);

                    GList *list = g_list_append(inNames, name);
                    if (inNames == NULL)
                    {
                        inNames = list;
                    }

                    GValue *gValue = (GValue *)malloc(sizeof (GValue));
                    memset(gValue, 0, sizeof (GValue));
                    if (UPNP_TYPE_BOOLEAN == type)
                    {
                        g_value_init(gValue, G_TYPE_BOOLEAN);
                        int boolValue = 0;
                        string lowerCaseValue = string(value);
                        transform(lowerCaseValue.begin(), lowerCaseValue.end(), lowerCaseValue.begin(), ::tolower);
                        if ("true" == lowerCaseValue || "yes" == lowerCaseValue || "on" == lowerCaseValue || "1" == lowerCaseValue)
                        {
                            boolValue = 1;
                        }
                        g_value_set_boolean(gValue, boolValue);
                    }
                    else if (UPNP_TYPE_STRING == type)
                    {
                        g_value_init(gValue, G_TYPE_STRING);
                        g_value_set_string(gValue, value);
                    }
                    else if (UPNP_TYPE_INT == type)
                    {
                        g_value_init(gValue, G_TYPE_INT);
                        g_value_set_int(gValue, stoi(value));
                    }
                    else if (UPNP_TYPE_UI4 == type)
                    {
                        g_value_init(gValue, G_TYPE_UINT);
                        g_value_set_uint(gValue, stoi(value));
                    }
                    else {
                        ERROR_PRINT("No GType known for upnp type " << type);
                    }

                    list = g_list_append(inValues, gValue);
                    if (inValues == NULL)
                    {
                        inValues = list;
                    }

                    // name is freed below from the inNames list
                    OICFree(type);
                    OICFree(value);
                }

                for (size_t i = 0; i < dimensionsIn[0]; ++i)
                {
                    OCRepPayloadDestroy(inputArgs[i]);
                }
                OICFree(inputArgs);
            }
            else {
                DEBUG_PRINT("No input args for put request for " << uri << " action " << actionName);
            }

            // Construct request output arg lists
            GList *outNames = NULL;
            GList *outTypes = NULL;

            OCRepPayload **outputArgs = NULL;
            size_t dimensionsOut[MAX_REP_ARRAY_DEPTH] = {0};
            if (OCRepPayloadGetPropObjectArray(putReqPayload, OUTPUT_ARGS.c_str(), &outputArgs, dimensionsOut))
            {
                for (size_t i = 0; i < dimensionsOut[0]; ++i) {
                    char *name = NULL;
                    char *type = NULL;
                    char *value = NULL;
                    OCRepPayloadGetPropString(outputArgs[i], GEN_NAME.c_str(), &name);
                    OCRepPayloadGetPropString(outputArgs[i], GEN_TYPE.c_str(), &type);
                    OCRepPayloadGetPropString(outputArgs[i], GEN_VALUE.c_str(), &value);
                    DEBUG_PRINT("--> " << OUTPUT_ARGS << "[" << i << "]");
                    DEBUG_PRINT("\t" << GEN_NAME << "=" << name);
                    DEBUG_PRINT("\t" << GEN_TYPE << "=" << type);
                    DEBUG_PRINT("\t" << GEN_VALUE << "=" << value);

                    GList *list = g_list_append(outNames, name);
                    if (outNames == NULL)
                    {
                        outNames = list;
                    }

                    GType gType = G_TYPE_NONE;
                    if (UPNP_TYPE_BOOLEAN == type)
                    {
                        gType = G_TYPE_BOOLEAN;
                    }
                    else if (UPNP_TYPE_STRING == type)
                    {
                        gType = G_TYPE_STRING;
                    }
                    else if (UPNP_TYPE_INT == type)
                    {
                        gType = G_TYPE_INT;
                    }
                    else if (UPNP_TYPE_UI4 == type)
                    {
                        gType = G_TYPE_UINT;
                    }
                    else {
                        ERROR_PRINT("No GType known for upnp type " << type);
                    }

                    list = g_list_append(outTypes, (gpointer)gType);
                    if (outTypes == NULL)
                    {
                        outTypes = list;
                    }

                    // name is freed below from the outNames list
                    OICFree(type);
                    OICFree(value);
                }

                for (size_t i = 0; i < dimensionsOut[0]; ++i)
                {
                    OCRepPayloadDestroy(outputArgs[i]);
                }
                OICFree(outputArgs);
            }
            else {
                DEBUG_PRINT("No output args for put request for " << uri << " action " << actionName);
            }

            GError *error = NULL;
            GList *outValues = NULL;
            if (gupnp_service_proxy_send_action_list(m_proxy, actionName, &error,
                    inNames, inValues, outNames, outTypes, &outValues))
            {
                if (OCRepPayloadSetPropString(payload, ACTION_NAME.c_str(), actionName))
                {
                    DEBUG_PRINT(uri << ": " << ACTION_NAME << ": " << actionName);

                    // Construct result input args payload
                    vector<_genArg> inputArgs;

                    GList *names = inNames;
                    GList *values;
                    for (values = inValues; values != NULL; values = values->next)
                    {
                        GValue *gValue = (GValue *)values->data;

                        GValue strValue;
                        memset(&strValue, 0, sizeof (GValue));
                        g_value_init(&strValue, G_TYPE_STRING);
                        g_value_transform(gValue, &strValue);

                        string valueAsString = g_value_get_string(&strValue);
                        g_value_unset(&strValue);

                        string gType = G_VALUE_TYPE_NAME(gValue);
                        string upnpType = GTypeToUpnpTypeMap[gType];

                        string name = (char *)names->data;

                        names = names->next;

                        _genArg genericArg;
                        genericArg.name = name;
                        genericArg.type = upnpType;
                        genericArg.value = valueAsString;
                        inputArgs.push_back(genericArg);
                    }

                    // Construct result output args payload
                    vector<_genArg> outputArgs;

                    names = outNames;
                    for (values = outValues; values != NULL; values = values->next)
                    {
                        GValue *gValue = (GValue *)values->data;

                        GValue strValue;
                        memset(&strValue, 0, sizeof (GValue));
                        g_value_init(&strValue, G_TYPE_STRING);
                        g_value_transform(gValue, &strValue);

                        string valueAsString = g_value_get_string(&strValue);
                        g_value_unset(&strValue);

                        string gType = G_VALUE_TYPE_NAME(gValue);
                        string upnpType = GTypeToUpnpTypeMap[gType];

                        string name = (char *)names->data;

                        names = names->next;

                        _genArg genericArg;
                        genericArg.name = name;
                        genericArg.type = upnpType;
                        genericArg.value = valueAsString;
                        outputArgs.push_back(genericArg);
                    }

                    if (!inputArgs.empty())
                    {
                        const OCRepPayload *args[inputArgs.size()];
                        size_t dimensions[MAX_REP_ARRAY_DEPTH] = {inputArgs.size(), 0, 0};
                        for (unsigned int i = 0; i < inputArgs.size(); ++i) {
                            DEBUG_PRINT("<-- " << INPUT_ARGS << "[" << i << "]");
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
                        const OCRepPayload *args[outputArgs.size()];
                        size_t dimensions[MAX_REP_ARRAY_DEPTH] = {outputArgs.size(), 0, 0};
                        for (unsigned int i = 0; i < outputArgs.size(); ++i) {
                            DEBUG_PRINT("<-- " << OUTPUT_ARGS << "[" << i << "]");
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

                    result = OC_EH_OK;
                }
                else
                {
                    ERROR_PRINT("Failed to set " << uri << ": " << ACTION_NAME << " value in payload");
                }
            }
            else
            {
                ERROR_PRINT("put request for " << uri << " action " << actionName << " Failed!");
                if (error)
                {
                    ERROR_PRINT("Error message: " << error->message);
                    g_error_free(error);
                }
            }

            if (inNames != NULL) {
                GList *list;
                for (list = inNames; list != NULL; list = list->next)
                {
                    OICFree(list->data);
                }
                g_list_free(inNames);
            }

            if (inValues != NULL)
            {
                GList *list;
                for (list = inValues; list != NULL; list = list->next)
                {
                    free(list->data);
                }
                g_list_free(inValues);
            }

            if (outNames != NULL) {
                GList *list;
                for (list = outNames; list != NULL; list = list->next)
                {
                    OICFree(list->data);
                }
                g_list_free(outNames);
            }

            if (outTypes != NULL)
            {
                g_list_free(outTypes);
            }

            if (outValues != NULL)
            {
                GList *list;
                for (list = outValues; list != NULL; list = list->next)
                {
                    g_value_unset((GValue *)list->data);
                    g_slice_free(GValue, list->data);
                }
                g_list_free(outValues);
            }

            OICFree(actionName);
        }
        else
        {
            ERROR_PRINT("Failed to get " << uri << ": " << ACTION_NAME << " value from payload");
        }

        return result;
    }

    ERROR_PRINT("process put request not allowed for " << uri << " resource type " << resourceType);
    return result;
}
