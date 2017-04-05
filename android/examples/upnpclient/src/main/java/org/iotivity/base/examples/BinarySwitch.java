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

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;

/**
 * BinarySwitch
 * <p/>
 * This class is used by UpnpClientActivity to create an object representation of a remote binary switch service
 * and update the values depending on the server response
 */
public class BinarySwitch extends Service {

    public static final String UPNP_OIC_TYPE_BINARY_SWITCH = "oic.r.switch.binary";
    public static final String UPNP_OIC_URI_PREFIX_BINARY_SWITCH = "/upnp/switch/";

    public static final String VALUE_KEY = "value";
    public static final boolean DEFAULT_VALUE = false;

    private boolean mValue;
    private boolean mIsInitialized;

    public BinarySwitch() {
        super();
        mValue = DEFAULT_VALUE;
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
        if (rep.hasAttribute(VALUE_KEY)) {
            Object obj = rep.getValue(VALUE_KEY);
            if (obj instanceof Boolean) {
                mValue = (Boolean) obj;
                mIsInitialized = true;
            }
        }
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
        rep.setValue(VALUE_KEY, mValue);
        return rep;
    }

    public boolean getValue() {
        return mValue;
    }

    public void setValue(boolean value) {
        mValue = value;
    }

    public boolean isInitialized() {
        return mIsInitialized;
    }

    @Override
    public String toString() {
        return "[" + super.toString() +
                ", " + "initialized: " + mIsInitialized +
                ", " + VALUE_KEY + ": " + mValue + "]";
    }
}
