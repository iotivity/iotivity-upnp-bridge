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
 * Configuration
 * <p/>
 * This class is used by UpnpClientActivity to create an object representation of a remote configuration service
 * and update the values depending on the server response
 */
public class Configuration extends Service {

    public static final String OIC_TYPE_CONFIG = "oic.wk.con";
    public static final String OCF_OIC_URI_PREFIX_CONFIG = "/ocf/light-config/"; // TODO: regex match "/ocf/.*-config/"

    public static final String NAME_KEY = "n";

    private String mName;
    private boolean mIsInitialized;

    public Configuration() {
        super();
        mName = "";
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
        if (rep.hasAttribute(NAME_KEY)) {
            Object obj = rep.getValue(NAME_KEY);
            if (obj instanceof String) {
                mName = (String) obj;
                mIsInitialized = true;
            }
        }
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
        rep.setValue(NAME_KEY, mName);
        return rep;
    }

    public String getName() {
        return mName;
    }

    public void setName(String name) {
        mName = name;
    }

    public boolean isInitialized() {
        return mIsInitialized;
    }

    @Override
    public String toString() {
        return "[" + super.toString() +
                ", " + "initialized: " + mIsInitialized +
                ", " + NAME_KEY + ": " + mName + "]";
    }
}
