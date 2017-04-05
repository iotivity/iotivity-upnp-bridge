/*
 * //******************************************************************
 * //
 * // Copyright 2016 Intel Corporation All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import android.util.Log;

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;

import java.util.Map;

/**
 * ConnectionManager
 * <p/>
 * This class is used by UpnpAvClientActivity to create an object representation of a remote connectionManager service
 * and update the values depending on the server response
 */
public class ConnectionManager extends Service {

    public static final String UPNP_OIC_TYPE_CONNECTION_MANAGER = "oic.r.connection.manager";
    public static final String UPNP_OIC_URI_PREFIX_CONNECTION_MANAGER = "/upnp/connection-manager/";

    public static final String PROTOCOL_INFO_KEY = "protocolInfo";
    public static final String PROTOCOL_INFO_SOURCE_KEY = "source";
    public static final String PROTOCOL_INFO_SINK_KEY = "sink";
    public static final String CONNECTION_IDS_KEY = "connectionIds";
    public static final String DEFAULT_PROTOCOL_INFO_SOURCE = "";
    public static final String DEFAULT_PROTOCOL_INFO_SINK = "";
    public static final String DEFAULT_CONNECTION_IDS = "";

    private String mProtocolInfoSource;
    private String mProtocolInfoSink;
    private String mConnectionIds;
    private boolean mIsInitialized;

    public ConnectionManager() {
        super();
        mProtocolInfoSource = DEFAULT_PROTOCOL_INFO_SOURCE;
        mProtocolInfoSink = DEFAULT_PROTOCOL_INFO_SINK;
        mConnectionIds = DEFAULT_CONNECTION_IDS;
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
        if (rep.hasAttribute(PROTOCOL_INFO_KEY)) {
            Object obj = rep.getValue(PROTOCOL_INFO_KEY);
            if (obj instanceof OcRepresentation) {
                OcRepresentation embeddedOcRep = (OcRepresentation)obj;
                mProtocolInfoSource = embeddedOcRep.getValue(PROTOCOL_INFO_SOURCE_KEY);
                mProtocolInfoSink = embeddedOcRep.getValue(PROTOCOL_INFO_SINK_KEY);
                mIsInitialized = true;
            }
        }
        if (rep.hasAttribute(CONNECTION_IDS_KEY)) {
            Object obj = rep.getValue(CONNECTION_IDS_KEY);
            if (obj instanceof String) {
                mConnectionIds = (String) obj;
            }
        }
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
        rep.setValue(CONNECTION_IDS_KEY, mConnectionIds);
        return rep;
    }

    public String getConnectionIds() {
        return mConnectionIds;
    }

    public boolean isInitialized() {
        return mIsInitialized;
    }

    @Override
    public String toString() {
        return "[" + super.toString() +
                ", " + "initialized: " + mIsInitialized +
                ", " + PROTOCOL_INFO_SOURCE_KEY + ": " + mProtocolInfoSource +
                ", " + PROTOCOL_INFO_SINK_KEY + ": " + mProtocolInfoSink +
                ", " + CONNECTION_IDS_KEY + ": " + mConnectionIds + "]";
    }
}
