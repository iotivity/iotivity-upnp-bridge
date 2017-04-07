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
 * Brightness
 * <p/>
 * This class is used by UpnpClientActivity to create an object representation of a remote brightness service
 * and update the values depending on the server response
 */
public class Brightness extends Service {

    public static final String UPNP_OIC_TYPE_BRIGHTNESS = "oic.r.light.brightness";
    public static final String UPNP_OIC_URI_PREFIX_BRIGHTNESS = "/upnp/brightness/";

    public static final String BRIGHTNESS_KEY = "brightness";
    public static final int DEFAULT_BRIGHTNESS = 100;

    private int mBrightness;
    private boolean mIsInitialized;

    public Brightness() {
        super();
        mBrightness = DEFAULT_BRIGHTNESS;
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
        if (rep.hasAttribute(BRIGHTNESS_KEY)) {
            Object obj = rep.getValue(BRIGHTNESS_KEY);
            if (obj instanceof Integer) {
                mBrightness = (Integer) obj;
                mIsInitialized = true;
            }
        }
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
        rep.setValue(BRIGHTNESS_KEY, mBrightness);
        return rep;
    }

    public int getBrightness() {
        return mBrightness;
    }

    public void setBrightness(int brightness) {
        mBrightness = brightness;
    }

    public boolean isInitialized() {
        return mIsInitialized;
    }

    @Override
    public String toString() {
        return "[" + super.toString() +
                ", " + "initialized: " + mIsInitialized +
                ", " + BRIGHTNESS_KEY + ": " + mBrightness + "]";
    }
}
